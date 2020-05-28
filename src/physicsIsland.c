#include "physicsIsland.h"


#include "physicsContact.h"
#include "physicsRigidBody.h"

#include "modulePhysics.h"


// Forward-declare any helper functions!
static void collisionCallback(void *const colliderA, void *const colliderB, void *const restrict island);
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void queryCollisions(physicsIsland *const restrict island, const float invDt);
#else
static void queryCollisions(physicsIsland *const restrict island);
#endif

static void freeNodeCallback(aabbNode *const restrict node, void *const restrict island);
static void removeCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider);

static void updateColliders(physicsIsland *const restrict island, physicsRigidBody *const restrict body);
static void updateRigidBodies(physicsIsland *const restrict island, const float dt);

#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider, const float invDt);
#else
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider);
#endif
static void updateColliderSeparations(physicsIsland *const restrict island, physicsCollider *const restrict collider);


void physIslandInit(physicsIsland *const restrict island){
	aabbTreeInit(&island->tree);

	island->bodies = NULL;
	island->contacts = NULL;
	island->separations = NULL;
}


// Insert a single rigid body into an island.
void physIslandInsertRigidBody(physicsIsland *const restrict island, physicsRigidBody *const body){
	// Insert the body at the beginning of the island's list.
	if(island->bodies != NULL){
		memDoubleListPrev(island->bodies) = body;
		memDoubleListNext(body) = island->bodies;
	}
	memDoubleListPrev(body) = NULL;
	island->bodies = body;
}

// Remove a single rigid body from an island.
void physIslandRemoveRigidBody(physicsIsland *const restrict island, physicsRigidBody *const body){
	physicsCollider *collider;

	physicsRigidBody *const prevBody = (physicsRigidBody *)memDoubleListPrev(body);
	physicsRigidBody *const nextBody = (physicsRigidBody *)memDoubleListNext(body);
	// Fix up the previous list element's pointers.
	if(prevBody != NULL){
		memDoubleListNext(prevBody) = nextBody;
	}else{
		island->bodies = nextBody;
	}
	// Fix up the next list element's pointers.
	if(nextBody != NULL){
		memDoubleListPrev(nextBody) = prevBody;
	}
	// Remove the body from the island's list.
	memDoubleListPrev(body) = NULL;
	memDoubleListNext(body) = NULL;

	collider = body->colliders;
	// Remove the body's colliders from the island.
	while(collider != NULL){
		removeCollider(island, collider);
		collider = (physicsCollider *)memSingleListNext(collider);
	}
}

// Insert a doubly linked list of rigid bodies into an island.
void physIslandInsertRigidBodyList(physicsIsland *const restrict island, physicsRigidBody *const bodies){
	if(bodies != NULL){
		if(island->bodies != NULL){
			physicsRigidBody *lastBody = bodies;
			// Find the last rigid body in the list.
			for(;;){
				if(memDoubleListNext(lastBody) == NULL){
					break;
				}
				lastBody = memDoubleListNext(lastBody);
			}

			// Insert the body list at the beginning of the island's list.
			memDoubleListPrev(island->bodies) = lastBody;
			memDoubleListNext(lastBody) = island->bodies;
		}

		memDoubleListPrev(bodies) = NULL;
		island->bodies = bodies;
	}
}

// Remove a doubly linked list of rigid bodies from an island.
void physIslandRemoveRigidBodyList(physicsIsland *const restrict island, physicsRigidBody *const bodies, size_t numBodies){
	if(bodies != NULL){
		physicsRigidBody *prevBody;
		physicsRigidBody *nextBody;

		physicsRigidBody *lastBody = bodies;
		// Find the last rigid body in the list.
		for(;;){
			physicsCollider *collider = lastBody->colliders;
			// Remove the body's colliders from the island.
			while(collider != NULL){
				removeCollider(island, collider);
				collider = (physicsCollider *)memSingleListNext(collider);
			}

			--numBodies;
			if(numBodies == 0){
				break;
			}
			lastBody = memDoubleListNext(lastBody);

		}

		prevBody = (physicsRigidBody *)memDoubleListPrev(bodies);
		nextBody = (physicsRigidBody *)memDoubleListNext(lastBody);
		// Fix up the previous list element's pointers.
		if(prevBody != NULL){
			memDoubleListNext(prevBody) = nextBody;
		}else{
			island->bodies = nextBody;
		}
		// Fix up the next list element's pointers.
		if(nextBody != NULL){
			memDoubleListPrev(nextBody) = prevBody;
		}
		// Remove the bodies from the island's list.
		memDoubleListPrev(bodies) = NULL;
		memDoubleListNext(lastBody) = NULL;
	}
}


#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physIslandUpdate(physicsIsland *const restrict island, const float dt, const float invDt){
#else
void physIslandUpdate(physicsIsland *const restrict island, const float dt){
#endif
	// Update each of the island's rigid bodies and their colliders.
	updateRigidBodies(island, dt);

	// Check for any new contacts and separations between colliders.
	// We also presolve any contact constraints in this function.
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	queryCollisions(island, invDt);
	#else
	queryCollisions(island);
	#endif
}


