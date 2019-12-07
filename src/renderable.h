#ifndef renderable_h
#define renderable_h


#include "model.h"
#include "textureGroup.h"
#include "skeleton.h"
#include "shader.h"


typedef struct renderableDef {
	model *mdl;
	textureGroup *texGroup;
} renderableDef;

typedef struct renderable {
	model *mdl;
	textureGroupState texState;
} renderable;


void renderableDefInit(renderableDef *renderDef, model *mdl);
void renderableInit(renderable *render, const renderableDef *renderDef);

void renderableUpdate(renderable *render, const float time);
void renderableDraw(const renderable *render, const skeleton *skele, const mat4 *animStates, const shader *shaderProgram);


#endif