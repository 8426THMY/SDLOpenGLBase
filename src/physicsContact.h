#ifndef physicsContact_h
#define physicsContact_h


#include "settingsPhysics.h"

#include "contact.h"
#ifdef PHYSCONTACT_USE_FRICTION_JOINT
#include "physicsJointFriction.h"
#endif


#ifndef PHYSCONTACT_SOLVER_NUM_ITERATIONS
	#define PHYSCONTACT_SOLVER_NUM_ITERATIONS 4
#endif

#ifndef PHYSCONSTRAINT_LINEAR_SLOP
	#define PHYSCONSTRAINT_LINEAR_SLOP 0.05f
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
#define PHYSCONTACT_SEPARATION_BIAS_TOTAL (PHYSCONTACT_SEPARATION_BIAS + PHYSCONTACT_SEPARATION_BIAS)

#ifndef PHYSCONTACT_BAUMGARTE_BIAS
	#define PHYSCONTACT_BAUMGARTE_BIAS 0.4f
#endif
#ifndef PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS
	#define PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS 4
#endif


// Depending on whether or not we're using friciton joints, the place
// we store normals and tangents changes. It's highly recommended that
// these macros be used so that the program works in both cases.

// Get the contact's normal vector.
#ifndef PHYSCONTACT_USE_FRICTION_JOINT
#define physContactNormal(pm) (pm->normal)
#else
#define physContactNormal(pm) (pm->frictionJoint.normal)
#endif
// Get a specific contact tangent vector.
#ifndef PHYSCONTACT_USE_FRICTION_JOINT
#define physContactTangent(pm, num) (pm->tangents[num])
#else
#define physContactTangent(pm, num) (pm->frictionJoint.tangents[num])
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

	vec3 normalLocal;
	#endif

	// Separation between the contact points.
	// Note that this is a negative quantity.
	float separation;
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

	// Stores the result of 1/((JM^-1)J^T) (which is equivalent
	// to the inverse denominator of the impulse equation)
	// prior to collision response, as it only needs to be
	// calculated once.
	float normalEffectiveMass;
	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	// Similar to "normalEffectiveMass", but uses the
	// tangent vectors. These values are used for friction.
	float tangentEffectiveMass[2];
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

	// Combined friction and restitution scalars.
	float friction;
	float restitution;
} physicsManifold;


typedef struct physicsRigidBody physicsRigidBody;
typedef struct physicsCollider physicsCollider;

void physManifoldInit(physicsManifold *pm, const contactManifold *cm, const physicsCollider *cA, const physicsCollider *cB);
void physManifoldPersist(physicsManifold *pm, const contactManifold *cm, const physicsCollider *cA, const physicsCollider *cB);

void physManifoldPresolve(physicsManifold *pm, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);
void physManifoldSolveVelocity(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
float physManifoldSolvePosition(const physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#endif


#endif