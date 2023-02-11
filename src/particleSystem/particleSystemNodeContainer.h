#ifndef particleSystemNodeContainer_h
#define particleSystemNodeContainer_h


#include "camera.h"


/*
** This represents a high-level node of the particle
** system, and contains each of the node's instances.
*/
typedef struct particleSystemNodeDef particleSystemNodeDef;
typedef struct particleSystemNode particleSystemNode;
typedef struct particleSystemNodeContainer particleSystemNodeContainer;
typedef struct particleSystemNodeContainer {
	particleSystemNodeDef *nodeDef;
	particleSystemNode *instances;

	particleSystemNodeContainer *children;
} particleSystemNodeContainer;


particleSystemNodeContainer *particleSysNodeContainerInit(
	particleSystemNodeContainer *const restrict container,
	const particleSystemNodeDef *const restrict nodeDef,
	particleSystemNodeContainer *const restrict children
);
particleSystemNode *particleSysNodeContainerInstantiate(
	particleSystemNodeContainer *const restrict container
);

void particleSysNodeContainerUpdate(
	particleSystemNodeContainer *const restrict container, const float dt
);
void particleSysNodeContainerBatch(
	particleSystemNodeContainer *const restrict container,
	const camera *const restrict cam, const float dt
);

void particleSysNodeContainerDelete(
	particleSystemNodeContainer *const restrict container
);


#endif