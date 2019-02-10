#include "vec3.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


//Initialize the vec3's values to 0!
void vec3InitZero(vec3 *v){
	memset(v, 0.f, sizeof(*v));
}

//Initialize the vec3's values to the ones specified!
void vec3InitSet(vec3 *v, const float x, const float y, const float z){
	v->x = x;
	v->y = y;
	v->z = z;
}


//Add a vec3 stored as three floats to "v"!
void vec3Add(vec3 *v, const float x, const float y, const float z){
	v->x += x;
	v->y += y;
	v->z += z;
}

//Add a vec3 stored as three floats to "v" and store the result in "out"!
void vec3AddOut(const vec3 *v, const float x, const float y, const float z, vec3 *out){
	out->x = v->x + x;
	out->y = v->y + y;
	out->z = v->z + z;
}

//Add "x" to "v"!
void vec3AddS(vec3 *v, const float x){
	v->x += x;
	v->y += x;
	v->z += x;
}

//Add "x" to "v" and store the result in "out"!
void vec3AddSOut(const vec3 *v, const float x, vec3 *out){
	out->x = v->x + x;
	out->y = v->y + x;
	out->z = v->z + x;
}

//Add "v2" to "v1"!
void vec3AddVec3(vec3 *v1, const vec3 *v2){
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
}

//Add "v2" to "v1" and store the result in "out"!
void vec3AddVec3Out(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
}

//Subtract a vec3 stored as three floats from "v"!
void vec3Subtract(vec3 *v, const float x, const float y, const float z){
	v->x -= x;
	v->y -= y;
	v->z -= z;
}

//Subtract a vec3 stored as three floats from "v" and store the result in "out"!
void vec3SubtractOut(const vec3 *v, const float x, const float y, const float z, vec3 *out){
	out->x = v->x - x;
	out->y = v->y - y;
	out->z = v->z - z;
}

//Subtract "v" from a vec3 stored as three floats!
void vec3SubtractFrom(vec3 *v, const float x, const float y, const float z){
	v->x = x - v->x;
	v->y = y - v->y;
	v->z = z - v->z;
}

//Subtract "v" from a vec3 stored as three floats and store the result in "out"!
void vec3SubtractFromOut(const vec3 *v, const float x, const float y, const float z, vec3 *out){
	out->x = x - v->x;
	out->y = y - v->y;
	out->z = z - v->z;
}

//Subtract "x" from "v"!
void vec3SubtractSFrom(vec3 *v, const float x){
	v->x -= x;
	v->y -= x;
	v->z -= x;
}

//Subtract "x" from "v" and store the result in "out"!
void vec3SubtractSFromOut(const vec3 *v, const float x, vec3 *out){
	out->x = v->x - x;
	out->y = v->y - x;
	out->z = v->z - x;
}

//Subtract "v" from "x"!
void vec3SubtractFromS(vec3 *v, const float x){
	v->x = x - v->x;
	v->y = x - v->y;
	v->z = x - v->z;
}

//Subtract "v" from "x" and store the result in "out"!
void vec3SubtractFromSOut(const vec3 *v, const float x, vec3 *out){
	out->x = x - v->x;
	out->y = x - v->y;
	out->z = x - v->z;
}

//Subtract "v2" from "v1"!
void vec3SubtractVec3From(vec3 *v1, const vec3 *v2){
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
}

//Subtract "v1" from "v2"!
void vec3SubtractFromVec3(vec3 *v1, const vec3 *v2){
	v1->x = v2->x - v1->x;
	v1->y = v2->y - v1->y;
	v1->z = v2->z - v1->z;
}

//Subtract "v2" from "v1" and store the result in "out"!
void vec3SubtractVec3FromOut(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
}


//Multiply "v" by "x"!
void vec3MultiplyS(vec3 *v, const float x){
	v->x *= x;
	v->y *= x;
	v->z *= x;
}

//Multiply "v" by "x" and store the result in "out"!
void vec3MultiplySOut(const vec3 *v, const float x, vec3 *out){
	out->x = v->x * x;
	out->y = v->y * x;
	out->z = v->z * x;
}

//Multiply "v1" by "v2"!
void vec3MultiplyVec3(vec3 *v1, const vec3 *v2){
	v1->x *= v2->x;
	v1->y *= v2->y;
	v1->z *= v2->z;
}

