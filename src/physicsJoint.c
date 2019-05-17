#include "physicsJoint.h"


void (*physJointPresolveTable[PHYSJOINT_NUM_TYPES])(void *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt) = {
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


void physJointPresolve(physicsJoint *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	physJointPresolveTable[joint->type]((void *)(&joint->data), bodyA, bodyB, dt);
}

void physJointSolveVelocity(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	physJointSolveVelocityTable[joint->type]((void *)(&joint->data), bodyA, bodyB);
}