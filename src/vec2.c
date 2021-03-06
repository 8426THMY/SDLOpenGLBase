#include "vec2.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


// Initialize the vec2's values to 0!
void vec2InitZero(vec2 *const restrict v){
	memset(v, 0.f, sizeof(*v));
}

// Initialize the vec2's values to 0!
vec2 vec2InitZeroC(){
	vec2 v;
	memset(&v, 0.f, sizeof(v));

	return(v);
}

// Initialize the vec2's values to the ones specified!
void vec2InitSet(vec2 *const restrict v, const float x, const float y){
	v->x = x;
	v->y = y;
}

// Initialize the vec2's values to the ones specified!
vec2 vec2InitSetC(const float x, const float y){
	const vec2 v = {.x = x, .y = y};

	return(v);
}


// Add a vec2 stored as two floats to "v"!
void vec2Add(vec2 *const restrict v, const float x, const float y){
	v->x += x;
	v->y += y;
}

// Add a vec2 stored as two floats to "v" and store the result in "out"!
void vec2AddOut(const vec2 *const restrict v, const float x, const float y, vec2 *const restrict out){
	out->x = v->x + x;
	out->y = v->y + y;
}

// Add a vec2 stored as two floats to "v"!
vec2 vec2AddC(vec2 v, const float x, const float y){
	v.x += x;
	v.y += y;

	return(v);
}

// Add "x" to "v"!
void vec2AddS(vec2 *const restrict v, const float x){
	v->x += x;
	v->y += x;
}

// Add "x" to "v" and store the result in "out"!
void vec2AddSOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	out->x = v->x + x;
	out->y = v->y + x;
}

// Add "x" to "v"!
vec2 vec2AddSC(vec2 v, const float x){
	v.x += x;
	v.y += x;

	return(v);
}

// Add "v2" to "v1"!
void vec2AddVec2(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x += v2->x;
	v1->y += v2->y;
}

// Add "v2" to "v1" and store the result in "out"!
void vec2AddVec2Out(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
}

// Add "v2" to "v1"!
vec2 vec2AddVec2C(vec2 v1, const vec2 v2){
	v1.x += v2.x;
	v1.y += v2.y;

	return(v1);
}

// Subtract a vec2 stored as two floats from "v"!
void vec2Subtract(vec2 *const restrict v, const float x, const float y){
	v->x -= x;
	v->y -= y;
}

// Subtract a vec2 stored as two floats from "v" and store the result in "out"!
void vec2SubtractOut(const vec2 *const restrict v, const float x, const float y, vec2 *const restrict out){
	out->x = v->x - x;
	out->y = v->y - y;
}

// Subtract a vec2 stored as two floats from "v"!
vec2 vec2SubtractC(vec2 v, const float x, const float y){
	v.x -= x;
	v.y -= y;

	return(v);
}

// Subtract "v" from a vec2 stored as two floats!
void vec2SubtractFrom(vec2 *const restrict v, const float x, const float y){
	v->x = x - v->x;
	v->y = y - v->y;
}

// Subtract "v" from a vec2 stored as two floats and store the result in "out"!
void vec2SubtractFromOut(const vec2 *const restrict v, const float x, const float y, vec2 *const restrict out){
	out->x = x - v->x;
	out->y = y - v->y;
}

// Subtract "v" from a vec2 stored as two floats!
vec2 vec2SubtractFromC(vec2 v, const float x, const float y){
	v.x = x - v.x;
	v.y = y - v.y;

	return(v);
}

// Subtract "x" from "v"!
void vec2SubtractSFrom(vec2 *const restrict v, const float x){
	v->x -= x;
	v->y -= x;
}

// Subtract "x" from "v" and store the result in "out"!
void vec2SubtractSFromOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	out->x = v->x - x;
	out->y = v->y - x;
}

// Subtract "x" from "v"!
vec2 vec2SubtractSFromC(vec2 v, const float x){
	v.x -= x;
	v.y -= x;

	return(v);
}

// Subtract "v" from "x"!
void vec2SubtractFromS(vec2 *const restrict v, const float x){
	v->x = x - v->x;
	v->y = x - v->y;
}

