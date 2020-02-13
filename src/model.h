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


void modelInit(model *const restrict mdl);

model *modelOBJLoad(const char *const restrict mdlPath, const size_t mdlPathLength);
model *modelSMDLoad(const char *const restrict mdlPath, const size_t mdlPathLength);
return_t modelSetupDefault();

void modelDelete(model *const restrict mdl);


extern model g_mdlDefault;


#endif