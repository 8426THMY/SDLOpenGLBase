#ifndef camera_h
#define camera_h


#include "vec3.h"
#include "quat.h"
#include "mat4.h"

#include "utilTypes.h"


#ifndef CAMERA_FAR
	#define CAMERA_FAR 1000.f
#endif

#ifndef CAMERA_NEAR
	#define CAMERA_NEAR 0.1f
#endif


#define CAMERA_INACTIVE        0x00
#define CAMERA_TYPE_ORTHO      0x01
#define CAMERA_TYPE_FRUSTUM    0x02
#define CAMERA_TYPE_FIXED_SIZE 0x04


/*
** Base camera structure. By itself, it can't really do much.
** For a dynamic camera, we need to use one of the camera controllers.
*/
typedef struct camera {
	vec3 pos;
	// This is mostly just used for
	// interpolation between frames.
	quat rot;

	vec3 up;
	float fov;

	// The view matrix is generally
	// updated by a camera controller.
	mat4 viewMatrix;
	// The projection matrix is generally updated right before
	// the view projection matrix, and is handled by the camera.
	mat4 projMatrix;
	mat4 vpMatrix;

	flags8_t flags;
} camera;


void cameraInit(
	camera *const restrict cam, const flags8_t flags,
	const float width, const float height
);

void cameraUpdateProjectionMatrix(
	camera *const restrict cam,
	const float width, const float height
);
void cameraUpdateViewProjectionMatrix(camera *const restrict cam);

float cameraDistance(const camera *const restrict cam, const vec3 *const restrict target);
float cameraDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target);
float cameraSignedDistance(const camera *const restrict cam, const vec3 *const restrict target);
float cameraSignedDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target);


#endif