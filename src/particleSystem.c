#include "particleSystem.h"


#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "sort.h"

#include "memoryManager.h"


// Forward-declare any helper functions!
static void initializeParticle(const particleSystem *const restrict partSys, particle *const restrict part);
static void operateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float time);
static void constrainParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float time);
static void spawnParticles(particleSystem *const restrict partSys, const size_t numParticles);
static void updateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float time);

static void updateSysEmitters(particleSystem *const restrict partSys, const float time);
static void updateSysParticles(particleSystem *const restrict partSys, const float time);

static void drawParticles(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float time
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

	partSysDef->lifetime = INFINITY;

	partSysDef->children = NULL;
	partSysDef->numChildren = 0;
}

// Instantiate a particle system definition.
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

		partSys->particles = memoryManagerGlobalAlloc(sizeof(*partSys->particles) * partSysDef->maxParticles);
		if(partSys->particles == NULL){
			/** MALLOC FAILED **/
		}

		curParticle = partSys->particles;
		lastParticle = &curParticle[partSysDef->maxParticles];
		// Initialize the system's particles.
		do {
			// Set up any initially active particles using the system's initializers.
			if(partSys->numParticles < partSysDef->initialParticles){
				initializeParticle(partSys, curParticle);
				++partSys->numParticles;

			// Otherwise, just initialize it to the dead state.
			}else{
				particleDelete(curParticle);
			}
			++curParticle;
		} while(curParticle < lastParticle);
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


void particleSysUpdate(particleSystem *const restrict partSys, const float time){
	particleSystem *firstChild = partSys->children;
	if(firstChild != NULL){
		particleSystem *curChild = &firstChild[partSys->partSysDef->numChildren];
		// Update all of the particle system's children, starting from the last!
		do {
			--curChild;
			particleSysUpdate(curChild, time);
		} while(curChild > firstChild);
	}

	// Now update the system itself!
	updateSysEmitters(partSys, time);
	updateSysParticles(partSys, time);
}

/*
** Draw a particle system and all of its children. The deepest
** child is drawn first and the highest parent is drawn last.
*/
void particleSysDraw(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float time
){

	const particleSystem *const firstChild = partSys->children;
	if(firstChild != NULL){
		// Draw all of the particle system's children, starting from the last!
		const particleSystem *curChild = &firstChild[partSys->partSysDef->numChildren];
		do {
			--curChild;
			particleSysDraw(curChild, cam, shader, time);
		} while(curChild > firstChild);
	}

	// Now draw the system itself!
	drawParticles(partSys, cam, shader, time);
}


// Return whether or not a particle system is still alive.
return_t particleSysAlive(particleSystem *const restrict partSys, const float time){
	partSys->lifetime -= time;
	return(partSys->lifetime > 0.f);
}


