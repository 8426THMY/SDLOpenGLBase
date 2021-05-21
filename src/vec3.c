#include "vec3.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


// Initialize the vec3's values to 0!
void vec3InitZero(vec3 *const restrict v){
	memset(v, 0.f, sizeof(*v));
}

// Initialize the vec3's values to 0!
vec3 vec3InitZeroC(){
	vec3 v;
	memset(&v, 0.f, sizeof(v));

	return(v);
}

// Initialize the vec3's values to the ones specified!
void vec3InitSet(vec3 *const restrict v, const float x, const float y, const float z){
	v->x = x;
	v->y = y;
	v->z = z;
}

// Initialize the vec3's values to the ones specified!
vec3 vec3InitSetC(const float x, const float y, const float z){
	const vec3 v = {
		.x = x,
		.y = y,
		.z = z
	};

	return(v);
}


// Add a vec3 stored as three floats to "v"!
void vec3Add(vec3 *const restrict v, const float x, const float y, const float z){
	v->x += x;
	v->y += y;
	v->z += z;
}

// Add a vec3 stored as three floats to "v" and store the result in "out"!
void vec3AddOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out){
	out->x = v->x + x;
	out->y = v->y + y;
	out->z = v->z + z;
}

// Add a vec3 stored as three floats to "v"!
vec3 vec3AddC(vec3 v, const float x, const float y, const float z){
	v.x += x;
	v.y += y;
	v.z += z;

	return(v);
}

// Add "x" to "v"!
void vec3AddS(vec3 *const restrict v, const float x){
	v->x += x;
	v->y += x;
	v->z += x;
}

// Add "x" to "v" and store the result in "out"!
void vec3AddSOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	out->x = v->x + x;
	out->y = v->y + x;
	out->z = v->z + x;
}

// Add "x" to "v"!
vec3 vec3AddSC(vec3 v, const float x){
	v.x += x;
	v.y += x;
	v.z += x;

	return(v);
}

// Add "v2" to "v1"!
void vec3AddVec3(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
}

// Add "v2" to "v1" and store the result in "out"!
void vec3AddVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
}

// Add "v2" to "v1"!
vec3 vec3AddVec3C(vec3 v1, const vec3 v2){
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;

	return(v1);
}

// Subtract a vec3 stored as three floats from "v"!
void vec3Subtract(vec3 *const restrict v, const float x, const float y, const float z){
	v->x -= x;
	v->y -= y;
	v->z -= z;
}

// Subtract a vec3 stored as three floats from "v" and store the result in "out"!
void vec3SubtractOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out){
	out->x = v->x - x;
	out->y = v->y - y;
	out->z = v->z - z;
}

// Subtract a vec3 stored as three floats from "v"!
vec3 vec3SubtractC(vec3 v, const float x, const float y, const float z){
	v.x -= x;
	v.y -= y;
	v.z -= z;

	return(v);
}

// Subtract "v" from a vec3 stored as three floats!
void vec3SubtractFrom(vec3 *const restrict v, const float x, const float y, const float z){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
}

// Subtract "v" from a vec3 stored as three floats and store the result in "out"!
void vec3SubtractFromOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out){
	out->x = x - v->x;
	out->y = y - v->y;
	out->z = z - v->z;
}

// Subtract "v" from a vec3 stored as three floats!
vec3 vec3SubtractFromC(vec3 v, const float x, const float y, const float z){
	v.x = x - v.x;
	v.y = y - v.y;
	v.z = z - v.z;

	return(v);
}

// Subtract "x" from "v"!
void vec3SubtractSFrom(vec3 *const restrict v, const float x){
	v->x -= x;
	v->y -= x;
	v->z -= x;
}

// Subtract "x" from "v" and store the result in "out"!
void vec3SubtractSFromOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	out->x = v->x - x;
	out->y = v->y - x;
	out->z = v->z - x;
}

// Subtract "x" from "v"!
vec3 vec3SubtractSFromC(vec3 v, const float x){
	v.x -= x;
	v.y -= x;
	v.z -= x;

	return(v);
}

// Subtract "v" from "x"!
void vec3SubtractFromS(vec3 *const restrict v, const float x){
	v->x = x - v->x;
	v->y = x - v->y;
	v->z = x - v->z;
}

