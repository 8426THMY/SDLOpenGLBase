#include "camera.h"


#include "settingsProgram.h"


void cameraStateInit(cameraState *camState){
	interpVec3InitZero(&camState->pos);
	interpQuatInitIdentity(&camState->rot);
	interpFloatInitSet(&camState->fov, FOV_DEFAULT);
}

void cameraInit(camera *cam){
	stateObjInit(cam, sizeof(cameraState));
	cameraStateInit((cameraState *)cam->states[0]);
}


void cameraStateAddPos(cameraState *camState, const float x, const float y, const float z, const float time){
	vec3Add(&camState->pos.next, x * time, y * time, z * time);
}

void cameraStateAddPosX(cameraState *camState, const float x, const float time){
	camState->pos.next.x += x * time;
}

void cameraStateAddPosY(cameraState *camState, const float y, const float time){
	camState->pos.next.y += y * time;
}

void cameraStateAddPosZ(cameraState *camState, const float z, const float time){
	camState->pos.next.z += z * time;
}

void cameraStateSetPos(cameraState *camState, const float x, const float y, const float z){
	vec3InitSet(&camState->pos.next, x, y, z);
	camState->pos.last = camState->pos.next;
}

void cameraStateSetPosX(cameraState *camState, const float x){
	camState->pos.next.x = x;
	camState->pos.last.x = x;
}

void cameraStateSetPosY(cameraState *camState, const float y){
	camState->pos.next.y = y;
	camState->pos.last.y = y;
}

void cameraStateSetPosZ(cameraState *camState, const float z){
	camState->pos.next.z = z;
	camState->pos.last.x = z;
}

void cameraStateAddRotEulerRad(cameraState *camState, const float x, const float y, const float z, const float time){
	quat q = camState->rot.next;
	quatRotateRad(&q, x, y, z);
	quatSlerpFast(&camState->rot.next, &q, time, &camState->rot.next);
}

void cameraStateAddRotEulerDeg(cameraState *camState, const float x, const float y, const float z, const float time){
	quat q = camState->rot.next;
	quatRotateDeg(&q, x, y, z);
	quatSlerpFast(&camState->rot.next, &q, time, &camState->rot.next);
}

void cameraStateAddRotQuat(cameraState *camState, const quat *q, const float time){
	quatSlerpFast(&camState->rot.next, q, time, &camState->rot.next);
}

void cameraStateSetRotEulerRad(cameraState *camState, const float x, const float y, const float z){
	quatInitEulerRad(&camState->rot.next, x, y, z);
	camState->rot.last = camState->rot.next;
}

void cameraStateSetRotEulerDeg(cameraState *camState, const float x, const float y, const float z){
	quatInitEulerDeg(&camState->rot.next, x, y, z);
	camState->rot.last = camState->rot.next;
}

void cameraStateSetRotQuat(cameraState *camState, const quat *q){
	camState->rot.next = *q;
	camState->rot.last = *q;
}

void cameraStateAddFov(cameraState *camState, const float fov, const float time){
	camState->fov.next += fov * time;
}

void cameraStateSetFov(cameraState *camState, const float fov){
	camState->fov.next = fov;
	camState->fov.last = fov;
}


//Interpolate between the camera's previous state and current state!
void cameraStateGenRenderState(const cameraState *camState, const float time, cameraInterpState *out){
	interpVec3GenRenderState(&camState->pos, time, &out->pos);
	interpQuatGenRenderState(&camState->rot, time, &out->rot);
	interpFloatGenRenderState(&camState->fov, time, &out->fov);
}

//Find the camera's render state and generate a view matrix for it!
/** To create a view matrix, just create a regular transformation matrix! The camera's position will need to be reversed though. **/
void cameraStateGenerateViewMatrix(const cameraState *camState, const float time, mat4 *out){
	cameraInterpState camInterp;
	cameraStateGenRenderState(camState, time, &camInterp);
	vec3Negate(&camInterp.pos);

	mat4TranslateVec3(out, &camInterp.pos);
	mat4RotateQuat(out, &camInterp.rot);
}


void cameraStateShift(camera *cam){
	stateObjShift(cam, sizeof(cameraState), NULL, NULL);

	cameraState *currentState = cam->states[0];
	if(currentState != NULL){
		interpVec3MoveToNextState(&currentState->pos);
		interpQuatMoveToNextState(&currentState->rot);
		interpFloatMoveToNextState(&currentState->fov);
	}
}


void cameraDelete(camera *cam){
	stateObjDelete(cam, NULL);
}