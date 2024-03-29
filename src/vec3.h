#ifndef vec3_h
#define vec3_h


#include "utilTypes.h"


typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;


void vec3InitZero(vec3 *const restrict v);
#define vec3InitZeroP(v) (vec3InitZero(v), v)
vec3 vec3InitZeroC();
void vec3InitSet(vec3 *const restrict v, const float x, const float y, const float z);
#define vec3InitSetP(v, x, y, z) (vec3InitSet(v, x, y, z), v)
vec3 vec3InitSetC(const float x, const float y, const float z);

return_t vec3IsZero(const float x, const float y, const float z);
return_t vec3IsZeroVec3(const vec3 *const restrict v);
return_t vec3IsZeroVec3C(const vec3 v);

void vec3Add(vec3 *const restrict v, const float x, const float y, const float z);
void vec3AddOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out);
#define vec3AddP(v, x, y, z, out) (vec3AddOut(v, x, y, z, out), out)
vec3 vec3AddC(vec3 v, const float x, const float y, const float z);
void vec3AddS(vec3 *const restrict v, const float x);
void vec3AddSOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3AddSP(v, x, out) (vec3AddSOut(v, x, out), out)
vec3 vec3AddSC(vec3 v, const float x);
void vec3AddVec3(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3AddVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3AddVec3P(v1, v2, out) (vec3AddVec3Out(v, v2, out), out)
vec3 vec3AddVec3C(vec3 v1, const vec3 v2);
void vec3Subtract(vec3 *const restrict v, const float x, const float y, const float z);
void vec3SubtractOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out);
#define vec3SubtractP(v, x, y, z, out) (vec3SubtractOut(v, x, y, z, out), out)
vec3 vec3SubtractC(vec3 v, const float x, const float y, const float z);
void vec3SubtractFrom(vec3 *const restrict v, const float x, const float y, const float z);
void vec3SubtractFromOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out);
#define vec3SubtractFromP(v, x, y, z, out) (vec3SubtractFromOut(v, x, y, z, out), out)
vec3 vec3SubtractFromC(vec3 v, const float x, const float y, const float z);
void vec3SubtractSFrom(vec3 *const restrict v, const float x);
void vec3SubtractSFromOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3SubtractSFromP (vec3SubtractSFromOut(v, x, out), out)
vec3 vec3SubtractSFromC(vec3 v, const float x);
void vec3SubtractFromS(vec3 *const restrict v, const float x);
void vec3SubtractFromSOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3SubtractFromSP (vec3SubtractFromSOut(v, x, out), out)
vec3 vec3SubtractFromSC(vec3 v, const float x);
void vec3SubtractVec3P1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3SubtractVec3P2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3SubtractVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3SubtractVec3P(v1, v2, out) (vec3SubtractVec3Out(v1, v2, out), out)
vec3 vec3SubtractVec3C(vec3 v1, const vec3 v2);

