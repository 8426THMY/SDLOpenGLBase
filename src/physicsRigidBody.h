#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdio.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"

#include "collider.h"


typedef struct physicsRigidBodyDef {
	collider collider;

	//The rigid body's physical properties.
	float mass;
	float invMass;

	vec3 centroid;
	//Matrix that describes how the body
	//resists rotation around an axis.
	mat3 invInertia;

	//Scalar representing the ratio of
	//energy to conserve after a collision.
	float restitution;
} physicsRigidBodyDef;

typedef struct physicsRigidBody {
	physicsRigidBodyDef *body;

	//A "physicsRigidBodyDef" stores the local
	//state of its collider, but we will need
	//to store its global, transformed state.
	collider collider;

	//The same idea as above applies here.
	vec3 centroid;
	mat3 invInertia;

	//Store the linear properties of the object.
	vec3 pos;
	vec3 linearVelocity;
	vec3 netForce;

	//Store the angular properties of the object.
	quat rot;
	vec3 angularVelocity;
	vec3 netTorque;
} physicsRigidBody;


void rigidBodyDefInitProperties(physicsRigidBodyDef *body);

void rigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *rb, const float time);
void rigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *rb, const float time);

//void rigidBodyDefCalculateCentroid(physicsRigidBodyDef *body);
//void rigidBodyDefCalculateInertiaTensor(physicsRigidBodyDef *body);


#endif