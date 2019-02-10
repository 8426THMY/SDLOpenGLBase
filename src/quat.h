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

void quatAdd(quat *q, const float x, const float y, const float z, const float w);
void quatAddOut(const quat *q, const float x, const float y, const float z, const float w, quat *out);
void quatAddS(quat *q, const float x);
void quatAddSOut(const quat *q, const float x, quat *out);
void quatAddVec4(quat *q, const vec4 *v);
void quatAddVec4Out(const quat *q, const vec4 *v, quat *out);
void quatSubtract(quat *q, const float x, const float y, const float z, const float w);
void quatSubtractOut(const quat *q, const float x, const float y, const float z, const float w, quat *out);
void quatSubtractFrom(quat *q, const float x, const float y, const float z, const float w);
void quatSubtractFromOut(const quat *q, const float x, const float y, const float z, const float w, quat *out);
void quatSubtractSFrom(quat *q, const float x);
void quatSubtractSFromOut(const quat *q, const float x, quat *out);
void quatSubtractFromS(quat *q, const float x);
void quatSubtractFromSOut(const quat *q, const float x, quat *out);
void quatSubtractVec4From(quat *q, const vec4 *v);
void quatSubtractFromVec4(quat *q, const vec4 *v);
void quatSubtractVec4FromOut(const quat *q, const vec4 *v, quat *out);

void quatMultiplyS(quat *q, const float x);
void quatMultiplySOut(const quat *q, const float x, quat *out);
void quatMultiplyVec4(quat *q, const vec4 *v);
void quatMultiplyVec4Out(const quat *q, const vec4 *v, quat *out);
void quatDivideByS(quat *q, const float x);
void quatDivideBySOut(const quat *q, const float x, quat *out);
void quatDivideSBy(quat *q, const float x);
void quatDivideSByOut(const quat *q, const float x, quat *out);
void quatDivideSByFast(quat *q, const float x);
void quatDivideSByFastOut(const quat *q, const float x, quat *out);
void quatDivideByVec4(quat *q, const vec4 *v);
void quatDivideVec4By(quat *q, const vec4 *v);
void quatDivideByVec4Out(const quat *q, const vec4 *v, quat *out);
void quatDivideByVec4Fast(quat *q, const vec4 *v);
void quatDivideVec4ByFast(quat *q, const vec4 *v);
void quatDivideByVec4FastOut(const quat *q, const vec4 *v, quat *out);

void quatMultiplyByQuat(quat *q1, const quat *q2);
void quatMultiplyQuatBy(quat *q1, const quat *q2);
void quatMultiplyQuatOut(const quat *q1, const quat *q2, quat *out);

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
void quatNormalizeQuat(quat *q);
void quatNormalizeQuatOut(const quat *q, quat *out);
void quatConjugate(const quat *q, quat *out);
void quatConjugateFast(const quat *q, quat *out);

void quatToAxisAngle(const quat *q, vec4 *out);

void quatRotateRad(quat *q, const float x, const float y, const float z);
void quatRotateDeg(quat *q, const float x, const float y, const float z);
void quatRotateVec3Rad(quat *q, const vec3 *v);
void quatRotateVec3Deg(quat *q, const vec3 *v);

void quatLerp(const quat *q1, const quat *q2, const float time, quat *out);
void quatLerpFast(const quat *q, const quat *offset, const float time, quat *out);
void quatSlerp(const quat *q1, const quat *q2, const float time, quat *out);
void quatSlerpFast(const quat *q1, const quat *q2, const float time, quat *out);

void quatDifferentiate(quat *q, const vec3 *w);
void quatDifferentiateOut(const quat *q, const vec3 *w, quat *out);
void quatIntegrate(quat *q, const vec3 *w, const float dt);
void quatIntegrateOut(const quat *q, const vec3 *w, const float dt, quat *out);


#endif