#include "renderObject.h"


return_t renderObjInFrustum(
	const renderObject *const restrict obj,
	const renderFrustum *const restrict frustum
){

	switch(obj->type){
		case RENDEROBJECT_ANIMATED_MODEL:
			return(modelInFrustum(&obj->data.mdl, frustum));
		break;
		case RENDEROBJECT_PARTICLE_SYSTEM:
			return(particleSysInFrustum(&obj->data.partSys, frustum));
		break;
	}
	return(0);
}

renderQueueID renderObjGetRenderQueueKey(
	const renderObject *const restrict obj,
	renderQueueKey *const restrict key
){

	switch(obj->type){
		case RENDEROBJECT_ANIMATED_MODEL:
			return(modelGetRenderQueueKey(&obj->data.mdl, key));
		break;
		case RENDEROBJECT_PARTICLE_SYSTEM:
			return(particleSysGetRenderQueueKey(&obj->data.partSys, key));
		break;
	}
	return(0);
}

void renderObjUpdateGlobalTransform(
	renderObject *const restrict obj, const float dt
){

	switch(obj->type){
		case RENDEROBJECT_ANIMATED_MODEL:
			modelUpdateGlobalTransform(&obj->data.mdl, dt);
		break;
		case RENDEROBJECT_PARTICLE_SYSTEM:
			particleSysUpdateGlobalTransform(&obj->data.partSys, dt);
		break;
	}
}

renderQueueKey renderObjPreDraw(
	const renderObject *const restrict obj,
	renderView *const restrict view
){

	switch(obj->type){
		case RENDEROBJECT_ANIMATED_MODEL:
			modelPreDraw(&obj->data.mdl, view);
		break;
		case RENDEROBJECT_PARTICLE_SYSTEM:
			particleSysPreDraw(&obj->data.partSys, view);
		break;
	}
	return(0);
}

void renderObjDraw(const renderObject *const restrict obj){
	switch(obj->type){
		case RENDEROBJECT_ANIMATED_MODEL:
			modelDraw(&obj->data.mdl);
		break;
		case RENDEROBJECT_PARTICLE_SYSTEM:
			particleSysDraw(&obj->data.partSys);
		break;
	}
}