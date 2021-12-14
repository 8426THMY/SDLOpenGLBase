#include "collider.h"


void (*const colliderInstantiateTable[COLLIDER_NUM_TYPES])(void *const restrict c, const void *const restrict cBase) = {
	colliderHullInstantiate
};

return_t (*const colliderLoadTable[COLLIDER_NUM_TYPES])(
	void *const restrict c, FILE *const restrict cFile,
	vec3 *const restrict centroid, mat3 *const restrict inertia
) = {

	colliderHullLoad
};
void (*const colliderUpdateTable[COLLIDER_NUM_TYPES])(
	void *const restrict c, const vec3 *const restrict centroid,
	const void *const restrict cBase, const vec3 *const restrict baseCentroid,
	transformState trans, colliderAABB *const restrict aabb
) = {

	colliderHullUpdate
};

void (*const colliderDeleteInstanceTable[COLLIDER_NUM_TYPES])(void *const restrict c) = {
	colliderHullDeleteInstance
};
void (*const colliderDeleteTable[COLLIDER_NUM_TYPES])(void *const restrict c) = {
	colliderHullDelete
};


// Ordinary colliders are used as a base for instances.
void colliderInit(collider *const restrict c, const colliderType_t type){
	c->type = type;
}

// Instances share the components of bases, but can be updated.
void colliderInstantiate(collider *const restrict c, const collider *const restrict cBase){
	c->type = cBase->type;
	colliderInstantiateTable[cBase->type]((void *)(&c->data), (const void *)(&cBase->data));
}


// Load a collider from the file specified and store its centroid
// and moment of inertia tensor in the appropriate parameters.
// Note that this function does NOT close the file.
return_t colliderLoad(
	collider *const restrict c, FILE *const restrict cFile, vec3 *const restrict centroid, mat3 *const restrict inertia
){

	return(colliderLoadTable[c->type]((void *)(&c->data), cFile, centroid, inertia));
}

// Update a collider instance and return its new bounding box.
void colliderUpdate(
	collider *const restrict c, const vec3 *const restrict centroid,
	const collider *const restrict cBase, const vec3 *const restrict baseCentroid,
	const transformState *const restrict trans, colliderAABB *const restrict aabb
){

	colliderUpdateTable[c->type]((void *)(&c->data), centroid, (void *)(&cBase->data), baseCentroid, *trans, aabb);
}


void colliderDeleteInstance(collider *const restrict c){
	colliderDeleteInstanceTable[c->type]((void *)(&c->data));
}

void colliderDelete(collider *const restrict c){
	colliderDeleteTable[c->type]((void *)(&c->data));
}