#include "renderObject.h"


return_t (*const renderObjInFrustumTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj,
	const renderFrustum *const restrict frustum
) = {

	modelInFrustum
	particleSysInFrustum
}

renderQueueID (*const renderObjGetRenderQueueKeyTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj,
	renderQueueKey *const restrict key
) = {

	modelGetRenderQueueKey
	particleSysGetRenderQueueKey
}

void (*const renderObjUpdateGlobalTransformTable[RENDEROBJECT_NUM_TYPES])(
	renderObject *const restrict obj
) = {

	modelUpdateGlobalTransform
	particleSysUpdateGlobalTransform
}

void (*const renderObjPreDrawTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj,
	renderView *const restrict view
) = {

	modelPreDraw
	particleSysPreDraw
}

void (*const renderObjDrawTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj
) = {

	modelDraw
	particleSysDraw
}


return_t renderObjInFrustum(
	const renderObject *const restrict obj,
	const renderFrustum *const restrict frustum
){

	return(renderObjInFrustumTable[obj->type]((const void *)(&obj->data), frustum));
}

renderQueueID renderObjGetRenderQueueKey(
	const renderObject *const restrict obj,
	renderQueueKey *const restrict key
){
	return(renderObjGetRenderQueueKeyTable[obj->type]((const void *)(&obj->data), key));
}

void renderObjUpdateGlobalTransform(renderObject *const restrict obj){
	renderObjUpdateGlobalTransformTable[obj->type]((void *)(&obj->data));
}

renderQueueKey renderObjPreDraw(
	const renderObject *const restrict obj,
	renderView *const restrict view
){

	renderObjPreDrawTable[obj->type]((const void *)(&obj->data), view);
}

void renderObjDraw(const renderObject *const restrict obj){
	renderObjDrawTable[obj->type]((const void *)(&obj->data));
}