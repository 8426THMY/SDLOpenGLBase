#include "visCullerPortal.h"


#include "renderQueue.h"
#include "renderView.h"

#include "memoryManager.h"


// Forward-declare any helper functions!
static void visCellPopulateRenderQueues(
	visCell *const restrict cell,
	renderView *const restrict view
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
);

static void visCellDelete(visCell *const restrict cell);


void visCullerPortalPopulateRenderView(
	visCullerPortal *const restrict vcPortal,
	renderView *const restrict view
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
){

	// Figure out which cell the camera is in.
	visCell *const camCell = NULL;
	// Add objects/lights in the camera's cell to the render queue.
	visCellPopulateRenderQueues(camCell, view, viewID, frameID);

	{
		visCell *curCell = camCell->visibleCells;
		// Add objects/lights in any visible cells to the render queue.
		if(curCell != NULL){
			const visCell *const lastCell = &curCell[camCell->numVisibleCells];
			do {
				visCellPopulateRenderQueues(curCell, view, viewID, frameID, dt);
				++curCell;
			} while(curCell != NULL);
		}
	}

	{
		visPortal *curPortal = camCell->portals;
		// Add any objects that are visible through portals.
		if(curPortal != NULL){
			const visPortal *const lastPortal = &curPortal[camCell->numPortals];
			do {
				#warning "Check if the portal is activated."
				#warning "Do portal culling."
				visCellPopulateRenderQueues(curPortal->cell, view, viewID, frameID, dt);
				++curPortal;
			} while(curPortal != NULL);
		}
	}
}


void visCullerPortalDelete(visCullerPortal *const restrict vcPortal){
	visCell *curCell = vcPortal->cells;
	// Delete the culler's cells.
	if(curCell != NULL){
		const visCell *const lastCell = &curCell[vcPortal->numCells];
		do {
			visCellDelete(curCell);
			++curCell;
		} while(curCell != lastCell);
		memoryManagerGlobalFree(vcPortal->cells);
	}
}


static void visCellPopulateRenderQueues(
	visCell *const restrict cell,
	renderView *const restrict view,
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
){

	{
		renderObject *curObj = cell->objs;
		const renderObject *const lastObj = &curObj[cell->numObjs];
		for(; curObj < lastObj; ++curObj){
			// If the current render object hasn't already been added to this
			// view and it's in the viewing frustum, add it to the queue!
			if(curObj->viewID != viewID && renderObjInFrustum(curObj, &view->frustum)){
				renderQueueKey key;
				renderQueue *const queue =
					&view->queues[renderObjGetRenderQueueKey(curObj, &key)];

				// Make sure we don't add this render object to
				// the queue again if it's in any other cells.
				curObj->viewID = viewID;

				if(queue->numKeyVals < RENDER_QUEUE_MAX_ITEMS){
					renderQueueKeyValue *const keyVal = queue->keyVals[queue->numKeyVals];
					keyVal->key = key;
					keyVal->value = (void *)curObj;
					++queue->numKeyVals;

					// Update the render object's global transform if
					// this is the first time we've seen it this frame.
					if(curObj->frameID != frameID){
						renderObjUpdateGlobalTransform(curObj, dt);
						curObj->frameID = frameID;
					}
					// Perform any extra logic before drawing. In the
					// case of mirrors or portals, this may add extra
					// render views after the current one.
					renderObjPreDraw(curObj, view);
				}else{
					printf("Warning: Render queue %u overflowed!\n", 0);
				}
			}
		}
	}

	{
		light *curLight = cell->lights;
		const light *const lastLight = &curLight[cell->numLights];
		for(; curLight < lastLight; ++curLight){
			// If the current light hasn't already been added to this
			// view and it's in the viewing frustum, add it to the queue!
			if(curLight->viewID != viewID && lightInFrustum(curLight, &view->frustum)){
				// Make sure we don't add this light to the
				// queue again if it's in any other cells.
				curLight->viewID = viewID;

				if(view->numLights < RENDER_VIEW_MAX_LIGHTS){
					view->lights[view->numLights] = curLight;
					++view->numLights;

					// Update the light's global transform if this
					// is the first time we've seen it this frame.
					if(curLight->frameID != frameID){
						lightUpdateGlobalTransform(curLight, dt);
						curLight->frameID = frameID;
					}
					// Perform any extra logic before drawing.
					// This typically involves adding the light's
					// views to the shared shadow map render target.
					lightPreDraw(curLight, view);
				}else{
					printf("Warning: Render view light array overflowed!\n", 0);
				}
			}
		}
	}
}


static void visCellDelete(visCell *const restrict cell){
	if(cell->visibleCells != NULL){
		memoryManagerGlobalFree(cell->visibleCells);
	}
	if(cell->portals != NULL){
		memoryManagerGlobalFree(cell->portals);
	}
}