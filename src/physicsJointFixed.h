#ifndef physicsJointFixed_h
#define physicsJointFixed_h


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
	quat rotOffsetA;
	quat rotOffsetB;
} physicsJointFixedDef;

typedef struct physJointFixed {
	// Vectors from the bodies' points of
	// reference to the ball and socket.
	vec3 anchorA;
	vec3 anchorB;
	// Orientations of the anchors
	// for the angular constraints.
	quat rotOffsetA;
	quat rotOffsetB;

	#ifdef PHYSJOINTFIXED_ACCURATE_ANGULAR_MASS
	// Relative orientation between the rigid bodies,
	// including the contributions of the rotOffsets.
	quat qR;
	#endif
	// These points are in global space,
	// relative to the centres of mass.
	vec3 rA;
	vec3 rB;

	// Baumgarte bias terms for the constraints.
	vec3 angularBias;
	vec3 linearBias;

	// Effective masses, (JMJ^T)^{-1}.
	mat3 angularInvMass;
	mat3 linearInvMass;

	// Accumulated impulses used for warm starting.
	vec3 angularImpulse;
	vec3 linearImpulse;
} physicsJointFixed;


typedef struct physicsRigidBody physicsRigidBody;

void physJointFixedInit(
	physicsJointFixed *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const quat *const restrict rotOffsetA, const quat *const restrict rotOffsetB
);

#ifdef PHYSJOINTFIXED_WARM_START
void physJointFixedWarmStart(
	const physicsJointFixed *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
);
#endif
void physJointFixedPresolve(
	physicsJointFixed *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB,
	const float dt
);
void physJointFixedSolveVelocity(
	physicsJointFixed *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
);
return_t physJointFixedSolvePosition(
	const physicsJointFixed *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
);


#endif