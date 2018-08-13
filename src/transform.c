#include "transform.h"


void transformStateInit(transformState *transform){
	interpVec3InitZero(&transform->pos);
	interpQuatInitIdentity(&transform->rot);
	interpVec3InitSet(&transform->scale, 1.f, 1.f, 1.f);
}


void transformStateAddPos(transformState *transform, const float x, const float y, const float z, const float time){
	vec3Add(&transform->pos.next, x * time, y * time, z * time, &transform->pos.next);
}

void transformStateAddPosX(transformState *transform, const float x, const float time){
	transform->pos.next.x += x * time;
}

void transformStateAddPosY(transformState *transform, const float y, const float time){
	transform->pos.next.y += y * time;
}

void transformStateAddPosZ(transformState *transform, const float z, const float time){
	transform->pos.next.z += z * time;
}

void transformStateSetPos(transformState *transform, const float x, const float y, const float z){
	vec3InitSet(&transform->pos.next, x, y, z);
	transform->pos.last = transform->pos.next;
}

void transformStateSetPosX(transformState *transform, const float x){
	transform->pos.next.x = x;
	transform->pos.last.x = x;
}

void transformStateSetPosY(transformState *transform, const float y){
	transform->pos.next.y = y;
	transform->pos.last.y = y;
}

void transformStateSetPosZ(transformState *transform, const float z){
	transform->pos.next.z = z;
	transform->pos.last.x = z;
}

void transformStateSetNextPos(transformState *transform, const float x, const float y, const float z){
	vec3InitSet(&transform->pos.next, x, y, z);
}

void transformStateSetNextPosX(transformState *transform, const float x){
	transform->pos.next.x = x;
}

void transformStateSetNextPosY(transformState *transform, const float y){
	transform->pos.next.y = y;
}

void transformStateSetNextPosZ(transformState *transform, const float z){
	transform->pos.next.z = z;
}


void transformStateAddRotEulerRad(transformState *transform, const float x, const float y, const float z, const float time){
	quat q = transform->rot.next;
	quatRotateRad(&q, x, y, z);
	quatSlerpFast(&transform->rot.next, &q, time, &transform->rot.next);
}

void transformStateAddRotEulerDeg(transformState *transform, const float x, const float y, const float z, const float time){
	quat q = transform->rot.next;
	quatRotateDeg(&q, x, y, z);
	quatSlerpFast(&transform->rot.next, &q, time, &transform->rot.next);
}

void transformStateAddRotQuat(transformState *transform, const quat *q, const float time){
	quatSlerpFast(&transform->rot.next, q, time, &transform->rot.next);
}

void transformStateSetRotEulerRad(transformState *transform, const float x, const float y, const float z){
	quatInitEulerRad(&transform->rot.next, x, y, z);
	transform->rot.last = transform->rot.next;
}

void transformStateSetRotEulerDeg(transformState *transform, const float x, const float y, const float z){
	quatInitEulerDeg(&transform->rot.next, x, y, z);
	transform->rot.last = transform->rot.next;
}

void transformStateSetRotQuat(transformState *transform, const quat *q){
	transform->rot.next = *q;
	transform->rot.last = *q;
}

void transformStateSetNextRotEulerRad(transformState *transform, const float x, const float y, const float z){
	quatInitEulerRad(&transform->rot.next, x, y, z);
}

void transformStateSetNextRotEulerDeg(transformState *transform, const float x, const float y, const float z){
	quatInitEulerDeg(&transform->rot.next, x, y, z);
}

void transformStateSetNextRotQuat(transformState *transform, const quat *q){
	transform->rot.next = *q;
}


void transformStateAddScale(transformState *transform, const float x, const float y, const float z, const float time){
	vec3Add(&transform->scale.next, x * time, y * time, z * time, &transform->scale.next);
}

void transformStateAddScaleX(transformState *transform, const float x, const float time){
	transform->scale.next.x += x * time;
}

void transformStateAddScaleY(transformState *transform, const float y, const float time){
	transform->scale.next.y += y * time;
}

void transformStateAddScaleZ(transformState *transform, const float z, const float time){
	transform->scale.next.z += z * time;
}

void transformStateSetScale(transformState *transform, const float x, const float y, const float z){
	vec3InitSet(&transform->scale.next, x, y, z);
	transform->scale.last = transform->scale.next;
}

void transformStateSetScaleX(transformState *transform, const float x){
	transform->scale.next.x = x;
	transform->scale.last.x = x;
}

void transformStateSetScaleY(transformState *transform, const float y){
	transform->scale.next.y = y;
	transform->scale.last.y = y;
}

void transformStateSetScaleZ(transformState *transform, const float z){
	transform->scale.next.z = z;
	transform->scale.last.x = z;
}

void transformStateSetNextScale(transformState *transform, const float x, const float y, const float z){
	vec3InitSet(&transform->scale.next, x, y, z);
}

void transformStateSetNextScaleX(transformState *transform, const float x){
	transform->scale.next.x = x;
}

void transformStateSetNextScaleY(transformState *transform, const float y){
	transform->scale.next.y = y;
}

void transformStateSetNextScaleZ(transformState *transform, const float z){
	transform->scale.next.z = z;
}


void transformStateGenRenderState(const transformState *transform, const float time, vec3 *pos, quat *rot, vec3 *scale){
	interpVec3FindRenderState(&transform->pos, time, pos);
	interpQuatFindRenderState(&transform->rot, time, rot);
	interpVec3FindRenderState(&transform->scale, time, scale);
}