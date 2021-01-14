#ifndef physicsJointSphere_h
#define physicsJointSphere_h


#include "vec3.h"
#include "mat3.h"
#include "utilTypes.h"


#define PHYSJOINT_TYPE_SPHERE 4


/*
** Spherical joints act similarly to ball and socket joints, that is,
** they keep two points (defined relative to the rigid bodies) coincident.
**
** Note that this is identical to a distance joint with the distance
** set to 0 and an additional constraint placed on the relative angles.
** We don't use the same type of point-to-point constraint as the
** distance joint though, as it seems unstable when the distance is 0.
*/
typedef struct physicsJointSphereDef {
	// Vectors from the bodies' points of
	// reference to the ball and socket.
	vec3 anchorA;
	vec3 anchorB;

	// Stores the minimum and maximum angular
	// limits (in that order) for each axis.
	float angleLimitsX[2];
	float angleLimitsY[2];
	float angleLimitsZ[2];
} physicsJointSphereDef;

typedef struct physicsJointSphere {
	// Vectors from the bodies' points of
	// reference to the ball and socket.
	vec3 anchorA;
	vec3 anchorB;
	// These points are in global space, but they
	// are still relative to the centres of mass.
	vec3 rA;
	vec3 rB;

	// Stores the minimum and maximum angular
	// limits (in that order) for each axis.
	float angleLimitsX[2];
	float angleLimitsY[2];
	float angleLimitsZ[2];
	// Difference between the current swing
	// and twist angles and their limits.
	float swingBias;
	float twistBias;
	// Swing and twist axes in global space.
	// Note that the twist axis is taken to
	// be rigid body B's transformed x-axis.
	vec3 swingAxis;
	vec3 twistAxis;

	// Effective masses for the point-to-point
	// (K1) and angular (K2) constraints.
	mat3 linearMass;
	float swingInvMass;
	float twistInvMass;

	// Accumulated impulses used for warm starting.
	vec3 linearImpulse;
	float swingImpulse;
	float twistImpulse;
} physicsJointSphere;


typedef struct physicsRigidBody physicsRigidBody;

void physJointSphereInit(
	physicsJointSphere *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
);

#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(const physicsJointSphere *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#endif
void physJointSpherePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointSphereSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
return_t physJointSphereSolvePosition(const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);


#endif