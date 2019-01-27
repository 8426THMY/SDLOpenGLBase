#ifndef physicsCollider_h
#define physicsCollider_h


#include "collider.h"
#include "physicsContactPair.h"
#include "aabbTree.h"


typedef struct physicsRigidBody physicsRigidBody;
typedef struct physicsCollider {
	//To reduce computation between frames, specifically
	//for convex hulls, we store a copy of the collider
	//in local space and one in global space.
	collider *colliderLocal;
	collider colliderGlobal;

	float density;
	float friction;
	float restitution;

	physicsRigidBody *owner;
	aabbNode *node;

	//Colliders store linked lists of
	//active contacts and separations.
	physicsContactPair *contacts;
	physicsSeparationPair *separations;
} physicsCollider;


void physColliderInit(physicsCollider *pc, collider *c, physicsRigidBody *body);

void physColliderUpdate(physicsCollider *collider);
void physColliderUpdatePairs(physicsCollider *collider);


#endif