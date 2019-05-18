#ifndef physicsContact_h
#define physicsContact_h


#include "settingsPhysics.h"

#include "contact.h"


#define PHYSCONTACT_NUM_TANGENTS 2

#ifndef PHYSCONTACT_SOLVER_NUM_ITERATIONS
	#define PHYSCONTACT_SOLVER_NUM_ITERATIONS 4
#endif

#ifndef PHYSCONTACT_LINEAR_SLOP
	#define PHYSCONTACT_LINEAR_SLOP 0.05f
#endif
#ifndef PHYSCONTACT_MAX_LINEAR_CORRECTION
	#define PHYSCONTACT_MAX_LINEAR_CORRECTION 0.2f
#endif
#ifndef PHYSCONTACT_RESTITUTION_THRESHOLD
	#define PHYSCONTACT_RESTITUTION_THRESHOLD 1.f
#endif

//If full non-linear Gauss-Seidel wasn't
//specified, use Baumgarte stabilisation.
#ifndef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	#define PHYSCONTACT_STABILISER_BAUMGARTE
#endif
#ifndef PHYSCONTACT_BAUMGARTE_BIAS
	#define PHYSCONTACT_BAUMGARTE_BIAS 0.4f
#endif
#ifndef PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS
	#define PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS 4
#endif


typedef struct physicsContactPoint {
	//Used to uniquely identify the contact point.
	contactKey key;

	//Points on both bodies involved in the collision
	//in global space and relative to the centres of mass.
	vec3 rA;
	vec3 rB;

	//If we're using non-linear Gauss-Seidel, we'll need to know
	//the relative positions of the contact points in local space.
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	vec3 rAlocal;
	vec3 rBlocal;
	#endif

	//Separation between the contact points.
	//Note that this is a negative quantity.
	float separation;
	//Stores the bias term, that is, the
	//restitution plus the Baumgarte term.
	float bias;

	//Stores the result of 1/JM^-1J^T (which is equivalent
	//to the inverse denominator of the impulse equation)
	//prior to collision response, as it only needs to be
	//calculated once.
	float normalEffectiveMass;
	//Similar to "normalEffectiveMass", but uses the
	//tangent vectors. These values are used for friction.
	float tangentEffectiveMass[PHYSCONTACT_NUM_TANGENTS];

	//Accumulated impulses used for warm starting.
	//This helps prevent jittering in persistent contacts.
	float normalImpulse;
	float tangentImpulse[PHYSCONTACT_NUM_TANGENTS];
} physicsContactPoint;

/** It would probably be best to do something similar to what Box2D  **/
/** does with this structure, as ours is very prone to cache misses. **/

//A physics manifold is similar to a regular contact manifold,
//but stores additional information required to solve contacts.
typedef struct physicsManifold {
	physicsContactPoint contacts[CONTACT_MAX_POINTS];
	contactPointIndex_t numContacts;

	//Note that this normal is for a face on body A.
	//Therefore, it should roughly point from body A to body B.
	vec3 normal;
	//As well as the contact normal, we also need to store
	//the tangent vectors that together form an orthonormal
	//basis for when we calculate the effect of friction.
	vec3 tangents[PHYSCONTACT_NUM_TANGENTS];

	//Combined friction and restitution scalars.
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
void physManifoldSolvePosition(const physicsManifold *pm, const physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#endif


#endif