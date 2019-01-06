#include "utilMath.h"


#include <stdint.h>


/*
** Return the distance between a point and a plane.
** The variable "vertex" can be any point on the
** plane, but the plane's normal must be a unit vector.
*/
float pointPlaneDist(const vec3 *point, const vec3 *vertex, const vec3 *normal){
	//point - vertex
	const vec3 offset = {
		.x = point->x - vertex->x,
		.y = point->y - vertex->y,
		.z = point->z - vertex->z
	};

	return(vec3DotVec3(normal, &offset));
}

/*
** Project a point onto a plane. The variable
** "vertex" can be any point on the plane.
*/
void pointPlaneProject(const vec3 *point, const vec3 *vertex, const vec3 *normal, vec3 *out){
	//point - vertex
	const vec3 offset = {
		.x = point->x - vertex->x,
		.y = point->y - vertex->y,
		.z = point->z - vertex->z
	};
	const float dist = vec3DotVec3(normal, &offset);

	out->x = point->x - dist * normal->x;
	out->y = point->y - dist * normal->y;
	out->z = point->z - dist * normal->z;
}


/*
** Find the two closest points on two line segments,
** with starting points "sn" and ending points "en",
** and store the new points in the variables "pn".
*/
void segmentClosestPoints(const vec3 *s1, const vec3 *e1, const vec3 *s2, const vec3 *e2, vec3 *p1, vec3 *p2){
	//e1 - s1
	const vec3 v1 = {
		.x = e1->x - s1->x,
		.y = e1->y - s1->y,
		.z = e1->z - s1->z
	};
	//s1 - s2
	const vec3 v2 = {
		.x = s1->x - s2->x,
		.y = s1->y - s2->y,
		.z = s1->z - s2->z
	};
	//e2 - s2
	const vec3 v3 = {
		.x = e2->x - s2->x,
		.y = e2->y - s2->y,
		.z = e2->z - s2->z
	};

	const float d11 = vec3NormVec3(&v1);
	const float d21 = vec3DotVec3(&v2, &v1);
	const float d23 = vec3DotVec3(&v2, &v3);
	const float d31 = vec3DotVec3(&v3, &v1);
	const float d33 = vec3NormVec3(&v3);
	const float denom = d11 * d33 - d31 * d31;
	//If the two edges are perfectly parallel, the closest
	//points should be in the middle of the first segment.
	const float m1 = (denom != 0.f) ? (d23 * d31 - d21 * d33) / denom : 0.5f;
	const float m2 = (d23 + d31 * m1) / d33;

	//Find the closest point on the first line.
	vec3LerpFast(s1, &v1, m1, p1);
	//Find the closest point on the second line.
	vec3LerpFast(s2, &v2, m2, p2);
}


/*
** Calculate the normal of the triangle with vertices
** "a", "b" and "c" and store it in the variable "out".
*/
void triangleNormal(const vec3 *a, const vec3 *b, const vec3 *c, vec3 *out){
	//b - a
	const vec3 v1 = {
		.x = b->x - a->x,
		.y = b->y - a->y,
		.z = b->z - a->z
	};
	//c - a
	const vec3 v2 = {
		.x = c->x - a->x,
		.y = c->y - a->y,
		.z = c->z - a->z
	};

	vec3CrossVec3(&v1, &v2, out);
}

/*
** Compute the barycentric coordinates of the
** point "p" with respect to the triangle "abc".
*/
void pointBarycentric(const vec3 *a, const vec3 *b, const vec3 *c, const vec3 *p, vec3 *out){
	//b - a
	const vec3 v1 = {
		.x = b->x - a->x,
		.y = b->y - a->y,
		.z = b->z - a->z
	};
	//c - a
	const vec3 v2 = {
		.x = c->x - a->x,
		.y = c->y - a->y,
		.z = c->z - a->z
	};
	//p - a
	const vec3 v3 = {
		.x = p->x - a->x,
		.y = p->y - a->y,
		.z = p->z - a->z
	};
	const float d11 = vec3DotVec3(&v1, &v1);
	const float d12 = vec3DotVec3(&v1, &v2);
	const float d22 = vec3DotVec3(&v2, &v2);
	const float d31 = vec3DotVec3(&v3, &v1);
	const float d32 = vec3DotVec3(&v3, &v2);
	const float invDenom = 1.f / (d11 * d22 - d12 * d12);

	out->y = (d22 * d31 - d12 * d32) * invDenom;
	out->z = (d11 * d32 - d12 * d31) * invDenom;
	out->x = 1.f - out->y - out->z;
}


/*
** Compute an orthonormal basis from the normalised vector
** "a" and store the other two vectors in "b" and "c".
**
** Special thanks to Erin Catto for this implementation!
*/
void normalBasis(const vec3 *a, vec3 *b, vec3 *c){
	//The magic number "0x3F13CD3A" is approximately equivalent to the
	//square root of 1 over 3. In three dimensions, at least one component
	//of any unit vector must be greater than or equal to this number.
	if(fabsf(a->x) >= 0x3F13CD3A){
		vec3InitSet(b, a->y, -a->x, 0.f);
	}else{
		vec3InitSet(b, 0.f, a->z, -a->y);
	}

	vec3NormalizeVec3(b, b);
	vec3CrossVec3(a, b, c);
}


/**
void linePlaneIntersection(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict line, vec3 *const restrict point){
	//
	// Finds the intersection between
	// a line and a plane.
	//
	const float startDistance = vec3Dot(normal, vertex);
	const float t = startDistance / (startDistance + pointPlaneDistance(normal, vertex, line));
	// Lerp
	point->x = line->x * t;
	point->y = line->y * t;
	point->z = line->z * t;
}
return_t segmentPlaneIntersection(const vec3 *const restrict normal, const vec3 *const restrict vertex, const vec3 *const restrict start, const vec3 *const restrict end, vec3 *const restrict point){
	//
	// Finds the intersection between
	// a line segment and a plane.
	//
	const float startDistance = pointPlaneDistance(normal, vertex, start);
	const float endDistance = pointPlaneDistance(normal, vertex, end);
	if(startDistance <= 0.f){
		if(endDistance <= 0.f){
			return 0;
		}
	}else if(endDistance > 0.f){
		return 0;
	}
	vec3LerpR(start, end, startDistance / (startDistance - endDistance), point);
	return 1;
}
**/


/*
** Quickly approximates "1.f / sqrtf(x)". This is almost completely
** copy-pasted from the Quake III Arena source, albeit with some
** small changes in order to increase accuracy and avoid triggering
** any compiler warnings.
*/
float fastInvSqrt(const float x){
	const float x2 = x * 0.5f;
	//By using a union here, we can avoid the
	//compiler warnings that the original had.
	union {
		float f;
		uint32_t l;
	} i;
	i.f = x;
	//The original magic number, "0x5F3759DF", is supposedly an
	//approximation of the square root of 2 to the power of 127.
	//I have found that the magic number "0x5F3504F3", being a
	//more accurate approximation of this value, provides far more
	//accurate results after any number of Newton-Raphson iterations.
	i.l = 0x5F3504F3 - (i.l >> 1);

	//Use the Newton-Raphson method to
	//gain a more accurate approximation.
	i.f *= 1.5f - x2 * i.f * i.f;
	//A second iteration provides
	//an even more accurate result.
	//i.f *= 1.5f - x2 * i.f * i.f;


	return(i.f);
}