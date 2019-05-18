#include "colliderHull.h"


#include <stddef.h>
#include <math.h>
#include <string.h>

#include "utilMath.h"
#include "utilString.h"
#include "memoryManager.h"

#include "colliderAABB.h"
#include "physicsCollider.h"

#include "contact.h"
#include "physicsContact.h"


#define COLLIDER_HULL_INSTANCE_STATIC_BYTES (offsetof(colliderHull, centroid) - offsetof(colliderHull, edges))


#define COLLISION_PARALLEL_THRESHOLD    0.005f
#define COLLISION_TOLERANCE_COEFFICIENT 0.95f
#ifdef PHYSCONTACT_PENETRATION_SLOP
	#define COLLISION_TOLERANCE_TERM    (0.5f * PHYSCONTACT_PENETRATION_SLOP)
#else
	#define COLLISION_TOLERANCE_TERM    0.25f
#endif

#define CLIPPING_INORDER 0
//We can use byte offsets or a conditional check to swap our keys around.
//It shouldn't matter which of these we use if the function gets inlined.
//If it doesn't, however, I'm not exactly sure which would be faster.
#ifdef CLIPPING_KEYSWAP_USE_OFFSETS

	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS

	#define CLIPPING_SWAPPED (offsetof(contactKey, edgeB) - offsetof(contactKey, edgeA))

	#define clippingGetKeyEdgeA(key, offset) *((colliderEdgeIndex_t *)(((byte_t *)&((key).edgeA)) + (offset)))
	#define clippingGetKeyEdgeB(key, offset) *((colliderEdgeIndex_t *)(((byte_t *)&((key).edgeA)) - (offset)))

	#else

	#define CLIPPING_SWAPPED (offsetof(contactKey, inEdgeB) - offsetof(contactKey, inEdgeA))

	#define clippingGetKeyInEdgeA(key, offset)  *((colliderEdgeIndex_t *)(((byte_t *)&((key).inEdgeA)) + (offset)))
	#define clippingGetKeyOutEdgeA(key, offset) *((colliderEdgeIndex_t *)(((byte_t *)&((key).outEdgeA)) + (offset)))
	#define clippingGetKeyInEdgeB(key, offset)  *((colliderEdgeIndex_t *)(((byte_t *)&((key).inEdgeB)) - (offset)))
	#define clippingGetKeyOutEdgeB(key, offset) *((colliderEdgeIndex_t *)(((byte_t *)&((key).outEdgeB)) - (offset)))

	#endif

#else

#define CLIPPING_SWAPPED 1

#endif


//These must be at least 1!
#define BASE_VERTEX_CAPACITY 1
#define BASE_FACE_CAPACITY   1
#define BASE_EDGE_CAPACITY   1


//Stores the index of a face and its distance
//from the closest point on another object.
typedef struct hullFaceData {
	colliderHullFace index;
	float separation;
} hullFaceData;

//Stores the index of an edge pair
//and their distance from each other.
typedef struct hullEdgeData {
	colliderEdgeIndex_t edgeA;
	colliderEdgeIndex_t edgeB;
	float separation;
} hullEdgeData;

//Stores the collision information
//returned by the Separating Axis Theorem.
typedef struct collisionData {
	hullFaceData faceA;
	hullFaceData faceB;
	hullEdgeData edgeData;
} collisionData;


//Clipped vertices need to store the
//indices of the edges that formed them.
typedef struct vertexClip {
	vec3 v;
	contactKey key;
} vertexClip;

//When we project vertices onto the
//reference face, we store their separation.
typedef struct vertexProject {
	vec3 v;
	float dist;
} vertexProject;


//Forward-declare any helper functions!
static void hullFaceDataInit(hullFaceData *faceData);
static void hullEdgeDataInit(hullEdgeData *edgeData);
static void collisionDataInit(collisionData *cd);

static return_t faceSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs);
static return_t edgeSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs);

static void clipManifoldSHC(const colliderHull *hullA, const colliderHull *hullB, const collisionData *cd, contactManifold *cm);
static return_t isMinkowskiFace(const colliderHull *hullA, const colliderHull *hullB,
                                const colliderHullEdge *eA, const vec3 *invEdgeA,
                                const colliderHullEdge *eB, const vec3 *invEdgeB);
static float edgeDistSquared(const vec3 *pA, const vec3 *edgeDirA,
                             const vec3 *pB, const vec3 *edgeDirB,
                             const vec3 *centroid);

static return_t noSeparatingFace(const colliderHull *hullA, const colliderHull *hullB, hullFaceData *faceData,
                                 contactSeparation *separation, const separationType_t separationType);
static return_t noSeparatingEdge(const colliderHull *hullA, const colliderHull *hullB,
                                 hullEdgeData *edgeData, contactSeparation *separation);

static colliderFaceIndex_t findIncidentFace(const colliderHull *hull, const vec3 *refNormal);
static void reduceContacts(const vertexProject *vProj, const vertexProject *vLast, const vertexClip *vClip, contactManifold *cm);
static void clipFaceContact(const colliderHull *hullA, const colliderHull *hullB, const colliderFaceIndex_t refIndex,
#ifdef CLIPPING_KEYSWAP_USE_OFFSETS
                            const size_t offset, contactManifold *cm);
#else
                            const byte_t swapped, contactManifold *cm);
#endif
static void clipEdgeContact(const colliderHull *hullA, const colliderHull *hullB,
                            const hullEdgeData *edgeData, contactManifold *cm);


#warning "What if we aren't using the global memory manager?"


void colliderHullInit(colliderHull *hull){
	//Clear the hull's data so it's ready for use.
	memset(hull, 0, sizeof(colliderHull));
}

void colliderHullInstantiate(void *hull, const void *base){
	//Allocate memory for the instance's arrays.
	((colliderHull *)hull)->vertices = memoryManagerGlobalAlloc(
		sizeof(*(((colliderHull *)hull)->vertices)) * ((colliderHull *)base)->numVertices
	);
	if(((colliderHull *)hull)->vertices == NULL){
		/** MALLOC FAILED **/
	}
	((colliderHull *)hull)->normals = memoryManagerGlobalAlloc(
		sizeof(*(((colliderHull *)hull)->normals)) * ((colliderHull *)base)->numFaces
	);
	if(((colliderHull *)hull)->normals == NULL){
		/** MALLOC FAILED **/
	}

	//Copy the rest of the components over from the base collider.
	memcpy(((colliderHull *)hull)->edges, ((colliderHull *)base)->edges, COLLIDER_HULL_INSTANCE_STATIC_BYTES);
}


