#ifndef physicsRigidBody_h
#define physicsRigidBody_h


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


/**
aabbNode {
	colliderAABB aabb;

	aabbNode *parent;
	union {
		struct {
			aabbNode *left;
			aabbNode *right;
		} children;
		struct {
			physicsRigidBody *body;
			collider *collider;
		} leaf;
	} data;

	size_t height;
};

aabbTree {
	aabbNode *root;
};


collisionPair {
	union {
		//Manifolds and separations store the bodies as
		//their first elements, so this is always usable.
		struct {
			physicsRigidBody *bodyA;
			physicsRigidBody *bodyB;
		} bodies;
		physicsManifold contact;
		physicsSeparation separation;
	} data;
	byte_t type;

	//We need these when we check
	//if the pair already exists.
	collider *colliderA;
	collider *colliderB;

	//Pointers to the pair's position
	//in the pair arrays of both bodies.
	//This is used to speed up removals.
	collisionPair **arrayA;
	collisionPair **arrayB;
};

physicsRigidBody {
	collider *colliders;
	size_t numColliders;

	//SLink(?)
	aabbNode *colliderNodes;

	//DLink
	//Both bodies involved store a
	//pointer to the collision pair.
	collisionPair **pairs;
};


We can either store the collisionPair pointer array
inside the physicsRigidBodies (in which case the pairs
themselves will need to store collider pointers) or
we can store them inside the AABB tree nodes.

Pros for storing it in body:
1. Uses way less memory.
   4 bytes per body as opposed to 4 bytes per tree node.
   Only leaf nodes need the pointer, so we would otherwise
   be storing the pointer a minimum of (2n - 1) as many
   times as we need to, assuming every body has one collider.

2. I can't think of anything else.


Pros for storing it in the node:
1. Don't need to search through all of the body's pairs
   when checking to see if one already exists.
   This is only valid for bodies with more than one collider.

2. Don't need to store collider pointers in the pairs.
   We only do this otherwise so we can check if a pair exists.


Justification for the removal of composite colliders:
Makes it a pain to allow adding and removing additional
colliders to a rigid body. If we want to add one to a body
that is not already using a composite collider, we need to
convert its current collider to one.

An alternative proposition: colliders cannot be added or removed.
Instead, we create a new rigid body and use constraints to "weld"
it to the original body.
**/


void rigidBodyDefInit(physicsRigidBodyDef *body);

void rigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *rb, const float time);
void rigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *rb, const float time);

//void rigidBodyDefCalculateCentroid(physicsRigidBodyDef *body);
//void rigidBodyDefCalculateInertiaTensor(physicsRigidBodyDef *body);


#endif