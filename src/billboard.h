#ifndef billboard_h
#define billboard_h


#include "vec3.h"

#include "camera.h"

#include "utilTypes.h"


#define BILLBOARD_DISABLED         0x00
// Stop the renderable from billboarding on certain axes.
#define BILLBOARD_LOCK_X           0x01
#define BILLBOARD_LOCK_Y           0x02
#define BILLBOARD_LOCK_Z           0x04
#define BILLBOARD_LOCK_XYZ         0x07
// Make the renderable keep a fixed scale regardless of distance.
#define BILLBOARD_SCALE            0x08
// Cheap billboarding for sprites, just use the camera's rotation matrix.
#define BILLBOARD_SPRITE           0x10
// Billboard like a sprite, but use axis locking.
#define BILLBOARD_TARGET_SPRITE    0x20
// Billboard towards the camera's actual position.
#define BILLBOARD_TARGET_CAMERA    0x40
// Billboard towards a specific target.
#define BILLBOARD_TARGET_ARBITRARY 0x80

// Distance an object must be from the camera to have a 1:1 scale.
#ifndef BILLBOARD_SCALE_CALIBRATION_DISTANCE
	#define BILLBOARD_SCALE_CALIBRATION_DISTANCE 7.5f
#endif


// Stores the data required to billboard an object.
typedef struct billboard {
	vec3 *axis;
	vec3 *target;
	float scale;

	flags_t flags;
} billboard;


void billboardInit(billboard *const restrict bbData);

void billboardState(const billboard *const restrict bbData, const camera *const restrict cam, const vec3 centroid, mat4 rootState, mat4 *const restrict out);


#endif