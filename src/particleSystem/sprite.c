#include "sprite.h"


#define GLEW_STATIC
#include <GL/glew.h>


// We use a single array object plus
// orphaning for every sprite draw call.
struct {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
} spriteArrayObject;


// Initialize the global sprite array object.
void spriteSetup(){
	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &spriteArrayObject.vertexArrayID);
	glBindVertexArray(spriteArrayObject.vertexArrayID);
		// Generate a buffer object for our vertices
		// and bind it to the vertex array object!
		glGenBuffers(1, &spriteArrayObject.vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteArrayObject.vertexBufferID);
			// Allocate the buffer, but don't initialize it yet.
			// We'll be updating this buffer every time we draw.
			glBufferData(GL_ARRAY_BUFFER, SPRITE_VERTEX_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
			// Set up the vertex array object attributes that require this buffer!
			// Vertex positions.
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, pos));
			// Vertex UVs.
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, uv));

		// Generate a buffer object for our indices
		// and bind it to the vertex array object!
		glGenBuffers(1, &spriteArrayObject.indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteArrayObject.indexBufferID);
		// As before, we'll be updating this buffer when we draw.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPRITE_INDEX_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	// Unbind the array object!
	glBindVertexArray(0);
}


/*
** Initialize a sprite batch using pointers to the
** storage for the vertex and index buffer objects.
** This storage can be written to, but not read from.
**
** This function will also orphan the buffers if
** they're still in use. The assumption is that
** this function will only be called once per batch.
*/
void spriteBatchInit(spriteBatch *const restrict batch){
	glBindBuffer(GL_ARRAY_BUFFER, spriteArrayObject.vertexBufferID);
	// Orphan the vertex buffer if it's still in use.
	glBufferData(GL_ARRAY_BUFFER, SPRITE_VERTEX_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	// Retrieve a pointer to the vertex buffer's storage.
	batch->vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	batch->numVertices = 0;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteArrayObject.indexBufferID);
	// Orphan the index buffer if it's still in use.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPRITE_INDEX_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	// Retrieve a pointer to the index buffer's storage.
	batch->indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	batch->numIndices = 0;
}

/*
** Return whether the batch has room for
** "numSprites" instances of "spriteData".
*/
return_t spriteBatchHasRoom(
	const spriteBatch *const restrict batch,
	const sprite *const restrict spriteData,
	const size_t numSprites
){

	const size_t maxVertices =
		batch->numVertices + spriteData->numVertices*numSprites;
	// Add one to account for primitive restart.
	const size_t maxIndices =
		batch->numIndices + (spriteData->numIndices + 1)*numSprites;

	return(
		maxVertices > SPRITE_BUFFER_MAX_VERTICES ||
		maxIndices  > SPRITE_BUFFER_MAX_INDICES
	);
}

void spriteDraw(const spriteBatch *const restrict batch){
	if(batch->numIndices > 0){
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

		// Remember to unmap the buffer storage before rendering.
		glBindBuffer(GL_ARRAY_BUFFER, spriteArrayObject.vertexBufferID);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteArrayObject.indexBufferID);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		// Bind the global sprite buffer and draw it!
		glBindVertexArray(spriteManager.vertexArrayID);
		glDrawElements(GL_TRIANGLE_STRIP, batch->numIndices, GL_UNSIGNED_INT, NULL);
	}
}


void spriteCleanup(){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our structure to have no padding.
	glDeleteBuffers(3, &spriteArrayObject.vertexBufferID);
	glDeleteVertexArrays(1, &spriteArrayObject.vertexArrayID);
}