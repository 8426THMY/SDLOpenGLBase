#ifndef utilMath_h
#define utilMath_h


#include <math.h>

#include "vec3.h"


#define DEG_TO_RAD (M_PI / 180.f)
#define RAD_TO_DEG (180.f / M_PI)

// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#define minFloatFast(x, y) (((x) < (y)) ? (x) : (y))
#define maxFloatFast(x, y) (((x) > (y)) ? (x) : (y))
#define minUintFast(x, y) (((x) < (y)) ? (x) : (y))
#define maxUintFast(x, y) (((x) > (y)) ? (x) : (y))

// According to Mark Harris in his 2015 blog GPU Pro Tip: Lerp Faster in C++,
// we can achieve better performance and accuracy by using two fmas here.
//
// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#ifdef FP_FAST_FMAF
	#define lerpFloatFast(x, y, t) fmaf(t, y, fmaf(-t, x, x))
	#define lerpDiffFast(x, y, t) fmaf(t, y, x)
#else
	#define lerpFloatFast(x, y, t) ((x) + (t)*((y) - (x)))
	#define lerpDiffFast(x, y, t) ((x) + (t)*(y))
#endif

// This should only be used on ancient hardware!
#ifdef MATH_USE_FAST_INV_SQRT
	#define invSqrt(x)     fastInvSqrtAccurate(x)
	#define invSqrtFast(x) fastInvSqrt(x)
#else
	#define invSqrt(x)     (1.f/sqrtf(x))
	#define invSqrtFast(x) (1.f/sqrtf(x))
#endif


float minFloat(const float x, const float y);
float maxFloat(const float x, const float y);
float clampFloat(const float x, const float min, const float max);
unsigned int minUint(const unsigned int x, const unsigned int y);
unsigned int maxUint(const unsigned int x, const unsigned int y);
unsigned int clampUint(const unsigned int x, const unsigned int min, const unsigned int max);

float lerpFloat(const float x, const float y, const float t);
float lerpDiff(const float x, const float y, const float t);

float copySign(const float x, const float y);
float copySignZero(const float x, const float y);

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