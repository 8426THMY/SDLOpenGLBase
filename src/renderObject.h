#ifndef renderObject_h
#define renderObject_h


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"
#include "vector.h"
#include "mat4.h"

#include "interp.h"
#include "state.h"

#include "textureGroup.h"
#include "skeleton.h"

#include "shader.h"


#define RENDEROBJMODULE


typedef struct renderObjState {
	interpTransform transform;

	size_t mdlPos;
	textureGroupAnim texGroup;

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