#include "physicsIsland.h"


#include "physicsContact.h"
#include "physicsRigidBody.h"

#include "modulePhysics.h"


// Forward-declare any helper functions!
static void insertColliders(physicsIsland *const restrict island, physicsRigidBody *const restrict body);
static void removeColliderNode(physicsIsland *const restrict island, physicsCollider *const restrict collider);
static void removeColliders(physicsIsland *const restrict island, physicsRigidBody *const restrict body);

static void insertJoint(physicsIsland *const restrict island, physicsJointPair *const restrict joint);
static void removeJoint(physicsIsland *const restrict island, physicsJointPair *const restrict joint);

static void updateRigidBodies(physicsIsland *const restrict island, const float dt);

static void collisionCallback(void *const colliderA, void *const colliderB, void *const restrict island);
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void queryCollisions(physicsIsland *const restrict island, const float frequency);
#else
static void queryCollisions(physicsIsland *const restrict island);
#endif

static void freeNodeCallback(aabbNode *const restrict node, void *const restrict island);

#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider, const float frequency);
#else
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider);
#endif
static void updateColliderSeparations(physicsIsland *const restrict island, physicsCollider *const restrict collider);

static void solveConstraints(physicsIsland *const restrict island, const float dt);


void physIslandInit(physicsIsland *const restrict island){
	aabbTreeInit(&island->tree);

	island->bodies = NULL;
	island->contacts = NULL;
	island->separations = NULL;
	island->joints = NULL;
}


// Insert a single rigid body into an island.
void physIslandInsertRigidBody(physicsIsland *const restrict island, physicsRigidBody *const body){
	physicsJointPair *joint;

	// Insert the body at the beginning of the island's list.
	if(island->bodies != NULL){
		memDoubleListNext(body) = island->bodies;
		memDoubleListPrev(island->bodies) = body;
	}
	memDoubleListPrev(body) = NULL;
	island->bodies = body;

	joint = body->joints;
	// Insert any joints owned by this body into the island.
	while(joint != NULL && joint->bodyA == body){
		insertJoint(island, joint);
		joint = joint->nextA;
	}
}

// Insert a doubly linked list of rigid bodies into an island.
void physIslandInsertRigidBodyList(physicsIsland *const restrict island, physicsRigidBody *const bodies, size_t numBodies){
	if(bodies != NULL){
		if(island->bodies != NULL){
			physicsRigidBody *lastBody = bodies;
			// Find the last rigid body in the list.
			for(;;){
				physicsJointPair *joint = lastBody->joints;
				// Insert any joints owned by this body into the island.
				while(joint != NULL && joint->bodyA == lastBody){
					insertJoint(island, joint);
					joint = joint->nextA;
				}

				--numBodies;
				if(numBodies <= 0){
					break;
				}
				lastBody = modulePhysicsBodyNext(lastBody);
			}

			// Insert the body list at the beginning of the island's list.
			memDoubleListPrev(island->bodies) = lastBody;
			memDoubleListNext(lastBody) = island->bodies;
		}

		memDoubleListPrev(bodies) = NULL;
		island->bodies = bodies;
	}
}

// Remove a single rigid body from an island.
void physIslandRemoveRigidBody(physicsIsland *const restrict island, physicsRigidBody *const body){
	physicsJointPair *joint;
	physicsCollider *collider;

	physicsRigidBody *const nextBody = (physicsRigidBody *)modulePhysicsBodyNext(body);
	physicsRigidBody *const prevBody = (physicsRigidBody *)modulePhysicsBodyPrev(body);
	// Fix up the next list element's pointers.
	if(nextBody != NULL){
		memDoubleListPrev(nextBody) = prevBody;
	}
	// Fix up the previous list element's pointers.
	if(prevBody != NULL){
		memDoubleListNext(prevBody) = nextBody;
	}else{
		island->bodies = nextBody;
	}
	// Remove the body from the island's list.
	memDoubleListNext(body) = NULL;
	memDoubleListPrev(body) = NULL;

	joint = body->joints;
	// Remove any joints owned by this body from the island.
	while(joint != NULL && joint->bodyA == body){
		removeJoint(island, joint);
		joint = joint->nextA;
	}

	collider = body->colliders;
	// Remove the body's colliders from the island.
	while(collider != NULL){
		removeColliderNode(island, collider);
		collider = (physicsCollider *)modulePhysicsColliderNext(collider);
	}
}