//Multiply "v1" by "v2" and store the result in "out"!
void vec3MultiplyVec3Out(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = v1->x * v2->x;
	out->y = v1->y * v2->y;
	out->z = v1->z * v2->z;
}

//Divide "v" by "x"!
void vec3DivideByS(vec3 *v, const float x){
	const float invX = 1.f / x;

	v->x *= invX;
	v->y *= invX;
	v->z *= invX;
}

//Divide "v" by "x" and store the result in "out"!
void vec3DivideBySOut(const vec3 *v, const float x, vec3 *out){
	const float invX = 1.f / x;

	out->x = v->x * invX;
	out->y = v->y * invX;
	out->z = v->z * invX;
}

//Divide "x" by "v"!
void vec3DivideSBy(vec3 *v, const float x){
	v->x = (v->x != 0.f) ? x / v->x : 0.f;
	v->y = (v->y != 0.f) ? x / v->y : 0.f;
	v->z = (v->z != 0.f) ? x / v->z : 0.f;
}

//Divide "x" by "v" and store the result in "out"!
void vec3DivideSByOut(const vec3 *v, const float x, vec3 *out){
	out->x = (v->x != 0.f) ? x / v->x : 0.f;
	out->y = (v->y != 0.f) ? x / v->y : 0.f;
	out->z = (v->z != 0.f) ? x / v->z : 0.f;
}

