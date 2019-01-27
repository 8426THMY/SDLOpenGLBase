#include "physicsContactPair.h"


#include <stddef.h>

#include "physicsCollider.h"


/*
** Delete a separation pair and remove it from the linked
** lists of both colliders involved in the separation.
*/
void physSeparationPairDelete(physicsSeparationPair *sPair){
	physicsSeparationPair *nextPair = sPair->nextA;
	physicsSeparationPair *prevPair = sPair->prevA;
	//The next pair in collider A's list
	//should point to the previous one.
	if(nextPair != NULL){
		//Make sure we set the right pointer.
		if(nextPair->cA == sPair->cA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	//The previous pair in collider A's
	//list should point to the next one.
	if(prevPair != NULL){
		if(nextPair->cA == sPair->cA){
			prevPair->nextA = nextPair;
		}else{
			prevPair->nextB = nextPair;
		}

	//If it's the first pair in the list,
	//update the collider's list pointer.
	}else{
		sPair->cA->separations = nextPair;
	}

	nextPair = sPair->nextB;
	prevPair = sPair->prevB;
	//Do the same again but for collider B.
	if(nextPair != NULL){
		if(nextPair->cB == sPair->cB){
			nextPair->prevB = prevPair;
		}else{
			nextPair->prevA = prevPair;
		}
	}
	if(prevPair != NULL){
		if(nextPair->cB == sPair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		sPair->cB->separations = nextPair;
	}
}

/*
** Delete a contact pair and remove it from the linked
** lists of both colliders involved in the contact.
*/
void physContactPairDelete(physicsContactPair *cPair){
	physicsContactPair *nextPair = cPair->nextA;
	physicsContactPair *prevPair = cPair->prevA;
	//The next pair in collider A's list
	//should point to the previous one.
	if(nextPair != NULL){
		//Make sure we set the right pointer.
		if(nextPair->cA == cPair->cA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	//The previous pair in collider A's
	//list should point to the next one.
	if(prevPair != NULL){
		if(nextPair->cA == cPair->cA){
			prevPair->nextA = nextPair;
		}else{
			prevPair->nextB = nextPair;
		}

	//If it's the first pair in the list,
	//update the collider's list pointer.
	}else{
		cPair->cA->contacts = nextPair;
	}

	nextPair = cPair->nextB;
	prevPair = cPair->prevB;
	//Do the same again but for collider B.
	if(nextPair != NULL){
		if(nextPair->cB == cPair->cB){
			nextPair->prevB = prevPair;
		}else{
			nextPair->prevA = prevPair;
		}
	}
	if(prevPair != NULL){
		if(nextPair->cB == cPair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		cPair->cB->contacts = nextPair;
	}
}