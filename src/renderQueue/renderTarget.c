#include "renderTarget.h"


#include "visCuller.h"
#include "scene.h"

#include "moduleRender.h"


void renderTargetPreDraw(
	const renderTarget *const target,
	scene *const restrict scn, const float dt
){

	target->views = moduleRenderViewAlloc();

	// Initialize the default view for this target.
	renderView *curView = moduleRenderTargetPrepend(&target->views);
	renderViewInit(curView, &target->cam, dt, &target->viewport);

	// The culling process may insert extra render views after
	// this one (in the case of mirrors or portals, for instance).
	do {
		// Fill up the render queues.
		visCullerPopulateRenderView(
			&scn->culler, curView,
			scn->viewID, scn->frameID, dt
		);
		++scn->viewID;
		#warning "This should be done on a separate thread..."
		renderViewPreDraw(curView);

		curView = moduleRenderViewNext(curView);
	} while(curView != NULL);
}


void renderTargetDraw(const renderTarget *const target){
	renderQueueID id;
	for(id = 0; id < RENDER_VIEW_NUM_BUCKETS; ++id){
		renderView *curView = target->views;
		do {
			renderViewDrawQueue(curView, id);
			curView = moduleRenderViewNext(curView);
		} while(curView != NULL);
	}

	// Clear the views, ready for the next frame.
	#warning "This will scramble the underlying memorySingleList."
	#warning "We probably need a way of doing it in the right order..."
}