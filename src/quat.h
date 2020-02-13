#ifndef quat_h
#define quat_h


#include "vec3.h"
#include "vec4.h"


typedef vec4 quat;


void quatInitZero(quat *const restrict q);
quat quatInitZeroR();
void quatInitIdentity(quat *const restrict q);
quat quatInitIdentityR();
void quatInitSet(quat *const restrict q, const float x, const float y, const float z, const float w);
quat quatInitSetR(const float x, const float y, const float z, const float w);
void quatInitEulerRad(quat *const restrict q, const float x, const float y, const float z);
quat quatInitEulerRadR(const float x, const float y, const float z);
void quatInitEulerDeg(quat *const restrict q, const float x, const float y, const float z);
quat quatInitEulerDegR(const float x, const float y, const float z);
void quatInitEulerVec3Rad(quat *const restrict q, const vec3 *const restrict v);
quat quatInitEulerVec3RadR(const vec3 v);
void quatInitEulerVec3Deg(quat *const restrict q, const vec3 *const restrict v);
quat quatInitEulerVec3DegR(const vec3 v);

void quatAdd(quat *const restrict q, const float x, const float y, const float z, const float w);
void quatAddOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatAddR(quat q, const float x, const float y, const float z, const float w);
void quatAddS(quat *const restrict q, const float x);
void quatAddSOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatAddSR(quat q, const float x);
void quatAddQuat(quat *const restrict q, const vec4 *const restrict v);
void quatAddQuatOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatAddQuatR(quat q, const vec4 v);
void quatSubtract(quat *const restrict q, const float x, const float y, const float z, const float w);
void quatSubtractOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatSubtractR(quat q, const float x, const float y, const float z, const float w);
void quatSubtractFrom(quat *const restrict q, const float x, const float y, const float z, const float w);
void quatSubtractFromOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatSubtractFromR(quat q, const float x, const float y, const float z, const float w);
void quatSubtractSFrom(quat *const restrict q, const float x);
void quatSubtractSFromOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatSubtractSFromR(quat q, const float x);
void quatSubtractFromS(quat *const restrict q, const float x);
void quatSubtractFromSOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatSubtractFromSR(quat q, const float x);
void quatSubtractQuatFrom(quat *const restrict q, const vec4 *const restrict v);
void quatSubtractFromQuat(quat *const restrict q, const vec4 *const restrict v);
void quatSubtractQuatFromOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatSubtractQuatFromR(quat q, const vec4 v);

void quatMultiplyS(quat *const restrict q, const float x);
void quatMultiplySOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatMultiplySR(quat q, const float x);
void quatMultiplyVec4(quat *const restrict q, const vec4 *const restrict v);
void quatMultiplyVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatMultiplyVec4R(quat q, const vec4 v);
void quatDivideByS(quat *const restrict q, const float x);
void quatDivideBySOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatDivideBySR(quat q, const float x);
void quatDivideSBy(quat *const restrict q, const float x);
void quatDivideSByOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatDivideSByR(quat q, const float x);
void quatDivideSByFast(quat *const restrict q, const float x);
void quatDivideSByFastOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatDivideSByFastR(quat q, const float x);
void quatDivideByVec4(quat *const restrict q, const vec4 *const restrict v);
void quatDivideByVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideByVec4R(quat q, const vec4 v);
void quatDivideVec4By(quat *const restrict q, const vec4 *const restrict v);
void quatDivideVec4ByOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideVec4ByR(quat q, const vec4 v);
void quatDivideByVec4Fast(quat *const restrict q, const vec4 *const restrict v);
void quatDivideByVec4FastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideByVec4FastR(quat q, const vec4 v);
void quatDivideVec4ByFast(quat *const restrict q, const vec4 *const restrict v);
void quatDivideVec4ByFastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideVec4ByFastR(quat q, const vec4 v);

void quatMultiplyByQuat(quat *const restrict q1, const quat *const restrict q2);
void quatMultiplyQuatBy(quat *const restrict q1, const quat *const restrict q2);
void quatMultiplyByQuatOut(const quat q1, const quat q2, quat *const restrict out);
void quatMultiplyByQuatFastOut(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict out);
quat quatMultiplyByQuatR(const quat q1, const quat q2);

