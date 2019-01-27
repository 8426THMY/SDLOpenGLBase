#include "physicsCollider.h"


#include "colliderAABB.h"
#include "physicsRigidBody.h"


static void physColliderUpdatePairsCallback(void *colliderA, void *colliderB);


//Initialise a physics collider from a base collider.
void physColliderInit(physicsCollider *pc, collider *c, physicsRigidBody *body){
	colliderAABB newAABB;

	pc->colliderLocal = c;

	/** These should be set properly... somehow. **/
	pc->density = 0.f;
	pc->friction = 0.f;
	pc->restitution = 0.f;

	pc->owner = body;
	/** Get the collider's bounding box (we don't need to update the collider). **/
	/** Tree can't be NULL. **/
	pc->node = aabbTreeInsertNode(NULL, &newAABB, pc);

	pc->contacts = NULL;
	pc->separations = NULL;
}


/*
** Update a physics collider. What is involved in an update
** mostly depends on the base collider (hulls, for instance,
** may update their vertices), although all colliders will
** also update their broadphase nodes in this function.
*/
void physColliderUpdate(physicsCollider *collider){
	colliderAABB newAABB;
	/** update collider and aabb **/

	//If the node is no longer completely enveloped by
	//its old bounding box, we will need to reinsert it.
	if(!colliderAABBEnvelopsAABB(&collider->node->aabb, &newAABB)){
		colliderAABBFattenVec3(&newAABB, &collider->owner->linearVelocity, &newAABB);
		/** Replace 0.2f with the defined value from aabbTree.c (AABB_EXTRA_FAT) and remove it from there. **/
		colliderAABBFattenFloat(&newAABB, 0.2f, &collider->node->aabb);

		/** Tree can't be NULL. **/
		aabbTreeUpdateNode(NULL, collider->node);
	}
}

/*
** Iterate through the broadphase, searching for
** other colliders that this one may be colliding
** with and check collision with any possible pairs.
*/
void physColliderUpdatePairs(physicsCollider *collider){
	//Check for narrowphase collision with every collider
	//whose bounding box collides with this collider's.
	aabbTreeQueryCollisionsStack(NULL, collider->node, &physColliderUpdatePairsCallback);
}


//Delete a physics collider and any of its contact or separation pairs.
void physColliderDelete(physicsCollider *collider){
	void *curPair;


	/** Tree can't be NULL. **/
	aabbTreeRemoveNode(NULL, collider->node);

	//Delete every contact that this collider is involved in.
	while((curPair = collider->contacts) != NULL){
		physContactPairDelete((physicsContactPair *)curPair);
	}
	//Delete every separation that this collider is involved in.
	while((curPair = collider->separations) != NULL){
		physSeparationPairDelete((physicsSeparationPair *)curPair);
	}
}


/*
** If the bounding boxes of "colliderA" and "colliderB" intersect, check
** for collision with the narrowphase and create a collision pair for them.
*/
static void physColliderUpdatePairsCallback(void *colliderA, void *colliderB){
	//We only check for collision if the address of the first collider is
	//less than the address of the second, as we don't want to do this twice.
	//We shouldn't check for collision if both colliders have the same owner, either.
	if(
		colliderA < colliderB && ((physicsCollider *)colliderA)->owner != ((physicsCollider *)colliderB)->owner &&
		colliderAABBCollidingAABB(&(((physicsCollider *)colliderA)->node->aabb), &(((physicsCollider *)colliderB)->node->aabb))
	){
		contactManifold baseManifold;
		physicsSeparationPair *sharedSeparation = NULL;


		//check if separation pair exists


		//
	}
}

/**
if collider < otherCollider {

	separation = NULL;
	for all separations pairs in collider {
		if pair contains these colliders {
			separation = pair;
			break;
		}
	}

	if narrowphase collision with separation {

		contact;
		if separation {
			delete separation pair(){
				pair->prevA->nextA = pair->nextA;
				pair->prevB->nextB = pair->nextB;
				free pair;
			}
			// Make sure the pair is added to each collider's list in order!
			contact = new contact pair;
		}else{
			contact = NULL;
			for all contact pairs in collider {
				if pair contains these colliders {
					contact = pair;
					break;
				}
			}
			if !contact {
				// Make sure the pair is added to each collider's list in order!
				contact = new contact pair;
			}
		}

		update contact pair;
		contact->active = 0;

	}else{

		if !separation {
			for all contact pairs in collider {
				if pair contains these colliders {
					delete contact pair(){
						pair->prevA->nextA = pair->nextA;
						pair->prevB->nextB = pair->nextB;
						free pair;
					}
					break;
				}
			}
			// Make sure the pair is added to each collider's list in order!
			separation = new separation pair;
		}

		update separation pair;
		separation->active = 0;

	}
}
**/