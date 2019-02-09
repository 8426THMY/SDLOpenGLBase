#ifndef contact_h
#define contact_h


#include <stddef.h>
#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"
#include "vec3.h"

#include "collider.h"


#define CONTACT_MAX_POINTS 4
#define CONTACT_KEY_INVALID_EDGE -1


typedef size_t separationFeature_t;
typedef uint_least8_t separationType_t;
typedef uint_least8_t contactPointIndex_t;


//Stores either the two faces or the two edges
//that is causing a separation between two hulls.
typedef struct contactSeparation {
	separationFeature_t featureA;
	separationFeature_t featureB;
	separationType_t type;
} contactSeparation;


//Identifying features used to
//construct the contact point.
typedef struct contactKey {
	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
	colliderEdgeIndex_t edgeA;
	colliderEdgeIndex_t edgeB;
	#else
	colliderEdgeIndex_t inEdgeA;
	colliderEdgeIndex_t outEdgeA;
	colliderEdgeIndex_t inEdgeB;
	colliderEdgeIndex_t outEdgeB;
	#endif
} contactKey;

typedef struct contactPoint {
	//Used to uniquely identify the contact point.
	contactKey key;

	//Position of the contact.
	vec3 position;
	//Penetration depth.
	float penetration;
} contactPoint;

//Stores every contact point involved in the collision.
typedef struct contactManifold {
	contactPoint contacts[CONTACT_MAX_POINTS];
	contactPointIndex_t numContacts;

	vec3 normal;
} contactManifold;


return_t collidersAreSeparated(const collider *cA, const collider *cB, contactSeparation *cs);
return_t collidersAreColliding(const collider *cA, const collider *cB, contactSeparation *cs, contactManifold *cm);


extern return_t (*contactSeparationTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *cA,
	const void *cB,
	contactSeparation *cs
);
extern return_t (*contactCollisionTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *cA,
	const void *cB,
	contactSeparation *cs,
	contactManifold *cm
);


#endif