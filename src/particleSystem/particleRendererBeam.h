#ifndef particleRendererBeam_h
#define particleRendererBeam_h


#include <stddef.h>

#include "sprite.h"
#include "spriteRenderer.h"

#include "sort.h"

#include "camera.h"


typedef struct particleRendererBeam {
	unsigned int subdivisions;
	// Half the vertical size of the beam.
	float halfWidth;
	// This value represents how much the horizontal
	// coordinate of the UVs should be incremented
	// with each particle in the beam. Subdivisions
	// should be ignored, as they're accounted for
	// during rendering. If the tile width is zero,
	// then the texture will be stretched to exactly
	// fill the length of the beam.
	float tileWidth;
} particleRendererBeam;


void particleRendererBeamInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
);
typedef struct particleManager particleManager;
size_t particleRendererBeamBatchSize(
	const void *const restrict renderer, const size_t numParticles
);
void particleRendererBeamBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


#endif