// Subtract "v" from "x" and store the result in "out"!
void vec3SubtractFromSOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	out->x = x - v->x;
	out->y = x - v->y;
	out->z = x - v->z;
}

// Subtract "v" from "x"!
vec3 vec3SubtractFromSC(vec3 v, const float x){
	v.x = x - v.x;
	v.y = x - v.y;
	v.z = x - v.z;

	return(v);
}

// Subtract "v2" from "v1"!
void vec3SubtractVec3From(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
}

// Subtract "v1" from "v2"!
void vec3SubtractFromVec3(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x = v2->x - v1->x;
	v1->y = v2->y - v1->y;
	v1->z = v2->z - v1->z;
}

// Subtract "v2" from "v1" and store the result in "out"!
void vec3SubtractVec3FromOut(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
}

// Subtract "v2" from "v1"!
vec3 vec3SubtractVec3FromC(vec3 v1, const vec3 v2){
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return(v1);
}


// Multiply "v" by "x"!
void vec3MultiplyS(vec3 *const restrict v, const float x){
	v->x *= x;
	v->y *= x;
	v->z *= x;
}

// Multiply "v" by "x" and store the result in "out"!
void vec3MultiplySOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	out->x = v->x * x;
	out->y = v->y * x;
	out->z = v->z * x;
}

// Multiply "v" by "x"!
vec3 vec3MultiplySC(vec3 v, const float x){
	v.x *= x;
	v.y *= x;
	v.z *= x;

	return(v);
}

// Multiply "v1" by "v2"!
void vec3MultiplyVec3(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
}

// Multiply "v1" by "v2" and store the result in "out"!
void vec3MultiplyVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = v1->x * v2->x;
	out->y = v1->y * v2->y;
	out->z = v1->z * v2->z;
}

// Multiply "v1" by "v2"!
vec3 vec3MultiplyVec3C(vec3 v1, const vec3 v2){
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;

	return(v1);
}

// Divide "v" by "x"!
void vec3DivideByS(vec3 *const restrict v, const float x){
	const float invX = 1.f / x;

	v->x *= invX;
	v->y *= invX;
	v->z *= invX;
}

// Divide "v" by "x" and store the result in "out"!
void vec3DivideBySOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	const float invX = 1.f / x;

	out->x = v->x * invX;
	out->y = v->y * invX;
	out->z = v->z * invX;
}

// Divide "v" by "x"!
vec3 vec3DivideBySC(vec3 v, const float x){
	const float invX = 1.f / x;

	v.x *= invX;
	v.y *= invX;
	v.z *= invX;

	return(v);
}

// Divide "x" by "v"!
void vec3DivideSBy(vec3 *const restrict v, const float x){
	v->x = (v->x != 0.f) ? x / v->x : 0.f;
	v->y = (v->y != 0.f) ? x / v->y : 0.f;
	v->z = (v->z != 0.f) ? x / v->z : 0.f;
}

// Divide "x" by "v" and store the result in "out"!
void vec3DivideSByOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	out->x = (v->x != 0.f) ? x / v->x : 0.f;
	out->y = (v->y != 0.f) ? x / v->y : 0.f;
	out->z = (v->z != 0.f) ? x / v->z : 0.f;
}

