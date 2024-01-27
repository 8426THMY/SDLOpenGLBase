#include "particleSystem.h"


#include <math.h>

#include "utilMath.h"

#include "memoryManager.h"


// Forward-declare any helper functions!
static void initializeParticle(const particleSystem *const restrict partSys, particle *const restrict part);
static void operateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float dt);
static void constrainParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float dt);
static void spawnParticles(particleSystem *const restrict partSys, const size_t numParticles);
static void updateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float dt);

static void updateSysEmitters(particleSystem *const restrict partSys, const float dt);
static void updateSysParticles(particleSystem *const restrict partSys, const float dt);
static void renderSysParticles(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float dt
);


void particleSysDefInit(particleSystemDef *const restrict partSysDef){
	partSysDef->name = NULL;

	particleDefInit(&partSysDef->properties);
	partSysDef->initialParticles = 0;
	partSysDef->maxParticles = 0;

	partSysDef->initializers    = NULL;
	partSysDef->lastInitializer = NULL;
	partSysDef->emitters        = NULL;
	partSysDef->numEmitters     = 0;
	partSysDef->operators       = NULL;
	partSysDef->lastOperator    = NULL;
	partSysDef->constraints     = NULL;
	partSysDef->lastConstraint  = NULL;
	partSysDef->renderers       = NULL;
	partSysDef->lastRenderer    = NULL;

	partSysDef->lifetime = INFINITY;

	partSysDef->children = NULL;
	partSysDef->numChildren = 0;
}

// Instantiate a particle system.
void particleSysInit(particleSystem *const restrict partSys, const particleSystemDef *const restrict partSysDef){
	partSys->partSysDef = partSysDef;

	// Allocate memory for any emitters and set them up.
	if(partSysDef->numEmitters != 0){
		particleEmitter *curEmitter;
		const particleEmitter *lastEmitter;

		partSys->emitters = memoryManagerGlobalAlloc(sizeof(*partSys->emitters) * partSysDef->numEmitters);
		if(partSys->emitters == NULL){
			/** MALLOC FAILED **/
		}

		curEmitter = partSys->emitters;
		lastEmitter = &curEmitter[partSysDef->numEmitters];
		// Initialize the system's emitters.
		do {
			particleEmitterInit(curEmitter);
			++curEmitter;
		} while(curEmitter < lastEmitter);
	}else{
		partSys->emitters = NULL;
	}


	vec3InitZero(&partSys->pos);
	quatInitIdentity(&partSys->rot);
	partSys->lifetime = partSysDef->lifetime;


	// Allocate memory for any particles and set them up.
	if(partSysDef->maxParticles != 0){
		particle *curParticle;
		const particle *lastParticle;
		// Make sure we don't initialize more
		// than the maximum number of particles.
		const size_t initialParticles = uintMin(partSysDef->initialParticles, partSysDef->maxParticles);

		// Allocate enough memory for the maximum number
		// of particles that the system can display.
		partSys->particles = memoryManagerGlobalAlloc(sizeof(*partSys->particles) * partSysDef->maxParticles);
		if(partSys->particles == NULL){
			/** MALLOC FAILED **/
		}
		partSys->numParticles = 0;

		curParticle = partSys->particles;
		lastParticle = &curParticle[initialParticles];
		// Set up any initially active particles
		// using the system's initializers.
		for(; curParticle < lastParticle; ++curParticle){
			initializeParticle(partSys, curParticle);
			++partSys->numParticles;
		}
	}else{
		partSys->particles = NULL;
	}


	// Allocate memory for any child systems and set them up.
	if(partSysDef->numChildren != 0){
		particleSystem *curChild;
		const particleSystem *lastChild;
		const particleSystemDef *curChildDef;

		partSys->children = memoryManagerGlobalAlloc(sizeof(*partSys->children) * partSysDef->numChildren);
		if(partSys->children == NULL){
			/** MALLOC FAILED **/
		}

		curChild = partSys->children;
		lastChild = &curChild[partSysDef->numChildren];
		curChildDef = partSysDef->children;
		// Initialize the system's children.
		do {
			particleSysInit(curChild, curChildDef);
			++curChild;
			++curChildDef;
		} while(curChild < lastChild);
	}else{
		partSys->children = NULL;
	}
}