/*
** Load the collider from a file. This assumes that
** neither "hull" nor "hullFile" are NULL. Also, this
** function MAY NOT close "hullFile" when it is done.
*/
return_t colliderHullLoad(void *hull, FILE *hullFile){
	return_t success = 1;


	char lineBuffer[1024];
	char *line;
	size_t lineLength;

	//Used when reading vertex data.
	char *tokPos;

	float totalWeighting = 0.f;
	colliderVertexIndex_t tempVerticesCapacity = BASE_VERTEX_CAPACITY;
	colliderFaceIndex_t tempFacesCapacity = BASE_FACE_CAPACITY;
	colliderEdgeIndex_t tempEdgesCapacity = BASE_EDGE_CAPACITY;

	colliderHull tempHull;
	colliderHullInit(&tempHull);

	//Set up the collider's data arrays!
	tempHull.vertices = memoryManagerGlobalAlloc(BASE_VERTEX_CAPACITY * sizeof(*tempHull.vertices));
	tempHull.normals = memoryManagerGlobalAlloc(BASE_FACE_CAPACITY * sizeof(*tempHull.normals));
	tempHull.edges = memoryManagerGlobalAlloc(BASE_EDGE_CAPACITY * sizeof(*tempHull.edges));
	tempHull.faces = memoryManagerGlobalAlloc(BASE_FACE_CAPACITY * sizeof(*tempHull.faces));

	while(success && (line = readLineFile(hullFile, &lineBuffer[0], &lineLength)) != NULL){
		//Hull vertex.
		if(memcmp(line, "v ", 2) == 0){
			//If we're out of space, allocate some more!
			if(tempHull.numVertices >= tempVerticesCapacity){
				tempVerticesCapacity = tempHull.numVertices * 2;
				tempHull.vertices = memoryManagerGlobalRealloc(tempHull.vertices, tempVerticesCapacity * sizeof(*tempHull.vertices));
				if(tempHull.vertices == NULL){
					/** REALLOC FAILED **/
				}
			}

			//Read the vertex positions from the line!
			tokPos = &line[2];
			tempHull.vertices[tempHull.numVertices].x = strtod(tokPos, &tokPos);
			tempHull.vertices[tempHull.numVertices].y = strtod(tokPos, &tokPos);
			tempHull.vertices[tempHull.numVertices].z = strtod(tokPos, &tokPos);

			//If everything was successful, make sure we don't overwrite them next time!
			if(*tokPos != '\0'){
				++tempHull.numVertices;
			}

		//Hull face.
		}else if(memcmp(line, "f ", 2) == 0){
			//

		//Hull end.
		}else if(line[0] == '}'){
			break;
		}
	}


	return(success);
}


/*
** Calculate the collider's centre of gravity. To
** do this, we calculate the centroid of each of the
** hull's triangles and then determine the average.
*/
void colliderHullGenerateCentroid(void *hull, vec3 *centroid){
	const colliderVertexIndex_t numVertices = ((colliderHull *)hull)->numVertices;

	//We don't want to divide by zero.
	if(numVertices > 0){
		const vec3 *curVertex = ((colliderHull *)hull)->vertices;
		const vec3 *lastVertex = &curVertex[numVertices];
		vec3 newCentroid;
		vec3InitZero(&newCentroid);

		//Add each vertex's contribution to the centroid.
		do {
			vec3AddVec3(&newCentroid, curVertex);
			++curVertex;
		} while(curVertex < lastVertex);

		vec3DivideBySOut(&newCentroid, numVertices, centroid);
		((colliderHull *)hull)->centroid = *centroid;
	}else{
		memset(centroid, 0.f, sizeof(*centroid));
		memset(&((colliderHull *)hull)->centroid, 0.f, sizeof(((colliderHull *)hull)->centroid));
	}
}

/*
** Calculate the weighted collider's centre of gravity.
** To do this, we calculate the centroid of each of the
** hull's triangles and then determine the average.
*/
void colliderHullGenerateCentroidWeighted(void *hull, const float *vertexMasses, vec3 *centroid){
	const colliderVertexIndex_t numVertices = ((colliderHull *)hull)->numVertices;
	const vec3 *curVertex = ((colliderHull *)hull)->vertices;
	const vec3 *lastVertex = &curVertex[numVertices];
	vec3 newCentroid;
	float newMass;

	vec3InitZero(&newCentroid);

	//Add each vertex's contribution to the centroid.
	for(; curVertex < lastVertex; ++curVertex, ++vertexMasses){
		vec3 weightedVertex;
		const float massValue = *vertexMasses;

		vec3MultiplySOut(curVertex, massValue, &weightedVertex);
		vec3AddVec3(&newCentroid, &weightedVertex);
		newMass += massValue;
	}

	//We don't want to divide by zero.
	if(newMass != 0.f){
		vec3DivideBySOut(&newCentroid, newMass, centroid);
		((colliderHull *)hull)->centroid = *centroid;
	}else{
		memset(centroid, 0.f, sizeof(*centroid));
		memset(&((colliderHull *)hull)->centroid, 0.f, sizeof(((colliderHull *)hull)->centroid));
	}
}

/*
** Calculate the collider's centre of gravity. To
** do this, we calculate the centroid of each of the
** hull's triangles and then determine the average.
** This function will also return the hull's mass.
*/
void colliderHullGenerateMassAndCentroid(void *hull, float *mass, float *invMass, vec3 *centroid){
	const colliderVertexIndex_t numVertices = ((colliderHull *)hull)->numVertices;

	//We don't want to divide by zero.
	if(numVertices > 0){
		const vec3 *curVertex = ((colliderHull *)hull)->vertices;
		const vec3 *lastVertex = &curVertex[numVertices];
		vec3 newCentroid;
		float newMass;

		vec3InitZero(&newCentroid);

		//Add each vertex's contribution to the centroid.
		for(; curVertex < lastVertex; ++curVertex){
			vec3AddVec3(&newCentroid, curVertex);
		}

		//Use the number of vertices to find the total mass.
		newMass = COLLIDER_HULL_DEFAULT_VERTEX_MASS * numVertices;
		*mass = newMass;
		*invMass = 1.f / newMass;

		vec3DivideBySOut(&newCentroid, numVertices, centroid);
		((colliderHull *)hull)->centroid = *centroid;
	}else{
		*mass = 0.f;
		*invMass = 0.f;

		memset(centroid, 0.f, sizeof(*centroid));
		memset(&((colliderHull *)hull)->centroid, 0.f, sizeof(((colliderHull *)hull)->centroid));
	}
}

/*
** Calculate the weighted collider's centre of gravity.
** To do this, we calculate the centroid of each of the
** hull's triangles and then determine the average.
** This function will also return the hull's mass.
*/
void colliderHullGenerateMassAndCentroidWeighted(void *hull, const float *vertexMasses, float *mass, float *invMass, vec3 *centroid){
	const colliderVertexIndex_t numVertices = ((colliderHull *)hull)->numVertices;
	const vec3 *curVertex = ((colliderHull *)hull)->vertices;
	const vec3 *lastVertex = &curVertex[numVertices];
	vec3 newCentroid;
	float newMass;

	vec3InitZero(&newCentroid);

	//Add each vertex's contribution to the centroid.
	for(; curVertex < lastVertex; ++curVertex, ++vertexMasses){
		vec3 weightedVertex;
		const float massValue = *vertexMasses;

		vec3MultiplySOut(curVertex, massValue, &weightedVertex);
		vec3AddVec3(&newCentroid, &weightedVertex);
		newMass += massValue;
	}

	*mass = newMass;
	//We don't want to divide by zero.
	if(newMass != 0.f){
		*invMass = 1.f / newMass;

		vec3MultiplySOut(&newCentroid, *invMass, centroid);
		((colliderHull *)hull)->centroid = *centroid;
	}else{
		*invMass = 0.f;

		memset(centroid, 0.f, sizeof(*centroid));
		memset(&((colliderHull *)hull)->centroid, 0.f, sizeof(((colliderHull *)hull)->centroid));
	}
}

