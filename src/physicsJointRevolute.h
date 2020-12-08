#ifndef physicsJointRevolute_h
#define physicsJointRevolute_h


#define PHYSJOINT_TYPE_REVOLUTE 2


#include "utilTypes.h"


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
#ifdef PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL
return_t physJointRevoluteSolvePosition(void *const restrict joint, physicsRigidBody *bodyA, physicsRigidBody *const restrict bodyB);
#endif


#endif