void particleSysUpdate(particleSystem *const restrict partSys, const float dt){
	particleSystem *firstChild = partSys->children;
	if(firstChild != NULL){
		particleSystem *curChild = &firstChild[partSys->partSysDef->numChildren];
		// Update all of the particle system's children, starting from the last!
		do {
			--curChild;
			particleSysUpdate(curChild, dt);
		} while(curChild > firstChild);
	}

	// Now update the system itself!
	updateSysEmitters(partSys, dt);
	updateSysParticles(partSys, dt);
}

/*
** Draw a particle system and all of its children. The deepest
** child is drawn first and the highest parent is drawn last.
*/
void particleSysDraw(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float dt
){

	const particleSystem *const firstChild = partSys->children;
	if(firstChild != NULL){
		// Draw all of the particle system's children, starting from the last!
		const particleSystem *curChild = &firstChild[partSys->partSysDef->numChildren];
		do {
			--curChild;
			particleSysDraw(curChild, cam, shader, dt);
		} while(curChild > firstChild);
	}

	// Now draw the system itself!
	renderSysParticles(partSys, cam, shader, dt);
}


// Return whether or not a particle system is still alive.
return_t particleSysAlive(particleSystem *const restrict partSys, const float dt){
	partSys->lifetime -= dt;
	return(partSys->lifetime > 0.f);
}


void particleSysDelete(particleSystem *const restrict partSys){
	particleSystem *curChild = partSys->children;

	if(partSys->emitters != NULL){
		memoryManagerGlobalFree(partSys->emitters);
	}

	if(partSys->particles != NULL){
		memoryManagerGlobalFree(partSys->particles);
	}

	if(curChild != NULL){
		const particleSystem *const lastChild = &curChild[partSys->partSysDef->numChildren];
		// Delete the system's children.
		do {
			particleSysDelete(curChild);
			++curChild;
		} while(curChild < lastChild);

		memoryManagerGlobalFree(partSys->children);
	}
}

/*
** Note that when we delete a particle system definition,
** we don't delete its children. There may be other systems
** using those children that we don't want to delete.
*/
void particleSysDefDelete(particleSystemDef *const restrict partSysDef){
	if(partSysDef->name != NULL){
		memoryManagerGlobalFree(partSysDef->name);
	}

	particleDefDelete(&partSysDef->properties);

	if(partSysDef->initializers != NULL){
		memoryManagerGlobalFree(partSysDef->initializers);
	}
	if(partSysDef->emitters != NULL){
		memoryManagerGlobalFree(partSysDef->emitters);
	}
	if(partSysDef->operators != NULL){
		memoryManagerGlobalFree(partSysDef->operators);
	}
	if(partSysDef->constraints != NULL){
		memoryManagerGlobalFree(partSysDef->constraints);
	}
	if(partSysDef->renderers != NULL){
		memoryManagerGlobalFree(partSysDef->renderers);
	}
}


// Execute each of the system's initializers on a particle.
static void initializeParticle(const particleSystem *const restrict partSys, particle *const restrict part){
	const particleSystemDef *const partSysDef = partSys->partSysDef;
	const particleInitializer *curInitializer = partSysDef->initializers;

	particleInit(part, partSysDef->properties.texGroup);

	if(curInitializer != NULL){
		const particleInitializer *const lastInitializer = partSysDef->lastInitializer;
		do {
			(*curInitializer->func)((const void *)(&curInitializer->data), part);
			++curInitializer;
		} while(curInitializer <= lastInitializer);
	}

	// Append the particle system's state.
	vec3AddVec3(&part->state.pos, &partSys->pos);
	quatMultiplyQuatP2(partSys->rot, &part->state.rot);

	#warning "If we do this, we'll need to scale the result of operators."
	#warning "Is this correct, though?"
	// 1. Initialize using the system's initializers.
	// 2. Transform position, scale and velocity using the system's scale (post-scale).
	// 3. Pre-translate and pre-rotate based on system's state.
}

// Execute each of the system's operators on a particle.
static void operateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float dt){
	const particleOperator *curOperator = partSysDef->operators;
	if(curOperator != NULL){
		const particleOperator *const lastOperator = partSysDef->lastOperator;
		do {
			(*curOperator->func)((const void *)(&curOperator->data), part, dt);
			++curOperator;
		} while(curOperator <= lastOperator);
	}
}

