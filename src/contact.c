#include "contact.h"


//Jump table for checking separation independent of collider types.
return_t (*contactSeparationTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *cA,
	const void *cB,
	contactSeparation *cs
) = {
	{
		colliderHullSeparated
	}
};

//Jump table for checking collision independent of collider types.
return_t (*contactCollisionTable[COLLIDER_NUM_TYPES][COLLIDER_NUM_TYPES])(
	const void *cA,
	const void *cB,
	contactSeparation *cs,
	contactManifold *cm
) = {
	{
		colliderHullCollidingSAT
	}
};


return_t collidersAreSeparated(const collider *cA, const collider *cB, contactSeparation *cs){
	return(contactSeparationTable[cA->type][cB->type]((void *)(&cA->data), (void *)(&cB->data), cs));
}

return_t collidersAreColliding(const collider *cA, const collider *cB, contactSeparation *cs, contactManifold *cm){
	return(contactCollisionTable[cA->type][cB->type]((void *)(&cA->data), (void *)(&cB->data), cs, cm));
}