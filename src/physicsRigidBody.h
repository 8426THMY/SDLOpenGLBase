#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "transform.h"

#include "physicsCollider.h"


typedef struct physicsRigidBodyDef {
	//Linked list of colliders used by this rigid body.
	physicsCollider *colliders;

	//The rigid body's physical properties.
	float mass;
	float invMass;

	vec3 centroid;
	//Matrix that describes how the body
	//resists rotation around an axis.
	mat3 invInertia;
} physicsRigidBodyDef;

//Rigid body instance.
typedef struct physicsRigidBody {
	//Linked list of colliders used by this rigid body.
	physicsCollider *colliders;

	float mass;
	float invMass;

	//We need to keep the local versions
	//for when we add new colliders.
	vec3 centroidLocal;
	vec3 centroidGlobal;
	mat3 invInertiaLocal;
	mat3 invInertiaGlobal;

	//Defines the body's spacial configuration.
	//The position stored here should not be
	//confused with the rigid body's centroid!
	transformState transform;

	//Defines the body's physical configuration.
	vec3 linearVelocity;
	vec3 angularVelocity;
	vec3 netForce;
	vec3 netTorque;
} physicsRigidBody;


void physRigidBodyDefInit(physicsRigidBodyDef *bodyDef);
void physRigidBodyInit(physicsRigidBody *body);

void physRigidBodyDefGenerateProperties(physicsRigidBodyDef *bodyDef);

void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *body, const float time);
void physRigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *body, const float time);

void physRigidBodyApplyImpulse(physicsRigidBody *body, const vec3 *r, const vec3 *J);
void physRigidBodyApplyImpulseInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J);

void physRigidBodyUpdate(physicsRigidBody *body, const float dt);

void physRigidBodyDefDelete(physicsRigidBodyDef *bodyDef);
void physRigidBodyDelete(physicsRigidBody *body);


//For the physical properties (mass, centroid and inertia tensor),
//it would probably be best to add a physics collider's contribution
//straight after it has been loaded. Our load function would then
//need to return the calculated mass, centroid and inertia tensor.
/**
Body mass, centroid and inertia tensor (with vertex weighting for hulls).

Body add colliders.
Body remove colliders.

Load colliders.
Load bodies.
**/


#endif