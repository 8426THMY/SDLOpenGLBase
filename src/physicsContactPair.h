#ifndef physicsContactPair_h
#define physicsContactPair_h


#include "settingsPhysics.h"

#include "utilTypes.h"
#include "physicsContact.h"


#define PHYSCOLLISIONPAIR_ACTIVE 0

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 0
#endif
#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 0
#endif

#define physPairRefresh(pair)           ((pair)->inactive = PHYSCOLLISIONPAIR_ACTIVE)
#define physSeparationPairIsInactive(pair) ((pair)->inactive > PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS)
#define physContactPairIsInactive(pair)    ((pair)->inactive > PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS)


typedef struct physicsCollider physicsCollider;

typedef uint_least8_t physPairTimestamp_t;

typedef struct physicsSeparationPair physicsSeparationPair;
//Stores the data required to represent
//a separation between two rigid bodies.
typedef struct physicsSeparationPair {
	contactSeparation separation;
	physPairTimestamp_t inactive;

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
	physPairTimestamp_t inactive;

	physicsCollider *cA;
	physicsCollider *cB;

	//Each collision pair is a member of two doubly linked
	//lists, one for both bodies involved in the pair.
	physicsContactPair *prevA;
	physicsContactPair *nextA;
	physicsContactPair *prevB;
	physicsContactPair *nextB;
} physicsContactPair;


void physSeparationPairInit(physicsSeparationPair *pair, const contactSeparation *separation,
                            physicsCollider *cA, physicsCollider *cB,
                            physicsSeparationPair *prev, physicsSeparationPair *next);
void physContactPairInit(physicsContactPair *pair, const contactManifold *manifold,
                         physicsCollider *cA, physicsCollider *cB,
                         physicsContactPair *prev, physicsContactPair *next);

void physSeparationPairDelete(physicsSeparationPair *pair);
void physContactPairDelete(physicsContactPair *pair);


#endif