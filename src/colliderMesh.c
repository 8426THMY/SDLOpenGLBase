#include "colliderMesh.h"


#define COLLISION_PARALLEL_EPSILON 0.001


#include <float.h>
#include <math.h>

#include <string.h>

#include "utilMath.h"

#include "physicsCollider.h"


static return_t isMinkowskiFace(const vec3 *A, const vec3 *B, const vec3 *BA,
                                const vec3 *C, const vec3 *D, const vec3 *DC);
static float edgeDistance(const vec3 *pointA, const vec3 *edgeDirA,
                          const vec3 *pointB, const vec3 *edgeDirB,
                          const vec3 *centroidA);

static return_t noSeparatingFace(const colliderMesh *mesh1, const colliderMesh *mesh2, meshFaceData *faceData);
static return_t noSeparatingEdge(const colliderMesh *mesh1, const colliderMesh *mesh2, const vec3 *m1Centroid, meshEdgeData *edgeData);

static size_t findIncidentFace(const colliderMesh *mesh, const vec3 *refNormal);
static size_t clipPolygonAgainstFace(const vec3 *refVertex, const vec3 *refNormal,
                              const vec3 *currentPolygon, vec3 *tempPolygon, size_t numPoints);
static void clipFaceContact(const colliderMesh *mesh1, const colliderMesh *mesh2, const meshFaceData *fd, contactManifold *cm);
static void clipEdgeContact(const colliderMesh *mesh1, const colliderMesh *mesh2, const meshEdgeData *ed, contactManifold *cm);


//Load the collider from a file.
void colliderMeshLoad(physicsCollider *collider){
	const colliderMesh *mesh = (colliderMesh *)collider->data;

	//warning_deleted
	mesh = mesh;
}

//Calculate the collider's centroid and other such properties.
void colliderMeshSetupProperties(physicsCollider *collider){
	const colliderMesh *mesh = (colliderMesh *)collider->data;

	colliderMeshCalculateCentroid(mesh, &collider->centroid);
}


