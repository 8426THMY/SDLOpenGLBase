#ifndef contact_h
#define contact_h


#include <stddef.h>
#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"
#include "vec3.h"

#include "collider.h"


#define CONTACT_MAX_POINTS 4
#define CONTACT_KEY_INVALID_EDGE valueInvalid(colliderEdgeIndex)


typedef size_t separationFeature;
typedef uint_least8_t separationType;
typedef uint_least8_t contactPointIndex;


// Stores either the two faces or the two edges
// that is causing a separation between two hulls.
typedef struct contactSeparation {
	separationFeature featureA;
	separationFeature featureB;
	separationType type;
} contactSeparation;


// Identifying features used to
// construct the contact point.
typedef struct contactKey {
	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
	colliderEdgeIndex edgeA;
	colliderEdgeIndex edgeB;
	#else
	colliderEdgeIndex inEdgeA;
	colliderEdgeIndex outEdgeA;
	colliderEdgeIndex inEdgeB;
	colliderEdgeIndex outEdgeB;
	#endif
} contactKey;

typedef struct contactPoint {
	// Position of the contacts in global space.
	// They are not relative to the centres of mass.
	vec3 pA;
	vec3 pB;

	vec3 normal;

	// Separation between the colliders.
	// Note that this is a negative quantity.
	float separation;
	// Used to uniquely identify the contact point.
	contactKey key;
} contactPoint;

// Stores every contact point involved in the collision.
typedef struct contactManifold {
	contactPoint contacts[CONTACT_MAX_POINTS];
	contactPointIndex numContacts;
} contactManifold;


return_t collidersAreSeparated(
	const collider *const restrict cA, const collider *const restrict cB, contactSeparation *const restrict cs
);
return_t collidersAreColliding(
	const collider *const restrict cA, const collider *const restrict cB,
	contactSeparation *const restrict cs, contactManifold *const restrict cm
);


extern return_t (*contactSeparationTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *const restrict cA,
	const void *const restrict cB,
	contactSeparation *const restrict cs
);
extern return_t (*contactCollisionTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *const restrict cA,
	const void *const restrict cB,
	contactSeparation *const restrict cs,
	contactManifold *const restrict cm
);


#endif