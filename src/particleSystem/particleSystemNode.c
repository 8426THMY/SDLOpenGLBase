#include "particleSystemNode.h"


#include <stdint.h>

#include "transform.h"

#include "particleSystemNodeContainer.h"

#include "memoryManager.h"
#include "moduleParticle.h"


// Forward-declare any helper functions!
static void emitParticles(
	particleSystemNode *const restrict node, const size_t spawnCount
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
		} while(curEmitter != lastEmitter);
	}

	// Set up the particle manager and allocate our particle free list.
	particleManagerInit(&node->manager, nodeDef->maxParticles);

	node->lifetime = nodeDef->lifetime;
}


/*
** Update the node's copy of the parent's state.
** This should be called by the node's owner.
*/
void particleSysNodeUpdateParentTransform(
	particleSystemNode *const restrict node,
	const transform *const restrict parentState
){

	const flags16_t flags =
		node->container->nodeDef->flags;
	// Compute the transforms that the particles should inherit.
	if(flagsContainsSet(flags, PARTICLE_INHERIT_POSITION_ALWAYS)){
		node->parentState.pos = parentState->pos;
	}else{
		vec3InitZero(&node->parentState.pos);
	}
	if(flagsContainsSet(flags, PARTICLE_INHERIT_ROTATION_ALWAYS)){
		node->parentState.rot = parentState->rot;
	}else{
		quatInitZero(&node->parentState.rot);
	}
	if(flagsContainsSet(flags, PARTICLE_INHERIT_SCALE_ALWAYS)){
		#ifdef TRANSFORM_MATRIX_SHEAR
		node->parentState.scale = parentState->scale;
		#else
		node->parentState.scale = parentState->scale;
		node->parentState.shear = parentState->shear;
		#endif
	}else{
		#ifdef TRANSFORM_MATRIX_SHEAR
		mat3InitIdentity(&node->parentState->scale);
		#else
		vec3InitSet(&node->parentState.scale, 1.f, 1.f, 1.f);
		quatInitIdentiy(&node->parentState.shear);
		#endif
	}
}

/*
** Update the node's particles. Note that we don't update any of
** the particles' nodes here! These are updated in the main loop.
*/
void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt){
	const particleSubsysDef *const nodeDef = node->container->nodeDef;
	
	particle *curParticle = node->manager.particles;
	const particle *lastParticle = &curParticle[node->manager.numParticles];
	particle *curFreeParticle = NULL;

	// Update all of the node's particles!  Any particles that are
	// dead will be overwritten with the particles that come after
	// them in the array. For some reason, this is just as fast as
	// overwritting them with the last element in the array.
	for(; curParticle != lastParticle; ++curParticle){
		// Delete the particle if it has died. This is
		// done before updating to ensure that particles
		// always live for at least one tick (unless the
		// lifetime is set to initialize to zero).
		#warning "This is a bit pointless, since we always update particles one tick after intialization."
		#warning "I think it would be good to update them on the same tick, though."
		if(particleDead(curParticle)){
			particleManagerFree(&node->manager, curParticle);

			// If this is the first particle we've killed,
			// keep a record of its position in the array!
			if(curFreeParticle != NULL){
				curFreeParticle = curParticle;
			}

		// Update the particle if it survived.
		}else{
			particlePreUpdate(part, dt);
			operateParticle(nodeDef, part, dt);
			constrainParticle(nodeDef, part, dt);
			particlePostUpdate(part, dt);
			// The functions above generate the particle's
			// local transform, so we need to get its global
			// one using the parent's current transform.
			#warning "We probably want the global transform for constraints..."
			#warning "We may need to just set the new global state first, apply the constraints, then undo the parent state at the end."
			particleUpdateGlobalTransform(part, &node->parentState);

			// Move this particle to replace the next dead one.
			// This ensures that prior to any sorting, particles
			// are stored in the order that they were spawned.
			if(curFreeParticle != NULL){
				*curFreeParticle = *curParticle;
				particleSubsysUpdateParentPointers(&curParticle->subsys);
				++curFreeParticle;
			}
		}
	} 
}

// Spawn particles using a node's emitters.
void particleSysNodeUpdateEmitters(particleSystemNode *const restrict node, const float dt){
	const particleSystemDef *const nodeDef = node->container->nodeDef;
	size_t spawnCount = 0;

	particleEmitter *curEmitter = node->emitters;
	const particleEmitter *const lastEmitter = &curEmitter[nodeDef->numEmitters];
	particleEmitterDef *curEmitterDef = nodeDef->emitters;
	// Update all of the emitters, and keep a sum of
	// the number of particles we should emit this tick.
	for(; curEmitter != lastEmitter; ++curEmitter, ++curEmitterDef){
		spawnCount += particleEmitterUpdate(curEmitter, curEmitterDef, dt);
	}

	// Spawn as many of the emitted particles as we can!
	emitParticles(node, particleManagerSpawnCount(&node->manager, nodeDef->maxParticles, spawnCount));
}

