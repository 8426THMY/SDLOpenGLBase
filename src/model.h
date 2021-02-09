#ifndef model_h
#define model_h


#include "utilTypes.h"

#include "mesh.h"
#include "skeleton.h"
#include "textureGroup.h"


typedef struct model {
	char *name;

	// Models use an individual mesh
	// for each separate texture group.
	mesh *meshes;
	textureGroup **texGroups;
	size_t numMeshes;

	skeleton *skele;
} model;


model *modelOBJLoad(const char *const restrict mdlPath, const size_t mdlPathLength);
model *modelSMDLoad(const char *const restrict mdlPath, const size_t mdlPathLength);

void modelDelete(model *const restrict mdl);

return_t modelSetup();
void modelCleanup();


extern model g_mdlDefault;


#endif