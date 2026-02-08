#ifndef camera_h
#define camera_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat4.h"


#ifndef CAMERA_NEAR
	#define CAMERA_NEAR 0.1f
#endif
#ifndef CAMERA_FAR
	#define CAMERA_FAR 1000.f
#endif


#define CAMERA_TYPE_FRUSTUM    0
#define CAMERA_TYPE_ORTHO      1
#define CAMERA_TYPE_FIXED_SIZE 2


typedef struct cameraState {
	vec3 pos;
	quat rot;
	float fov;
} cameraState;

typedef uint_least8_t cameraType;

/*
** Base camera structure. By itself, it can't really do much.
** For a dynamic camera, we need to use one of the camera controllers.
*/
typedef struct camera {
	cameraState state;
	cameraState prevState;
	cameraType type;
} camera;


void cameraInit(camera *const restrict cam, const cameraType type);

void cameraComputeViewMatrix(
	const camera *const restrict cam, const float dt,
	mat3x4 *const restrict viewMatrix
);
void cameraComputeProjectionMatrix(
	const camera *const restrict cam, const float dt,
	const float width, const float height,
	mat4 *const restrict projMatrix
);

#warning "We should get the interpolated state for these..."
#if 0
float cameraDistance(const camera *const restrict cam, const vec3 *const restrict target);
float cameraDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target);
float cameraSignedDistance(const camera *const restrict cam, const vec3 *const restrict target);
float cameraSignedDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target);
#endif


#endif