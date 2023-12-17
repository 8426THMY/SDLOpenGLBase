#include "particleSystemNode.h"


#include "transform.h"

#include "particleSystemNodeContainer.h"

#include "memoryManager.h"
#include "moduleParticle.h"


// Forward-declare any helper functions!
static void initializeParticle(
	const particleSystemNode *const restrict node,
	particle *const restrict part
);
static void emitParticles(
	particleSystemNode *const restrict node, size_t spawnCount
);
static void updateEmitters(particleSystemNode *const restrict node, const float dt);

static void operateParticle(
	const particleSystemNodeDef *const restrict nodeDef,
	particle *const restrict part, const float dt
);
static void constrainParticle(
	const particleSystemNodeDef *const restrict nodeDef,
	particle *const restrict part, const float dt
);
static void updateParticles(particleSystemNode *const restrict node, const float dt);


/*
** Initialize a particle system node. Note that owners
** must initialize the pointers they explicitly control,
** such as those related to the container and subsystem.
*/
void particleSysNodeInit(
	particleSystemNode *const restrict node,
	const particleSystemNodeDef *const restrict nodeDef
){

	if(nodeDef->numEmitters <= 0){
		node->emitters = NULL;

	// Initialize all of the node's emitters!
	}else{
		particleEmitter *curEmitter;
		const particleEmitter *lastEmitter;

		curEmitter = memoryManagerGlobalAlloc(
			sizeof(*node->emitters) * nodeDef->numEmitters
		);
		if(curEmitter == NULL){
			/** MALLOC FAILED **/
		}
		lastEmitter = &curEmitter[nodeDef->numEmitters];

		node->emitters = curEmitter;

		do {
			particleEmitterInit(curEmitter);
			++curEmitter;
		} while(curEmitter < lastEmitter);
	}

	// Set up the particle manager and allocate our particle free list.
	particleManagerInit(&node->manager, nodeDef->maxParticles);

	node->lifetime = nodeDef->lifetime;
}


// Spawn particles using a node's emitters.
void particleSysNodeUpdateEmitters(particleSystemNode *const restrict node, const float dt){
	const particleSystemDef *const nodeDef = node->container->nodeDef;
	const size_t maxParticles = nodeDef->maxParticles;

	particleEmitter *curEmitter = node->emitters;
	const particleEmitter *const lastEmitter = &curEmitter[nodeDef->numEmitters];
	particleEmitterDef *curEmitterDef = nodeDef->emitters;
	// This loop exits when our system reaches its particle limit.
	for(; curEmitter < lastEmitter; ++curEmitter, ++curEmitterDef){
		// Check how many particles this emitter should spawn.
		const size_t spawnCount = particleEmitterUpdate(curEmitter, curEmitterDef, dt);
		const size_t freeCount  = particleManagerRemaining(&node->manager, maxParticles);

		// If we still have room, spawn the new particles.
		if(spawnCount < freeCount){
			emitParticles(node, spawnCount);

		// Otherwise, spawn as many as possible and exit the loop.
		}else{
			emitParticles(node, freeCount);
			break;
		}
	}
}

/*
** Update the node's particles. Note that we don't update any of
** the particles' nodes here! These are updated in the main loop.
** This should only be called if there are living particles!
*/
void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt){
	const particleSubsysDef *const nodeDef = node->container->nodeDef;
	const transform *const parentState = (node->parent == NULL) ? NULL : node->parent->state;
	
	particle *curParticle = node->manager.first;
	// Update all of the node's particles!
	do {
		particle *const nextParticle = curParticle->next;

		// Delete the particle if it has died. This is
		// done before updating to ensure that particles
		// always live for at least one tick (unless the
		// lifetime is set to initialize to zero).
		if(particleDead(curParticle)){
			particleManagerFree(curParticle);

		// Update the particle if it survived.
		}else{
			particlePreUpdate(part, parentState, dt);
			operateParticle(nodeDef, part, dt);
			constrainParticle(nodeDef, part, dt);
			particlePostUpdate(part, dt);
		}

		curParticle = nextParticle;
	} while(curParticle != NULL);

	#warning "We should sort our particles here. This means we need the camera!"
	#warning "Maybe try radix sort?"
}


/*
** When a particle system node's owner dies, the
** node itself should be killed as soon as possible.
*/
void particleSysNodeOrphan(particleSystemNode *const restrict node){
	node->lifetime = 0.f;
	node->parent = NULL;
	node->prevSibling = NULL;
	node->nextSibling = NULL;
}

void particleSysNodeDelete(particleSystemNode *const restrict node){
	if(node->emitters != NULL){
		memoryManagerGlobalFree(node->emitters);
	}
	particleManagerDelete(&node->manager);

	// Fix up the subsystem pointers.
	if(node->parent != NULL){
		particleSubsysRemove(node->parent, node);
	}
}

void particleSysNodeDefDelete(particleSystemNodeDef *const restrict nodeDef){
	if(nodeDef->emitters != NULL){
		memoryManagerGlobalFree(nodeDef->emitters);
	}
	if(nodeDef->initializers != NULL){
		memoryManagerGlobalFree(nodeDef->initializers);
	}
	if(nodeDef->operators != NULL){
		memoryManagerGlobalFree(nodeDef->operators);
	}
	if(nodeDef->constraints != NULL){
		memoryManagerGlobalFree(nodeDef->constraints);
	}
}


// Execute each of the node's initializers on a particle.
static void initializeParticle(
	const particleSystemNode *const restrict node,
	particle *const restrict part
){

	const particleSubsystemDef *nodeDef = node->container->nodeDef;
	const particleInitializer *curInitializer = nodeDef->initializers;

	// This prepares the particle for the initializers.
	particlePreInit(part, node);

	// Run through all of the initializers for this subsystem.
	if(curInitializer != NULL){
		const particleInitializer *const lastInitializer = nodeDef->lastInitializer;
		do {
			(*curInitializer->func)((const void *)(&curInitializer->data), part);
			++curInitializer;
		} while(curInitializer <= lastInitializer);
	}

	// Now that we've initialized the particle,
	// set up some of its subsystem properties.
	particlePostInit(part, node);
}

/*
** Spawn the number of new particles given by "spawnCount".
** We assume that the node has enough room for them.
*/
static void emitParticles(
	particleSystemNode *const restrict node, size_t spawnCount
){

	// Allocate and initialize all of our particles!
	for(; spawnCount > 0; --spawnCount){
		particle *const part = particleManagerAlloc(&node->manager);
		initializeParticle(node, part);
	}
}


// Execute each of the node's operators on a particle.
static void operateParticle(
	const particleSystemNodeDef *const restrict nodeDef,
	particle *const restrict part, const float dt
){

	const particleOperator *curOperator = nodeDef->operators;
	if(curOperator != NULL){
		const particleOperator *const lastOperator = nodeDef->lastOperator;
		do {
			(*curOperator->func)((const void *)(&curOperator->data), part, dt);
			++curOperator;
		} while(curOperator <= lastOperator);
	}
}

// Execute each of the node's constraints on a particle.
static void constrainParticle(
	const particleSystemNodeDef *const restrict nodeDef,
	particle *const restrict part, const float dt
){

	const particleConstraint *curConstraint = nodeDef->constraints;
	if(curConstraint != NULL){
		const particleConstraint *const lastConstraint = nodeDef->lastConstraint;
		do {
			(*curConstraint->func)((const void *)(&curConstraint->data), part, dt);
			++curConstraint;
		} while(curConstraint <= lastConstraint);
	}
}