#include "interp.h"


void interpFloatInitZero(interpFloat *iFloat){
	iFloat->next = 0.f;
	iFloat->last = 0.f;
}

void interpFloatInitSet(interpFloat *iFloat, const float x){
	iFloat->next = x;
	iFloat->last = x;
}

void interpVec3InitZero(interpVec3 *iVec3){
	vec3InitZero(&iVec3->next);
	iVec3->last = iVec3->next;
}

void interpVec3InitSet(interpVec3 *iVec3, const float x, const float y, const float z){
	vec3InitSet(&iVec3->next, x, y, z);
	iVec3->last = iVec3->next;
}

void interpQuatInitIdentity(interpQuat *iQuat){
	quatInitIdentity(&iQuat->next);
	iQuat->last = iQuat->next;
}

void interpQuatInitSet(interpQuat *iQuat, const float x, const float y, const float z, const float w){
	quatInitSet(&iQuat->next, x, y, z, w);
	iQuat->last = iQuat->next;
}

void interpTransInit(interpTransform *iTrans){
	interpVec3InitZero(&iTrans->pos);
	interpQuatInitIdentity(&iTrans->rot);
	interpVec3InitSet(&iTrans->scale, 1.f, 1.f, 1.f);
}


// Interpolate between the interpFloat's two values!
void interpFloatGenRenderState(const interpFloat *iFloat, const float time, float *out){
	*out = iFloat->last + (iFloat->next - iFloat->last) * time;
}

// Interpolate between the interpVec3's two values!
void interpVec3GenRenderState(const interpVec3 *iVec3, const float time, vec3 *out){
	vec3Lerp(&iVec3->last, &iVec3->next, time, out);
}

// Interpolate between the interpQuat's two values!
void interpQuatGenRenderState(const interpQuat *iQuat, const float time, quat *out){
	quatSlerpFastOut(&iQuat->last, &iQuat->next, time, out);
}

// Interpolate between the three components of an interpTransform!
void interpTransGenRenderState(const interpTransform *iTrans, const float time, transformState *out){
	interpVec3GenRenderState(&iTrans->pos, time, &out->pos);
	interpQuatGenRenderState(&iTrans->rot, time, &out->rot);
	interpVec3GenRenderState(&iTrans->scale, time, &out->scale);
}


void interpFloatMoveToNextState(interpFloat *iFloat){
	iFloat->last = iFloat->next;
}

void interpVec3MoveToNextState(interpVec3 *iVec3){
	iVec3->last = iVec3->next;
}

void interpQuatMoveToNextState(interpQuat *iQuat){
	iQuat->last = iQuat->next;
}


void interpTransAddPos(interpTransform *iTrans, const float x, const float y, const float z, const float time){
	vec3Add(&iTrans->pos.next, x * time, y * time, z * time);
}

void interpTransAddPosX(interpTransform *iTrans, const float x, const float time){
	iTrans->pos.next.x += x * time;
}

void interpTransAddPosY(interpTransform *iTrans, const float y, const float time){
	iTrans->pos.next.y += y * time;
}

void interpTransAddPosZ(interpTransform *iTrans, const float z, const float time){
	iTrans->pos.next.z += z * time;
}

void interpTransSetPos(interpTransform *iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->pos.next, x, y, z);
	iTrans->pos.last = iTrans->pos.next;
}

void interpTransSetPosX(interpTransform *iTrans, const float x){
	iTrans->pos.next.x = x;
	iTrans->pos.last.x = x;
}

void interpTransSetPosY(interpTransform *iTrans, const float y){
	iTrans->pos.next.y = y;
	iTrans->pos.last.y = y;
}

void interpTransSetPosZ(interpTransform *iTrans, const float z){
	iTrans->pos.next.z = z;
	iTrans->pos.last.x = z;
}

void interpTransSetNextPos(interpTransform *iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->pos.next, x, y, z);
}

