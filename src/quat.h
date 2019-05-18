#ifndef quat_h
#define quat_h


#include "vec3.h"
#include "vec4.h"


typedef vec4 quat;


void quatInitZero(quat *q);
quat quatInitZeroR();
void quatInitIdentity(quat *q);
quat quatInitIdentityR();
void quatInitSet(quat *q, const float x, const float y, const float z, const float w);
quat quatInitSetR(const float x, const float y, const float z, const float w);
void quatInitEulerRad(quat *q, const float x, const float y, const float z);
quat quatInitEulerRadR(const float x, const float y, const float z);
void quatInitEulerDeg(quat *q, const float x, const float y, const float z);
quat quatInitEulerDegR(const float x, const float y, const float z);
void quatInitEulerVec3Rad(quat *q, const vec3 *v);
quat quatInitEulerVec3RadR(const vec3 v);
void quatInitEulerVec3Deg(quat *q, const vec3 *v);
quat quatInitEulerVec3DegR(const vec3 v);

void quatAdd(quat *q, const float x, const float y, const float z, const float w);
void quatAddOut(const quat *q, const float x, const float y, const float z, const float w, quat *out);
quat quatAddR(quat q, const float x, const float y, const float z, const float w);
void quatAddS(quat *q, const float x);
void quatAddSOut(const quat *q, const float x, quat *out);
quat quatAddSR(quat q, const float x);
void quatAddVec4(quat *q, const vec4 *v);
void quatAddVec4Out(const quat *q, const vec4 *v, quat *out);
quat quatAddVec4R(quat q, const vec4 v);
void quatSubtract(quat *q, const float x, const float y, const float z, const float w);
void quatSubtractOut(const quat *q, const float x, const float y, const float z, const float w, quat *out);
quat quatSubtractR(quat q, const float x, const float y, const float z, const float w);
void quatSubtractFrom(quat *q, const float x, const float y, const float z, const float w);
void quatSubtractFromOut(const quat *q, const float x, const float y, const float z, const float w, quat *out);
quat quatSubtractFromR(quat q, const float x, const float y, const float z, const float w);
void quatSubtractSFrom(quat *q, const float x);
void quatSubtractSFromOut(const quat *q, const float x, quat *out);
quat quatSubtractSFromR(quat q, const float x);
void quatSubtractFromS(quat *q, const float x);
void quatSubtractFromSOut(const quat *q, const float x, quat *out);
quat quatSubtractFromSR(quat q, const float x);
void quatSubtractVec4From(quat *q, const vec4 *v);
void quatSubtractFromVec4(quat *q, const vec4 *v);
void quatSubtractVec4FromOut(const quat *q, const vec4 *v, quat *out);
quat quatSubtractVec4FromR(quat q, const vec4 v);

void quatMultiplyS(quat *q, const float x);
void quatMultiplySOut(const quat *q, const float x, quat *out);
quat quatMultiplySR(quat q, const float x);
void quatMultiplyVec4(quat *q, const vec4 *v);
void quatMultiplyVec4Out(const quat *q, const vec4 *v, quat *out);
quat quatMultiplyVec4R(quat q, const vec4 v);
void quatDivideByS(quat *q, const float x);
void quatDivideBySOut(const quat *q, const float x, quat *out);
quat quatDivideBySR(quat q, const float x);
void quatDivideSBy(quat *q, const float x);
void quatDivideSByOut(const quat *q, const float x, quat *out);
quat quatDivideSByR(quat q, const float x);
void quatDivideSByFast(quat *q, const float x);
void quatDivideSByFastOut(const quat *q, const float x, quat *out);
quat quatDivideSByFastR(quat q, const float x);
void quatDivideByVec4(quat *q, const vec4 *v);
void quatDivideByVec4Out(const quat *q, const vec4 *v, quat *out);
quat quatDivideByVec4R(quat q, const vec4 v);
void quatDivideVec4By(quat *q, const vec4 *v);
void quatDivideVec4ByOut(const quat *q, const vec4 *v, quat *out);
quat quatDivideVec4ByR(quat q, const vec4 v);
void quatDivideByVec4Fast(quat *q, const vec4 *v);
void quatDivideByVec4FastOut(const quat *q, const vec4 *v, quat *out);
quat quatDivideByVec4FastR(quat q, const vec4 v);
void quatDivideVec4ByFast(quat *q, const vec4 *v);
void quatDivideVec4ByFastOut(const quat *q, const vec4 *v, quat *out);
quat quatDivideVec4ByFastR(quat q, const vec4 v);

