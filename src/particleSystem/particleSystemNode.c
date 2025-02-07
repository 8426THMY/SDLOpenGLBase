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


#if 0
/*
** Remove any properties of a node's parent state
** that aren't being inherited. Since the node doesn't
** store the parent's previous state, we update a
** temporary copy that is passed into the function.
*/
void particleSysNodeUpdateParentTransform(
	particleSystemNode *const restrict node,
	transform *const restrict prevParentState
){

	if(node->parent != NULL){
		const flags8_t inheritFlags = node->inheritFlags;
		// Calculate the where the particle would have
		// been had it been alive on the previous frame.
		// The subsystem pointer should never be NULL here.
		if(flagsContainsSubset(inheritFlags, PARTICLE_INHERIT_TRANSFORM)){
			if(flagsContainsSubset(inheritFlags, PARTICLE_INHERIT_POSITION)){
				node->parentState.pos = node->parent.state.pos;
				prevParentState->pos = node->parent.prevState.pos;
			}else{
				vec3InitZero(&node->parentState.pos);
				vec3InitZero(&prevParentState->pos);
			}
			if(flagsContainsSubset(inheritFlags, PARTICLE_INHERIT_ROTATION)){
				node->parentState.rot = node->parent.state.rot;
				prevParentState->rot = node->parent.prevState.rot;
			}else{
				quatInitIdentiy(&node->parentState.rot);
				quatInitIdentiy(&prevParentState->rot);
			}
			if(flagsContainsSubset(inheritFlags, PARTICLE_INHERIT_SCALE)){
				#ifdef TRANSFORM_MATRIX_SHEAR
				node->parentState.scale = node->parent.state.scale;
				prevParentState->scale = node->parent.prevState.scale;
				#else
				node->parentState.scale = node->parent.state.scale;
				node->parentState.shear = node->parent.state.shear;
				prevParentState->scale = node->parent.prevState.scale;
				prevParentState->shear = node->parent.prevState.shear;
				#endif
			}else{
				#ifdef TRANSFORM_MATRIX_SHEAR
				mat3InitIdentity(&node->parentState.scale);
				mat3InitIdentity(&prevParentState->scale);
				#else
				vec3InitSet(&node->parentState.scale, 1.f, 1.f, 1.f);
				quatInitIdentiy(&node->parentState.shear);
				vec3InitSet(&prevParentState->scale, 1.f, 1.f, 1.f);
				quatInitIdentiy(&prevParentState->shear);
				#endif
			}
		}else{
			transformInit(&node->parentState);
			transformInit(prevParentState);
		}
	}else{
		*prevParentState = node->parentState;
	}
}
#endif

