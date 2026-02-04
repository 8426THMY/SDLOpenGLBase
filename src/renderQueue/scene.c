#include "scene.h"


void sceneDraw(scene *const restrict scn, const float dt){
	renderTarget *curTarget = scn->targets;
	const renderTarget *const lastTarget = NULL;//&curTarget[scn->numTargets];

	// Set up the render queues for each render target.
	for(; curTarget != lastTarget; ++curTarget){
		renderTargetPreDraw(curTarget, scn, dt);
	}
	// Draw each render target.
	for(curTarget = scn->targets; curTarget != lastTarget; ++curTarget){
		renderTargetDraw(curTarget);
	}
	++scn->frameID;

	#warning "We should probably force coalescence on the render view memory manager."
	#warning "Memory managers already count used blocks, could we make them count blocks we've freed?"
	#warning "When a block is freed, add one. When a block is allocated and we've counted at least one free block, subtract one."
	#warning "When doing coalescence, we can modify the number of free blocks to only count those before the last used block."
	moduleRenderViewClear();
}