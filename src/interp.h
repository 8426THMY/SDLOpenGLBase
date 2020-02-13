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


void interpFloatInitZero(interpFloat *const restrict iFloat);
void interpFloatInitSet(interpFloat *const restrict iFloat, const float x);
void interpVec3InitZero(interpVec3 *const restrict iVec3);
void interpVec3InitSet(interpVec3 *const restrict iVec3, const float x, const float y, const float z);
void interpQuatInitIdentity(interpQuat *const restrict iQuat);
void interpQuatInitSet(interpQuat *const restrict iQuat, const float x, const float y, const float z, const float w);
void interpTransInit(interpTransform *const restrict iTrans);

/*void interpFloatUpdateNextState(interpFloat *iFloat);
void interpVec3UpdateNextState(interpVec3 *iVec3);
void interpQuatUpdateNextState(interpQuat *iQuat);*/

void interpFloatGenRenderState(const interpFloat *const restrict iFloat, const float time, float *const restrict out);
void interpVec3GenRenderState(const interpVec3 *const restrict iVec3, const float time, vec3 *const restrict out);
void interpQuatGenRenderState(const interpQuat *const restrict iQuat, const float time, quat *const restrict out);
void interpTransGenRenderState(const interpTransform *const restrict iTrans, const float time, transformState *const restrict out);

void interpFloatMoveToNextState(interpFloat *const restrict iFloat);
void interpVec3MoveToNextState(interpVec3 *const restrict iVec3);
void interpQuatMoveToNextState(interpQuat *const restrict iQuat);

void interpTransAddPos(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time);
void interpTransAddPosX(interpTransform *const restrict iTrans, const float x, const float time);
void interpTransAddPosY(interpTransform *const restrict iTrans, const float y, const float time);
void interpTransAddPosZ(interpTransform *const restrict iTrans, const float z, const float time);
void interpTransSetPos(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetPosX(interpTransform *const restrict iTrans, const float x);
void interpTransSetPosY(interpTransform *const restrict iTrans, const float y);
void interpTransSetPosZ(interpTransform *const restrict iTrans, const float z);
void interpTransSetNextPos(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetNextPosX(interpTransform *const restrict iTrans, const float x);
void interpTransSetNextPosY(interpTransform *const restrict iTrans, const float y);
void interpTransSetNextPosZ(interpTransform *const restrict iTrans, const float z);

void interpTransAddRotEulerRad(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time);
void interpTransAddRotEulerDeg(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time);
void interpTransAddRotQuat(interpTransform *const restrict iTrans, const quat *const restrict q, const float time);
void interpTransSetRotEulerRad(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetRotEulerDeg(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetRotQuat(interpTransform *const restrict iTrans, const quat *const restrict q);
void interpTransSetNextRotEulerRad(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetNextRotEulerDeg(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetNextRotQuat(interpTransform *const restrict iTrans, const quat *const restrict q);

void interpTransAddScale(interpTransform *const restrict iTrans, const float x, const float y, const float z, const float time);
void interpTransAddScaleX(interpTransform *const restrict iTrans, const float x, const float time);
void interpTransAddScaleY(interpTransform *const restrict iTrans, const float y, const float time);
void interpTransAddScaleZ(interpTransform *const restrict iTrans, const float z, const float time);
void interpTransSetScale(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetScaleX(interpTransform *const restrict iTrans, const float x);
void interpTransSetScaleY(interpTransform *const restrict iTrans, const float y);
void interpTransSetScaleZ(interpTransform *const restrict iTrans, const float z);
void interpTransSetNextScale(interpTransform *const restrict iTrans, const float x, const float y, const float z);
void interpTransSetNextScaleX(interpTransform *const restrict iTrans, const float x);
void interpTransSetNextScaleY(interpTransform *const restrict iTrans, const float y);
void interpTransSetNextScaleZ(interpTransform *const restrict iTrans, const float z);


#endif