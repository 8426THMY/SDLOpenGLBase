#ifndef renderObject_h
#define renderObject_h


#define RENDEROBJMODULE


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"

#include "vector.h"

#include "mat4.h"
#include "transform.h"
#include "state.h"

#include "shader.h"

#include "textureGroup.h"
#include "skeleton.h"


typedef struct renderObjState {
	transformState transform;

	size_t mdlPos;
	textureGroupAnimInst texGroup;

	skeletonObject *skeleObj;
} renderObjState;

typedef stateObject renderObject;


/**
//Core
	//State Management
		stateShift;

	//Game logic
		input;
		update;

	//Networking
		?

//Display
	render;
**/


void renderObjStateInit(renderObjState *objState, const size_t mdlPos);
void renderObjCreate(const size_t mdlPos);

void renderObjStateDraw(const renderObjState *objState, const vec3 *camPos, const mat4 *viewProjectionMatrix, const shader *shaderProgram, const float time);
void renderObjStateShift(renderObject *renderObj);

void renderObjDelete(renderObject *renderObj);


return_t renderObjModuleSetup();
void renderObjModuleCleanup();


extern vector allRenderObjects;


#endif