// Execute each of the system's constraints on a particle.
static void constrainParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float dt){
	const particleConstraint *curConstraint = partSysDef->constraints;
	if(curConstraint != NULL){
		const particleConstraint *const lastConstraint = partSysDef->lastConstraint;
		do {
			(*curConstraint->func)((const void *)(&curConstraint->data), part, dt);
			++curConstraint;
		} while(curConstraint <= lastConstraint);
	}
}

/*
** Spawn the number of new particles given by "numParticles".
** We assume that the particle system has enough room for them.
*/
static void spawnParticles(particleSystem *const restrict partSys, const size_t numParticles){
	particle *curParticle = &partSys->particles[partSys->numParticles];
	const particle *const lastParticle = &curParticle[numParticles];
	for(; curParticle < lastParticle; ++curParticle){
		initializeParticle(partSys, curParticle);
	}
	partSys->numParticles += numParticles;
}

// Update a particular particle.
static void updateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float dt){
	operateParticle(partSysDef, part, dt);
	constrainParticle(partSysDef, part, dt);
	particleUpdate(part, dt);
}


/*
** Update the emitters in a particle system. This may spawn
** new particles, but it will never destroy old ones.
*/
static void updateSysEmitters(particleSystem *const restrict partSys, const float dt){
	size_t remainingParticles = partSys->partSysDef->maxParticles - partSys->numParticles;

	particleEmitter *curEmitter = partSys->emitters;
	const particleEmitter *const lastEmitter = &curEmitter[partSys->partSysDef->numEmitters];
	particleEmitterDef *curEmitterDef = partSys->partSysDef->emitters;
	// This loop exits when our system reaches its particle limit.
	for(; curEmitter < lastEmitter; ++curEmitter, ++curEmitterDef){
		// Check how many particles this emitter should spawn.
		const size_t spawnCount = particleEmitterUpdate(curEmitter, curEmitterDef, dt);

		// If we still have room, spawn the new particles.
		if(spawnCount < remainingParticles){
			spawnParticles(partSys, spawnCount);
			remainingParticles -= spawnCount;

		// Otherwise, spawn as many as possible and exit the loop.
		}else{
			spawnParticles(partSys, remainingParticles);
			break;
		}
	}
}

// Update a system's particles. This includes executing the operators and constraints.
static void updateSysParticles(particleSystem *const restrict partSys, const float dt){
	const particleSystemDef *const partSysDef = partSys->partSysDef;

	particle *curParticle = partSys->particles;
	particle *freePos = curParticle;
	const particle *const lastParticle = &curParticle[partSys->numParticles];
	// Update every living particle using the system's operators and constraints.
	//
	// Every living particle should be copied back to the first free
	// position in the array to ensure that it doesn't have any gaps.
	#warning "It might be better to use a free list or something rather than copying particles, as they're quite large."
	for(; curParticle < lastParticle; ++curParticle){
		// Update the particle!
		updateParticle(partSysDef, curParticle, dt);

		// If it has died, decrease the particle count.
		if(particleDead(curParticle)){
			--partSys->numParticles;

		// Otherwise, move it to the first free position in the array.
		// This lets us overwrite particles that have died this tick.
		}else{
			if(freePos != curParticle){
				*freePos = *curParticle;
			}
			++freePos;
		}
	}
}

// Draw a particle system using all of its renderers!
static void renderSysParticles(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float dt
){

	const particleSystemDef *const partSysDef = partSys->partSysDef;
	const particleRenderer *curRenderer = partSysDef->renderers;

	if(curRenderer != NULL){
		// These will never change at this point, so
		// we can save a dereference for each renderer.
		const particle *const particles = partSys->particles;
		const size_t numParticles = partSys->numParticles;
		const particleDef *const properties = &partSysDef->properties;

		/** We should interpolate and sort particles here.         */
		/** Each renderer needs to use the interpolated positions, */
		/** but they should all be the same between renders.       */

		const particleRenderer *const lastRenderer = partSysDef->lastRenderer;
		do {
			(*curRenderer->func)(
				(const void *)(&curRenderer->data), particles,
				numParticles, properties,
				cam, shader, dt
			);
			++curRenderer;
		} while(curRenderer <= lastRenderer);
	}
}