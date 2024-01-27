#ifndef physicsCollider_h
#define physicsCollider_h


#include "settingsPhysics.h"

#include "collider.h"
#include "physicsContact.h"
#include "aabbTree.h"

#include "utilTypes.h"


// If we're using non-linear Gauss-Seidel stabilisation
// anywhere, we'll need to define some additional functions.
#if defined(PHYSCONTACT_STABILISER_GAUSS_SEIDEL) || defined(PHYSJOINT_USE_POSITIONAL_CORRECTION)
	#define PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
#endif

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


typedef uint_least16_t physicsColliderLayer;

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

	// Colliders store doubly linked lists of active contacts and
	// separations. These lists are mostly sorted according to the
	// addresses of the second collider involved in the contact or
	// separation. Check the explanation given in "physicsConstraintPair.h".
	//
	// Note that the list is actually allocated and managed by the
	// island, this just stores a pointer to the collider's first joint.
	physicsContactPair *contacts;
	physicsSeparationPair *separations;

	// Stores the layers that this collider lives on.
	physicsColliderLayer layer;
	// Stores the layers that this collider checks for collision with.
	physicsColliderLayer mask;
} physicsCollider;


void physColliderInit(physicsCollider *const restrict pc, const colliderType type);
void physColliderInstantiate(physicsCollider *const restrict pc, const physicsCollider *const restrict base, physicsRigidBody *const restrict owner);

void physColliderUpdate(physicsCollider *const restrict collider);

return_t physColliderPermitCollision(const physicsCollider *const colliderA, const physicsCollider *const colliderB);

physicsContactPair *physColliderFindContact(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsContactPair **const restrict prev, physicsContactPair **const restrict next
);
physicsSeparationPair *physColliderFindSeparation(
	const physicsCollider *const restrict colliderA, const physicsCollider *const restrict colliderB,
	physicsSeparationPair **const restrict prev, physicsSeparationPair **const restrict next
);
void physColliderClearPairs(physicsCollider *const restrict collider);

void physColliderDeleteInstance(physicsCollider *const restrict collider);
void physColliderDelete(physicsCollider *const restrict collider);


#endif