void particleSysDelete(particleSystem *const restrict partSys){
	if(partSys->emitters != NULL){
		memoryManagerGlobalFree(partSys->emitters);
	}

	if(partSys->particles != NULL){
		memoryManagerGlobalFree(partSys->particles);
	}

	if(partSys->children != NULL){
		particleSystem *curChild = partSys->children;
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
}


// Execute each of the system's initializers on a particle.
static void initializeParticle(const particleSystem *const restrict partSys, particle *const restrict part){
	const particleSystemDef *const partSysDef = partSys->partSysDef;
	const particleInitializer *curInitializer = partSysDef->initializers;

	particleInit(part);

	if(curInitializer != NULL){
		do {
			(*curInitializer->func)((const void *)(&curInitializer->data), part);
			++curInitializer;
		} while(curInitializer <= partSysDef->lastInitializer);
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
static void operateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float time){
	const particleOperator *curOperator = partSysDef->operators;
	if(curOperator != NULL){
		do {
			(*curOperator->func)((const void *)(&curOperator->data), part, time);
			++curOperator;
		} while(curOperator <= partSysDef->lastOperator);
	}
}

// Execute each of the system's constraints on a particle.
static void constrainParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float time){
	const particleConstraint *curConstraint = partSysDef->constraints;
	if(curConstraint != NULL){
		do {
			(*curConstraint->func)((const void *)(&curConstraint->data), part, time);
			++curConstraint;
		} while(curConstraint <= partSysDef->lastConstraint);
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
static void updateParticle(const particleSystemDef *const restrict partSysDef, particle *const restrict part, const float time){
	#warning "Can the particle die while we're updating it?"
	#warning "If so, I guess we can wait a tick to delete it."
	operateParticle(partSysDef, part, time);
	constrainParticle(partSysDef, part, time);
	particleUpdate(part, time);
}


/*
** Update the emitters in a particle system. This may spawn
** new particles, but it will never destroy old ones.
*/
static void updateSysEmitters(particleSystem *const restrict partSys, const float time){
	size_t remainingParticles = partSys->partSysDef->maxParticles - partSys->numParticles;

	particleEmitter *curEmitter = partSys->emitters;
	const particleEmitter *const lastEmitter = &curEmitter[partSys->partSysDef->numEmitters];
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
static void updateSysParticles(particleSystem *const restrict partSys, const float time){
	const particleSystemDef *const partSysDef = partSys->partSysDef;

	particle *curParticle = partSys->particles;
	particle *freePos = curParticle;
	const particle *const lastParticle = &curParticle[partSys->numParticles];
	// Update every living particle using the system's operators and constraints.
	//
	// Every living particle should be copied back to the first free
	// position in the array to ensure that it doesn't have any gaps.
	for(; curParticle < lastParticle; ++curParticle){
		// If the particle hasn't died, update it!
		if(particleAlive(curParticle, time)){
			updateParticle(partSysDef, curParticle, time);

			// Shift the particle over to the
			// first free position in the array.
			if(freePos != curParticle){
				*freePos = *curParticle;
			}
			++freePos;

		// Otherwise, just decrease the particle count.
		}else{
			--partSys->numParticles;
		}
	}
}


// Draw a particle system using all of its renderers!
#warning "This is temporary, as renderers have not been implemented yet."
#warning "Ideally, they should handle most of the draw code. Each renderer should redraw every particle."
#warning "We should, however, get the state matrix and UV offsets once in this function."
#warning "If we get the state matrix once though, what about billboarding?"
static void drawParticles(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float time
){

	const particleSystemDef *const partSysDef = partSys->partSysDef;
	const sprite *particleSprite = &partSysDef->properties.spriteData;

	particle *curParticle = partSys->particles;
	const particle *const lastParticle = &curParticle[partSys->numParticles];

	#warning "If we use a global sprite buffer, should we make this global too?"
	spriteState particleStates[SPRITE_MAX_INSTANCES];
	spriteState *curState = particleStates;


	// Send the view-projection matrix to the shader!
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&cam->viewProjectionMatrix);
	// Particle systems do not currently support SDF.
	#warning "Add SDF and MSDF support for particle systems?"
	glUniform1ui(shader->sdfTypeID, SPRITE_IMAGE_TYPE_NORMAL);

	// Bind the sprite we're using!
	glBindVertexArray(particleSprite->vertexArrayID);

	glActiveTexture(GL_TEXTURE0);
	// Bind the texture that this system uses!
	glBindTexture(GL_TEXTURE_2D, g_texDefault.id);


	// Store our particle data in the arrays.
	#warning "This is pretty bad, specifically creating the textureGroupState."
	for(; curParticle < lastParticle; ++curParticle, ++curState){
		const textureGroupState texState = {
			.texGroup = partSysDef->properties.texGroup,
			.currentAnim = curParticle->currentAnim,
			.texGroupAnim = curParticle->texAnim
		};
		const textureGroupFrame *const texFrame = texGroupStateGetFrame(&texState);

		// Convert the particle's state to a matrix!
		transformToMat4(&curParticle->state, &curState->state);
		// Get the particle's UV coordinates!
		curState->uvOffsets = texFrame->bounds;

		curParticle->camDistance = cameraDistanceSquared(cam, &curParticle->state.pos);
	}

	#warning "This should be a property of renderers."
	#warning "We need to sort the particles before we generate the render states."
	#warning "Since we need to update camDistance, this means looping through the particles twice. Yuck."
	/** SORT PARTICLES HERE(?) **/
	if(partSys->numParticles > 0){
		timsort(partSys->particles, partSys->numParticles, sizeof(*partSys->particles), &particleCompare);
	}


	#warning "This may be a problem, especially if we're multithreading and other systems are using the same sprite."
	#warning "Since multithreading isn't an option, maybe use a global particle state buffer for all systems?"
	// Upload the particles' states to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, particleSprite->stateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particleStates[0]) * partSys->numParticles, &particleStates[0]);
	// Draw each instance of the particle!
	glDrawElementsInstanced(GL_TRIANGLES, particleSprite->numIndices, GL_UNSIGNED_INT, NULL, partSys->numParticles);
}