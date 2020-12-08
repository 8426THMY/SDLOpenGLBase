#ifndef vec4_h
#define vec4_h


#include "vec3.h"


typedef struct vec4 {
	float x;
	float y;
	float z;
	float w;
} vec4;


void vec4InitZero(vec4 *const restrict v);
vec4 vec4InitZeroC();
void vec4InitVec3(vec4 *const restrict v1, const vec3 *const restrict v2, const float w);
vec4 vec4InitVec3C(const vec3 v, const float w);
void vec4InitSet(vec4 *const restrict v, const float x, const float y, const float z, const float w);
vec4 vec4InitSetC(const float x, const float y, const float z, const float w);

void vec4Add(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4AddOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4AddC(vec4 v, const float x, const float y, const float z, const float w);
void vec4AddS(vec4 *const restrict v, const float x);
void vec4AddSOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4AddSC(vec4 v, const float x);
void vec4AddVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4AddVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4AddVec4C(vec4 v1, const vec4 v2);
void vec4Subtract(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4SubtractOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4SubtractC(vec4 v, const float x, const float y, const float z, const float w);
void vec4SubtractFrom(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4SubtractFromOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4SubtractFromC(vec4 v, const float x, const float y, const float z, const float w);
void vec4SubtractSFrom(vec4 *const restrict v, const float x);
void vec4SubtractSFromOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4SubtractSFromC(vec4 v, const float x);
void vec4SubtractFromS(vec4 *const restrict v, const float x);
void vec4SubtractFromSOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4SubtractFromSC(vec4 v, const float x);
void vec4SubtractVec4From(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4SubtractFromVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4SubtractVec4FromOut(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4SubtractVec4FromC(vec4 v1, const vec4 v2);

void vec4MultiplyS(vec4 *const restrict v, const float x);
void vec4MultiplySOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4MultiplySC(vec4 v, const float x);
void vec4MultiplyVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4MultiplyVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4MultiplyVec4C(vec4 v1, const vec4 v2);
void vec4DivideByS(vec4 *const restrict v, const float x);
void vec4DivideBySOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4DivideBySC(vec4 v, const float x);
void vec4DivideSBy(vec4 *const restrict v, const float x);
void vec4DivideSByOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4DivideSByC(vec4 v, const float x);
void vec4DivideSByFast(vec4 *const restrict v, const float x);
void vec4DivideSByFastOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4DivideSByFastC(vec4 v, const float x);
void vec4DivideByVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideVec4By(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideByVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4DivideByVec4C(vec4 v1, const vec4 v2);
void vec4DivideByVec4Fast(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideVec4ByFast(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideByVec4FastOut(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4DivideByVec4FastC(vec4 v1, const vec4 v2);

float vec4Magnitude(const float x, const float y, const float z, const float w);
float vec4MagnitudeVec4(const vec4 *const restrict v);
float vec4MagnitudeVec4C(const vec4 v);
float vec4DistanceSquared(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
float vec4DistanceSquaredC(const vec4 v, const float x, const float y, const float z, const float w);
float vec4DistanceSquaredVec4(const vec4 *const restrict v1, const vec4 *const restrict v2);
float vec4DistanceSquaredVec4C(const vec4 v1, const vec4 v2);

float vec4Dot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float vec4DotVec4Float(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
float vec4DotVec4FloatC(const vec4 v, const float x, const float y, const float z, const float w);
float vec4DotVec4(const vec4 *const restrict v1, const vec4 *const restrict v2);
float vec4DotVec4C(const vec4 v1, const vec4 v2);
float vec4Norm(const float x, const float y, const float z, const float w);
float vec4NormVec4(const vec4 *const restrict v);
float vec4NormVec4C(const vec4 v);

void vec4Normalize(const float x, const float y, const float z, const float w, vec4 *const restrict out);
void vec4NormalizeFast(const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4NormalizeC(const float x, const float y, const float z, const float w);
vec4 vec4NormalizeFastC(const float x, const float y, const float z, const float w);
void vec4NormalizeVec4(vec4 *const restrict v);
void vec4NormalizeVec4Fast(vec4 *const restrict v);
void vec4NormalizeVec4Out(const vec4 *const restrict v, vec4 *const restrict out);
void vec4NormalizeVec4FastOut(const vec4 *const restrict v, vec4 *const restrict out);
vec4 vec4NormalizeVec4C(vec4 v);
vec4 vec4NormalizeVec4FastC(vec4 v);
void vec4Negate(vec4 *const restrict v);
void vec4NegateOut(const vec4 *const restrict v, vec4 *const restrict out);
vec4 vec4NegateC(vec4 v);

void vec4DegToRad(vec4 *const restrict v);
void vec4RadToDeg(vec4 *const restrict v);

void vec4Lerp(const vec4 *const restrict v1, const vec4 *const restrict v2, const float time, vec4 *const restrict out);
vec4 vec4LerpC(vec4 v1, const vec4 v2, const float time);
void vec4LerpDiff(const vec4 *const restrict v, const vec4 *offset, const float time, vec4 *const restrict out);
vec4 vec4LerpDiffC(vec4 v, const vec4 offset, const float time);

void vec4Min(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4MinC(vec4 v1, const vec4 v2);
void vec4Max(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4MaxC(vec4 v1, const vec4 v2);


#endif