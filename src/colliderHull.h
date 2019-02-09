#ifndef colliderHull_h
#define colliderHull_h


#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"
#include "vec3.h"
#include "transform.h"


#define COLLIDER_TYPE_HULL 0

#define COLLIDER_HULL_SEPARATION_FACE_A 0
#define COLLIDER_HULL_SEPARATION_FACE_B 1
#define COLLIDER_HULL_SEPARATION_EDGE   2

#define COLLIDER_HULL_INVALID_FEATURE -1


typedef uint_least16_t colliderVertexIndex_t;
typedef uint_least16_t colliderEdgeIndex_t;
typedef uint_least16_t colliderFaceIndex_t;


//Stores the indices of data relevant to the edge.
typedef struct colliderHullEdge {
	//Indices of the edge's vertices.
	colliderVertexIndex_t startVertexIndex;
	colliderVertexIndex_t endVertexIndex;

	//Index of the next edge on this face.
	colliderEdgeIndex_t nextIndex;
	//Index of the normal that this edge's twin face uses.
	colliderEdgeIndex_t twinNextIndex;

	//Index of the face that this edge uses.
	colliderFaceIndex_t faceIndex;
	//Index of the face that this edge's twin uses.
	colliderFaceIndex_t twinFaceIndex;
} colliderHullEdge;

//A face can be represented by any edge on
//its border, so we store it as an index.
typedef colliderFaceIndex_t colliderHullFace;

//Stores the data for a hull in a format
//that we can use for collision detection.
typedef struct colliderHull {
	//These two arrays need to be
	//reallocated for each instance.
	vec3 *vertices;
	vec3 *normals;
	//These three don't.
	colliderHullEdge *edges;
	colliderHullFace *faces;
	//Array storing the mass of each vertex. The sum of each element
	//in this array should add up to the total mass of the collider.
	//If the vertices are not weighted, this will be a NULL pointer.
	float *massArray;

	//We don't need to store the number of
	//normals since we can use "numFaces".
	colliderVertexIndex_t numVertices;
	colliderEdgeIndex_t numEdges;
	colliderFaceIndex_t numFaces;
	//Stores twice the number of edges that the
	//face with the most edges has, as each edge
	//can create at most two vertices. This is
	//mostly useful for memory preallocation.
	colliderFaceIndex_t maxFaceVertices;

	//Hulls are the only colliders that
	//need their centroids for collision.
	vec3 centroid;
} colliderHull;


typedef struct contactSeparation contactSeparation;
typedef struct contactManifold contactManifold;
typedef struct colliderAABB colliderAABB;


void colliderHullInit(colliderHull *hull);
void colliderHullInstantiate(void *hull, const void *base);

void colliderHullLoad(void *hull);
void colliderHullSetupProperties(void *hull);

void colliderHullCalculateInertia(const void *hull, float inertia[6]);
void colliderHullCalculateCentroid(const colliderHull *hull);

void colliderHullUpdate(void *hull, const void *base, const transformState *trans, colliderAABB *aabb);

const vec3 *colliderHullSupport(const colliderHull *hull, const vec3 *dir);

return_t colliderHullSeparated(const void *hullA, const void *hullB, contactSeparation *cs);
return_t colliderHullCollidingSAT(const void *hullA, const void *hullB, contactSeparation *cs, contactManifold *cm);

void colliderHullDeleteInstance(void *hull);
void colliderHullDelete(void *hull);


#endif