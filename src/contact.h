#ifndef contact_h
#define contact_h


#define CONTACT_MAX_POINTS 4
#define CONTACT_KEY_INVALID_EDGE -1


#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"
#include "vec3.h"


//Stores either the two faces or the two edges
//that is causing a separation between two hulls.
typedef struct contactSeparation {
	uint_least16_t featureA;
	uint_least16_t featureB;
	byte_t type;
} contactSeparation;


//Identifying features used to
//construct the contact point.
typedef struct contactKey {
	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
	uint_least16_t edgeA;
	uint_least16_t edgeB;
	#else
	uint_least16_t inEdgeA;
	uint_least16_t outEdgeA;
	uint_least16_t inEdgeB;
	uint_least16_t outEdgeB;
	#endif
} contactKey;

typedef struct contactPoint {
	//Used to uniquely identify the contact point.
	contactKey key;

	//Offsets of the contact points from
	//their colliders' centres of mass.
	vec3 rA;
	vec3 rB;

	//Penetration depth.
	float penetration;
} contactPoint;

//Stores every contact point involved in the collision.
typedef struct contactManifold {
	contactPoint contacts[CONTACT_MAX_POINTS];
	byte_t numContacts;

	vec3 normal;
} contactManifold;


#endif