/*
** Divide "x" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec3DivideSByFast(vec3 *v, const float x){
	v->x = x / v->x;
	v->y = x / v->y;
	v->z = x / v->z;
}

/*
** Divide "x" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec3DivideSByFastOut(const vec3 *v, const float x, vec3 *out){
	out->x = x / v->x;
	out->y = x / v->y;
	out->z = x / v->z;
}

//Divide "v1" by "v2"!
void vec3DivideByVec3(vec3 *v1, const vec3 *v2){
	v1->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	v1->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	v1->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
}

//Divide "v2" by "v1"!
void vec3DivideVec3By(vec3 *v1, const vec3 *v2){
	v1->x = (v2->x != 0.f) ? v1->x / v2->x : 0.f;
	v1->y = (v2->y != 0.f) ? v1->y / v2->y : 0.f;
	v1->z = (v2->z != 0.f) ? v1->z / v2->z : 0.f;
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec3DivideByVec3Fast(vec3 *v1, const vec3 *v2){
	v1->x /= v2->x;
	v1->y /= v2->y;
	v1->z /= v2->z;
}

/*
** Divide "v1" by "v2"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void vec3DivideVec3ByFast(vec3 *v1, const vec3 *v2){
	v1->x = v2->x / v1->x;
	v1->y = v2->y / v1->y;
	v1->z = v2->z / v1->z;
}

/*
** Divide "v1" by "v2" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec3DivideByVec3FastOut(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = v1->x / v2->x;
	out->y = v1->y / v2->y;
	out->z = v1->z / v2->z;
}


//Find the norm of a vec3 stored as three floats!
float vec3Norm(const float x, const float y, const float z){
	return(x * x + y * y + z * z);
}

//Find the norm of a vec3!
float vec3NormVec3(const vec3 *v){
	return(v->x * v->x + v->y * v->y + v->z * v->z);
}


//Find the magnitude (length) of a vec3 stored as three floats!
float vec3Magnitude(const float x, const float y, const float z){
	return(sqrtf(vec3Norm(x, y, z)));
}

//Find the magnitude (length) of a vec3!
float vec3MagnitudeVec3(const vec3 *v){
	return(sqrtf(vec3NormVec3(v)));
}

//Find the distance between a vec3 and one stored as three floats!
float vec3DistanceSquared(const vec3 *v, const float x, const float y, const float z){
	return(vec3Norm(x - v->x, y - v->y, z - v->z));
}

//Find the distance between two vec3s!
float vec3DistanceSquaredVec3(const vec3 *v1, const vec3 *v2){
	return(vec3Norm(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z));
}


//Find the dot product of two vec3s stored as three floats!
float vec3Dot(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2){
	return(x1 * x2 + y1 * y2 + z1 * z2);
}

//Find the dot product of a vec3 and one stored as three floats!
float vec3DotVec3Float(const vec3 *v, const float x, const float y, const float z){
	return(v->x * x + v->y * y + v->z * z);
}

//Find the dot product of two vec3s!
float vec3DotVec3(const vec3 *v1, const vec3 *v2){
	return(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}


//Find the cross product of two vec3s stored as three floats and store the result in "out"!
void vec3Cross(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, vec3 *out){
	out->x = y1 * z2 - z1 * y2;
	out->y = z1 * x2 - x1 * z2;
	out->z = x1 * y2 - y1 * x2;
}

//Find the cross product of a vec3 and one stored as three floats and store the result in "out"!
void vec3CrossVec3Float(const vec3 *v, const float x, const float y, const float z, vec3 *out){
	out->x = v->y * z - v->z * y;
	out->y = v->z * x - v->x * z;
	out->z = v->x * y - v->y * x;
}

//Find the cross product of a vec3 stored as three floats and a normal one and store the result in "out"!
void vec3CrossFloatVec3(const float x, const float y, const float z, const vec3 *v, vec3 *out){
	out->x = y * v->z - z * v->y;
	out->y = z * v->x - x * v->z;
	out->z = x * v->y - y * v->x;
}

//Find the cross product of two vec3s and store the result in "out"!
void vec3CrossVec3(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = v1->y * v2->z - v1->z * v2->y;
	out->y = v1->z * v2->x - v1->x * v2->z;
	out->z = v1->x * v2->y - v1->y * v2->x;
}


//Normalize a vec3 stored as three floats and store the result in "out"!
void vec3Normalize(const float x, const float y, const float z, vec3 *out){
	const float magnitude = fastInvSqrt(vec3Norm(x, y, z));

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
}

//Normalize a vec3!
void vec3NormalizeVec3(vec3 *v){
	const float magnitude = fastInvSqrt(vec3NormVec3(v));

	v->x = v->x * magnitude;
	v->y = v->y * magnitude;
	v->z = v->z * magnitude;
}

//Normalize a vec3 and store the result in "out"!
void vec3NormalizeVec3Out(const vec3 *v, vec3 *out){
	const float magnitude = fastInvSqrt(vec3NormVec3(v));

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
	out->z = v->z * magnitude;
}

//Negate the values of a vec3!
void vec3Negate(vec3 *v){
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

//Negate the values of a vec3 and store the result in "out"!
void vec3NegateOut(const vec3 *v, vec3 *out){
	out->x = -v->x;
	out->y = -v->y;
	out->z = -v->z;
}


//Convert a vec3 from degrees to radians!
void vec3DegToRad(vec3 *v){
	v->x *= DEG_TO_RAD;
	v->y *= DEG_TO_RAD;
	v->z *= DEG_TO_RAD;
}

//Convert a vec3 from radians to degrees!
void vec3RadToDeg(vec3 *v){
	v->x *= RAD_TO_DEG;
	v->y *= RAD_TO_DEG;
	v->z *= RAD_TO_DEG;
}


//Perform linear interpolation between two vec3s and store the result in "out"!
void vec3Lerp(const vec3 *v1, const vec3 *v2, const float time, vec3 *out){
	out->x = lerpNum(v1->x, v2->x, time);
	out->y = lerpNum(v1->y, v2->y, time);
	out->z = lerpNum(v1->z, v2->z, time);
}

/*
** Perform linear interpolation between two vec3s and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
void vec3LerpFast(const vec3 *v, const vec3 *offset, const float time, vec3 *out){
	out->x = lerpNumFast(v->x, offset->x, time);
	out->y = lerpNumFast(v->y, offset->y, time);
	out->z = lerpNumFast(v->z, offset->z, time);
}


/*
** Compare two vec3s to find the minimum value per axis
** and return a vec3 composed of these minima in "out".
*/
void vec3Min(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = minNumFast(v1->x, v2->x);
	out->y = minNumFast(v1->y, v2->y);
	out->z = minNumFast(v1->z, v2->z);
}

/*
** Compare two vec3s to find the maximum value per axis
** and return a vec3 composed of these maxima in "out".
*/
void vec3Max(const vec3 *v1, const vec3 *v2, vec3 *out){
	out->x = maxNumFast(v1->x, v2->x);
	out->y = maxNumFast(v1->y, v2->y);
	out->z = maxNumFast(v1->z, v2->z);
}