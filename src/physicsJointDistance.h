#ifndef physicsJointDistance_h
#define physicsJointDistance_h


#include "vec3.h"


#define PHYSJOINT_TYPE_DISTANCE 0


//Distance joints try to keep two rigid bodies a fixed
//distance away from each other. Depending on the damping,
//this joint can also be made to act like a spring.
typedef struct physicsJointDistanceDef {
	//These points define where the joint is
	//connected to the respective colliders.
	//They are defined in local space and
	//relative to the rigid bodies' origins.
	vec3 anchorA;
	vec3 anchorB;

	//The distance to maintain between anchor points.
	float length;
	//The natural (angular) frequency of the mass-spring-damper
	//system in radians per second. When set to 0, the joint
	//will not exhibit simple harmonic motion.
	float angularFrequency;
	//How quickly oscillations should stop. This isn't quite the
	//damping coefficient, as we haven't multiplied by the mass.
	//d = 2 * w * z
	float damping;
} physicsJointDistanceDef;

typedef struct physicsJointDistance {
	//These points are in local space.
	vec3 anchorA;
	vec3 anchorB;

	float length;
	float angularFrequency;
	float damping;

	//These points are in global space.
	vec3 rA;
	vec3 rB;
	vec3 rAB;

	float effectiveMass;
} physicsJointDistance;


typedef struct physicsRigidBody physicsRigidBody;

void physJointDistanceInit(physicsJointDistance *joint, const float frequency, const float dampingRatio);

void physJointDistancePresolve(void *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);
void physJointDistanceSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


#endif