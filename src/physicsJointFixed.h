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

void physJointFixedPresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointFixedSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
float physJointFixedSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);


#endif