/*
** Calculate the collider's centre of gravity. To
** do this, we calculate the centroid of each of the
** hull's triangles and then determine the average.
** While this doesn't generate the collider's mass, it
** should give a more accurate estimate of its centroid.
*/
void colliderHullGenerateCentroidAccurate(void *hull, vec3 *centroid){
	const colliderFaceIndex_t numFaces = ((colliderHull *)hull)->numFaces;

	//We don't want to divide by zero.
	if(numFaces > 0){
		const vec3 *hullVertices = ((colliderHull *)hull)->vertices;
		const colliderHullEdge *hullEdges = ((colliderHull *)hull)->edges;

		const colliderHullFace *curFace = ((colliderHull *)hull)->faces;
		const colliderHullFace *lastFace = &curFace[numFaces];

		vec3 newCentroid;
		vec3InitZero(&newCentroid);

		do {
			const colliderHullEdge *curEdge = &hullEdges[*curFace];
			const colliderHullEdge *startEdge = curEdge;
			vec3 faceCentroid;

			vec3InitZero(&faceCentroid);

			//Calculate the current face's
			//contribution to the centroid!
			do {
				//If this edge forms the current
				//face, get the start vertex!
				if(*curFace == curEdge->faceIndex){
					vec3AddVec3(&faceCentroid, &hullVertices[curEdge->startVertexIndex]);
					curEdge = &hullEdges[curEdge->nextIndex];

				//If this edge's twin forms the current
				//face, we should use the end vertex!
				}else{
					vec3AddVec3(&faceCentroid, &hullVertices[curEdge->endVertexIndex]);
					curEdge = &hullEdges[curEdge->twinNextIndex];
				}
			} while(curEdge != startEdge);

			//Add this face's contribution
			//to the collider's centroid!
			vec3MultiplyS(&faceCentroid, 1.f / 3.f);
			vec3AddVec3(&newCentroid, &faceCentroid);
		} while(curFace < lastFace);

		vec3DivideBySOut(&newCentroid, numFaces * 3.f, centroid);
		((colliderHull *)hull)->centroid = *centroid;
	}else{
		memset(centroid, 0.f, sizeof(*centroid));
		memset(&((colliderHull *)hull)->centroid, 0.f, sizeof(((colliderHull *)hull)->centroid));
	}
}

/*
** Calculate the collider's weighted centre of gravity.
** To do this, we calculate the centroid of each of the
** hull's triangles and then determine the average.
** While this doesn't generate the collider's mass, it
** should give a more accurate estimate of its centroid.
*/
void colliderHullGenerateCentroidAccurateWeighted(void *hull, const float *vertexMasses, vec3 *centroid){
	const vec3 *hullVertices = ((colliderHull *)hull)->vertices;
	const colliderHullEdge *hullEdges = ((colliderHull *)hull)->edges;

	const colliderHullFace *curFace = ((colliderHull *)hull)->faces;
	const colliderHullFace *lastFace = &curFace[((colliderHull *)hull)->numFaces];

	vec3 newCentroid;
	float newMass = 0.f;

	vec3InitZero(&newCentroid);

	for(; curFace < lastFace; ++curFace){
		const colliderHullEdge *curEdge = &hullEdges[*curFace];
		const colliderHullEdge *startEdge = curEdge;
		vec3 faceCentroid;
		float faceMass = 0.f;

		vec3InitZero(&faceCentroid);

		//Calculate the current face's
		//contribution to the centroid!
		do {
			vec3 curVertex;
			float massValue;

			//If this edge forms the current
			//face, get the start vertex!
			if(*curFace == curEdge->faceIndex){
				curVertex = hullVertices[curEdge->startVertexIndex];
				massValue = vertexMasses[curEdge->startVertexIndex];
				curEdge = &hullEdges[curEdge->nextIndex];

			//If this edge's twin forms the current
			//face, we should use the end vertex!
			}else{
				curVertex = hullVertices[curEdge->endVertexIndex];
				massValue = vertexMasses[curEdge->endVertexIndex];
				curEdge = &hullEdges[curEdge->twinNextIndex];
			}

			//Add this vertex's contribution
			//to the current face's centroid!
			vec3MultiplyS(&curVertex, massValue);
			vec3AddVec3(&faceCentroid, &curVertex);
			faceMass += massValue;
		} while(curEdge != startEdge);

		//Add this face's contribution
		//to the collider's centroid!
		vec3DivideByS(&faceCentroid, faceMass);
		vec3AddVec3(&newCentroid, &faceCentroid);
		newMass += faceMass;
	}

	//We don't want to divide by zero.
	if(newMass != 0.f){
		vec3DivideBySOut(&newCentroid, newMass, centroid);
		((colliderHull *)hull)->centroid = *centroid;
	}else{
		memset(centroid, 0.f, sizeof(*centroid));
		memset(&((colliderHull *)hull)->centroid, 0.f, sizeof(((colliderHull *)hull)->centroid));
	}
}

//Calculate the collider's inertia tensor relative to "centroid".
void colliderHullGenerateInertia(const void *hull, const vec3 *centroid, mat3 *inertia){
	const vec3 *curVertex = ((colliderHull *)hull)->vertices;
	const vec3 *lastVertex = &curVertex[((colliderHull *)hull)->numVertices];
	float tempInertia[6];

	memset(tempInertia, 0.f, sizeof(tempInertia));

	for(; curVertex < lastVertex; ++curVertex){
		vec3 offset;
		float xx;
		float yy;
		float zz;

		vec3SubtractVec3FromOut(curVertex, centroid, &offset);
		xx = offset.x * offset.x;
		yy = offset.y * offset.y;
		zz = offset.z * offset.z;

		tempInertia[0] += yy + zz;
		tempInertia[1] += xx + zz;
		tempInertia[2] += xx + yy;
		tempInertia[3] -= offset.x * offset.y;
		tempInertia[4] -= offset.x * offset.z;
		tempInertia[5] -= offset.y * offset.z;
	}

	inertia->m[0][0] = tempInertia[0];
	inertia->m[1][1] = tempInertia[1];
	inertia->m[2][2] = tempInertia[2];
	inertia->m[0][1] = tempInertia[3];
	inertia->m[0][2] = tempInertia[4];
	inertia->m[1][2] = tempInertia[5];
	//These should be the same as the values we've already calculated.
	inertia->m[1][0] = tempInertia[3];
	inertia->m[2][0] = tempInertia[4];
	inertia->m[2][1] = tempInertia[5];
}

/*
** Calculate the collider's weighted inertia tensor
** relative to "centroid". Vertices with a greater mass
** will have a greater contribution to the final tensor.
*/
void colliderHullGenerateInertiaWeighted(const void *hull, const vec3 *centroid, const float *vertexMasses, mat3 *inertia){
	const vec3 *curVertex = ((colliderHull *)hull)->vertices;
	const vec3 *lastVertex = &curVertex[((colliderHull *)hull)->numVertices];
	float tempInertia[6];

	memset(tempInertia, 0.f, sizeof(tempInertia));

	for(; curVertex < lastVertex; ++curVertex, ++vertexMasses){
		vec3 offset;
		float xx;
		float yy;
		float zz;
		const float massValue = *vertexMasses;

		vec3SubtractVec3FromOut(curVertex, centroid, &offset);
		xx = offset.x * offset.x;
		yy = offset.y * offset.y;
		zz = offset.z * offset.z;

		tempInertia[0] += (yy + zz) * massValue;
		tempInertia[1] += (xx + zz) * massValue;
		tempInertia[2] += (xx + yy) * massValue;
		tempInertia[3] -= offset.x * offset.y * massValue;
		tempInertia[4] -= offset.x * offset.z * massValue;
		tempInertia[5] -= offset.y * offset.z * massValue;
	}

	inertia->m[0][0] = tempInertia[0];
	inertia->m[1][1] = tempInertia[1];
	inertia->m[2][2] = tempInertia[2];
	inertia->m[0][1] = tempInertia[3];
	inertia->m[0][2] = tempInertia[4];
	inertia->m[1][2] = tempInertia[5];
	//These should be the same as the values we've already calculated.
	inertia->m[1][0] = tempInertia[3];
	inertia->m[2][0] = tempInertia[4];
	inertia->m[2][1] = tempInertia[5];
}


