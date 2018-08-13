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

	size_t i;
	//Accumulate the weighted positions of each vertex.
	for(i = 0; i < collider->numVertices; ++i){
		const vec3 *curVertex = &collider->vertices[i];

		//Add this vertex's contribution to the collider's centroid.
		if(collider->massArray != NULL){
			const float curMass = collider->massArray[i];

			centroid->x += curVertex->x * curMass;
			centroid->y += curVertex->y * curMass;
			centroid->z += curVertex->z * curMass;

		//If the vertices are not weighted, we can save some multiplications.
		}else{
			centroid->x += curVertex->x;
			centroid->y += curVertex->y;
			centroid->z += curVertex->z;
		}
	}

	//Multiply the accumulated positions by the inverse mass to calculate the centroid.
	if(collider->massArray != NULL){
		vec3MultiplyS(centroid, collider->inverseMass, centroid);

	//If the vertices are not weighted, divide by the total number of them.
	}else{
		vec3DivideByS(centroid, collider->numVertices, centroid);
	}
}*/