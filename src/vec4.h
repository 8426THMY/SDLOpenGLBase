#ifndef vec4_h
#define vec4_h


#include "vec3.h"


typedef struct vec4 {
	float x;
	float y;
	float z;
	float w;
} vec4;


void vec4InitZero(vec4 *v);
void vec4InitVec3(vec4 *v1, const vec3 *v2, const float w);
void vec4InitSet(vec4 *v, const float x, const float y, const float z, const float w);

void vec4Add(vec4 *v, const float x, const float y, const float z, const float w);
void vec4AddOut(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out);
void vec4AddS(vec4 *v, const float x);
void vec4AddSOut(const vec4 *v, const float x, vec4 *out);
void vec4AddVec4(vec4 *v1, const vec4 *v2);
void vec4AddVec4Out(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4Subtract(vec4 *v, const float x, const float y, const float z, const float w);
void vec4SubtractOut(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out);
void vec4SubtractFrom(vec4 *v, const float x, const float y, const float z, const float w);
void vec4SubtractFromOut(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out);
void vec4SubtractSFrom(vec4 *v, const float x);
void vec4SubtractSFromOut(const vec4 *v, const float x, vec4 *out);
void vec4SubtractFromS(vec4 *v, const float x);
void vec4SubtractFromSOut(const vec4 *v, const float x, vec4 *out);
void vec4SubtractVec4From(vec4 *v1, const vec4 *v2);
void vec4SubtractFromVec4(vec4 *v1, const vec4 *v2);
void vec4SubtractVec4FromOut(const vec4 *v1, const vec4 *v2, vec4 *out);

void vec4MultiplyS(vec4 *v, const float x);
void vec4MultiplySOut(const vec4 *v, const float x, vec4 *out);
void vec4MultiplyVec4(vec4 *v1, const vec4 *v2);
void vec4MultiplyVec4Out(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4DivideByS(vec4 *v, const float x);
void vec4DivideBySOut(const vec4 *v, const float x, vec4 *out);
void vec4DivideSBy(vec4 *v, const float x);
void vec4DivideSByOut(const vec4 *v, const float x, vec4 *out);
void vec4DivideSByFast(vec4 *v, const float x);
void vec4DivideSByFastOut(const vec4 *v, const float x, vec4 *out);
void vec4DivideByVec4(vec4 *v1, const vec4 *v2);
void vec4DivideVec4By(vec4 *v1, const vec4 *v2);
void vec4DivideByVec4Out(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4DivideByVec4Fast(vec4 *v1, const vec4 *v2);
void vec4DivideVec4ByFast(vec4 *v1, const vec4 *v2);
void vec4DivideByVec4FastOut(const vec4 *v1, const vec4 *v2, vec4 *out);

float vec4Norm(const float x, const float y, const float z, const float w);
float vec4NormVec4(const vec4 *v);
float vec4Magnitude(const float x, const float y, const float z, const float w);
float vec4MagnitudeVec4(const vec4 *v);
float vec4DistanceSquared(const vec4 *v, const float x, const float y, const float z, const float w);
float vec4DistanceSquaredVec4(const vec4 *v1, const vec4 *v2);
float vec4Dot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2);
float vec4DotVec4Float(const vec4 *v, const float x, const float y, const float z, const float w);
float vec4DotVec4(const vec4 *v1, const vec4 *v2);
void vec4Normalize(const float x, const float y, const float z, const float w, vec4 *out);
void vec4NormalizeVec4(vec4 *v);
void vec4NormalizeVec4Out(const vec4 *v, vec4 *out);
void vec4Negate(vec4 *v);
void vec4NegateOut(const vec4 *v, vec4 *out);

void vec4DegToRad(vec4 *v);
void vec4RadToDeg(vec4 *v);

void vec4Lerp(const vec4 *v1, const vec4 *v2, const float time, vec4 *out);
void vec4LerpFast(const vec4 *v, const vec4 *offset, const float time, vec4 *out);

void vec4Min(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4Max(const vec4 *v1, const vec4 *v2, vec4 *out);


#endif