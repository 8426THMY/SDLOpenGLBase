#ifndef contact_h
#define contact_h


#define CONTACT_MAX_POINTS 4


#include "utilTypes.h"
#include "vec3.h"


typedef struct contactPoint {
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