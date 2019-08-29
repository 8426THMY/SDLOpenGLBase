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
	collider *local;
	// Tightly-fitting bounding box, used to check
	// if we should perform narrowphase collision.
	colliderAABB aabb;

	float density;
	float friction;
	float restitution;

	physicsRigidBody *owner;
	aabbNode *node;

	// Colliders store linked lists of active contacts
	// and separations. These lists are mostly sorted
	// according to the addresses of the second collider
	// involved in the contact or separation. I say
	// "mostly" because only pairs where this collider
	// is the first really need to be sorted.
	physicsSeparationPair *separations;
	physicsContactPair *contacts;

	// They also store a linked list of joints
	// that they are a part of. These behave
	// similarly to the previous linked lists.
	physicsJointPair *joints;
} physicsCollider;


typedef struct physicsIsland physicsIsland;

void physColliderInit(physicsCollider *pc, const colliderType_t type, void *owner);
void physColliderInstantiate(physicsCollider *pc, physicsCollider *base, void *owner);

void physColliderGenerateCentroid(physicsCollider *collider, vec3 *centroid);
void physColliderGenerateInertia(physicsCollider *collider, const vec3 *centroid, mat3 *inertia);

void physColliderUpdate(physicsCollider *collider, physicsIsland *island);
void physColliderQueryCollisions(physicsCollider *collider);

physicsSeparationPair *physColliderFindSeparation(const physicsCollider *colliderA, const physicsCollider *colliderB,
                                                  physicsSeparationPair **prev, physicsSeparationPair **next);
physicsContactPair *physColliderFindContact(const physicsCollider *colliderA, const physicsCollider *colliderB,
                                            physicsContactPair **prev, physicsContactPair **next);
void physColliderUpdateSeparations(physicsCollider *collider);
void physColliderUpdateContacts(physicsCollider *collider, const float dt);
void physColliderClearPairs(physicsCollider *collider);

void physColliderDeleteInstance(physicsCollider *collider);
void physColliderDelete(physicsCollider *collider);

void physColliderCollisionCallback(void *colliderA, void *colliderB);


extern void (*physColliderGenerateCentroidTable[COLLIDER_NUM_TYPES])(
	const void *collider,
	vec3 *centroid
);

extern void (*physColliderGenerateInertiaTable[COLLIDER_NUM_TYPES])(
	const void *collider,
	const vec3 *centroid,
	mat3 *inertia
);


#endif