/*
** Transform a hull instance's vertices according to the transformation
** information supplied and, in doing so, generate a new bounding box.
*/
void colliderHullUpdate(void *hull, const void *base, const transformState *trans, colliderAABB *aabb){
	vec3 *curFeature = ((colliderHull *)hull)->vertices;
	const vec3 *baseFeature = ((colliderHull *)base)->vertices;
	const vec3 *lastFeature = &curFeature[((colliderHull *)hull)->numVertices];

	//Find the collider's new centroid!
	transformStateTransformVec3(trans, &(((colliderHull *)base)->centroid), &(((colliderHull *)hull)->centroid));

	//We can only update the hull's
	//vertices if it actually has any.
	if(curFeature < lastFeature){
		colliderAABB tempAABB;

		//Transform the first vertex and use it for the bounding box.
		transformStateTransformVec3(trans, baseFeature, curFeature);
		tempAABB.min = tempAABB.max = *curFeature;

		//Transform the remaining vertices!
		for(; curFeature < lastFeature;){
			transformStateTransformVec3(trans, ++baseFeature, ++curFeature);

			//If this vertex exceeds the bounds of
			//our current bounding box, we should
			//update the bounding box to contain it.
			//Left and right.
			if(curFeature->x < tempAABB.min.x){
				tempAABB.min.x = curFeature->x;
			}else if(curFeature->x > tempAABB.max.x){
				tempAABB.max.x = curFeature->x;
			}
			//Up and down.
			if(curFeature->y < tempAABB.min.y){
				tempAABB.min.y = curFeature->y;
			}else if(curFeature->y > tempAABB.max.y){
				tempAABB.max.y = curFeature->y;
			}
			//Front and back.
			if(curFeature->z < tempAABB.min.z){
				tempAABB.min.z = curFeature->z;
			}else if(curFeature->z > tempAABB.max.z){
				tempAABB.max.z = curFeature->z;
			}
		}


		curFeature = ((colliderHull *)hull)->normals;
		baseFeature = ((colliderHull *)base)->normals;
		lastFeature = &curFeature[((colliderHull *)hull)->numFaces];

		//The hull's faces have been rotated,
		//so we need to rotate their normals.
		for(; curFeature < lastFeature; ++curFeature, ++baseFeature){
			quatApplyRotationFast(&trans->rot, baseFeature, curFeature);
		}


		if(aabb != NULL){
			*aabb = tempAABB;
		}

	//If the hull has no vertices but we
	//want a bounding box, initialise it.
	}else if(aabb != NULL){
		memset(aabb, 0.f, sizeof(*aabb));
	}
}


//Return a pointer to the vertex farthest from the origin in the direction "dir".
const vec3 *colliderHullSupport(const colliderHull *hull, const vec3 *dir){
	const vec3 *bestVertex = hull->vertices;
	float bestDistance = vec3DotVec3(bestVertex, dir);

	const vec3 *curVertex = &bestVertex[1];
	const vec3 *lastVertex = &bestVertex[hull->numVertices];

	for(; curVertex < lastVertex; ++curVertex){
		const float curDistance = vec3DotVec3(curVertex, dir);
		//If the current vertex is farther in the direction
		//of "dir" than our current best, store it instead.
		if(curDistance > bestDistance){
			bestVertex = curVertex;
			bestDistance = curDistance;
		}
	}

	return(bestVertex);
}


//Check if a separation still exists between two hulls.
return_t colliderHullSeparated(const void *hullA, const void *hullB, contactSeparation *cs){
	switch(cs->type){
		case COLLIDER_HULL_SEPARATION_FACE_A:
			return(faceSeparation(hullA, hullB, cs));
		break;
		case COLLIDER_HULL_SEPARATION_FACE_B:
			return(faceSeparation(hullB, hullA, cs));
		break;
		default:
			return(edgeSeparation(hullA, hullB, cs));
		break;
	}
}

/*
** Return whether or not two hulls are colliding using
** the Separating Axis Theorem. Special thanks to Dirk
** Gregorius for his GDC 2013 presentation on utilising
** Gauss map and Minkowski space to optimise this!
*/
return_t colliderHullCollidingSAT(const void *hullA, const void *hullB, contactSeparation *separation, contactManifold *cm){
	collisionData cd;
	//We technically don't need to initialise
	//this, but it's always good to be safe.
	collisionDataInit(&cd);

	if(noSeparatingFace((colliderHull *)hullA, (colliderHull *)hullB, &cd.faceA, separation, COLLIDER_HULL_SEPARATION_FACE_A)){
		if(noSeparatingFace((colliderHull *)hullB, (colliderHull *)hullA, &cd.faceB, separation, COLLIDER_HULL_SEPARATION_FACE_B)){
			if(noSeparatingEdge((colliderHull *)hullA, (colliderHull *)hullB, &cd.edgeData, separation)){
				//If there are no separating axes, the hulls are colliding and
				//we can clip the colliding area to create a contact manifold.
				clipManifoldSHC((colliderHull *)hullA, (colliderHull *)hullB, &cd, cm);

				return(1);
			}
		}
	}

	return(0);
}


void colliderHullDeleteInstance(void *hull){
	if(((colliderHull *)hull)->vertices != NULL){
		memoryManagerGlobalFree(((colliderHull *)hull)->vertices);
	}
	if(((colliderHull *)hull)->normals != NULL){
		memoryManagerGlobalFree(((colliderHull *)hull)->normals);
	}
}

void colliderHullDelete(void *hull){
	if(((colliderHull *)hull)->vertices != NULL){
		memoryManagerGlobalFree(((colliderHull *)hull)->vertices);
	}
	if(((colliderHull *)hull)->normals != NULL){
		memoryManagerGlobalFree(((colliderHull *)hull)->normals);
	}
	if(((colliderHull *)hull)->edges != NULL){
		memoryManagerGlobalFree(((colliderHull *)hull)->edges);
	}
	if(((colliderHull *)hull)->faces != NULL){
		memoryManagerGlobalFree(((colliderHull *)hull)->faces);
	}
}


static void hullFaceDataInit(hullFaceData *faceData){
	faceData->index = COLLIDER_HULL_INVALID_FEATURE;
	faceData->separation = -INFINITY;
}

static void hullEdgeDataInit(hullEdgeData *edgeData){
	edgeData->edgeA = COLLIDER_HULL_INVALID_FEATURE;
	edgeData->edgeB = COLLIDER_HULL_INVALID_FEATURE;
	edgeData->separation = -INFINITY;
}

/*
** This is only required if we're trying to check
** collision against an object with no vertices.
*/
static void collisionDataInit(collisionData *cd){
	hullFaceDataInit(&cd->faceA);
	hullFaceDataInit(&cd->faceB);
	hullEdgeDataInit(&cd->edgeData);
}


//Return whether a face separation between two convex hulls still exists.
static return_t faceSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs){
	const vec3 *normal = &hullA->normals[cs->featureA];
	const vec3 invNormal = {
		.x = -normal->x,
		.y = -normal->y,
		.z = -normal->z
	};

	return(
		pointPlaneDist(
			colliderHullSupport(hullB, &invNormal),
			&hullA->vertices[cs->featureA],
			normal
		) > 0.f
	);
}

