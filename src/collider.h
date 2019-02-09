#ifndef collider_h
#define collider_h


#include "utilTypes.h"
#include "vec3.h"

#include "colliderAABB.h"
#include "colliderHull.h"


#define COLLIDER_NUM_TYPES 1


typedef uint_least8_t colliderType_t;


typedef struct collider {
	//This array should be large enough
	//to store any type of collider.
	union {
		colliderHull hull;
	} data;
	//Stores which type of
	//collider this object is.
	colliderType_t type;
} collider;


void colliderInit(collider *c, const colliderType_t type);
void colliderInstantiate(collider *c, const collider *cBase);

void colliderUpdate(collider *c, const collider *cBase, const transformState *trans, colliderAABB *aabb);

void colliderDeleteInstance(collider *c);
void colliderDelete(collider *c);

//void colliderCalculateInertia(const collider *c, float inertia[6]);


extern void (*colliderInstantiateTable[COLLIDER_NUM_TYPES])(
	void *c,
	const void *cBase
);
extern void (*colliderDeleteTable[COLLIDER_NUM_TYPES])(
	void *c
);

extern void (*colliderUpdateTable[COLLIDER_NUM_TYPES])(
	void *c,
	const void *cBase,
	const transformState *trans,
	colliderAABB *aabb
);


#endif