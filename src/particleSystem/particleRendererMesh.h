#ifndef particleRendererMesh_h
#define particleRendererMesh_h


#include <stddef.h>

#include "mesh.h"
#include "spriteRenderer.h"

#include "sort.h"

#include "camera.h"


typedef struct particleRendererMesh {
	mesh meshData;
} particleRendererMesh;


void particleRendererMeshInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
);
typedef struct particleManager particleManager;
size_t particleRendererMeshBatchSize(
	const void *const restrict renderer, const size_t numParticles
);
void particleRendererMeshBatch(
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


#endif