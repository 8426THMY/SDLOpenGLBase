#ifndef shader_h
#define shader_h


#define GLEW_STATIC
#include <GL/glew.h>


typedef struct shader {
	GLuint shaderID;

	GLuint modelViewMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} shader;


void shaderInit(shader *shaderProgram);
unsigned char shaderLoad(shader *shaderProgram, const char *vertexPath, const char *fragmentPath);
void shaderDelete(shader *shaderProgram);


#endif
