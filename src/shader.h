#ifndef shader_h
#define shader_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


// These shaders are used for drawing most things, such as objects.
typedef struct shaderObject {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} shaderObject;

// We use these shaders for drawing particles and other sprites.
typedef struct spriteShader {
	GLuint programID;

	GLuint vpMatrixID;
} shaderSprite;


return_t shaderObjectInit(shaderObject *const restrict shader, const GLuint programID);
return_t shaderSpriteInit(shaderSprite *const restrict shader, const GLuint programID);

GLuint shaderLoad(const char *const restrict shaderPath, const GLenum shaderType);
GLuint shaderLoadProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID);

void shaderDelete(const GLuint shaderID);
void shaderDeleteProgram(const GLuint programID);


#endif
