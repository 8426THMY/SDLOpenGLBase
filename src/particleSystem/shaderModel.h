#ifndef shaderModel_h
#define shaderModel_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


typedef struct shaderModel {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} shaderModel;


return_t shaderModelInit(shaderModel *const restrict shader, const GLuint programID);


#endif