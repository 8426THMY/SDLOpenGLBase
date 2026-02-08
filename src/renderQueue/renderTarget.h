#ifndef renderTarget_h
#define renderTarget_h


#include "framebuffer.h"

#include "camera.h"
#include "renderView.h"


typedef struct renderTarget {
	// Doubly linked list of views
	// drawn to this render target.
	renderView *views;
	// Render views are drawn from last to first.
	renderView *lastView;

	// Views may have their own cameras, but
	// we need a camera for the base view.
	camera cam;
	// Multiple render targets may share the same
	// framebuffer (splitscreen, for example).
	framebuffer *fb;
	framebufferViewport viewport;
} renderTarget;


typedef scene scene;
void renderTargetPreDraw(
	const renderTarget *const target,
	scene *const restrict scn, const float dt
);
void renderTargetDraw(const renderTarget *const target);


#endif