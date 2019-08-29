#include "physicsConstraintPair.h"


#include <stddef.h>

#include "physicsCollider.h"


// Initialise a separation pair from a separation.
void physSeparationPairInit(physicsSeparationPair *pair, const contactSeparation *separation,
                            physicsCollider *cA, physicsCollider *cB,
                            physicsSeparationPair *prev, physicsSeparationPair *next){

	pair->separation = *separation;
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
		cA->separations = pair;
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

// Initialise a contact pair from a manifold.
void physContactPairInit(physicsContactPair *pair, const contactManifold *manifold,
                         physicsCollider *cA, physicsCollider *cB,
                         physicsContactPair *prev, physicsContactPair *next){

	physManifoldInit(&pair->manifold, manifold, cA, cB);
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
		cA->contacts = pair;
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
void physJointPairInit(physicsJointPair *pair,
                       physicsCollider *cA, physicsCollider *cB,
                       physicsJointPair *prev, physicsJointPair *next){

	pair->cA = cA;
	pair->cB = cB;

	// Make the previous pair in collider A's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cA->joints = pair;
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
	next = cB->joints;
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
		cA->joints = pair;
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


/*
** Delete a separation pair and remove it from the linked
** lists of both colliders involved in the separation.
*/
void physSeparationPairDelete(physicsSeparationPair *pair){
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
		if(prevPair->cA == pair->cA){
			prevPair->nextA = nextPair;
		}else{
			prevPair->nextB = nextPair;
		}

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->separations = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		if(nextPair->cB == pair->cB){
			nextPair->prevB = prevPair;
		}else{
			nextPair->prevA = prevPair;
		}
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
** Delete a contact pair and remove it from the linked
** lists of both colliders involved in the contact.
*/
void physContactPairDelete(physicsContactPair *pair){
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
		if(prevPair->cA == pair->cA){
			prevPair->nextA = nextPair;
		}else{
			prevPair->nextB = nextPair;
		}

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->contacts = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		if(nextPair->cB == pair->cB){
			nextPair->prevB = prevPair;
		}else{
			nextPair->prevA = prevPair;
		}
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
** Delete a joint pair and remove it from the linked
** lists of both colliders involved in the joint.
*/
void physJointPairDelete(physicsJointPair *pair){
	physicsJointPair *nextPair = pair->nextA;
	physicsJointPair *prevPair = pair->prevA;
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
		if(prevPair->cA == pair->cA){
			prevPair->nextA = nextPair;
		}else{
			prevPair->nextB = nextPair;
		}

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->joints = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		if(nextPair->cB == pair->cB){
			nextPair->prevB = prevPair;
		}else{
			nextPair->prevA = prevPair;
		}
	}
	if(prevPair != NULL){
		if(prevPair->cB == pair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		pair->cB->joints = nextPair;
	}
}