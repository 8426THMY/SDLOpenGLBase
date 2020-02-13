#ifndef physicsJointRevolute_h
#define physicsJointRevolute_h


#define PHYSJOINT_TYPE_REVOLUTE 2


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
float physJointRevoluteSolvePosition(void *const restrict joint, physicsRigidBody *bodyA, physicsRigidBody *const restrict bodyB);


#endif