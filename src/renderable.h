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


void renderableDefInit(renderableDef *const restrict renderDef, model *const restrict mdl);
void renderableInit(renderable *const restrict render, const renderableDef *const restrict renderDef);

void renderableUpdate(renderable *const restrict render, const float time);
void renderableDraw(
	const renderable *const restrict render, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const shaderObject *const restrict shader
);


#endif