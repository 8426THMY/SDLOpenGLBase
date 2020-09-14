#ifndef physicsConstraintPair_h
#define physicsConstraintPair_h


#include <stdint.h>

#include "settingsPhysics.h"

#include "physicsContact.h"
#include "physicsJoint.h"


#define PHYSCOLLISIONPAIR_ACTIVE 0

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 1
#endif
#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 1
#endif

#define physPairRefresh(pair)              ((pair)->inactive = PHYSCOLLISIONPAIR_ACTIVE)
#define physContactPairIsInactive(pair)    ((pair)->inactive >= PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS)
#define physSeparationPairIsInactive(pair) ((pair)->inactive >= PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS)
#define physConstraintPairIsNew(pair)      ((pair)->inactive == 0)


typedef struct physicsCollider physicsCollider;

typedef uint_least8_t physPairTimestamp_t;

#warning "The timestamps are also unused at the moment."
typedef struct physicsContactPair physicsContactPair;
// Stores the data required to represent
// a contact between two rigid bodies.
typedef struct physicsContactPair {
	physicsManifold manifold;
	physPairTimestamp_t inactive;

	physicsCollider *cA;
	physicsCollider *cB;

	// Each collision pair is a member of two doubly linked
	// lists, one for both bodies involved in the pair.
	physicsContactPair *prevA;
	physicsContactPair *nextA;
	physicsContactPair *prevB;
	physicsContactPair *nextB;
} physicsContactPair;

typedef struct physicsSeparationPair physicsSeparationPair;
// Stores the data required to represent
// a separation between two rigid bodies.
typedef struct physicsSeparationPair {
	contactSeparation separation;
	physPairTimestamp_t inactive;

	physicsCollider *cA;
	physicsCollider *cB;

	// Each collision pair is a member of two doubly linked
	// lists, one for both bodies involved in the pair.
	physicsSeparationPair *prevA;
	physicsSeparationPair *nextA;
	physicsSeparationPair *prevB;
	physicsSeparationPair *nextB;
} physicsSeparationPair;

#warning "Maybe move this somewhere else or remove it entirely?"
/*typedef struct physicsJointPair physicsJointPair;
// Stores the data required to represent
// a joint between two rigid bodies.
typedef struct physicsJointPair {
	physicsJoint joint;

	physicsCollider *cA;
	physicsCollider *cB;

	// Each joint pair is a member of two doubly linked
	// lists, one for both bodies involved in the pair.
	physicsJointPair *prevA;
	physicsJointPair *nextA;
	physicsJointPair *prevB;
	physicsJointPair *nextB;
} physicsJointPair;*/


void physContactPairInit(
	physicsContactPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsContactPair *prev, physicsContactPair *next
);
void physSeparationPairInit(
	physicsSeparationPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsSeparationPair *prev, physicsSeparationPair *next
);
/*void physJointPairInit(
	physicsJointPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsJointPair *prev, physicsJointPair *next
);*/

void physContactPairDelete(physicsContactPair *const restrict pair);
void physSeparationPairDelete(physicsSeparationPair *const restrict pair);
//void physJointPairDelete(physicsJointPair *const restrict pair);


#endif