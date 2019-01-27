#ifndef colliderHull_h
#define colliderHull_h


#define COLLIDER_TYPE_HULL 0

#define COLLIDER_HULL_SEPARATION_FACE_A 0
#define COLLIDER_HULL_SEPARATION_FACE_B 1
#define COLLIDER_HULL_SEPARATION_EDGE   2


#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"
#include "vec3.h"

#include "contact.h"


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

	//Hulls are the only colliders that
	//need their centroids for collision.
	vec3 centroid;
} colliderHull;


void colliderHullLoad(void *hull);
void colliderHullSetupProperties(void *hull);

void colliderHullCalculateInertia(const void *hull, float inertia[6]);
void colliderHullCalculateCentroid(const colliderHull *hull);

const vec3 *colliderHullSupport(const colliderHull *hull, const vec3 *dir);
const vec3 *colliderHullSupportIndex(const colliderHull *hull, const vec3 *dir, uint_least16_t *index);

return_t colliderHullNotSeparated(const void *hullA, const void *hullB, contactSeparation *cs);
return_t colliderHullCollidingSAT(const void *hullA, const void *hullB, contactSeparation *cs, contactManifold *cm);

void colliderHullDelete(colliderHull *hull);


#endif