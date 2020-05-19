#ifndef shader_h
#define shader_h


#define SHADER_INVALID_ID 0


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


GLuint shaderLoad(const char *const restrict shaderPath, const GLenum shaderType);
GLuint shaderLoadProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID);

void shaderDelete(const GLuint shaderID);
void shaderDeleteProgram(const GLuint programID);


#endif