//Return whether an edge separation between two convex hulls still exists.
static return_t edgeSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs){
	const colliderHullEdge *edgeA = &hullA->edges[cs->featureA];
	const vec3 *startVertexA = &hullA->vertices[edgeA->endVertexIndex];
	vec3 invEdgeA;
	const colliderHullEdge *edgeB = &hullB->edges[cs->featureB];
	const vec3 *startVertexB = &hullB->vertices[edgeB->endVertexIndex];
	vec3 invEdgeB;

	vec3SubtractVec3FromOut(startVertexA, &hullA->vertices[edgeA->endVertexIndex], &invEdgeA);
	vec3SubtractVec3FromOut(startVertexB, &hullB->vertices[edgeB->endVertexIndex], &invEdgeB);

	return(
		!isMinkowskiFace(hullA, hullB, edgeA, &invEdgeA, edgeB, &invEdgeB) ||
		edgeDistSquared(startVertexA, &invEdgeA, startVertexB, &invEdgeB, &hullA->centroid) > 0.f
	);
}


/*
** Using the collision information generated from the
** Separating Axis Theorem, clip the penetrating area
** using the Sutherland-Hodgman clipping algorithm
** to create the contact manifold.
*/
static void clipManifoldSHC(const colliderHull *hullA, const colliderHull *hullB, const collisionData *cd, contactManifold *cm){
	const float bestFaceSeparation = maxNumFast(cd->faceA.separation, cd->faceB.separation);

	//If the greatest separation occurs on
	//an edge normal, clip using the edges.
	if(cd->edgeData.separation > bestFaceSeparation){
		clipEdgeContact(hullA, hullB, &cd->edgeData, cm);

	//Otherwise, clip using whichever face has the
	//greatest separation as the reference face.
	}else{
		if(cd->faceB.separation > COLLISION_TOLERANCE_COEFFICIENT * cd->faceA.separation + COLLISION_TOLERANCE_TERM){
			clipFaceContact(hullB, hullA, cd->faceB.index, CLIPPING_SWAPPED, cm);
			//If the hulls were swapped, we'll
			//need to invert the contact normal.
			vec3Negate(&cm->normal);
		}else{
			clipFaceContact(hullA, hullB, cd->faceA.index, CLIPPING_INORDER, cm);
		}
	}
}


/*
** We know that the face normals of the two shapes'
** Minkowski difference are the only separating axes.
** However, not all edge pairs form a face on the
** Minkowski difference.
**
** This function will project an edge pair onto a
** unit sphere (the Gauss map of the pair) and return
** whether their arcs intersect. This will tell us
** whether they form a face on the Minkowski difference.
*/
static return_t isMinkowskiFace(const colliderHull *hullA, const colliderHull *hullB,
                                const colliderHullEdge *eA, const vec3 *invEdgeA,
                                const colliderHullEdge *eB, const vec3 *invEdgeB){

	//Note that in this function, the following terminology is used:
	//A  - Normal of edge A's face.
	//B  - Normal of edge A's twin face.
	//C  - Normal of edge B's face.
	//D  - Normal of edge B's twin face.
	//BA - Cross product of B and A (normal of their shared edge).
	//DC - Cross product of D and C (normal of their shared edge).

	const float CBA = vec3DotVec3(&hullB->normals[eB->faceIndex], invEdgeA);

	//First, make sure the edge formed by CD are on
	//opposite sides of the plane with normal BA.
	if(CBA * vec3DotVec3(&hullB->normals[eB->twinFaceIndex], invEdgeA) < 0.f){
		const float BDC = vec3DotVec3(&hullA->normals[eA->twinFaceIndex], invEdgeB);

		//Now make sure the edge formed by AB are on
		//opposite sides of the plane with normal DC.
		if(vec3DotVec3(&hullA->normals[eA->faceIndex], invEdgeB) * BDC < 0.f){
			//If the two arcs are on the same hemisphere of the unit
			//sphere, they form a face on the Minkowski difference!
			return(CBA * BDC > 0.f);
		}
	}

	return(0);
}

//Return the squared distance between two edges.
static float edgeDistSquared(const vec3 *pA, const vec3 *edgeDirA,
                             const vec3 *pB, const vec3 *edgeDirB,
                             const vec3 *centroid){

	vec3 edgeNormal;
	vec3 offset;
	float edgeCrossLength;


	vec3CrossVec3Out(edgeDirA, edgeDirB, &edgeNormal);
	//The norm of the vector is the square of its magnitude.
	edgeCrossLength = vec3NormVec3(&edgeNormal);
	//If the two edges are parallel, we can exit early.
	if(edgeCrossLength < COLLISION_PARALLEL_THRESHOLD * (vec3NormVec3(edgeDirA) + vec3NormVec3(edgeDirB))){
		return(-INFINITY);
	}


	vec3MultiplyS(&edgeNormal, fastInvSqrt(edgeCrossLength));
	vec3SubtractVec3FromOut(pA, centroid, &offset);
	//If the edge normal does not point from
	//object A to object B, we need to invert it.
	if(vec3DotVec3(&edgeNormal, &offset) < 0.f){
		vec3Negate(&edgeNormal);
	}


	vec3SubtractVec3FromOut(pB, pA, &offset);

	//Return the distance between the edges.
	return(vec3DotVec3(&edgeNormal, &offset));
}


/*
** For each face on "hullA", check the points on "hullB"
** farthest in the direction opposite the face's normal.
** If the separation is positive, we've found a separating
** axis and the shapes are not colliding. Otherwise, we
** can only assume that they might be colliding.
**
** We store the index of the least separating face in
** the variable "faceData".
*/
static return_t noSeparatingFace(const colliderHull *hullA, const colliderHull *hullB, hullFaceData *faceData,
                                 contactSeparation *separation, const separationType_t separationType){

	const colliderHullFace *curFace = hullA->faces;
	const vec3 *curNormal = hullA->normals;
	const colliderHullFace *lastFace = &hullA->faces[hullA->numFaces];

	colliderFaceIndex_t i;
	for(i = 0; curFace < lastFace; ++i){
		const vec3 invNormal = {
			.x = -curNormal->x,
			.y = -curNormal->y,
			.z = -curNormal->z
		};
		//First, find the point on "hullB" farthest in the
		//direction opposite the current face's normal.
		//Then, find the distance between the two.
		const float curDistance = pointPlaneDist(
			colliderHullSupport(hullB, &invNormal),
			&hullA->vertices[hullA->edges[*curFace].startVertexIndex],
			curNormal
		);

		//If the distance is greater than zero, we can
		//exit early as the hulls aren't colliding.
		if(curDistance > 0.f){
			if(separation != NULL){
				separation->featureA = i;
				separation->type = separationType;
			}

			return(0);

		//If this is the least separating face we've
		//found so far, record its index and distance.
		}else if(curDistance > faceData->separation){
			faceData->index = i;
			faceData->separation = curDistance;
		}

		++curFace;
		++curNormal;
	}

	return(1);
}

