#ifndef particleRenderer_h
#define particleRenderer_h


#include <stddef.h>
#include <stdint.h>

#include "spriteRenderer.h"

#include "particleManager.h"
#include "particleRendererSprite.h"
#include "particleRendererBeam.h"
#include "particleRendererMesh.h"


#define PARTICLE_RENDERER_NUM_TYPES 3

#define PARTICLE_RENDERER_SPRITE 0
#define PARTICLE_RENDERER_BEAM   1
#define PARTICLE_RENDERER_MESH   2


typedef uint_least8_t particleRendererType_t;

typedef struct particleRenderer {
	// This should be large enough
	// to store any type of renderer.
	union {
		particleRendererSprite;
		particleRendererBeam;
		particleRendererMesh;
	} data;
	// Stores which type of
	// renderer this object is.
	particleRendererType_t type;
} particleRenderer;


void particleRendererInitBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch
);
size_t particleRendererBatchSize(
	const particleRenderer *const restrict renderer,
	const particleManager *const restrict manager
);
void particleRendererBatch(
	const particleRenderer *const restrict renderer,
	const particleManager *const restrict manager,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
);


extern void (*const particleRendererInitBatchTable[PARTICLE_RENDERER_NUM_TYPES])(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch
);

extern size_t (*const particleRendererBatchSizeTable[PARTICLE_RENDERER_NUM_TYPES])(
	const void *const restrict renderer,
	const particleManager *const restrict manager
);

extern void (*const particleRendererBatchTable[PARTICLE_RENDERER_NUM_TYPES])(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
);


#endif