// Free every node in a physics island's tree.
void physIslandDelete(physicsIsland *const restrict island){
	aabbTreeTraverse(&island->tree, &freeNodeCallback, island);
}


/*
** If the bounding boxes of "colliderA" and "colliderB" intersect, check
** for collision with the narrowphase and create a collision pair for them.
*/
static void collisionCallback(void *const colliderA, void *const colliderB, void *const restrict island){
	// Make sure these colliders are actually allowed
	// to collide before executing the narrowphase.
	if(physColliderPermitCollision((physicsCollider *)colliderA, (physicsCollider *)colliderB)){
		// Used when searching for an
		// existing contact or separation
		// or when creating a new one.
		void *prevPair;
		void *nextPair;
		void *sharedPair;
		// These variables store the results
		// of our narrowphase collision check.
		contactSeparation *separationPointer;
		contactSeparation separation;
		contactManifold manifold;


		// Check if a separation involving our colliders exists.
		sharedPair = physColliderFindSeparation(
			(physicsCollider *)colliderA, (physicsCollider *)colliderB,
			(physicsSeparationPair **)&prevPair, (physicsSeparationPair **)&nextPair
		);
		// If it does, we need to check if it is still valid.
		if(sharedPair != NULL){
			separationPointer = &(((physicsSeparationPair *)sharedPair)->separation);

			// If the separation still exists, refresh it and return.
			if(collidersAreSeparated(&(((physicsCollider *)colliderA)->global), &(((physicsCollider *)colliderB)->global), separationPointer)){
				physPairRefresh((physicsSeparationPair *)sharedPair);
				return;
			}

		// Otherwise, direct our pointer to our new separation.
		}else{
			separationPointer = &separation;
		}


		// Narrowphase collision check.
		if(collidersAreColliding(&(((physicsCollider *)colliderA)->global), &(((physicsCollider *)colliderB)->global), separationPointer, &manifold)){
			sharedPair = physColliderFindContact(
				(physicsCollider *)colliderA, (physicsCollider *)colliderB,
				(physicsContactPair **)&prevPair, (physicsContactPair **)&nextPair
			);
			// If a contact pair already exists, we need to refresh it
			// and update the manifold components required for persistence.
			if(sharedPair != NULL){
				physManifoldPersist(&(((physicsContactPair *)sharedPair)->manifold), &manifold, colliderA, colliderB);
				physPairRefresh((physicsContactPair *)sharedPair);

			// If this is a new contact, allocate a new pair.
			}else{
				sharedPair = modulePhysicsContactPairPrepend(&((physicsIsland *)island)->contacts);
				if(sharedPair == NULL){
					/** MALLOC FAILED **/
				}

				// Set up the new contact pair. This involves building
				// a physics manifold from our contact manifold and
				// setting up its linked lists components.
				physManifoldInit(&((physicsContactPair *)sharedPair)->manifold, &manifold, (physicsCollider *)colliderA, (physicsCollider *)colliderB);
				physContactPairInit(
					(physicsContactPair *)sharedPair,
					(physicsCollider *)colliderA, (physicsCollider *)colliderB,
					(physicsContactPair *)prevPair, (physicsContactPair *)nextPair
				);
			}

		// Colliders are not penetrating, so we have a separation.
		}else{
			// If a separation pair containing these colliders
			// already existed, we just have to refresh it.
			if(sharedPair != NULL){
				physPairRefresh((physicsSeparationPair *)sharedPair);

			// Otherwise, we'll have to create one.
			}else{
				sharedPair = modulePhysicsSeparationPairPrepend(&((physicsIsland *)island)->separations);
				if(sharedPair == NULL){
					/** MALLOC FAILED **/
				}

				((physicsSeparationPair *)sharedPair)->separation = *separationPointer;
				// Set up the new separation pair, including its lists.
				physSeparationPairInit(
					(physicsSeparationPair *)sharedPair,
					(physicsCollider *)colliderA, (physicsCollider *)colliderB,
					(physicsSeparationPair *)prevPair, (physicsSeparationPair *)nextPair
				);
			}
		}
	}
}

