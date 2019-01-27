#include "colliderHull.h"


#define COLLISION_PARALLEL_THRESHOLD 0.005f

#define CLIPPING_HULLS_INORDER 0
#define CLIPPING_HULLS_SWAPPED 1


#include <math.h>
#include <string.h>

#include "utilMath.h"

#include "memoryManager.h"


//Stores the index of a face and its distance
//from the closest point on another object.
typedef struct hullFaceData {
	colliderHullFace index;
	float penetration;
} hullFaceData;

//Stores the index of an edge pair
//and their distance from each other.
typedef struct hullEdgeData {
	colliderHullEdge *edgeA;
	colliderHullEdge *edgeB;
	float penetration;
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


static void hullFaceDataInit(hullFaceData *faceData);
static void hullEdgeDataInit(hullEdgeData *edgeData);
static void collisionDataInit(collisionData *cd);

static return_t noFaceSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs);
static return_t noEdgeSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs);

static void clipManifoldSHC(const colliderHull *hullA, const colliderHull *hullB, const collisionData *cd, contactManifold *cm);
static return_t isMinkowskiFace(const colliderHull *hullA, const colliderHull *hullB,
                                const colliderHullEdge *eA, const vec3 *invEdgeA,
                                const colliderHullEdge *eB, const vec3 *invEdgeB);
static float edgeDistSquared(const vec3 *pA, const vec3 *edgeDirA,
                             const vec3 *pB, const vec3 *edgeDirB,
                             const vec3 *centroid);

static return_t noSeparatingFace(const colliderHull *hullA, const colliderHull *hullB,
                                 hullFaceData *faceData, contactSeparation *separation, const byte_t separationType);
static return_t noSeparatingEdge(const colliderHull *hullA, const colliderHull *hullB,
                                 hullEdgeData *edgeData, contactSeparation *separation);

static uint_least16_t findIncidentFace(const colliderHull *hull, const vec3 *refNormal);
static void reduceContacts(const vertexClip *refVertices, const vertexClip *lastRefVertex, const vertexClip *incVertices,
                           const byte_t swapped, contactManifold *cm);
static void clipFaceContact(const colliderHull *hullA, const colliderHull *hullB, const uint_least16_t refIndex,
                            const byte_t swapped, contactManifold *cm);
static void clipEdgeContact(const colliderHull *hullA, const colliderHull *hullB,
                            const colliderHullEdge *refEdge, const colliderHullEdge *incEdge,
                            contactManifold *cm);


#warning "What if we aren't using the global memory manager?"


//Load the collider from a file.
void colliderHullLoad(void *hull){
	//
}

//Calculate the collider's centroid and other such properties.
void colliderHullSetupProperties(void *hull){
	colliderHullCalculateCentroid((colliderHull *)hull);
}


//Calculate the collider's inertia tensor relative to its centroid.
void colliderHullCalculateInertia(const void *hull, float inertia[6]){
	const vec3 *curVertex = ((colliderHull *)hull)->vertices;
	const float *curMass = ((colliderHull *)hull)->massArray;
	const vec3 *lastVertex = &((colliderHull *)hull)->vertices[((colliderHull *)hull)->numVertices];
	const vec3 *centroid = &((colliderHull *)hull)->centroid;

	memset(inertia, 0, 24);

	if(curMass != NULL){
		//Add each vertex's contribution to the collider's inertia tensor.
		for(; curVertex < lastVertex; ++curVertex, ++curMass){
			const float x = curVertex->x - centroid->x;
			const float y = curVertex->y - centroid->y;
			const float z = curVertex->z - centroid->z;
			const float xx = x * x;
			const float yy = y * y;
			const float zz = z * z;
			const float massValue = *curMass;

			inertia[0] += (yy + zz) * massValue;
			inertia[1] += (xx + zz) * massValue;
			inertia[2] += (xx + yy) * massValue;
			inertia[3] -= x * y * massValue;
			inertia[4] -= x * z * massValue;
			inertia[5] -= y * z * massValue;
		}
	}else{
		//If the vertices are not weighted, we can save some multiplications.
		for(; curVertex < lastVertex; ++curVertex){
			const float x = curVertex->x - centroid->x;
			const float y = curVertex->y - centroid->y;
			const float z = curVertex->z - centroid->z;
			const float xx = x * x;
			const float yy = y * y;
			const float zz = z * z;

			inertia[0] += yy + zz;
			inertia[1] += xx + zz;
			inertia[2] += xx + yy;
			inertia[3] -= x * y;
			inertia[4] -= x * z;
			inertia[5] -= y * z;
		}
	}
}

