#ifndef physicsJointFixed_h
#define physicsJointFixed_h


#define PHYSJOINT_TYPE_FIXED 1


#include "settingsPhysics.h"

#include "utilTypes.h"


// Fixed joints prevent any relative translational or rotational movement.
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
return_t physJointFixedSolvePosition(const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);


#endif