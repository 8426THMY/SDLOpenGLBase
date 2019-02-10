#ifndef physicsContact_h
#define physicsContact_h


#include "settingsPhysics.h"

#include "contact.h"


#define PHYSCONTACT_NUM_TANGENTS 2

#ifndef PHYSICS_SOLVER_NUM_ITERATIONS
	#define PHYSICS_SOLVER_NUM_ITERATIONS 4
#endif

#ifndef PHYSCONTACT_PENETRATION_SLOP
	#define PHYSCONTACT_PENETRATION_SLOP 0.05f
#endif
#ifndef PHYSCONTACT_BAUMGARTE_BIAS
	#define PHYSCONTACT_BAUMGARTE_BIAS 0.4f
#endif
#ifndef PHYSCONTACT_RESTITUTION_THRESHOLD
	#define PHYSCONTACT_RESTITUTION_THRESHOLD -1.f
#endif


typedef struct physicsContactPoint {
	//Used to uniquely identify the contact point.
	contactKey key;

	vec3 rA;
	vec3 rB;

	float penetration;
	//Stores the bias term, that is, the
	//restitution plus the Baumgarte term.
	float bias;

	//Stores the result of 1/JM^-1J^T (which is equivalent
	//to the inverse denominator of the impulse equation)
	//prior to collision response, as it only needs to be
	//calculated once. This is the effective mass.
	float normalImpulse;
	//Similar to "normalImpulse", but uses the tangent
	//vectors. These values are used for friction.
	float tangentImpulse[PHYSCONTACT_NUM_TANGENTS];

	//Accumulated impulses used for warm starting.
	//This helps prevent jittering in persistent contacts.
	float normalAccumulator;
	float tangentAccumulator[PHYSCONTACT_NUM_TANGENTS];
} physicsContactPoint;

/** It would probably be best to do something similar to what Box2D  **/
/** does with this structure, as ours is very prone to cache misses. **/

//A physics manifold is similar to a regular contact manifold,
//but stores additional information required to solve contacts.
typedef struct physicsManifold {
	physicsContactPoint contacts[CONTACT_MAX_POINTS];
	contactPointIndex_t numContacts;

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

void physManifoldPresolve(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt);
void physManifoldSolve(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


#endif