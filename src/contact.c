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