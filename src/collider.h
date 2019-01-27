#ifndef collider_h
#define collider_h


#include "utilTypes.h"
#include "vec3.h"

#include "colliderHull.h"


#define COLLIDER_NUM_TYPES 1


typedef struct collider {
	//This array should be large enough
	//to store any type of collider.
	union {
		colliderHull hull;
	} data;
	//Stores which type of
	//collider this object is.
	byte_t type;
} collider;


//void colliderCalculateInertia(const collider *c, float inertia[6]);
return_t colliderCheckCollision(const collider *cA, const collider *cB, contactSeparation *cs, contactManifold *cm);


#endif