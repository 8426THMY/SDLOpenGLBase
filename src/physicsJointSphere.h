#ifndef physicsJointSphere_h
#define physicsJointSphere_h


#define PHYSJOINT_TYPE_SPHERE 4


typedef struct physicsJointSphereDef {
	//
} physicsJointSphereDef;

typedef struct physicsJointSphere {
	//
} physicsJointSphere;


typedef struct physicsRigidBody physicsRigidBody;

void physJointSpherePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointSphereSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
float physJointSphereSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);


#endif