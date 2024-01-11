#include "particleSystemNode.h"


#include <stdint.h>

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
	particleSystemNode *const restrict node,
	const size_t spawnCount, const flags_t sortFlags
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


/*
** Update the node's particles. Note that we don't update any of
** the particles' nodes here! These are updated in the main loop.
** This should only be called if there are living particles!
*/
void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt){
	const particleSubsysDef *const nodeDef = node->container->nodeDef;
	const transform *const parentState = (node->parent == NULL) ? NULL : node->parent->state;
	
	particle *curParticle = node->manager.particles;
	const particle *lastParticle = &curParticle[node->manager.numParticles];
	particle *curFreeParticle = NULL;

	// Update all of the node's particles! Remember that we
	// assume that there is at least one living particle.
	//
	// Any particles that are dead will be overwritten with
	// the particles that come after them in the array. For
	// some reason, this is just as fast as overwritting
	// them with the last element in the array.
	do {
		// Delete the particle if it has died. This is
		// done before updating to ensure that particles
		// always live for at least one tick (unless the
		// lifetime is set to initialize to zero).
		if(particleDead(curParticle)){
			particleManagerFree(&node->manager, curParticle);

			// If this is the first particle we've killed,
			// keep a record of its position in the array!
			if(curFreeParticle != NULL){
				curFreeParticle = curParticle;
			}

		// Update the particle if it survived.
		}else{
			particlePreUpdate(part, parentState, dt);
			operateParticle(nodeDef, part, dt);
			constrainParticle(nodeDef, part, dt);
			particlePostUpdate(part, dt);

			// Move this particle to replace the next dead one.
			// This ensures that prior to any sorting, particles
			// are stored in the order that they were spawned.
			if(curFreeParticle != NULL){
				*curFreeParticle = *curParticle;
				++curFreeParticle;
			}
		}

		++curParticle;
	} while(curParticle != lastParticle);
}

// Spawn particles using a node's emitters.
void particleSysNodeUpdateEmitters(particleSystemNode *const restrict node, const float dt){
	const particleSystemDef *const nodeDef = node->container->nodeDef;
	size_t spawnCount = 0;

	particleEmitter *curEmitter = node->emitters;
	const particleEmitter *const lastEmitter = &curEmitter[nodeDef->numEmitters];
	particleEmitterDef *curEmitterDef = nodeDef->emitters;
	// Update all of the emitters,
	for(; curEmitter < lastEmitter; ++curEmitter, ++curEmitterDef){
		// Check how many particles this emitter should spawn.
		spawnCount += particleEmitterUpdate(curEmitter, curEmitterDef, dt);
	}

	// Spawn as many of the emitted particles as we can!
	emitParticles(
		node,
		unitMin(spawnCount, particleManagerRemaining(&node->manager, nodeDef->maxParticles)),
		nodeDef->sortFlags
	);
}

/*
** Sort all of a particle system node's particles!
** By default, particles are automatically stored
** in sorted order if we want to sort by the time
** of creation, so we only need to do anything if
** we're sorting by distance from the camera.
*/
void particleSysNodeUpdateSort(particleSystemNode *const restrict node, const camera *const restrict cam){
	const flags_t sortFlags = node->container->nodeDef->sortFlags;
	if(flagsAreSet(sortFlags, PARTICLE_SORT_DISTANCE)){
		const particleManager manager = node->manager;
		particleKeyValue *const keyValues = memoryManagerGlobalAlloc(
			sizeof(*keyValues) * manager.numParticles
		);
		particle *const sortedArray = memoryManagerGlobalAlloc(
			sizeof(*sortedArray) * manager.numParticles
		);
		if(keyValues == NULL){
			/** MALLOC FAILED **/
		}
		if(sortedArray == NULL){
			/** MALLOC FAILED **/
		}

		// Initialize the new array of key-values!
		if(flagsAreSet(sortFlags, PARTICLE_SORT_REVERSED)){
			const particle *curParticle = manager.particles;
			particleKeyValue *curKeyValue = keyValues;
			unsigned int i;
			// Our sorting functions sort from smallest to greatest, so this will
			// sort particles from nearest to farthest distance from the camera.
			for(i = 0; i < manager.numParticles; ++i, ++curParticle, ++curKeyValue){
				curKeyValue->value = cameraDistanceSquared(cam, &curParticle->subsys.state[0].pos);
				curKeyValue->key = i;
			}
		}else{
			const particle *curParticle = manager.particles;
			particleKeyValue *curKeyValue = keyValues;
			unsigned int i;
			// By simply negating the distance from the camera,
			// we can naively sort from farthest to nearest.
			for(i = 0; i < manager.numParticles; ++i, ++curParticle, ++curKeyValue){
				curKeyValue->value = -cameraDistanceSquared(cam, &curParticle->subsys.state[0].pos);
				curKeyValue->key = i;
			}
		}

		// Sort the key-values!
		timsortKeyValues(keyValues, manager.numParticles);

		// Copy the particles into the new array in sorted order!
		{
			particle *curParticle = sortedArray;
			particleKeyValue *curKeyValue = keyValues;
			const particle *const lastParticle = &sortedArray[manager.numParticles];
			for(; curParticle != lastParticle; ++curParticle, ++curKeyValue){
				particleNode *curChild;

				*curParticle = manager.particles[curKeyValue->key];

				// Update each child nodes' parent pointers.
				for(curChild = curParticle->children; curChild != NULL; curChild = curChild->nextSibling){
					curChild->parent = curParticle;
				}
			}
		}
		memoryManagerGlobalFree(keyValues);
		memoryManagerGlobalFree(node->manager.particles);

		// Overwrite the original array of particles with the new sorted array.
		node->manager.particles = sortedArray;
	}
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
** We assume that the manager has enough room for them.
*/
static void emitParticles(
	particleSystemNode *const restrict node,
	const size_t spawnCount, const flags_t sortFlags
){

	// If we're sorting by youngest to oldest, allocate the
	// new particles at the front of the array. Otherwise,
	// allocate them at the back.
	particle *curParticle = (sortFlags == PARTICLE_SORT_CREATION_REVERSED) ?
		particleManagerAllocFront(&node->manager, spawnCount) :
		particleManagerAllocBack(&node->manager, spawnCount);
	const particle *const lastParticle = &curParticle[spawnCount];

	// Initialize the particles we just allocated!
	for(; curParticle != lastParticle; ++curParticle){
		initializeParticle(node, curParticle);
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