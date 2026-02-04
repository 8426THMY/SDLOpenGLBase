#ifndef model_h
#define model_h


#include "mat3x4.h"

#include "mesh.h"
#include "skeleton.h"
#include "textureGroup.h"
#include "shader.h"

#include "utilTypes.h"


typedef struct modelDef {
	char *name;

	// Models use an individual mesh
	// for each separate texture group.
	#warning "It would be better to store everything in a single mesh, then store where the indices start and end for each submesh."
	mesh *meshes;
	// This is an array of texture group pointers, one per mesh.
	// The texture groups are stored in their respective allocator.
	textureGroup **texGroups;
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
	const mat3x4 *const restrict animStates, const meshShader *const restrict shader
);

void modelDelete(model *const restrict mdl);
void modelDefDelete(modelDef *const restrict mdlDef);

return_t modelSetup();
void modelCleanup();


extern modelDef g_mdlDefDefault;


#endif