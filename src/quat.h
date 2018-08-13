#ifndef quat_h
#define quat_h


#include "vec3.h"
#include "vec4.h"


typedef vec4 quat;


void quatInitZero(quat *q);
void quatInitIdentity(quat *q);
void quatInitSet(quat *q, const float x, const float y, const float z, const float w);
void quatInitEulerRad(quat *q, const float x, const float y, const float z);
void quatInitEulerDeg(quat *q, const float x, const float y, const float z);
void quatInitEulerVec3Rad(quat *q, const vec3 *v);
void quatInitEulerVec3Deg(quat *q, const vec3 *v);

void quatAdd(const quat *q, const float x, const float y, const float z, const float w, quat *out);
void quatAddS(const quat *q, const float x, quat *out);
void quatAddVec4(const quat *q, const vec4 *v, quat *out);
void quatSubtractFrom(const quat *q, const float x, const float y, const float z, const float w, quat *out);
void quatSubtractSFrom(const quat *q, const float x, quat *out);
void quatSubtractFromS(const quat *q, const float x, quat *out);
void quatSubtractVec4From(const quat *q, const vec4 *v, quat *out);
void quatMultiplyS(const quat *q, const float x, quat *out);
void quatMultiplyVec4(const quat *q, const vec4 *v, quat *out);
void quatDivideByS(const quat *q, const float x, quat *out);
void quatDivideSBy(const quat *q, const float x, quat *out);
void quatDivideByVec4(const quat *q, const vec4 *v, quat *out);

void quatMultiplyQuat(const quat *q1, const quat *q2, quat *out);
void quatMultiplyQuatR(const quat *q1, const quat *q2, quat *out);

void quatApplyRotation(const quat *q, const vec3 *v, vec3 *out);
void quatApplyRotationFast(const quat *q, const vec3 *v, vec3 *out);

float quatNorm(const float x, const float y, const float z, const float w);
float quatNormQuat(const quat *q);
float quatMagnitude(const float x, const float y, const float z, const float w);
float quatMagnitudeQuat(const quat *q);
float quatDot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float quatDotQuatFloat(const quat *q, const float x, const float y, const float z, const float w);
float quatDotQuat(const quat *q1, const quat *q2);
void quatNormalize(const float x, const float y, const float z, const float w, quat *out);
void quatNormalizeQuat(const quat *q, quat *out);
void quatConjugate(const quat *q, quat *out);
void quatConjugateFast(const quat *q, quat *out);

void quatToAxisAngle(const quat *q, vec4 *out);

void quatRotateRad(quat *q, const float x, const float y, const float z);
void quatRotateDeg(quat *q, const float x, const float y, const float z);
void quatRotateVec3Rad(quat *q, const vec3 *v);
void quatRotateVec3Deg(quat *q, const vec3 *v);

void quatLerp(const quat *q1, const quat *q2, const float time, quat *out);
void quatSlerp(const quat *q1, const quat *q2, const float time, quat *out);
void quatSlerpFast(const quat *q1, const quat *q2, const float time, quat *out);


#endif