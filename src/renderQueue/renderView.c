#include "renderView.h"


#include "camera.h"


void renderViewInit(
	renderView *const restrict view,
	const camera *const restrict cam, const float dt,
	const renderViewport *const restrict viewport
){

	cameraComputeViewMatrix(cam, dt, &view->viewMatrix);
	cameraComputeProjectionMatrix(
		cam, dt, viewport->width, viewport->height, &view->vpMatrix
	);
	mat4MultiplyMat3x4(&view->vpMatrix, &view->viewMatrix);
	renderFrustumInit(&view->frustum, &view->vpMatrix);

	view->numLights = 0;

	{
		size_t i;
		for(i = 0; i < RENDER_TARGET_NUM_BUCKETS; ++i){
			view->queues[i].numKeyVals = 0;
		}
	}
}


void renderViewPreDraw(renderView *const restrict view){
	// Light clusters.
	// Sort the render queues.
}

// Draw every render object in the specified render queue!
void renderViewDrawQueue(
	renderView *const restrict view,
	const renderQueueID id
){

	const renderQueue *const queue = &view->queues[id];
	const renderQueueKeyValue *curKeyVal = queue->keyVals;
	const renderQueueKeyValue *lastKeyVal = &curKeyVal[queue->numKeyVals];
	while(curKeyVal != lastKeyVal){
		renderObjectDraw((const renderObject *)curKeyVal->value);
		++curKeyVal;
	}
}