/*
** Update the node's particles. Note that we don't update any of
** the particles' nodes here! These are updated in the main loop.
*/
void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt){
	const particleSubsysDef *const nodeDef = node->container->nodeDef;
	
	particle *curParticle = node->manager.particles;
	const particle *lastParticle = &curParticle[node->manager.numParticles];
	particle *curFreeParticle = NULL;

	const transform *const prevParentState = (node->parent == NULL) ?
		&node->parentState :
		&node->parent->prevState;
	transform curInheritState;
	transform prevInheritState;
	
	// Compute the transforms that the particles should inherit.
	if(flagsContainsSet(flags, PARTICLE_INHERIT_POSITION_ALWAYS)){
		curInheritState.pos  = node->parentState.pos;
		prevInheritState.pos = prevParentState->pos;
	}else{
		vec3InitZero(&curInheritState.pos);
		vec3InitZero(&prevInheritState.pos);
	}
	if(flagsContainsSet(flags, PARTICLE_INHERIT_ROTATION_ALWAYS)){
		curInheritState.rot  = node->parentState.rot;
		prevInheritState.rot = prevParentState->rot;
	}else{
		quatInitZero(&curInheritState.rot);
		quatInitZero(&prevInheritState.rot);
	}
	if(flagsContainsSet(flags, PARTICLE_INHERIT_SCALE_ALWAYS)){
		#ifdef TRANSFORM_MATRIX_SHEAR
		curInheritState.scale = node->parentState.scale;
		prevInheritState.scale = prevParentState->scale;
		#else
		curInheritState.scale = node->parentState.scale;
		curInheritState.shear = node->parentState.shear;
		prevInheritState.scale = prevParentState->scale;
		prevInheritState.shear = prevParentState->shear;
		#endif
	}else{
		#ifdef TRANSFORM_MATRIX_SHEAR
		mat3InitIdentity(&curInheritState.scale);
		mat3InitIdentity(&prevParentState.scale);
		#else
		vec3InitSet(&curInheritState.scale, 1.f, 1.f, 1.f);
		quatInitIdentiy(&curInheritState.shear);
		vec3InitSet(&prevInheritState.scale, 1.f, 1.f, 1.f);
		quatInitIdentiy(&prevInheritState.shear);
		#endif
	}

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
			particleUpdateGlobalTransform(part, &curInheritState, &prevInheritState);

			// Move this particle to replace the next dead one.
			// This ensures that prior to any sorting, particles
			// are stored in the order that they were spawned.
			if(curFreeParticle != NULL){
				*curFreeParticle = *curParticle;
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
		// Keep a reference to the comparison function so
		// we don't have to keep checking which one to use.
		// Note that the comparison functions look backwards
		// because our sorting functions go from smallest to
		// largest, whereas particles are typically sorted
		// from largest distance to smallest (back to front).
		compareFunc compare = flagsContainsSubset(flags, PARTICLE_SORT_REVERSED) ?
			&keyValueCompare :
			&keyValueCompareReversed;
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
			keyValue *curKeyValue = keyValues;
			keyValue *prevKeyValue = keyValues;
			unsigned int i;
			// Our sorting functions sort from smallest to greatest, so this will
			// sort particles from nearest to farthest distance from the camera.
			for(i = 0; i < manager.numParticles; ++i, ++curParticle, ++curKeyValue){
				curKeyValue->value = cameraDistanceSquared(cam, &curParticle->state.pos);
				curKeyValue->key = curParticle;

				// If the previous element should come after the
				// current one, the array will need to be sorted.
				if((*compare)(prevKeyValue->value, curKeyValue->value) == SORT_COMPARE_GREATER){
					sorted = 0;
				}
				prevKeyValue = curKeyValue;
			}
		}

		// If the array is already sorted, we can exit early.
		if(sorted){
			memoryManagerGlobalFree(keyValues);
		}else{
			// Once we've sorted the key-values, we can simply copy
			// the particles into this new array in sorted order.
			particle *const sortedArray = memoryManagerGlobalAlloc(
				sizeof(*sortedArray) * manager.numParticles
			);
			if(sortedArray == NULL){
				/** MALLOC FAILED **/
			}

			// Sort the key-values!
			timsortFlexibleKeyValues(keyValues, manager.numParticles, compare);

			// Copy the particles into the new array in sorted order!
			{
				particle *curParticle = sortedArray;
				keyValue *curKeyValue = keyValues;
				const particle *const lastParticle = &sortedArray[manager.numParticles];
				for(; curParticle != lastParticle; ++curParticle, ++curKeyValue){
					particleSystemNode *curChild;

					*curParticle = *((particle *)curKeyValue->key);

					// Update each child nodes' parent pointers.
					for(curChild = curParticle->subsys.nodes; curChild != NULL; curChild = curChild->nextSibling){
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
	// Keep a reference to the comparison function so
	// we don't have to keep checking which one to use.
	// Note that the comparison functions look backwards
	// because our sorting functions go from smallest to
	// largest, whereas particles are typically sorted
	// from largest distance to smallest (back to front).
	compareFunc compare = flagsContainsSubset(flags, PARTICLE_SORT_REVERSED) ?
		&keyValueCompare :
		&keyValueCompareReversed;
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
		keyValue *curKeyValue = keyValues;
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
			curKeyValue->value = cameraDistanceSquared(cam, &interpPos);
			curKeyValue->key = curParticle;

			// If the previous element should come after the
			// current one, the array will need to be sorted.
			if((*compare)(prevKeyValue->value, curKeyValue->value) == SORT_COMPARE_GREATER){
				sorted = 0;
			}
			prevKeyValue = curKeyValue;
		}
	}

	// The key-values also only need to be
	// sorted if we're sorting by distance.
	if(!sorted && flagsContainsSubset(flags, PARTICLE_SORT_DISTANCE)){
		timsortFlexibleKeyValues(keyValues, manager.numParticles, compare);
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
		particleSubsysRemove(&node->parent.subsys, node);
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

	const transform *const prevParentState = (node->parent == NULL) ?
		&node->parentState :
		&node->parent->prevState;
	transform curInheritState;
	transform prevInheritState;
	
	// Compute the transforms that the particles should inherit.
	if(flagsContainsSubset(flags, PARTICLE_INHERIT_POSITION_CREATE)){
		curInheritState.pos  = node->parentState.pos;
		prevInheritState.pos = prevParentState->pos;
	}else{
		vec3InitZero(&curInheritState.pos);
		vec3InitZero(&prevInheritState.pos);
	}
	if(flagsContainsSubset(flags, PARTICLE_INHERIT_ROTATION_CREATE)){
		curInheritState.rot  = node->parentState.rot;
		prevInheritState.rot = prevParentState->rot;
	}else{
		quatInitZero(&curInheritState.rot);
		quatInitZero(&prevInheritState.rot);
	}
	if(flagsContainsSubset(flags, PARTICLE_INHERIT_SCALE_CREATE)){
		#ifdef TRANSFORM_MATRIX_SHEAR
		curInheritState.scale = node->parentState.scale;
		prevInheritState.scale = prevParentState->scale;
		#else
		curInheritState.scale = node->parentState.scale;
		curInheritState.shear = node->parentState.shear;
		prevInheritState.scale = prevParentState->scale;
		prevInheritState.shear = prevParentState->shear;
		#endif
	}else{
		#ifdef TRANSFORM_MATRIX_SHEAR
		mat3InitIdentity(&curInheritState.scale);
		mat3InitIdentity(&prevParentState.scale);
		#else
		vec3InitSet(&curInheritState.scale, 1.f, 1.f, 1.f);
		quatInitIdentiy(&curInheritState.shear);
		vec3InitSet(&prevInheritState.scale, 1.f, 1.f, 1.f);
		quatInitIdentiy(&prevInheritState.shear);
		#endif
	}

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
		particleUpdateGlobalTransform(part, &curInheritState, &prevInheritState);
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