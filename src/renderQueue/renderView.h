#ifndef renderView_h
#define renderView_h


#include <stdint.h>

#include "renderFrustum.h"
#include "light.h"
#include "renderQueue.h"


#define RENDER_VIEW_MAX_LIGHTS 1024

// We have one bucket for opaque objects
// and one for translucent objects.
#define RENDER_VIEW_NUM_BUCKETS 2

// If we want to avoid sorting opaque objects, we could
// use multiple buckets for them like the Source engine.
// It could also be helpful to have buckets for common
// shaders so we can avoid sorting them on that criteria.
#define RENDER_VIEW_OPAQUE_BUCKET      0
#define RENDER_VIEW_TRANSLUCENT_BUCKET 1


typedef uint_least8_t renderQueueID;

typedef struct renderView {
	mat3x4 viewMatrix;
	mat4 vpMatrix;
	renderFrustum frustum;

	// For each visible light source, we'll need to store its index
	// and an array of clusters (for forward+). When we're rendering
	// individual objects, we should select a subset of lights that
	// have the greatest impact on the mesh.
	light *lights[RENDER_VIEW_MAX_LIGHTS];
	size_t numLights;

	#warning "For shadow maps, we probably have to do things a bit differently."
	#warning "We only want to draw depth, and we want to ignore certain geometry and translucent objects."
	#warning "Should we have flags for these things?"
	#warning "Shadow maps don't need multiple buckets, so they should have their own structure."

	renderQueue queues[RENDER_VIEW_NUM_BUCKETS];
} renderView;


typedef camera camera;
void renderViewInit(
	renderView *const restrict view,
	const camera *const restrict cam, const float dt,
	const renderViewport *const restrict viewport
);

void renderViewPreDraw(renderView *const restrict view);
void renderViewDrawQueue(
	renderView *const restrict view,
	const renderQueueID id
);


#endif