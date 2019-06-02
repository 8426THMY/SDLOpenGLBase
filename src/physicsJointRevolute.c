#include "physicsJointRevolute.h"


#include "physicsRigidBody.h"


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointRevolutePresolve(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt){
	//
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointRevoluteSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	//
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times.
*/
void physJointRevoluteSolvePosition(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	//
}