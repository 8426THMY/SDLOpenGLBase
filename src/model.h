#ifndef model_h
#define model_h


#include "utilTypes.h"

#include "mesh.h"
#include "skeleton.h"
#include "textureGroup.h"
#include "shader.h"


typedef struct modelDef {
	char *name;

	// Models use an individual mesh
	// for each separate texture group.
	mesh *meshes;
	// This is an array of texture group pointers, one per mesh.
	// The texture groups are stored in their respective allocator.
	const textureGroup **texGroups;
	size_t numMeshes;

	skeleton *skele;
} modelDef;

typedef struct model {
	const modelDef *mdlDef;
	// The size of this array should
	// always be equal to "mdlDef->numMeshes".
	textureGroupState *texStates;

	//billboard billboardData;
} model;


void modelInit(model *const restrict mdl, const modelDef *const restrict mdlDef);

#warning "The '.c' file is a bit messy. It would be nice to move the loading code out."
modelDef *modelDefOBJLoad(const char *const restrict mdlDefPath, const size_t mdlDefPathLength);
modelDef *modelDefSMDLoad(const char *const restrict mdlDefPath, const size_t mdlDefPathLength);

void modelUpdate(model *const restrict mdl, const float dt);
void modelDraw(
	const model *const restrict mdl, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const meshShader *const restrict shader
);

void modelDelete(model *const restrict mdl);
void modelDefDelete(modelDef *const restrict mdlDef);

return_t modelSetup();
void modelCleanup();


extern modelDef g_mdlDefDefault;


#endif