void vec3MultiplyS(vec3 *const restrict v, const float x);
void vec3MultiplySOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3MultiplySP(v, x, out), (vec3MultiplySOut(v, x, out), out)
vec3 vec3MultiplySC(vec3 v, const float x);
void vec3MultiplyVec3(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3MultiplyVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3MultiplyVec3P(v1, v2, out), (vec3MultiplyVec3Out(v1, v2, out), out)
vec3 vec3MultiplyVec3C(vec3 v1, const vec3 v2);
void vec3DivideByS(vec3 *const restrict v, const float x);
void vec3DivideBySOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3DivideBySP(v, x, out) (vec3DivideBySOut(v, x, out), out)
vec3 vec3DivideBySC(vec3 v, const float x);
void vec3DivideSBy(vec3 *const restrict v, const float x);
void vec3DivideSByOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3DivideSByP(v, x, out) (vec3DivideSByOut(v, x, out), out)
vec3 vec3DivideSByC(vec3 v, const float x);
void vec3DivideSByFast(vec3 *const restrict v, const float x);
void vec3DivideSByFastOut(const vec3 *const restrict v, const float x, vec3 *const restrict out);
#define vec3DivideSByFastP(v, x, out) (vec3DivideSByFastOut(v, x, out), out)
vec3 vec3DivideSByFastC(vec3 v, const float x);
void vec3DivideVec3P1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3DivideVec3P2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3DivideVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3DivideVec3P(v1, v2, out) (vec3DivideVec3Out(v1, v2, out), out)
vec3 vec3DivideVec3C(vec3 v1, const vec3 v2);
void vec3DivideVec3FastP1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3DivideVec3FastP2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3DivideVec3FastOut(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3DivideVec3FastP(v1, v2, out) (vec3DivideVec3FastOut(v1, v2, out), out)
vec3 vec3DivideVec3FastC(vec3 v1, const vec3 v2);

void vec3FmaP1(const float x, vec3 *const restrict u, const vec3 *const restrict v);
void vec3FmaP2(const float x, const vec3 *const restrict u, vec3 *const restrict v);
void vec3FmaOut(const float x, const vec3 *const restrict u, const vec3 *const restrict v, vec3 *const restrict out);
#define vec3FmaP(x, u, v, out) (vec3FmaOut(x, u, v, out), out)
vec3 vec3FmaC(const float x, const vec3 u, const vec3 v);

float vec3Magnitude(const float x, const float y, const float z);
float vec3MagnitudeVec3(const vec3 *const restrict v);
float vec3MagnitudeVec3C(const vec3 v);
float vec3MagnitudeSquared(const float x, const float y, const float z);
float vec3MagnitudeSquaredVec3(const vec3 *const restrict v);
float vec3MagnitudeSquaredVec3C(const vec3 v);
float vec3Distance(const vec3 *const restrict v, const float x, const float y, const float z);
float vec3DistanceC(const vec3 v, const float x, const float y, const float z);
float vec3DistanceSquared(const vec3 *const restrict v, const float x, const float y, const float z);
float vec3DistanceSquaredC(const vec3 v, const float x, const float y, const float z);
float vec3DistanceVec3(const vec3 *const restrict v1, const vec3 *const restrict v2);
float vec3DistanceVec3C(const vec3 v1, const vec3 v2);
float vec3DistanceSquaredVec3(const vec3 *const restrict v1, const vec3 *const restrict v2);
float vec3DistanceSquaredVec3C(const vec3 v1, const vec3 v2);

float vec3Dot(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2);
float vec3DotVec3Float(const vec3 *const restrict v, const float x, const float y, const float z);
float vec3DotVec3FloatC(const vec3 v, const float x, const float y, const float z);
float vec3DotVec3(const vec3 *const restrict v1, const vec3 *const restrict v2);
float vec3DotVec3C(const vec3 v1, const vec3 v2);

void vec3Cross(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, vec3 *const restrict out);
#define vec3CrossP(x1, y1, z1, x2, y2, z2, out) (vec3Cross(x1, y1, z1, x2, y2, z2, out), out)
vec3 vec3CrossC(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2);
void vec3CrossVec3Float(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out);
#define vec3CrossVec3FloatP(v, x, y, z, out) (vec3CrossVec3Float(v, x, y, z, out), out)
vec3 vec3CrossVec3FloatC(const vec3 v, const float x, const float y, const float z);
void vec3CrossFloatVec3(const float x, const float y, const float z, const vec3 *const restrict v, vec3 *const restrict out);
#define vec3CrossFloatVec3P(x, y, z, v, out) (vec3CrossFloatVec3(x, y, z, v, out), out)
vec3 vec3CrossFloatVec3C(const float x, const float y, const float z, const vec3 v);
void vec3CrossVec3P1(vec3 *const restrict v1, const vec3 *const restrict v2);
void vec3CrossVec3P2(const vec3 *const restrict v1, vec3 *const restrict v2);
void vec3CrossVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3CrossVec3P(v1, v2, out) (vec3CrossVec3Out(v1, v2, out), out)
vec3 vec3CrossVec3C(const vec3 v1, const vec3 v2);

void vec3Normalize(const float x, const float y, const float z, vec3 *const restrict out);
#define vec3NormalizeP(x, y, z, out) (vec3Normalize(x, y, z, out), out)
vec3 vec3NormalizeC(const float x, const float y, const float z);
void vec3NormalizeFast(const float x, const float y, const float z, vec3 *const restrict out);
#define vec3NormalizeFastP(x, y, z, out) (vec3NormalizeFast(x, y, z, out), out)
vec3 vec3NormalizeFastC(const float x, const float y, const float z);
void vec3NormalizeVec3(vec3 *const restrict v);
void vec3NormalizeVec3Out(const vec3 *const restrict v, vec3 *const restrict out);
#define vec3NormalizeVec3P(v, out) (vec3NormalizeVec3Out(v, out), out)
vec3 vec3NormalizeVec3C(vec3 v);
void vec3NormalizeVec3Fast(vec3 *const restrict v);
void vec3NormalizeVec3FastOut(const vec3 *const restrict v, vec3 *const restrict out);
#define vec3NormalizeVec3FastP(v, out) (vec3NormalizeVec3FastOut(v, out), out)
vec3 vec3NormalizeVec3FastC(vec3 v);
return_t vec3CanNormalize(const float x, const float y, const float z, vec3 *const restrict out);
return_t vec3CanNormalizeVec3(vec3 *const restrict v);
return_t vec3CanNormalizeVec3Out(const vec3 *const restrict v, vec3 *const restrict out);
return_t vec3CanNormalizeFast(const float x, const float y, const float z, vec3 *const restrict out);
return_t vec3CanNormalizeVec3Fast(vec3 *const restrict v);
return_t vec3CanNormalizeVec3FastOut(const vec3 *const restrict v, vec3 *const restrict out);

void vec3Negate(vec3 *const restrict v);
void vec3NegateOut(const vec3 *const restrict v, vec3 *const restrict out);
#define vec3NegateP(v, out) (vec3NegateOut(v, out), out)
vec3 vec3NegateC(vec3 v);

void vec3Orthogonal(const vec3 *const restrict v, vec3 *const restrict out);
#define vec3OrthogonalP(v, out) (vec3Orthogonal(v, out), out)
vec3 vec3OrthogonalC(const vec3 v);
void vec3Orthonormal(const vec3 *const restrict v, vec3 *const restrict out);
#define vec3OrthonormalP(v, out) (vec3Orthonormal(v, out), out)
vec3 vec3OrthonormalC(const vec3 v);
void vec3OrthonormalFast(const vec3 *const restrict v, vec3 *const restrict out);
#define vec3OrthonormalFastP(v, out) (vec3OrthonormalFast(v, out), out)
vec3 vec3OrthonormalFastC(const vec3 v);

void vec3DegToRad(vec3 *const restrict v);
void vec3RadToDeg(vec3 *const restrict v);

void vec3RotateByEulerXYZ(vec3 *const restrict v, const float x, const float y, const float z);
void vec3RotateByEulerZXY(vec3 *const restrict v, const float x, const float y, const float z);
vec3 vec3RotateByEulerXYZC(const vec3 v, const float x, const float y, const float z);
vec3 vec3RotateByEulerZXYC(const vec3 v, const float x, const float y, const float z);
void vec3RotateByEulerXYZOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out);
#define vec3RotateByEulerXYZP(v, x, y, z, out) (vec3RotateByEulerXYZOut(v, x, y, z, out), out)
void vec3RotateByEulerZXYOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out);
#define vec3RotateByEulerZXYP(v, x, y, z, out) (vec3RotateByEulerZXYOut(v, x, y, z, out), out)

void vec3Lerp(const vec3 *const restrict v1, const vec3 *const restrict v2, const float time, vec3 *const restrict out);
#define vec3LerpP(v1, v2, time, out) (vec3Lerp(v1, v2, time, out), out)
vec3 vec3LerpC(vec3 v1, const vec3 v2, const float time);
void vec3LerpDiff(const vec3 *const restrict v, const vec3 *offset, const float time, vec3 *const restrict out);
#define vec3LerpDiffP(v1, v2, time, out) (vec3LerpDiff(v1, v2, time, out), out)
vec3 vec3LerpDiffC(vec3 v, const vec3 offset, const float time);

void vec3Min(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3MinP(v1, v2, out) (vec3Min(v1, v2, out), out)
vec3 vec3MinC(vec3 v1, const vec3 v2);
void vec3Max(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out);
#define vec3MaxP(v1, v2, out) (vec3Max(v1, v2, out), out)
vec3 vec3MaxC(vec3 v1, const vec3 v2);


#endif