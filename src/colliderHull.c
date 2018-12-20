#include "colliderHull.h"


#define COLLISION_PARALLEL_EPSILON 0.001


#include <math.h>

#include <string.h>

#include "utilMath.h"

#include "memoryManager.h"
#include "physicsCollider.h"


//Stores the index of a face and its distance
//from the closest point on another object.
typedef struct hullFaceData {
	colliderHullFace index;
	float distance;
} hullFaceData;

//Stores the index of an edge pair
//and their distance from each other.
typedef struct colliderHullEdge colliderHullEdge;
typedef struct hullEdgeData {
	colliderHullEdge *edge1;
	colliderHullEdge *edge2;
	float distance;
} hullEdgeData;

//Stores the collision information
//returned by the Separating Axis Theorem.
typedef struct collisionData {
	hullFaceData hull1Face;
	hullFaceData hull2Face;
	hullEdgeData hullEdges;
} collisionData;


static void clipManifoldSHC(const colliderHull *hull1, const colliderHull *hull2, const collisionData *cd, contactManifold *cm);
static return_t isMinkowskiFace(const colliderHull *hull1, const colliderHull *hull2,
                                const colliderHullEdge *e1, const vec3 *invEdge1,
                                const colliderHullEdge *e2, const vec3 *invEdge2);
static float edgeDistSquared(const vec3 *p1, const vec3 *edgeDir1,
                             const vec3 *p2, const vec3 *edgeDir2,
                             const vec3 *centroid);

static return_t noSeparatingFace(const colliderHull *hull1, const colliderHull *hull2, hullFaceData *faceData);
static return_t noSeparatingEdge(const colliderHull *hull1, const colliderHull *hull2, const vec3 *h1Centroid, hullEdgeData *edgeData);

//static uint_least16_t findIncidentFace(const colliderHull *hull, const vec3 *refNormal);
static void clipFaceContact(const colliderHull *hull1, const colliderHull *hull2, const uint_least16_t refIndex, contactManifold *cm);
static void clipEdgeContact(const colliderHull *hull1, const colliderHull *hull2, const hullEdgeData *ed, contactManifold *cm);


#warning "What if we aren't using the global memory manager?"


//Load the collider from a file.
void colliderHullLoad(physicsCollider *collider){
	const colliderHull *hull = (colliderHull *)collider->data;

	//warning_deleted
	hull = hull;
}

//Calculate the collider's centroid and other such properties.
void colliderHullSetupProperties(physicsCollider *collider){
	const colliderHull *hull = (colliderHull *)collider->data;

	colliderHullCalculateCentroid(hull, &collider->centroid);
}


//Calculate the collider's inertia tensor relative to "centroid".
void colliderHullCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]){
	const colliderHull *hull = (colliderHull *)collider->data;
	const vec3 *curVertex = hull->vertices;
	const float *curMass = hull->massArray;
	const vec3 *lastVertex = &hull->vertices[hull->numVertices];

	memset(inertiaTensor, 0, 24);

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

			inertiaTensor[0] += (yy + zz) * massValue;
			inertiaTensor[1] += (xx + zz) * massValue;
			inertiaTensor[2] += (xx + yy) * massValue;
			inertiaTensor[3] -= x * y * massValue;
			inertiaTensor[4] -= x * z * massValue;
			inertiaTensor[5] -= y * z * massValue;
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

			inertiaTensor[0] += yy + zz;
			inertiaTensor[1] += xx + zz;
			inertiaTensor[2] += xx + yy;
			inertiaTensor[3] -= x * y;
			inertiaTensor[4] -= x * z;
			inertiaTensor[5] -= y * z;
		}
	}
}

//Calculate the collider's centre of gravity, storing the result in "centroid".
void colliderHullCalculateCentroid(const colliderHull *hull, vec3 *centroid){
	//
}


//Return the index of the vertex farthest from the origin in the direction "dir".
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
** Return whether or not two hulls are colliding using
** the Separating Axis Theorem. Special thanks to Dirk
** Gregorius for his GDC 2013 presentation on utilising
** Gauss map and Minkowski space to optimise this!
*/
return_t colliderHullCollidingSAT(const colliderHull *hull1, const colliderHull *hull2, const vec3 *h1Centroid, contactManifold *cm){
	collisionData cd;
	//Stores a reference to the feature of minimum separation.
	#warning "We still need to do something with this."
	void *feature = NULL;

	if(noSeparatingFace(hull1, hull2, &cd.hull1Face)){
		if(noSeparatingFace(hull2, hull1, &cd.hull2Face)){
			if(noSeparatingEdge(hull1, hull2, h1Centroid, &cd.hullEdges)){
				//If there are no separating axes, the hulls are colliding and
				//we can clip the colliding area to create a contact manifold.
				clipManifoldSHC(hull1, hull2, &cd, cm);

				return(1);
			}
		}
	}

	return(0);
}