// Subtract "v" from "x" and store the result in "out"!
void vec2SubtractFromSOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	out->x = x - v->x;
	out->y = x - v->y;
}

// Subtract "v" from "x"!
vec2 vec2SubtractFromSC(vec2 v, const float x){
	v.x = x - v.x;
	v.y = x - v.y;

	return(v);
}

// Subtract "v2" from "v1"!
void vec2SubtractVec2From(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
}

// Subtract "v1" from "v2"!
void vec2SubtractFromVec2(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x = v2->x - v1->x;
	v1->y = v2->y - v1->y;
}

// Subtract "v2" from "v1" and store the result in "out"!
void vec2SubtractVec2FromOut(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
}

// Subtract "v2" from "v1"!
vec2 vec2SubtractVec2FromC(vec2 v1, const vec2 v2){
	v1.x -= v2.x;
	v1.y -= v2.y;

	return(v1);
}


// Multiply "v" by "x"!
void vec2MultiplyS(vec2 *const restrict v, const float x){
	v->x *= x;
	v->y *= x;
}

// Multiply "v" by "x" and store the result in "out"!
void vec2MultiplySOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	out->x = v->x * x;
	out->y = v->y * x;
}

// Multiply "v" by "x"!
vec2 vec2MultiplySC(vec2 v, const float x){
	v.x *= x;
	v.y *= x;

	return(v);
}

// Multiply "v1" by "v2"!
void vec2MultiplyVec2(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
}

// Multiply "v1" by "v2" and store the result in "out"!
void vec2MultiplyVec2Out(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = v1->x * v2->x;
	out->y = v1->y * v2->y;
}

// Multiply "v1" by "v2"!
vec2 vec2MultiplyVec2C(vec2 v1, const vec2 v2){
	v1.x *= v2.x;
	v1.y *= v2.y;

	return(v1);
}

// Divide "v" by "x"!
void vec2DivideByS(vec2 *const restrict v, const float x){
	const float invX = 1.f / x;

	v->x *= invX;
	v->y *= invX;
}

// Divide "v" by "x" and store the result in "out"!
void vec2DivideBySOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	const float invX = 1.f / x;

	out->x = v->x * invX;
	out->y = v->y * invX;
}

// Divide "v" by "x"!
vec2 vec2DivideBySC(vec2 v, const float x){
	const float invX = 1.f / x;

	v.x *= invX;
	v.y *= invX;

	return(v);
}

// Divide "x" by "v"!
void vec2DivideSBy(vec2 *const restrict v, const float x){
	v->x = (v->x != 0.f) ? x / v->x : 0.f;
	v->y = (v->y != 0.f) ? x / v->y : 0.f;
}

// Divide "x" by "v" and store the result in "out"!
void vec2DivideSByOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	out->x = (v->x != 0.f) ? x / v->x : 0.f;
	out->y = (v->y != 0.f) ? x / v->y : 0.f;
}

