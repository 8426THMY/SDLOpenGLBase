#ifndef physicsJointSphere_h
#define physicsJointSphere_h


#include "settingsPhysics.h"

#include "vec3.h"
#include "quat.h"
#include "mat3.h"

#include "utilTypes.h"


#ifndef PHYSJOINTSPHERE_BAUMGARTE_BIAS
	#define PHYSJOINTSPHERE_BAUMGARTE_BIAS 0.3f
#endif


#ifdef PHYSJOINTSPHERE_DEBUG
extern vec3 swingAxisDebug;
extern float swingAngleDebug;
extern vec3 twistAxisDebug;
extern float twistAngleDebug;

/*
if(debugObj != NULL){
	static float t = 0.f;

	//const quat rot = quatInitAxisAngleC(swingAxisDebug, swingAngleDebug);
	//const quat rot = quatInitAxisAngleC(twistAxisDebug, twistAngleDebug);
	const quat rot = quatInitIdentityC();//quatMultiplyQuatC(quatInitAxisAngleC(swingAxisDebug, swingAngleDebug), quatInitAxisAngleC(twistAxisDebug, twistAngleDebug));
	debugObj->boneTransforms[0].pos = vec3AddVec3C(
		vec3AddVec3C(
			vec3InitSetC(0.23907208442687988f, -0.20703732967376709f+2.f, -0.17244648933410645f-2.f),
			quatRotateVec3FastC(rot, vec3InitSetC(2.5f, 0.f, 0.f))
		),
		vec3NegateC(quatRotateVec3FastC(rot, debugObj->physBodies->base->centroid))
	);

	debugObj->boneTransforms[0].rot = rot;

	t += 0.01f;
}
*/
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
	quat rotOffsetA;
	quat rotOffsetB;

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
	quat rotOffsetA;
	quat rotOffsetB;

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
	// Swing and twist axes in global space.
	// Note that the twist axis is taken to
	// be rigid body B's transformed x-axis.
	vec3 swingAxis;
	vec3 twistAxis;
	// Difference between the current swing
	// and twist angles and their limits.
	float swingBias;
	float twistBias;

	// Inverse effective masses, (JMJ^T)^{-1}, for the
	// point-to-point (K1) and angular (K2, K3, K4) constraints.
	float swingInvMass;
	float twistInvMass;
	mat3 linearInvMass;

	// Accumulated impulses used for warm starting.
	float swingImpulse;
	float twistImpulse;
	vec3 linearImpulse;
} physicsJointSphere;


typedef struct physicsRigidBody physicsRigidBody;

void physJointSphereInit(
	physicsJointSphere *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const quat *const restrict rotOffsetA, const quat *const restrict rotOffsetB,
	const float restitution,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
);

#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(
	const physicsJointSphere *const restrict joint,
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