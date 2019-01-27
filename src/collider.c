#include "collider.h"


/*typedef void (*colliderCalculateInertiaPrototype)(const void *collider, float inertia[6]);
//Create a jump table so we can calculate a collider's inertia tensor depending on its type.
static const colliderCalculateInertiaPrototype colliderCalculateInertiaTable[COLLIDER_NUM_TYPES] = {
	colliderHullCalculateInertia, NULL
};*/

typedef return_t (*colliderCheckCollisionPrototype)(const void *cA, const void *cB, contactSeparation *cs, contactManifold *cm);
//Create a jump table so we can check collision between two colliders depending on their types.
static const colliderCheckCollisionPrototype colliderCheckCollisionTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES] = {
	{
		colliderHullCollidingSAT
	}
};


//These functions merely act as interfaces to the jump tables.
/*void colliderCalculateInertia(const collider *c, float inertia[6]){
	colliderCalculateInertiaTable[c->type]((void *)(&c->data), inertia);
}*/

return_t colliderCheckCollision(const collider *cA, const collider *cB, contactSeparation *cs, contactManifold *cm){
	return(colliderCheckCollisionTable[cA->type][cB->type]((void *)(&cA->data), cs, (void *)(&cB->data), cm));
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
		vec3MultiplyS(centroid, collider->invMass, centroid);
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