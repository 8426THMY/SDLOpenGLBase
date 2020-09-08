#include "physicsJoint.h"


void (*const physJointPresolveTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB,
	const float dt
) = {

	physJointDistancePresolve,
	physJointFixedPresolve,
	physJointRevolutePresolve,
	physJointPrismaticPresolve,
	physJointSpherePresolve
};

void (*const physJointSolveVelocityTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
) = {

	physJointDistanceSolveVelocity,
	physJointFixedSolveVelocity,
	physJointRevoluteSolveVelocity,
	physJointPrismaticSolveVelocity,
	physJointSphereSolveVelocity
};

float (*const physJointSolvePositionTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
) = {

	physJointDistanceSolvePosition,
	physJointFixedSolvePosition,
	physJointRevoluteSolvePosition,
	physJointPrismaticSolvePosition,
	physJointSphereSolvePosition
};


#warning "We probably shouldn't just take in dt here, as we need 1/dt too."
void physJointPresolve(
	physicsJoint *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
){

	physJointPresolveTable[joint->type]((void *)(&joint->data), bodyA, bodyB, dt);
}

void physJointSolveVelocity(physicsJoint *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	physJointSolveVelocityTable[joint->type]((void *)(&joint->data), bodyA, bodyB);
}

void physJointSolvePosition(physicsJoint *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	physJointSolvePositionTable[joint->type]((void *)(&joint->data), bodyA, bodyB);
}