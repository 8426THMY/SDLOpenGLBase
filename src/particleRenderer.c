#include "particleRenderer.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "sort.h"


/*
** Render an array of particles as a polyboard.
** The order that the particles' positions are
** connected in is the order they were spawned.
**
** Thanks to "Mathematics for 3D Game Programming
** and Computer Graphics, Third Edition" for the
** polyboarding technique used here.
*/
void particleRendererBeam(
	const void *const restrict renderer, const particle *const restrict particles,
	const size_t numParticles, const particleDef *const restrict properties,
	const camera *const restrict cam, const spriteShader *const restrict shader, const float dt
){

	/** Use Catmull-Rom splines for subdivision. **/
	//
}


#warning "This is temporary, as renderers have not been implemented yet."
#warning "Ideally, they should handle most of the draw code. Each renderer should redraw every particle."
#warning "We should, however, get the state matrix and UV offsets once in this function."
#warning "If we get the state matrix once though, what about billboarding?"
#error "Finish this stuff."
/*
** Render an array of particles as billboarded sprites.
** We assume that the particles have been sorted by depth.
*/
void particleRendererSprites(
	const void *const restrict renderer, const particle *const restrict particles,
	const size_t numParticles, const particleDef *const restrict properties,
	const camera *const restrict cam, const spriteShader *const restrict shader, const float dt
){

	const sprite *const particleSprite = &properties->spriteData;

	const particle *curParticle = particles;
	const particle *const lastParticle = &curParticle[numParticles];

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
	glBindTexture(GL_TEXTURE_2D, properties->tex->id);


	// Store our particle data in the arrays.
	for(; curParticle < lastParticle; ++curParticle, ++curState){
		const textureGroupFrame *const texFrame = texGroupStateGetFrame(&curParticle->texState);

		// Convert the particle's state to a matrix!
		transformToMat3x4(&curParticle->state, &curState->state);
		// Get the particle's UV coordinates!
		curState->uvOffsets = texFrame->bounds;

		//curParticle->camDistance = cameraDistanceSquared(cam, &curParticle->state.pos);
	}

	#warning "This should be a property of renderers."
	#warning "We need to sort the particles before we generate the render states."
	#warning "Since we need to update camDistance, this means looping through the particles twice. Yuck."
	/** SORT PARTICLES HERE(?) **/
	/** STOP FORGETTING THIS! We need to sort particles every render    **/
	/** anyway, as we will be interpolating their positions eventually. **/
	/** We shouldn't have to do much sorting between frames, so can we  **/
	/** make the renderers store data per particle to speed this up?    **/
	/*if(partSys->numParticles > 0){
		timsort(partSys->particles, partSys->numParticles, sizeof(*partSys->particles), &particleCompare);
	}*/


	#warning "This may be a problem, especially if we're multithreading and other systems are using the same sprite."
	#warning "Since multithreading isn't an option, maybe use a global particle state buffer for all systems?"
	// Upload the particles' states to the shader.
	glBindBuffer(GL_ARRAY_BUFFER, particleSprite->stateBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particleStates[0]) * numParticles, &particleStates[0]);
	// Draw each instance of the particle!
	glDrawElementsInstanced(GL_TRIANGLES, particleSprite->numIndices, GL_UNSIGNED_INT, NULL, numParticles);
}