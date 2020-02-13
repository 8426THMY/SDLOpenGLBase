#ifndef physicsConstraintPair_h
#define physicsConstraintPair_h


#include <stdint.h>

#include "settingsPhysics.h"

#include "physicsContact.h"
#include "physicsJoint.h"


#define PHYSCOLLISIONPAIR_ACTIVE 0

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 0
#endif
#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 0
#endif

#define physPairRefresh(pair)              ((pair)->inactive = PHYSCOLLISIONPAIR_ACTIVE)
#define physSeparationPairIsInactive(pair) ((pair)->inactive > PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS)
#define physContactPairIsInactive(pair)    ((pair)->inactive > PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS)


typedef struct physicsCollider physicsCollider;

typedef uint_least8_t physPairTimestamp_t;

#warning "Use quad-lists for these. It's literally what they were made."
#warning "The timestamps are also unused at the moment."
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


void physSeparationPairInit(
	physicsSeparationPair *const restrict pair, const contactSeparation *const restrict separation,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsSeparationPair *prev, physicsSeparationPair *next
);
void physContactPairInit(
	physicsContactPair *const restrict pair, const contactManifold *const restrict manifold,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsContactPair *prev, physicsContactPair *next
);
/*void physJointPairInit(
	physicsJointPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsJointPair *prev, physicsJointPair *next
);*/

void physSeparationPairDelete(physicsSeparationPair *const restrict pair);
void physContactPairDelete(physicsContactPair *const restrict pair);
//void physJointPairDelete(physicsJointPair *const restrict pair);


#endif