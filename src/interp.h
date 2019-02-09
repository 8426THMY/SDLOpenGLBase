#ifndef interp_h
#define interp_h


#include "vec3.h"
#include "quat.h"
#include "transform.h"


typedef struct interpFloat {
	float last;
	float next;
} interpFloat;

typedef struct interpVec3 {
	vec3 last;
	vec3 next;
} interpVec3;

typedef struct interpQuat {
	quat last;
	quat next;
} interpQuat;

typedef struct interpTransform {
	interpVec3 pos;
	interpQuat rot;
	interpVec3 scale;
} interpTransform;


void interpFloatInitZero(interpFloat *iFloat);
void interpFloatInitSet(interpFloat *iFloat, const float x);
void interpVec3InitZero(interpVec3 *iVec3);
void interpVec3InitSet(interpVec3 *iVec3, const float x, const float y, const float z);
void interpQuatInitIdentity(interpQuat *iQuat);
void interpQuatInitSet(interpQuat *iQuat, const float x, const float y, const float z, const float w);
void interpTransInit(interpTransform *iTrans);

/*void interpFloatUpdateNextState(interpFloat *iFloat);
void interpVec3UpdateNextState(interpVec3 *iVec3);
void interpQuatUpdateNextState(interpQuat *iQuat);*/

void interpFloatGenRenderState(const interpFloat *iFloat, const float time, float *out);
void interpVec3GenRenderState(const interpVec3 *iVec3, const float time, vec3 *out);
void interpQuatGenRenderState(const interpQuat *iQuat, const float time, quat *out);
void interpTransGenRenderState(const interpTransform *iTrans, const float time, transformState *out);

void interpFloatMoveToNextState(interpFloat *iFloat);
void interpVec3MoveToNextState(interpVec3 *iVec3);
void interpQuatMoveToNextState(interpQuat *iQuat);

void interpTransAddPos(interpTransform *iTrans, const float x, const float y, const float z, const float time);
void interpTransAddPosX(interpTransform *iTrans, const float x, const float time);
void interpTransAddPosY(interpTransform *iTrans, const float y, const float time);
void interpTransAddPosZ(interpTransform *iTrans, const float z, const float time);
void interpTransSetPos(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetPosX(interpTransform *iTrans, const float x);
void interpTransSetPosY(interpTransform *iTrans, const float y);
void interpTransSetPosZ(interpTransform *iTrans, const float z);
void interpTransSetNextPos(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetNextPosX(interpTransform *iTrans, const float x);
void interpTransSetNextPosY(interpTransform *iTrans, const float y);
void interpTransSetNextPosZ(interpTransform *iTrans, const float z);

void interpTransAddRotEulerRad(interpTransform *iTrans, const float x, const float y, const float z, const float time);
void interpTransAddRotEulerDeg(interpTransform *iTrans, const float x, const float y, const float z, const float time);
void interpTransAddRotQuat(interpTransform *iTrans, const quat *q, const float time);
void interpTransSetRotEulerRad(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetRotEulerDeg(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetRotQuat(interpTransform *iTrans, const quat *q);
void interpTransSetNextRotEulerRad(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetNextRotEulerDeg(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetNextRotQuat(interpTransform *iTrans, const quat *q);

void interpTransAddScale(interpTransform *iTrans, const float x, const float y, const float z, const float time);
void interpTransAddScaleX(interpTransform *iTrans, const float x, const float time);
void interpTransAddScaleY(interpTransform *iTrans, const float y, const float time);
void interpTransAddScaleZ(interpTransform *iTrans, const float z, const float time);
void interpTransSetScale(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetScaleX(interpTransform *iTrans, const float x);
void interpTransSetScaleY(interpTransform *iTrans, const float y);
void interpTransSetScaleZ(interpTransform *iTrans, const float z);
void interpTransSetNextScale(interpTransform *iTrans, const float x, const float y, const float z);
void interpTransSetNextScaleX(interpTransform *iTrans, const float x);
void interpTransSetNextScaleY(interpTransform *iTrans, const float y);
void interpTransSetNextScaleZ(interpTransform *iTrans, const float z);


#endif