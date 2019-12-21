#include "particleSystem.h"


#include <math.h>


#include "memoryManager.h"


// Forward-declare any helper functions!
static void initializeParticle(const particleSystemDef *partSysDef, particle *particle);
static void operateParticle(const particleSystemDef *partSysDef, particle *particle, const float time);
static void constrainParticle(const particleSystemDef *partSysDef, particle *particle, const float time);
static void spawnParticles(particleSystem *partSys, const size_t numParticles);

static void updateEmitters(particleSystem *partSys, const float time);
static void updateParticles(particleSystem *partSys, const float time);


void particleSysDefInit(particleSystemDef *partSysDef){
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

	partSysDef->lifetime = INFINITY;

	partSysDef->children = NULL;
	partSysDef->numChildren = 0;
}

// Instantiate a particle system definition.
void particleSysInit(particleSystem *partSys, particleSystemDef *partSysDef){
	particle *curParticle;
	const particle *lastParticle;
	particleSystem *curChild;
	const particleSystem *lastChild;
	particleSystemDef *curChildDef;

	partSys->partSysDef = partSysDef;
	partSys->emitters = memoryManagerGlobalAlloc(sizeof(*partSys->emitters) * partSysDef->numEmitters);

	vec3InitZero(&partSys->pos);
	quatInitIdentity(&partSys->rot);
	partSys->lifetime = partSysDef->lifetime;

	partSys->particles = memoryManagerGlobalAlloc(sizeof(*partSys->particles) * partSysDef->maxParticles);
	curParticle = partSys->particles;
	lastParticle = &curParticle[partSysDef->maxParticles];
	// Initialize the system's particles.
	for(; curParticle < lastParticle; ++curParticle){
		particleInit(curParticle);
	}
	partSys->numParticles = 0;

	partSys->children = memoryManagerGlobalAlloc(sizeof(*partSys->children) * partSysDef->numChildren);
	curChild = partSys->children;
	lastChild = &curChild[partSysDef->numChildren];
	curChildDef = partSysDef->children;
	// Initialize the system's children.
	for(; curChild < lastChild; ++curChild, ++curChildDef){
		particleSysInit(curChild, curChildDef);
	}
}


void particleSysUpdate(particleSystem *partSys, const float time){
	particleSystem *curChild;
	const particleSystem *lastChild;

	updateEmitters(partSys, time);
	updateParticles(partSys, time);
	// If we keep track of the number of paricles that have died, we
	// can avoid sorting the dead particles at the end of the array.
	// sort particles

	curChild = partSys->children;
	lastChild = &partSys->children[partSys->partSysDef->numChildren];
	// Update the system's children!
	for(; curChild < lastChild; ++curChild){
		particleSysUpdate(curChild, time);
	}
}

void particleSysDraw(const particleSystem *partSys, const float time){
	// bind texture
	// upload particle data to shader
}


// Return whether or not a particle system is still alive.
return_t particleSysAlive(particleSystem *partSys, const float time){
	partSys->lifetime -= time;
	return(partSys->lifetime > 0.f);
}


void particleSysDelete(particleSystem *partSys){
	particleSystem *curChild;
	const particleSystem *lastChild;

	if(partSys->emitters != NULL){
		memoryManagerGlobalFree(partSys->emitters);
	}

	if(partSys->particles != NULL){
		memoryManagerGlobalFree(partSys->particles);
	}

	if(partSys->children != NULL){
		curChild = partSys->children;
		lastChild = &curChild[partSys->partSysDef->numChildren];
		// Delete the system's children.
		for(; curChild < lastChild; ++curChild){
			particleSysDelete(curChild);
		}

		memoryManagerGlobalFree(partSys->children);
	}
}

