#include "vec4.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


// Initialize the vec4's values to 0!
void vec4InitZero(vec4 *const restrict v){
	memset(v, 0.f, sizeof(*v));
}

// Initialize the vec4's values to 0!
vec4 vec4InitZeroC(){
	vec4 v;
	memset(&v, 0.f, sizeof(v));

	return(v);
}

// Initialize the vec4's values using a vec3!
void vec4InitVec3(vec4 *const restrict v1, const vec3 *const restrict v2, const float w){
	v1->x = v1->x;
	v1->y = v1->y;
	v1->z = v1->z;
	v1->w = w;
}

// Initialize the vec4's values using a vec3!
vec4 vec4InitVec3C(const vec3 v, const float w){
	vec4 v2 = {
		.x = v.x,
		.y = v.y,
		.z = v.z,
		.w = w
	};

	return(v2);
}

// Initialize the vec4's values to the ones specified!
void vec4InitSet(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}

// Initialize the vec4's values to the ones specified!
vec4 vec4InitSetC(const float x, const float y, const float z, const float w){
	vec4 v = {
		.x = x,
		.y = y,
		.z = z,
		.w = w
	};

	return(v);
}


// Add a vec4 stored as four floats to "v"!
void vec4Add(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x += x;
	v->y += y;
	v->z += z;
	v->w += w;
}

// Add a vec4 stored as four floats to "v" and store the result in "out"!
void vec4AddOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *out){
	out->x = v->x + x;
	out->y = v->y + y;
	out->z = v->z + z;
	out->w = v->w + w;
}

// Add a vec4 stored as four floats to "v"!
vec4 vec4AddC(vec4 v, const float x, const float y, const float z, const float w){
	v.x += x;
	v.y += y;
	v.z += z;
	v.w += w;

	return(v);
}

// Add "x" to "v"!
void vec4AddS(vec4 *const restrict v, const float x){
	v->x += x;
	v->y += x;
	v->z += x;
	v->w += x;
}

// Add "x" to "v" and store the result in "out"!
void vec4AddSOut(const vec4 *const restrict v, const float x, vec4 *out){
	out->x = v->x + x;
	out->y = v->y + x;
	out->z = v->z + x;
	out->w = v->w + x;
}

// Add "x" to "v"!
vec4 vec4AddSC(vec4 v, const float x){
	v.x += x;
	v.y += x;
	v.z += x;
	v.w += x;

	return(v);
}

// Add "v2" to "v1"!
void vec4AddVec4(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
	v1->w += v2->w;
}

// Add "v2" to "v1" and store the result in "out"!
void vec4AddVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
	out->w = v1->w + v2->w;
}

// Add "v2" to "v1"!
vec4 vec4AddVec4C(vec4 v1, const vec4 v2){
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;

	return(v1);
}

// Subtract a vec4 stored as four floats from "v"!
void vec4Subtract(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x -= x;
	v->y -= y;
	v->z -= z;
	v->w -= w;
}

// Subtract a vec4 stored as four floats from "v" and store the result in "out"!
void vec4SubtractOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *out){
	out->x = v->x - x;
	out->y = v->y - y;
	out->z = v->z - z;
	out->w = v->w - w;
}

// Subtract a vec4 stored as four floats from "v"!
vec4 vec4SubtractC(vec4 v, const float x, const float y, const float z, const float w){
	v.x -= x;
	v.y -= y;
	v.z -= z;
	v.w -= w;

	return(v);
}

// Subtract "v" from a vec4 stored as four floats!
void vec4SubtractFrom(vec4 *const restrict v, const float x, const float y, const float z, const float w){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
	v->w = w - v->w;
}

// Subtract "v" from a vec4 stored as four floats and store the result in "out"!
void vec4SubtractFromOut(const vec4 *const restrict v, const float x, const float y, const float z, const float w, vec4 *out){
	out->x = x - v->x;
	out->y = y - v->y;
	out->z = z - v->z;
	out->w = w - v->w;
}

// Subtract "v" from a vec4 stored as four floats!
vec4 vec4SubtractFromC(vec4 v, const float x, const float y, const float z, const float w){
	v.x = x - v.x;
	v.y = y - v.y;
	v.z = z - v.z;
	v.w = w - v.w;

	return(v);
}

