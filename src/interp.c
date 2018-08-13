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


//Using the interpolate between the interpFloat's two values!
void interpFloatFindRenderState(const interpFloat *iFloat, const float time, float *out){
	*out = iFloat->last + (iFloat->next - iFloat->last) * time;
}

//Using the interpolate between the interpVec3's two values!
void interpVec3FindRenderState(const interpVec3 *iVec3, const float time, vec3 *out){
	vec3Lerp(&iVec3->last, &iVec3->next, time, out);
}

//Using the interpolate between the interpQuat's two values!
void interpQuatFindRenderState(const interpQuat *iQuat, const float time, quat *out){
	quatSlerpFast(&iQuat->last, &iQuat->next, time, out);
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