// Divide "x" by "v"!
vec3 vec3DivideSByC(vec3 v, const float x){
	v.x = (v.x != 0.f) ? x / v.x : 0.f;
	v.y = (v.y != 0.f) ? x / v.y : 0.f;
	v.z = (v.z != 0.f) ? x / v.z : 0.f;

	return(v);
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec3DivideSByFast(vec3 *const restrict v, const float x){
	v->x = x / v->x;
	v->y = x / v->y;
	v->z = x / v->z;
}

/*
** Divide "x" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec3DivideSByFastOut(const vec3 *const restrict v, const float x, vec3 *const restrict out){
	out->x = x / v->x;
	out->y = x / v->y;
	out->z = x / v->z;
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
vec3 vec3DivideSByFastC(vec3 v, const float x){
	v.x = x / v.x;
	v.y = x / v.y;
	v.z = x / v.z;

	return(v);
}

// Divide "v1" by "v2"!
void vec3DivideByVec3(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	v1->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	v1->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
}

// Divide "v2" by "v1"!
void vec3DivideVec3By(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	v1->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	v1->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
}

// Divide "v1" by "v2" and store the result in "out"!
void vec3DivideByVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	out->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	out->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
}

// Divide "v1" by "v2"!
vec3 vec3DivideByVec3C(vec3 v1, const vec3 v2){
	v1.x = (v2.x != 0.f) ? v1.x / v2.x : 0.f;
	v1.y = (v2.y != 0.f) ? v1.y / v2.y : 0.f;
	v1.z = (v2.z != 0.f) ? v1.z / v2.z : 0.f;

	return(v1);
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec3DivideByVec3Fast(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec3DivideVec3ByFast(vec3 *const restrict v1, const vec3 *const restrict v2){
	v1->x = v2->x / v1->x;
	v1->y = v2->y / v1->y;
	v1->z = v2->z / v1->z;
}

/*
** Divide "v1" by "v2" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec3DivideByVec3FastOut(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = v1->x / v2->x;
	out->y = v1->y / v2->y;
	out->z = v1->z / v2->z;
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
vec3 vec3DivideByVec3FastC(vec3 v1, const vec3 v2){
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;

	return(v1);
}


// Multiply "u" by "x" and add it to "v"!
void vec3Fmaf(const float x, const vec3 *const restrict u, vec3 *const restrict v){
	#ifdef FP_FAST_FMAF
	v->x = fmaf(x, u->x, v->x);
	v->y = fmaf(x, u->y, v->y);
	v->z = fmaf(x, u->z, v->z);
	#else
	v->x += x*u->x;
	v->y += x*u->y;
	v->z += x*u->z;
	#endif
}

// Multiply "u" by "x", add it to "v" and store the result in "out"!
void vec3FmafOut(const float x, const vec3 *const restrict u, const vec3 *const restrict v, vec3 *const restrict out){
	#ifdef FP_FAST_FMAF
	out->x = fmaf(x, u->x, v->x);
	out->y = fmaf(x, u->y, v->y);
	out->z = fmaf(x, u->z, v->z);
	#else
	out->x = x*u->x + v->x;
	out->y = x*u->y + v->y;
	out->z = x*u->z + v->z;
	#endif
}

// Multiply "u" by "x", add it to "v" and return the result!
vec3 vec3FmafC(const float x, const vec3 u, const vec3 v){
	const vec3 out = {
	#ifdef FP_FAST_FMAF
		.x = fmaf(x, u.x, v.x),
		.y = fmaf(x, u.y, v.y),
		.z = fmaf(x, u.z, v.z)
	#else
		.x = x*u.x + v.x,
		.y = x*u.y + v.y,
		.z = x*u.z + v.z
	#endif
	};
	return(out);
}


// Find the magnitude (length) of a vec3 stored as three floats!
float vec3Magnitude(const float x, const float y, const float z){
	return(sqrtf(vec3MagnitudeSquared(x, y, z)));
}

// Find the magnitude (length) of a vec3!
float vec3MagnitudeVec3(const vec3 *const restrict v){
	return(sqrtf(vec3MagnitudeSquaredVec3(v)));
}

// Find the magnitude (length) of a vec3!
float vec3MagnitudeVec3C(const vec3 v){
	return(sqrtf(vec3MagnitudeSquaredVec3C(v)));
}

// Find the squared magnitude of a vec3 stored as three floats!
float vec3MagnitudeSquared(const float x, const float y, const float z){
	return(x * x + y * y + z * z);
}

// Find the squared magnitude of a vec3!
float vec3MagnitudeSquaredVec3(const vec3 *const restrict v){
	return(v->x * v->x + v->y * v->y + v->z * v->z);
}

// Find the squared magnitude of a vec3!
float vec3MagnitudeSquaredVec3C(const vec3 v){
	return(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Find the distance between a vec3 and one stored as three floats!
float vec3Distance(const vec3 *const restrict v, const float x, const float y, const float z){
	vec3 dist;
	vec3SubtractFromOut(v, x, y, z, &dist);
	return(vec3MagnitudeVec3(&dist));
}

// Find the distance between a vec3 and one stored as three floats!
float vec3DistanceC(const vec3 v, const float x, const float y, const float z){
	const vec3 dist = vec3SubtractFromC(v, x, y, z);
	return(vec3MagnitudeVec3C(dist));
}

// Find the squared distance between a vec3 and one stored as three floats!
float vec3DistanceSquared(const vec3 *const restrict v, const float x, const float y, const float z){
	vec3 dist;
	vec3SubtractFromOut(v, x, y, z, &dist);
	return(vec3MagnitudeSquaredVec3(&dist));
}

// Find the squared distance between a vec3 and one stored as three floats!
float vec3DistanceSquaredC(const vec3 v, const float x, const float y, const float z){
	const vec3 dist = vec3SubtractFromC(v, x, y, z);
	return(vec3MagnitudeSquaredVec3C(dist));
}

// Find the distance between two vec3s!
float vec3DistanceVec3(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 dist;
	vec3SubtractVec3FromOut(v1, v2, &dist);
	return(vec3MagnitudeVec3(&dist));
}

// Find the distance between two vec3s!
float vec3DistanceVec3C(const vec3 v1, const vec3 v2){
	const vec3 dist = vec3SubtractVec3FromC(v1, v2);
	return(vec3MagnitudeVec3C(dist));
}

// Find the squared distance between two vec3s!
float vec3DistanceSquaredVec3(const vec3 *const restrict v1, const vec3 *const restrict v2){
	vec3 dist;
	vec3SubtractVec3FromOut(v1, v2, &dist);
	return(vec3MagnitudeSquaredVec3(&dist));
}

// Find the squared distance between two vec3s!
float vec3DistanceSquaredVec3C(const vec3 v1, const vec3 v2){
	const vec3 dist = vec3SubtractVec3FromC(v1, v2);
	return(vec3MagnitudeSquaredVec3C(dist));
}


// Find the dot product of two vec3s stored as three floats!
float vec3Dot(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2){
	return(x1 * x2 + y1 * y2 + z1 * z2);
}

// Find the dot product of a vec3 and one stored as three floats!
float vec3DotVec3Float(const vec3 *const restrict v, const float x, const float y, const float z){
	return(v->x * x + v->y * y + v->z * z);
}

// Find the dot product of a vec3 and one stored as three floats!
float vec3DotVec3FloatC(const vec3 v, const float x, const float y, const float z){
	return(v.x * x + v.y * y + v.z * z);
}

// Find the dot product of two vec3s!
float vec3DotVec3(const vec3 *const restrict v1, const vec3 *const restrict v2){
	return(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

// Find the dot product of two vec3s!
float vec3DotVec3C(const vec3 v1, const vec3 v2){
	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}


// Find the cross product of two vec3s (f1 x f2) stored as three floats and store the result in "out"!
void vec3Cross(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, vec3 *const restrict out){
	out->x = y1 * z2 - z1 * y2;
	out->y = z1 * x2 - x1 * z2;
	out->z = x1 * y2 - y1 * x2;
}

// Find the cross product of two vec3s (f1 x f2) stored as three floats!
vec3 vec3CrossC(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2){
	vec3 v;
	v.x = y1 * z2 - z1 * y2;
	v.y = z1 * x2 - x1 * z2;
	v.z = x1 * y2 - y1 * x2;

	return(v);
}

// Find the cross product of a vec3 and one stored as three floats (v x f) and store the result in "out"!
void vec3CrossVec3Float(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out){
	out->x = v->y * z - v->z * y;
	out->y = v->z * x - v->x * z;
	out->z = v->x * y - v->y * x;
}

// Find the cross product of a vec3 and one stored as three floats (v x f)!
vec3 vec3CrossVec3FloatC(const vec3 v, const float x, const float y, const float z){
	vec3 out;

	out.x = v.y * z - v.z * y;
	out.y = v.z * x - v.x * z;
	out.z = v.x * y - v.y * x;

	return(out);
}

// Find the cross product of a vec3 stored as three floats and a normal one (f x v) and store the result in "out"!
void vec3CrossFloatVec3(const float x, const float y, const float z, const vec3 *const restrict v, vec3 *const restrict out){
	out->x = y * v->z - z * v->y;
	out->y = z * v->x - x * v->z;
	out->z = x * v->y - y * v->x;
}

// Find the cross product of a vec3 stored as three floats and a normal one (f x v)!
vec3 vec3CrossFloatVec3C(const float x, const float y, const float z, const vec3 v){
	vec3 out;

	out.x = y * v.z - z * v.y;
	out.y = z * v.x - x * v.z;
	out.z = x * v.y - y * v.x;

	return(out);
}

// Find the cross product of two vec3s (v1 x v2) and store the result in "v1"!
void vec3CrossByVec3(vec3 *const restrict v1, const vec3 *const restrict v2){
	const vec3 temp = *v1;

	v1->x = temp.y * v2->z - temp.z * v2->y;
	v1->y = temp.z * v2->x - temp.x * v2->z;
	v1->z = temp.x * v2->y - temp.y * v2->x;
}

// Find the cross product of two vec3s (v2 x v1) and store the result in "v1"!
void vec3CrossVec3By(vec3 *const restrict v1, const vec3 *const restrict v2){
	const vec3 temp = *v1;

	v1->x = v2->y * temp.z - v2->z * temp.y;
	v1->y = v2->z * temp.x - v2->x * temp.z;
	v1->z = v2->x * temp.y - v2->y * temp.x;
}

// Find the cross product of two vec3s (v1 x v2) and store the result in "out"!
void vec3CrossVec3Out(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = v1->y * v2->z - v1->z * v2->y;
	out->y = v1->z * v2->x - v1->x * v2->z;
	out->z = v1->x * v2->y - v1->y * v2->x;
}

// Find the cross product of two vec3s (v1 x v2)!
vec3 vec3CrossVec3C(const vec3 v1, const vec3 v2){
	vec3 out;

	out.x = v1.y * v2.z - v1.z * v2.y;
	out.y = v1.z * v2.x - v1.x * v2.z;
	out.z = v1.x * v2.y - v1.y * v2.x;

	return(out);
}


// Normalize a vec3 stored as three floats and store the result in "out"!
void vec3Normalize(const float x, const float y, const float z, vec3 *const restrict out){
	const float magnitude = invSqrt(x * x + y * y + z * z);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
}

// Normalize a vec3 stored as three floats and store the result in "out"!
void vec3NormalizeFast(const float x, const float y, const float z, vec3 *const restrict out){
	const float magnitude = invSqrtFast(x * x + y * y + z * z);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
}

// Normalize a vec3 stored as three floats and store the result in "out"!
vec3 vec3NormalizeC(const float x, const float y, const float z){
	const float magnitude = invSqrt(x * x + y * y + z * z);
	vec3 v;

	v.x = x * magnitude;
	v.y = y * magnitude;
	v.z = z * magnitude;

	return(v);
}

// Normalize a vec3 stored as three floats and store the result in "out"!
vec3 vec3NormalizeFastC(const float x, const float y, const float z){
	const float magnitude = invSqrtFast(x * x + y * y + z * z);
	vec3 v;

	v.x = x * magnitude;
	v.y = y * magnitude;
	v.z = z * magnitude;

	return(v);
}

// Normalize a vec3!
void vec3NormalizeVec3(vec3 *const restrict v){
	const float magnitude = invSqrt(v->x * v->x + v->y * v->y + v->z * v->z);

	v->x *= magnitude;
	v->y *= magnitude;
	v->z *= magnitude;
}

// Normalize a vec3!
void vec3NormalizeVec3Fast(vec3 *const restrict v){
	const float magnitude = invSqrtFast(v->x * v->x + v->y * v->y + v->z * v->z);

	v->x *= magnitude;
	v->y *= magnitude;
	v->z *= magnitude;
}

// Normalize a vec3 and store the result in "out"!
void vec3NormalizeVec3Out(const vec3 *const restrict v, vec3 *const restrict out){
	const float magnitude = invSqrt(v->x * v->x + v->y * v->y + v->z * v->z);

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
	out->z = v->z * magnitude;
}

// Normalize a vec3 and store the result in "out"!
void vec3NormalizeVec3FastOut(const vec3 *const restrict v, vec3 *const restrict out){
	const float magnitude = invSqrtFast(v->x * v->x + v->y * v->y + v->z * v->z);

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
	out->z = v->z * magnitude;
}

// Normalize a vec3!
vec3 vec3NormalizeVec3C(vec3 v){
	const float magnitude = invSqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	v.x *= magnitude;
	v.y *= magnitude;
	v.z *= magnitude;

	return(v);
}

// Normalize a vec3!
vec3 vec3NormalizeVec3FastC(vec3 v){
	const float magnitude = invSqrtFast(v.x * v.x + v.y * v.y + v.z * v.z);

	v.x *= magnitude;
	v.y *= magnitude;
	v.z *= magnitude;

	return(v);
}

// Negate the values of a vec3!
void vec3Negate(vec3 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

// Negate the values of a vec3 and store the result in "out"!
void vec3NegateOut(const vec3 *const restrict v, vec3 *const restrict out){
	out->x = -v->x;
	out->y = -v->y;
	out->z = -v->z;
}

// Negate the values of a vec3!
vec3 vec3NegateC(vec3 v){
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;

	return(v);
}


/*
** Generate a vector that is orthogonal to the
** input vector and store the result in "out"!
**
** Note that the output vector is not normalized in general.
*/
void vec3Orthogonal(const vec3 *const restrict v, vec3 *const restrict out){
	// At least one component is always greater
	// than 1/sqrt(3) if 'v' is unit length.
	if(fabsf(v->x) >= 0.57735026f){
		vec3InitSet(out, v->y, -v->x, 0.f);
	}else{
		vec3InitSet(out, 0.f, v->z, -v->y);
	}
}

/*
** Generate a vector that is orthogonal to the input vector.
**
** Note that the output vector is not normalized in general.
*/
vec3 vec3OrthogonalC(const vec3 v){
	// At least one component is always greater
	// than 1/sqrt(3) if 'v' is unit length.
	if(fabsf(v.x) >= 0.57735026f){
		return(vec3InitSetC(v.y, -v.x, 0.f));
	}
	return(vec3InitSetC(0.f, v.z, -v.y));
}

/*
** Generate a vector that is orthogonal to the
** input vector and store the result in "out"!
**
** Note that this is basically copied from Pixar's
** code for generating an orthonormal basis.
** See "orthonormalBasisFaster" in "utilMath.c".
*/
void vec3Orthonormal(const vec3 *const restrict v, vec3 *const restrict out){
	const float sign = copySign(1.f, v->z);
	const float a = -1.f/(sign + v->z);
	const float b = v->x*v->y*a;
	vec3InitSet(out, b, sign + v->y*v->y*a, -v->y);
}

/*
** Generate a vector that is orthogonal to the input vector.
**
** Note that this is basically copied from Pixar's
** code for generating an orthonormal basis.
** See "orthonormalBasisFaster" in "utilMath.c".
*/
vec3 vec3OrthonormalC(const vec3 v){
	const float sign = copySign(1.f, v.z);
	const float a = -1.f/(sign + v.z);
	const float b = v.x*v.y*a;
	return(vec3InitSetC(b, sign + v.y*v.y*a, -v.y));
}


// Convert a vec3 from degrees to radians!
void vec3DegToRad(vec3 *const restrict v){
	v->x *= DEG_TO_RAD;
	v->y *= DEG_TO_RAD;
	v->z *= DEG_TO_RAD;
}

// Convert a vec3 from radians to degrees!
void vec3RadToDeg(vec3 *const restrict v){
	v->x *= RAD_TO_DEG;
	v->y *= RAD_TO_DEG;
	v->z *= RAD_TO_DEG;
}


// Rotate a vec3 using XYZ Euler angles (in radians)!
void vec3RotateByEulerXYZ(vec3 *const restrict v, const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;
	// Note that this is row-major, whereas everywhere else we use column-major.
	// It's better using row-major here since we can read the entries in order.
	const float rotMatrix[9] = {
		cy*cz, sxsy*cz - cx*sz, cxsy*cz + sx*sz,
		cy*sz, sxsy*sz + cx*cz, cxsy*sz - sx*cz,
		  -sy,           sx*cy,           cx*cy
	};
	const vec3 tempVec = *v;

	v->x = rotMatrix[0]*tempVec.x + rotMatrix[1]*tempVec.y + rotMatrix[2]*tempVec.z;
	v->y = rotMatrix[3]*tempVec.x + rotMatrix[4]*tempVec.y + rotMatrix[5]*tempVec.z;
	v->z = rotMatrix[6]*tempVec.x + rotMatrix[7]*tempVec.y + rotMatrix[8]*tempVec.z;
}

// Rotate a vec3 using ZXY Euler angles (in radians)!
void vec3RotateByEulerZXY(vec3 *const restrict v, const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;
	// Note that this is row-major, whereas everywhere else we use column-major.
	// It's better using row-major here since we can read the entries in order.
	const float rotMatrix[9] = {
		sxsy*sz + cy*cz, sxsy*cz - cy*sz, cx*sy,
		          cx*sz,           cx*cz,   -sx,
		sxcy*sz - sy*cz, sxcy*cz + sy*sz, cx*cy
	};
	const vec3 tempVec = *v;

	v->x = rotMatrix[0]*tempVec.x + rotMatrix[1]*tempVec.y + rotMatrix[2]*tempVec.z;
	v->y = rotMatrix[3]*tempVec.x + rotMatrix[4]*tempVec.y + rotMatrix[5]*tempVec.z;
	v->z = rotMatrix[6]*tempVec.x + rotMatrix[7]*tempVec.y + rotMatrix[8]*tempVec.z;
}

// Rotate a vec3 using XYZ Euler angles (in radians) and return the result!
vec3 vec3RotateByEulerXYZC(const vec3 v, const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;
	// Note that this is row-major, whereas everywhere else we use column-major.
	// It's better using row-major here since we can read the entries in order.
	const float rotMatrix[9] = {
		cy*cz, sxsy*cz - cx*sz, cxsy*cz + sx*sz,
		cy*sz, sxsy*sz + cx*cz, cxsy*sz - sx*cz,
		  -sy,           sx*cy,           cx*cy
	};

	return(vec3InitSetC(
		rotMatrix[0]*v.x + rotMatrix[1]*v.y + rotMatrix[2]*v.z,
		rotMatrix[3]*v.x + rotMatrix[4]*v.y + rotMatrix[5]*v.z,
		rotMatrix[6]*v.x + rotMatrix[7]*v.y + rotMatrix[8]*v.z
	));
}

// Rotate a vec3 using ZXY Euler angles (in radians) and return the result!
vec3 vec3RotateByEulerZXYC(const vec3 v, const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;
	// Note that this is row-major, whereas everywhere else we use column-major.
	// It's better using row-major here since we can read the entries in order.
	const float rotMatrix[9] = {
		sxsy*sz + cy*cz, sxsy*cz - cy*sz, cx*sy,
		          cx*sz,           cx*cz,   -sx,
		sxcy*sz - sy*cz, sxcy*cz + sy*sz, cx*cy
	};

	return(vec3InitSetC(
		rotMatrix[0]*v.x + rotMatrix[1]*v.y + rotMatrix[2]*v.z,
		rotMatrix[3]*v.x + rotMatrix[4]*v.y + rotMatrix[5]*v.z,
		rotMatrix[6]*v.x + rotMatrix[7]*v.y + rotMatrix[8]*v.z
	));
}

// Rotate a vec3 using XYZ Euler angles (in radians) and store the result in "out"!
void vec3RotateByEulerXYZOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;
	// Note that this is row-major, whereas everywhere else we use column-major.
	// It's better using row-major here since we can read the entries in order.
	const float rotMatrix[9] = {
		cy*cz, sxsy*cz - cx*sz, cxsy*cz + sx*sz,
		cy*sz, sxsy*sz + cx*cz, cxsy*sz - sx*cz,
		  -sy,           sx*cy,           cx*cy
	};

	out->x = rotMatrix[0]*v->x + rotMatrix[1]*v->y + rotMatrix[2]*v->z;
	out->y = rotMatrix[3]*v->x + rotMatrix[4]*v->y + rotMatrix[5]*v->z;
	out->z = rotMatrix[6]*v->x + rotMatrix[7]*v->y + rotMatrix[8]*v->z;
}

