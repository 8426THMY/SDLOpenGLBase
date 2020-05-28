#ifndef physicsCollider_h
#define physicsCollider_h


#include "settingsPhysics.h"

#include "collider.h"
#include "physicsConstraintPair.h"
#include "aabbTree.h"

#include "utilTypes.h"


#ifndef PHYSCOLLIDER_DEFAULT_MASS
	#define PHYSCOLLIDER_DEFAULT_MASS        0.f
#endif
#ifndef PHYSCOLLIDER_DEFAULT_DENSITY
	#define PHYSCOLLIDER_DEFAULT_DENSITY     0.f
#endif
#ifndef PHYSCOLLIDER_DEFAULT_FRICTION
	#define PHYSCOLLIDER_DEFAULT_FRICTION    1.f
#endif
#ifndef PHYSCOLLIDER_DEFAULT_RESTITUTION
	#define PHYSCOLLIDER_DEFAULT_RESTITUTION 1.f
#endif


typedef struct physicsRigidBody physicsRigidBody;
typedef struct physicsCollider {
	// To reduce computation between frames, specifically
	// for convex hulls, we store a copy of the collider
	// in local space and one in global space.
	collider global;
	const collider *local;
	// Tightly-fitting bounding box, used to check
	// if we should perform narrowphase collision.
	colliderAABB aabb;

	float density;
	float friction;
	float restitution;

	// This will be NULL unless the
	// collider has been instantiated.
	physicsRigidBody *owner;
	aabbNode *node;

	// Colliders store linked lists of active contacts
	// and separations. These lists are mostly sorted
	// according to the addresses of the second collider
	// involved in the contact or separation. I say
	// "mostly" because only pairs where this collider
	// is the first really need to be sorted.
	physicsContactPair *contacts;
	physicsSeparationPair *separations;
} physicsCollider;


void physColliderInit(physicsCollider *const restrict pc, const colliderType_t type);
void physColliderInstantiate(physicsCollider *const restrict pc, const physicsCollider *const restrict base, physicsRigidBody *const restrict owner);

void physColliderUpdate(physicsCollider *const restrict collider);

return_t physColliderPermitCollision(physicsCollider *const colliderA, physicsCollider *const colliderB);

physicsContactPair *physColliderFindContact(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsContactPair **const restrict prev, physicsContactPair **const restrict next
);
physicsSeparationPair *physColliderFindSeparation(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsSeparationPair **const restrict prev, physicsSeparationPair **const restrict next
);
void physColliderUpdateSeparations(physicsCollider *const restrict collider);
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physColliderUpdateContacts(physicsCollider *const restrict collider, const float invDt);
#else
void physColliderUpdateContacts(physicsCollider *const restrict collider);
#endif
void physColliderClearPairs(physicsCollider *const restrict collider);

void physColliderDeleteInstance(physicsCollider *const restrict collider);
void physColliderDelete(physicsCollider *const restrict collider);


#endif