// Subtract "x" from "v"!
void vec4SubtractSFrom(vec4 *const restrict v, const float x){
	v->x -= x;
	v->y -= x;
	v->z -= x;
	v->w -= x;
}

// Subtract "x" from "v" and store the result in "out"!
void vec4SubtractSFromOut(const vec4 *const restrict v, const float x, vec4 *out){
	out->x = v->x - x;
	out->y = v->y - x;
	out->z = v->z - x;
	out->w = v->w - x;
}

// Subtract "x" from "v"!
vec4 vec4SubtractSFromC(vec4 v, const float x){
	v.x -= x;
	v.y -= x;
	v.z -= x;
	v.w -= x;

	return(v);
}

// Subtract "v" from "x"!
void vec4SubtractFromS(vec4 *const restrict v, const float x){
	v->x = x - v->x;
	v->y = x - v->y;
	v->z = x - v->z;
	v->w = x - v->w;
}

// Subtract "v" from "x" and store the result in "out"!
void vec4SubtractFromSOut(const vec4 *const restrict v, const float x, vec4 *out){
	out->x = x - v->x;
	out->y = x - v->y;
	out->z = x - v->z;
	out->w = x - v->w;
}

// Subtract "v" from "x"!
vec4 vec4SubtractFromSC(vec4 v, const float x){
	v.x = x - v.x;
	v.y = x - v.y;
	v.z = x - v.z;
	v.w = x - v.w;

	return(v);
}

// Subtract "v2" from "v1"!
void vec4SubtractVec4From(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
	v1->w -= v2->w;
}

// Subtract "v1" from "v2"!
void vec4SubtractFromVec4(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x = v2->x - v1->x;
	v1->y = v2->y - v1->y;
	v1->z = v2->z - v1->z;
	v1->w = v2->w - v1->w;
}

// Subtract "v2" from "v1" and store the result in "out"!
void vec4SubtractVec4FromOut(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
	out->w = v1->w - v2->w;
}

// Subtract "v2" from "v1"!
vec4 vec4SubtractVec4FromC(vec4 v1, const vec4 v2){
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;

	return(v1);
}


// Multiply "v" by "x"!
void vec4MultiplyS(vec4 *const restrict v, const float x){
	v->x *= x;
	v->y *= x;
	v->z *= x;
	v->w *= x;
}

// Multiply "v" by "x" and store the result in "out"!
void vec4MultiplySOut(const vec4 *const restrict v, const float x, vec4 *out){
	out->x = v->x * x;
	out->y = v->y * x;
	out->z = v->z * x;
	out->w = v->w * x;
}

// Multiply "v" by "x"!
vec4 vec4MultiplySC(vec4 v, const float x){
	v.x *= x;
	v.y *= x;
	v.z *= x;
	v.w *= x;

	return(v);
}

// Multiply "v1" by "v2"!
void vec4MultiplyVec4(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
	v1->w *= v2->w;
}

// Multiply "v1" by "v2" and store the result in "out"!
void vec4MultiplyVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = v1->x * v2->x;
	out->y = v1->y * v2->y;
	out->z = v1->z * v2->z;
	out->w = v1->w * v2->w;
}

// Multiply "v1" by "v2"!
vec4 vec4MultiplyVec4C(vec4 v1, const vec4 v2){
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;

	return(v1);
}

// Divide "v" by "x"!
void vec4DivideByS(vec4 *const restrict v, const float x){
	const float invX = 1.f / x;

	v->x *= invX;
	v->y *= invX;
	v->z *= invX;
	v->w *= invX;
}

// Divide "v" by "x" and store the result in "out"!
void vec4DivideBySOut(const vec4 *const restrict v, const float x, vec4 *out){
	const float invX = 1.f / x;

	out->x = v->x * invX;
	out->y = v->y * invX;
	out->z = v->z * invX;
	out->w = v->w * invX;
}

// Divide "v" by "x"!
vec4 vec4DivideBySC(vec4 v, const float x){
	const float invX = 1.f / x;

	v.x *= invX;
	v.y *= invX;
	v.z *= invX;
	v.w *= invX;

	return(v);
}

