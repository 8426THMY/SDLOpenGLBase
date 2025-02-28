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


// The shader used for instanced rendering has a uniform
// block large enough to store the data for 40 instances.
#define SPRITE_RENDERER_INSTANCED_MAX_INSTANCES 40
#define SPRITE_RENDERER_INSTANCED_BUFFER_SIZE \
	(SPRITE_RENDERER_INSTANCED_MAX_INSTANCES * sizeof(spriteInstancedData))


// We use a single state buffer plus
// orphaning for all instanced rendering.
GLuint instanceDataBufferID;


// Initialize the global instance buffer.
void spriteRendererInstancedSetup(){
	glGenBuffers(1, &instanceDataBufferID);	
}

/*
** Initialize an instanced sprite renderer by obtaining
** a mapping to the instance buffer object's storage.
*/
void spriteRendererInstancedInit(spriteRendererInstanced *const restrict instancedRenderer){
	spriteRendererInstancedOrphan(instancedRenderer);
	
}

void spriteRendererInstancedDraw(const spriteRendererInstanced *const restrict instancedRenderer){
	if(instancedRenderer->numInstances > 0){
		const size_t instanceDataSize = instancedRenderer->numInstances * sizeof(spriteInstancedData);
		
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

		// Bind the sprite we're using!
		glBindVertexArray(instancedRenderer->base->vertexArrayID);
		// Remember to unmap the buffer storage before rendering.
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		// Draw each instance of our sprite!
		glDrawElementsInstanced(
			GL_TRIANGLES, instancedRenderer->base->numIndices,
			GL_UNSIGNED_INT, NULL, instancedRenderer->numInstances
		);
	}
}

// Orphan the old instance buffer and allocate a new one.
void spriteRendererInstancedOrphan(spriteRendererInstanced *const restrict instancedRenderer){
	glBindBuffer(GL_UNIFORM_BUFFER, instanceDataBufferID);
	// Orphan the old instance buffer and allocate a new one.
	glBufferData(GL_UNIFORM_BUFFER, SPRITE_RENDERER_INSTANCED_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	// Obtain a mapping to the new storage!
	instancedRenderer->curInstance = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
}


#error "This should be moved out."
// Generate buffers to hold our sprite data!
void spriteInstancedGenerateBuffers(
	spriteInstanced *const restrict spriteData,
	const spriteVertex *const restrict vertices, const spriteVertexIndex numVertices,
	const spriteVertexIndex *const restrict indices, const spriteVertexIndex numIndices
){

	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &spriteData->vertexArrayID);
	glBindVertexArray(spriteData->vertexArrayID);
		// Generate a buffer object for our vertex data and bind it!
		glGenBuffers(1, &spriteData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * numVertices, vertices, GL_STATIC_DRAW);
			// Set up the vertex attributes that require this buffer!
			// Vertex positions.
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				0, 3, GL_FLOAT, GL_FALSE,
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, pos)
			);
			// Vertex UVs.
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1, 2, GL_FLOAT, GL_FALSE,
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, uv)
			);
			// Vertex normal.
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE,
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, normal)
			);
		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &spriteData->indexBufferID);
		// Bind the buffer to the vertex array object.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(0);

	spriteData->numIndices = numIndices;
}

return_t spriteRendererInstancedIsFull(const spriteRendererInstanced *const restrict instancedRenderer){
	return(instancedRenderer->numInstances == SPRITE_RENDERER_INSTANCED_MAX_INSTANCES);
}

// Add an instance to the batch.
void spriteRendererInstancedAddInstance(
	spriteRendererInstanced *const restrict instancedRenderer,
	const spriteInstancedData *const restrict instance
){

	*instancedRenderer->curInstance = *instance;
	++instancedRenderer->curInstance;
}


void spriteRendererInstancedCleanup(){
	glDeleteBuffers(1, &instanceDataBufferID);
}