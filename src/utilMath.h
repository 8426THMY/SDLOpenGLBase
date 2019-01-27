#ifndef utilMath_h
#define utilMath_h


#include <math.h>

#include "vec3.h"


#define DEG_TO_RAD (M_PI / 180.f)
#define RAD_TO_DEG (180.f / M_PI)

#define lerpNum(x, y, t) (x + t * (y - x))
#define lerpNumFast(x, y, t) (x + t * y)

#define minNum(x, y) (((x) <= (y)) ? (x) : (y))
#define maxNum(x, y) (((x) >= (y)) ? (x) : (y))


float fastInvSqrt(const float x);

void pointBarycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p, vec3 *out);

void normalBasis(const vec3 *a, vec3 *b, vec3 *c);
void segmentClosestPoints(const vec3 *s1, const vec3 *e1, const vec3 *s2, const vec3 *e2, vec3 *p1, vec3 *p2);

void triangleNormal(const vec3 *a, const vec3 *b, const vec3 *c, vec3 *out);

float pointPlaneDist(const vec3 *point, const vec3 *vertex, const vec3 *normal);
void pointPlaneProject(const vec3 *point, const vec3 *vertex, const vec3 *normal, vec3 *out);


#endif