//Calculate the collider's centre of gravity.
void colliderHullCalculateCentroid(const colliderHull *hull){
	//
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

/*
** Return a pointer to the vertex farthest from the origin in the
** direction "dir" "dir" and output its index to the variable "index".
*/
const vec3 *colliderHullSupportIndex(const colliderHull *hull, const vec3 *dir, uint_least16_t *index){
	const vec3 *bestVertex = hull->vertices;
	float bestDistance = vec3DotVec3(bestVertex, dir);

	const vec3 *curVertex = &bestVertex[1];
	const vec3 *lastVertex = &bestVertex[hull->numVertices];

	uint_least16_t i;
	*index = 0;

	for(i = 1; curVertex < lastVertex; ++curVertex, ++i){
		const float curDistance = vec3DotVec3(curVertex, dir);
		//If the current vertex is farther in the direction
		//of "dir" than our current best, store it instead.
		if(curDistance > bestDistance){
			bestVertex = curVertex;
			bestDistance = curDistance;
			*index = i;
		}
	}

	return(bestVertex);
}


//Check if a separation no longer exists between two hulls.
return_t colliderHullNotSeparated(const void *hullA, const void *hullB, contactSeparation *cs){
	switch(cs->type){
		case COLLIDER_HULL_SEPARATION_FACE_A:
			return(noFaceSeparation(hullA, hullB, cs));
		break;
		case COLLIDER_HULL_SEPARATION_FACE_B:
			return(noFaceSeparation(hullB, hullA, cs));
		break;
		default:
			return(noEdgeSeparation(hullA, hullB, cs));
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


void colliderHullDelete(colliderHull *hull){
	if(hull->vertices != NULL){
		memoryManagerGlobalFree(hull->vertices);
	}
	if(hull->edges != NULL){
		memoryManagerGlobalFree(hull->edges);
	}
	if(hull->normals != NULL){
		memoryManagerGlobalFree(hull->normals);
	}
	if(hull->faces != NULL){
		memoryManagerGlobalFree(hull->faces);
	}
	if(hull->massArray != NULL){
		memoryManagerGlobalFree(hull->massArray);
	}
}


static void hullFaceDataInit(hullFaceData *faceData){
	faceData->index = -1;
	faceData->penetration = -INFINITY;
}

static void hullEdgeDataInit(hullEdgeData *edgeData){
	edgeData->edgeA = NULL;
	edgeData->edgeB = NULL;
	edgeData->penetration = -INFINITY;
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


//Return whether a face separation between two convex hulls no longer exists.
static return_t noFaceSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs){
	return(
		pointPlaneDist(
			&hullB->vertices[cs->featureB],
			&hullA->vertices[cs->featureA],
			&hullA->normals[cs->featureA]
		) <= 0.f
	);
}

//Return whether an edge separation between two convex hulls no longer exists.
static return_t noEdgeSeparation(const colliderHull *hullA, const colliderHull *hullB, contactSeparation *cs){
	const colliderHullEdge *edgeA = &hullA->edges[cs->featureA];
	const vec3 *startVertexA = &hullA->vertices[edgeA->endVertexIndex];
	vec3 invEdgeA;
	const colliderHullEdge *edgeB = &hullB->edges[cs->featureB];
	const vec3 *startVertexB = &hullB->vertices[edgeB->endVertexIndex];
	vec3 invEdgeB;

	vec3SubtractVec3From(startVertexA, &hullA->vertices[edgeA->endVertexIndex], &invEdgeA);
	vec3SubtractVec3From(startVertexB, &hullB->vertices[edgeB->endVertexIndex], &invEdgeB);

	return(
		isMinkowskiFace(hullA, hullB, edgeA, &invEdgeA, edgeB, &invEdgeB) &&
		edgeDistSquared(startVertexA, &invEdgeA, startVertexB, &invEdgeB, &hullA->centroid) <= 0.f
	);
}


/*
** Using the collision information generated from the
** Separating Axis Theorem, clip the penetrating area
** using the Sutherland-Hodgman clipping algorithm
** to create the contact manifold.
*/
static void clipManifoldSHC(const colliderHull *hullA, const colliderHull *hullB, const collisionData *cd, contactManifold *cm){
	const float bestFaceSeparation = maxNum(cd->faceA.penetration, cd->faceB.penetration);

	//If the deepest penetration occurs on an edge normal, clip using the edges.
	if(cd->edgeData.penetration > bestFaceSeparation){
		clipEdgeContact(hullA, hullB, cd->edgeData.edgeA, cd->edgeData.edgeB, cm);

	//Otherwise, clip using whichever face has the
	//greatest penetration depth as the reference face.
	}else{
		if(cd->faceA.penetration >= cd->faceB.penetration){
			clipFaceContact(hullA, hullB, cd->faceA.index, CLIPPING_HULLS_INORDER, cm);
		}else{
			clipFaceContact(hullB, hullA, cd->faceB.index, CLIPPING_HULLS_SWAPPED, cm);
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


	vec3CrossVec3(edgeDirA, edgeDirB, &edgeNormal);
	//The norm of the vector is the square of its magnitude.
	edgeCrossLength = vec3NormVec3(&edgeNormal);
	//If the two edges are parallel, we can exit early.
	if(edgeCrossLength < COLLISION_PARALLEL_THRESHOLD * (vec3NormVec3(edgeDirA) + vec3NormVec3(edgeDirB))){
		return(-INFINITY);
	}


	vec3MultiplyS(&edgeNormal, fastInvSqrt(edgeCrossLength), &edgeNormal);
	vec3SubtractVec3From(pA, centroid, &offset);
	//If the edge normal does not point from
	//object A to object B, we need to invert it.
	if(vec3DotVec3(&edgeNormal, &offset) < 0.f){
		vec3Negate(&edgeNormal);
	}


	vec3SubtractVec3From(pB, pA, &offset);

	//Return the distance between the edges.
	return(vec3DotVec3(&edgeNormal, &offset));
}


/*
** For each face on "hullA", check the points on "hullB"
** farthest in the direction opposite the face's normal.
** If the penetration depth is positive, we have found a
** separating axis and the shapes are not colliding.
** Otherwise, we can assume that they might be colliding.
**
** We store the index of the least separating face in
** the variable "faceData".
*/
static return_t noSeparatingFace(const colliderHull *hullA, const colliderHull *hullB,
                                 hullFaceData *faceData, contactSeparation *separation, const byte_t separationType){

	const colliderHullFace *curFace = hullA->faces;
	const vec3 *curNormal = hullA->normals;
	const colliderHullFace *lastFace = &hullA->faces[hullA->numFaces];

	uint_least16_t i;
	for(i = 0; curFace < lastFace; ++i){
		const vec3 invNormal = {
			.x = -curNormal->x,
			.y = -curNormal->y,
			.z = -curNormal->z
		};
		uint_least16_t hullBVertexIndex;
		//First, find the point on "hullB" farthest in the
		//direction opposite the current face's normal.
		//Then, find the distance between the two.
		const float curDistance = pointPlaneDist(
			colliderHullSupportIndex(hullB, &invNormal, &hullBVertexIndex),
			&hullA->vertices[hullA->edges[*curFace].startVertexIndex],
			curNormal
		);

		//If the distance is greater than zero, we can
		//exit early as the hulls aren't colliding.
		if(curDistance > 0.f){
			if(separation != NULL){
				separation->featureA = i;
				separation->featureB = hullBVertexIndex;
				separation->type = separationType;
			}

			return(0);

		//If this is the least separating face we've
		//found so far, record its index and distance.
		}else if(curDistance > faceData->penetration){
			faceData->index = i;
			faceData->penetration = curDistance;
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
	const colliderHullEdge *lastEdgeA = &hullA->edges[hullA->numEdges];
	const colliderHullEdge *lastEdgeB = &hullB->edges[hullB->numEdges];
	uint_least16_t a;

	for(a = 0; edgeA < lastEdgeA; ++edgeA, a += 2){
		colliderHullEdge *edgeB = hullB->edges;
		const vec3 *startVertexA = &hullA->vertices[edgeA->startVertexIndex];
		vec3 invEdgeA;
		uint_least16_t b;

		//Get the inverse of edge 1's normal.
		vec3SubtractVec3From(startVertexA, &hullA->vertices[edgeA->endVertexIndex], &invEdgeA);

		for(b = 0; edgeB < lastEdgeB; ++edgeB, b += 2){
			const vec3 *startVertexB = &hullB->vertices[edgeB->startVertexIndex];
			vec3 invEdgeB;
			//Get the inverse of edge 2's normal
			vec3SubtractVec3From(startVertexB, &hullB->vertices[edgeB->endVertexIndex], &invEdgeB);

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
				}else if(curDistance > edgeData->penetration){
					edgeData->edgeA = edgeA;
					edgeData->edgeB = edgeB;
					edgeData->penetration = curDistance;
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
static colliderHullFace findIncidentFace(const colliderHull *hull, const vec3 *refNormal){
	const vec3 *curNormal = hull->normals;
	colliderHullFace bestFace = 0;
	float bestDistance = vec3DotVec3(curNormal, refNormal);
	uint_least16_t i;

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
static void reduceContacts(const vertexClip *refVertices, const vertexClip *lastRefVertex, const vertexClip *incVertices,
                           const byte_t swapped, contactManifold *cm){

	//We start with our best and worst vertices as the
	//first one so we can begin the loop on the second.
	const vertexClip *bestRefVertex = refVertices;
	const vertexClip *bestIncVertex = incVertices;
	float bestDist = vec3NormVec3(&bestRefVertex->v);
	const vertexClip *worstRefVertex = refVertices;
	const vertexClip *worstIncVertex = incVertices;
	float worstDist = bestDist;

	const vec3 *firstContact;
	const vec3 *secondContact;
	contactPoint *curContact = cm->contacts;

	vec3 edgeNormal;

	const vertexClip *curRefPoint = &refVertices[1];
	const vertexClip *curIncPoint = &incVertices[1];
	float curDist;


	//Start by finding the vertices with the greatest
	//positive and negative distances from the origin.
	for(; curRefPoint < lastRefVertex; ++curRefPoint, ++curIncPoint){
		curDist = vec3NormVec3(&curRefPoint->v);
		//The first vertex will be the one with the
		//most positive distance from the origin.
		if(curDist > bestDist){
			bestRefVertex = curRefPoint;
			bestIncVertex = curIncPoint;
			bestDist = curDist;

		//The second vertex will be the one with the
		//most negative distance from the origin.
		}else if(curDist < worstDist){
			worstRefVertex = curRefPoint;
			worstIncVertex = curIncPoint;
			worstDist = curDist;
		}
	}

	//We need to remember these pointers so we
	//don't store the same points multiple times.
	firstContact = &bestRefVertex->v;
	secondContact = &worstRefVertex->v;

	//Add the points to our manifold.
	++curContact;
	curContact->key = bestIncVertex->key;
	curContact->rA = bestRefVertex->v;
	curContact->rB = bestIncVertex->v;
	curContact->penetration = pointPlaneDist(&bestIncVertex->v, &bestRefVertex->v, &cm->normal);
	++curContact;
	curContact->key = worstIncVertex->key;
	curContact->rA = worstRefVertex->v;
	curContact->rB = worstIncVertex->v;
	curContact->penetration = pointPlaneDist(&worstIncVertex->v, &worstRefVertex->v, &cm->normal);
	++curContact;


	//The normal of the edge is the cross product of the reference
	//face's normal and the difference between the edge's vertices.
	//This works because every vertex lies on the reference face.
	vec3CrossVec3Float(&cm->normal, worstRefVertex->v.x - bestRefVertex->v.x, worstRefVertex->v.y - bestRefVertex->v.y, worstRefVertex->v.z - bestRefVertex->v.z, &edgeNormal);

	//Again, we start with our best and worst vertices as
	//the first one so we can begin the loop on the second.
	bestRefVertex = refVertices;
	bestIncVertex = incVertices;
	bestDist = vec3DotVec3Float(&edgeNormal, firstContact->x - bestRefVertex->v.x, firstContact->y - bestRefVertex->v.y, firstContact->z - bestRefVertex->v.z);
	worstRefVertex = refVertices;
	worstIncVertex = incVertices;
	worstDist = bestDist;

	curRefPoint = &refVertices[1];
	curIncPoint = &incVertices[1];

	//Now, we can find the points with the greatest positive
	//and negative distances from the edge we have created.
	for(; curRefPoint < lastRefVertex; ++curRefPoint, ++curIncPoint){
		//Make sure this vertex is not already part of the manifold.
		if(&curRefPoint->v != firstContact && &curRefPoint->v != secondContact){
			curDist = vec3DotVec3Float(&edgeNormal, firstContact->x - curRefPoint->v.x, firstContact->y - curRefPoint->v.y, firstContact->z - curRefPoint->v.z);
			//The third vertex will be the one with
			//the most positive distance from our edge.
			if(curDist > bestDist){
				bestRefVertex = curRefPoint;
				bestIncVertex = curIncPoint;
				bestDist = curDist;

			//The fourth vertex will be the one with
			//the most negative distance from our edge.
			}else if(curDist < worstDist){
				worstRefVertex = curRefPoint;
				worstIncVertex = curIncPoint;
				worstDist = curDist;
			}
		}
	}

	//Add the points to our manifold.
	if(!swapped){
		contactPointInit(curContact, bestRefVertex, bestIncVertex, pointPlaneDist(&bestIncVertex->v, &bestRefVertex->v, &cm->normal));
		++curContact;
		contactPointInit(curContact, worstRefVertex, worstIncVertex, pointPlaneDist(&worstIncVertex->v, &worstRefVertex->v, &cm->normal));
	}else{
		contactPointInitSwap(curContact, bestRefVertex, bestIncVertex, pointPlaneDist(&bestIncVertex->v, &bestRefVertex->v, &cm->normal));
		++curContact;
		contactPointInitSwap(curContact, worstRefVertex, worstIncVertex, pointPlaneDist(&worstIncVertex->v, &worstRefVertex->v, &cm->normal));
	}
	curContact->key = bestIncVertex->key;
	curContact->rA = bestRefVertex->v;
	curContact->rB = bestIncVertex->v;
	curContact->penetration = pointPlaneDist(&bestIncVertex->v, &bestRefVertex->v, &cm->normal);
	++curContact;
	curContact->key = worstIncVertex->key;
	curContact->rA = worstRefVertex->v;
	curContact->rB = worstIncVertex->v;
	curContact->penetration = pointPlaneDist(&worstIncVertex->v, &worstRefVertex->v, &cm->normal);

	//We should now have only four contact points.
	cm->numContacts = 4;
}

/*
** Clip the vertices of the incident face by the normals of the
** faces adjacent to the reference face using the Sutherland-Hodgman
** clipping algorithm. The reference face is always assumed to be on
** "hullA" and the incident face is always assumed to be on "hullB".
*/
static void clipFaceContact(const colliderHull *hullA, const colliderHull *hullB, const colliderHullFace refIndex,
                            const byte_t swapped, contactManifold *cm){

	//Meshes store the number of edges that their largest faces have,
	//so we can use this to preallocate enough memory for any face.
	//We will need to store two arrays that are both large enough
	//to store twice the maximum number of edges that a face has, as
	//each edge can intersect two faces at most. This means that, in
	//practice, we are really storing four times the number of edges.
	vertexClip *vertices = memoryManagerGlobalAlloc(hullB->maxFaceVertices * sizeof(*vertices) * 2);
	if(vertices == NULL){
		/** MALLOC FAILED **/
	}

	//Points to the beginning of the
	//array of vertices to loop through.
	vertexClip *loopVertices = vertices;
	vertexClip *loopVertex;
	//Points to the beginning of the
	//array of vertices to loop through.
	vertexClip *clipVertices = &vertices[hullB->maxFaceVertices];
	vertexClip *clipVertex;

	vec3 refNormal = hullA->normals[refIndex];
	//Using the normal of the reference face, find the face
	//on the incident hull whose normal is most opposite it.
	const colliderHullFace incIndex = findIncidentFace(hullB, &refNormal);

	float curDist;

	const vec3 *refVertex;
	const vertexClip *curVertex;
	vertexClip *lastVertex = loopVertices;
	const colliderHullEdge *curEdge = &hullB->edges[hullB->faces[incIndex]];
	const colliderHullEdge *startEdge = curEdge;

	uint_least16_t inEdgeIndex;
	uint_least16_t outEdgeIndex = incIndex;

	//Store the incident face's vertices in the array.
	for(;;){
		//The "inEdgeIndex" variable will not be set on
		//the first iteration, so we can't use it here.
		#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
		lastVertex->key.edgeB = outEdgeIndex;
		#else
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
		lastVertex->key.inEdgeB = inEdgeIndex;
		#endif
	}

	//We skipped setting the first vertex's
	//in-edge since we didn't know what it was,
	//but now that we do we can set it here.
	#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
	loopVertices[0].key.edgeA = inEdgeIndex;
	#else
	loopVertices[0].inEdgeB = inEdgeIndex;
	#endif


	curEdge = &hullA->edges[hullA->faces[refIndex]];
	startEdge = curEdge;
	inEdgeIndex = refIndex;

	//For each face surrounding the reference face,
	//clip each incident vertex against its normal.
	do {
		const vertexClip *nextVertex;
		const vec3 *adjNormal;

		clipVertex = clipVertices;
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
				*clipVertex = *curVertex;
				++clipVertex;

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
					vec3Lerp(&curVertex->v, &nextVertex->v, curDist / (curDist - nextDist), &clipVertex->v);
					#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
					clipVertex->key.edgeA = curVertex->key.edgeA;
					clipVertex->key.edgeB = inEdgeIndex;
					#else
					clipVertex->key.inEdgeA  = CONTACT_KEY_INVALID_EDGE;
					clipVertex->key.outEdgeA = inEdgeIndex;
					clipVertex->key.inEdgeB  = curVertex->key.outEdgeB;
					clipVertex->key.outEdgeB = CONTACT_KEY_INVALID_EDGE;
					#endif
					++clipVertex;
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
				vec3Lerp(&curVertex->v, &nextVertex->v, curDist / (curDist - nextDist), &clipVertex->v);
				#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
				clipVertex->key.edgeA = inEdgeIndex;
				clipVertex->key.edgeB = curVertex->key.edgeB;
				#else
				clipVertex->key.inEdgeA  = inEdgeIndex;
				clipVertex->key.outEdgeA = CONTACT_KEY_INVALID_EDGE;
				clipVertex->key.inEdgeB  = CONTACT_KEY_INVALID_EDGE;
				clipVertex->key.outEdgeB = curVertex->key.outEdgeB;
				#endif
				++clipVertex;
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
		lastVertex = clipVertices;
		clipVertices = loopVertices;
		loopVertices = lastVertex;
		//The variable "clipVertex" will point to the vertex after the
		//last one that was set, so we need to get the one before it.
		//The array should never be empty, so we don't have to worry
		//about funny stuff happening as a result of this subtraction.
		lastVertex = &clipVertex[-1];
	} while(curEdge != startEdge);


	//Store the contact normal for collision
	//response and the two tangents for friction.
	cm->normal = refNormal;

	//We'll need to calculate the inverse of the
	//reference face's normal for the next step.
	vec3Negate(&refNormal);


	loopVertex = loopVertices;
	clipVertex = clipVertices;
	curVertex = loopVertices;
	//Clip the remaining vertices against the actual reference face.
	//Unlike before, we won't keep any intersection points, as they
	//do not contribute towards the stability of the contact manifold.
	//
	//Note that we do not need to set "refVertex". This is because we
	//guarantee that it will always be set to one of the reference face's
	//vertices in the previous loop by only looping through its edges.
	for(; curVertex <= lastVertex; ++curVertex){
		//Find the distance between the current vertex and a
		//and a vertex on the reference face and only keep
		//vertices that are inside the clipping region.
		curDist = pointPlaneDist(&curVertex->v, refVertex, &refNormal);
		if(curDist >= 0.f){
			//Store the vertex in "clipVertices".
			*clipVertex = *curVertex;
			++clipVertex;
			//Project the vertex onto the reference
			//face and store it in "loopVertices".
			pointPlaneProject(&curVertex->v, refVertex, &refNormal, &loopVertex->v);
			++loopVertex;
		}
	}
	//We need to set the last vertex pointer. The variable
	//"loopVertex" should point to the vertex after the last.
	lastVertex = loopVertex;

	//If there are four or less vertices,
	//we can simply keep all of them.
	if(lastVertex <= &loopVertices[CONTACT_MAX_POINTS]){
		contactPoint *curContact = cm->contacts;

		clipVertex = clipVertices;
		curVertex = loopVertices;
		//Add out contact points to the manifold.
		for(; curVertex < lastVertex; ++curVertex, ++clipVertex, ++curContact){
			curContact->key = clipVertex->key;
			curContact->rA = curVertex->v;
			curContact->rB = clipVertex->v;
			curContact->penetration = pointPlaneDist(&clipVertex->v, refVertex, &refNormal);

			++cm->numContacts;
		}

	//If there are more than four vertices, we'll
	//need to perform contact point reduction.
	}else{
		reduceContacts(loopVertices, lastVertex, clipVertices, cm);
	}


	memoryManagerGlobalFree(vertices);
}

/*
** If the axis of minimum penetration is realised
** by an edge pair, the contact point must be the
** closest points between the minimising edges.
*/
static void clipEdgeContact(const colliderHull *hullA, const colliderHull *hullB,
                            const colliderHullEdge *refEdge, const colliderHullEdge *incEdge,
                            contactManifold *cm){

	const vec3 *refStart = &hullA->vertices[refEdge->startVertexIndex];
	const vec3 *refEnd   = &hullA->vertices[refEdge->endVertexIndex];

	const vec3 *incStart = &hullB->vertices[incEdge->startVertexIndex];
	const vec3 *incEnd   = &hullB->vertices[incEdge->endVertexIndex];

	contactPoint *contact = &cm->contacts[0];
	float magnitudeSquared;


	#warning "Store the edges in contact->key!"
	//The edges may not necessarily be intersecting, so we'll
	//need to find the closest points on both line segments.
	segmentClosestPoints(refStart, refEnd, incStart, incEnd, &contact->rA, &contact->rB);

	//Find the collision's normal and magnitude.
	vec3SubtractVec3From(&contact->rB, &contact->rA, &cm->normal);
	magnitudeSquared = vec3NormVec3(&cm->normal);
	contact->penetration = sqrtf(magnitudeSquared);
	//Now that we know the penetration depth,
	//we can make the normal a unit vector.
	vec3MultiplyS(&cm->normal, fastInvSqrt(magnitudeSquared), &cm->normal);

	cm->numContacts = 1;
}