/*
** After checking for any separating axes between the faces
** of the two shapes, we must now check between their edges.
** Note that we're able to skip every second edge, as they
** will represent the twins of the previous edges.
**
** Luckily, we do not need to check the distance between
** every edge pair due to the optimisations outlined by
** Dirk Gregorius in his GDC 2013 presentation.
**
** We store the least separating edge pair in "edgeData".
*/
static return_t noSeparatingEdge(const colliderHull *hullA, const colliderHull *hullB,
                                 hullEdgeData *edgeData, contactSeparation *separation){

	colliderHullEdge *edgeA = hullA->edges;
	const colliderHullEdge *lastEdgeA = &edgeA[hullA->numEdges];
	const colliderHullEdge *lastEdgeB = &hullB->edges[hullB->numEdges];
	colliderEdgeIndex_t a;

	//We skip every second edge since
	//it will be the last one's twin.
	for(a = 0; edgeA < lastEdgeA; ++edgeA, a += 2){
		colliderHullEdge *edgeB = hullB->edges;
		const vec3 *startVertexA = &hullA->vertices[edgeA->startVertexIndex];
		vec3 invEdgeA;
		colliderEdgeIndex_t b;

		//Get the inverse of edge 1's normal.
		vec3SubtractVec3FromOut(startVertexA, &hullA->vertices[edgeA->endVertexIndex], &invEdgeA);

		for(b = 0; edgeB < lastEdgeB; ++edgeB, b += 2){
			const vec3 *startVertexB = &hullB->vertices[edgeB->startVertexIndex];
			vec3 invEdgeB;
			//Get the inverse of edge 2's normal
			vec3SubtractVec3FromOut(startVertexB, &hullB->vertices[edgeB->endVertexIndex], &invEdgeB);

			//We only need to compare two edges if they
			//form a face on the Minkowski difference.
			if(isMinkowskiFace(hullA, hullB, edgeA, &invEdgeA, edgeB, &invEdgeB)){
				const float curDistance = edgeDistSquared(startVertexA, &invEdgeA, startVertexB, &invEdgeB, &hullA->centroid);

				//If the distance is less than zero, we can
				//exit early as the hulls aren't colliding.
				if(curDistance > 0.f){
					if(separation != NULL){
						separation->featureA = a;
						separation->featureB = b;
						separation->type = COLLIDER_HULL_SEPARATION_EDGE;
					}

					return(0);

				//If this is the least separating edge pair we've
				//found so far, record the edges and their distance.
				}else if(curDistance > edgeData->separation){
					edgeData->edgeA = a;
					edgeData->edgeB = b;
					edgeData->separation = curDistance;
				}
			}
		}
	}

	return(1);
}


/*
** Using the normal of the reference face on one object,
** find the incident face on another. The incident face
** is defined as the face farthest in the direction
** opposite the reference face's normal.
*/
static colliderFaceIndex_t findIncidentFace(const colliderHull *hull, const vec3 *refNormal){
	const vec3 *curNormal = hull->normals;
	colliderFaceIndex_t bestFace = 0;
	float bestDistance = vec3DotVec3(curNormal, refNormal);
	colliderFaceIndex_t i;

	for(i = 1; i < hull->numFaces; ++i){
		const float curDistance = vec3DotVec3(++curNormal, refNormal);
		//If the current face's normal is farther in the direction
		//of "refNormal" than our current best, store it instead.
		if(curDistance < bestDistance){
			bestFace = i;
			bestDistance = curDistance;
		}
	}

	return(bestFace);
}

/*
** In his lecture "Robust Contact Creation for Physics
** Simulations" (p. 93), Dirk Gregorius suggests only
** keeping the four points that form the polygon with
** the greatest total area.
*/
static void reduceContacts(const vertexProject *vProj, const vertexProject *vLast, const vertexClip *vClip, contactManifold *cm){
	//We start with our best and worst vertices as the
	//first one so we can begin the loop on the second.
	const vertexProject *bestProj = vProj;
	const vertexClip *bestClip = vClip;
	float bestDist = vec3NormVec3(&bestProj->v);
	const vertexProject *worstProj = vProj;
	const vertexClip *worstClip = vClip;
	float worstDist = bestDist;

	const vec3 *firstContact;
	const vec3 *secondContact;
	contactPoint *curContact = cm->contacts;

	vec3 edgeNormal;

	const vertexProject *curProj = &vProj[1];
	const vertexClip *curClip = &vClip[1];
	float curDist;


	//Start by finding the vertices with the greatest
	//positive and negative distances from the origin.
	for(; curProj < vLast; ++curProj, ++curClip){
		curDist = vec3NormVec3(&curProj->v);
		//The first vertex will be the one with the
		//most positive distance from the origin.
		if(curDist > bestDist){
			bestProj = curProj;
			bestClip = curClip;
			bestDist = curDist;

		//The second vertex will be the one with the
		//most negative distance from the origin.
		}else if(curDist < worstDist){
			worstProj = curProj;
			worstClip = curClip;
			worstDist = curDist;
		}
	}

	//We need to remember these pointers so we
	//don't store the same points multiple times.
	firstContact = &bestProj->v;
	secondContact = &worstProj->v;

	//Add the points to our manifold.
	curContact->key = bestClip->key;
	curContact->pA = bestProj->v;
	curContact->pB = bestClip->v;
	curContact->separation = bestProj->dist;
	++curContact;
	curContact->key = worstClip->key;
	curContact->pA = worstProj->v;
	curContact->pB = worstClip->v;
	curContact->separation = worstProj->dist;
	++curContact;


	//The normal of the edge is the cross product of the reference
	//face's normal and the difference between the edge's vertices.
	//This works because every vertex lies on the reference face.
	vec3CrossVec3Float(&cm->normal, worstProj->v.x - bestProj->v.x, worstProj->v.y - bestProj->v.y, worstProj->v.z - bestProj->v.z, &edgeNormal);

	//Again, we start with our best and worst vertices as
	//the first one so we can begin the loop on the second.
	bestProj  = vProj;
	bestClip = vClip;
	bestDist = vec3DotVec3Float(&edgeNormal, firstContact->x - bestProj->v.x, firstContact->y - bestProj->v.y, firstContact->z - bestProj->v.z);
	worstProj = vProj;
	worstClip = vClip;
	worstDist = bestDist;

	curProj = &vProj[1];
	curClip = &vClip[1];

	//Now, we can find the points with the greatest positive
	//and negative distances from the edge we have created.
	for(; curProj < vLast; ++curProj, ++curClip){
		//Make sure this vertex is not already part of the manifold.
		if(&curProj->v != firstContact && &curProj->v != secondContact){
			curDist = vec3DotVec3Float(&edgeNormal, firstContact->x - curProj->v.x, firstContact->y - curProj->v.y, firstContact->z - curProj->v.z);
			//The third vertex will be the one with
			//the most positive distance from our edge.
			if(curDist > bestDist){
				bestProj = curProj;
				bestClip = curClip;
				bestDist = curDist;

			//The fourth vertex will be the one with
			//the most negative distance from our edge.
			}else if(curDist < worstDist){
				worstProj = curProj;
				worstClip = curClip;
				worstDist = curDist;
			}
		}
	}

	//Add the points to our manifold.
	curContact->key = bestClip->key;
	curContact->pA = bestProj->v;
	curContact->pB = bestClip->v;
	curContact->separation = bestProj->dist;
	++curContact;
	curContact->key = worstClip->key;
	curContact->pA = worstProj->v;
	curContact->pB = worstClip->v;
	curContact->separation = worstProj->dist;


	//We should now have only four contact points.
	cm->numContacts = CONTACT_MAX_POINTS;
}

