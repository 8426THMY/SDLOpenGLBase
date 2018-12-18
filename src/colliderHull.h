#ifndef colliderHull_h
#define colliderHull_h


#include <stdio.h>
#include <stdint.h>

#include "utilTypes.h"

#include "vec3.h"

#include "collision.h"


/*
//A collider hull edge only needs to store the index of its
//start vertex, as the next edge will contain its end vertex.
typedef struct colliderHullEdge {
	size_t startVertexIndex;
	size_t oppositeFaceIndex;
} colliderHullEdge;

typedef struct colliderHullFace {
	colliderHullEdge *edges;
	size_t numEdges;

	vec3 normal;
} colliderHullFace;

typedef struct colliderHull {
	vec3 *vertices;
	size_t numVertices;

	colliderHullFace *faces;
	size_t numFaces;
} colliderHull;
*/


//Stores the indices of data relevant to the edge.
typedef struct colliderHullEdge {
	//Indices of the edge's vertices.
	uint_least16_t startVertexIndex;
	uint_least16_t endVertexIndex;

	//Index of the next edge on this face.
	uint_least16_t nextIndex;
	//Index of the normal that this edge's twin face uses.
	uint_least16_t twinNextIndex;

	//Index of the face that this edge uses.
	uint_least16_t faceIndex;
	//Index of the face that this edge's twin uses.
	uint_least16_t twinFaceIndex;
} colliderHullEdge;

//A face can be represented by any edge on
//its border, so we store it as an index.
typedef uint_least16_t colliderHullFace;

//Stores the data for a hull in a format
//that we can use for collision detection.
typedef struct colliderHull {
	vec3 *vertices;
	colliderHullEdge *edges;
	//We don't need to store the number of
	//normals since we can use "numFaces".
	vec3 *normals;
	colliderHullFace *faces;
	//Array storing the mass of each vertex. The sum of each element
	//in this array should add up to the total mass of the collider.
	//If the vertices are not weighted, this will be a NULL pointer.
	float *massArray;

	uint_least16_t numVertices;
	uint_least16_t numEdges;
	uint_least16_t numFaces;
	//Stores twice the number of edges that the
	//face with the most edges has, as each edge
	//can create at most two vertices. This is
	//mostly useful for memory preallocation.
	uint_least16_t maxFaceVertices;
} colliderHull;


typedef struct physicsCollider physicsCollider;
void colliderHullLoad(physicsCollider *collider);
void colliderHullSetupProperties(physicsCollider *collider);

void colliderHullCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]);
void colliderHullCalculateCentroid(const colliderHull *hull, vec3 *centroid);

const vec3 *colliderHullSupport(const colliderHull *hull, const vec3 *dir);

return_t colliderHullCollidingSAT(const colliderHull *hull1, const colliderHull *hull2, const vec3 *h1Centroid, contactManifold *cm);


#endif