// Remove a doubly linked list of rigid bodies from an island.
void physIslandRemoveRigidBodyList(physicsIsland *const restrict island, physicsRigidBody *const bodies, size_t numBodies){
	if(bodies != NULL){
		physicsRigidBody *nextBody;
		physicsRigidBody *prevBody;

		physicsRigidBody *lastBody = bodies;
		// Find the last rigid body in the list.
		for(;;){
			physicsJointPair *joint;
			physicsCollider *collider;

			joint = lastBody->joints;
			// Remove any joints owned by this body from the island.
			while(joint != NULL && joint->bodyA == lastBody){
				removeJoint(island, joint);
				joint = joint->nextA;
			}

			collider = lastBody->colliders;
			// Remove the body's colliders from the island.
			while(collider != NULL){
				removeColliderNode(island, collider);
				collider = (physicsCollider *)modulePhysicsColliderNext(collider);
			}

			--numBodies;
			if(numBodies <= 0){
				break;
			}
			lastBody = modulePhysicsBodyNext(lastBody);

		}

		nextBody = (physicsRigidBody *)modulePhysicsBodyNext(lastBody);
		prevBody = (physicsRigidBody *)modulePhysicsBodyPrev(bodies);
		// Fix up the next list element's pointers.
		if(nextBody != NULL){
			memDoubleListPrev(nextBody) = prevBody;
		}
		// Fix up the previous list element's pointers.
		if(prevBody != NULL){
			memDoubleListNext(prevBody) = nextBody;
		}else{
			island->bodies = nextBody;
		}
		// Remove the bodies from the island's list.
		memDoubleListNext(lastBody) = NULL;
		memDoubleListPrev(bodies) = NULL;
	}
}


#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physIslandUpdate(physicsIsland *const restrict island, const float dt, const float frequency){
#else
void physIslandUpdate(physicsIsland *const restrict island, const float dt){
#endif
	// Update each of the island's rigid bodies and their colliders.
	updateRigidBodies(island, dt);

	// Check for any new contacts and separations between colliders.
	// We also presolve any contact constraints in this function.
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	queryCollisions(island, frequency);
	#else
	queryCollisions(island);
	#endif

	solveConstraints(island, dt);
}


// Free every node in a physics island's tree.
void physIslandDelete(physicsIsland *const restrict island){
	aabbTreeTraverse(&island->tree, &freeNodeCallback, island);
}


/*
** For every physics collider that is a part of
** this rigid body, we will need to update its
** base collider and its node in the broadphase.
*/
static void insertColliders(physicsIsland *const restrict island, physicsRigidBody *const restrict body){
	physicsCollider *curCollider = body->colliders;
	for(; curCollider != NULL; curCollider = modulePhysicsColliderNext(curCollider)){
		// Update the base collider and bounding box.
		physColliderUpdate(curCollider);

		// If the collider isn't already part of an island, add it to this one!
		if(curCollider->node == NULL){
			colliderAABB aabb;
			// Pad the bounding box out a bit so we don't have to update the node as frequently.
			colliderAABBExpandFloat(&curCollider->aabb, PHYSISLAND_AABBTREE_NODE_PADDING, &aabb);
			#ifdef PHYSISLAND_AABBTREE_NODE_EXPAND_BY_VELOCITY
			colliderAABBExpandVec3(&aabb, &curCollider->owner->linearVelocity, &aabb);
			#endif
			curCollider->node = aabbTreeInsertNode(&island->tree, &aabb, (void *)curCollider, &modulePhysicsAABBNodeAlloc);

		// Otherwise, update its node!
		}else if(!colliderAABBEnvelopsAABB(&curCollider->node->aabb, &curCollider->aabb)){
			// Pad the bounding box out a bit so we don't have to update the node as frequently.
			colliderAABBExpandFloat(&curCollider->aabb, PHYSISLAND_AABBTREE_NODE_PADDING, &curCollider->node->aabb);
			#ifdef PHYSISLAND_AABBTREE_NODE_EXPAND_BY_VELOCITY
			colliderAABBExpandVec3(&curCollider->node->aabb, &curCollider->owner->linearVelocity, &curCollider->node->aabb);
			#endif
			aabbTreeUpdateNode(&island->tree, curCollider->node);
		}
	}
}

