#ifndef particleRendererMesh_h
#define particleRendererMesh_h


#include "mesh.h"
#include "renderBatch.h"


typedef struct particleRendererMesh {
	mesh meshData;
} particleRendererMesh;


void particleRendererMeshInitBatch(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
);
typedef struct particleManager particleManager;
void particleRendererMeshBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
);


#endif