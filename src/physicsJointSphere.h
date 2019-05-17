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

void physJointSpherePresolve(void *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);
void physJointSphereSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


#endif