// Remove a single collider from the island.
static void removeColliderNode(physicsIsland *const restrict island, physicsCollider *const restrict collider){
	if(collider->node != NULL){
		aabbTreeRemoveNode(&island->tree, collider->node, &freeNodeCallback, island);
	}
}

// Remove each of the rigid body's colliders from the island.
static void removeColliders(physicsIsland *const restrict island, physicsRigidBody *const restrict body){
	physicsCollider *curCollider = body->colliders;
	for(; curCollider != NULL; curCollider = modulePhysicsColliderNext(curCollider)){
		removeColliderNode(island, curCollider);
	}
}


static void insertJoint(physicsIsland *const restrict island, physicsJointPair *const restrict joint){
	// Insert the joint at the beginning of the island's list.
	if(island->joints != NULL){
		memDoubleListNext(joint) = island->joints;
		memDoubleListPrev(island->joints) = joint;
	}
	memDoubleListPrev(joint) = NULL;
	island->joints = joint;
}

static void removeJoint(physicsIsland *const restrict island, physicsJointPair *const restrict joint){
	physicsJointPair *const nextJoint = (physicsJointPair *)modulePhysicsJointPairNext(joint);
	physicsJointPair *const prevJoint = (physicsJointPair *)modulePhysicsJointPairPrev(joint);
	// Fix up the next list element's pointers.
	if(nextJoint != NULL){
		memDoubleListPrev(nextJoint) = prevJoint;
	}
	// Fix up the previous list element's pointers.
	if(prevJoint != NULL){
		memDoubleListNext(prevJoint) = nextJoint;
	}else{
		island->joints = nextJoint;
	}
	// Remove the joint from the island's list.
	memDoubleListNext(joint) = NULL;
	memDoubleListPrev(joint) = NULL;
}


// Updating a rigid body involves integrating its velocity.
static void updateRigidBodies(physicsIsland *const restrict island, const float dt){
	physicsRigidBody *curBody = island->bodies;
	for(; curBody != NULL; curBody = modulePhysicsBodyNext(curBody)){
		physRigidBodyUpdate(curBody, dt);

		// If the rigid body allows collisions and has changed in the last update,
		// we'll need to transform the colliders and re-add them to the island.
		if(physRigidBodyIsCollidable(curBody)){
			if(flagsAreSet(curBody->flags, PHYSRIGIDBODY_TRANSFORMED)){
				insertColliders(island, curBody);
				flagsUnset(curBody->flags, PHYSRIGIDBODY_COLLISION_MODIFIED);
			}

		// Otherwise, we should remove them from the physics island.
		}else if(flagsAreSet(curBody->flags, PHYSRIGIDBODY_COLLISION_MODIFIED)){
			removeColliders(island, curBody);
			flagsUnset(curBody->flags, PHYSRIGIDBODY_COLLISION_MODIFIED);
		}
	}
}


