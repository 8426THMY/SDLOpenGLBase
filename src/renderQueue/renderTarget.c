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

		target->lastView = curView;
		curView = moduleRenderViewNext(curView);
	} while(curView != NULL);
}


void renderTargetDraw(const renderTarget *const target){
	renderView *curView = target->lastView;
	// We draw our render views starting from the end of the list.
	// Dependencies like mirrors are usually added after the main
	// view, so this means transluscent objects in front of them
	// will appear correctly.
	do {
		renderViewDraw(curView);
		curView = moduleRenderViewPrev(curView);
	} while(curView != NULL);
}