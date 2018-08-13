#ifndef colliderMesh_h
#define colliderMesh_h


#define MAX_CONTACTS_NUM 8


#include <stdio.h>

#include "vec3.h"


/*
//A collider mesh edge only needs to store the index of its
//start vertex, as the next edge will contain its end vertex.
typedef struct colliderMeshEdge {
	size_t startVertexIndex;
	size_t oppositeFaceIndex;
} colliderMeshEdge;

typedef struct colliderMeshFace {
	colliderMeshEdge *edges;
	size_t numEdges;

	vec3 normal;
} colliderMeshFace;

typedef struct colliderMesh {
	vec3 *vertices;
	size_t numVertices;

	colliderMeshFace *faces;
	size_t numFaces;
} colliderMesh;
*/


//Stores the index of a face and its distance
//from the closest point on another object.
typedef struct meshFaceData {
	size_t index;
	float distance;
} meshFaceData;

//Stores the index of an edge pair
//and their distance from each other.
typedef struct meshEdgeData {
	size_t indexA;
	size_t indexB;
	float distance;
} meshEdgeData;

//Stores the collision information
//returned by the Separating Axis Theorem.
typedef struct collisionData {
	meshFaceData mesh1Face;
	meshFaceData mesh2Face;
	meshEdgeData meshEdges;
} collisionData;


//Stores every contact point involved in the collision.
typedef struct contactManifold {
	vec3 points[MAX_CONTACTS_NUM];
	size_t numPoints;
} contactManifold;


//Stores the indices of data relevant to the edge.
typedef struct colliderMeshEdge {
	//Indices of the edge's vertices.
	size_t startVertexIndex;
	size_t endVertexIndex;

	//Index of the next edge on this face.
	size_t nextIndex;
	//Index of the normal that this edge's twin face uses.
	size_t twinNextIndex;

	//Index of the normal that this edge's face uses.
	size_t normalIndex;
	//Index of the normal that this edge's twin uses.
	size_t twinNormalIndex;
} colliderMeshEdge;

//Stores the data for a mesh in a format
//that we can use for collision detection.
typedef struct colliderMesh {
	vec3 *vertices;
	size_t numVertices;

	//Array storing the mass of each vertex. The sum of each element
	//in this array should add up to the total mass of the collider.
	//If the vertices are not weighted, this will be a NULL-pointer.
	float *massArray;

	colliderMeshEdge *edges;
	size_t numEdges;

	vec3 *normals;

	//A face is stored as the index
	//of any edge on its border.
	size_t *faces;
	size_t numFaces;
} colliderMesh;


typedef struct physicsCollider physicsCollider;
void colliderMeshLoad(physicsCollider *collider);
void colliderMeshSetupProperties(physicsCollider *collider);

void colliderMeshCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]);
void colliderMeshCalculateCentroid(const colliderMesh *mesh, vec3 *centroid);

size_t colliderMeshSupport(const colliderMesh *mesh, const vec3 *dir);

unsigned char colliderMeshCollidingSAT(const colliderMesh *mesh1, const colliderMesh *mesh2, const vec3 *m1Centroid, collisionData *cd);
void colliderMeshGenerateContactManifoldSHC(const colliderMesh *mesh1, const colliderMesh *mesh2, const collisionData *cd, contactManifold *cm);


#endif