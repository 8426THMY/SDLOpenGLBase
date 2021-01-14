#include "interp.h"


void interpFloatInitZero(interpFloat *const restrict iFloat){
	iFloat->next = 0.f;
	iFloat->last = 0.f;
}

void interpFloatInitSet(interpFloat *const restrict iFloat, const float x){
	iFloat->next = x;
	iFloat->last = x;
}

void interpVec3InitZero(interpVec3 *const restrict iVec3){
	vec3InitZero(&iVec3->next);
	iVec3->last = iVec3->next;
}

void interpVec3InitSet(interpVec3 *const restrict iVec3, const float x, const float y, const float z){
	vec3InitSet(&iVec3->next, x, y, z);
	iVec3->last = iVec3->next;
}

void interpQuatInitIdentity(interpQuat *const restrict iQuat){
	quatInitIdentity(&iQuat->next);
	iQuat->last = iQuat->next;
}

void interpQuatInitSet(interpQuat *const restrict iQuat, const float x, const float y, const float z, const float w){
	quatInitSet(&iQuat->next, x, y, z, w);
	iQuat->last = iQuat->next;
}

void interpTransInit(interpTransform *const restrict iTrans){
	interpVec3InitZero(&iTrans->pos);
	interpQuatInitIdentity(&iTrans->rot);
	interpVec3InitSet(&iTrans->scale, 1.f, 1.f, 1.f);
}


// Interpolate between the interpFloat's two values!
void interpFloatGenRenderState(const interpFloat *const restrict iFloat, const float time, float *const restrict out){
	*out = iFloat->last + (iFloat->next - iFloat->last) * time;
}

// Interpolate between the interpVec3's two values!
void interpVec3GenRenderState(const interpVec3 *const restrict iVec3, const float time, vec3 *const restrict out){
	vec3Lerp(&iVec3->last, &iVec3->next, time, out);
}

// Interpolate between the interpQuat's two values!
void interpQuatGenRenderState(const interpQuat *const restrict iQuat, const float time, quat *const restrict out){
	quatSlerpFasterOut(&iQuat->last, &iQuat->next, time, out);
}

// Interpolate between the three components of an interpTransform!
void interpTransGenRenderState(const interpTransform *const restrict iTrans, const float time, transformState *const restrict out){
	interpVec3GenRenderState(&iTrans->pos, time, &out->pos);
	interpQuatGenRenderState(&iTrans->rot, time, &out->rot);
	interpVec3GenRenderState(&iTrans->scale, time, &out->scale);
}


void interpFloatMoveToNextState(interpFloat *const restrict iFloat){
	iFloat->last = iFloat->next;
}

void interpVec3MoveToNextState(interpVec3 *const restrict iVec3){
	iVec3->last = iVec3->next;
}

void interpQuatMoveToNextState(interpQuat *const restrict iQuat){
	iQuat->last = iQuat->next;
}


void interpTransAddPos(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time){
	vec3Add(&iTrans->pos.next, x * time, y * time, z * time);
}

void interpTransAddPosX(interpTransform *const restrict iTrans, const float x, const float time){
	iTrans->pos.next.x += x * time;
}

void interpTransAddPosY(interpTransform *const restrict iTrans, const float y, const float time){
	iTrans->pos.next.y += y * time;
}

void interpTransAddPosZ(interpTransform *const restrict iTrans, const float z, const float time){
	iTrans->pos.next.z += z * time;
}

void interpTransSetPos(interpTransform *const restrict iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->pos.next, x, y, z);
	iTrans->pos.last = iTrans->pos.next;
}

void interpTransSetPosX(interpTransform *const restrict iTrans, const float x){
	iTrans->pos.next.x = x;
	iTrans->pos.last.x = x;
}

void interpTransSetPosY(interpTransform *const restrict iTrans, const float y){
	iTrans->pos.next.y = y;
	iTrans->pos.last.y = y;
}

void interpTransSetPosZ(interpTransform *const restrict iTrans, const float z){
	iTrans->pos.next.z = z;
	iTrans->pos.last.x = z;
}

void interpTransSetNextPos(interpTransform *const restrict iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->pos.next, x, y, z);
}

void interpTransSetNextPosX(interpTransform *const restrict iTrans, const float x){
	iTrans->pos.next.x = x;
}

void interpTransSetNextPosY(interpTransform *const restrict iTrans, const float y){
	iTrans->pos.next.y = y;
}

void interpTransSetNextPosZ(interpTransform *const restrict iTrans, const float z){
	iTrans->pos.next.z = z;
}


void interpTransAddRotEuler(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time){
	quat q = iTrans->rot.next;
	quatRotateByXYZ(&q, x, y, z);
	quatSlerpFaster(&iTrans->rot.next, &q, time);
}

void interpTransAddRotQuat(interpTransform *const restrict iTrans, const quat *const restrict q, const float time){
	quatSlerpFaster(&iTrans->rot.next, q, time);
}

void interpTransSetRotEuler(interpTransform *const restrict iTrans, const float x, const float y, const float z){
	quatInitEulerXYZ(&iTrans->rot.next, x, y, z);
	iTrans->rot.last = iTrans->rot.next;
}

void interpTransSetRotQuat(interpTransform *const restrict iTrans, const quat *const restrict q){
	iTrans->rot.next = *q;
	iTrans->rot.last = *q;
}

void interpTransSetNextRotEuler(interpTransform *const restrict iTrans, const float x, const float y, const float z){
	quatInitEulerXYZ(&iTrans->rot.next, x, y, z);
}

void interpTransSetNextRotQuat(interpTransform *const restrict iTrans, const quat *const restrict q){
	iTrans->rot.next = *q;
}


// Below this comment are the functions for interpTransforms. There are a lot of them.
void interpTransAddScale(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time){
	vec3Add(&iTrans->scale.next, x * time, y * time, z * time);
}

void interpTransAddScaleX(interpTransform *const restrict iTrans, const float x, const float time){
	iTrans->scale.next.x += x * time;
}

void interpTransAddScaleY(interpTransform *const restrict iTrans, const float y, const float time){
	iTrans->scale.next.y += y * time;
}

void interpTransAddScaleZ(interpTransform *const restrict iTrans, const float z, const float time){
	iTrans->scale.next.z += z * time;
}

void interpTransSetScale(interpTransform *const restrict iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->scale.next, x, y, z);
	iTrans->scale.last = iTrans->scale.next;
}

void interpTransSetScaleX(interpTransform *const restrict iTrans, const float x){
	iTrans->scale.next.x = x;
	iTrans->scale.last.x = x;
}

void interpTransSetScaleY(interpTransform *const restrict iTrans, const float y){
	iTrans->scale.next.y = y;
	iTrans->scale.last.y = y;
}

void interpTransSetScaleZ(interpTransform *const restrict iTrans, const float z){
	iTrans->scale.next.z = z;
	iTrans->scale.last.x = z;
}

void interpTransSetNextScale(interpTransform *const restrict iTrans, const float x, const float y, const float z){
	vec3InitSet(&iTrans->scale.next, x, y, z);
}

void interpTransSetNextScaleX(interpTransform *const restrict iTrans, const float x){
	iTrans->scale.next.x = x;
}

void interpTransSetNextScaleY(interpTransform *const restrict iTrans, const float y){
	iTrans->scale.next.y = y;
}

void interpTransSetNextScaleZ(interpTransform *const restrict iTrans, const float z){
	iTrans->scale.next.z = z;
}