#ifndef collider_h
#define collider_h


#include <stdint.h>
#include <stdio.h>

#include "vec3.h"
#include "mat3.h"

#include "utilTypes.h"
#include "transform.h"

#include "colliderAABB.h"
#include "colliderHull.h"


#define COLLIDER_NUM_TYPES 1


typedef uint_least8_t colliderType_t;

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


void colliderInit(collider *const restrict c, const colliderType_t type);
void colliderInstantiate(collider *const restrict c, const collider *const restrict cBase);

return_t colliderLoad(
	collider *const restrict c, FILE *const restrict cFile, vec3 *const restrict centroid, mat3 *const restrict inertia
);
void colliderUpdate(
	collider *const restrict c, const vec3 *const restrict centroid,
	const collider *const restrict cBase, const vec3 *const restrict baseCentroid,
	const transformState *const restrict trans, colliderAABB *const restrict aabb
);

void colliderDeleteInstance(collider *const restrict c);
void colliderDelete(collider *const restrict c);


extern void (*colliderInstantiateTable[COLLIDER_NUM_TYPES])(
	void *const restrict c, const void *const restrict cBase
);

extern return_t (*colliderLoadTable[COLLIDER_NUM_TYPES])(
	void *const restrict c, FILE *const restrict cFile,
	vec3 *const restrict centroid, mat3 *const restrict inertia
);
extern void (*colliderUpdateTable[COLLIDER_NUM_TYPES])(
	void *const restrict c, const vec3 *const restrict centroid,
	const void *const restrict cBase, const vec3 *const restrict baseCentroid,
	const transformState *const restrict trans, colliderAABB *const restrict aabb
);

extern void (*colliderDeleteInstanceTable[COLLIDER_NUM_TYPES])(
	void *const restrict c
);
extern void (*colliderDeleteTable[COLLIDER_NUM_TYPES])(
	void *const restrict c
);


#endif