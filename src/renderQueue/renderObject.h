#ifndef renderObject_h
#define renderObject_h


#include <stdint.h>

#include "renderFrustum.h"
#include "renderView.h"

#include "utilTypes.h"


#define RENDEROBJECT_NUM_TYPES 2

#define RENDEROBJECT_ANIMATED_MODEL  0
#define RENDEROBJECT_PARTICLE_SYSTEM 1


#warning "We need to think about how we want to handle particle systems."
#warning "A single particle system can have a lot of nodes, which can each have different textures."
#warning "The way things work currently, the entire particle system gets drawn together."
#warning "We could potentially make it so render objects can be particle system nodes instead of particle systems."
#warning "The difficulty then is that updating the global transformation becomes difficult unless every node knows the identity of the root system."


typedef uint_least8_t renderObjectType;

typedef struct renderObject {
	union {
		model mdl;
		particleSystem partSys;
	} data;
	renderObjectType type;

	// This is the I.D. of the last render view to draw this render object.
	// A render object can be in multiple leaves of the visibility culler,
	// so we use this to make sure it doesn't get drawn more than once.
	unsigned int viewID;
	// This is the I.D. of the last frame this render object was drawn on.
	// The global transform should only be updated once per frame.
	unsigned int frameID;
} renderObject;


return_t renderObjInFrustum(
	const renderObject *const restrict obj,
	const renderFrustum *const restrict frustum
);
renderQueueID renderObjGetRenderQueueKey(
	const renderObject *const restrict obj,
	renderQueueKey *const restrict key
);
void renderObjUpdateGlobalTransform(renderObject *const restrict obj);
void renderObjPreDraw(
	const renderObject *const restrict obj,
	renderView *const restrict view
);
void renderObjDraw(const renderObject *const restrict obj);


extern return_t (*const renderObjInFrustumTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj,
	const renderFrustum *const restrict frustum
);
extern renderQueueID (*const renderObjGetRenderQueueKeyTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj,
	renderQueueKey *const restrict key
);
extern void (*const renderObjUpdateGlobalTransformTable[RENDEROBJECT_NUM_TYPES])(
	renderObject *const restrict obj
);
extern void (*const renderObjPreDrawTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj,
	renderView *const restrict view
);
extern void (*const renderObjDrawTable[RENDEROBJECT_NUM_TYPES])(
	const renderObject *const restrict obj
);


#endif