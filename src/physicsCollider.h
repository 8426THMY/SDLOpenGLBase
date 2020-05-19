#ifndef physicsCollider_h
#define physicsCollider_h


#include "settingsPhysics.h"

#include "collider.h"
#include "physicsConstraintPair.h"
#include "aabbTree.h"


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

	physicsRigidBody *owner;
	aabbNode *node;

	#warning "Use quad-lists for these!"
	// Colliders store linked lists of active contacts
	// and separations. These lists are mostly sorted
	// according to the addresses of the second collider
	// involved in the contact or separation. I say
	// "mostly" because only pairs where this collider
	// is the first really need to be sorted.
	physicsSeparationPair *separations;
	physicsContactPair *contacts;
} physicsCollider;


typedef struct physicsIsland physicsIsland;

void physColliderInit(physicsCollider *const restrict pc, const colliderType_t type, void *const restrict owner);
void physColliderInstantiate(physicsCollider *const restrict pc, const physicsCollider *const restrict base, void *const restrict owner);

void physColliderGenerateCentroid(physicsCollider *const restrict collider, vec3 *const restrict centroid);
void physColliderGenerateInertia(physicsCollider *const restrict collider, const vec3 *const restrict centroid, mat3 *const restrict inertia);

void physColliderUpdate(physicsCollider *const restrict collider, physicsIsland *const restrict island);
void physColliderQueryCollisions(physicsCollider *const restrict collider);

physicsSeparationPair *physColliderFindSeparation(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsSeparationPair **const restrict prev, physicsSeparationPair **const restrict next
);
physicsContactPair *physColliderFindContact(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsContactPair **const restrict prev, physicsContactPair **const restrict next
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

void physColliderCollisionCallback(void *const restrict colliderA, void *const restrict colliderB);


extern void (*physColliderGenerateCentroidTable[COLLIDER_NUM_TYPES])(
	const void *const restrict collider,
	vec3 *const restrict centroid
);

extern void (*physColliderGenerateInertiaTable[COLLIDER_NUM_TYPES])(
	const void *const restrict collider,
	const vec3 *const restrict centroid,
	mat3 *const restrict inertia
);


#endif