#include "physicsConstraintPair.h"


#include <stddef.h>

#include "physicsRigidBody.h"
#include "physicsCollider.h"


// Initialise a contact pair from a manifold.
void physContactPairInit(
	physicsContactPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsContactPair *prev, physicsContactPair *next
){

	physPairRefresh(pair);

	pair->cA = cA;
	pair->cB = cB;

	// Make the previous pair in collider A's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cA->contacts = pair;
	}
	// Make the next pair in collider A's
	// linked list point to the new one.
	if(next != NULL){
		if(cA == next->cA){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	// Set the new pair's list
	// pointers for collider A.
	pair->prevA = prev;
	pair->nextA = next;

	prev = NULL;
	next = cB->contacts;
	// Find the position in collider B's list
	// where this pair should be inserted.
	// We insert it after the last pair
	// whose first collider is collider B.
	while(next != NULL && cB == next->cA){
		prev = next;
		next = next->nextA;
	}

	// Make the previous pair in collider B's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cB->contacts = pair;
	}
	// Make the next pair in collider B's
	// linked list point to the new one.
	if(next != NULL){
		next->prevB = pair;
	}
	// Set the new pair's list
	// pointers for collider B.
	pair->prevB = prev;
	pair->nextB = next;
}

// Initialise a separation pair from a separation.
void physSeparationPairInit(
	physicsSeparationPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsSeparationPair *prev, physicsSeparationPair *next
){

	physPairRefresh(pair);

	pair->cA = cA;
	pair->cB = cB;

	// Make the previous pair in collider A's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cA->separations = pair;
	}
	// Make the next pair in collider A's
	// linked list point to the new one.
	if(next != NULL){
		if(cA == next->cA){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	// Set the new pair's list
	// pointers for collider A.
	pair->prevA = prev;
	pair->nextA = next;

	prev = NULL;
	next = cB->separations;
	// Find the position in collider B's list
	// where this pair should be inserted.
	// We insert it after the last pair
	// whose first collider is collider B.
	while(next != NULL && cB == next->cA){
		prev = next;
		next = next->nextA;
	}

	// Make the previous pair in collider B's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cB->separations = pair;
	}
	// Make the next pair in collider B's
	// linked list point to the new one.
	if(next != NULL){
		next->prevB = pair;
	}
	// Set the new pair's list
	// pointers for collider B.
	pair->prevB = prev;
	pair->nextB = next;
}

// Initialise a joint pair from a joint.
void physJointPairInit(
	physicsJointPair *const restrict pair,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB,
	physicsJointPair *prev, physicsJointPair *next
){

	pair->bodyA = bodyA;
	pair->bodyB = bodyB;

	// Make the previous pair in body A's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		bodyA->joints = pair;
	}
	// Make the next pair in body A's
	// linked list point to the new one.
	if(next != NULL){
		if(bodyA == next->bodyA){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	// Set the new pair's list
	// pointers for body A.
	pair->prevA = prev;
	pair->nextA = next;

	prev = NULL;
	next = bodyB->joints;
	// Find the position in body B's list
	// where this pair should be inserted.
	// We insert it after the last pair
	// whose first rigid body is body B.
	while(next != NULL && bodyB == next->bodyA){
		prev = next;
		next = next->nextA;
	}

	// Make the previous pair in body B's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		bodyB->joints = pair;
	}
	// Make the next pair in body B's
	// linked list point to the new one.
	if(next != NULL){
		next->prevB = pair;
	}
	// Set the new pair's list
	// pointers for body B.
	pair->prevB = prev;
	pair->nextB = next;
}


/*
** Delete a contact pair and remove it from the linked
** lists of both colliders involved in the contact.
*/
void physContactPairDelete(physicsContactPair *const restrict pair){
	physicsContactPair *nextPair = pair->nextA;
	physicsContactPair *prevPair = pair->prevA;
	// The next pair in collider A's list
	// should point to the previous one.
	if(nextPair != NULL){
		// Make sure we set the right pointer.
		if(nextPair->cA == pair->cA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	// The previous pair in collider A's
	// list should point to the next one.
	if(prevPair != NULL){
		prevPair->nextA = nextPair;

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->contacts = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		nextPair->prevB = prevPair;
	}
	if(prevPair != NULL){
		if(prevPair->cB == pair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		pair->cB->contacts = nextPair;
	}
}

/*
** Delete a separation pair and remove it from the linked
** lists of both colliders involved in the separation.
*/
void physSeparationPairDelete(physicsSeparationPair *const restrict pair){
	physicsSeparationPair *nextPair = pair->nextA;
	physicsSeparationPair *prevPair = pair->prevA;
	// The next pair in collider A's list
	// should point to the previous one.
	if(nextPair != NULL){
		// Make sure we set the right pointer.
		if(nextPair->cA == pair->cA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	// The previous pair in collider A's
	// list should point to the next one.
	if(prevPair != NULL){
		prevPair->nextA = nextPair;

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->separations = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		nextPair->prevB = prevPair;
	}
	if(prevPair != NULL){
		if(prevPair->cB == pair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		pair->cB->separations = nextPair;
	}
}

/*
** Delete a joint pair and remove it from the linked
** lists of both colliders involved in the joint.
*/
void physJointPairDelete(physicsJointPair *const restrict pair){
	physicsJointPair *nextPair = pair->nextA;
	physicsJointPair *prevPair = pair->prevA;
	// The next pair in body A's list
	// should point to the previous one.
	if(nextPair != NULL){
		// Make sure we set the right pointer.
		if(nextPair->bodyA == pair->bodyA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	// The previous pair in body A's
	// list should point to the next one.
	if(prevPair != NULL){
		if(prevPair->bodyA == pair->bodyA){
			prevPair->nextA = nextPair;
		}else{
			prevPair->nextB = nextPair;
		}

	// If it's the first pair in the list,
	// update the rigid body's list pointer.
	}else{
		pair->bodyA->joints = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for body B.
	if(nextPair != NULL){
		if(nextPair->bodyB == pair->bodyB){
			nextPair->prevB = prevPair;
		}else{
			nextPair->prevA = prevPair;
		}
	}
	if(prevPair != NULL){
		if(prevPair->bodyB == pair->bodyB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		pair->bodyB->joints = nextPair;
	}
}