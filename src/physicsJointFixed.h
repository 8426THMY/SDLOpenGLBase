#ifndef physicsJointFixed_h
#define physicsJointFixed_h


#define PHYSJOINT_TYPE_FIXED 1


#include "utilTypes.h"


typedef struct physJointFixedDef {
	//
} physicsJointFixedDef;

typedef struct physJointFixed {
	//
} physicsJointFixed;


typedef struct physicsRigidBody physicsRigidBody;

void physJointFixedPresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointFixedSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#ifdef PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL
return_t physJointFixedSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
#endif


#endif