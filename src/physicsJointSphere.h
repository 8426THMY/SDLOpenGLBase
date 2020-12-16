#ifndef physicsJointSphere_h
#define physicsJointSphere_h


#include "vec3.h"
#include "mat3.h"
#include "utilTypes.h"


#define PHYSJOINT_TYPE_SPHERE 4


typedef struct physicsJointSphereDef {
	// Vectors from the bodies' points of
	// reference to the ball and socket.
	vec3 axisLocalA;
	vec3 axisLocalB;

	// Minimum and maximum Euler angles used
	// when clamping body B's orientation.
	vec3 anglesMin;
	vec3 anglesMax;
} physicsJointSphereDef;

typedef struct physicsJointSphere {
	// Vectors from the bodies' points of
	// reference to the ball and socket.
	vec3 axisLocalA;
	vec3 axisLocalB;
	vec3 axisGlobalA;
	vec3 axisGlobalB;

	// Minimum and maximum Euler angles used
	// when clamping body B's orientation.
	vec3 anglesMin;
	vec3 anglesMax;

	// Effective masses for the point-to-point
	// (K1) and angular (K2) constraints.
	vec3 invLinearMass;
	mat3 angularMass;
} physicsJointSphere;


typedef struct physicsRigidBody physicsRigidBody;

void physJointSphereInit(
	physicsJointSphere *const restrict joint,
	const vec3 *axisA, const vec3 *axisB,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
);

#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(physicsJointSphere *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#endif
void physJointSpherePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointSphereSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#ifdef PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL
return_t physJointSphereSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#endif


#endif