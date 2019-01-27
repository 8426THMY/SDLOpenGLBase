#ifndef physicsContactPair_h
#define physicsContactPair_h


#define COLLISIONPAIR_ACTIVE   0
#define COLLISIONPAIR_INACTIVE 1


#include "utilTypes.h"

#include "physicsContact.h"


typedef struct physicsCollider physicsCollider;

typedef struct physicsSeparationPair physicsSeparationPair;
//Stores the data required to represent
//a separation between two rigid bodies.
typedef struct physicsSeparationPair {
	contactSeparation separation;
	byte_t active;

	physicsCollider *cA;
	physicsCollider *cB;

	//Each collision pair is a member of two doubly linked
	//lists, one for both bodies involved in the pair.
	physicsSeparationPair *prevA;
	physicsSeparationPair *nextA;
	physicsSeparationPair *prevB;
	physicsSeparationPair *nextB;
} physicsSeparationPair;

typedef struct physicsContactPair physicsContactPair;
//Stores the data required to represent
//a contact between two rigid bodies.
typedef struct physicsContactPair {
	physicsManifold manifold;
	byte_t active;

	physicsCollider *cA;
	physicsCollider *cB;

	//Each collision pair is a member of two doubly linked
	//lists, one for both bodies involved in the pair.
	physicsContactPair *prevA;
	physicsContactPair *nextA;
	physicsContactPair *prevB;
	physicsContactPair *nextB;
} physicsContactPair;


void physSeparationPairDelete(physicsSeparationPair *sPair);
void physContactPairDelete(physicsContactPair *cPair);


#endif