#include "particleSystem.h"


#include <math.h>


// Forward-declare any helper functions!
static void initializeParticle(const particleSystemDef *partSysDef, particle *particle);
static void operateParticle(const particleSystemDef *partSysDef, particle *particle, const float time);
static void constrainParticle(const particleSystemDef *partSysDef, particle *particle, const float time);
static void updateEmitters(const particleSystem *partSys, const float time);
static void updateParticles(const particleSystem *partSys, const float time);


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


void particleSysUpdate(particleSystem *partSys, const float time){
	updateEmitters(partSys, time);
	updateParticles(partSys, time);
}

void particleSysDraw(const particleSystem *partSys, const float time){
	//
}


// Execute each of the system's initializers on a particle.
static void initializeParticle(const particleSystemDef *partSysDef, particle *particle){
	particleInitializer *curInitializer = partSysDef->initializers;
	for(; curInitializer < partSysDef->lastInitializer; ++curInitializer){
		// execute on the particle
	}
}

// Execute each of the system's operators on a particle.
static void operateParticle(const particleSystemDef *partSysDef, particle *particle, const float time){
	particleOperator *curOperator = partSysDef->operators;
	for(; curOperator < partSysDef->lastOperator; ++curOperator){
		// execute on the particle
	}
}

// Execute each of the system's constraints on a particle.
static void constrainParticle(const particleSystemDef *partSysDef, particle *particle, const float time){
	particleConstraint *curConstraint = partSysDef->constraints;
	for(; curConstraint < partSysDef->lastConstraint; ++curConstraint){
		// execute on the particle
	}
}


/*
** Update the emitters in a particle system.
** This may cause particles to be spawned or destroyed.
*/
static void updateEmitters(const particleSystem *partSys, const float time){
	const particleSystemDef *partSysDef = partSys->partSysDef;

	particleEmitter *curEmitter = partSys->emitters;
	const particleEmitter *lastEmitter = &curEmitter[partSysDef->numEmitters];
	for(; curEmitter < lastEmitter; ++curEmitter){
		//particleEmitterUpdate(curEmitter);
		//initializeParticle(partSysDef, newParticles);
	}
}

// Update a particle. This includes executing the operators and constraints.
static void updateParticles(const particleSystem *partSys, const float time){
	const particleSystemDef *partSysDef = partSys->partSysDef;

	particle *curParticle = partSys->particles;
	const particle *lastParticle = &curParticle[partSys->numParticles];
	// Update all of the particles using the
	// system's operators and constraints.
	for(; curParticle < lastParticle; ++curParticle){
		// If the particle couldn't be updated, we can remove it.
		if(!particleUpdate(curParticle, time)){
			// kill the particle
		}

		operateParticle(partSysDef, curParticle, time);
		constrainParticle(partSysDef, curParticle, time);
	}
}