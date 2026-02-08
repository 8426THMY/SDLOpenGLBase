#ifndef collider_h
#define collider_h


#include <stdint.h>
#include <stdio.h>

#include "vec3.h"
#include "mat3.h"

#include "utilTypes.h"
#include "transform.h"

#include "colliderAABB.h"
#include "colliderPoint.h"
#include "colliderSphere.h"
#include "colliderCapsule.h"
#include "colliderHull.h"


#define COLLIDER_NUM_TYPES 1

#define COLLIDER_TYPE_POINT   0
#define COLLIDER_TYPE_SPHERE  1
#define COLLIDER_TYPE_CAPSULE 2
#define COLLIDER_TYPE_HULL    3


typedef uint_least8_t colliderType;

typedef struct collider {
	// This should be large enough
	// to store any type of collider.
	union {
		colliderHull hull;
	} data;
	// Stores which type of
	// collider this object is.
	colliderType type;
} collider;


void colliderInit(collider *const restrict c, const colliderType type);
void colliderInstantiate(
	collider *const restrict c,
	const collider *const restrict cBase
);

return_t colliderLoad(
	collider *const restrict c,
	FILE *const restrict cFile,
	vec3 *const restrict centroid,
	mat3 *const restrict inertia
);
void colliderUpdate(
	collider *const restrict c, const vec3 *const restrict centroid,
	const collider *const restrict cBase, const vec3 *const restrict baseCentroid,
	const transform *const restrict trans, colliderAABB *const restrict aabb
);

void colliderDeleteInstance(collider *const restrict c);
void colliderDelete(collider *const restrict c);


#endif