/*
** Check for collision between every collider in
** the island and update their collision pairs.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void queryCollisions(physicsIsland *const restrict island, const float invDt){
#else
static void queryCollisions(physicsIsland *const restrict island){
#endif
	physicsCollider *collider;
	aabbNode *node = island->tree.leaves;

	while(node != NULL){
		// Update all of the current collider's separations and contacts.
		aabbTreeQueryCollisionsStack(&island->tree, node, &collisionCallback, island);

		collider = (physicsCollider *)node->data.leaf.value;
		node = node->data.leaf.next;

		// Remove any separations and contacts that are now inactive.
		// We also need to presolve any of the collider's contacts.
		#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		updateColliderContacts(island, collider, invDt);
		#else
		updateColliderContacts(island, collider);
		#endif
		updateColliderSeparations(island, collider);
	}
}


/*
** When we free a collider's node from the tree,
** we need to destroy its contacts and separations.
*/
static void freeNodeCallback(aabbNode *const restrict node, void *const restrict island){
	if(aabbNodeIsLeaf(node)){
		physicsCollider *collider = node->data.leaf.value;
		physicsContactPair *contact = collider->contacts;
		physicsSeparationPair *separation = collider->separations;

		// Delete all of the collider's contact pairs.
		while(contact != NULL){
			physicsContactPair *const nextContact = (collider == contact->cA) ? contact->nextA : contact->nextB;
			modulePhysicsContactPairFree(&((physicsIsland *)island)->contacts, contact);
			contact = nextContact;
		}
		// Delete all of the collider's separation pairs.
		while(separation != NULL){
			physicsSeparationPair *const nextSeparation = (collider == separation->cA) ? separation->nextA : separation->nextB;
			modulePhysicsSeparationPairFree(&((physicsIsland *)island)->separations, separation);
			separation = nextSeparation;
		}
	}

	modulePhysicsAABBNodeFree(node);
}

static void removeCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider){
	if(collider->node != NULL){
		aabbTreeRemoveNode(&island->tree, collider->node, &freeNodeCallback, island);
	}
}


/*
** For every physics collider that is a part of
** this rigid body, we will need to update its
** base collider and its node in the broadphase.
*/
static void updateColliders(physicsIsland *const restrict island, physicsRigidBody *const restrict body){
	physicsCollider *curCollider = body->colliders;
	for(; curCollider != NULL; curCollider = memSingleListNext(curCollider)){
		// Update the base collider and bounding box.
		physColliderUpdate(curCollider);

		// If the collider isn't already part of an island, add it to this one!
		if(curCollider->node == NULL){
			curCollider->node = aabbTreeInsertNode(&island->tree, &curCollider->aabb, (void *)curCollider, &modulePhysicsAABBNodeAlloc);

		// Otherwise, update its node!
		}else if(!colliderAABBEnvelopsAABB(&curCollider->node->aabb, &curCollider->aabb)){
			#ifdef PHYSISLAND_AABB_EXPAND_BY_VELOCITY
			colliderAABBExpandVec3(&curCollider->aabb, &curCollider->owner->linearVelocity, &curCollider->node->aabb);
			colliderAABBExpandFloat(&curCollider->node->aabb, PHYSISLAND_AABB_PADDING, &curCollider->node->aabb);
			#else
			colliderAABBExpandFloat(&curCollider->aabb, PHYSISLAND_AABB_PADDING, &curCollider->node->aabb);
			#endif

			aabbTreeUpdateNode(&island->tree, curCollider->node);
		}
	}
}

// Updating a rigid body involves integrating its velocity.
static void updateRigidBodies(physicsIsland *const restrict island, const float dt){
	physicsRigidBody *curBody = island->bodies;
	for(; curBody != NULL; curBody = memDoubleListNext(curBody)){
		physRigidBodyUpdate(curBody, dt);
		updateColliders(island, curBody);
	}
}


/*
** Remove any contacts that have been inactive for too long and
** update the inactivity flags of contacts that are still active.
** This will also update the manifolds of active contact pairs.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider, const float invDt){
#else
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider){
#endif
	physicsContactPair *curPair = collider->contacts;

	while(curPair != NULL && curPair->cA == collider){
		physicsContactPair *nextPair = curPair->nextA;

		// If the current contact has been
		// inactive for too long, deallocate it.
		if(physContactPairIsInactive(curPair)){
			modulePhysicsContactPairFree(&island->contacts, curPair);

		// For contacts, we need to precalculate their impulses and
		// bias terms as well as increment their inactivity flag.
		}else{
			#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
			physManifoldPresolve(&curPair->manifold, curPair->cA->owner, curPair->cB->owner, invDt);
			#else
			physManifoldPresolve(&curPair->manifold, curPair->cA->owner, curPair->cB->owner);
			#endif
			++curPair->inactive;
		}

		curPair = nextPair;
	}
}

/*
** Remove any separations that have been inactive for too long and
** update the inactivity flags of separations that are still active.
*/
static void updateColliderSeparations(physicsIsland *const restrict island, physicsCollider *const restrict collider){
	physicsSeparationPair *curPair = collider->separations;

	while(curPair != NULL && curPair->cA == collider){
		physicsSeparationPair *nextPair = curPair->nextA;

		// If the current separation has been
		// inactive for too long, deallocate it.
		if(physSeparationPairIsInactive(curPair)){
			modulePhysicsSeparationPairFree(&island->separations, curPair);

		// We only need to increment the inactivity flag for
		// separations. On the next physics step, this will
		// let us know that this particular pair is not new.
		}else{
			++curPair->inactive;
		}

		curPair = nextPair;
	}
}