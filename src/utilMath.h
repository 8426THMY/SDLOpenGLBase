#ifndef utilMath_h
#define utilMath_h


#include <stdint.h>
#include <math.h>

#include "vec2.h"
#include "vec3.h"
#include "quat.h"


// Used to convert from degrees to radians.
#define DEG_TO_RAD(x) (M_PI/180.f*(x))
// Used to convert from radians to degrees.
#define RAD_TO_DEG(x) (180.f/M_PI*(x))
#define SQRT_ONE_THIRD 0.57735026918962576f

#define MATH_NORMALIZE_EPSILON 0.000001f

// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#define floatMinFast(x, y) (((x) < (y)) ? (x) : (y))
#define floatMaxFast(x, y) (((x) > (y)) ? (x) : (y))
#define uintMinFast(x, y) (((x) < (y)) ? (x) : (y))
#define uintMaxFast(x, y) (((x) > (y)) ? (x) : (y))

#define floatIsZero(x) (fabsf(x) < MATH_NORMALIZE_EPSILON)
#define floatIsUnit(x) (fabsf((x) - 1.f) < MATH_NORMALIZE_EPSILON)

// According to Mark Harris in his 2015 blog "GPU Pro Tip: Lerp Faster in C++",
// we can achieve better performance and accuracy by using two fmas here.
//
// Note that these are prone to double evaluation,
// so the equivalent functions should be used to prevent this.
#ifdef FP_FAST_FMAF
	#define floatLerpFast(x, y, t) fmaf(t, y, fmaf(-t, x, x))
	#define floatLerpDiffFast(x, y, t) fmaf(t, y, x)
#else
	#define floatLerpFast(x, y, t) ((x) + (t)*((y) - (x)))
	#define floatLerpDiffFast(x, y, t) ((x) + (t)*(y))
#endif

// This should only be used on ancient hardware!
#ifdef MATH_USE_FAST_INV_SQRT
	#define invSqrt(x)     fastInvSqrtAccurate(x)
	#define invSqrtFast(x) fastInvSqrt(x)
#else
	#define invSqrt(x)     (1.f/sqrtf(x))
	#define invSqrtFast(x) (1.f/sqrtf(x))
#endif


// These unions are used for performing
// bitwise operations on floats and doubles.
typedef union bitFloat {
	float f;
	uint32_t l;
} bitFloat;

typedef union bitDouble {
	double d;
	uint64_t l;
} bitDouble;


float floatMin(const float x, const float y);
float floatMax(const float x, const float y);
float floatClamp(const float x, const float min, const float max);
unsigned int uintMin(const unsigned int x, const unsigned int y);
unsigned int uintMax(const unsigned int x, const unsigned int y);
unsigned int uintClamp(const unsigned int x, const unsigned int min, const unsigned int max);

float floatLerp(const float x, const float y, const float t);
float floatLerpDiff(const float x, const float y, const float t);

float copySign(const float x, const float y);
float copySignZero(const float x, const float y);

float fastInvSqrt(const float x);
float fastInvSqrtAccurate(const float x);

void pointBarycentric(
	const vec3 *const restrict a, const vec3 *const restrict b,
	const vec3 *const restrict c, const vec3 *const restrict p,
	vec3 *const restrict out
);

void orthonormalBasis(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3);
void orthonormalBasisFast(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3);
void orthonormalBasisFaster(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3);

void segmentClosestPoints(
	const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2,
	const vec3 *const restrict e2, vec3 *const restrict p1, vec3 *const restrict p2
);

void triangleNormal(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict out);

float pointPlaneDist(const vec3 *const restrict point, const vec3 *const restrict vertex, const vec3 *const restrict normal);
void pointPlaneProject(const vec3 *const restrict point, const vec3 *const restrict vertex, const vec3 *const restrict normal, vec3 *const restrict out);

float clampEllipseDistanceFast(const float Ex, const float Ey, const float Ea, const float Eb);
float clampEllipseDistanceNormalFast(const float Ex, const float Ey, const float Ea, const float Eb, vec2 *const restrict normal);


#endif