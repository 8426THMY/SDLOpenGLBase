#include "collider.h"


void (*colliderInstantiateTable[COLLIDER_NUM_TYPES])(void *c, const void *cBase) = {
	colliderHullInstantiate
};

return_t (*colliderLoadTable[COLLIDER_NUM_TYPES])(void *c, FILE *cFile, vec3 *centroid, mat3 *inertia) = {
	colliderHullLoad
};
void (*colliderUpdateTable[COLLIDER_NUM_TYPES])(void *c, const void *cBase, const transformState *trans, colliderAABB *aabb) = {
	colliderHullUpdate
};

void (*colliderDeleteInstanceTable[COLLIDER_NUM_TYPES])(void *c) = {
	colliderHullDeleteInstance
};
void (*colliderDeleteTable[COLLIDER_NUM_TYPES])(void *c) = {
	colliderHullDelete
};


// Ordinary colliders are used as a base for instances.
void colliderInit(collider *c, const colliderType_t type){
	c->type = type;
}

// Instances share the components of bases, but can be updated.
void colliderInstantiate(collider *c, const collider *cBase){
	c->type = cBase->type;
	colliderInstantiateTable[cBase->type]((void *)(&c->data), (const void *)(&cBase->data));
}


// Load a collider from the file specified and store its centroid
// and moment of inertia tensor in the appropriate parameters.
// Note that this function does NOT close the file.
return_t colliderLoad(collider *c, FILE *cFile, vec3 *centroid, mat3 *inertia){
	return(colliderLoadTable[c->type]((void *)(&c->data), cFile, centroid, inertia));
}

// Update a collider instance and return its new bounding box.
void colliderUpdate(collider *c, const collider *cBase, const transformState *trans, colliderAABB *aabb){
	colliderUpdateTable[c->type]((void *)(&c->data), (void *)(&cBase->data), trans, aabb);
}


void colliderDeleteInstance(collider *c){
	colliderDeleteInstanceTable[c->type]((void *)(&c->data));
}

void colliderDelete(collider *c){
	colliderDeleteTable[c->type]((void *)(&c->data));
}