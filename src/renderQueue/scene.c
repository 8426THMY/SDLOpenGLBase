#include "scene.h"


void sceneDraw(scene *const restrict scn, const float dt){
	renderTarget *curTarget = scn->targets;
	const renderTarget *const lastTarget = &curTarget[scn->numTargets];

	// Set up the render queues for each render target.
	for(; curTarget != lastTarget; ++curTarget){
		renderTargetPreDraw(curTarget, scn, dt);
	}
	// Draw each render target.
	for(curTarget = scn->targets; curTarget != lastTarget; ++curTarget){
		renderTargetDraw(curTarget);
	}
	++scn->frameID;

	// Clear all of the views we've allocated.
	moduleRenderViewClear();
}