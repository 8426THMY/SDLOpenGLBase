#ifndef physicsJointSphere_h
#define physicsJointSphere_h


#include "settingsPhysics.h"

#include "vec3.h"
#include "quat.h"
#include "mat3.h"

#include "utilTypes.h"


#define PHYSJOINT_TYPE_SPHERE 4

#ifndef PHYSJOINTSPHERE_BAUMGARTE_BIAS
	#define PHYSJOINTSPHERE_BAUMGARTE_BIAS 0.3f
#endif


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
	// Orientations of the anchors for the angular constraints.
	// Although our minimum angular limits are non-positive and
	// our maximum angular limits are non-negative, these allow
	// us to effectively bypass these constraints by specifying
	// a new "origin" for our cone.
	quat anchorRotA;
	quat anchorRotB;

	// The hardness is a real number in [0, 1], where '1' gives
	// a rigid constraint and '0' gives a totally soft constraint.
	///float hardness;
	// The restitution is a real number in [0, 1], where '1'
	// means full bounciness and '0' means no bounciness.
	float restitution;

	// Stores the minimum and maximum angular
	// limits (in that order) for each axis.
	// Note that we require min <= 0 <= max.
	//
	// For Euler angle constraints, we use the
	// order XYZ for rotations. Therefore, to
	// guarantee a unique representation for any
	// orientation, we must limit X to [-pi, pi],
	// Y to [-pi/2, pi/2] and Z to [-pi, pi].
	float angularLimitsX[2];
	float angularLimitsY[2];
	float angularLimitsZ[2];
} physicsJointSphereDef;

typedef struct physicsJointSphere {
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

	// Stores flags that tell us which limits need to be solved.
	// If we're using quaternion swing-twist limits, we use a
	// single bit to say whether the swing limit is broken. For
	// all other angular limits, we use two bits to say whether
	// the lower or upper limits have been broken.
	flags_t limitStates;

	// The hardness is a real number in [0, 1], where '1' gives
	// a rigid constraint and '0' gives a totally soft constraint.
	///float hardness;
	// The restitution is a real number in [0, 1], where '1'
	// means full bounciness and '0' means no bounciness.
	float restitution;

	// Offset of the ball (rigid body B)
	// from the socket (rigid body A).
	vec3 linearBias;

	// Stores the minimum and maximum angular
	// limits (in that order) for each axis.
	//
	// For Euler angle constraints, we use the
	// order XYZ for rotations. Therefore, to
	// guarantee a unique representation for any
	// orientation, we must limit X to [-pi, pi],
	// Y to [-pi/2, pi/2] and Z to [-pi, pi].
	float angularLimitsX[2];
	float angularLimitsY[2];
	float angularLimitsZ[2];
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	// Swing and twist axes in global space.
	// Note that the twist axis is taken to
	// be rigid body B's transformed x-axis.
	vec3 swingAxis;
	vec3 twistAxis;
	// Difference between the current swing
	// and twist angles and their limits.
	float swingBias;
	float twistBias;
	#else
	// Rotation axes in global space.
	vec3 angularAxisX;
	vec3 angularAxisY;
	vec3 angularAxisZ;
	// Difference between the current
	// axis angles and their limits.
	vec3 angularBias;
	#endif

	// Inverse effective masses, (JMJ^T)^{-1}, for the
	// point-to-point (K1) and angular (K2, K3, K4) constraints.
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	float swingInvMass;
	float twistInvMass;
	#else
	vec3 angularInvMass;
	#endif
	mat3 linearInvMass;

	// Accumulated impulses used for warm starting.
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	float swingImpulse;
	float twistImpulse;
	#else
	vec3 angularImpulse;
	#endif
	vec3 linearImpulse;
} physicsJointSphere;


typedef struct physicsRigidBody physicsRigidBody;

void physJointSphereInit(
	physicsJointSphere *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const quat *const restrict anchorRotA, const quat *const restrict anchorRotB,
	const float restitution,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
);

#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(
	const physicsJointSphere *const restrict joint, const flags_t limitStates,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif
void physJointSpherePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointSphereSolveVelocity(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
return_t physJointSphereSolvePosition(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);


#endif