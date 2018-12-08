#ifndef model_h
#define model_h


#define MODEL_VERTEX_MAX_WEIGHTS 4


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"

#include "skeleton.h"
#include "moduleTextureGroup.h"


typedef struct model {
	char *name;

	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint vertexArrayID;
	size_t totalIndices;

	skeleton skele;
	textureGroup *texGroup;
} model;


void modelInit(model *mdl);

return_t modelLoadOBJ(model *mdl, const char *mdlName);
return_t modelLoadSMD(model *mdl, const char *mdlName);
return_t modelSetupError();

void modelDelete(model *mdl);


extern model errorMdl;


#endif