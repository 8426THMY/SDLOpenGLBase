#ifndef light_h
#define light_h


#include <stdint.h>

#include "lightSpot.h"
#include "lightPoint.h"
#include "lightSpotOrtho.h"
#include "lightOrtho.h"


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

	// This is the I.D. of the last render view to draw this light.
	// A light can be in multiple leaves of the visibility culler,
	// so we use this to make sure it doesn't get drawn more than once.
	unsigned int viewID;
	// This is the I.D. of the last frame this light was drawn on.
	// The global transform should only be updated once per frame.
	unsigned int frameID;
} light;


#endif