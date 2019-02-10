#include "physicsCollider.h"


#include "colliderAABB.h"
#include "physicsIsland.h"
#include "physicsRigidBody.h"


/*
** Initialise a physics collider base
** object using the type of its collider.
*/
void physColliderInit(physicsCollider *pc, const colliderType_t type, physicsRigidBody *body){
	colliderInit(&pc->global, type);
	pc->local = NULL;

	/** These should be set properly... somehow. **/
	pc->density = 0.f;
	pc->friction = 0.f;
	pc->restitution = 0.f;

	pc->owner = body;
	pc->node = NULL;

	pc->contacts = NULL;
	pc->separations = NULL;
}

/*
** Create a new instance of a physics
** collider from a base physics collider.
*/
void physColliderInstantiate(physicsCollider *pc, physicsCollider *local, physicsRigidBody *body){
	colliderInstantiate(&pc->global, &local->global);
	pc->local = &local->global;

	pc->density = local->density;
	pc->friction = local->friction;
	pc->restitution = local->restitution;

	pc->owner = body;
	pc->node = NULL;

	pc->contacts = NULL;
	pc->separations = NULL;
}


/*
** Update a physics collider. What is involved in an update
** mostly depends on the base collider (hulls, for instance,
** may update their vertices), although all colliders will
** also update their broadphase nodes in this function.
*/
void physColliderUpdate(physicsCollider *collider, physicsIsland *island){
	//Update the collider and generate its new bounding box!
	colliderUpdate(&collider->global, collider->local, &collider->owner->transform, &collider->aabb);
	//Update its tree node if required.
	physIslandUpdateCollider(island, collider);
}

/*
** Iterate through the broadphase, searching for
** other colliders that this one may be colliding
** with and check collision with any possible pairs.
*/
void physColliderQueryCollisions(physicsCollider *collider){
	//Check for narrowphase collision with every collider
	//whose bounding box collides with this collider's.
	aabbTreeQueryCollisionsStack(NULL, collider->node, &physColliderCollisionCallback);
}


/*
** Find and return a separation involving "colliderA" and "colliderB".
** If such a separation could not be found, a NULL pointer is returned.
**
** We don't need to loop through every separation, as our linked lists
** are sorted according to the address of the second collider involved
** in the collision. If we find a pair whose second collider's address
** is greater than "colliderB", we can exit early.
*/
physicsSeparationPair *physColliderFindSeparation(const physicsCollider *colliderA, const physicsCollider *colliderB,
                                                  physicsSeparationPair **prev, physicsSeparationPair **next){

	physicsSeparationPair *prevPair = NULL;
	physicsSeparationPair *curPair = colliderA->separations;

	while(curPair != NULL && colliderB >= curPair->cB){
		//If this pair contains both of our colliders, return it!
		if(curPair->cB == colliderB){
			*prev = prevPair;
			*next = curPair;

			return(curPair);
		}

		prevPair = curPair;
		curPair = curPair->nextA;
	}

	*prev = prevPair;
	*next = curPair;

	return(NULL);
}

/*
** Find and return a contact involving "colliderA" and "colliderB".
** If such a contact could not be found, a NULL pointer is returned.
**
** We don't need to loop through every contact, as our linked lists
** are sorted according to the address of the second collider involved
** in the collision. If we find a pair whose second collider's address
** is greater than "colliderB", we can exit early.
*/
physicsContactPair *physColliderFindContact(const physicsCollider *colliderA, const physicsCollider *colliderB,
                                            physicsContactPair **prev, physicsContactPair **next){

	physicsContactPair *prevPair = NULL;
	physicsContactPair *curPair = colliderA->contacts;

	while(curPair != NULL && colliderB >= curPair->cB){
		//If this pair contains both of our colliders, return it!
		if(curPair->cB == colliderB){
			*prev = prevPair;
			*next = curPair;

			return(curPair);
		}

		prevPair = curPair;
		curPair = curPair->nextA;
	}

	*prev = prevPair;
	*next = curPair;

	return(NULL);
}


/*
** Remove any separations that have been inactive for too long and
** update the inactivity flags of separations that are still active.
*/
void physColliderUpdateSeparations(physicsCollider *collider){
	physicsSeparationPair *curPair = collider->separations;

	while(curPair != NULL && curPair->cA == collider){
		physicsSeparationPair *nextPair = curPair->nextA;

		//If the current separation has been
		//inactive for too long, deallocate it.
		if(physSeparationPairIsInactive(curPair)){
			/** Free the separation too. The function below should be part of the free function. **/
			physSeparationPairDelete(curPair);

		//We only need to increment the inactivity flag for
		//separations. On the next physics step, this will
		//let us know that this particular pair is not new.
		}else{
			++curPair->inactive;
		}

		curPair = nextPair;
	}
}