//Calculate the collider's inertia tensor relative to "centroid".
void colliderMeshCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]){
	const colliderMesh *mesh = (colliderMesh *)collider->data;
	const vec3 *curVertex = mesh->vertices;
	const float *curMass = mesh->massArray;
	const vec3 *lastVertex = &mesh->vertices[mesh->numVertices];

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
void colliderMeshCalculateCentroid(const colliderMesh *mesh, vec3 *centroid){
	//
}


//Return the index of the vertex farthest from the origin in the direction "dir".
vec3 *colliderMeshSupport(const colliderMesh *mesh, const vec3 *dir){
	vec3 *curVertex = mesh->vertices;
	vec3 *bestVertex = curVertex;
	float bestDistance = vec3DotVec3(curVertex, dir);
	const vec3 *lastVertex = &mesh->vertices[mesh->numVertices];

	++curVertex;
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


//Return whether two colliderMeshes are
//colloiding using the Separating Axis Theorem.
//Credit to Dirk Gregorius for his his GDC 2013
//presentation on Gauss map optimisations.
return_t colliderMeshCollidingSAT(const colliderMesh *mesh1, const colliderMesh *mesh2, const vec3 *m1Centroid, collisionData *cd){
	//If there are no separating axes, the meshes are colliding!
	return(
		noSeparatingFace(mesh1, mesh2, &cd->mesh1Face) &&
		noSeparatingFace(mesh2, mesh1, &cd->mesh2Face) &&
		noSeparatingEdge(mesh1, mesh2, m1Centroid, &cd->meshEdges)
	);
}

//Use the Sutherland-Hodgman clipping algorithm to generate a contact manifold.
#warning "Make sure this is correct."
void colliderMeshGenerateContactManifoldSHC(const colliderMesh *mesh1, const colliderMesh *mesh2, const collisionData *cd, contactManifold *cm){
	float bestFaceSeparation = (cd->mesh1Face.distance > cd->mesh2Face.distance) ? cd->mesh1Face.distance : cd->mesh2Face.distance;
	//If the deepest penetration occurs on an edge normal, clip using the edges.
	if(cd->meshEdges.distance > bestFaceSeparation){
		//b3CreateEdgeContact(output, edgeQuery, transform1, hull1, transform2, hull2);
		clipEdgeContact(mesh1, mesh2, &cd->meshEdges, cm);

	//Otherwise, clip using whichever face has the
	//greatest penetration depth as the reference face.
	}else{
		if(cd->mesh2Face.distance > cd->mesh1Face.distance){
			//b3CreateFaceContact(output, faceQuery2, transform2, hull2, transform1, hull1, true);
			clipFaceContact(mesh2, mesh1, &cd->mesh2Face, cm);
		}else{
			//b3CreateFaceContact(output, faceQuery1, transform1, hull1, transform2, hull2, false);
			clipFaceContact(mesh1, mesh2, &cd->mesh1Face, cm);
		}
	}
}


//If the arcs of the two faces when projected onto a Gauss Map
//intersect, they form an edge face of the Minkowski difference!
static return_t isMinkowskiFace(const vec3 *A, const vec3 *B, const vec3 *BA,
                                const vec3 *C, const vec3 *D, const vec3 *DC){

	const float BDC = vec3DotVec3(B, DC);

	//First, make sure that A and B are on opposite
	//sides of a plane with normal DC. Then
	if(vec3DotVec3(A, DC) * BDC < 0.f){
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
		const float CBA = vec3DotVec3(C, BA);

		//Check whether the two arcs are on the
		//same hemisphere of the unit sphere.
		if(CBA * BDC < 0.f){
			//If C and D are on opposite sides of the
			//plane with normal BA, the two edges form
			//a face on the Minkowski difference!
			return(CBA * vec3DotVec3(D, BA) < 0.f);
		}
	}

	return(0);
}

//Find the distance between two edges!
static float edgeDistance(const vec3 *pointA, const vec3 *edgeDirA,
                          const vec3 *pointB, const vec3 *edgeDirB,
                          const vec3 *centroidA){

	vec3 edgeNormal;
	vec3 offset;
	float edgeCrossLength;


	vec3CrossVec3(edgeDirA, edgeDirB, &edgeNormal);
	//The norm of the vector is the square of its magnitude.
	edgeCrossLength = vec3NormVec3(&edgeNormal);
	//If the two edges are parallel, we can exit early.
	if(edgeCrossLength < COLLISION_PARALLEL_EPSILON *
	                     (vec3NormVec3(edgeDirA) + vec3NormVec3(edgeDirB))){

		return(-INFINITY);
	}


	vec3MultiplyS(&edgeNormal, fastInvSqrt(edgeCrossLength), &edgeNormal);
	vec3SubtractVec3From(pointA, centroidA, &offset);
	//If the edge normal does not point from
	//object A to object B, we need to invert it.
	if(vec3DotVec3(&edgeNormal, &offset) < 0.f){
		vec3Negate(&edgeNormal, &edgeNormal);
	}


	vec3SubtractVec3From(pointB, pointA, &offset);

	//Return the distance between the edges.
	return(vec3DotVec3(&edgeNormal, &offset));
}


//Make sure there are no points on "mesh2" that are outside
//"mesh1" and record the face with the highest distance.
static return_t noSeparatingFace(const colliderMesh *mesh1, const colliderMesh *mesh2, meshFaceData *faceData){
	const size_t *curFace = mesh1->faces;
	const vec3 *curNormal = mesh1->normals;
	const size_t *lastFace = &mesh1->faces[mesh1->numFaces];
	size_t i = 0;

	//Initialise the face data.
	faceData->index = -1;
	faceData->distance = -INFINITY;

	for(; curFace < lastFace; ++curFace, ++curNormal, ++i){
		vec3 invNormal;
		float curDistance;

		vec3Negate(curNormal, &invNormal);
		//Search for a point on "mesh2" in the opposite
		//direction to the normal of the face on "mesh1".
		curDistance = vec3DotVec3(&mesh1->vertices[mesh1->edges[*curFace].startVertexIndex], colliderMeshSupport(mesh2, &invNormal));

		//If this is the farthest face we've found
		//so far, record its index and distance.
		if(curDistance > faceData->distance){
			faceData->index = i;
			faceData->distance = curDistance;

			//If the distance is greater than zero, we can
			//exit early as the meshes aren't colliding.
			if(curDistance > 0.f){
				return(0);
			}
		}
	}

	return(1);
}

static return_t noSeparatingEdge(const colliderMesh *mesh1, const colliderMesh *mesh2, const vec3 *m1Centroid, meshEdgeData *edgeData){
	colliderMeshEdge *edgeA = mesh1->edges;
	const colliderMeshEdge *lastEdgeA = &mesh1->edges[mesh1->numEdges];

	//Initialise the edge data.
	edgeData->edgeA = NULL;
	edgeData->edgeB = NULL;
	edgeData->distance = -INFINITY;

	for(; edgeA < lastEdgeA; ++edgeA){
		colliderMeshEdge *edgeB = mesh2->edges;
		const colliderMeshEdge *lastEdgeB = &mesh2->edges[mesh2->numEdges];

		//Find the normals of the two faces that use this edge.
		const vec3 *faceA = &mesh1->normals[edgeA->normalIndex];
		const vec3 *faceB = &mesh1->normals[edgeA->twinNormalIndex];
		const vec3 *startVertexA = &mesh1->vertices[edgeA->startVertexIndex];
		vec3 BA;
		//Get the normal perpendicular to faces A and B.
		vec3SubtractVec3From(startVertexA, &mesh1->vertices[edgeA->endVertexIndex], &BA);

		for(; edgeB < lastEdgeB; ++edgeB){
			//Find the normals of the two faces that use this edge.
			const vec3 *faceC = &mesh2->normals[edgeB->normalIndex];
			const vec3 *faceD = &mesh2->normals[edgeB->twinNormalIndex];
			const vec3 *startVertexB = &mesh2->vertices[edgeB->startVertexIndex];
			vec3 DC;
			//Get the normal perpendicular to faces C and D.
			vec3SubtractVec3From(startVertexB, &mesh2->vertices[edgeB->endVertexIndex], &DC);

			//We only need to compare two edges if they
			//form a face on the Minkowski difference.
			if(isMinkowskiFace(faceA, faceB, &BA, faceC, faceD, &DC)){
				const float curDistance = edgeDistance(startVertexA, &BA, startVertexB, &DC, m1Centroid);

				//If this is the farthest edge pair we've found
				//so far, record the edges' indices and distance.
				if(curDistance > edgeData->distance){
					edgeData->edgeA = edgeA;
					edgeData->edgeB = edgeB;
					edgeData->distance = curDistance;

					//If the distance is greater than zero, we can
					//exit early as the meshes aren't colliding.
					if(curDistance > 0.f){
						return(0);
					}
				}
			}
		}
	}

	return(1);
}


//Using the normal of the reference face on one object, find the incident face on another.
static size_t findIncidentFace(const colliderMesh *mesh, const vec3 *refNormal){
	const vec3 *curNormal = mesh->normals;
	size_t bestFace = 0;
	float bestDistance = INFINITY;

	size_t i;
	for(i = 0; i < mesh->numFaces; ++i, ++curNormal){
		const float curDistance = vec3DotVec3(curNormal, refNormal);
		//If the current face's normal is farther in the direction
		//of "refNormal" than our current best, store it instead.
		if(curDistance < bestDistance){
			bestFace = i;
			bestDistance = curDistance;
		}
	}

	return(bestFace);
}

//Clip a polygon against a face.
static size_t clipPolygonAgainstFace(const vec3 *refVertex, const vec3 *refNormal,
                                     const vec3 *currentPolygon, vec3 *tempPolygon, size_t numPoints){

	//Record the new number of points we're keeping.
	size_t newNumPoints = 0;

	const vec3 *startVertex = currentPolygon;
	float startDist;
	const vec3 *endVertex = &currentPolygon[numPoints - 1];

	vec3 planeOffset;
	//Find the distance from the first vertex of the
	//current incident edge to the clipping plane!
	vec3SubtractVec3From(startVertex, refVertex, &planeOffset);
	startDist = vec3DotVec3(&planeOffset, refNormal);


	//Note that we start at the end of the array.
	while(endVertex > currentPolygon){
		float endDist;

		--endVertex;

		//Find the distance from the second vertex of the
		//current incident edge to the clipping plane!
		vec3SubtractVec3From(endVertex, refVertex, &planeOffset);
		endDist = vec3DotVec3(&planeOffset, refNormal);

		if(endDist < 0.f){
			//If the end vertex is outside the clipping region but
			//the start vertex is inside it, clip the end vertex!
			if(startDist >= 0.f){
				vec3Lerp(startVertex, endVertex, startDist / (startDist - endDist), &tempPolygon[newNumPoints]);
				++newNumPoints;
				//Check if the array is full.
				if(newNumPoints == MAX_CONTACTS_NUM){
					return(newNumPoints);
				}
			}

		//If the end vertex is inside the clipping region but
		//the start vertex is outside it, clip the start vertex!
		}else if(startDist < 0.f){
			vec3Lerp(startVertex, endVertex, startDist / (startDist - endDist), &tempPolygon[newNumPoints]);
			++newNumPoints;
			//Check if the array is full.
			if(newNumPoints == MAX_CONTACTS_NUM){
				return(newNumPoints);
			}
			//We also need to add the end vertex.
			tempPolygon[newNumPoints] = *endVertex;
			++newNumPoints;
			//Check if the array is full. Again.
			if(newNumPoints == MAX_CONTACTS_NUM){
				return(newNumPoints);
			}
		}

		//The next start vertex will be this end vertex, so
		//there's no need to calculate all that stuff again.
		startVertex = endVertex;
		startDist = endDist;
	} while(numPoints > 0);


	return(newNumPoints);
}

//Clip the vertices of the face on "mesh2" against the normal of
//the face on "mesh1" (and the normals of its surrounding faces).
static void clipFaceContact(const colliderMesh *mesh1, const colliderMesh *mesh2, const meshFaceData *fd, contactManifold *cm){
	const vec3 *refNormal = &mesh1->normals[fd->index];
	//Find the face on mesh 2 whose normal is most opposite the reference face's.
	const size_t incFaceIndex = findIncidentFace(mesh2, refNormal);


	vec3 polygonData1[MAX_CONTACTS_NUM];
	vec3 *currentPolygon = polygonData1;
	vec3 polygonData2[MAX_CONTACTS_NUM];
	vec3 *tempPolygon = polygonData2;
	size_t numPoints = 0;

	size_t curIndex = incFaceIndex;
	//Add all of the incident face's vertices to "polygonData1".
	do {
		const colliderMeshEdge *curIncEdge = &mesh2->edges[curIndex];

		//If the incident face is not this edge's twin, add the vertices normally.
		if(curIncEdge->normalIndex == curIndex){
			curIndex = curIncEdge->nextIndex;

			*currentPolygon = mesh2->vertices[curIncEdge->startVertexIndex];
			++currentPolygon;
			*currentPolygon = mesh2->vertices[curIncEdge->endVertexIndex];
			++currentPolygon;

		//Otherwise, we'll need to add them in reverse order.
		}else{
			curIndex = curIncEdge->twinNextIndex;

			*tempPolygon = mesh2->vertices[curIncEdge->endVertexIndex];
			++tempPolygon;
			*tempPolygon = mesh2->vertices[curIncEdge->startVertexIndex];
			++tempPolygon;
		}
		//We always add two contacts.
		numPoints += 2;
	} while(curIndex != incFaceIndex);


	//Points to the beginning of the array currently in use.
	currentPolygon = polygonData1;
	//Points to the beginning of the temporary array.
	tempPolygon = polygonData2;

	//The current reference vertex is stored outside the
	//loop so we can use it for the reference face itself.
	vec3 *refVertex;

	const size_t startFaceIndex = mesh1->faces[fd->index];
	curIndex = startFaceIndex;

	//Loop through every face surrounding the reference
	//face and clip the incident face against it!
	do {
		vec3 *swapPolygon = currentPolygon;

		const colliderMeshEdge *curRefEdge = &mesh1->edges[curIndex];
		vec3 *curFaceNormal;

		//It doesn't matter which vertex we use.
		refVertex = &mesh1->vertices[curRefEdge->startVertexIndex];
		//Make sure we get the twin face's normal and the next edge correct.
		if(curRefEdge->normalIndex == curIndex){
			curIndex = curRefEdge->nextIndex;
			curFaceNormal = &mesh1->normals[curRefEdge->twinNormalIndex];
		}else{
			curIndex = curRefEdge->twinNextIndex;
			curFaceNormal = &mesh1->normals[curRefEdge->normalIndex];
		}

		//Clip the polygon against the current face!
		numPoints = clipPolygonAgainstFace(refVertex, curFaceNormal, currentPolygon, tempPolygon, numPoints);
		//If there are no points left, we can exit the function.
		if(numPoints == 0){
			return;
		}

		//Swap the arrays!
		currentPolygon = tempPolygon;
		tempPolygon = swapPolygon;
	} while(curIndex != startFaceIndex);


	const vec3 *incVertex = currentPolygon;
	const vec3 *lastVertex = &currentPolygon[numPoints];
	numPoints = 0;
	//Clip the polygon against the reference face,
	//storing the points in our collision manifold.
	for(; incVertex < lastVertex; ++incVertex){
		vec3 planeOffset;
		float vertDist;

		//Find the distance from the second vertex of the
		//current incident edge to the clipping plane!
		vec3SubtractVec3From(incVertex, refVertex, &planeOffset);
		vertDist = vec3DotVec3(&planeOffset, refNormal);

		//Only keep vertices that are inside the clipping region!
		if(vertDist > 0.f){
			cm->points[numPoints] = *incVertex;
			++numPoints;
		}
		#warning "In his 2015 GDC presentation, Dirk Gregorius suggests projecting the valid contact points onto the reference face, explaining that it improves coherance. Is this correct?"
	}

	//We'll need to remember the number of contact points we've stored.
	cm->numPoints = numPoints;
}

#warning "Implement this."
//According to Gregorius, If the axis of minimum penetration is realized by an edge
//pair, the contact point must be the closest points between the minimizing edges.
static void clipEdgeContact(const colliderMesh *mesh1, const colliderMesh *mesh2, const meshEdgeData *ed, contactManifold *cm){
	const colliderMeshEdge *e1 = ed->edgeA;
	const colliderMeshEdge *e2 = ed->edgeB;

	const vec3 *p1 = &mesh1->vertices[e1->startVertexIndex];
	const vec3 *p2 = &mesh1->vertices[e1->endVertexIndex];

	const vec3 *q1 = &mesh2->vertices[e2->startVertexIndex];
	const vec3 *q2 = &mesh2->vertices[e2->endVertexIndex];

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