#ifndef shader_h
#define shader_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


typedef struct shader {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} shader;


void shaderInit(shader *shaderPrg);

GLuint shaderLoadVertex(const char *shaderPath, const GLenum shaderType);
return_t shaderLoadProgram(shader *shaderPrg, const char *vertexPath, const char *fragmentPath);

void shaderDelete(shader *shaderPrg);


#endif
