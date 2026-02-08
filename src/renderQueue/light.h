#ifndef light_h
#define light_h


#include <stdint.h>

#include "lightSpot.h"
#include "lightPoint.h"
#include "lightSpotOrtho.h"
#include "lightOrtho.h"

#include "utilTypes.h"


#define LIGHT_NUM_TYPES 4

#define LIGHT_TYPE_SPOT      0
#define LIGHT_TYPE_POINT     1
#define LIGHT_TYPE_SPOTORTHO 2
#define LIGHT_TYPE_ORTHO     3


#warning "We need to think of some way of only updating shadow maps when something moves."


typedef uint_least8_t lightType;

typedef struct light {
	union {
		lightSpot spot;
		lightPoint point;
		lightSpotOrtho spotOrtho;
		lightOrtho ortho;
	} data;
	lightType type;

	// This is the ID of the last render view to draw this light.
	// A light can be in multiple leaves of the visibility culler,
	// so we use this to make sure it doesn't get drawn more than once.
	unsigned int viewID;
	// This is the ID of the last frame this light was drawn on.
	// The global transform should only be updated once per frame.
	unsigned int frameID;
} light;


return_t lightInFrustum(
	const light *const restrict l,
	const renderFrustum *const restrict frustum
);
void lightUpdateGlobalTransform(light *const restrict l);
void lightPreDraw(
	const light *const restrict l,
	renderView *const restrict view
);
void lightDraw(const light *const restrict l);


#endif