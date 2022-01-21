#ifndef physicsJointFixed_h
#define physicsJointFixed_h


#define PHYSJOINT_TYPE_FIXED 1


#include "settingsPhysics.h"

#include "vec3.h"
#include "quat.h"
#include "mat3.h"

#include "utilTypes.h"


// Fixed joints prevent any relative translational or rotational movement.
typedef struct physJointFixedDef {
	// Vectors from the bodies' points of reference to
	// the points that we want to be fixed to each other.
	vec3 anchorA;
	vec3 anchorB;
	// Relative orientations that we want
	// the rigid bodies to be fixed at.
	quat anchorRotA;
	quat anchorRotB;
} physicsJointFixedDef;

typedef struct physJointFixed {
	// Vectors from the bodies' points of
	// reference to the ball and socket.
	vec3 anchorA;
	vec3 anchorB;
	// Orientations of the anchors
	// for the angular constraints.
	quat anchorRotA;
	quat anchorRotB;
	// These points are in global space,
	// relative to the centres of mass.
	vec3 rA;
	vec3 rB;

	// Baumgarte bias terms for the constraints.
	vec3 linearBias;

	// Effective masses, (JMJ^T)^{-1}.
	mat3 linearInvMass;

	// Accumulated impulses used for warm starting.
	vec3 linearImpulse;
} physicsJointFixed;


typedef struct physicsRigidBody physicsRigidBody;

void physJointFixedInit(
	physicsJointFixed *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const quat *const restrict anchorRotA, const quat *const restrict anchorRotB
);

#ifdef PHYSJOINTFIXED_WARM_START
void physJointFixedWarmStart(
	const physicsJointFixed *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif
void physJointFixedPresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointFixedSolveVelocity(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
return_t physJointFixedSolvePosition(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);


#endif