// Return whether a particle system node is dead.
return_t particleSysNodeDead(const particleSystemNode *const restrict node){
	const flags16_t flags = node->container->nodeDef->flags;
	return(
		node->lifetime <= 0.f ||
		(flagsContainsSubset(flags, PARTICLE_DELETE_PARENT)   && node->parent == NULL) ||
		(flagsContainsSubset(flags, PARTICLE_DELETE_CHILDREN) && node->manager.numParticles <= 0)
	);
}


/*
** Although particles are sorted during rendering, we also
** sort them after updating. Because rendering should never
** modify the state, the hope is that presorting after each
** update should mean that the sorting during rendering is
** essentially free.
**
** Note that by default, particles are automatically stored
** in order if we want to sort by the time of creation, so
** we only need to do anything if we're sorting by distance.
*/
void particleSysNodePresort(
	particleSystemNode *const restrict node,
	const camera *const restrict cam
){

	const flags16_t flags = node->container->nodeDef->flags;
	if(flagsContainsSubset(flags, PARTICLE_SORT_DISTANCE)){
		const particleManager manager = node->manager;
		// Our sorting functions sort values from smallest to
		// largest, whereas particles are typically sorted
		// from largest distance to smallest (back to front).
		// To fix this, we multiply the distance by -1 unless
		// the sorting is supposed to be reversed.
		const float sortSign =
			flagsContainsSubset(flags, PARTICLE_SORT_REVERSED) ? 1.f : -1.f;
		return_t sorted = 1;

		// Particles are pretty big, so it's more efficient
		// to first sort an array of smaller key-values.
		keyValue *const keyValues = memoryManagerGlobalAlloc(
			sizeof(*keyValues) * manager.numParticles
		);
		if(keyValues == NULL){
			/** MALLOC FAILED **/
		}

		// Initialize the new array of key-values!
		{
			const particle *curParticle = manager.particles;
			keyValue *curKeyValue  = keyValues;
			keyValue *prevKeyValue = keyValues;
			unsigned int i;
			// Our sorting functions sort from smallest to greatest, so this will
			// sort particles from nearest to farthest distance from the camera.
			for(i = 0; i < manager.numParticles; ++i, ++curParticle, ++curKeyValue){
				curKeyValue->key   = sortSign*cameraDistanceSquared(cam, &curParticle->state.pos);
				curKeyValue->value = curParticle;

				// If the previous element should come after the
				// current one, the array will need to be sorted.
				if(sorted && keyValueCompare(prevKeyValue->key, curKeyValue->key) == SORT_COMPARE_GREATER){
					sorted = 0;
				}
				prevKeyValue = curKeyValue;
			}
		}

		// Sort the array of any elements are out of sequence.
		if(!sorted){
			// Once we've sorted the key-values, we can simply copy
			// the particles into this new array in sorted order.
			particle *const sortedArray = memoryManagerGlobalAlloc(
				sizeof(*sortedArray) * manager.numParticles
			);
			if(sortedArray == NULL){
				/** MALLOC FAILED **/
			}

			// Sort the key-values!
			timsortKeyValues(keyValues, manager.numParticles);

			// Copy the particles into the new array in sorted order!
			{
				particle *curParticle = sortedArray;
				keyValue *curKeyValue = keyValues;
				const particle *const lastParticle = &sortedArray[manager.numParticles];
				for(; curParticle != lastParticle; ++curParticle, ++curKeyValue){
					particleSystemNode *curChild;

					*curParticle = *((particle *)curKeyValue->value);
					particleSubsysUpdateParentPointers(&curParticle->subsys);
				}
			}
			memoryManagerGlobalFree(keyValues);
			memoryManagerGlobalFree(node->manager.particles);

			// Overwrite the original array of particles with the new sorted array.
			node->manager.particles = sortedArray;
		}

		memoryManagerGlobalFree(keyValues);
	}
}

