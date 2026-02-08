#include "collider.h"


// Ordinary colliders are used as a base for instances.
void colliderInit(collider *const restrict c, const colliderType type){
	c->type = type;
}

// Instances share the components of bases, but can be updated.
void colliderInstantiate(
	collider *const restrict c,
	const collider *const restrict cBase
){

	c->type = cBase->type;
	switch(c->type){
		case 0:
			colliderHullInstantiate(&c->data.hull, &cBase->data.hull);
		break;
	}
}


// Load a collider from the file specified and store its centroid
// and moment of inertia tensor in the appropriate parameters.
// Note that this function does NOT close the file.
return_t colliderLoad(
	collider *const restrict c,
	FILE *const restrict cFile,
	vec3 *const restrict centroid,
	mat3 *const restrict inertia
){

	switch(c->type){
		case 0:
			return(colliderHullLoad(&c->data.hull, cFile, centroid, inertia));
		break;
	}
	return(0);
}

// Update a collider instance and return its new bounding box.
void colliderUpdate(
	collider *const restrict c, const vec3 *const restrict centroid,
	const collider *const restrict cBase, const vec3 *const restrict baseCentroid,
	const transform *const restrict trans, colliderAABB *const restrict aabb
){

	switch(c->type){
		case 0:
			colliderHullUpdate(
				&c->data.hull, centroid,
				&cBase->data.hull, baseCentroid,
				trans, aabb
			);
		break;
	}
}


void colliderDeleteInstance(collider *const restrict c){
	switch(c->type){
		case 0:
			colliderHullDeleteInstance(&c->data.hull);
		break;
	}
}

void colliderDelete(collider *const restrict c){
	switch(c->type){
		case 0:
			colliderHullDelete(&c->data.hull);
		break;
	}
}