#include "renderBuffer.h"


#define GLEW_STATIC
#include <GL/glew.h>


GLenum renderBufferTypeLookup[RENDER_BUFFER_NUM_TYPES] = {
	GL_ARRAY_BUFFER,
	GL_UNIFORM_BUFFER
};


//