/*
** Using the collision information generated from the
** Separating Axis Theorem, clip the penetrating area
** using the Sutherland-Hodgman clipping algorithm
** to create the contact manifold.
*/
#warning "Make sure this is correct."
static void clipManifoldSHC(const colliderHull *hull1, const colliderHull *hull2, const collisionData *cd, contactManifold *cm){
	const float bestFaceSeparation = (cd->hull1Face.distance >= cd->hull2Face.distance) ? cd->hull1Face.distance : cd->hull2Face.distance;

	//If the deepest penetration occurs on an edge normal, clip using the edges.
	if(cd->hullEdges.distance > bestFaceSeparation){
		clipEdgeContact(hull1, hull2, &cd->hullEdges, cm);

	//Otherwise, clip using whichever face has the
	//greatest penetration depth as the reference face.
	}else{
		if(cd->hull1Face.distance >= cd->hull2Face.distance){
			clipFaceContact(hull1, hull2, cd->hull1Face.index, cm);
		}else{
			clipFaceContact(hull2, hull1, cd->hull2Face.index, cm);
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
static return_t isMinkowskiFace(const colliderHull *hull1, const colliderHull *hull2,
                                const colliderHullEdge *e1, const vec3 *invEdge1,
                                const colliderHullEdge *e2, const vec3 *invEdge2){

	//Note that in this function, the following terminology is used:
	//A - Normal of edge A's face.
	//B - Normal of edge A's twin face.
	//C - Normal of edge B's face.
	//D - Normal of edge B's twin face.

	const float BDC = vec3DotVec3(&hull1->normals[e1->twinFaceIndex], invEdge2);

	//First, make sure that A and B are on opposite
	//sides of a plane with normal DC. Then
	if(vec3DotVec3(&hull1->normals[e1->faceIndex], invEdge2) * BDC < 0.f){
		//We don't have to invert the vectors C and
		//D here, as the following identities hold:
		//
		//-dot(C, BA) = dot(-C, BA)
		//x * y = -x * -y
		//dot(C, BA) * dot(D, BA) = dot(-C, BA) * dot(-D, BA)
		//
		//This does mean, however, that the hemisphere check
		//below must check for a result less than zero rather
		//than one that is greater than zero.
		const float CBA = vec3DotVec3(&hull2->normals[e2->faceIndex], invEdge1);

		//Check whether the two arcs are on the
		//same hemisphere of the unit sphere.
		if(CBA * BDC < 0.f){
			//If C and D are on opposite sides of the
			//plane with normal BA, the two edges form
			//a face on the Minkowski difference!
			return(CBA * vec3DotVec3(&hull2->normals[e2->twinFaceIndex], invEdge1) < 0.f);
		}
	}

	return(0);
}

//Return the squared distance between two edges.
static float edgeDistSquared(const vec3 *p1, const vec3 *edgeDir1,
                             const vec3 *p2, const vec3 *edgeDir2,
                             const vec3 *centroid){

	vec3 edgeNormal;
	vec3 offset;
	float edgeCrossLength;


	vec3CrossVec3(edgeDir1, edgeDir2, &edgeNormal);
	//The norm of the vector is the square of its magnitude.
	edgeCrossLength = vec3NormVec3(&edgeNormal);
	//If the two edges are parallel, we can exit early.
	if(edgeCrossLength < COLLISION_PARALLEL_EPSILON *
	                     (vec3NormVec3(edgeDir1) + vec3NormVec3(edgeDir2))){

		return(-INFINITY);
	}


	vec3MultiplyS(&edgeNormal, fastInvSqrt(edgeCrossLength), &edgeNormal);
	vec3SubtractVec3From(p1, centroid, &offset);
	//If the edge normal does not point from
	//object A to object B, we need to invert it.
	if(vec3DotVec3(&edgeNormal, &offset) < 0.f){
		vec3Negate(&edgeNormal, &edgeNormal);
	}


	vec3SubtractVec3From(p2, p1, &offset);

	//Return the distance between the edges.
	return(vec3DotVec3(&edgeNormal, &offset));
}


/*
** For each face on "hull1", check the points on "hull2"
** farthest in the direction opposite the face's normal.
** If the penetration depth is positive, we have found a
** separating axis and the shapes are not colliding.
** Otherwise, we can assume that they might be colliding.
**
** We store the index of the least separating face in
** the variable "faceData".
*/
static return_t noSeparatingFace(const colliderHull *hull1, const colliderHull *hull2, hullFaceData *faceData){
	const colliderHullFace *curFace = hull1->faces;
	const vec3 *curNormal = hull1->normals;
	const colliderHullFace *lastFace = &hull1->faces[hull1->numFaces];

	uint_least16_t i;
	for(i = 0; curFace < lastFace; ++i){
		const vec3 invNormal = {
			.x = -curNormal->x,
			.y = -curNormal->y,
			.z = -curNormal->z
		};
		//First, find the point on "hull2" farthest in the
		//direction opposite the current face's normal.
		//Then, find the distance between the two.
		const float curDistance = pointPlaneDistSquared(
			&hull1->vertices[hull1->edges[*curFace].startVertexIndex],
			curNormal,
			colliderHullSupport(hull2, &invNormal)
		);

		//If the distance is greater than zero, we can
		//exit early as the hulls aren't colliding.
		if(curDistance > 0.f){
			return(0);

		//If this is the least separating face we've
		//found so far, record its index and distance.
		}else if(curDistance > faceData->distance){
			faceData->index = i;
			faceData->distance = curDistance;
		}

		++curFace;
		++curNormal;
	}

	return(1);
}

/*
** After checking for any separating axes between the faces
** of the two shapes, we must now check between their edges.
**
** Luckily, we do not need to check the distance between
** every edge pair due to the optimisations outlined by
** Dirk Gregorius in his GDC 2013 presentation.
**
** We store the least separating edge pair in "edgeData".
*/
static return_t noSeparatingEdge(const colliderHull *hull1, const colliderHull *hull2, const vec3 *h1Centroid, hullEdgeData *edgeData){
	colliderHullEdge *edge1 = hull1->edges;
	const colliderHullEdge *lastEdge1 = &hull1->edges[hull1->numEdges];
	const colliderHullEdge *lastEdge2 = &hull2->edges[hull2->numEdges];

	for(; edge1 < lastEdge1; ++edge1){
		colliderHullEdge *edge2 = hull2->edges;
		const vec3 *startVertex1 = &hull1->vertices[edge1->startVertexIndex];
		vec3 invEdge1;
		//Get the inverse of edge 1's normal.
		vec3SubtractVec3From(startVertex1, &hull1->vertices[edge1->endVertexIndex], &invEdge1);

		for(; edge2 < lastEdge2; ++edge2){
			const vec3 *startVertex2 = &hull2->vertices[edge2->startVertexIndex];
			vec3 invEdge2;
			//Get the inverse of edge 2's normal
			vec3SubtractVec3From(startVertex2, &hull2->vertices[edge2->endVertexIndex], &invEdge2);

			//We only need to compare two edges if they
			//form a face on the Minkowski difference.
			if(isMinkowskiFace(hull1, hull2, edge1, &invEdge1, edge2, &invEdge2)){
				const float curDistance = edgeDistSquared(startVertex1, &invEdge1, startVertex2, &invEdge2, h1Centroid);

				//If the distance is greater than zero, we can
				//exit early as the hulls aren't colliding.
				if(curDistance > 0.f){
					return(0);

				//If this is the least separating edge pair we've
				//found so far, record the edges and their distance.
				}else if(curDistance > edgeData->distance){
					edgeData->edge1 = edge1;
					edgeData->edge2 = edge2;
					edgeData->distance = curDistance;
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
** Clip the vertices of the incident face by the normals of the
** faces adjacent to the reference face using the Sutherland-Hodgman
** clipping algorithm. The reference face is always assumed to be on
** "hull1" and the incident face is always assumed to be on "hull2".
*/
static void clipFaceContact(const colliderHull *hull1, const colliderHull *hull2, const colliderHullFace refIndex, contactManifold *cm){
	//Meshes store the number of edges that their largest faces have,
	//so we can use this to preallocate enough memory for any face.
	//We will need to store two arrays that are both large enough
	//to store twice the maximum number of edges that a face has, as
	//each edge can intersect two faces at most. This means that, in
	//practice, we are really storing four times the number of edges.
	vec3 *vertices = memoryManagerGlobalAlloc(hull2->maxFaceVertices * 2 * sizeof(*vertices));
	if(vertices == NULL){
		/** MALLOC FAILED **/
	}

	//Points to the beginning of the
	//array of vertices to loop through.
	vec3 *loopVertices = vertices;
	//Points to the beginning of the
	//array of vertices to loop through.
	vec3 *clipVertices = &vertices[hull2->maxFaceVertices];

	const vec3 *refNormal = &hull1->normals[refIndex];
	//Using the normal of the reference face, find the face
	//on the incident hull whose normal is most opposite it.
	colliderHullFace incIndex = findIncidentFace(hull2, refNormal);

	const vec3 *refVertex;
	const vec3 *curVertex;
	vec3 *lastVertex = loopVertices;
	const colliderHullEdge *curEdge = &hull2->edges[hull2->faces[incIndex]];
	const colliderHullEdge *startEdge = curEdge;
	//Store the incident face's vertices in the array.
	for(;;){
		//Edges are shared by both faces that use
		//them, so we need to check whether or not
		//the incident face is this edge's twin
		//face to make sure we get the right vertex.
		if(curEdge->faceIndex == incIndex){
			*lastVertex = hull2->vertices[curEdge->startVertexIndex];
			curEdge = &hull2->edges[curEdge->nextIndex];
		}else{
			*lastVertex = hull2->vertices[curEdge->endVertexIndex];
			curEdge = &hull2->edges[curEdge->twinNextIndex];
		}

		//If we've finished adding all of the vertices, break the loop.
		if(curEdge == startEdge){
			break;
		}
		//Otherwise, add the next vertex. We only increment after
		//checking if the loop can be ended to ensure that "lastVertex"
		//points to the last vertex and not the one before the last.
		++lastVertex;
	}


	curEdge = &hull1->edges[hull1->faces[refIndex]];
	startEdge = curEdge;
	//For each face surrounding the reference face,
	//clip each incident vertex against its normal.
	do {
		float curDist;
		const vec3 *nextVertex;
		vec3 *clipVertex = clipVertices;

		const vec3 *adjNormal;
		//Edges are shared by both faces that use
		//them, so we need to check whether or not
		//the reference face is this edge's twin
		//face to make sure we get the right data.
		if(curEdge->faceIndex == refIndex){
			refVertex = &hull1->vertices[curEdge->startVertexIndex];
			adjNormal = &hull1->normals[curEdge->twinFaceIndex];
			curEdge = &hull1->edges[curEdge->nextIndex];
		}else{
			refVertex = &hull1->vertices[curEdge->startVertexIndex];
			adjNormal = &hull1->normals[curEdge->nextIndex];
			curEdge = &hull1->edges[curEdge->twinFaceIndex];
		}

		curVertex = lastVertex;
		curDist = pointPlaneDistSquared(refVertex, adjNormal, curVertex);
		nextVertex = loopVertices;
		//For each edge on the incident face, clip
		//it against the current reference face.
		do {
			const float nextDist = pointPlaneDistSquared(refVertex, adjNormal, nextVertex);
			//The starting vertex is inside the clipping
			//region, so we will have to keep it.
			if(curDist <= 0.f){
				*clipVertex = *curVertex;
				++clipVertex;

				//The edge leaves the clipping region, so we
				//need to find and keep the intersection point.
				if(nextDist > 0.f){
					vec3Lerp(curVertex, nextVertex, curDist / (curDist - nextDist), clipVertex);
					++clipVertex;
				}

			//If the starting vertex is outside the clipping
			//region and the ending vertex is inside it, we
			//need to find and keep the intersection point.
			}else if(nextDist <= 0.f){
				vec3Lerp(curVertex, nextVertex, curDist / (curDist - nextDist), clipVertex);
				++clipVertex;
			}

			//We already know the next vertex and its
			//distance, so set the current vertex to it.
			curVertex = nextVertex;
			curDist = nextDist;
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


	//Clip the remaining vertices against the actual reference face.
	//Unlike before, we won't keep any intersection points, as they
	//do not contribute towards the stability of the contact manifold.
	//
	//Note that we do not need to set "refVertex". This is because we
	//guarantee that it will always be set to one of the reference face's
	//vertices in the previous loop by only looping through its edges.
	curVertex = loopVertices;
	for(; curVertex <= lastVertex; ++curVertex){
		//Find the distance between the current vertex and a
		//vertex on the reference face and only keep vertices
		//that are inside the clipping region.
		/*if(pointPlaneDistSquared(refVertex, invNormal, curVertex) >= 0.f){

			++cm->numPoints;
			if(cm->numPoints == COLLISION_MAX_CONTACTS){
				return;
			}
		}*/
	}
	//Robust Contact Creation for Physics Simulations, p. 93
	#warning "Dirk Gregorius suggests projecting the valid contact points onto the reference face as it improves coherance. Is this correct?"
	#warning "He also suggests limiting manifolds to the four points that give the greatest area. This requires the previous suggestion."
}

#warning "Implement this."
/*
** If the axis of minimum penetration is realised
** by an edge pair, the contact point must be the
** closest points between the minimising edges.
**
** Yet again, special thanks to Dirk Gregorius!
*/
static void clipEdgeContact(const colliderHull *hull1, const colliderHull *hull2, const hullEdgeData *ed, contactManifold *cm){
	const colliderHullEdge *e1 = ed->edge1;
	const colliderHullEdge *e2 = ed->edge2;

	const vec3 *p1 = &hull1->vertices[e1->startVertexIndex];
	const vec3 *p2 = &hull1->vertices[e1->endVertexIndex];

	const vec3 *q1 = &hull2->vertices[e2->startVertexIndex];
	const vec3 *q2 = &hull2->vertices[e2->endVertexIndex];

	//In a perfect world, we would find the point at which the two edges intersect.
	//Realistically, however, we must find the closest points on both line segments
	//and get the point halfway between the two, as they probably do not intersect.
	//
	/*
	// Create an edge contact if the minimum separation distance comes from a
	// edge combination.
	const b3HalfEdge* edge1 = hull1->GetEdge(input.index1);
	const b3HalfEdge* twin1 = hull1->GetEdge(input.index1 + 1);

	b3Vec3 P1 = transform1 * hull1->GetVertex(edge1->origin);
	b3Vec3 Q1 = transform1 * hull1->GetVertex(twin1->origin);
	b3Vec3 E1 = Q1 - P1;

	const b3HalfEdge* edge2 = hull2->GetEdge(input.index2);
	const b3HalfEdge* twin2 = hull2->GetEdge(input.index2 + 1);

	b3Vec3 P2 = transform2 * hull2->GetVertex(edge2->origin);
	b3Vec3 Q2 = transform2 * hull2->GetVertex(twin2->origin);
	b3Vec3 E2 = Q2 - P2;

	b3Vec3 normal = b3Normalize(b3Cross(Q1 - P1, Q2 - P2));

	b3Vec3 C2C1 = transform2.translation - transform1.translation;

	b3ContactID id;

	if (b3Dot(normal, C2C1) < B3_ZERO) {
		// Flip
		output.normal = -normal;
		id.featurePair.inEdge1 = input.index2;
		id.featurePair.outEdge1 = input.index2 + 1;

		id.featurePair.inEdge2 = input.index1 + 1;
		id.featurePair.outEdge2 = input.index1;
	}
	else {
		output.normal = normal;

		id.featurePair.inEdge1 = input.index1;
		id.featurePair.outEdge1 = input.index1 + 1;

		id.featurePair.inEdge2 = input.index2 + 1;
		id.featurePair.outEdge2 = input.index2;
	}

	// Compute the closest points between the two edges.
	b3Vec3 C1, C2;
	b3ClosestPointsSegmentSegment(P1, Q1, P2, Q2, C1, C2);
	{
		b3Vec3 P2P1 = P1 - P2;

		b3Vec3 E1 = Q1 - P1;
		b3Vec3 E2 = Q2 - P2;

		r32 D1 = b3LenSq(E1);
		r32 D2 = b3LenSq(E2);

		r32 D12 = b3Dot(E1, E2);
		r32 DE1P1 = b3Dot(E1, P2P1);
		r32 DE2P1 = b3Dot(E2, P2P1);

		r32 DNM = D1 * D2 - D12 * D12;

		// Get the two fractions.
		r32 F1 = (D12 * DE2P1 - DE1P1 * D2) / DNM;
		r32 F2 = (D12 * F1 + DE2P1) / D2;

		C1 = P1 + F1 * E1;
		C2 = P2 + F2 * E2;
	}
	// Use the point between them.
	b3Vec3 position = B3_HALF * (C1 + C2);
	output.AddEntry(position, input.distance, id);
	*/
}