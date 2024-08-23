#include "contact.h"


// Jump table for checking separation independent of collider types.
return_t (*contactSeparationTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *const restrict cA,
	const void *const restrict cB,
	contactSeparation *const restrict cs
) = {
	{
		colliderHullSeparated
	}
};

// Jump table for checking collision independent of collider types.
return_t (*contactCollisionTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *const restrict cA,
	const void *const restrict cB,
	contactSeparation *const restrict cs,
	contactManifold *const restrict cm
) = {
	{
		colliderHullCollidingSAT
	}
};


void contactPointInit(
	contactPoint *const restrict contact,
	const vec3 *const restrict pA, const vec3 *const restrict pB,
	const vec3 *const restrict normal, const float separation,
	const contactKey *const restrict key,
	const unsigned int swapped
){

	if(swapped){
		contact->pA = *pB;
		contact->pB = *pA;
		vec3NegateOut(normal, &contact->normal);
		contact->separation = separation;
		#ifdef CONTACT_MANIFOLD_SIMPLE_KEYS
		contact->key.edgeA = key->edgeB;
		contact->key.edgeB = key->edgeA;
		#else
		contact->key.inEdgeA  = key->inEdgeB;
		contact->key.outEdgeA = key->outEdgeB;
		contact->key.inEdgeB  = key->inEdgeA;
		contact->key.outEdgeB = key->outEdgeA;
		#endif
	}else{
		contact->pA = *pA;
		contact->pB = *pB;
		contact->normal = *normal;
		contact->separation = separation;
		contact->key = *key;
	}
}


return_t collidersAreSeparated(
	const collider *const restrict cA, const collider *const restrict cB, contactSeparation *const restrict cs
){

	return(contactSeparationTable[cA->type][cB->type]((void *)(&cA->data), (void *)(&cB->data), cs));
}

return_t collidersAreColliding(
	const collider *const restrict cA, const collider *const restrict cB,
	contactSeparation *const restrict cs, contactManifold *const restrict cm
){

	return(contactCollisionTable[cA->type][cB->type]((void *)(&cA->data), (void *)(&cB->data), cs, cm));
}