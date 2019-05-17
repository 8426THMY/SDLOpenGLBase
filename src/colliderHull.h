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


#ifndef COLLIDER_HULL_DEFAULT_VERTEX_MASS
	#define COLLIDER_HULL_DEFAULT_VERTEX_MASS 1.f
#endif


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
	//These two don't.
	colliderHullFace *faces;
	colliderHullEdge *edges;

	//We don't need to store the number of
	//normals since we can use "numFaces".
	colliderVertexIndex_t numVertices;
	colliderFaceIndex_t numFaces;
	colliderEdgeIndex_t numEdges;
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

return_t colliderHullLoad(void *hull, FILE *hullFile);

void colliderHullGenerateCentroid(void *hull, vec3 *centroid);
void colliderHullGenerateCentroidWeighted(void *hull, const float *vertexMasses, vec3 *centroid);
void colliderHullGenerateMassAndCentroid(void *hull, float *mass, float *invMass, vec3 *centroid);
void colliderHullGenerateMassAndCentroidWeighted(void *hull, const float *vertexMasses, float *mass, float *invMass, vec3 *centroid);
void colliderHullGenerateCentroidAccurate(void *hull, vec3 *centroid);
void colliderHullGenerateCentroidAccurateWeighted(void *hull, const float *vertexMasses, vec3 *centroid);
void colliderHullGenerateInertia(const void *hull, const vec3 *centroid, mat3 *inertia);
void colliderHullGenerateInertiaWeighted(const void *hull, const vec3 *centroid, const float *vertexMasses, mat3 *inertia);

void colliderHullUpdate(void *hull, const void *base, const transformState *trans, colliderAABB *aabb);

const vec3 *colliderHullSupport(const colliderHull *hull, const vec3 *dir);

return_t colliderHullSeparated(const void *hullA, const void *hullB, contactSeparation *cs);
return_t colliderHullCollidingSAT(const void *hullA, const void *hullB, contactSeparation *cs, contactManifold *cm);

void colliderHullDeleteInstance(void *hull);
void colliderHullDelete(void *hull);


#endif