// Rotate a vec3 using ZXY Euler angles (in radians) and store the result in "out"!
void vec3RotateByEulerZXYOut(const vec3 *const restrict v, const float x, const float y, const float z, vec3 *const restrict out){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;
	// Note that this is row-major, whereas everywhere else we use column-major.
	// It's better using row-major here since we can read the entries in order.
	const float rotMatrix[9] = {
		sxsy*sz + cy*cz, sxsy*cz - cy*sz, cx*sy,
		          cx*sz,           cx*cz,   -sx,
		sxcy*sz - sy*cz, sxcy*cz + sy*sz, cx*cy
	};

	out->x = rotMatrix[0]*v->x + rotMatrix[1]*v->y + rotMatrix[2]*v->z;
	out->y = rotMatrix[3]*v->x + rotMatrix[4]*v->y + rotMatrix[5]*v->z;
	out->z = rotMatrix[6]*v->x + rotMatrix[7]*v->y + rotMatrix[8]*v->z;
}


// Perform linear interpolation between two vec3s and store the result in "out"!
void vec3Lerp(const vec3 *const restrict v1, const vec3 *const restrict v2, const float time, vec3 *const restrict out){
	out->x = lerpFloatFast(v1->x, v2->x, time);
	out->y = lerpFloatFast(v1->y, v2->y, time);
	out->z = lerpFloatFast(v1->z, v2->z, time);
}

