#ifndef particleRendererPoint_h
#define particleRendererPoint_h


#include <stddef.h>

#include "spriteRenderer.h"

#include "sort.h"

#include "camera.h"


#error "These should still use the batched renderer, but we might need to do some modifications to make it work."
typedef struct particleRendererPoint {
	#warning "We should have something here for the size."
	//
} particleRendererPoint;


void particleRendererPointInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
);
typedef struct particleManager particleManager;
size_t particleRendererPointBatchSize(
	const void *const restrict renderer, const size_t numParticles
);
void particleRendererPointBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


#endif