#include "spriteRendererBatched.h"


#define GLEW_STATIC
#include <GL/glew.h>


#warning "It might be a good idea to double check that we're using buffer orphaning correctly."
/// https://community.khronos.org/t/vbos-strangely-slow/60109/33
/// https://community.khronos.org/t/buffer-orphaning-question-streaming-vbos/61550
#warning "Our current method should be fine, but we can do better by handling synchronization ourselves."


// We use a single array object plus
// orphaning for every sprite draw call.
struct {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	// Offsets (in bytes) into the vertex
	// buffer's vertex and index arrays.
	size_t vertexOffset;
	size_t indexOffset;
} batchArrayObject;


// Initialize the global sprite array object.
void spriteRendererBatchedSetup(){
	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &batchArrayObject.vertexArrayID);
	glBindVertexArray(batchArrayObject.vertexArrayID);
		// Generate a buffer object for our vertices and indices.
		// We store all of our vertices, then all of our indices.
		glGenBuffers(1, &batchArrayObject.vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, batchArrayObject.vertexBufferID);
		// Allocate memory for the maximum number of vertices and indices.
		glBufferData(GL_ARRAY_BUFFER, SPRITE_RENDERER_BATCHED_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
		glBindBuffers(GL_ELEMENT_ARRAY_BUFFER, batchArrayObject.vertexBufferID);
	// Unbind the array object!
	glBindVertexArray(0);

	batchArrayObject.vertexOffset = 0;
	batchArrayObject.indexOffset  = 0;
}


/*
** Initialize a sprite renderer. Essentially, we just continue
*/
void spriteRendererBatchedInit(spriteRendererBatched *const restrict batchedRenderer){
	batchedRenderer->numVertices = 0;
	batchedRenderer->numIndices = 0;
	spriteRendererBatchedOffset(batchedRenderer);
}

/*
** Invalidate the currently-occupied region of the global buffer
** and provide an unsynchronized mapping to the remaining memory.
** This lets us continue to prepare for additional draw calls
** without having to wait for the previous ones to finish.
*/
void spriteRendererBatchedOffset(spriteRendererBatched *const restrict batchedRenderer){
	// Since we're using an offset for the vertices, we'll need to
	// update the vertex attributes to point to the correct positions.
	glBindBuffer(GL_ARRAY_BUFFER, batchArrayObject.vertexBufferID);
		// Vertex positions.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE, sizeof(spriteVertex),
			(GLvoid *)(batchArrayObject.vertexOffset + offsetof(spriteVertex, pos))
		);
		// Vertex UVs.
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1, 2, GL_FLOAT, GL_FALSE, sizeof(spriteVertex),
			(GLvoid *)(batchArrayObject.vertexOffset + offsetof(spriteVertex, uv))
		);
		// Vertex normals.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2, 3, GL_FLOAT, GL_FALSE, sizeof(spriteVertex),
			(GLvoid *)(batchArrayObject.vertexOffset + offsetof(spriteVertex, normal))
		);
	// Increment the buffer offsets. This allows us to
	// prepare for a new draw call without interrupting
	// the old one or allocating a new buffer object.
	batchArrayObject.vertexOffset += batchedRenderer->numVertices * sizeof(spriteVertex);
	batchArrayObject.indexOffset  += batchedRenderer->numIndices * sizeof(spriteVertexIndex);

	// Retrieve pointers to the vertex buffer storage. The flag
	// GL_MAP_INVALIDATE_RANGE_BIT is a promise not to interfere
	// with the old data, so there's no need for synchronization.
	batchedRenderer->curVertex = glMapBufferRange(
		GL_ARRAY_BUFFER,
		batchArrayObject.vertexOffset,
		SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE - batchArrayObject.vertexOffset,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
	);
	batchedRenderer->curIndex = glMapBufferRange(
		GL_ARRAY_BUFFER,
		SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE + batchArrayObject.indexOffset,
		SPRITE_RENDERER_BATCHED_INDEX_BUFFER_SIZE - batchArrayObject.indexOffset,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
	);
	// Reset the vertex and index counts.
	batchedRenderer->numVertices = 0;
	batchedRenderer->numIndices = 0;
}

