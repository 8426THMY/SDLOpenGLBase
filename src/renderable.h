#ifndef renderable_h
#define renderable_h


#include "vec3.h"
#include "mat4.h"

#include "model.h"
#include "textureGroup.h"
#include "skeleton.h"

#include "billboard.h"
#include "camera.h"
#include "shader.h"

#include "utilTypes.h"


typedef struct renderableDef {
	model *mdl;
	textureGroup *texGroup;
} renderableDef;

typedef struct renderable {
	model *mdl;
	textureGroupState texState;
	billboard billboardData;
} renderable;


void renderableDefInit(renderableDef *renderDef, model *mdl);
void renderableInit(renderable *render, const renderableDef *renderDef);

void renderableUpdate(renderable *render, const float time);
void renderableDraw(const renderable *render, const skeleton *objSkele, const mat4 *animStates, const shaderObject *shader);

void renderableGenerateBillboardMatrix(
	const renderable *render, const camera *cam,
	const vec3 *centroid, const mat4 *root, mat4 *out
);


#endif