#ifndef shader_h
#define shader_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


typedef struct shader {
	GLuint shaderID;

	GLuint mvpMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} shader;


void shaderInit(shader *shaderPrg);
return_t shaderLoad(shader *shaderPrg, const char *vertexPath, const char *fragmentPath);
void shaderDelete(shader *shaderPrg);


#endif
