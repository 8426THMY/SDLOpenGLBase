#include "physicsCollider.h"


#include <stdio.h>
#include <string.h>


typedef void (*colliderCalculateInertiaPrototype)(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]);
//Create a jump table so we can calculate a collider's inertia tensor depending on its type.
const static colliderCalculateInertiaPrototype colliderCalculateInertiaTable[NUM_COLLIDER_TYPES] = {
	colliderMeshCalculateInertia
};

void colliderCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]){
	//Call the collider's function from the jump table.
	colliderCalculateInertiaTable[collider->type](collider, centroid, inertiaTensor);
}


/*void colliderCalculateCentreOfGeometry(const physicsCollider *collider, vec3 *centroid){
	memset(centroid, 0, sizeof(*centroid));

	const vec3 *curVertex = collider->vertices;
	const float *curMass = collider->massArray;
	const vec3 *lastVertex = &collider->vertices[collider->numVertices];
	if(curMass != NULL){
		//Add each vertex's contribution to the collider's centroid.
		for(; curVertex < lastVertex; ++curVertex, ++curMass){
			const float massValue = *curMass;

			centroid->x += curVertex->x * massValue;
			centroid->y += curVertex->y * massValue;
			centroid->z += curVertex->z * massValue;
		}

		//Multiply the accumulated positions by the inverse mass to calculate the centroid.
		vec3MultiplyS(centroid, collider->inverseMass, centroid);
	}else{
		//If the vertices are not weighted, we can save some multiplications.
		for(; curVertex < lastVertex; ++curVertex){
			centroid->x += curVertex->x;
			centroid->y += curVertex->y;
			centroid->z += curVertex->z;
		}

		//If the vertices are not weighted, divide by the total number of them.
		vec3DivideByS(centroid, collider->numVertices, centroid);
	}
}*/