/*
** Return whether the batch has room for the specified
** number of indices. Note that we always have at most
** as many indices as vertices, so the index buffer
** should always be filled first.
*/
return_t spriteRendererBatchedHasRoom(
	const spriteRendererBatched *const restrict batchedRenderer, const size_t numIndices
){

	return(batchedRenderer->numIndices + numIndices <= SPRITE_RENDERER_BATCHED_BUFFER_MAX_INDICES);
}

void spriteRendererBatchedDraw(const spriteRendererBatched *const restrict batchedRenderer){
	if(batchedRenderer->numIndices > 0){
		#warning "All of this stuff should be moved out eventually."
		#if 0
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture that this batch uses!
		glBindTexture(GL_TEXTURE_2D, texID);

		#warning "Use a uniform buffer object for the view-projection matrix."
		// Send the view-projection matrix to the shader!
		glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&cam->viewProjectionMatrix);
		#warning "Particles shouldn't support SDF."
		#warning "We should make a proper GUI shader for this sort of thing."
		// Particle systems do not currently support SDF.
		glUniform1ui(shader->sdfTypeID, SPRITE_IMAGE_TYPE_NORMAL);
		#endif

		// Bind the global sprite buffer!
		glBindVertexArray(spriteManager.vertexArrayID);
		// Remember to unmap the buffer storage before rendering.
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		// Draw the sprites!
		glDrawElements(
			GL_TRIANGLE_STRIP, batchedRenderer->numIndices, GL_UNSIGNED_INT,
			(GLvoid *)(SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE + batchArrayObject.indexOffset)
		);
	}
}

// Orphan the old vertex and index buffers and allocate new ones.
void spriteRendererBatchedOrphan(spriteRendererBatched *const restrict batchedRenderer){
	#warning "This should always be bound, can we remove this?"
	glBindBuffer(GL_ARRAY_BUFFER, batchArrayObject.vertexBufferID);
	// Reset the vertex and index offsets.
	batchArrayObject.vertexOffset = 0;
	batchArrayObject.indexOffset  = 0;
	
	// Retrieve pointers to the vertex buffer storage. The flag
	// GL_MAP_INVALIDATE_BUFFER_BIT indicates that the original
	// buffer should be orphaned, and a new one allocated.
	batchedRenderer->curVertex = glMapBufferRange(
		GL_ARRAY_BUFFER,
		0,
		SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
	);
	// We use GL_MAP_INVALIDATE_RANGE_BIT to avoid reallocating
	// a new buffer directly after the one we allocate above.
	batchedRenderer->curIndex = glMapBufferRange(
		GL_ARRAY_BUFFER,
		SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE,
		SPRITE_RENDERER_BATCHED_INDEX_BUFFER_SIZE,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
	);
	// Reset the vertex and index counts.
	batchedRenderer->numVertices = 0;
	batchedRenderer->numIndices = 0;
}


// Add a vertex to the batch.
void spriteRendererBatchedAddVertex(
	spriteRendererBatched *const restrict batchedRenderer,
	const spriteVertex *const restrict vertex
){

	*batchedRenderer->curVertex = *vertex;
	++batchedRenderer->curVertex;
}

// Add an index to the batch.
void spriteRendererBatchedAddIndex(
	spriteRendererBatched *const restrict batchedRenderer,
	const spriteVertexIndex index
){

	*batchedRenderer->curIndex = index;
	++batchedRenderer->curIndex;
}


void spriteRendererBatchedCleanup(){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our structure to have no padding.
	glDeleteBuffers(3, &batchArrayObject.vertexBufferID);
	glDeleteVertexArrays(1, &batchArrayObject.vertexArrayID);
}