#ifndef quat_h
#define quat_h


#include "vec3.h"
#include "vec4.h"


/*
** Note that the 'w' component is at the end of
** the data structure. This lets us more efficiently
** treat the imaginary component as a vector.
*/
typedef vec4 quat;


void quatInitZero(quat *const restrict q);
quat quatInitZeroC();
void quatInitIdentity(quat *const restrict q);
quat quatInitIdentityC();
void quatInitSet(quat *const restrict q, const float x, const float y, const float z, const float w);
quat quatInitSetC(const float x, const float y, const float z, const float w);
void quatInitVec3F(quat *const restrict q, const vec3 *const restrict v, const float w);
quat quatInitVec3FC(const vec3 v, const float w);
void quatInitAxisAngle(quat *const restrict q, const vec3 *const restrict v, const float t);
quat quatInitAxisAngleC(const vec3 v, const float t);
void quatInitEulerXYZ(quat *const restrict q, const float x, const float y, const float z);
void quatInitEulerZXY(quat *const restrict q, const float x, const float y, const float z);
quat quatInitEulerXYZC(const float x, const float y, const float z);
quat quatInitEulerZXYC(const float x, const float y, const float z);
void quatInitEulerVec3XYZ(quat *const restrict q, const vec3 *const restrict v);
void quatInitEulerVec3ZXY(quat *const restrict q, const vec3 *const restrict v);
quat quatInitEulerVec3XYZC(const vec3 v);
quat quatInitEulerVec3ZXYC(const vec3 v);

void quatAdd(quat *const restrict q, const float x, const float y, const float z, const float w);
void quatAddOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatAddC(quat q, const float x, const float y, const float z, const float w);
void quatAddS(quat *const restrict q, const float x);
void quatAddSOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatAddSC(quat q, const float x);
void quatAddVec4(quat *const restrict q, const vec4 *const restrict v);
void quatAddVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatAddVec4C(quat q, const vec4 v);
void quatSubtract(quat *const restrict q, const float x, const float y, const float z, const float w);
void quatSubtractOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatSubtractC(quat q, const float x, const float y, const float z, const float w);
void quatSubtractFrom(quat *const restrict q, const float x, const float y, const float z, const float w);
void quatSubtractFromOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatSubtractFromC(quat q, const float x, const float y, const float z, const float w);
void quatSubtractSFrom(quat *const restrict q, const float x);
void quatSubtractSFromOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatSubtractSFromC(quat q, const float x);
void quatSubtractFromS(quat *const restrict q, const float x);
void quatSubtractFromSOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatSubtractFromSC(quat q, const float x);
void quatSubtractVec4From(quat *const restrict q, const vec4 *const restrict v);
void quatSubtractFromVec4(quat *const restrict q, const vec4 *const restrict v);
void quatSubtractVec4FromOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatSubtractVec4FromC(quat q, const vec4 v);

void quatMultiplyS(quat *const restrict q, const float x);
void quatMultiplySOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatMultiplySC(quat q, const float x);
void quatMultiplyVec4(quat *const restrict q, const vec4 *const restrict v);
void quatMultiplyVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatMultiplyVec4C(quat q, const vec4 v);
void quatDivideByS(quat *const restrict q, const float x);
void quatDivideBySOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatDivideBySC(quat q, const float x);
void quatDivideSBy(quat *const restrict q, const float x);
void quatDivideSByOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatDivideSByC(quat q, const float x);
void quatDivideSByFast(quat *const restrict q, const float x);
void quatDivideSByFastOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatDivideSByFastC(quat q, const float x);
void quatDivideByVec4(quat *const restrict q, const vec4 *const restrict v);
void quatDivideByVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideByVec4C(quat q, const vec4 v);
void quatDivideVec4By(quat *const restrict q, const vec4 *const restrict v);
void quatDivideVec4ByOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideVec4ByC(quat q, const vec4 v);
void quatDivideByVec4Fast(quat *const restrict q, const vec4 *const restrict v);
void quatDivideByVec4FastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideByVec4FastC(quat q, const vec4 v);
void quatDivideVec4ByFast(quat *const restrict q, const vec4 *const restrict v);
void quatDivideVec4ByFastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out);
quat quatDivideVec4ByFastC(quat q, const vec4 v);

