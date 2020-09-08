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
vec4 vec4InitZeroR();
void vec4InitVec3(vec4 *const restrict v1, const vec3 *const restrict v2, const float w);
vec4 vec4InitVec3R(const vec3 v, const float w);
void vec4InitSet(vec4 *const restrict v, const float x, const float y, const float z, const float w);
vec4 vec4InitSetR(const float x, const float y, const float z, const float w);

void vec4Add(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4AddOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4AddR(vec4 v, const float x, const float y, const float z, const float w);
void vec4AddS(vec4 *const restrict v, const float x);
void vec4AddSOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4AddSR(vec4 v, const float x);
void vec4AddVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4AddVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4AddVec4R(vec4 v1, const vec4 v2);
void vec4Subtract(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4SubtractOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4SubtractR(vec4 v, const float x, const float y, const float z, const float w);
void vec4SubtractFrom(vec4 *const restrict v, const float x, const float y, const float z, const float w);
void vec4SubtractFromOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4SubtractFromR(vec4 v, const float x, const float y, const float z, const float w);
void vec4SubtractSFrom(vec4 *const restrict v, const float x);
void vec4SubtractSFromOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4SubtractSFromR(vec4 v, const float x);
void vec4SubtractFromS(vec4 *const restrict v, const float x);
void vec4SubtractFromSOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4SubtractFromSR(vec4 v, const float x);
void vec4SubtractVec4From(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4SubtractFromVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4SubtractVec4FromOut(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4SubtractVec4FromR(vec4 v1, const vec4 v2);

void vec4MultiplyS(vec4 *const restrict v, const float x);
void vec4MultiplySOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4MultiplySR(vec4 v, const float x);
void vec4MultiplyVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4MultiplyVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4MultiplyVec4R(vec4 v1, const vec4 v2);
void vec4DivideByS(vec4 *const restrict v, const float x);
void vec4DivideBySOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4DivideBySR(vec4 v, const float x);
void vec4DivideSBy(vec4 *const restrict v, const float x);
void vec4DivideSByOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4DivideSByR(vec4 v, const float x);
void vec4DivideSByFast(vec4 *const restrict v, const float x);
void vec4DivideSByFastOut(const vec4 *const restrict v, const float x, vec4 *const restrict out);
vec4 vec4DivideSByFastR(vec4 v, const float x);
void vec4DivideByVec4(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideVec4By(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideByVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4DivideByVec4R(vec4 v1, const vec4 v2);
void vec4DivideByVec4Fast(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideVec4ByFast(vec4 *const restrict v1, const vec4 *const restrict v2);
void vec4DivideByVec4FastOut(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4DivideByVec4FastR(vec4 v1, const vec4 v2);

float vec4Magnitude(const float x, const float y, const float z, const float w);
float vec4MagnitudeVec4(const vec4 *const restrict v);
float vec4MagnitudeVec4R(const vec4 v);
float vec4DistanceSquared(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
float vec4DistanceSquaredR(const vec4 v, const float x, const float y, const float z, const float w);
float vec4DistanceSquaredVec4(const vec4 *const restrict v1, const vec4 *const restrict v2);
float vec4DistanceSquaredVec4R(const vec4 v1, const vec4 v2);

float vec4Dot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float vec4DotVec4Float(const vec4 *const restrict v, const float x, const float y, const float z, const float w);
float vec4DotVec4FloatR(const vec4 v, const float x, const float y, const float z, const float w);
float vec4DotVec4(const vec4 *const restrict v1, const vec4 *const restrict v2);
float vec4DotVec4R(const vec4 v1, const vec4 v2);
float vec4Norm(const float x, const float y, const float z, const float w);
float vec4NormVec4(const vec4 *const restrict v);
float vec4NormVec4R(const vec4 v);

void vec4Normalize(const float x, const float y, const float z, const float w, vec4 *const restrict out);
void vec4NormalizeFast(const float x, const float y, const float z, const float w, vec4 *const restrict out);
vec4 vec4NormalizeR(const float x, const float y, const float z, const float w);
vec4 vec4NormalizeFastR(const float x, const float y, const float z, const float w);
void vec4NormalizeVec4(vec4 *const restrict v);
void vec4NormalizeVec4Fast(vec4 *const restrict v);
void vec4NormalizeVec4Out(const vec4 *const restrict v, vec4 *const restrict out);
void vec4NormalizeVec4FastOut(const vec4 *const restrict v, vec4 *const restrict out);
vec4 vec4NormalizeVec4R(vec4 v);
vec4 vec4NormalizeVec4FastR(vec4 v);
void vec4Negate(vec4 *const restrict v);
void vec4NegateOut(const vec4 *const restrict v, vec4 *const restrict out);
vec4 vec4NegateR(vec4 v);

void vec4DegToRad(vec4 *const restrict v);
void vec4RadToDeg(vec4 *const restrict v);

void vec4Lerp(const vec4 *const restrict v1, const vec4 *const restrict v2, const float time, vec4 *const restrict out);
vec4 vec4LerpR(vec4 v1, const vec4 v2, const float time);
void vec4LerpFast(const vec4 *const restrict v, const vec4 *offset, const float time, vec4 *const restrict out);
vec4 vec4LerpFastR(vec4 v, const vec4 offset, const float time);

void vec4Min(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4MinR(vec4 v1, const vec4 v2);
void vec4Max(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *const restrict out);
vec4 vec4MaxR(vec4 v1, const vec4 v2);


#endif