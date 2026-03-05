#ifndef renderBuffer_h
#define renderBuffer_h


#include <stddef.h>
#include <stdint.h>


#define RENDER_BUFFER_NUM_TYPES 3

#define RENDER_BUFFER_TYPE_ARRAY         0
#define RENDER_BUFFER_TYPE_ELEMENT_ARRAY 1
#define RENDER_BUFFER_TYPE_UNIFORM       2


#warning "It might be good to use a ring buffer rather than just buffer orphaning."
#warning "This is explained in chapter 28 of Cozzi and Riccio's OpenGL Insights."


typedef uint_least8_t renderBufferType;

// Render buffers are handles for buffer objects
// that we intend to use with buffer orphaning.
typedef struct renderBuffer {
	unsigned int id;
	size_t size;
	renderBufferType type;
} renderBuffer;


void renderBufferInit(
	renderBuffer *const restrict buffer,
	const unsigned int id, const size_t size, const renderBufferType type
);

void *renderBufferMap(renderBuffer *const restrict buffer);
void renderBufferUnmap(renderBuffer *const restrict buffer);
void *renderBufferOrphan(renderBuffer *const restrict buffer);


#endif