/*
** Clip the vertices of the incident face by the normals of the
** faces adjacent to the reference face using the Sutherland-Hodgman
** clipping algorithm. The reference face is always assumed to be on
** "hullA" and the incident face is always assumed to be on "hullB".
*/
static void clipFaceContact(const colliderHull *hullA, const colliderHull *hullB, const colliderFaceIndex_t refIndex,
#ifdef CLIPPING_KEYSWAP_USE_OFFSETS
                            const size_t offset, contactManifold *cm){
#else
                            const byte_t swapped, contactManifold *cm){
#endif

	//Meshes store the number of edges that their largest faces have,
	//so we can use this to preallocate enough memory for any face.
	//We will need to store two arrays that are both large enough
	//to store twice the maximum number of edges that a face has, as
	//each edge can intersect two faces at most. This means that, in
	//practice, we are really storing four times the number of edges.
	vertexClip *vertices = memoryManagerGlobalAlloc(hullB->maxFaceEdges * sizeof(*vertices) * 2);
	if(vertices == NULL){
		/** MALLOC FAILED **/
	}

	//Points to the beginning of the
	//array of vertices to loop through.
	vertexClip *loopVertices = vertices;
	vertexClip *loopVertex;
	//The variable "clipVertices" points to the beginning
	//of the array of vertices that have been clipped. It
	//will also store the separation of each vertex when
	//we clip against the reference face itself.
	union {
		vertexClip *v;
		vertexProject *p;
	} clipVertices, clipVertex;
	clipVertices.v = &vertices[hullB->maxFaceEdges];

	vec3 refNormal = hullA->normals[refIndex];
	//Using the normal of the reference face, find the face
	//on the incident hull whose normal is most opposite it.
	const colliderFaceIndex_t incIndex = findIncidentFace(hullB, &refNormal);

	float curDist;

	const vec3 *refVertex;
	const vertexClip *curVertex;
	vertexClip *lastVertex = loopVertices;
	const colliderHullEdge *curEdge = &hullB->edges[hullB->faces[incIndex]];
	const colliderHullEdge *startEdge = curEdge;

	colliderEdgeIndex_t inEdgeIndex;
	colliderEdgeIndex_t outEdgeIndex = incIndex;

	//Store the incident face's vertices in the array.
	for(;;){
		//The "inEdgeIndex" variable will not be set on
		//the first iteration, so we can't use it here.
		#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
		lastVertex->key.edgeB = outEdgeIndex;
		#else
		lastVertex->key.inEdgeB  = CONTACT_KEY_INVALID_EDGE;
		lastVertex->key.outEdgeB = outEdgeIndex;
		#endif
		inEdgeIndex = outEdgeIndex;

		//Edges are shared by both faces that use
		//them, so we need to check whether or not
		//the incident face is this edge's twin
		//face to make sure we get the right vertex.
		if(curEdge->faceIndex == incIndex){
			lastVertex->v = hullB->vertices[curEdge->startVertexIndex];
			outEdgeIndex = curEdge->nextIndex;
		}else{
			lastVertex->v = hullB->vertices[curEdge->endVertexIndex];
			outEdgeIndex = curEdge->twinNextIndex;
		}
		curEdge = &hullB->edges[outEdgeIndex];

		//If we've finished adding all of the vertices, break the loop.
		if(curEdge == startEdge){
			break;
		}
		//Otherwise, add the next vertex. We only increment after
		//checking if the loop can be ended to ensure that "lastVertex"
		//points to the last vertex and not the one before the last.
		++lastVertex;
		//Now we can set the in-edge for the next vertex.
		//This ensures that every vertex except the first
		//will have a valid in-edge by the time we're done.
		#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
		lastVertex->key.edgeA = inEdgeIndex;
		#else
		lastVertex->key.inEdgeB  = inEdgeIndex;
		lastVertex->key.outEdgeB = CONTACT_KEY_INVALID_EDGE;
		#endif
	}

	//We skipped setting the first vertex's
	//in-edge since we didn't know what it was,
	//but now that we do we can set it here.
	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
	loopVertices[0].key.edgeA = inEdgeIndex;
	#else
	loopVertices[0].key.inEdgeB  = inEdgeIndex;
	loopVertices[0].key.outEdgeB = CONTACT_KEY_INVALID_EDGE;
	#endif


	curEdge = &hullA->edges[hullA->faces[refIndex]];
	startEdge = curEdge;
	inEdgeIndex = refIndex;

	//For each face surrounding the reference face,
	//clip each incident vertex against its normal.
	do {
		const vertexClip *nextVertex;
		const vec3 *adjNormal;

		clipVertex.v = clipVertices.v;
		//Edges are shared by both faces that use
		//them, so we need to check whether or not
		//the reference face is this edge's twin
		//face to make sure we get the right data.
		if(curEdge->faceIndex == refIndex){
			refVertex = &hullA->vertices[curEdge->startVertexIndex];
			adjNormal = &hullA->normals[curEdge->twinFaceIndex];
			outEdgeIndex = curEdge->nextIndex;
		}else{
			refVertex = &hullA->vertices[curEdge->startVertexIndex];
			adjNormal = &hullA->normals[curEdge->nextIndex];
			outEdgeIndex = curEdge->twinFaceIndex;
		}
		curEdge = &hullA->edges[outEdgeIndex];

		curVertex = lastVertex;
		curDist = pointPlaneDist(&curVertex->v, refVertex, adjNormal);
		nextVertex = loopVertices;
		//For each edge on the incident face, clip
		//it against the current reference face.
		do {
			const float nextDist = pointPlaneDist(&nextVertex->v, refVertex, adjNormal);
			//The starting vertex is inside the clipping
			//region, so we will have to keep it.
			if(curDist <= 0.f){
				/*
				The starting incident vertex sI is behind the plane,
				but there may or may not be an intersection. Regardless,
				its in-edge and out-edge need not be changed.

				   ___________________
				  |                   |
				  |                   |
				  |  eI _________ sI  |
				  |    |         |    |
				  |____|_________|____|
				sR     |         |     eR
				       |         |
				       |_________|
				*/
				*clipVertex.v = *curVertex;
				++clipVertex.v;

				//The edge leaves the clipping region, so we
				//need to find and keep the intersection point.
				if(nextDist > 0.f){
					/*
					An intersection between incident edge I (sI, eI)
					and reference edge R (sR, eR) exists. The in-edge
					for the clipped vertex is I and the out-edge is R.

					   _________
					  |         |
					  |         |
					  |  sI ____|____
					  |    |    |    |
					  |____|____|    |
					sR     |     eR  |
					       |         |
					       |_________|
					     eI
					*/
					vec3Lerp(&curVertex->v, &nextVertex->v, curDist / (curDist - nextDist), &clipVertex.v->v);
					#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
					clipVertex.v->key.edgeA = curVertex->key.edgeA;
					clipVertex.v->key.edgeB = inEdgeIndex;
					#else
					clipVertex.v->key.inEdgeA  = CONTACT_KEY_INVALID_EDGE;
					clipVertex.v->key.outEdgeA = inEdgeIndex;
					clipVertex.v->key.inEdgeB  = curVertex->key.outEdgeB;
					clipVertex.v->key.outEdgeB = CONTACT_KEY_INVALID_EDGE;
					#endif
					++clipVertex.v;
				}

			//If the starting vertex is outside the clipping
			//region and the ending vertex is inside it, we
			//need to find and keep the intersection point.
			}else if(nextDist <= 0.f){
				/*
				An intersection between incident edge I (sI, eI)
				and reference edge R (sR, eR) exists. The in-edge
				for the clipped vertex is R and the out-edge is I.

				 _________ eR
				|         |
				|         |
				|  eI ____|____ sI
				|    |    |    |
				|____|____|    |
				     |     sR  |
				     |         |
				     |_________|
				*/
				vec3Lerp(&curVertex->v, &nextVertex->v, curDist / (curDist - nextDist), &clipVertex.v->v);
				#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
				clipVertex.v->key.edgeA = inEdgeIndex;
				clipVertex.v->key.edgeB = curVertex->key.edgeB;
				#else
				clipVertex.v->key.inEdgeA  = inEdgeIndex;
				clipVertex.v->key.outEdgeA = CONTACT_KEY_INVALID_EDGE;
				clipVertex.v->key.inEdgeB  = CONTACT_KEY_INVALID_EDGE;
				clipVertex.v->key.outEdgeB = curVertex->key.outEdgeB;
				#endif
				++clipVertex.v;
			}

			//We already know the next vertex and its
			//distance, so set the current vertex to it.
			curVertex = nextVertex;
			curDist = nextDist;
			inEdgeIndex = outEdgeIndex;
			++nextVertex;
		} while(curVertex != lastVertex);

		//After clipping the face, we need to swap the "loop" array
		//with the "clip" array. This will allow us to loop through
		//the newly clipped vertices on the next iteration.
		lastVertex = clipVertices.v;
		clipVertices.v = loopVertices;
		loopVertices = lastVertex;
		//The variable "clipVertex" will point to the vertex after the
		//last one that was set, so we need to get the one before it.
		//The array should never be empty, so we don't have to worry
		//about funny stuff happening as a result of this subtraction.
		lastVertex = &clipVertex.v[-1];
	} while(curEdge != startEdge);

	loopVertex = loopVertices;
	clipVertex.p = clipVertices.p;
	curVertex = loopVertices;
	//Keep any points below the reference face and project them
	//onto it. We won't keep any intersection points, as they do
	//not contribute towards the stability of the contact manifold.
	//
	//Note that we do not need to set "refVertex". This is because we
	//guarantee that it will always be set to one of the reference face's
	//vertices in the previous loop by only looping through its edges.
	for(; curVertex <= lastVertex; ++curVertex){
		//Use the distance between the current
		//vertex and the reference face to ensure
		//that it is within the clipping region.
		curDist = pointPlaneDist(&curVertex->v, refVertex, &refNormal);
		if(curDist <= 0.f){
			//If the hulls were swapped before being passed into
			//this function, we'll need to swap the key values.
			#ifdef CLIPPING_KEYSWAP_USE_OFFSETS
				#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
			loopVertex->key.edgeA = clippingGetKeyEdgeA(curVertex->key, offset);
			loopVertex->key.edgeB = clippingGetKeyEdgeB(curVertex->key, offset);
				#else
			loopVertex->key.inEdgeA  = clippingGetKeyInEdgeA(curVertex->key, offset);
			loopVertex->key.outEdgeA = clippingGetKeyOutEdgeA(curVertex->key, offset);
			loopVertex->key.inEdgeB  = clippingGetKeyInEdgeB(curVertex->key, offset);
			loopVertex->key.outEdgeB = clippingGetKeyOutEdgeB(curVertex->key, offset);
				#endif
			#else
				#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
			if(swapped == CLIPPING_INORDER){
				loopVertex->key = curVertex->key;
			}else{
				loopVertex->key.edgeA = curVertex->key.edgeB;
				loopVertex->key.edgeB = curVertex->key.edgeA;
			}
				#else
			if(swapped == CLIPPING_INORDER){
				loopVertex->key = curVertex->key;
			}else{
				loopVertex->key.inEdgeA  = curVertex->key.inEdgeB;
				loopVertex->key.outEdgeA = curVertex->key.outEdgeB;
				loopVertex->key.inEdgeB  = curVertex->key.inEdgeA;
				loopVertex->key.outEdgeB = curVertex->key.outEdgeA;
			}
				#endif
			#endif

			loopVertex->v = curVertex->v;
			++loopVertex;
			//Project the vertex onto the reference
			//face and store it in "clipVertices".
			pointPlaneProject(&curVertex->v, refVertex, &refNormal, &clipVertex.p->v);
			//Keep the vertex's separation, too.
			clipVertex.p->dist = curDist;
			++clipVertex.p;
		}
	}


	//Now that we've finished clipping, we can begin adding
	//our remaining contact points to the manifold.

	//If there are four or less vertices, we can
	//simply keep all of them. Note that we check
	//"loopVertex" instead of "lastVertex", as it
	//points to the vertex after the last valid one.
	if(loopVertex <= &loopVertices[CONTACT_MAX_POINTS]){
		contactPoint *curContact = cm->contacts;

		clipVertex.p = clipVertices.p;
		curVertex = loopVertices;
		//Add out contact points to the manifold.
		for(; curVertex < loopVertex; ++curVertex, ++clipVertex.p, ++curContact){
			curContact->key = curVertex->key;
			curContact->pA = clipVertex.p->v;
			curContact->pB = curVertex->v;
			curContact->separation = clipVertex.p->dist;

			++cm->numContacts;
		}

	//If there are more than four vertices, we'll
	//need to perform contact point reduction.
	}else{
		reduceContacts(clipVertices.p, clipVertex.p, loopVertices, cm);
	}


	//Store the contact normal. If the hulls were swapped before being
	//passed in, this normal will be inverted outside this function.
	cm->normal = refNormal;


	memoryManagerGlobalFree(vertices);
}