// Perform linear interpolation between two vec3s!
vec3 vec3LerpC(vec3 v1, const vec3 v2, const float time){
	v1.x = lerpFloatFast(v1.x, v2.x, time);
	v1.y = lerpFloatFast(v1.y, v2.y, time);
	v1.z = lerpFloatFast(v1.z, v2.z, time);

	return(v1);
}

/*
** Perform linear interpolation between two vec3s and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
void vec3LerpDiff(const vec3 *const restrict v, const vec3 *offset, const float time, vec3 *const restrict out){
	out->x = lerpDiffFast(v->x, offset->x, time);
	out->y = lerpDiffFast(v->y, offset->y, time);
	out->z = lerpDiffFast(v->z, offset->z, time);
}

/*
** Perform linear interpolation between two vec3s!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
vec3 vec3LerpDiffC(vec3 v, const vec3 offset, const float time){
	v.x = lerpDiffFast(v.x, offset.x, time);
	v.y = lerpDiffFast(v.y, offset.y, time);
	v.z = lerpDiffFast(v.z, offset.z, time);

	return(v);
}


/*
** Compare two vec3s to find the minimum value per axis
** and return a vec3 composed of these minima in "out".
*/
void vec3Min(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = minFloatFast(v1->x, v2->x);
	out->y = minFloatFast(v1->y, v2->y);
	out->z = minFloatFast(v1->z, v2->z);
}

/*
** Compare two vec3s to find the minimum value per
** axis and return a vec3 composed of these minima.
*/
vec3 vec3MinC(vec3 v1, const vec3 v2){
	v1.x = minFloatFast(v1.x, v2.x);
	v1.y = minFloatFast(v1.y, v2.y);
	v1.z = minFloatFast(v1.z, v2.z);

	return(v1);
}

/*
** Compare two vec3s to find the maximum value per axis
** and return a vec3 composed of these maxima in "out".
*/
void vec3Max(const vec3 *const restrict v1, const vec3 *const restrict v2, vec3 *const restrict out){
	out->x = maxFloatFast(v1->x, v2->x);
	out->y = maxFloatFast(v1->y, v2->y);
	out->z = maxFloatFast(v1->z, v2->z);
}

/*
** Compare two vec3s to find the maximum value per
** axis and return a vec3 composed of these maxima.
*/
vec3 vec3MaxC(vec3 v1, const vec3 v2){
	v1.x = maxFloatFast(v1.x, v2.x);
	v1.y = maxFloatFast(v1.y, v2.y);
	v1.z = maxFloatFast(v1.z, v2.z);

	return(v1);
}