#ifndef utilMath_h
#define utilMath_h


#include <math.h>

#include "vec3.h"


#define DEG_TO_RAD (M_PI / 180.f)
#define RAD_TO_DEG (180.f / M_PI)

#define floatLerp(x, y, t) ((y - x) * t + x)


float pointPlaneDistSquared(const vec3 *vertex, const vec3 *normal, const vec3 *point);

float fastInvSqrt(const float x);


#endif