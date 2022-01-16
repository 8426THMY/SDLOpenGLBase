#ifndef physicsJointRevolute_h
#define physicsJointRevolute_h


#define PHYSJOINT_TYPE_REVOLUTE 2


#include "settingsPhysics.h"

#include "utilTypes.h"


/*
** Revolute joints, also known as hinge joints, constrain all
** relative movement to rotation about a single, fixed axis.
*/
typedef struct physicsJointRevoluteDef {
	//
} physicsJointRevoluteDef;

typedef struct physicsJointRevolute {
	//
} physicsJointRevolute;


typedef struct physicsRigidBody physicsRigidBody;

void physJointRevolutePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointRevoluteSolveVelocity(void *const restrict joint, physicsRigidBody *bodyA, physicsRigidBody *const restrict bodyB);
return_t physJointRevoluteSolvePosition(const void *const restrict joint, physicsRigidBody *bodyA, physicsRigidBody *const restrict bodyB);


#endif