/*
** Note that when we delete a particle system definition,
** we don't delete its children. There may be other systems
** using those children that we don't want to delete.
*/
void particleSysDefDelete(particleSystemDef *partSysDef){
	if(partSysDef->name != NULL){
		memoryManagerGlobalFree(partSysDef->name);
	}

	particleDefDelete(&partSysDef->properties);

	if(partSysDef->initializers != NULL){
		memoryManagerGlobalFree(partSysDef->initializers);
	}
	if(partSysDef->initializers != NULL){
		memoryManagerGlobalFree(partSysDef->emitters);
	}
	if(partSysDef->initializers != NULL){
		memoryManagerGlobalFree(partSysDef->operators);
	}
	if(partSysDef->initializers != NULL){
		memoryManagerGlobalFree(partSysDef->constraints);
	}
}


// Execute each of the system's initializers on a particle.
static void initializeParticle(const particleSystemDef *partSysDef, particle *particle){
	particleInitializer *curInitializer = partSysDef->initializers;
	for(; curInitializer < partSysDef->lastInitializer; ++curInitializer){
		(*curInitializer->func)((void *)(&curInitializer->data), particle);
	}
}

// Execute each of the system's operators on a particle.
static void operateParticle(const particleSystemDef *partSysDef, particle *particle, const float time){
	particleOperator *curOperator = partSysDef->operators;
	for(; curOperator < partSysDef->lastOperator; ++curOperator){
		(*curOperator->func)((void *)(&curOperator->data), particle, time);
	}
}

// Execute each of the system's constraints on a particle.
static void constrainParticle(const particleSystemDef *partSysDef, particle *particle, const float time){
	particleConstraint *curConstraint = partSysDef->constraints;
	for(; curConstraint < partSysDef->lastConstraint; ++curConstraint){
		(*curConstraint->func)((void *)(&curConstraint->data), particle, time);
	}
}

/*
** Spawn the number of new particles given by "numParticles".
** We assume that the particle system has enough room for them.
*/
static void spawnParticles(particleSystem *partSys, const size_t numParticles){
	particle *curParticle = &partSys->particles[partSys->numParticles];
	const particle *lastParticle = &curParticle[numParticles];
	for(; curParticle < lastParticle; ++curParticle){
		initializeParticle(partSys->partSysDef, curParticle);
	}
	partSys->numParticles += numParticles;
}


/*
** Update the emitters in a particle system. This may spawn
** new particles, but it will never destroy old ones.
*/
static void updateEmitters(particleSystem *partSys, const float time){
	size_t remainingParticles = partSys->partSysDef->maxParticles - partSys->numParticles;

	particleEmitter *curEmitter = partSys->emitters;
	const particleEmitter *lastEmitter = &curEmitter[partSys->partSysDef->numEmitters];
	// This loop exits when our system reaches its particle limit.
	for(; curEmitter < lastEmitter; ++curEmitter){
		// Check how many particles this emitter should spawn.
		const size_t numParticles = particleEmitterUpdate(curEmitter, time);

		// If we still have room, spawn the new particles.
		if(numParticles < remainingParticles){
			spawnParticles(partSys, numParticles);
			remainingParticles -= numParticles;

		// Otherwise, spawn as many as possible and exit the loop.
		}else{
			spawnParticles(partSys, remainingParticles);
			break;
		}
	}
}

// Update a system's particles. This includes executing the operators and constraints.
static void updateParticles(particleSystem *partSys, const float time){
	const particleSystemDef *partSysDef = partSys->partSysDef;

	particle *curParticle = partSys->particles;
	const particle *lastParticle = &curParticle[partSys->numParticles];
	// Update every living particles using
	// the system's operators and constraints.
	for(; curParticle < lastParticle && curParticle->lifetime > 0.f; ++curParticle){
		// If the particle hasn't died, update it!
		if(particleAlive(curParticle, time)){
			// Note that the particle may die while we're updating it.
			operateParticle(partSysDef, curParticle, time);
			constrainParticle(partSysDef, curParticle, time);
			particleUpdate(curParticle, time);

		// Otherwise, we should kill the particle.
		}else{
			particleDelete(curParticle);
		}
	}
}