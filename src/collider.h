#ifndef collider_h
#define collider_h


#include <stdio.h>


#include "vec3.h"
#include "mat3.h"

#include "utilTypes.h"
#include "transform.h"

#include "colliderAABB.h"
#include "colliderHull.h"


#define COLLIDER_NUM_TYPES 1


typedef type_t colliderType_t;


typedef struct collider {
	// This should be large enough
	// to store any type of collider.
	union {
		colliderHull hull;
	} data;
	// Stores which type of
	// collider this object is.
	colliderType_t type;
} collider;


void colliderInit(collider *c, const colliderType_t type);
void colliderInstantiate(collider *c, const collider *cBase);

return_t colliderLoad(collider *c, FILE *cFile, vec3 *centroid, mat3 *inertia);
void colliderUpdate(collider *c, const collider *cBase, const transformState *trans, colliderAABB *aabb);

void colliderDeleteInstance(collider *c);
void colliderDelete(collider *c);


extern void (*colliderInstantiateTable[COLLIDER_NUM_TYPES])(
	void *c,
	const void *cBase
);

extern return_t (*colliderLoadTable[COLLIDER_NUM_TYPES])(
	void *c,
	FILE *cFile,
	vec3 *centroid,
	mat3 *inertia
);
extern void (*colliderUpdateTable[COLLIDER_NUM_TYPES])(
	void *c,
	const void *cBase,
	const transformState *trans,
	colliderAABB *aabb
);

extern void (*colliderDeleteInstanceTable[COLLIDER_NUM_TYPES])(
	void *c
);
extern void (*colliderDeleteTable[COLLIDER_NUM_TYPES])(
	void *c
);


#endif