/*
** If the bounding boxes of "colliderA" and "colliderB" intersect, check
** for collision with the narrowphase and create a collision pair for them.
*/
static void collisionCallback(void *const colliderA, void *const colliderB, void *const restrict island){
	// Make sure these colliders and rigid bodies are actually allowed to collide.
	if(
		physColliderPermitCollision((physicsCollider *)colliderA, (physicsCollider *)colliderB) &&
		physRigidBodyPermitCollision(((physicsCollider *)colliderA)->owner, ((physicsCollider *)colliderB)->owner)
	){
		// We should only run the narrowphase if the broadphase succeeds.
		if(colliderAABBCollidingAABB(&((physicsCollider *)colliderA)->node->aabb, &((physicsCollider *)colliderB)->node->aabb)){
			// Used when searching for an existing contact
			// or separation or when creating a new one.
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
}

/*
** Check for collision between every collider in
** the island and update their collision pairs.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void queryCollisions(physicsIsland *const restrict island, const float frequency){
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
		updateColliderContacts(island, collider, frequency);
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


/*
** Remove any contacts that have been inactive for too long and
** update the inactivity flags of contacts that are still active.
** This will also update the manifolds of active contact pairs.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void updateColliderContacts(physicsIsland *const restrict island, physicsCollider *const restrict collider, const float frequency){
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
			physManifoldPresolve(&curPair->manifold, curPair->cA->owner, curPair->cB->owner, frequency);
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


/*
** Solve the constraints, such as contacts and joints,
** for every system that is currently being simulated.
**
** Currently, our physics tick looks something like this:
**
** for all scenes {
**     sceneTick(){
**         for all scene.objects {
**             objTick(){
**                 for all object.bones {
**                     if bone has rigid body {
**                         update collider configuration;
**                         update collider vertices;
**                         update collider AABB node;
**                     }else{
**                         animate bone;
**                     }
**                 }
**             }
**         }
**     }
** }
**
** for all rigid bodies {
**     integrate velocity;
**     reset forces;
**     update colliders;
**     update bounding boxes;
** }
**
** queryIslands(){
**     for all islands {
**         for all island.rigidBodies {
**             check collisions;
**             update separations;
**             update contacts {
**                 if contact persists {
**                     presolve contact;
**                 }
**             }
**         }
**     }
** }
**
** solveConstraints(){
**     for all joints {
**         presolve joint;
**     }
**
**     for all velocity solver iterations {
**         for all joints {
**             solve joint velocity constraints;
**         }
**
**         for all contacts {
**             solve contact velocity constraints;
**         }
**     }
**
**     for all rigid bodies {
**         integrate position;
**     }
**
**     for configuration solver iterations {
**         for all joints {
**             solve joint configuration constraints;
**         }
**
**         for all contacts {
**             solve contact configuration constraints;
**         }
**     }
** }
*/
static void solveConstraints(physicsIsland *const restrict island, const float dt){
	size_t i;
	physicsRigidBody *body;
	physicsJointPair *jointPair;
	physicsContactPair *contactPair;


	// Presolve joints.
	jointPair = island->joints;
	while(jointPair != NULL){
		physJointPresolve(&jointPair->joint, jointPair->bodyA, jointPair->bodyB, dt);
		jointPair = (physicsJointPair *)modulePhysicsJointPairNext(jointPair);
	}


	// Iteratively solve joint and contact velocity constraints.
	for(i = PHYSICS_VELOCITY_SOLVER_NUM_ITERATIONS; i > 0; --i){
		// Solve joint velocity constraints.
		jointPair = island->joints;
		while(jointPair != NULL){
			physJointSolveVelocity(&jointPair->joint, jointPair->bodyA, jointPair->bodyB);
			jointPair = (physicsJointPair *)modulePhysicsJointPairNext(jointPair);
		}

		// Solve contact velocity constraints.
		contactPair = island->contacts;
		while(contactPair != NULL){
			physManifoldSolveVelocity(&contactPair->manifold, contactPair->cA->owner, contactPair->cB->owner);
			contactPair = (physicsContactPair *)modulePhysicsContactPairNext(contactPair);
		}
	}


	// Integrate each physics object's position.
	body = island->bodies;
	while(body != NULL){
		physRigidBodyIntegratePosition(body, dt);
		body = (physicsRigidBody *)modulePhysicsBodyNext(body);
	}


	#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
	// Iteratively solve joint and contact configuration constraints.
	for(i = PHYSICS_POSITION_SOLVER_NUM_ITERATIONS; i > 0; --i){
		return_t solved = 1;
		float separation = 0.f;

		#ifdef PHYSJOINT_USE_POSITIONAL_CORRECTION
		// Solve joint configuration constraints.
		jointPair = island->joints;
		while(jointPair != NULL){
			solved &= physJointSolvePosition(&jointPair->joint, jointPair->bodyA, jointPair->bodyB);
			jointPair = (physicsJointPair *)modulePhysicsJointPairNext(jointPair);
		}
		#endif

		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		// Solve contact configuration constraints.
		contactPair = island->contacts;
		while(contactPair != NULL){
			separation = physManifoldSolvePosition(&contactPair->manifold, contactPair->cA->owner, contactPair->cB->owner, separation);
			contactPair = (physicsContactPair *)modulePhysicsContactPairNext(contactPair);
		}
		#endif

		// Exit if the errors are small.
		if(solved && separation >= PHYSCONTACT_LINEAR_POSITIONAL_ERROR_THRESHOLD){
			return;
		}
	}
	#endif
}