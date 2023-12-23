#include "spriteRendererBatched.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "settingsSprites.h"


// By default, we allow 25,000 quads to be batched at once.
#define SPRITE_RENDERER_BATCHED_BUFFER_MAX_QUADS    25000
#define SPRITE_RENDERER_BATCHED_BUFFER_MAX_VERTICES \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_QUADS*4)
#define SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE  \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_VERTICES*sizeof(spriteVertex))
// Using triangle strips, we normally expect 4 indices per quad.
// However, we need to add an extra index for primitive restart,
// since we'll often be batching many quads together in one call.
#define SPRITE_RENDERER_BATCHED_BUFFER_MAX_INDICES \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_QUADS*5)
#define SPRITE_RENDERER_BATCHED_INDEX_BUFFER_SIZE  \
	(SPRITE_RENDERER_BATCHED_BUFFER_MAX_INDICES*sizeof(spriteVertexIndex))

#define SPRITE_RENDERER_BATCHED_BUFFER_SIZE \
	SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE + SPRITE_RENDERER_BATCHED_INDEX_BUFFER_SIZE


GLuint batchArrayID;
// We use a single buffer object plus
// orphaning for every sprite draw call.
GLuint batchBufferID;
// Offsets (in bytes) into the vertex
// buffer's vertex and index arrays.
size_t vertexOffset;
size_t indexOffset;


// Initialize the global sprite array object.
void spriteRendererBatchedSetup(){
	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &batchArrayID);
	glBindVertexArray(batchArrayID);
		// Generate a buffer object for our vertices and indices.
		// We store all of our vertices, then all of our indices.
		glGenBuffers(1, &batchBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, batchBufferID);
			// Enable the vertex attributes, but don't define
			// them yet. The offsets into the buffer change
			// between draw calls, so we define them then.
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		// Allocate memory for the maximum number of vertices and indices.
		glBufferData(GL_ARRAY_BUFFER, SPRITE_RENDERER_BATCHED_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
		glBindBuffers(GL_ELEMENT_ARRAY_BUFFER, batchBufferID);
	// Unbind the array object!
	glBindVertexArray(0);

	vertexOffset = 0;
	// The indices begin after all of the vertices.
	indexOffset  = SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE;
}


/*
** Initialize a batched sprite renderer by obtaining a mapping
** to the next available region of the global buffer object.
** Rather than orphaning the buffer, we try to use whatever
** memory is left in it. We don't yet know how big the batch
** is, so we can always orphan it later if we really have to.
*/
void spriteRendererBatchedInit(spriteRendererBatched *const restrict batchedRenderer){
	glBindBuffer(GL_ARRAY_BUFFER, batchBufferID);
	// Retrieve pointers to the vertex buffer storage. The flag
	// GL_MAP_INVALIDATE_RANGE_BIT is a promise that the range we're
	// requesting is not in use by any draw call, so there's no need
	// for any synchronization.
	batchedRenderer->curVertex = glMapBufferRange(
		GL_ARRAY_BUFFER,
		vertexOffset,
		SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE - vertexOffset,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
	);
	batchedRenderer->curIndex = glMapBufferRange(
		GL_ARRAY_BUFFER,
		indexOffset,
		SPRITE_RENDERER_BATCHED_BUFFER_SIZE - indexOffset,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
	);
	// Reset the vertex and index counts.
	batchedRenderer->numVertices = 0;
	batchedRenderer->numIndices = 0;
}

void spriteRendererBatchedDraw(const spriteRendererBatched *const restrict batchedRenderer){
	if(batchedRenderer->numIndices > 0){
		#warning "All of this stuff should be moved out eventually."
		#if 0
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture that this batch uses!
		glBindTexture(GL_TEXTURE_2D, texID);

		#warning "The uniforms should be moved out eventually."
		glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&cam->viewProjectionMatrix);
		#warning "Particles shouldn't support SDF."
		#warning "We should make a proper GUI shader for this sort of thing."
		// Particle systems do not currently support SDF.
		glUniform1ui(shader->sdfTypeID, SPRITE_IMAGE_TYPE_NORMAL);
		#endif

		// Bind the global sprite buffer!
		glBindVertexArray(spriteManager.batchArrayID);
		// Remember to unmap the buffer storage before rendering.
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		// Since we're using an offset for the vertices, we'll need to
		// update the vertex attributes to point to the correct positions.
			// Vertex positions.
			glVertexAttribPointer(
				0, 3, GL_FLOAT, GL_FALSE, sizeof(spriteVertex),
				(GLvoid *)(vertexOffset + offsetof(spriteVertex, pos))
			);
			// Vertex UVs.
			glVertexAttribPointer(
				1, 2, GL_FLOAT, GL_FALSE, sizeof(spriteVertex),
				(GLvoid *)(vertexOffset + offsetof(spriteVertex, uv))
			);
			// Vertex normals.
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE, sizeof(spriteVertex),
				(GLvoid *)(vertexOffset + offsetof(spriteVertex, normal))
			);
		// Draw the sprites!
		glDrawElements(
			GL_TRIANGLE_STRIP, batchedRenderer->numIndices,
			GL_UNSIGNED_INT, (GLvoid *)indexOffset
		);

		// Increment the buffer offsets. This allows us to
		// prepare for a new draw call without interrupting
		// the old one or allocating a new buffer object.
		vertexOffset += batchedRenderer->numVertices * sizeof(spriteVertex);
		indexOffset  += batchedRenderer->numIndices * sizeof(spriteVertexIndex);
	}
}

// Orphan the old vertex and index buffers and allocate new ones.
void spriteRendererBatchedOrphan(spriteRendererBatched *const restrict batchedRenderer){
	// Reset the vertex and index offsets.
	vertexOffset = 0;
	indexOffset  = SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE;

	#warning "This should always be bound, can we remove this?"
	glBindBuffer(GL_ARRAY_BUFFER, batchBufferID);
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


/*
** Return whether the batch has room for the
** specified numbers of vertices and indices.
*/
return_t spriteRendererBatchedHasRoom(
	const spriteRendererBatched *const restrict batchedRenderer,
	const size_t numVertices, const size_t numIndices
){

	return(
		(batchedRenderer->numVertices + numVertices) * sizeof(spriteVertexIndex) <=
		SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE - vertexOffset ||
		(batchedRenderer->numIndices + numIndices) * sizeof(spriteVertexIndex) <=
		SPRITE_RENDERER_BATCHED_BUFFER_SIZE - indexOffset
	);
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
	glDeleteBuffers(3, &batchBufferID);
	glDeleteVertexArrays(1, &batchArrayID);
}