void interpTransSetNextPosX(interpTransform *iTrans, const float x){
	iTrans->pos.next.x = x;
}

void interpTransSetNextPosY(interpTransform *iTrans, const float y){
	iTrans->pos.next.y = y;
}

void interpTransSetNextPosZ(interpTransform *iTrans, const float z){
	iTrans->pos.next.z = z;
}


void interpTransAddRotEulerRad(interpTransform *iTrans, const float x, const float y, const float z, const float time){
	quat q = iTrans->rot.next;
	quatRotateByRad(&q, x, y, z);
	quatSlerpFast(&iTrans->rot.next, &q, time);
}

void interpTransAddRotEulerDeg(interpTransform *iTrans, const float x, const float y, const float z, const float time){
	quat q = iTrans->rot.next;
	quatRotateByDeg(&q, x, y, z);
	quatSlerpFast(&iTrans->rot.next, &q, time);
}

void interpTransAddRotQuat(interpTransform *iTrans, const quat *q, const float time){
	quatSlerpFast(&iTrans->rot.next, q, time);
}

void interpTransSetRotEulerRad(interpTransform *iTrans, const float x, const float y, const float z){
	quatInitEulerRad(&iTrans->rot.next, x, y, z);
	iTrans->rot.last = iTrans->rot.next;
}

void interpTransSetRotEulerDeg(interpTransform *iTrans, const float x, const float y, const float z){
	quatInitEulerDeg(&iTrans->rot.next, x, y, z);
	iTrans->rot.last = iTrans->rot.next;
}

void interpTransSetRotQuat(interpTransform *iTrans, const quat *q){
	iTrans->rot.next = *q;
	iTrans->rot.last = *q;
}

void interpTransSetNextRotEulerRad(interpTransform *iTrans, const float x, const float y, const float z){
	quatInitEulerRad(&iTrans->rot.next, x, y, z);
}

void interpTransSetNextRotEulerDeg(interpTransform *iTrans, const float x, const float y, const float z){
	quatInitEulerDeg(&iTrans->rot.next, x, y, z);
}

void interpTransSetNextRotQuat(interpTransform *iTrans, const quat *q){
	iTrans->rot.next = *q;
}


// Below this comment are the functions for interpTransforms. There are a lot of them.
void interpTransAddScale(interpTransform *iTrans, const float x, const float y, const float z, const float time){
	vec3Add(&iTrans->scale.next, x * time, y * time, z * time);
}

void interpTransAddScaleX(interpTransform *iTrans, const float x, const float time){
	iTrans->scale.next.x += x * time;
}

void interpTransAddScaleY(interpTransform *iTrans, const float y, const float time){
	iTrans->scale.next.y += y * time;
}

void interpTransAddScaleZ(interpTransform *iTrans, const float z, const float time){
	iTrans->scale.next.z += z * time;
}

void interpTransSetScale(interpTransform *iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->scale.next, x, y, z);
	iTrans->scale.last = iTrans->scale.next;
}

void interpTransSetScaleX(interpTransform *iTrans, const float x){
	iTrans->scale.next.x = x;
	iTrans->scale.last.x = x;
}

void interpTransSetScaleY(interpTransform *iTrans, const float y){
	iTrans->scale.next.y = y;
	iTrans->scale.last.y = y;
}

void interpTransSetScaleZ(interpTransform *iTrans, const float z){
	iTrans->scale.next.z = z;
	iTrans->scale.last.x = z;
}

void interpTransSetNextScale(interpTransform *iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->scale.next, x, y, z);
}

void interpTransSetNextScaleX(interpTransform *iTrans, const float x){
	iTrans->scale.next.x = x;
}

void interpTransSetNextScaleY(interpTransform *iTrans, const float y){
	iTrans->scale.next.y = y;
}

void interpTransSetNextScaleZ(interpTransform *iTrans, const float z){
	iTrans->scale.next.z = z;
}