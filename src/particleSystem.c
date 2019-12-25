#include "particleSystem.h"


#include <math.h>

#include "rectangle.h"
#include "sprite.h"

#include "memoryManager.h"


// Forward-declare any helper functions!
static void initializeParticle(const particleSystem *partSys, particle *part);
static void operateParticle(const particleSystemDef *partSysDef, particle *part, const float time);
static void constrainParticle(const particleSystemDef *partSysDef, particle *part, const float time);
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
void particleSysInit(particleSystem *partSys, const particleSystemDef *partSysDef){
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
		for(; curEmitter < lastEmitter; ++curEmitter){
			particleEmitterInit(curEmitter);
		}
	}


	transformStateInit(&partSys->state);
	partSys->lifetime = partSysDef->lifetime;


	// Allocate memory for any particles and set them up.
	if(partSysDef->maxParticles != 0){
		particle *curParticle;
		const particle *lastParticle;

		partSys->particles = memoryManagerGlobalAlloc(sizeof(*partSys->particles) * partSysDef->maxParticles);
		if(partSys->particles == NULL){
			/** MALLOC FAILED **/
		}

		curParticle = partSys->particles;
		lastParticle = &curParticle[partSysDef->maxParticles];
		// Initialize the system's particles.
		for(; curParticle < lastParticle; ++curParticle){
			particleInit(curParticle);

			// Set up any initially active particles using the system's initializers.
			if(partSys->numParticles < partSysDef->initialParticles){
				initializeParticle(partSys, curParticle);
				++partSys->numParticles;
			}
		}
	}


	// Allocate memory for any child systems and set them up.
	if(partSysDef->numChildren != 0){
		particleSystem *curChild;
		const particleSystem *lastChild;
		particleSystemDef *curChildDef;

		partSys->children = memoryManagerGlobalAlloc(sizeof(*partSys->children) * partSysDef->numChildren);
		if(partSys->children == NULL){
			/** MALLOC FAILED **/
		}

		curChild = partSys->children;
		lastChild = &curChild[partSysDef->numChildren];
		curChildDef = partSysDef->children;
		// Initialize the system's children.
		for(; curChild < lastChild; ++curChild, ++curChildDef){
			particleSysInit(curChild, curChildDef);
		}
	}
}


void particleSysUpdate(particleSystem *partSys, const float time){
	particleSystem *curChild;
	const particleSystem *lastChild;

	updateEmitters(partSys, time);
	updateParticles(partSys, time);
	// If we keep track of the number of paricles that have died, we
	// can avoid sorting the dead particles at the end of the array.
	/** SORT PARTICLES HERE **/

	curChild = partSys->children;
	lastChild = &partSys->children[partSys->partSysDef->numChildren];
	// Update the system's children!
	for(; curChild < lastChild; ++curChild){
		particleSysUpdate(curChild, time);
	}
}