/*
** If the axis of minimum penetration is realised
** by an edge pair, the contact point must be the
** closest points between the minimising edges.
*/
static void clipEdgeContact(const colliderHull *hullA, const colliderHull *hullB,
                            const hullEdgeData *edgeData, contactManifold *cm){

	const colliderHullEdge *refEdge = &hullA->edges[edgeData->edgeA];
	const vec3 *refStart = &hullA->vertices[refEdge->startVertexIndex];
	const vec3 *refEnd   = &hullA->vertices[refEdge->endVertexIndex];
	const vec3 ref = {
		.x = refEnd->x - refStart->x,
		.y = refEnd->y - refStart->y,
		.z = refEnd->z - refStart->z
	};

	const colliderHullEdge *incEdge = &hullB->edges[edgeData->edgeB];
	const vec3 *incStart = &hullB->vertices[incEdge->startVertexIndex];
	const vec3 *incEnd   = &hullB->vertices[incEdge->endVertexIndex];
	const vec3 inc = {
		.x = incEnd->x - incStart->x,
		.y = incEnd->y - incStart->y,
		.z = incEnd->z - incStart->z
	};

	const vec3 offset = {
		.x = refStart->x - incStart->x,
		.y = refStart->y - incStart->y,
		.z = refStart->z - incStart->z
	};
	const vec3 normalDir = {
		.x = refStart->x - hullA->centroid.x,
		.y = refStart->y - hullA->centroid.y,
		.z = refStart->z - hullA->centroid.z
	};

	contactPoint *contact = &cm->contacts[0];


	//The edges may not necessarily be intersecting, so we'll
	//need to find the closest points on both line segments.
	//This is basically copied from "segmentClosestPoints".
	const float d11 = vec3NormVec3(&ref);
	const float d21 = vec3DotVec3(&offset, &ref);
	const float d23 = vec3DotVec3(&offset, &inc);
	const float d31 = vec3DotVec3(&inc, &ref);
	const float d33 = vec3NormVec3(&inc);
	const float denom = d11 * d33 - d31 * d31;
	//If the two edges are perfectly parallel, the closest
	//points should be in the middle of the first segment.
	const float m1 = (denom != 0.f) ? (d23 * d31 - d21 * d33) / denom : 0.5f;
	const float m2 = (d23 + d31 * m1) / d33;

	//Find the closest point on the first line.
	vec3LerpFast(refStart, &ref, m1, &contact->pA);
	//Find the closest point on the second line.
	vec3LerpFast(incStart, &inc, m2, &contact->pB);


	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
	contact->key.edgeA = edgeData->edgeA;
	contact->key.edgeB = edgeData->edgeB;
	#else
	contact->key.inEdgeA  = edgeData->edgeA;
	contact->key.outEdgeA = edgeData->edgeA;
	contact->key.inEdgeB  = edgeData->edgeB;
	contact->key.outEdgeB = edgeData->edgeB;
	#endif

	contact->separation = edgeData->separation;


	//Find the collision's normal. We use the
	//cross product of the two intersecting edges.
	vec3CrossVec3Out(&ref, &inc, &cm->normal);
	vec3NormalizeVec3(&cm->normal);
	//We'll need to make sure the normal roughly
	//points from body A to body B, too.
	if(vec3DotVec3(&cm->normal, &normalDir) < 0.f){
		vec3Negate(&cm->normal);
	}


	cm->numContacts = 1;
}