void quatMultiplyByQuat(quat *q1, const quat *q2);
void quatMultiplyQuatBy(quat *q1, const quat *q2);
void quatMultiplyByQuatOut(const quat *q1, const quat *q2, quat *out);
quat quatMultiplyByQuatR(const quat q1, const quat q2);

void quatApplyRotation(const quat *q, const vec3 *v, vec3 *out);
quat quatApplyRotationR(quat q, const vec3 v);
void quatApplyInverseRotation(const quat *q, const vec3 *v, vec3 *out);
quat quatApplyInverseRotationR(quat q, const vec3 v);
void quatApplyRotationFast(const quat *q, const vec3 *v, vec3 *out);
quat quatApplyRotationFastR(quat q, const vec3 v);
void quatApplyInverseRotationFast(const quat *q, const vec3 *v, vec3 *out);
quat quatApplyInverseRotationFastR(quat q, const vec3 v);

float quatMagnitude(const float x, const float y, const float z, const float w);
float quatMagnitudeQuat(const quat *q);
float quatMagnitudeQuatR(const quat q);

float quatDot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float quatDotQuatFloat(const quat *q, const float x, const float y, const float z, const float w);
float quatDotQuatFloatR(const quat q, const float x, const float y, const float z, const float w);
float quatDotQuat(const quat *q1, const quat *q2);
float quatDotQuatR(const quat q1, const quat q2);
float quatNorm(const float x, const float y, const float z, const float w);
float quatNormQuat(const quat *q);
float quatNormQuatR(const quat q);

void quatNormalize(const float x, const float y, const float z, const float w, quat *out);
quat quatNormalizeR(const float x, const float y, const float z, const float w);
void quatNormalizeQuat(quat *q);
void quatNormalizeQuatOut(const quat *q, quat *out);
quat quatNormalizeQuatR(quat q);
void quatConjugate(quat *q);
void quatConjugateOut(const quat *q, quat *out);
quat quatConjugateR(quat q);
void quatConjugateFast(quat *q);
void quatConjugateFastOut(const quat *q, quat *out);
quat quatConjugateFastR(quat q);

void quatToAxisAngle(const quat *q, vec4 *out);
vec4 quatToAxisAngleR(const quat q);

void quatRotateRad(quat *q, const float x, const float y, const float z);
quat quatRotateRadR(const quat q, const float x, const float y, const float z);
void quatRotateDeg(quat *q, const float x, const float y, const float z);
quat quatRotateDegR(const quat q, const float x, const float y, const float z);
void quatRotateVec3Rad(quat *q, const vec3 *v);
quat quatRotateVec3RadR(const quat q, const vec3 v);
void quatRotateVec3Deg(quat *q, const vec3 *v);
quat quatRotateVec3DegR(const quat q, const vec3 v);

void quatLerp(const quat *q1, const quat *q2, const float time, quat *out);
quat quatLerpR(quat q1, const quat q2, const float time);
void quatLerpFast(const quat *q, const quat *offset, const float time, quat *out);
quat quatLerpFastR(quat q, const quat offset, const float time);
void quatSlerp(const quat *q1, const quat *q2, const float time, quat *out);
quat quatSlerpR(quat q1, const quat q2, const float time);
void quatSlerpFast(const quat *q1, const quat *q2, const float time, quat *out);
quat quatSlerpFastR(quat q1, const quat q2, const float time);

void quatDifferentiate(quat *q, const vec3 *w);
void quatDifferentiateOut(const quat *q, const vec3 *w, quat *out);
quat quatDifferentiateR(const quat q, const vec3 w);
void quatIntegrate(quat *q, const vec3 *w, float dt);
void quatIntegrateOut(const quat *q, const vec3 *w, float dt, quat *out);
quat quatIntegrateR(const quat q, const vec3 w, float dt);


#endif