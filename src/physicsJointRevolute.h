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

void physJointRevolutePresolve(void *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);
void physJointRevoluteSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


#endif