void quatRotateVec3(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3R(const quat q, vec3 v);
void quatRotateVec3Inverse(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3InverseR(const quat q, vec3 v);
void quatRotateVec3Fast(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3FastR(const quat q, vec3 v);
void quatRotateVec3InverseFast(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3InverseFastR(const quat q, vec3 v);

float quatMagnitude(const float x, const float y, const float z, const float w);
float quatMagnitudeQuat(const quat *const restrict q);
float quatMagnitudeQuatR(const quat q);

float quatDot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float quatDotQuatFloat(const quat *const restrict q, const float x, const float y, const float z, const float w);
float quatDotQuatFloatR(const quat q, const float x, const float y, const float z, const float w);
float quatDotQuat(const quat *const restrict q1, const quat *const restrict q2);
float quatDotQuatR(const quat q1, const quat q2);
float quatNorm(const float x, const float y, const float z, const float w);
float quatNormQuat(const quat *const restrict q);
float quatNormQuatR(const quat q);

void quatNormalize(const float x, const float y, const float z, const float w, quat *const restrict out);
void quatNormalizeFast(const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatNormalizeR(const float x, const float y, const float z, const float w);
quat quatNormalizeFastR(const float x, const float y, const float z, const float w);
void quatNormalizeQuat(quat *const restrict q);
void quatNormalizeQuatFast(quat *const restrict q);
void quatNormalizeQuatOut(const quat *const restrict q, quat *const restrict out);
void quatNormalizeQuatFastOut(const quat *const restrict q, quat *const restrict out);
quat quatNormalizeQuatR(quat q);
quat quatNormalizeQuatFastR(quat q);
void quatConjugate(quat *const restrict q);
void quatConjugateOut(const quat *const restrict q, quat *const restrict out);
quat quatConjugateR(quat q);
void quatConjugateFast(quat *const restrict q);
void quatConjugateFastOut(const quat *const restrict q, quat *const restrict out);
quat quatConjugateFastR(quat q);
void quatNegate(quat *const restrict q);
void quatNegateOut(const quat *const restrict q, quat *const restrict out);
quat quatNegateR(quat q);

void quatToAxisAngle(const quat *const restrict q, vec4 *const restrict out);
void quatToAxisAngleFast(const quat *const restrict q, vec4 *const restrict out);
vec4 quatToAxisAngleR(const quat q);
vec4 quatToAxisAngleFastR(const quat q);

void quatRotateByRad(quat *const restrict q, const float x, const float y, const float z);
quat quatRotateByRadR(const quat q, const float x, const float y, const float z);
void quatRotateByDeg(quat *const restrict q, const float x, const float y, const float z);
quat quatRotateByDegR(const quat q, const float x, const float y, const float z);
void quatRotateByVec3Rad(quat *const restrict q, const vec3 *const restrict v);
quat quatRotateByVec3RadR(const quat q, const vec3 v);
void quatRotateByVec3Deg(quat *const restrict q, const vec3 *const restrict v);
quat quatRotateByVec3DegR(const quat q, const vec3 v);

void quatLerp(quat *const restrict q1, const quat *const restrict q2, const float time);
void quatLerpOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out);
quat quatLerpR(quat q1, const quat q2, const float time);
void quatLerpFast(quat *const restrict q, const quat *const restrict offset, const float time);
void quatLerpFastOut(const quat *const restrict q, const quat *const restrict offset, const float time, quat *const restrict out);
quat quatLerpFastR(quat q, const quat offset, const float time);
void quatSlerp(quat *const restrict q1, quat q2, const float time);
void quatSlerpOut(const quat *const restrict q1, quat q2, const float time, quat *const restrict out);
quat quatSlerpR(quat q1, quat q2, const float time);
void quatSlerpFast(quat *const restrict q1, quat q2, const float time);
void quatSlerpFastOut(const quat *const restrict q1, quat q2, const float time, quat *const restrict out);
quat quatSlerpFastR(quat q1, quat q2, const float time);
void quatSlerpFaster(quat *const restrict q1, const quat *const restrict q2, const float time);
void quatSlerpFasterOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out);
quat quatSlerpFasterR(quat q1, const quat q2, const float time);

void quatScale(quat *const restrict q, const float x);
void quatScaleOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatScaleR(quat q, const float x);
void quatScaleFast(quat *const restrict q, const float x);
void quatScaleFastOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatScaleFastR(quat q, const float x);
void quatScaleFaster(quat *const restrict q, const float time);
void quatScaleFasterOut(const quat *const restrict q, const float time, quat *const restrict out);
quat quatScaleFasterR(quat q, const float time);

void quatDifferentiate(quat *const restrict q, const vec3 *const restrict w);
void quatDifferentiateOut(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict out);
quat quatDifferentiateR(const quat q, const vec3 w);
void quatIntegrate(quat *const restrict q, const vec3 *const restrict w, float dt);
void quatIntegrateOut(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict out);
quat quatIntegrateR(const quat q, const vec3 w, float dt);


extern quat g_quatIdentity;


#endif