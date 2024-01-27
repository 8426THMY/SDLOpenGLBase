#ifndef physicsContact_h
#define physicsContact_h


#include "utilMath.h"
#include "settingsPhysics.h"

#include "contact.h"
#ifdef PHYSCONTACT_USE_FRICTION_JOINT
#include "physicsJointFriction.h"
#endif


#ifndef PHYSCONTACT_LINEAR_SLOP
	#define PHYSCONTACT_LINEAR_SLOP 0.005f
#endif
#ifndef PHYSCONTACT_MAX_LINEAR_CORRECTION
	#define PHYSCONTACT_MAX_LINEAR_CORRECTION 0.2f
#endif

#ifndef PHYSCONTACT_RESTITUTION_THRESHOLD
	#define PHYSCONTACT_RESTITUTION_THRESHOLD 1.f
#endif
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	#ifndef PHYSCONTACT_LINEAR_POSITIONAL_ERROR_THRESHOLD
		#define PHYSCONTACT_LINEAR_POSITIONAL_ERROR_THRESHOLD (-3.f * PHYSCONTACT_LINEAR_SLOP)
	#endif
#endif

#ifndef PHYSCONTACT_BAUMGARTE_BIAS
	#define PHYSCONTACT_BAUMGARTE_BIAS 0.2f
#endif

#define PHYSCOLLISIONPAIR_ACTIVE 0

#ifndef PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS 1
#endif
#ifndef PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS
	#define PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS 1
#endif


// Depending on whether or not we're using friciton joints, the place
// we store normals and tangents changes. It's highly recommended that
// these macros be used so that the program works in both cases.

#ifndef PHYSCONTACT_USE_FRICTION_JOINT
#define physContactNormal(pm) ((pm)->normal)
#define physContactTangent(pm, num) ((pm)->tangents[num])
#define physContactFriction(pm) ((pm)->friction)
#else
#define physContactNormal(pm) ((pm)->frictionJoint.normal)
#define physContactTangent(pm, num) ((pm)->frictionJoint.tangents[num])
#define physContactFriction(pm) ((pm)->frictionJoint.friction)
#endif

#define physPairRefresh(pair)              ((pair)->inactive = PHYSCOLLISIONPAIR_ACTIVE)
#define physContactPairIsInactive(pair)    ((pair)->inactive >= PHYSICS_CONTACT_PAIR_MAX_INACTIVE_STEPS)
#define physSeparationPairIsInactive(pair) ((pair)->inactive >= PHYSICS_SEPARATION_PAIR_MAX_INACTIVE_STEPS)
#define physConstraintPairIsNew(pair)      ((pair)->inactive == 0)


typedef struct physicsContactPoint {
	// Points on both bodies involved in the collision
	// in global space and relative to the centres of mass.
	vec3 rA;
	vec3 rB;

	// If we're using non-linear Gauss-Seidel, we'll need to know
	// the untransformed positions of the contact points relative
	// to their rigid bodies' centres of mass.
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	vec3 rAlocal;
	vec3 rBlocal;
	#endif

	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	// Separation between the contact points.
	// Note that this is a negative quantity.
	float separation;
	#endif
	// Used to uniquely identify the contact point.
	contactKey key;

	// Stores the bias term, that is, the
	// restitution plus the Baumgarte term.
	float bias;

	// Accumulated impulses used for warm starting.
	// This helps prevent jittering in persistent contacts.
	float normalImpulse;
	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	float tangentImpulse[2];
	#endif

	// Stores the result of (JM^{-1}J^T)^{-1} (which is equivalent
	// to the inverse denominator of the impulse equation) prior
	// to collision response, as it only needs to be calculated once.
	float invNormalMass;
	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	// Same as "invNormalMass", but for tangents.
	// These values are used for friction.
	float invTangentMass[2];
	#endif
} physicsContactPoint;

/** It would probably be best to do something similar to what Box2D      **/
/** does with this structure, as ours is probably prone to cache misses. **/

// A physics manifold is similar to a regular contact manifold,
// but stores additional information required to solve contacts.
typedef struct physicsManifold {
	physicsContactPoint contacts[CONTACT_MAX_POINTS];
	contactPointIndex numContacts;

	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	// Note that this normal is for a face on body A.
	// Therefore, it should roughly point from body A to body B.
	vec3 normal;
	// As well as the contact normal, we also need to store
	// the tangent vectors that together form an orthonormal
	// basis for when we calculate the effect of friction.
	vec3 tangents[2];
	#else
	// The friction joint stores the normal and tangent vectors.
	physicsJointFriction frictionJoint;
	#endif

	// If we're using non-linear Gauss-Seidel, we'll
	// need to know the untransformed contact normal.
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	vec3 normalLocal;
	#endif

	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	//Combined friction scalar;
	float friction;
	#endif
	// Combined restitution scalar.
	float restitution;
} physicsManifold;

typedef struct physicsRigidBody physicsRigidBody;
typedef struct physicsCollider physicsCollider;

typedef uint_least8_t physPairTimestamp;

/*
** These pairs are stored in a very particular way.
**     1. The address of collider A must always be greater than the address of collider B.
**     2. Colliders store contacts they own before contacts they don't.
**     3. Contacts owned by a particular collider are sorted by the address of collider B.
**     4. Pairs are in general allocated by islands: colliders and rigid bodies just store pointers to particular elements.
** This makes lookups faster, though there is a slightly higher cost for creating pairs.
*/

#warning "The timestamps are also unused at the moment."
typedef struct physicsContactPair physicsContactPair;
// Stores the data required to represent
// a contact between two rigid bodies.
typedef struct physicsContactPair {
	physicsManifold manifold;
	physPairTimestamp inactive;

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
	physPairTimestamp inactive;

	physicsCollider *cA;
	physicsCollider *cB;

	// Each collision pair is a member of two doubly linked
	// lists, one for both bodies involved in the pair.
	physicsSeparationPair *prevA;
	physicsSeparationPair *nextA;
	physicsSeparationPair *prevB;
	physicsSeparationPair *nextB;
} physicsSeparationPair;


void physManifoldInit(
	physicsManifold *const restrict pm, const contactManifold *const restrict cm,
	const physicsCollider *const restrict cA, const physicsCollider *const restrict cB
);
void physManifoldPersist(
	physicsManifold *const restrict pm, const contactManifold *const restrict cm,
	const physicsCollider *const restrict cA, const physicsCollider *const restrict cB
);

#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physManifoldPresolve(
	physicsManifold *const restrict pm, const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB, const float frequency
);
#else
void physManifoldPresolve(
	physicsManifold *const restrict pm, const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
#endif
void physManifoldSolveVelocity(physicsManifold *const restrict pm, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
float physManifoldSolvePosition(const physicsManifold *const restrict pm, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, float separation);
#endif

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
void physContactPairDelete(physicsContactPair *const restrict pair);
void physSeparationPairDelete(physicsSeparationPair *const restrict pair);


#endif