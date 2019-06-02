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
	//These points are in global space, but they
	//are still relative to the centres of mass.
	//We get the average point halfway between
	//contact points points on both bodies.
	vec3 rA;
	vec3 rB;

	//These points are in global space. Note that
	//the normal is for a face on body A, and so
	//should roughly point from body A to body B.
	vec3 normal;
	vec3 tangents[2];

	//Angular and linear effective masses.
	mat2 linearMass;
	float angularMass;
	//Accumulated impulses used for warm starting.
	vec2 linearImpulse;
	float angularImpulse;
} physicsJointFriction;


typedef struct physicsRigidBody physicsRigidBody;

void physJointFrictionWarmStart(const physicsJointFriction *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
void physJointFrictionCalculateEffectiveMass(physicsJointFriction *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
void physJointFrictionSolveVelocity(physicsJointFriction *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float maxFriction);


#endif