#ifndef particleRendererPoint_h
#define particleRendererPoint_h


#include <stddef.h>

#include "spriteRenderer.h"

#include "sort.h"

#include "camera.h"


#error "These should still use the batched renderer, but we might need to do some modifications to make it work."
typedef struct particleRendererPoint {
	#warning "Do we need to revert the point size back to 1 when we're done? If we're using glPointSize we would, but if we use a uniform and set gl_PointSize in the shader, probably not."
	float size;
} particleRendererPoint;


size_t particleRendererPointBatchSize(
	const void *const restrict renderer, const size_t numParticles
);
void particleRendererPointBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


#endif