/*
** Remove any contacts that have been inactive for too long and
** update the inactivity flags of contacts that are still active.
** This will also update the manifolds of active contact pairs.
*/
void physColliderUpdateContacts(physicsCollider *collider, const float dt){
	physicsContactPair *curPair = collider->contacts;

	while(curPair != NULL && curPair->cA == collider){
		physicsContactPair *nextPair = curPair->nextA;

		//If the current contact has been
		//inactive for too long, deallocate it.
		if(physSeparationPairIsInactive(curPair)){
			/** Free the contact too. The function below should be part of the free function. **/
			physContactPairDelete(curPair);

		//For contacts, we need to precalculate their impulses and
		//bias terms as well as increment their inactivity flag.
		}else{
			physManifoldPresolve(&curPair->manifold, curPair->cA->owner, curPair->cB->owner, dt);
			++curPair->inactive;
		}

		curPair = nextPair;
	}
}

//Clear all of a physics collider's contact and separation pairs.
void physColliderClearPairs(physicsCollider *collider){
	void *curPair;

	//Delete every separation that this collider is involved in.
	while((curPair = collider->separations) != NULL){
		/** Free the separation too. The function below should be part of the free function. **/
		physSeparationPairDelete((physicsSeparationPair *)curPair);
	}
	//Delete every contact that this collider is involved in.
	while((curPair = collider->contacts) != NULL){
		/** Free the contact too. The function below should be part of the free function. **/
		physContactPairDelete((physicsContactPair *)curPair);
	}
}


/*
** Delete a physics collider instance and
** any of its contact or separation pairs.
**/
void physColliderDeleteInstance(physicsCollider *collider){
	colliderDeleteInstance(&collider->global);
	physColliderClearPairs(collider);
}

//Delete a physics collider base.
void physColliderDelete(physicsCollider *collider){
	colliderDelete(&collider->global);
}


/*
** If the bounding boxes of "colliderA" and "colliderB" intersect, check
** for collision with the narrowphase and create a collision pair for them.
*/
void physColliderCollisionCallback(void *colliderA, void *colliderB){
	//We only check for collision if the address of the first collider is
	//less than the address of the second, as we don't want to do this twice.
	//We shouldn't check for collision if both colliders have the same owner, either.
	if(colliderA < colliderB && ((physicsCollider *)colliderA)->owner != ((physicsCollider *)colliderB)->owner){
		//Broadphase collision check.
		if(colliderAABBCollidingAABB(&(((physicsCollider *)colliderA)->node->aabb), &(((physicsCollider *)colliderB)->node->aabb))){
			//Used when searching for an
			//existing contact or separation
			//or when creating a new one.
			void *prevPair;
			void *nextPair;
			void *sharedPair;
			//These variables store the results
			//of our narrowphase collision check.
			contactSeparation *separationPointer;
			contactSeparation separation;
			contactManifold manifold;


			//Check if a separation involving our colliders exists.
			sharedPair = physColliderFindSeparation(
				(physicsCollider *)colliderA, (physicsCollider *)colliderB,
				(physicsSeparationPair **)&prevPair, (physicsSeparationPair **)&nextPair
			);
			//If it does, we need to check if it is still valid.
			if(sharedPair != NULL){
				separationPointer = &(((physicsSeparationPair *)sharedPair)->separation);

				//If the separation still exists, refresh it and return.
				if(collidersAreSeparated(&(((physicsCollider *)colliderA)->global), &(((physicsCollider *)colliderB)->global), separationPointer)){
					physPairRefresh((physicsSeparationPair *)sharedPair);
					return;
				}

			//Otherwise, direct our pointer to our new separation.
			}else{
				separationPointer = &separation;
			}


			//Narrowphase collision check.
			if(collidersAreColliding(&(((physicsCollider *)colliderA)->global), &(((physicsCollider *)colliderB)->global), separationPointer, &manifold)){
				sharedPair = physColliderFindContact(
					(physicsCollider *)colliderA, (physicsCollider *)colliderB,
					(physicsContactPair **)&prevPair, (physicsContactPair **)&nextPair
				);
				//If a contact pair already exists, we need to refresh it
				//and update the manifold components required for persistence.
				if(sharedPair != NULL){
					physManifoldPersist(&(((physicsContactPair *)sharedPair)->manifold), &manifold, colliderA, colliderB);
					physPairRefresh((physicsContactPair *)sharedPair);

				//If this is a new contact, allocate a new pair.
				}else{
					/** allocate contact pair **/

					//Set up the new contact pair. This involves building
					//a physics manifold from our contact manifold and
					//setting up its linked lists components.
					physContactPairInit(
						(physicsContactPair *)sharedPair, &manifold,
						(physicsCollider *)colliderA, (physicsCollider *)colliderB,
						(physicsContactPair *)prevPair, (physicsContactPair *)nextPair
					);
				}

			//Colliders are not penetrating, so we have a separation.
			}else{
				//If a separation pair containing these colliders
				//already existed, we just have to refresh it.
				if(sharedPair != NULL){
					physPairRefresh((physicsSeparationPair *)sharedPair);

				//Otherwise, we'll have to create one.
				}else{
					/** allocate separation pair **/

					//Set up the new separation pair, including its lists.
					physSeparationPairInit(
						(physicsSeparationPair *)sharedPair, separationPointer,
						(physicsCollider *)colliderA, (physicsCollider *)colliderB,
						(physicsSeparationPair *)prevPair, (physicsSeparationPair *)nextPair
					);
				}
			}
		}
	}
}