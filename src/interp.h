#ifndef interp_h
#define interp_h


#include "vec3.h"
#include "quat.h"


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


void interpFloatInitZero(interpFloat *iFloat);
void interpFloatInitSet(interpFloat *iFloat, const float x);
void interpVec3InitZero(interpVec3 *iVec3);
void interpVec3InitSet(interpVec3 *iVec3, const float x, const float y, const float z);
void interpQuatInitIdentity(interpQuat *iQuat);
void interpQuatInitSet(interpQuat *iQuat, const float x, const float y, const float z, const float w);

/*void interpFloatUpdateNextState(interpFloat *iFloat);
void interpVec3UpdateNextState(interpVec3 *iVec3);
void interpQuatUpdateNextState(interpQuat *iQuat);*/

void interpFloatFindRenderState(const interpFloat *iFloat, const float time, float *out);
void interpVec3FindRenderState(const interpVec3 *iVec3, const float time, vec3 *out);
void interpQuatFindRenderState(const interpQuat *iQuat, const float time, quat *out);

void interpFloatMoveToNextState(interpFloat *iFloat);
void interpVec3MoveToNextState(interpVec3 *iVec3);
void interpQuatMoveToNextState(interpQuat *iQuat);


#endif