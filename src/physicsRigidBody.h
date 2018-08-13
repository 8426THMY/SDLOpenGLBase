#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdio.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"

#include "physicsCollider.h"


typedef struct physicsRigidBody {
	//Array containing the rigid body's convex hulls.
	physicsCollider *colliders;
	size_t numColliders;

	//The rigid body's physical properties.
	float mass;
	float inverseMass;

	//Matrix that describes how the body
	//resists rotation around an axis.
	mat3 inverseInertiaTensor;

	//Scalar representing the ratio of
	//energy to conserve after a collision.
	float restitution;

	//Stores the rigid body's centre of mass.
	vec3 centroid;
} physicsRigidBody;

typedef struct physRigidBodyInst {
	physicsRigidBody *body;

	//While physicsRigidBodies store the local state of each of their
	//colliders, we also need to store their global, transformed states.
	physicsCollider *colliders;
	mat3 inverseInertiaTensor;

	//The same idea as above applies here.
	vec3 centroid;

	//Store the linear properties of the object.
	vec3 pos;
	vec3 linearVelocity;
	vec3 netForce;

	//Store the angular properties of the object.
	quat rot;
	vec3 angularVelocity;
	vec3 netTorque;
} physRigidBodyInst;


void rbInitProperties(physicsRigidBody *body);

void rbInstIntegrateVelocitySymplecticEuler(physRigidBodyInst *rb, const float time);
void rbInstIntegratePositionSymplecticEuler(physRigidBodyInst *rb, const float time);

void rbCalculateCentroid(physicsRigidBody *body);
void rbCalculateInertiaTensor(physicsRigidBody *body);


#endif