void quatMultiplyQuatBy(quat *const restrict q1, const quat q2);
void quatMultiplyByQuat(quat *const restrict q1, const quat q2);
void quatMultiplyQuatByOut(const quat q1, const quat q2, quat *const restrict out);
quat quatMultiplyQuatByC(const quat q1, const quat q2);
void quatMultiplyConjByQuat(quat *const restrict q1, const quat q2);
void quatMultiplyQuatByConj(quat *const restrict q1, const quat q2);
void quatMultiplyByQuatConj(quat *const restrict q1, const quat q2);
void quatMultiplyByQuatConjOut(const quat q1, const quat q2, quat *const restrict out);
quat quatMultiplyByQuatConjC(const quat q1, const quat q2);
void quatMultiplyQuatConjBy(quat *const restrict q1, const quat q2);
void quatMultiplyQuatConjByOut(const quat q1, const quat q2, quat *const restrict out);
quat quatMultiplyQuatConjByC(const quat q1, const quat q2);

void quatRotateVec3(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3Out(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3C(const quat q, vec3 v);
void quatRotateVec3Conj(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3ConjOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3ConjC(const quat q, vec3 v);
void quatRotateVec3Fast(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3FastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3FastC(const quat q, vec3 v);
void quatRotateVec3ConjFast(const quat *const restrict q, vec3 *const restrict v);
void quatRotateVec3ConjFastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out);
vec3 quatRotateVec3ConjFastC(const quat q, vec3 v);

void quatBasisX(const quat *const restrict q, vec3 *const restrict out);
vec3 quatBasisXC(const quat q);
void quatBasisY(const quat *const restrict q, vec3 *const restrict out);
vec3 quatBasisYC(const quat q);
void quatBasisZ(const quat *const restrict q, vec3 *const restrict out);
vec3 quatBasisZC(const quat q);

void quatShortestArc(const vec3 *const restrict v1, const vec3 *const restrict v2, quat *const restrict out);
quat quatShortestArcC(const vec3 v1, const vec3 v2);
void quatShortestArcAlt(const vec3 *const restrict v1, const vec3 *const restrict v2, quat *const restrict out);
quat quatShortestArcAltC(const vec3 v1, const vec3 v2);

float quatMagnitude(const float x, const float y, const float z, const float w);
float quatMagnitudeQuat(const quat *const restrict q);
float quatMagnitudeQuatC(const quat q);
float quatMagnitudeSquared(const float x, const float y, const float z, const float w);
float quatMagnitudeSquaredQuat(const quat *const restrict q);
float quatMagnitudeSquaredQuatC(const quat q);

float quatDot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float quatDotQuatFloat(const quat *const restrict q, const float x, const float y, const float z, const float w);
float quatDotQuatFloatC(const quat q, const float x, const float y, const float z, const float w);
float quatDotQuat(const quat *const restrict q1, const quat *const restrict q2);
float quatDotQuatC(const quat q1, const quat q2);

void quatNormalize(const float x, const float y, const float z, const float w, quat *const restrict out);
void quatNormalizeFast(const float x, const float y, const float z, const float w, quat *const restrict out);
quat quatNormalizeC(const float x, const float y, const float z, const float w);
quat quatNormalizeFastC(const float x, const float y, const float z, const float w);
void quatNormalizeQuat(quat *const restrict q);
void quatNormalizeQuatFast(quat *const restrict q);
void quatNormalizeQuatOut(const quat *const restrict q, quat *const restrict out);
void quatNormalizeQuatFastOut(const quat *const restrict q, quat *const restrict out);
quat quatNormalizeQuatC(quat q);
quat quatNormalizeQuatFastC(quat q);
void quatConjugate(quat *const restrict q);
void quatConjugateOut(const quat *const restrict q, quat *const restrict out);
quat quatConjugateC(quat q);
void quatConjugateFast(quat *const restrict q);
void quatConjugateFastOut(const quat *const restrict q, quat *const restrict out);
quat quatConjugateFastC(quat q);
void quatNegate(quat *const restrict q);
void quatNegateOut(const quat *const restrict q, quat *const restrict out);
quat quatNegateC(quat q);

void quatAxis(const quat *const restrict q, vec3 *const restrict out);
vec3 quatAxisC(const quat q);
void quatAxisFast(const quat *const restrict q, vec3 *const restrict out);
vec3 quatAxisFastC(const quat q);
float quatAngle(const quat *const restrict q);
float quatAngleC(const quat q);

float quatAngleAboutAxis(const float qa, const float w);
void quatFromTanVector(const vec3 *const restrict v, quat *const restrict out);
quat quatFromTanVectorC(const vec3 v);

void quatToEulerAnglesXYZ(const quat q, vec3 *const restrict out);
void quatToEulerAnglesZXY(const quat q, vec3 *const restrict out);
vec3 quatToEulerAnglesXYZC(const quat q);
vec3 quatToEulerAnglesZXYC(const quat q);
void quatToAxisAngle(const quat *const restrict q, vec4 *const restrict out);
vec4 quatToAxisAngleC(const quat q);
void quatToAxisAngleFast(const quat *const restrict q, vec4 *const restrict out);
vec4 quatToAxisAngleFastC(const quat q);

void quatRotateByEulerXYZ(quat *const restrict q, const float x, const float y, const float z);
void quatRotateByEulerZXY(quat *const restrict q, const float x, const float y, const float z);
quat quatRotateByEulerXYZC(const quat q, const float x, const float y, const float z);
quat quatRotateByEulerZXYC(const quat q, const float x, const float y, const float z);
void quatRotateByVec3EulerXYZ(quat *const restrict q, const vec3 *const restrict v);
void quatRotateByVec3EulerZXY(quat *const restrict q, const vec3 *const restrict v);
quat quatRotateByVec3EulerXYZC(const quat q, const vec3 v);
quat quatRotateByVec3EulerZXYC(const quat q, const vec3 v);

void quatLerp(quat *const restrict q1, const quat *const restrict q2, const float time);
void quatLerpOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out);
quat quatLerpC(quat q1, const quat q2, const float time);
void quatLerpDiff(quat *const restrict q, const quat *const restrict offset, const float time);
void quatLerpDiffOut(const quat *const restrict q, const quat *const restrict offset, const float time, quat *const restrict out);
quat quatLerpDiffC(quat q, const quat offset, const float time);
void quatSlerp(quat *const restrict q1, quat q2, const float time);
void quatSlerpOut(const quat *const restrict q1, quat q2, const float time, quat *const restrict out);
quat quatSlerpC(quat q1, quat q2, const float time);
void quatSlerpFast(quat *const restrict q1, quat q2, const float time);
void quatSlerpFastOut(const quat *const restrict q1, quat q2, const float time, quat *const restrict out);
quat quatSlerpFastC(quat q1, quat q2, const float time);
void quatSlerpFaster(quat *const restrict q1, const quat *const restrict q2, const float time);
void quatSlerpFasterOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out);
quat quatSlerpFasterC(quat q1, const quat q2, const float time);

void quatScale(quat *const restrict q, const float x);
void quatScaleOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatScaleC(quat q, const float x);
void quatScaleFast(quat *const restrict q, const float x);
void quatScaleFastOut(const quat *const restrict q, const float x, quat *const restrict out);
quat quatScaleFastC(quat q, const float x);
void quatScaleFaster(quat *const restrict q, const float time);
void quatScaleFasterOut(const quat *const restrict q, const float time, quat *const restrict out);
quat quatScaleFasterC(quat q, const float time);

void quatSwingTwist(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s);
void quatSwingTwistC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s);
void quatSwingTwistFast(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s);
void quatSwingTwistFastC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s);
void quatSwingTwistFaster(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s);
void quatSwingTwistFasterC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s);

void quatDifferentiate(quat *const restrict q, const vec3 *const restrict w);
void quatDifferentiateOut(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict out);
quat quatDifferentiateC(const quat q, const vec3 w);
void quatIntegrate(quat *const restrict q, const vec3 *const restrict w, float dt);
void quatIntegrateOut(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict out);
quat quatIntegrateC(const quat q, const vec3 w, float dt);


extern quat g_quatIdentity;


#endif