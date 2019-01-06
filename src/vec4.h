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

void vec4Add(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out);
void vec4AddS(const vec4 *v, const float x, vec4 *out);
void vec4AddVec4(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4SubtractFrom(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out);
void vec4SubtractSFrom(const vec4 *v, const float x, vec4 *out);
void vec4SubtractFromS(const vec4 *v, const float x, vec4 *out);
void vec4SubtractVec4From(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4MultiplyS(const vec4 *v, const float x, vec4 *out);
void vec4MultiplyVec4(const vec4 *v1, const vec4 *v2, vec4 *out);
void vec4DivideByS(const vec4 *v, const float x, vec4 *out);
void vec4DivideSBy(const vec4 *v, const float x, vec4 *out);
void vec4DivideByVec4(const vec4 *v1, const vec4 *v2, vec4 *out);

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
void vec4NormalizeVec4(const vec4 *v, vec4 *out);
void vec4Negate(const vec4 *v, vec4 *out);

void vec4DegToRad(vec4 *v);
void vec4RadToDeg(vec4 *v);

void vec4Lerp(const vec4 *v1, const vec4 *v2, const float time, vec4 *out);
void vec4LerpFast(const vec4 *v, const vec4 *offset, const float time, vec4 *out);


#endif