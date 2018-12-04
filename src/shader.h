#ifndef shader_h
#define shader_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


typedef struct shader {
	GLuint shaderID;

	GLuint modelViewMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} shader;


void shaderInit(shader *shaderProgram);
return_t shaderLoad(shader *shaderProgram, const char *vertexPath, const char *fragmentPath);
void shaderDelete(shader *shaderProgram);


#endif
