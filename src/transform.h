#ifndef transform_h
#define transform_h


#include "interp.h"


typedef struct transformState {
	interpVec3 pos;
	interpQuat rot;
	interpVec3 scale;
} transformState;


void transformStateInit(transformState *transform);

void transformStateAddPos(transformState *transform, const float x, const float y, const float z, const float time);
void transformStateAddPosX(transformState *transform, const float x, const float time);
void transformStateAddPosY(transformState *transform, const float y, const float time);
void transformStateAddPosZ(transformState *transform, const float z, const float time);
void transformStateSetPos(transformState *transform, const float x, const float y, const float z);
void transformStateSetPosX(transformState *transform, const float x);
void transformStateSetPosY(transformState *transform, const float y);
void transformStateSetPosZ(transformState *transform, const float z);
void transformStateSetNextPos(transformState *transform, const float x, const float y, const float z);
void transformStateSetNextPosX(transformState *transform, const float x);
void transformStateSetNextPosY(transformState *transform, const float y);
void transformStateSetNextPosZ(transformState *transform, const float z);

void transformStateAddRotEulerRad(transformState *transform, const float x, const float y, const float z, const float time);
void transformStateAddRotEulerDeg(transformState *transform, const float x, const float y, const float z, const float time);
void transformStateAddRotQuat(transformState *transform, const quat *q, const float time);
void transformStateSetRotEulerRad(transformState *transform, const float x, const float y, const float z);
void transformStateSetRotEulerDeg(transformState *transform, const float x, const float y, const float z);
void transformStateSetRotQuat(transformState *transform, const quat *q);
void transformStateSetNextRotEulerRad(transformState *transform, const float x, const float y, const float z);
void transformStateSetNextRotEulerDeg(transformState *transform, const float x, const float y, const float z);
void transformStateSetNextRotQuat(transformState *transform, const quat *q);

void transformStateAddScale(transformState *transform, const float x, const float y, const float z, const float time);
void transformStateAddScaleX(transformState *transform, const float x, const float time);
void transformStateAddScaleY(transformState *transform, const float y, const float time);
void transformStateAddScaleZ(transformState *transform, const float z, const float time);
void transformStateSetScale(transformState *transform, const float x, const float y, const float z);
void transformStateSetScaleX(transformState *transform, const float x);
void transformStateSetScaleY(transformState *transform, const float y);
void transformStateSetScaleZ(transformState *transform, const float z);
void transformStateSetNextScale(transformState *transform, const float x, const float y, const float z);
void transformStateSetNextScaleX(transformState *transform, const float x);
void transformStateSetNextScaleY(transformState *transform, const float y);
void transformStateSetNextScaleZ(transformState *transform, const float z);

void transformStateGenRenderState(const transformState *transform, const float time, vec3 *pos, quat *rot, vec3 *scale);


#endif