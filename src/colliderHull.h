#ifndef colliderHull_h
#define colliderHull_h


#include <stdio.h>
#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"
#include "vec3.h"
#include "mat3.h"
#include "transform.h"


#define COLLIDER_TYPE_HULL 3

#define COLLIDER_HULL_SEPARATION_FACE_A 0
#define COLLIDER_HULL_SEPARATION_FACE_B 1
#define COLLIDER_HULL_SEPARATION_EDGE   2

#define COLLIDER_HULL_INVALID_FEATURE -1


#ifndef COLLIDER_HULL_DEFAULT_VERTEX_WEIGHT
	#define COLLIDER_HULL_DEFAULT_VERTEX_WEIGHT 1.f
#endif


typedef uint_least16_t colliderVertexIndex_t;
typedef uint_least16_t colliderEdgeIndex_t;
typedef uint_least16_t colliderFaceIndex_t;


// Stores the indices of data relevant to the edge.
typedef struct colliderHullEdge {
	// Indices of the edge's vertices.
	colliderVertexIndex_t startVertexIndex;
	colliderVertexIndex_t endVertexIndex;

	// Index of the next edge on this face.
	colliderEdgeIndex_t nextIndex;
	// Index of the next edge on this face's twin.
	colliderEdgeIndex_t twinNextIndex;

	// Index of the normal or face that this edge uses.
	colliderFaceIndex_t faceIndex;
	// Index of the normal or face that this edge's twin uses.
	colliderFaceIndex_t twinFaceIndex;
} colliderHullEdge;

// A face can be represented by any edge on
// its border, so we store it as an index.
typedef colliderFaceIndex_t colliderHullFace;

// Stores the data for a hull in a format
// that we can use for collision detection.
typedef struct colliderHull {
	// These two arrays need to be
	// reallocated for each instance.
	vec3 *vertices;
	vec3 *normals;
	// These two don't.
	colliderHullFace *faces;
	colliderHullEdge *edges;

	// We don't need to store the number of
	// normals since we can use "numFaces".
	colliderVertexIndex_t numVertices;
	colliderFaceIndex_t numFaces;
	colliderEdgeIndex_t numEdges;
	// Stores twice the number of edges that the
	// face with the most edges has, as each edge
	// can create at most two vertices when we're
	// clipping. This is mostly useful for memory
	// preallocation during collision detection.
	colliderFaceIndex_t maxFaceEdges;

	// Hulls are the only colliders that
	// need their centroids for collision.
	vec3 centroid;
} colliderHull;


typedef struct contactSeparation contactSeparation;
typedef struct contactManifold contactManifold;
typedef struct colliderAABB colliderAABB;


void colliderHullInit(colliderHull *hull);
void colliderHullInstantiate(void *const restrict hull, const void *const restrict base);

return_t colliderHullLoad(
	void *const restrict hull, FILE *const restrict hullFile,
	vec3 *const restrict centroid, mat3 *const restrict inertia
);

void colliderHullGenerateCentroid(const colliderHull *const restrict hull, vec3 *const restrict centroid);
void colliderHullGenerateCentroidWeighted(
	const colliderHull *const restrict hull, const float *restrict vertexWeights, vec3 *const restrict centroid
);
void colliderHullGenerateInertia(const colliderHull *const restrict hull, mat3 *const restrict inertia);
void colliderHullGenerateInertiaWeighted(
	const colliderHull *const restrict hull, const float *restrict vertexWeights, mat3 *const restrict inertia
);

void colliderHullUpdate(
	void *const restrict hull, const vec3 *const restrict hullCentroid,
	const void *const restrict base, const vec3 *const restrict baseCentroid,
	const transformState *const restrict trans, colliderAABB *const restrict aabb
);

const vec3 *colliderHullSupport(const colliderHull *const restrict hull, const vec3 *const restrict dir);

return_t colliderHullSeparated(
	const void *const restrict hullA, const void *const restrict hullB, contactSeparation *const restrict cs
);
return_t colliderHullCollidingSAT(
	const void *const restrict hullA, const void *const restrict hullB,
	contactSeparation *const restrict separation, contactManifold *const restrict cm
);

void colliderHullDeleteInstance(void *const restrict hull);
void colliderHullDelete(void *const restrict hull);


#endif