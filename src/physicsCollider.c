#include "physicsCollider.h"


#include "modulePhysics.h"

#include "colliderAABB.h"
#include "physicsRigidBody.h"


/*
** Initialise a physics collider base
** object using the type of its collider.
*/
void physColliderInit(physicsCollider *const restrict pc, const colliderType type){
	colliderInit(&pc->global, type);
	pc->local = NULL;

	pc->density = PHYSCOLLIDER_DEFAULT_DENSITY;
	pc->friction = PHYSCOLLIDER_DEFAULT_FRICTION;
	pc->restitution = PHYSCOLLIDER_DEFAULT_RESTITUTION;

	pc->owner = NULL;
	pc->node = NULL;

	pc->contacts = NULL;
	pc->separations = NULL;
}

/*
** Create a new instance of a physics
** collider from a base physics collider.
*/
void physColliderInstantiate(physicsCollider *const restrict pc, const physicsCollider *const restrict local, physicsRigidBody *const restrict owner){
	colliderInstantiate(&pc->global, &local->global);
	pc->local = &local->global;

	pc->density = local->density;
	pc->friction = local->friction;
	pc->restitution = local->restitution;

	pc->owner = owner;
	pc->node = NULL;

	pc->contacts = NULL;
	pc->separations = NULL;
}


/*
** Update a physics collider. What is involved in an update mostly depends
** on the base collider (hulls, for instance, may update their vertices).
**
** Every collider needs to update its axis-aligned bounding box, though.
*/
void physColliderUpdate(physicsCollider *const restrict collider){
	colliderUpdate(&collider->global, &collider->owner->centroid, collider->local, &collider->owner->base->centroid, &collider->owner->state, &collider->aabb);
}


/*
** Check whether or not two colliders should be
** considered for the narrowphase collision check.
*/
return_t physColliderPermitCollision(physicsCollider *const colliderA, physicsCollider *const colliderB){
	// We only want to run the narrowphase on two colliders once,
	// so we do it when "colliderA" has the greater address.
	return(colliderA > colliderB && colliderA->owner != colliderB->owner);
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
physicsContactPair *physColliderFindContact(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsContactPair **const restrict prev, physicsContactPair **const restrict next
){

	physicsContactPair *prevPair = NULL;
	physicsContactPair *curPair = colliderA->contacts;

	// Iterate through the contacts until we find one where
	// the second collider is less than or equal to "colliderB".
	//
	// That is, search for a matching contact or
	// the first onethat "colliderA" doesn't own.
	while(curPair != NULL && colliderB >= curPair->cB){
		// If the pair we ended on involves our colliders, return it!
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
** Find and return a separation involving "colliderA" and "colliderB".
** If such a separation could not be found, a NULL pointer is returned.
**
** We don't need to loop through every separation, as our linked lists
** are sorted according to the address of the second collider involved
** in the collision. If we find a pair whose second collider's address
** is greater than "colliderB", we can exit early.
*/
physicsSeparationPair *physColliderFindSeparation(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsSeparationPair **const restrict prev, physicsSeparationPair **const restrict next
){

	physicsSeparationPair *prevPair = NULL;
	physicsSeparationPair *curPair = colliderA->separations;

	// Iterate through the separations until we find one where
	// the second collider is less than or equal to "colliderB".
	//
	// That is, search for a matching separation or
	// the first onethat "colliderA" doesn't own.
	while(curPair != NULL && colliderB >= curPair->cB){
		// If the pair we ended on involves our colliders, return it!
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
** Clear all of a physics collider's contact and separation pairs.
** Note that islands should control the constraints for colliders,
** so this really should not be necessary.
*/
void physColliderClearPairs(physicsCollider *const restrict collider){
	// Delete every contact that this collider is involved in.
	while(collider->contacts != NULL){
		modulePhysicsContactPairFree(NULL, collider->contacts);
	}
	// Delete every separation that this collider is involved in.
	while(collider->separations != NULL){
		modulePhysicsSeparationPairFree(NULL, collider->separations);
	}
}


/*
** Delete a physics collider instance and
** any of its contact or separation pairs.
*/
void physColliderDeleteInstance(physicsCollider *const restrict collider){
	colliderDeleteInstance(&collider->global);
	physColliderClearPairs(collider);
}

// Delete a physics collider base.
void physColliderDelete(physicsCollider *const restrict collider){
	colliderDelete(&collider->global);
}