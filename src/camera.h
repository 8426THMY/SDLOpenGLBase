#ifndef camera_h
#define camera_h


#include <stdlib.h>

#include "mat4.h"
#include "interp.h"
#include "state.h"


// This type is used to store the interpolated state of a camera.
typedef struct cameraInterpState {
	vec3 pos;
	quat rot;
	float fov;
} cameraInterpState;

// Stores the current and last positions of the camera.
typedef struct cameraState {
	interpVec3 pos;
	interpQuat rot;
	interpFloat fov;
} cameraState;

typedef stateObject camera;


void cameraStateInit(cameraState *camState);
void cameraInit(camera *cam);

void cameraStateAddPos(cameraState *camState, const float x, const float y, const float z, const float time);
void cameraStateAddPosX(cameraState *camState, const float x, const float time);
void cameraStateAddPosY(cameraState *camState, const float y, const float time);
void cameraStateAddPosZ(cameraState *camState, const float z, const float time);
void cameraStateSetPos(cameraState *camState, const float x, const float y, const float z);
void cameraStateSetPosX(cameraState *camState, const float x);
void cameraStateSetPosY(cameraState *camState, const float y);
void cameraStateSetPosZ(cameraState *camState, const float z);
void cameraStateAddRotEulerRad(cameraState *camState, const float x, const float y, const float z, const float time);
void cameraStateAddRotEulerDeg(cameraState *camState, const float x, const float y, const float z, const float time);
void cameraStateAddRotQuat(cameraState *camState, const quat *q, const float time);
void cameraStateSetRotEulerRad(cameraState *camState, const float x, const float y, const float z);
void cameraStateSetRotEulerDeg(cameraState *camState, const float x, const float y, const float z);
void cameraStateSetRotQuat(cameraState *camState, const quat *q);
void cameraStateAddFov(cameraState *camState, const float fov, const float time);
void cameraStateSetFov(cameraState *camState, const float fov);

void cameraStateGenRenderState(const cameraState *camState, const float time, cameraInterpState *out);
void cameraStateGenerateViewMatrix(const cameraState *camState, const float time, mat4 *out);

void cameraStateShift(camera *cam);

void cameraDelete(camera *cam);


#endif