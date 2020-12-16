#ifndef physicsContact_h
#define physicsContact_h


#include "settingsPhysics.h"

#include "contact.h"
#ifdef PHYSCONTACT_USE_FRICTION_JOINT
#include "physicsJointFriction.h"
#endif


#ifndef PHYSCONSTRAINT_LINEAR_SLOP
	#define PHYSCONSTRAINT_LINEAR_SLOP 0.005f
#endif
#ifndef PHYSCONSTRAINT_MAX_LINEAR_CORRECTION
	#define PHYSCONSTRAINT_MAX_LINEAR_CORRECTION 0.2f
#endif

#ifndef PHYSCONTACT_RESTITUTION_THRESHOLD
	#define PHYSCONTACT_RESTITUTION_THRESHOLD 1.f
#endif
#ifndef PHYSCONTACT_SEPARATION_BIAS
	#define PHYSCONTACT_SEPARATION_BIAS 0.f
#endif
#define PHYSCONTACT_SEPARATION_BIAS_TOTAL (2.f * PHYSCONTACT_SEPARATION_BIAS)

#ifndef PHYSCONTACT_BAUMGARTE_BIAS
	#define PHYSCONTACT_BAUMGARTE_BIAS 0.1f
#endif
#ifndef PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS
	#define PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS 4
#endif
#ifndef PHYSCONTACT_PENETRATION_ERROR_THRESHOLD
	#define PHYSCONTACT_PENETRATION_ERROR_THRESHOLD (-3.f * PHYSCONSTRAINT_LINEAR_SLOP)
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


typedef struct physicsContactPoint {
	// Points on both bodies involved in the collision
	// in global space and relative to the centres of mass.
	vec3 rA;
	vec3 rB;

	// If we're using non-linear Gauss-Seidel, we'll need to know
	// the untransformed positions of the contact normal and contact
	// points relative to their bodies' centres of mass.
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

	// Stores the result of 1/((JM^(-1))J^T) (which is equivalent
	// to the inverse denominator of the impulse equation)
	// prior to collision response, as it only needs to be
	// calculated once.
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
	contactPointIndex_t numContacts;

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

	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	vec3 normalA;
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


#endif