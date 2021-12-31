#ifndef physicsJointFriction_h
#define physicsJointFriction_h


#include "settingsPhysics.h"

#include "vec3.h"
#include "mat2.h"


/*
** Friction joints are special types of motor joints
** that are intended to only be used by contacts
** should "PHYSCONTACT_USE_FRICTION_JOINT" be defined.
**
** As a result, they are not used by the "physicsJoint"
** structure and thus cannot be created on the fly. If
** you want this functionality, consider a motor joint.
*/
typedef struct physicsJointFriction {
	// These points are in global space, but they
	// are still relative to the centres of mass.
	// We get the average point halfway between
	// contact points points on both bodies.
	vec3 rA;
	vec3 rB;

	float friction;

	// These points are in global space. Note that
	// the normal is for a face on body A, and so
	// should roughly point from body A to body B.
	vec3 normal;
	vec3 tangents[2];

	// Linear and inverse angular effective masses.
	mat2 linearInvMass;
	float angularInvMass;

	// Accumulated impulses used for warm starting.
	vec2 linearImpulse;
	float angularImpulse;
} physicsJointFriction;


typedef struct physicsRigidBody physicsRigidBody;

#ifdef PHYSJOINTFRICTION_WARM_START
void physJointFrictionWarmStart(const physicsJointFriction *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#endif
void physJointFrictionCalculateInverseEffectiveMass(
	physicsJointFriction *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
void physJointFrictionSolveVelocity(
	physicsJointFriction *joint,
	physicsRigidBody *bodyA, physicsRigidBody *bodyB,
	const float maxForce
);


#endif