// Divide "x" by "v"!
void vec4DivideSBy(vec4 *const restrict v, const float x){
	v->x = (v->x != 0.f) ? x / v->x : 0.f;
	v->y = (v->y != 0.f) ? x / v->y : 0.f;
	v->z = (v->z != 0.f) ? x / v->z : 0.f;
	v->w = (v->w != 0.f) ? x / v->w : 0.f;
}

// Divide "x" by "v" and store the result in "out"!
void vec4DivideSByOut(const vec4 *const restrict v, const float x, vec4 *out){
	out->x = (v->x != 0.f) ? x / v->x : 0.f;
	out->y = (v->y != 0.f) ? x / v->y : 0.f;
	out->z = (v->z != 0.f) ? x / v->z : 0.f;
	out->w = (v->w != 0.f) ? x / v->w : 0.f;
}

// Divide "x" by "v"!
vec4 vec4DivideSByC(vec4 v, const float x){
	v.x = (v.x != 0.f) ? x / v.x : 0.f;
	v.y = (v.y != 0.f) ? x / v.y : 0.f;
	v.z = (v.z != 0.f) ? x / v.z : 0.f;
	v.w = (v.w != 0.f) ? x / v.w : 0.f;

	return(v);
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec4DivideSByFast(vec4 *const restrict v, const float x){
	v->x = x / v->x;
	v->y = x / v->y;
	v->z = x / v->z;
	v->w = x / v->w;
}

/*
** Divide "x" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec4DivideSByFastOut(const vec4 *const restrict v, const float x, vec4 *out){
	out->x = x / v->x;
	out->y = x / v->y;
	out->z = x / v->z;
	out->w = x / v->w;
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
vec4 vec4DivideSByFastC(vec4 v, const float x){
	v.x = x / v.x;
	v.y = x / v.y;
	v.z = x / v.z;
	v.w = x / v.w;

	return(v);
}

// Divide "v" by "v"!
void vec4DivideByVec4(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	v1->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	v1->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
	v1->w = (v2->w != 0.f) ? v1->w / v2->w : 0.f;
}

// Divide "v2" by "v1"!
void vec4DivideVec4By(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x = (v1->x != 0.f) ? v2->x / v1->x : 0.f;
	v1->y = (v1->y != 0.f) ? v2->y / v1->y : 0.f;
	v1->z = (v1->z != 0.f) ? v2->z / v1->z : 0.f;
	v1->w = (v1->w != 0.f) ? v2->w / v1->w : 0.f;
}

// Divide "v1" by "v2" and store the result in "out"!
void vec4DivideByVec4Out(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	out->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	out->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
	out->w = (v2->w != 0.f) ? v1->w / v2->w : 0.f;
}

// Divide "v1" by "v2"!
vec4 vec4DivideByVec4C(vec4 v1, const vec4 v2){
	v1.x = (v2.x != 0.f) ? v1.x / v2.x : 0.f;
	v1.y = (v2.y != 0.f) ? v1.y / v2.y : 0.f;
	v1.z = (v2.z != 0.f) ? v1.z / v2.z : 0.f;
	v1.w = (v2.w != 0.f) ? v1.w / v2.w : 0.f;

	return(v1);
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec4DivideByVec4Fast(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
	v1->w /= v2->w;
}

/*
** Divide "v2" by "v1"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec4DivideVec4ByFast(vec4 *const restrict v1, const vec4 *const restrict v2){
	v1->x = v2->x / v1->x;
	v1->y = v2->y / v1->y;
	v1->z = v2->z / v1->z;
	v1->w = v2->w / v1->w;
}

/*
** Divide "v1" by "v2" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec4DivideByVec4FastOut(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = v1->x / v2->x;
	out->y = v1->y / v2->y;
	out->z = v1->z / v2->z;
	out->w = v1->w / v2->w;
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
vec4 vec4DivideByVec4FastC(vec4 v1, const vec4 v2){
	v1.x = v1.x / v2.x;
	v1.y = v1.y / v2.y;
	v1.z = v1.z / v2.z;
	v1.w = v1.w / v2.w;

	return(v1);
}


// Multiply "u" by "x" and add it to "v"!
void vec4Fmaf(const float x, const vec4 *const restrict u, vec4 *const restrict v){
	#ifdef FP_FAST_FMAF
	v->x = fmaf(x, u->x, v->x);
	v->y = fmaf(x, u->y, v->y);
	v->z = fmaf(x, u->z, v->z);
	v->w = fmaf(x, u->w, v->w);
	#else
	v->x += x*u->x;
	v->y += x*u->y;
	v->z += x*u->z;
	v->w += x*u->w;
	#endif
}

// Multiply "u" by "x", add it to "v" and store the result in "out"!
void vec4FmafOut(const float x, const vec4 *const restrict u, const vec4 *const restrict v, vec4 *const restrict out){
	#ifdef FP_FAST_FMAF
	out->x = fmaf(x, u->x, v->x);
	out->y = fmaf(x, u->y, v->y);
	out->z = fmaf(x, u->z, v->z);
	out->w = fmaf(x, u->w, v->w);
	#else
	out->x = x*u->x + v->x;
	out->y = x*u->y + v->y;
	out->z = x*u->z + v->z;
	out->w = x*u->w + v->w;
	#endif
}

// Multiply "u" by "x", add it to "v" and return the result!
vec4 vec4FmafC(const float x, const vec4 u, const vec4 v){
	const vec4 out = {
	#ifdef FP_FAST_FMAF
		.x = fmaf(x, u.x, v.x),
		.y = fmaf(x, u.y, v.y),
		.z = fmaf(x, u.z, v.z),
		.w = fmaf(x, u.w, v.w)
	#else
		.x = x*u.x + v.x,
		.y = x*u.y + v.y,
		.z = x*u.z + v.z,
		.w = x*u.w + v.w
	#endif
	};
	return(out);
}


// Find the magnitude (length) of a vec4 stored as four floats!
float vec4Magnitude(const float x, const float y, const float z, const float w){
	return(sqrtf(vec4MagnitudeSquared(x, y, z, w)));
}

// Find the magnitude (length) of a vec4!
float vec4MagnitudeVec4(const vec4 *const restrict v){
	return(sqrtf(vec4MagnitudeSquaredVec4(v)));
}

// Find the magnitude (length) of a vec4!
float vec4MagnitudeVec4C(const vec4 v){
	return(sqrtf(vec4MagnitudeSquaredVec4C(v)));
}

// Find the squared magnitude of a vec4 stored as four floats!
float vec4MagnitudeSquared(const float x, const float y, const float z, const float w){
	return(x * x + y * y + z * z + w * w);
}

// Find the squared magnitude of a vec4!
float vec4MagnitudeSquaredVec4(const vec4 *const restrict v){
	return(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}

// Find the squared magnitude of a vec4!
float vec4MagnitudeSquaredVec4C(const vec4 v){
	return(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// Find the distance between a vec4 and one stored as four floats!
float vec4Distance(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	vec4 dist;
	vec4SubtractFromOut(v, x, y, z, w, &dist);
	return(vec4MagnitudeVec4(&dist));
}

// Find the distance between a vec4 and one stored as four floats!
float vec4DistanceC(const vec4 v, const float x, const float y, const float z, const float w){
	const vec4 dist = vec4SubtractFromC(v, x, y, z, w);
	return(vec4MagnitudeVec4C(dist));
}

// Find the squared distance between a vec4 and one stored as four floats!
float vec4DistanceSquared(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	vec4 dist;
	vec4SubtractFromOut(v, x, y, z, w, &dist);
	return(vec4MagnitudeSquaredVec4(&dist));
}

// Find the squared distance between a vec4 and one stored as four floats!
float vec4DistanceSquaredC(const vec4 v, const float x, const float y, const float z, const float w){
	const vec4 dist = vec4SubtractFromC(v, x, y, z, w);
	return(vec4MagnitudeSquaredVec4C(dist));
}

// Find the distance between two vec4s!
float vec4DistanceVec4(const vec4 *const restrict v1, const vec4 *const restrict v2){
	vec4 dist;
	vec4SubtractVec4FromOut(v1, v2, &dist);
	return(vec4MagnitudeVec4(&dist));
}

// Find the distance between two vec4s!
float vec4DistanceVec4C(const vec4 v1, const vec4 v2){
	const vec4 dist = vec4SubtractVec4FromC(v1, v2);
	return(vec4MagnitudeVec4C(dist));
}

// Find the squared distance between two vec4s!
float vec4DistanceSquaredVec4(const vec4 *const restrict v1, const vec4 *const restrict v2){
	vec4 dist;
	vec4SubtractVec4FromOut(v1, v2, &dist);
	return(vec4MagnitudeSquaredVec4(&dist));
}

// Find the squared distance between two vec4s!
float vec4DistanceSquaredVec4C(const vec4 v1, const vec4 v2){
	const vec4 dist = vec4SubtractVec4FromC(v1, v2);
	return(vec4MagnitudeSquaredVec4C(dist));
}


// Find the dot product of two vec4s stored as four floats!
float vec4Dot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2){
	return(x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2);
}

// Find the dot product of a vec4 and one stored as four floats!
float vec4DotVec4Float(const vec4 *const restrict v, const float x, const float y, const float z, const float w){
	return(v->x * x + v->y * y + v->z * z + v->w * w);
}

// Find the dot product of a vec4 and one stored as four floats!
float vec4DotVec4FloatC(const vec4 v, const float x, const float y, const float z, const float w){
	return(v.x * x + v.y * y + v.z * z + v.w * w);
}

// Find the dot product of two vec4s!
float vec4DotVec4(const vec4 *const restrict v1, const vec4 *const restrict v2){
	return(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w);
}

// Find the dot product of two vec4s!
float vec4DotVec4C(const vec4 v1, const vec4 v2){
	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}


// Normalize a vec4 stored as four floats and store the result in "out"!
void vec4Normalize(const float x, const float y, const float z, const float w, vec4 *out){
	const float magnitude = invSqrt(x * x + y * y + z * z + w * w);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

// Normalize a vec4 stored as four floats and store the result in "out"!
void vec4NormalizeFast(const float x, const float y, const float z, const float w, vec4 *out){
	const float magnitude = invSqrtFast(x * x + y * y + z * z + w * w);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

// Normalize a vec4 stored as four floats!
vec4 vec4NormalizeC(const float x, const float y, const float z, const float w){
	const float magnitude = invSqrt(x * x + y * y + z * z + w * w);
	vec4 v;

	v.x = x * magnitude;
	v.y = y * magnitude;
	v.z = z * magnitude;
	v.w = w * magnitude;

	return(v);
}

// Normalize a vec4 stored as four floats!
vec4 vec4NormalizeFastC(const float x, const float y, const float z, const float w){
	const float magnitude = invSqrtFast(x * x + y * y + z * z + w * w);
	vec4 v;

	v.x = x * magnitude;
	v.y = y * magnitude;
	v.z = z * magnitude;
	v.w = w * magnitude;

	return(v);
}

// Normalize a vec4!
void vec4NormalizeVec4(vec4 *const restrict v){
	const float magnitude = invSqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);

	v->x *= magnitude;
	v->y *= magnitude;
	v->z *= magnitude;
	v->w *= magnitude;
}

// Normalize a vec4!
void vec4NormalizeVec4Fast(vec4 *const restrict v){
	const float magnitude = invSqrtFast(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);

	v->x *= magnitude;
	v->y *= magnitude;
	v->z *= magnitude;
	v->w *= magnitude;
}

// Normalize a vec4 and store the result in "out"!
void vec4NormalizeVec4Out(const vec4 *const restrict v, vec4 *out){
	const float magnitude = invSqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
	out->z = v->z * magnitude;
	out->w = v->w * magnitude;
}

// Normalize a vec4 and store the result in "out"!
void vec4NormalizeVec4FastOut(const vec4 *const restrict v, vec4 *out){
	const float magnitude = invSqrtFast(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
	out->z = v->z * magnitude;
	out->w = v->w * magnitude;
}

// Normalize a vec4!
vec4 vec4NormalizeVec4C(vec4 v){
	const float magnitude = invSqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);

	v.x *= magnitude;
	v.y *= magnitude;
	v.z *= magnitude;
	v.w *= magnitude;

	return(v);
}

// Normalize a vec4!
vec4 vec4NormalizeVec4FastC(vec4 v){
	const float magnitude = invSqrtFast(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);

	v.x *= magnitude;
	v.y *= magnitude;
	v.z *= magnitude;
	v.w *= magnitude;

	return(v);
}

// Negate the values of a vec4!
void vec4Negate(vec4 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
	v->w = -v->w;
}

// Negate the values of a vec4 and store the result in "out"!
void vec4NegateOut(const vec4 *const restrict v, vec4 *out){
	out->x = -v->x;
	out->y = -v->y;
	out->z = -v->z;
	out->w = -v->w;
}

// Negate the values of a vec4!
vec4 vec4NegateC(vec4 v){
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	v.w = -v.w;

	return(v);
}


// Convert a vec4 from degrees to radians!
void vec4DegToRad(vec4 *const restrict v){
	v->x *= DEG_TO_RAD;
	v->y *= DEG_TO_RAD;
	v->z *= DEG_TO_RAD;
	v->w *= DEG_TO_RAD;
}

// Convert a vec4 from radians to degrees!
void vec4RadToDeg(vec4 *const restrict v){
	v->x *= RAD_TO_DEG;
	v->y *= RAD_TO_DEG;
	v->z *= RAD_TO_DEG;
	v->w *= RAD_TO_DEG;
}


// Perform linear interpolation between two vec4s and store the result in "out"!
void vec4Lerp(const vec4 *const restrict v1, const vec4 *const restrict v2, const float time, vec4 *out){
	out->x = floatLerpFast(v1->x, v2->x, time);
	out->y = floatLerpFast(v1->y, v2->y, time);
	out->z = floatLerpFast(v1->z, v2->z, time);
	out->w = floatLerpFast(v1->w, v2->w, time);
}

// Perform linear interpolation between two vec4s!
vec4 vec4LerpC(vec4 v1, const vec4 v2, const float time){
	v1.x = floatLerpFast(v1.x, v2.x, time);
	v1.y = floatLerpFast(v1.y, v2.y, time);
	v1.z = floatLerpFast(v1.z, v2.z, time);
	v1.w = floatLerpFast(v1.w, v2.w, time);

	return(v1);
}

/*
** Perform linear interpolation between two vec4s and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
void vec4LerpDiff(const vec4 *const restrict v, const vec4 *offset, const float time, vec4 *out){
	out->x = floatLerpDiffFast(v->x, offset->x, time);
	out->y = floatLerpDiffFast(v->y, offset->y, time);
	out->z = floatLerpDiffFast(v->z, offset->z, time);
	out->w = floatLerpDiffFast(v->w, offset->w, time);
}

/*
** Perform linear interpolation between two vec4s!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
vec4 vec4LerpDiffC(vec4 v, const vec4 offset, const float time){
	v.x = floatLerpDiffFast(v.x, offset.x, time);
	v.y = floatLerpDiffFast(v.y, offset.y, time);
	v.z = floatLerpDiffFast(v.z, offset.z, time);
	v.w = floatLerpDiffFast(v.w, offset.w, time);

	return(v);
}


/*
** Compare two vec4s to find the minimum value per axis
** and return a vec4 composed of these minima in "out".
*/
void vec4Min(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = floatMinFast(v1->x, v2->x);
	out->y = floatMinFast(v1->y, v2->y);
	out->z = floatMinFast(v1->z, v2->z);
	out->w = floatMinFast(v1->w, v2->w);
}

/*
** Compare two vec4s to find the minimum value per
** axis and return a vec4 composed of these minima.
*/
vec4 vec4MinC(vec4 v1, const vec4 v2){
	v1.x = floatMinFast(v1.x, v2.x);
	v1.y = floatMinFast(v1.y, v2.y);
	v1.z = floatMinFast(v1.z, v2.z);
	v1.w = floatMinFast(v1.w, v2.w);

	return(v1);
}

/*
** Compare two vec4s to find the maximum value per axis
** and return a vec4 composed of these maxima in "out".
*/
void vec4Max(const vec4 *const restrict v1, const vec4 *const restrict v2, vec4 *out){
	out->x = floatMaxFast(v1->x, v2->x);
	out->y = floatMaxFast(v1->y, v2->y);
	out->z = floatMaxFast(v1->z, v2->z);
	out->w = floatMaxFast(v1->w, v2->w);
}

/*
** Compare two vec4s to find the maximum value per
** axis and return a vec4 composed of these maxima.
*/
vec4 vec4MaxC(vec4 v1, const vec4 v2){
	v1.x = floatMaxFast(v1.x, v2.x);
	v1.y = floatMaxFast(v1.y, v2.y);
	v1.z = floatMaxFast(v1.z, v2.z);
	v1.w = floatMaxFast(v1.w, v2.w);

	return(v1);
}