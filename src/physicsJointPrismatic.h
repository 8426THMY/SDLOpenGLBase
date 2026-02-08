#ifndef physicsJointPrismatic_h
#define physicsJointPrismatic_h


#include "settingsPhysics.h"

#include "vec2.h"
#include "vec3.h"
#include "quat.h"
#include "mat2.h"
#include "mat3.h"
#include "utilTypes.h"


#ifndef PHYSJOINTPRISMATIC_BAUMGARTE_BIAS
	#define PHYSJOINTPRISMATIC_BAUMGARTE_BIAS 0.4f
#endif


/*
** Prismatic joints prevent two rigid bodies from
** rotating relative to each other, but allows them
** to move along a fixed axis.
*/
typedef struct physicsJointPrismaticDef {
	// These points define where the joint is connected to the
	// respective colliders. They should be untransformed and
	// relative to the bodies' reference positions (origins).
	vec3 anchorA;
	vec3 anchorB;
	// Initial offset in orientation from rigid body A to B.
	quat rotOffsetAB;

	// This is a unit vector in the direction of
	// the axis the bodies should slide along.
	vec3 axis;
} physicsJointPrismaticDef;

typedef struct physicsJointPrismatic {
	// These points are in local space.
	vec3 anchorA;
	vec3 anchorB;
	// Initial offset in orientation from rigid body A to B.
	quat rotOffsetAB;

	// These store the constraint axis and
	// its tangent vectors in local space.
	vec3 axisLocal;
	vec3 tangentsLocal[2];
	// These store them in global space,
	// with respect to the centres of mass.
	vec3 axisGlobal;
	vec3 tangentsGlobal[2];

	// Used for the limit and motor constraints.
	// These vectors are stored with respect to
	// the rigid bodies' centres of mass.
	// ((rA + d) X a)
	vec3 rAa;
	// (rB X a)
	vec3 rBa;
	// Used for the linear and angular constraints.
	// ((rA + d) X u1)
	vec3 rAu1;
	// (rB X u1)
	vec3 rBu1;
	// ((rA + d) X u2)
	vec3 rAu2;
	// (rB X u2)
	vec3 rBu2;

	// Linear and angular effective masses.
	mat2 linearInvMass;
	mat3 angularInvMass;
	float limitMotorInvMass;

	// Accumulated impulses used for warm starting.
	vec2 linearImpulse;
	float angularImpulse;
	float limitImpulse;
	float motorImpulse;
} physicsJointPrismatic;


typedef struct physicsRigidBody physicsRigidBody;

void physJointPrismaticInit(physicsJointPrismatic *const restrict joint);

#ifdef PHYSJOINTPRISMATIC_WARM_START
void physJointPrismaticWarmStart(
	const physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
);
#endif
void physJointPrismaticPresolve(
	physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB,
	const float dt
);
void physJointPrismaticSolveVelocity(
	physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
);
return_t physJointPrismaticSolvePosition(
	const physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
);


#endif