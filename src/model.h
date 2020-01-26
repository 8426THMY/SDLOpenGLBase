#ifndef model_h
#define model_h


#include "utilTypes.h"

#include "mesh.h"
#include "skeleton.h"
#include "textureGroup.h"


typedef struct model {
	char *name;

	mesh meshData;
	skeleton *skele;
	textureGroup *texGroup;
} model;


void modelInit(model *mdl);

model *modelOBJLoad(const char *mdlPath);
model *modelSMDLoad(const char *mdlPath);
return_t modelSetupDefault();

void modelDelete(model *mdl);


extern model g_mdlDefault;


#endif