#ifndef camera_h
#define camera_h


#include "vec3.h"
#include "quat.h"
#include "mat4.h"

#include "utilTypes.h"


#define CAMERA_INACTIVE     0x00
#define CAMERA_TYPE_ORTHO   0x00
#define CAMERA_TYPE_FRUSTUM 0x01


typedef struct camera {
	vec3 pos;
	quat rot;
	float fov;

	vec3 up;
	vec3 *target;

	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 viewProjectionMatrix;

	flags_t flags;
} camera;


void cameraInit(camera *const restrict cam);

void cameraUpdateViewMatrix(camera *const restrict cam);
void cameraUpdateProjectionMatrix(camera *const restrict cam, const float windowWidth, const float windowHeight);
void cameraUpdateViewProjectionMatrix(camera *const restrict cam, const float windowWidth, const float windowHeight);

float cameraDistance(const camera *const restrict cam, const vec3 *const restrict target);
float cameraDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target);


#endif