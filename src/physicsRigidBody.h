#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "transform.h"

#include "physicsCollider.h"


typedef struct physicsRigidBodyDef {
	physicsCollider *colliders;
	uint_least16_t numColliders;

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
	physicsCollider *colliders;
	uint_least16_t numColliders;

	float mass;
	float invMass;

	//We need to keep the local versions
	//for when we add new colliders.
	vec3 centroidLocal;
	vec3 centroidGlobal;
	mat3 invInertiaLocal;
	mat3 invInertiaGlobal;

	transformState transform;

	//Store the linear properties of the object.
	vec3 linearVelocity;
	vec3 netForce;

	//Store the angular properties of the object.
	vec3 angularVelocity;
	vec3 netTorque;
} physicsRigidBody;


void physRigidBodyDefInit(physicsRigidBodyDef *bodyDef);

void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *body, const float time);
void physRigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *body, const float time);

void physRigidBodyApplyImpulse(physicsRigidBody *body, const vec3 *r, const vec3 *J);
void physRigidBodyApplyImpulseInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J);

void physRigidBodyUpdate(physicsRigidBody *body, const float dt);

//void physRigidBodyDefCalculateCentroid(physicsRigidBodyDef *bodyDef);
//void physRigidBodyDefCalculateInertiaTensor(physicsRigidBodyDef *bodyDef);


/**
Collider centroid.
Body centroid.
Collider inertia tensor.
Body inertia tensor.
Body integration.

Body add colliders.
Body remove colliders.

Physics modules.
Physics allocators.

Load colliders.
Load bodies.
**/


#endif