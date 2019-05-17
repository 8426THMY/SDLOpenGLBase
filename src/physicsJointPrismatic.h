#ifndef physicsJointPrismatic_h
#define physicsJointPrismatic_h


#define PHYSJOINT_TYPE_PRISMATIC 3


typedef struct physicsJointPrismaticDef {
	//
} physicsJointPrismaticDef;

typedef struct physicsJointPrismatic {
	//
} physicsJointPrismatic;


typedef struct physicsRigidBody physicsRigidBody;

void physJointPrismaticPresolve(void *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);
void physJointPrismaticSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


#endif