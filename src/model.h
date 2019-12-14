#ifndef model_h
#define model_h


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
	size_t numIndices;

	skeleton *skele;
	textureGroup *texGroup;
} model;


void modelInit(model *mdl);

return_t modelLoadOBJ(model *mdl, const char *mdlName);
return_t modelLoadSMD(model *mdl, const char *mdlName);
return_t modelSetupDefault();

void modelDelete(model *mdl);


extern model mdlDefault;


#endif