/*
** Particles need to be sorted when rendering, but this time
** we only generate a sorted array of key-values. This array
** is used by the rendering functions to iterate through the
** particles in the right order.
*/
keyValue *const void particleSysNodeSort(
	particleSystemNode *const restrict node,
	const camera *const restrict cam, const float dt
){

	const particleManager manager = node->manager;
	const flags16_t flags = node->container->nodeDef->flags;
	// Our sorting functions sort values from smallest to
	// largest, whereas particles are typically sorted
	// from largest distance to smallest (back to front).
	// To fix this, we multiply the distance by -1 unless
	// the sorting is supposed to be reversed.
	const float sortSign =
		flagsContainsSubset(flags, PARTICLE_SORT_REVERSED) ? 1.f : -1.f;
	return_t sorted = 1;

	// Particles are pretty big, so it's more efficient
	// to first sort an array of smaller key-values.
	keyValue *const keyValues = memoryManagerGlobalAlloc(
		sizeof(*keyValues) * manager.numParticles
	);
	if(keyValues == NULL){
		/** MALLOC FAILED **/
	}

	// Initialize the new array of key-values!
	{
		const particle *curParticle = manager.particles;
		keyValue *curKeyValue  = keyValues;
		keyValue *prevKeyValue = keyValues;
		unsigned int i;
		// Our sorting functions sort from smallest to greatest, so this will
		// sort particles from nearest to farthest distance from the camera.
		for(i = 0; i < manager.numParticles; ++i, ++curParticle, ++curKeyValue){
			vec3 interpPos;
			vec3Lerp(
				&curParticle->state.pos,
				&curParticle->prevState.pos,
				dt, &interpPos
			);
			curKeyValue->key   = sortSign*cameraDistanceSquared(cam, &interpPos);
			curKeyValue->value = curParticle;

			// If the previous element should come after the
			// current one, the array will need to be sorted.
			if(sorted && keyValueCompare(prevKeyValue->key, curKeyValue->key) == SORT_COMPARE_GREATER){
				sorted = 0;
			}
			prevKeyValue = curKeyValue;
		}
	}

	// The key-values also only need to be
	// sorted if we're sorting by distance.
	if(!sorted && flagsContainsSubset(flags, PARTICLE_SORT_DISTANCE)){
		timsortFlexibleKeyValues(keyValues, manager.numParticles);
	}
	
	return(keyValues);
}


void particleSysNodeOrphan(particleSystemNode *const restrict node){
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


/*
** Spawn the number of new particles given by "spawnCount".
** We assume that the manager has enough room for them.
*/
static void emitParticles(
	particleSystemNode *const restrict node, const size_t spawnCount
){

	const particleSystemNodeDef *nodeDef = node->container->nodeDef;
	const flags16_t flags = nodeDef->flags;

	// If we're sorting by youngest to oldest, allocate the
	// new particles at the front of the array. Otherwise,
	// allocate them at the back.
	particle *curParticle = flagsContainsSet(flags, PARTICLE_SORT_CREATION_REVERSED) ?
		particleManagerAllocFront(&node->manager, spawnCount) :
		particleManagerAllocBack(&node->manager, spawnCount);
	const particle *const lastParticle = &curParticle[spawnCount];

	// Initialize the particles we just allocated!
	for(; curParticle != lastParticle; ++curParticle){
		const particleInitializer *curInitializer = nodeDef->initializers;
		const particleInitializer *const lastInitializer = nodeDef->lastInitializer;

		particleInit(part, node->container->children, nodeDef->numChildren);

		// Run through all of the initializers for this subsystem.
		for(; curInitializer != lastInitializer; ++curInitializer);
			(*curInitializer->func)((const void *)(&curInitializer->data), part);
		}

		// Now that we've initialized the particle,
		// we need to get its global transform.
		particleUpdateGlobalTransform(part, &node->parentState);
	}
}


// Execute each of the node's operators on a particle.
static void operateParticle(
	const particleSystemNodeDef *const restrict nodeDef,
	particle *const restrict part, const float dt
){

	const particleOperator *curOperator = nodeDef->operators;
	const particleOperator *const lastOperator = nodeDef->lastOperator;
	for(; curOperator != lastOperator; ++curOperator){
		(*curOperator->func)((const void *)(&curOperator->data), part, dt);
	}
}

// Execute each of the node's constraints on a particle.
static void constrainParticle(
	const particleSystemNodeDef *const restrict nodeDef,
	particle *const restrict part, const float dt
){

	const particleConstraint *curConstraint = nodeDef->constraints;
	const particleConstraint *const lastConstraint = nodeDef->lastConstraint;
	for(; curConstraint != lastConstraint; ++curConstraint);
		(*curConstraint->func)((const void *)(&curConstraint->data), part, dt);
	}
}