#ifndef utilMath_h
#define utilMath_h


#include <math.h>

#include "vec3.h"


#define DEG_TO_RAD (M_PI / 180.f)
#define RAD_TO_DEG (180.f / M_PI)

#define lerpNum(x, y, t) (x + t * (y - x))
#define lerpNumFast(x, y, t) (x + t * y)

// Note: These are prone to double evaluation!
#define minNumFast(x, y) (((x) <= (y)) ? (x) : (y))
#define maxNumFast(x, y) (((x) >= (y)) ? (x) : (y))
#define clampNumFast(min, x, max) (((min) >= (x)) ? (min) : (((x) >= (max)) ? (max) : (x)))

// This should only be used on ancient hardware!
#ifdef MATH_USE_FAST_INV_SQRT
	#define invSqrt(x)     fastInvSqrtAccurate(x)
	#define invSqrtFast(x) fastInvSqrt(x)
#else
	#define invSqrt(x)     (1.f/sqrtf(x))
	#define invSqrtFast(x) (1.f/sqrtf(x))
#endif


float minNum(const float x, const float y);
float maxNum(const float x, const float y);
float clampNum(const float min, const float x, const float max);

float fastInvSqrt(const float x);
float fastInvSqrtAccurate(const float x);

void pointBarycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p, vec3 *out);

void normalBasis(const vec3 *a, vec3 *b, vec3 *c);
void normalBasisFast(const vec3 *a, vec3 *b, vec3 *c);
void segmentClosestPoints(const vec3 *s1, const vec3 *e1, const vec3 *s2, const vec3 *e2, vec3 *p1, vec3 *p2);

void triangleNormal(const vec3 *a, const vec3 *b, const vec3 *c, vec3 *out);

float pointPlaneDist(const vec3 *point, const vec3 *vertex, const vec3 *normal);
void pointPlaneProject(const vec3 *point, const vec3 *vertex, const vec3 *normal, vec3 *out);


#endif