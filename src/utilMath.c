#include "utilMath.h"


/*
** Return the squared distance between
** a point and a plane. The variable
** "vertex" is any point on the plane.
*/
float pointPlaneDistSquared(const vec3 *vertex, const vec3 *normal, const vec3 *point){
	const vec3 offset = {
		.x = point->x - vertex->x,
		.y = point->y - vertex->y,
		.z = point->z - vertex->z
	};

	return(vec3DotVec3(normal, &offset));
}


//Fast inverse square root, literally copy-pasted from the Quake III Arena source.
float fastInvSqrt(const float x){
	const float halfX = x * 0.5f;
	//Don't dereference here, we don't want any compiler warnings.
	const long *xr = (long *)&x;
	const long i = 0x5F3759DF - (*xr >> 1);
	//We avoid dereferencing here for the same reason as before.
	const float *ir = (float *)&i;
	float y = *ir;
	//Newton-Raphson method.
	y *= 1.5f - halfX * y * y;
	//A second iteration provides a more accurate result.
	//y *= 1.5f - halfX * y * y;

	return(y);
}