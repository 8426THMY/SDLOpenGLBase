#ifndef model_h
#define model_h


#define MODELMODULE


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "vec3.h"
#include "mat4.h"
#include "quat.h"

#include "vector.h"
#include "animation.h"
#include "skeleton.h"
#include "shader.h"


typedef struct model {
	char *name;

	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint vertexArrayID;
	size_t totalIndices;

	skeleton skele;
	//This is the position of the default textureGroup that the model should use.
	size_t texGroupPos;
} model;


void modelInit(model *mdl);

size_t modelLoad(const char *mdlName);
size_t modelLoadSMD(const char *mdlName);

void modelDelete(model *mdl);


unsigned char modelModuleSetup();
void modelModuleCleanup();


extern vector loadedModels;


#endif