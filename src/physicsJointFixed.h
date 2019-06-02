#ifndef physicsJointFixed_h
#define physicsJointFixed_h


#define PHYSJOINT_TYPE_FIXED 1


typedef struct physJointFixedDef {
	//
} physicsJointFixedDef;

typedef struct physJointFixed {
	//
} physicsJointFixed;


typedef struct physicsRigidBody physicsRigidBody;

void physJointFixedPresolve(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt);
void physJointFixedSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
void physJointFixedSolvePosition(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


#endif