#include "spriteRendererInstanced.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilMath.h"

#include "settingsSprites.h"


#warning "It might be a good idea to double check that we're using buffer orphaning correctly."
/// https://community.khronos.org/t/vbos-strangely-slow/60109/33
/// https://community.khronos.org/t/buffer-orphaning-question-streaming-vbos/61550
#warning "Our current method should be fine, but we can do better by handling synchronization ourselves."
#warning "It's a bit more annoying with uniform buffer objects, since we need to think about alignment."

#error "We should move the instance buffer stuff to our mesh file."


// The shader used for instanced rendering has a uniform
// block large enough to store the data for 40 instances.
#define SPRITE_RENDERER_INSTANCED_MAX_INSTANCES 40
#define SPRITE_RENDERER_INSTANCED_BUFFER_SIZE \
	(SPRITE_RENDERER_INSTANCED_MAX_INSTANCES * sizeof(meshInstance))


// We use a single state buffer plus
// orphaning for all instanced rendering.
GLuint instanceBufferID;
// Offset into the instance buffer.
size_t instanceOffset;


// Initialize the global instance buffer.
void spriteRendererInstancedSetup(){
	glGenBuffers(1, &instanceBufferID);	
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

	glBindBuffer(GL_UNIFORM_BUFFER, instanceBufferID);
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
	
	glBindBuffer(GL_UNIFORM_BUFFER, instanceBufferID);
	// Orphan the old instance buffer and allocate a new one.
	glBufferData(GL_UNIFORM_BUFFER, SPRITE_RENDERER_INSTANCED_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	// Obtain a mapping to the new storage!
	instancedRenderer->curInstance = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
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
	glDeleteBuffers(1, &instanceBufferID);
}