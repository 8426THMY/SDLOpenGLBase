#include "physicsJoint.h"


void (*physJointPresolveTable[PHYSJOINT_NUM_TYPES])(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt) = {
	physJointDistancePresolve,
	physJointFixedPresolve,
	physJointRevolutePresolve,
	physJointPrismaticPresolve,
	physJointSpherePresolve
};

void (*physJointSolveVelocityTable[PHYSJOINT_NUM_TYPES])(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB) = {
	physJointDistanceSolveVelocity,
	physJointFixedSolveVelocity,
	physJointRevoluteSolveVelocity,
	physJointPrismaticSolveVelocity,
	physJointSphereSolveVelocity
};

float (*physJointSolvePositionTable[PHYSJOINT_NUM_TYPES])(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB) = {
	physJointDistanceSolvePosition,
	physJointFixedSolvePosition,
	physJointRevoluteSolvePosition,
	physJointPrismaticSolvePosition,
	physJointSphereSolvePosition
};


#warning "We probably shouldn't just take in dt here, as we need 1/dt too."
void physJointPresolve(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt){
	physJointPresolveTable[joint->type]((void *)(&joint->data), bodyA, bodyB, dt);
}

void physJointSolveVelocity(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	physJointSolveVelocityTable[joint->type]((void *)(&joint->data), bodyA, bodyB);
}

void physJointSolvePosition(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	physJointSolvePositionTable[joint->type]((void *)(&joint->data), bodyA, bodyB);
}