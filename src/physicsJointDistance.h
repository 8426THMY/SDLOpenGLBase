#ifndef physicsJointDistance_h
#define physicsJointDistance_h


#include "settingsPhysics.h"

#include "vec3.h"
#include "utilTypes.h"


/*
** Distance joints try to keep two rigid bodies a fixed
** distance away from each other. Depending on the damping,
** this joint can also be made to act like a spring.
*/
typedef struct physicsJointDistanceDef {
	// These points define where the joint is connected to the
	// respective colliders. They should be untransformed and
	// relative to the bodies' reference positions (origins).
	vec3 anchorA;
	vec3 anchorB;

	// The distance to maintain between anchor points.
	float distance;
	// The stiffness k of the spring. When set to 0, the
	// joint will not use ODE's soft, spring constraints.
	float stiffness;
	// The damping coefficient c of the spring.
	float damping;
} physicsJointDistanceDef;

typedef struct physicsJointDistance {
	// These points are in local space, with respect
	// to the bodies reference positions (origins).
	vec3 anchorA;
	vec3 anchorB;

	// These points are in global space,
	// relative to the centres of mass.
	vec3 rA;
	vec3 rB;
	vec3 rAB;

	float distance;
	// The natural frequency is w = sqrt(k/m).
	// The critical damping is c = 2sqrt(km).
	float stiffness;
	float damping;

	// The "magic" constants from Erin Catto's
	// GDC 2011 talk on soft constraints. The
	// bias is simply a Baumgarte term with a
	// special value for beta.
	//
	// gamma = 1/(hk + c)
	// beta  = hk/(hk + c)
	// bias  = beta/h * C(x)
	float gamma;
	float bias;

	float invEffectiveMass;

	// Accumulated impulse used for warm starting.
	float impulse;
} physicsJointDistance;


typedef struct physicsRigidBody physicsRigidBody;

void physJointDistanceInit(
	physicsJointDistance *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const float distance, const float stiffness, const float damping
);

#ifdef PHYSJOINTDISTANCE_WARM_START
void physJointDistanceWarmStart(const physicsJointDistance *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#endif
void physJointDistancePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointDistanceSolveVelocity(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
return_t physJointDistanceSolvePosition(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);


#endif