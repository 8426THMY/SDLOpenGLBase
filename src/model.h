#ifndef model_h
#define model_h


#define MODEL_VERTEX_MAX_WEIGHTS 4


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"

#include "skeleton.h"


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

return_t modelLoadOBJ(model *mdl, const char *mdlName);
return_t modelLoadSMD(model *mdl, const char *mdlName);
return_t modelSetupError();

void modelDelete(model *mdl);


#endif