#include "renderBuffer.h"


#define GLEW_STATIC
#include <GL/glew.h>


GLenum renderBufferTypeLookup[RENDER_BUFFER_NUM_TYPES] = {
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_UNIFORM_BUFFER
};


/*
** Set up a render buffer using a buffer that's already
** been created (and bound), and allocate some storage for it.
*/
void renderBufferInit(
	renderBuffer *const restrict buffer,
	const unsigned int id, const size_t size, const renderBufferType type
){

	buffer->id   = id;
	buffer->size = dataSize;
	buffer->type = renderBufferTypeLookup[type];

	glBufferData(type, size, NULL, GL_STREAM_DRAW);
}

/*
** Return a pointer to the memory in the buffer
** at the specified offset with the specified length.
*/
void *renderBufferMap(
	renderBuffer *const restrict buffer, const size_t offset
){

	return(glMapBufferRange(
		buffer->type, offset, buffer->size - offset,
		GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT
	));
}

// Unmap the render buffer, ready for drawing!
void renderBufferUnmap(renderBuffer *const restrict buffer){
	glUnmapBuffer(buffer->type);
}

// Orphan the render buffer and return a pointer to the new memory.
void *renderBufferOrphan(renderBuffer *const restrict buffer){
	return(glMapBufferRange(
		buffer->type, 0, buffer->size,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
	));
}