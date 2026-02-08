#include "renderView.h"


#include "camera.h"


void renderViewInit(
	renderView *const restrict view,
	const camera *const restrict cam, const float dt,
	const framebufferViewport *const restrict viewport
){

	cameraComputeViewMatrix(cam, dt, &view->viewMatrix);
	cameraComputeProjectionMatrix(
		cam, dt, viewport->w, viewport->h, &view->vpMatrix
	);
	mat4MultiplyMat3x4(&view->vpMatrix, &view->viewMatrix);
	renderFrustumInit(&view->frustum, &view->vpMatrix);

	view->numLights = 0;

	{
		const renderQueue *curQueue = view->queues;
		const renderQueue *const lastQueue = &view->queues[RENDER_VIEW_NUM_BUCKETS];
		for(; curQueue < lastQueue; ++curQueue){
			curQueue->numKeyVals = 0;
		}
	}
}


void renderViewPreDraw(renderView *const restrict view){
	// Light clusters.

	{
		size_t i;
		for(i = 0; i < RENDER_TARGET_NUM_BUCKETS; ++i){
			// Sort the render queues.
		}
	}
}

// Draw every render object in the specified render queue!
void renderViewDraw(renderView *const restrict view){
	const renderQueue *curQueue = view->queues;
	const renderQueue *const lastQueue = &view->queues[RENDER_VIEW_NUM_BUCKETS];

	// Send the view projection matrix to the shader.
	shaderPrgLoadSharedUniforms(&view->vpMatrix);

	for(; curQueue != lastQueue; ++curQueue){
		const renderQueueKeyValue *curKeyVal = curQueue->keyVals;
		const renderQueueKeyValue *const lastKeyVal = &curKeyVal[curQueue->numKeyVals];
		// Draw each render object in this render queue!
		for(; curKeyVal != lastKeyVal; ++curKeyVal){
			renderObjectDraw((const renderObject *)curKeyVal->value);
		}
	}
}