#ifndef particleRenderer_h
#define particleRenderer_h


#include <stddef.h>
#include <stdint.h>

#include "spriteRenderer.h"
#include "spriteRendererCommon.h"

#include "particleManager.h"
#include "particleRendererPoint.h"
#include "particleRendererSprite.h"
#include "particleRendererBeam.h"
#include "particleRendererMesh.h"

#include "sort.h"

#include "camera.h"


#define PARTICLE_RENDERER_NUM_TYPES 4

#define PARTICLE_RENDERER_POINT  0
#define PARTICLE_RENDERER_SPRITE 1
#define PARTICLE_RENDERER_BEAM   2
#define PARTICLE_RENDERER_MESH   3


typedef uint_least8_t particleRendererType;

typedef struct particleRenderer {
	// This should be large enough
	// to store any type of renderer.
	union {
		particleRendererPoint;
		particleRendererSprite;
		particleRendererBeam;
		particleRendererMesh;
	} data;
	spriteRendererCommon common;
	// Stores which type of
	// renderer this object is.
	particleRendererType type;
} particleRenderer;


void particleRendererInitBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch
);
size_t particleRendererBatchSize(
	const particleRenderer *const restrict renderer, const size_t numParticles
);
void particleRendererBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


extern spriteRendererType particleRendererTypeTable[PARTICLE_RENDERER_NUM_TYPES];

extern size_t (*const particleRendererBatchSizeTable[PARTICLE_RENDERER_NUM_TYPES])(
	const particleRenderer *const restrict renderer, const size_t numParticles
);

extern void (*const particleRendererBatchTable[PARTICLE_RENDERER_NUM_TYPES])(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


#endif