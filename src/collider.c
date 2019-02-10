#include "collider.h"


void (*colliderInstantiateTable[COLLIDER_NUM_TYPES])(void *c, const void *cBase) = {
	colliderHullInstantiate
};

void (*colliderDeleteInstanceTable[COLLIDER_NUM_TYPES])(void *c) = {
	colliderHullDeleteInstance
};

void (*colliderUpdateTable[COLLIDER_NUM_TYPES])(void *c, const void *cBase, const transformState *trans, colliderAABB *aabb) = {
	colliderHullUpdate
};

void (*colliderDeleteTable[COLLIDER_NUM_TYPES])(void *c) = {
	colliderHullDelete
};


//Ordinary colliders are used as a base for instances.
void colliderInit(collider *c, const colliderType_t type){
	c->type = type;
}

//Instances share the components of bases, but can be updated.
void colliderInstantiate(collider *c, const collider *cBase){
	c->type = cBase->type;
	colliderInstantiateTable[cBase->type]((void *)(&c->data), (const void *)(&cBase->data));
}


//Update a collider instance and return its new bounding box.
void colliderUpdate(collider *c, const collider *cBase, const transformState *trans, colliderAABB *aabb){
	colliderUpdateTable[c->type]((void *)(&c->data), (void *)(&cBase->data), trans, aabb);
}


void colliderDeleteInstance(collider *c){
	colliderDeleteInstanceTable[c->type]((void *)(&c->data));
}

void colliderDelete(collider *c){
	colliderDeleteTable[c->type]((void *)(&c->data));
}

/*typedef void (*colliderCalculateInertiaPrototype)(const void *c, float inertia[6]);
//Create a jump table so we can calculate a collider's inertia tensor depending on its type.
static const colliderCalculateInertiaPrototype colliderCalculateInertiaTable[COLLIDER_NUM_TYPES] = {
	colliderHullCalculateInertia, NULL
};*/


//These functions merely act as interfaces to the jump tables.
/*void colliderCalculateInertia(const collider *c, float inertia[6]){
	colliderCalculateInertiaTable[c->type]((void *)(&c->data), inertia);
}*/


/*void colliderCalculateCentreOfGeometry(const physicsCollider *collider, vec3 *centroid){
	memset(centroid, 0.f, sizeof(*centroid));

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
		vec3MultiplyS(centroid, collider->invMass);
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