void particleSysDraw(const particleSystem *partSys, mat4 viewProjectionMatrix, const shader *shaderPrg, const float time){
	const sprite *particleSprite = &partSys->partSysDef->properties.spriteData;

	particle *curParticle = partSys->particles;
	const particle *lastParticle = &curParticle[partSys->numParticles];

	mat4 particleStates[SPRITE_MAX_INSTANCES];
	mat4 *curState = particleStates;
	rectangle particleUVs[SPRITE_MAX_INSTANCES];
	rectangle *curUV = particleUVs;


	// Send the view projection matrix to the shader!
	#warning "Maybe do this outside since it applies to all particle systems?"
	glUniformMatrix4fv(shaderPrg->mvpMatrixID, 1, GL_FALSE, (GLfloat *)&viewProjectionMatrix);

	// Bind the sprite we're using!
	glBindVertexArray(particleSprite->vertexArrayID);

	glActiveTexture(GL_TEXTURE0);
	// Bind the texture that this system uses!
	glBindTexture(GL_TEXTURE_2D, texDefault.id);


	// Store our particle data in the array.
	#warning "This is pretty bad, specifically creating the textureGroupState."
	for(; curParticle < lastParticle; ++curParticle, ++curState, ++curUV){
		const textureGroupFrame *texFrame;
		const textureGroupState texState = {
			.texGroup = partSys->partSysDef->properties.texGroup,
			.currentAnim = curParticle->currentAnim,
			.texGroupAnim = curParticle->texAnim
		};

		// Convert the particle's state to a matrix!
		transformStateToMat4(&curParticle->state, curState);

		// Get the particle's UV coordinates!
		texFrame = texGroupStateGetFrame(&texState);
		*curUV = texFrame->bounds;
	}

	#warning "This may be a problem, especially if other systems are using the same sprite."
	// Upload the particles' states to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, particleSprite->stateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particleStates[0]) * partSys->numParticles, &particleStates[0]);
	// Upload the particles' UV offsets to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, particleSprite->uvBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particleUVs[0]) * partSys->numParticles, particleUVs);


	// Draw each instance of the particle!
	glDrawElementsInstanced(GL_TRIANGLES, particleSprite->numIndices, GL_UNSIGNED_INT, NULL, partSys->numParticles);


	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
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
	if(partSysDef->emitters != NULL){
		memoryManagerGlobalFree(partSysDef->emitters);
	}
	if(partSysDef->operators != NULL){
		memoryManagerGlobalFree(partSysDef->operators);
	}
	if(partSysDef->constraints != NULL){
		memoryManagerGlobalFree(partSysDef->constraints);
	}
}


// Execute each of the system's initializers on a particle.
static void initializeParticle(const particleSystem *partSys, particle *part){
	const particleSystemDef *partSysDef = partSys->partSysDef;

	particleInitializer *curInitializer = partSysDef->initializers;
	for(; curInitializer < partSysDef->lastInitializer; ++curInitializer){
		particleInit(part);
		(*curInitializer->func)((void *)(&curInitializer->data), part);
	}

	#warning "If we do this, we'll need to scale the result of operators."
	// 1. Initialize using the system's initializers.
	// 2. Transform position, scale and velocity using the system's scale (post-scale).
	// 3. Pre-translate and pre-rotate based on system's state.
}

// Execute each of the system's operators on a particle.
static void operateParticle(const particleSystemDef *partSysDef, particle *part, const float time){
	particleOperator *curOperator = partSysDef->operators;
	for(; curOperator < partSysDef->lastOperator; ++curOperator){
		(*curOperator->func)((void *)(&curOperator->data), part, time);
	}
}

// Execute each of the system's constraints on a particle.
static void constrainParticle(const particleSystemDef *partSysDef, particle *part, const float time){
	particleConstraint *curConstraint = partSysDef->constraints;
	for(; curConstraint < partSysDef->lastConstraint; ++curConstraint){
		(*curConstraint->func)((void *)(&curConstraint->data), part, time);
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
		initializeParticle(partSys, curParticle);
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
	particleEmitterDef *curEmitterDef = partSys->partSysDef->emitters;
	// This loop exits when our system reaches its particle limit.
	for(; curEmitter < lastEmitter; ++curEmitter, ++curEmitterDef){
		// Check how many particles this emitter should spawn.
		const size_t spawnCount = particleEmitterUpdate(curEmitter, curEmitterDef, time);

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
static void updateParticles(particleSystem *partSys, const float time){
	const particleSystemDef *partSysDef = partSys->partSysDef;

	particle *curParticle = partSys->particles;
	const particle *lastParticle = &curParticle[partSys->numParticles];
	// Update every living particle using the system's operators and constraints.
	// Note that because emitters cannot destroy particles, they're all alive.
	for(; curParticle < lastParticle; ++curParticle){
		// If the particle hasn't died, update it!
		if(particleAlive(curParticle, time)){
			// Note that the particle may die while we're updating it.
			operateParticle(partSysDef, curParticle, time);
			constrainParticle(partSysDef, curParticle, time);
			particleUpdate(curParticle, time);

		// Otherwise, we should kill the particle.
		}else{
			particleInit(curParticle);
		}
	}
}