// Divide "x" by "v"!
vec2 vec2DivideSByC(vec2 v, const float x){
	v.x = (v.x != 0.f) ? x / v.x : 0.f;
	v.y = (v.y != 0.f) ? x / v.y : 0.f;

	return(v);
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec2DivideSByFast(vec2 *const restrict v, const float x){
	v->x = x / v->x;
	v->y = x / v->y;
}

/*
** Divide "x" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec2DivideSByFastOut(const vec2 *const restrict v, const float x, vec2 *const restrict out){
	out->x = x / v->x;
	out->y = x / v->y;
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
vec2 vec2DivideSByFastC(vec2 v, const float x){
	v.x = x / v.x;
	v.y = x / v.y;

	return(v);
}

// Divide "v" by "v"!
void vec2DivideByVec2(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	v1->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
}

// Divide "v2" by "v1"!
void vec2DivideVec2By(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x = (v1->x != 0.f) ? v2->x / v1->x : 0.f;
	v1->y = (v1->y != 0.f) ? v2->y / v1->y : 0.f;
}

// Divide "v1" by "v2" and store the result in "out"!
void vec2DivideByVec2Out(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	out->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
}

// Divide "v1" by "v2"!
vec2 vec2DivideByVec2C(vec2 v1, const vec2 v2){
	v1.x = (v2.x != 0.f) ? v1.x / v2.x : 0.f;
	v1.y = (v2.y != 0.f) ? v1.y / v2.y : 0.f;

	return(v1);
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec2DivideByVec2Fast(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
}

/*
** Divide "v2" by "v1"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec2DivideVec2ByFast(vec2 *const restrict v1, const vec2 *const restrict v2){
	v1->x = v2->x / v1->x;
	v1->y = v2->y / v1->y;
}

/*
** Divide "v1" by "v2" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec2DivideByVec2FastOut(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = v1->x / v2->x;
	out->y = v1->y / v2->y;
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
vec2 vec2DivideByVec2FastC(vec2 v1, const vec2 v2){
	v1.x = v1.x / v2.x;
	v1.y = v1.y / v2.y;

	return(v1);
}


// Multiply "u" by "x" and add it to "v"!
void vec2Fmaf(const float x, const vec2 *const restrict u, vec2 *const restrict v){
	#ifdef FP_FAST_FMAF
	v->x = fmaf(x, u->x, v->x);
	v->y = fmaf(x, u->y, v->y);
	#else
	v->x += x*u->x;
	v->y += x*u->y;
	#endif
}

// Multiply "u" by "x", add it to "v" and store the result in "out"!
void vec2FmafOut(const float x, const vec2 *const restrict u, const vec2 *const restrict v, vec2 *const restrict out){
	#ifdef FP_FAST_FMAF
	out->x = fmaf(x, u->x, v->x);
	out->y = fmaf(x, u->y, v->y);
	#else
	out->x = x*u->x + v->x;
	out->y = x*u->y + v->y;
	#endif
}

// Multiply "u" by "x", add it to "v" and return the result!
vec2 vec2FmafC(const float x, const vec2 u, const vec2 v){
	const vec2 out = {
	#ifdef FP_FAST_FMAF
		.x = fmaf(x, u.x, v.x),
		.y = fmaf(x, u.y, v.y)
	#else
		.x = x*u.x + v.x,
		.y = x*u.y + v.y
	#endif
	};
	return(out);
}


// Find the magnitude (length) of a vec2 stored as two floats!
float vec2Magnitude(const float x, const float y){
	return(sqrtf(vec2MagnitudeSquared(x, y)));
}

// Find the magnitude (length) of a vec2!
float vec2MagnitudeVec2(const vec2 *const restrict v){
	return(sqrtf(vec2MagnitudeSquaredVec2(v)));
}

// Find the magnitude (length) of a vec2!
float vec2MagnitudeVec2C(const vec2 v){
	return(sqrtf(vec2MagnitudeSquaredVec2C(v)));
}

// Find the squared magnitude of a vec2 stored as two floats!
float vec2MagnitudeSquared(const float x, const float y){
	return(x * x + y * y);
}

// Find the squared magnitude of a vec2!
float vec2MagnitudeSquaredVec2(const vec2 *const restrict v){
	return(v->x * v->x + v->y * v->y);
}

// Find the squared magnitude of a vec2!
float vec2MagnitudeSquaredVec2C(const vec2 v){
	return(v.x * v.x + v.y * v.y);
}

// Find the distance between a vec2 and one stored as two floats!
float vec2Distance(const vec2 *const restrict v, const float x, const float y){
	vec2 dist;
	vec2SubtractFromOut(v, x, y, &dist);
	return(vec2MagnitudeVec2(&dist));
}

// Find the distance between a vec2 and one stored as two floats!
float vec2DistanceC(const vec2 v, const float x, const float y){
	const vec2 dist = vec2SubtractFromC(v, x, y);
	return(vec2MagnitudeVec2C(dist));
}

// Find the squared distance between a vec2 and one stored as two floats!
float vec2DistanceSquared(const vec2 *const restrict v, const float x, const float y){
	vec2 dist;
	vec2SubtractFromOut(v, x, y, &dist);
	return(vec2MagnitudeSquaredVec2(&dist));
}

// Find the squared distance between a vec2 and one stored as two floats!
float vec2DistanceSquaredC(const vec2 v, const float x, const float y){
	const vec2 dist = vec2SubtractFromC(v, x, y);
	return(vec2MagnitudeSquaredVec2C(dist));
}

// Find the distance between two vec2s!
float vec2DistanceVec2(const vec2 *const restrict v1, const vec2 *const restrict v2){
	vec2 dist;
	vec2SubtractVec2FromOut(v1, v2, &dist);
	return(vec2MagnitudeVec2(&dist));
}

// Find the distance between two vec2s!
float vec2DistanceVec2C(const vec2 v1, const vec2 v2){
	const vec2 dist = vec2SubtractVec2FromC(v1, v2);
	return(vec2MagnitudeVec2C(dist));
}

// Find the squared distance between two vec2s!
float vec2DistanceSquaredVec2(const vec2 *const restrict v1, const vec2 *const restrict v2){
	vec2 dist;
	vec2SubtractVec2FromOut(v1, v2, &dist);
	return(vec2MagnitudeSquaredVec2(&dist));
}

// Find the squared distance between two vec2s!
float vec2DistanceSquaredVec2C(const vec2 v1, const vec2 v2){
	const vec2 dist = vec2SubtractVec2FromC(v1, v2);
	return(vec2MagnitudeSquaredVec2C(dist));
}


// Find the dot product of two vec2s stored as two floats!
float vec2Dot(const float x1, const float y1, const float x2, const float y2){
	return(x1 * x2 + y1 * y2);
}

// Find the dot product of a vec2 and one stored as two floats!
float vec2DotVec2Float(const vec2 *const restrict v, const float x, const float y){
	return(v->x * x + v->y * y);
}

// Find the dot product of a vec2 and one stored as two floats!
float vec2DotVec2FloatC(const vec2 v, const float x, const float y){
	return(v.x * x + v.y * y);
}

// Find the dot product of two vec2s!
float vec2DotVec2(const vec2 *const restrict v1, const vec2 *const restrict v2){
	return(v1->x * v2->x + v1->y * v2->y);
}

// Find the dot product of two vec2s!
float vec2DotVec2C(const vec2 v1, const vec2 v2){
	return(v1.x * v2.x + v1.y * v2.y);
}


// Normalize a vec2 stored as two floats and store the result in "out"!
void vec2Normalize(const float x, const float y, vec2 *const restrict out){
	const float magnitude = invSqrt(x * x + y * y);

	out->x = x * magnitude;
	out->y = y * magnitude;
}

// Normalize a vec2 stored as two floats and store the result in "out"!
void vec2NormalizeFast(const float x, const float y, vec2 *const restrict out){
	const float magnitude = invSqrtFast(x * x + y * y);

	out->x = x * magnitude;
	out->y = y * magnitude;
}

// Normalize a vec2 stored as two floats!
vec2 vec2NormalizeC(const float x, const float y){
	const float magnitude = invSqrt(x * x + y * y);
	vec2 v;

	v.x = x * magnitude;
	v.y = y * magnitude;

	return(v);
}

// Normalize a vec2 stored as two floats!
vec2 vec2NormalizeFastC(const float x, const float y){
	const float magnitude = invSqrtFast(x * x + y * y);
	vec2 v;

	v.x = x * magnitude;
	v.y = y * magnitude;

	return(v);
}

// Normalize a vec2!
void vec2NormalizeVec2(vec2 *const restrict v){
	const float magnitude = invSqrt(v->x * v->x + v->y * v->y);

	v->x *= magnitude;
	v->y *= magnitude;
}

// Normalize a vec2!
void vec2NormalizeVec2Fast(vec2 *const restrict v){
	const float magnitude = invSqrtFast(v->x * v->x + v->y * v->y);

	v->x *= magnitude;
	v->y *= magnitude;
}

// Normalize a vec2 and store the result in "out"!
void vec2NormalizeVec2Out(const vec2 *const restrict v, vec2 *const restrict out){
	const float magnitude = invSqrt(v->x * v->x + v->y * v->y);

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
}

// Normalize a vec2 and store the result in "out"!
void vec2NormalizeVec2FastOut(const vec2 *const restrict v, vec2 *const restrict out){
	const float magnitude = invSqrtFast(v->x * v->x + v->y * v->y);

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
}

// Normalize a vec2!
vec2 vec2NormalizeVec2C(vec2 v){
	const float magnitude = invSqrt(v.x * v.x + v.y * v.y);

	v.x *= magnitude;
	v.y *= magnitude;

	return(v);
}

// Normalize a vec2!
vec2 vec2NormalizeVec2FastC(vec2 v){
	const float magnitude = invSqrtFast(v.x * v.x + v.y * v.y);

	v.x *= magnitude;
	v.y *= magnitude;

	return(v);
}

// Negate the values of a vec2!
void vec2Negate(vec2 *const restrict v){
	v->x = -v->x;
	v->y = -v->y;
}

// Negate the values of a vec2 and store the result in "out"!
void vec2NegateOut(const vec2 *const restrict v, vec2 *const restrict out){
	out->x = -v->x;
	out->y = -v->y;
}

// Negate the values of a vec2!
vec2 vec2NegateC(vec2 v){
	v.x = -v.x;
	v.y = -v.y;

	return(v);
}


/*
** Generate a vector that is orthogonal to the
** input vector and store the result in "out"!
*/
void vec2Orthogonal(const vec2 *const restrict v, vec2 *const restrict out){
	vec2InitSet(out, -v->y, v->x);
}

// Generate a vector that is orthogonal to the input vector.
vec2 vec2OrthogonalC(const vec2 v){
	return(vec2InitSetC(-v.y, v.x));
}


// Convert a vec2 from degrees to radians!
void vec2DegToRad(vec2 *const restrict v){
	v->x *= DEG_TO_RAD;
	v->y *= DEG_TO_RAD;
}

// Convert a vec2 from radians to degrees!
void vec2RadToDeg(vec2 *const restrict v){
	v->x *= RAD_TO_DEG;
	v->y *= RAD_TO_DEG;
}


// Perform linear interpolation between two vec2s and store the result in "out"!
void vec2Lerp(const vec2 *const restrict v1, const vec2 *const restrict v2, const float time, vec2 *const restrict out){
	out->x = lerpFloatFast(v1->x, v2->x, time);
	out->y = lerpFloatFast(v1->y, v2->y, time);
}

// Perform linear interpolation between two vec2s!
vec2 vec2LerpC(vec2 v1, const vec2 v2, const float time){
	v1.x = lerpFloatFast(v1.x, v2.x, time);
	v1.y = lerpFloatFast(v1.y, v2.y, time);

	return(v1);
}

/*
** Perform linear interpolation between two vec2s and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
void vec2LerpDiff(const vec2 *const restrict v, const vec2 *offset, const float time, vec2 *const restrict out){
	out->x = lerpDiffFast(v->x, offset->x, time);
	out->y = lerpDiffFast(v->y, offset->y, time);
}

/*
** Perform linear interpolation between two vec2s!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
vec2 vec2LerpDiffC(vec2 v, const vec2 offset, const float time){
	v.x = lerpDiffFast(v.x, offset.x, time);
	v.y = lerpDiffFast(v.y, offset.y, time);

	return(v);
}


/*
** Compare two vec2s to find the minimum value per axis
** and return a vec2 composed of these minima in "out".
*/
void vec2Min(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = minFloatFast(v1->x, v2->x);
	out->y = minFloatFast(v1->y, v2->y);
}

/*
** Compare two vec2s to find the minimum value per
** axis and return a vec2 composed of these minima.
*/
vec2 vec2MinC(vec2 v1, const vec2 v2){
	v1.x = minFloatFast(v1.x, v2.x);
	v1.y = minFloatFast(v1.y, v2.y);

	return(v1);
}

/*
** Compare two vec2s to find the maximum value per axis
** and return a vec2 composed of these maxima in "out".
*/
void vec2Max(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out){
	out->x = maxFloatFast(v1->x, v2->x);
	out->y = maxFloatFast(v1->y, v2->y);
}

/*
** Compare two vec2s to find the maximum value per
** axis and return a vec2 composed of these maxima.
*/
vec2 vec2MaxC(vec2 v1, const vec2 v2){
	v1.x = maxFloatFast(v1.x, v2.x);
	v1.y = maxFloatFast(v1.y, v2.y);

	return(v1);
}