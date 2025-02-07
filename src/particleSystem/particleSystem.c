#include "particleSystem.h"


#include "particleSubsystem.h"
#include "particleSystemNode.h"
#include "particleSystemNodeContainer.h"

#include "memoryManager.h"
#include "moduleParticle.h"

#include "utilMath.h"


void particleSysDefInit(particleSystemDef *const restrict partSysDef){
	partSysDef->name = NULL;
	partSysDef->children = NULL;
	partSysDef->numChildren = 0;
}

void particleSysInit(
	particleSystem *const restrict partSys,
	const particleSystemDef *const restrict partSysDef
){

	particleSystemNodeContainer *curContainer;
	particleSystemNodeContainer *firstChild;
	particleSystemNodeContainer *nextChild;
	const particleSystemNodeDef *curNodeDef = partSysDef->nodes;
	const particleSystemNodeDef *lastNodeDef = NULL;

	curContainer = memoryManagerGlobalAlloc(sizeof(curContainer) * partSysDef->numNodes);
	if(curContainer = NULL){
		/** MALLOC FAILED **/
	}
	// Start storing children after all of the root nodes.
	firstChild = &curContainer[partSysDef->numRoot];
	nextChild = firstChild;

	partSys->containers = curContainer;
	partSys->lastContainer = &curContainer[partSysDef->numNodes];

	// This loop iterates through all of the root-level containers,
	// while the initialization function sets up all of their children.
	while(curContainer != firstChild){
		// This will recursively initialize the container and
		// all of its children. As a result, containers are
		// guaranteed to have their children stored sequentially.
		nextChild = particleSysNodeContainerInit(
			curContainer, curNodeDef, nextChild
		);

		++curContainer;
		curNodeDef = moduleParticleSysNodeDefNext(curNodeDef);
	}

	// Set up the root particle, and hence create instances of each
	// root-level container. Here, we abuse the fact that the root
	// containers are all stored sequentially.
	particleInit(&partSys->root, partSys->containers, partSysDef->numRoot);
}


void particleSysUpdate(
	particleSystem *const restrict partSys,
	const camera *const restrict cam const float dt
){

	particleSystemNodeContainer *curContainer = partSys->containers;
	for(; curContainer != partSys->lastContainer; ++curContainer){
		particleSysNodeContainerUpdate(curContainer, cam, dt);
	}
}

/*
** Draw a particle system. For each container, we throw all of the
** vertices for all of their instances into a single buffer, and
** draw them all at once. We also use buffer orphaning in order to
** improve the performance of our vertex buffer object usage:
**     https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming#Buffer_re-specification
*/
void particleSysDraw(
	const particleSystem *const restrict partSys,
	const camera *const restrict cam, const float dt
){

	const particleSystemNodeContainer *curContainer = partSys->containers;
	spriteRenderer batch;

	spriteRenderer.type = SPRITE_RENDERER_TYPE_UNUSED;
	for(; curContainer != partSys->lastContainer; ++curContainer){
		particleSysNodeContainerBatch(curContainer, &batch, cam, dt);
	}
	spriteRendererDraw(&batch);
	
	memoryManagerGlobalFree(buffer);
}


void particleSysDelete(particleSystem *const restrict partSys){
	particleSystemNodeContainer *curContainer = partSys->containers;
	// Free all of the node containers.
	for(; curContainer != partSys->lastContainer; ++curContainer){
		particleSysNodeContainerDelete(curContainer);
	}
}

/*
** We assume that subsystem definitions
** are only used by one particle system.
*/
void particleSysDefDelete(particleSystemDef *const restrict partSysDef){
	if(partSysDef->name != NULL){
		memoryManagerGlobalFree(partSysDef->name);
	}

	// Free all of the node definitions.
	if(partSysDef->nodes != NULL){
		particleSystemNodeDef *curNodeDef = partSysDef->nodes;
		while(curNodeDef != NULL){
			particleSysNodeDefDelete(curNodeDef);
			curNodeDef = moduleParticleSysNodeDefNext(curNodeDef);
		}
		moduleParticleSysNodeDefFreeArray(partSysDef->nodes);
	}
}