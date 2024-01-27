#ifndef particleRendererBeam_h
#define particleRendererBeam_h


#include <stddef.h>

#include "sprite.h"
#include "spriteRenderer.h"

#include "sort.h"

#include "camera.h"


/**
 ** We probably want two texturing modes:
 **     1. The texture goes from one end of the beam to the other.
 **     2. The texture covers each quad of the beam once.
 ** We should also allow the width of the beam to be dilated depending on the particle's scale.
 **/
typedef struct particleRendererBeam {
	float halfWidth;
	unsigned int subdivisions;
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