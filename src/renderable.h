#ifndef renderable_h
#define renderable_h


#include "vec3.h"
#include "mat4.h"

#include "mesh.h"
#include "model.h"
#include "textureGroup.h"
#include "skeleton.h"

#include "billboard.h"
#include "camera.h"
#include "shader.h"

#include "utilTypes.h"


#error "Merge these with models. We should have 'modelBases' and 'models', the latter of which represents instances."
#error "While we're at it, maybe rename our other 'defs' to 'bases'."

#error "Should models only store a single mesh and texture group like before? That would mean we need multiple renderables for a 'full' model."


typedef struct renderableDef {
	model *mdl;
	// Default array of texture groups for this renderable.
	// The size of this array should always be equal to
	// "mdl->numMeshes" so that this array matches the model's.
	textureGroup **texGroups;
} renderableDef;

typedef struct renderable {
	model *mdl;
	// The size of this array should
	// always be equal to "mdl->numMeshes".
	textureGroupState *texStates;

	//billboard billboardData;
} renderable;


void renderableDefInit(
	renderableDef *const restrict renderDef,
	model *const restrict mdl, textureGroup **const restrict texGroups
);
void renderableInit(renderable *const restrict render, const renderableDef *const restrict renderDef);

void renderableUpdate(renderable *const restrict render, const float time);
void renderableDraw(
	const renderable *const restrict render, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const meshShader *const restrict shader
);

void renderableDelete(renderable *const restrict render);
void renderableDefDelete(renderableDef *const restrict renderDef);


#endif