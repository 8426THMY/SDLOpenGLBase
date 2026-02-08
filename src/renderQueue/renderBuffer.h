#ifndef renderBuffer_h
#define renderBuffer_h


#include <stdint.h>


#define RENDER_BUFFER_NUM_TYPES 2

#define RENDER_BUFFER_TYPE_ARRAY   0
#define RENDER_BUFFER_TYPE_UNIFORM 1


typedef uint_least8_t renderBufferType;

typedef struct renderBuffer {
	unsigned int id;
	renderBufferType type;
} renderBuffer;


#endif