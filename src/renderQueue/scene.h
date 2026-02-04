#ifndef scene_h
#define scene_h


#include <stddef.h>

//#include "entity.h"
#include "camera.h"

#include "renderView.h"
#include "renderTarget.h"
#include "visCuller.h"


#warning "We might need sceneDefs and sceneInstances."
#warning "Also, how do we want to support particles?"


/*
** A scene should be thought of as a level or map. It has an
** array of objects and lights that can be drawn and an array
** of renderTargets, which define perspectives to draw from.
*/
typedef struct scene {
	//entity *entities;
	camera *cams;

	#warning "We could probably have an array of cullers."
	visCuller culler;

	renderTarget *targets;
	size_t numTargets;
	#warning "We should probably have a special render target for all shadow maps."
	#warning "When a light is added to a render view, we add the light's views to the shadow map render target."

	// This counter is incremented once per render view, and helps
	// us avoid adding an object to its render queue multiple times.
	unsigned int viewID;
	// This counter is incremented once every frame, and helps
	// us avoid computing a lights' shadow maps multiple times.
	unsigned int frameID;
} scene;


void sceneDraw(scene *const restrict scn, const float dt);


#endif