#ifndef model_h
#define model_h


#include <stdio.h>

#include "utilTypes.h"

#include "mesh.h"
#include "skeleton.h"
#include "moduleTextureGroup.h"


typedef struct model {
	char *name;

	mesh meshData;
	skeleton *skele;
	textureGroup *texGroup;
} model;


void modelInit(model *mdl);

return_t modelLoadOBJ(model *mdl, const char *mdlPath);
return_t modelLoadSMD(model *mdl, const char *mdlPath);
return_t modelSetupDefault();

void modelDelete(model *mdl);


extern model mdlDefault;


#endif