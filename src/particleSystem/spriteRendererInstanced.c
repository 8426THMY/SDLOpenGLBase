#include "spriteRendererInstanced.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilMath.h"

#include "settingsSprites.h"


#warning "It's probably possible to do this using vertex attributes."
#warning "That is, we create a new array buffer for the instance data and enable the attributes for them."
#warning "Our instance data uses a lot of vertex attributes, though..."


// The shader used for instanced rendering has a uniform
// block large enough to store the data for 40 instances.
#define SPRITE_RENDERER_INSTANCED_MAX_INSTANCES 40
#define SPRITE_RENDERER_INSTANCED_BUFFER_SIZE \
	(SPRITE_RENDERER_INSTANCED_MAX_INSTANCES * sizeof(meshInstance))


// We use a single state buffer plus
// orphaning for all instanced rendering.
GLuint instanceDataBufferID;
// Offset into the instance buffer.
size_t instanceOffset;


// Initialize the global instance buffer.
void spriteRendererInstancedSetup(){
	glGenBuffers(1, &instanceDataBufferID);
	instanceOffset = 0;
}

/*
** Initialize an instanced sprite renderer by obtaining a mapping
** to the next available region of the global buffer object. Rather
** than orphaning the buffer, we try to use whatever memory is left
** in it. We don't yet know how big the batch is, so we can always
** orphan it later if we really have to.
*/
#warning "Can we bind the vertex array object here to avoid binds everywhere else?"
void spriteRendererInstancedInit(spriteRendererInstanced *const restrict instancedRenderer){
	const size_t instanceOffsetBytes = instanceOffset * sizeof(*instancedRenderer->curInstance);

	glBindBuffer(GL_UNIFORM_BUFFER, instanceDataBufferID);
	// Retrieve pointers to the instance buffer storage. The flag
	// GL_MAP_INVALIDATE_RANGE_BIT is a promise that the range we're
	// requesting is not in use by any draw call, so there's no need
	// for any synchronization.
	instancedRenderer->curInstance = glMapBufferRange(
		GL_UNIFORM_BUFFER,
		instanceOffsetBytes,
		SPRITE_RENDERER_INSTANCED_BUFFER_SIZE - instanceOffsetBytes,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
	);
	// Reset the instance counts.
	batchedRenderer->numInstances = 0;
	
}

void spriteRendererInstancedDraw(const spriteRendererInstanced *const restrict instancedRenderer){
	if(instancedRenderer->numInstances > 0){
		#warning "All of this stuff should be moved out eventually."
		#if 0
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture that these instances use!
		glBindTexture(GL_TEXTURE_2D, texFrame->tex->id);

		#warning "The uniforms should be moved out eventually."
		glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&cam->viewProjectionMatrix);
		#warning "Particles shouldn't support SDF."
		#warning "We should make a proper GUI shader for this sort of thing."
		// Particle systems do not currently support SDF.
		glUniform1ui(shader->sdfTypeID, SPRITE_IMAGE_TYPE_NORMAL);
		#endif

		// The buffer should be bound before unmapping,
		// so we might as well bind the array object first.
		glBindVertexArray(meshData->vertexArrayID);
		#warning "This should be done when creating the shader."
		//instanceDataID = glGetUniformBlockIndex(objectProgramID, "instanceData");
		//glBindBufferBase(GL_UNIFORM_BUFFER, instanceDataID, instanceDataBufferID);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glDrawElementsInstanced(GL_TRIANGLES, instancedRenderer->base->numIndices, GL_UNSIGNED_INT, NULL, instancedRenderer->numInstances);

		// Increment the instance buffer offset. This allows us
		// to prepare for a new draw call without interrupting
		// the old one or allocating a new buffer object.
		instanceOffset += instancedRenderer->numInstances;
	}
}

// Orphan the old instance buffer and allocate a new one.
void spriteRendererInstancedOrphan(spriteRendererInstanced *const restrict instancedRenderer){
	// Reset the instance offset.
	instanceOffset = 0;
	
	glBindBuffer(GL_UNIFORM_BUFFER, instanceDataBufferID);
	// Retrieve pointers to the vertex buffer storage. The flag
	// GL_MAP_INVALIDATE_BUFFER_BIT indicates that the original
	// buffer should be orphaned, and a new one allocated.
	instancedRenderer->curInstance = glMapBufferRange(
		GL_UNIFORM_BUFFER,
		0,
		SPRITE_RENDERER_INSTANCED_BUFFER_SIZE,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
	);
}


return_t spriteRendererInstancedIsFull(const spriteRendererInstanced *const restrict instancedRenderer){
	return(instancedRenderer->numInstances >= SPRITE_RENDERER_INSTANCED_MAX_INSTANCES);
}

// Add an instance to the batch.
void spriteRendererInstancedAddInstance(
	spriteRendererInstanced *const restrict instancedRenderer,
	const meshInstance *const restrict instance
){

	*instancedRenderer->curInstance = *instance;
	++instancedRenderer->curInstance;
}


void spriteRendererInstancedCleanup(){
	glDeleteBuffers(1, &instanceDataBufferID);
}