#include "quat.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


#define QUAT_SINGULARITY_THRESHOLD 0.0001f
// We'll do linear interpolation if the angle between
// the two quaternions is less than half a degree.
#define QUAT_LERP_THRESHOLD        cosf(1.f*DEG_TO_RAD)


quat g_quatIdentity = {
	.x = 0.f,
	.y = 0.f,
	.z = 0.f,
	.w = 1.f
};


// These constants are used by David Eberly's slerp functions.
static const float u[8] = {
	1.f/(1.f * 3.f),
	1.f/(2.f * 5.f),
	1.f/(3.f * 7.f),
	1.f/(4.f * 9.f),
	1.f/(5.f * 11.f),
	1.f/(6.f * 13.f),
	1.f/(7.f * 15.f),
	1.90110745351730037f/(8.f * 17.f)
};
static const float v[8] = {
	1.f/3.f,
	2.f/5.f,
	3.f/7.f,
	4.f/9.f,
	5.f/11.f,
	6.f/13.f,
	7.f/15.f,
	1.90110745351730037f * (8.f/17.f)
};


// Initialize the quaternion's values to 0!
void quatInitZero(quat *const restrict q){
	memset(q, 0.f, sizeof(*q));
}

// Initialize the quaternion's values to 0!
quat quatInitZeroC(){
	quat q;
	memset(&q, 0.f, sizeof(q));

	return(q);
}

// Initialize the quaternion to an identity quaternion!
void quatInitIdentity(quat *const restrict q){
	*q = g_quatIdentity;
}

// Initialize the quaternion to an identity quaternion!
quat quatInitIdentityC(){
	return(g_quatIdentity);
}

// Initialize the quaternion's values to the ones specified!
void quatInitSet(quat *const restrict q, const float x, const float y, const float z, const float w){
	q->x = x;
	q->y = y;
	q->z = z;
	q->w = w;
}

// Initialize the quaternion's values to the ones specified!
quat quatInitSetC(const float x, const float y, const float z, const float w){
	quat q;

	q.x = x;
	q.y = y;
	q.z = z;
	q.w = w;

	return(q);
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZYX.
*/
void quatInitEulerRad(quat *const restrict q, const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const float czcy = cz*cy;
	const float szsy = sz*sy;
	const float czsy = cz*sy;
	const float szcy = sz*cy;

	q->x = czcy * sx - szsy * cx;
	q->y = czsy * cx + szcy * sx;
	q->z = szcy * cx - czsy * sx;
	q->w = czcy * cx + szsy * sx;
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZYX.
*/
quat quatInitEulerRadC(const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const float czcy = cz*cy;
	const float szsy = sz*sy;
	const float czsy = cz*sy;
	const float szcy = sz*cy;

	const quat q = {
		.x = czcy * sx - szsy * cx,
		.y = czsy * cx + szcy * sx,
		.z = szcy * cx - czsy * sx,
		.w = czcy * cx + szsy * sx
	};

	return(q);
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZYX.
*/
void quatInitEulerDeg(quat *const restrict q, const float x, const float y, const float z){
	quatInitEulerRad(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZYX.
*/
quat quatInitEulerDegC(const float x, const float y, const float z){
	return(quatInitEulerRadC(x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD));
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is ZYX.
*/
void quatInitEulerVec3Rad(quat *const restrict q, const vec3 *const restrict v){
	quatInitEulerRad(q, v->x, v->y, v->z);
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is ZYX.
*/
quat quatInitEulerVec3RadC(const vec3 v){
	return(quatInitEulerRadC(v.x, v.y, v.z));
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is ZYX.
*/
void quatInitEulerVec3Deg(quat *const restrict q, const vec3 *const restrict v){
	quatInitEulerRad(q, v->x * DEG_TO_RAD, v->y * DEG_TO_RAD, v->z * DEG_TO_RAD);
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is ZYX.
*/
quat quatInitEulerVec3DegC(const vec3 v){
	return(quatInitEulerRadC(v.x * DEG_TO_RAD, v.y * DEG_TO_RAD, v.z * DEG_TO_RAD));
}


// Add a quat stored as four floats to "q"!
void quatAdd(quat *const restrict q, const float x, const float y, const float z, const float w){
	q->x += x;
	q->y += y;
	q->z += z;
	q->w += w;
}

// Add a quat stored as four floats to "q" and store the result in "out"!
void quatAddOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out){
	out->x = q->x + x;
	out->y = q->y + y;
	out->z = q->z + z;
	out->w = q->w + w;
}

// Add a quat stored as four floats to "q"!
quat quatAddC(quat q, const float x, const float y, const float z, const float w){
	q.x += x;
	q.y += y;
	q.z += z;
	q.w += w;

	return(q);
}

// Add "x" to "q"!
void quatAddS(quat *const restrict q, const float x){
	q->x += x;
	q->y += x;
	q->z += x;
	q->w += x;
}

// Add "x" to "q" and store the result in "out"!
void quatAddSOut(const quat *const restrict q, const float x, quat *const restrict out){
	out->x = q->x + x;
	out->y = q->y + x;
	out->z = q->z + x;
	out->w = q->w + x;
}

// Add "x" to "q"!
quat quatAddSC(quat q, const float x){
	q.x += x;
	q.y += x;
	q.z += x;
	q.w += x;

	return(q);
}

// Add "v" to "q"!
void quatAddQuat(quat *const restrict q, const vec4 *const restrict v){
	q->x += v->x;
	q->y += v->y;
	q->z += v->z;
	q->w += v->w;
}

// Add "v" to "q" and store the result in "out"!
void quatAddQuatOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x + v->x;
	out->y = q->y + v->y;
	out->z = q->z + v->z;
	out->w = q->w + v->w;
}

// Add "v" to "q"!
quat quatAddQuatC(quat q, const vec4 v){
	q.x += v.x;
	q.y += v.y;
	q.z += v.z;
	q.w += v.w;

	return(q);
}

// Subtract a quat stored as four floats from "q"!
void quatSubtract(quat *const restrict q, const float x, const float y, const float z, const float w){
	q->x -= x;
	q->y -= y;
	q->z -= z;
	q->w -= w;
}

// Subtract a quat stored as four floats from "q" and store the result in "out"!
void quatSubtractOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out){
	out->x = q->x - x;
	out->y = q->y - y;
	out->z = q->z - z;
	out->w = q->w - w;
}

// Subtract a quat stored as four floats from "q"!
quat quatSubtractC(quat q, const float x, const float y, const float z, const float w){
	q.x -= x;
	q.y -= y;
	q.z -= z;
	q.w -= w;

	return(q);
}

// Subtract "q" from a quat stored as four floats!
void quatSubtractFrom(quat *const restrict q, const float x, const float y, const float z, const float w){
	q->x = x - q->x;
	q->y = y - q->y;
	q->z = z - q->z;
	q->w = w - q->w;
}

// Subtract "q" from a quat stored as four floats and store the result in "out"!
void quatSubtractFromOut(const quat *const restrict q, const float x, const float y, const float z, const float w, quat *const restrict out){
	out->x = x - q->x;
	out->y = y - q->y;
	out->z = z - q->z;
	out->w = w - q->w;
}

// Subtract "q" from a quat stored as four floats!
quat quatSubtractFromC(quat q, const float x, const float y, const float z, const float w){
	q.x = x - q.x;
	q.y = y - q.y;
	q.z = z - q.z;
	q.w = w - q.w;

	return(q);
}

// Subtract "x" from "q"!
void quatSubtractSFrom(quat *const restrict q, const float x){
	q->x -= x;
	q->y -= x;
	q->z -= x;
	q->w -= x;
}

// Subtract "x" from "q" and store the result in "out"!
void quatSubtractSFromOut(const quat *const restrict q, const float x, quat *const restrict out){
	out->x = q->x - x;
	out->y = q->y - x;
	out->z = q->z - x;
	out->w = q->w - x;
}

// Subtract "x" from "q"!
quat quatSubtractSFromC(quat q, const float x){
	q.x -= x;
	q.y -= x;
	q.z -= x;
	q.w -= x;

	return(q);
}

// Subtract "q" from "x"!
void quatSubtractFromS(quat *const restrict q, const float x){
	q->x = x - q->x;
	q->y = x - q->y;
	q->z = x - q->z;
	q->w = x - q->w;
}

// Subtract "q" from "x" and store the result in "out"!
void quatSubtractFromSOut(const quat *const restrict q, const float x, quat *const restrict out){
	out->x = x - q->x;
	out->y = x - q->y;
	out->z = x - q->z;
	out->w = x - q->w;
}

// Subtract "q" from "x"!
quat quatSubtractFromSC(quat q, const float x){
	q.x = x - q.x;
	q.y = x - q.y;
	q.z = x - q.z;
	q.w = x - q.w;

	return(q);
}

// Subtract "v" from "q"!
void quatSubtractQuatFrom(quat *const restrict q, const vec4 *const restrict v){
	q->x -= v->x;
	q->y -= v->y;
	q->z -= v->z;
	q->w -= v->w;
}

// Subtract "q" from "v"!
void quatSubtractFromQuat(quat *const restrict q, const vec4 *const restrict v){
	q->x = v->x - q->x;
	q->y = v->y - q->y;
	q->z = v->z - q->z;
	q->w = v->w - q->w;
}

// Subtract "v" from "q" and store the result in "out"!
void quatSubtractQuatFromOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x - v->x;
	out->y = q->y - v->y;
	out->z = q->z - v->z;
	out->w = q->w - v->w;
}

// Subtract "v" from "q"!
quat quatSubtractQuatFromC(quat q, const vec4 v){
	q.x -= v.x;
	q.y -= v.y;
	q.z -= v.z;
	q.w -= v.w;

	return(q);
}


// Multiply "q" by "x"!
void quatMultiplyS(quat *const restrict q, const float x){
	q->x *= x;
	q->y *= x;
	q->z *= x;
	q->w *= x;
}

// Multiply "q" by "x" and store the result in "out"!
void quatMultiplySOut(const quat *const restrict q, const float x, quat *const restrict out){
	out->x = q->x * x;
	out->y = q->y * x;
	out->z = q->z * x;
	out->w = q->w * x;
}

// Multiply "q" by "x"!
quat quatMultiplySC(quat q, const float x){
	q.x *= x;
	q.y *= x;
	q.z *= x;
	q.w *= x;

	return(q);
}

// Multiply "q" by "v"!
void quatMultiplyVec4(quat *const restrict q, const vec4 *const restrict v){
	q->x *= v->x;
	q->y *= v->y;
	q->z *= v->z;
	q->w *= v->w;
}

// Multiply "q" by "v" and store the result in "out"!
void quatMultiplyVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x * v->x;
	out->y = q->y * v->y;
	out->z = q->z * v->z;
	out->w = q->w * v->w;
}

// Multiply "q" by "v"!
quat quatMultiplyVec4C(quat q, const vec4 v){
	q.x *= v.x;
	q.y *= v.y;
	q.z *= v.z;
	q.w *= v.w;

	return(q);
}

// Divide "q" by "x"!
void quatDivideByS(quat *const restrict q, const float x){
	const float invX = 1.f / x;

	q->x *= invX;
	q->y *= invX;
	q->z *= invX;
	q->w *= invX;
}

// Divide "q" by "x" and store the result in "out"!
void quatDivideBySOut(const quat *const restrict q, const float x, quat *const restrict out){
	const float invX = 1.f / x;

	out->x = q->x * invX;
	out->y = q->y * invX;
	out->z = q->z * invX;
	out->w = q->w * invX;
}

// Divide "q" by "x"!
quat quatDivideBySC(quat q, const float x){
	const float invX = 1.f / x;

	q.x *= invX;
	q.y *= invX;
	q.z *= invX;
	q.w *= invX;

	return(q);
}

// Divide "x" by "q"!
void quatDivideSBy(quat *const restrict q, const float x){
	q->x = (q->x != 0.f) ? x / q->x : 0.f;
	q->y = (q->y != 0.f) ? x / q->y : 0.f;
	q->z = (q->z != 0.f) ? x / q->z : 0.f;
	q->w = (q->w != 0.f) ? x / q->w : 0.f;
}

// Divide "x" by "q" and store the result in "out"!
void quatDivideSByOut(const quat *const restrict q, const float x, quat *const restrict out){
	out->x = (q->x != 0.f) ? x / q->x : 0.f;
	out->y = (q->y != 0.f) ? x / q->y : 0.f;
	out->z = (q->z != 0.f) ? x / q->z : 0.f;
	out->w = (q->w != 0.f) ? x / q->w : 0.f;
}

// Divide "x" by "q"!
quat quatDivideSByC(quat q, const float x){
	q.x = (q.x != 0.f) ? x / q.x : 0.f;
	q.y = (q.y != 0.f) ? x / q.y : 0.f;
	q.z = (q.z != 0.f) ? x / q.z : 0.f;
	q.w = (q.w != 0.f) ? x / q.w : 0.f;

	return(q);
}

/*
** Divide "x" by "q"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void quatDivideSByFast(quat *const restrict q, const float x){
	q->x = x / q->x;
	q->y = x / q->y;
	q->z = x / q->z;
	q->w = x / q->w;
}

/*
** Divide "x" by "q" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideSByFastOut(const quat *const restrict q, const float x, quat *const restrict out){
	out->x = x / q->x;
	out->y = x / q->y;
	out->z = x / q->z;
	out->w = x / q->w;
}

/*
** Divide "x" by "q"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
quat quatDivideSByFastC(quat q, const float x){
	q.x = x / q.x;
	q.y = x / q.y;
	q.z = x / q.z;
	q.w = x / q.w;

	return(q);
}

// Divide "q" by "v"!
void quatDivideByVec4(quat *const restrict q, const vec4 *const restrict v){
	q->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	q->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	q->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	q->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

// Divide "q" by "v" and store the result in "out"!
void quatDivideByVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	out->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	out->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	out->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

// Divide "q" by "v"!
quat quatDivideByVec4C(quat q, const vec4 v){
	q.x = (v.x != 0.f) ? q.x / v.x : 0.f;
	q.y = (v.y != 0.f) ? q.y / v.y : 0.f;
	q.z = (v.z != 0.f) ? q.z / v.z : 0.f;
	q.w = (v.w != 0.f) ? q.w / v.w : 0.f;

	return(q);
}

// Divide "v" by "q"!
void quatDivideVec4By(quat *const restrict q, const vec4 *const restrict v){
	q->x = (q->x != 0.f) ? v->x / q->x : 0.f;
	q->y = (q->y != 0.f) ? v->y / q->y : 0.f;
	q->z = (q->z != 0.f) ? v->z / q->z : 0.f;
	q->w = (q->w != 0.f) ? v->w / q->w : 0.f;
}

// Divide "v" by "q" and store the result in "out"!
void quatDivideVec4ByOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = (q->x != 0.f) ? v->x / q->x : 0.f;
	out->y = (q->y != 0.f) ? v->y / q->y : 0.f;
	out->z = (q->z != 0.f) ? v->z / q->z : 0.f;
	out->w = (q->w != 0.f) ? v->w / q->w : 0.f;
}

// Divide "v" by "q"!
quat quatDivideVec4ByC(quat q, const vec4 v){
	q.x = (q.x != 0.f) ? v.x / q.x : 0.f;
	q.y = (q.y != 0.f) ? v.y / q.y : 0.f;
	q.z = (q.z != 0.f) ? v.z / q.z : 0.f;
	q.w = (q.w != 0.f) ? v.w / q.w : 0.f;

	return(q);
}

/*
** Divide "q" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void quatDivideByVec4Fast(quat *const restrict q, const vec4 *const restrict v){
	q->x /= v->x;
	q->y /= v->y;
	q->z /= v->z;
	q->w /= v->w;
}

/*
** Divide "q" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideByVec4FastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x / v->x;
	out->y = q->y / v->y;
	out->z = q->z / v->z;
	out->w = q->w / v->w;
}

/*
** Divide "q" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
quat quatDivideByVec4FastC(quat q, const vec4 v){
	q.x /= v.x;
	q.y /= v.y;
	q.z /= v.z;
	q.w /= v.w;

	return(q);
}

/*
** Divide "v" by "q"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void quatDivideVec4ByFast(quat *const restrict q, const vec4 *const restrict v){
	q->x = v->x / q->x;
	q->y = v->y / q->y;
	q->z = v->z / q->z;
	q->w = v->w / q->w;
}

/*
** Divide "v" by "q" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideVec4ByFastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = v->x / q->x;
	out->y = v->y / q->y;
	out->z = v->z / q->z;
	out->w = v->w / q->w;
}

/*
** Divide "v" by "q"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
quat quatDivideVec4ByFastC(quat q, const vec4 v){
	q.x = v.x / q.x;
	q.y = v.y / q.y;
	q.z = v.z / q.z;
	q.w = v.w / q.w;

	return(q);
}


// Multiply q1*q2 (apply a rotation of "q1" to "q2")!
void quatMultiplyByQuat(quat *const restrict q1, const quat q2){
	quat tempQuat = *q1;

	q1->x = tempQuat.w * q2.x + tempQuat.x * q2.w + tempQuat.y * q2.z - tempQuat.z * q2.y;
	q1->y = tempQuat.w * q2.y + tempQuat.y * q2.w + tempQuat.z * q2.x - tempQuat.x * q2.z;
	q1->z = tempQuat.w * q2.z + tempQuat.z * q2.w + tempQuat.x * q2.y - tempQuat.y * q2.x;
	q1->w = tempQuat.w * q2.w - tempQuat.x * q2.x - tempQuat.y * q2.y - tempQuat.z * q2.z;
}

// Multiply q2*q1 (apply a rotation of "q2" to "q1")!
void quatMultiplyQuatBy(quat *const restrict q1, const quat q2){
	quat tempQuat = *q1;

	q1->x = q2.w * tempQuat.x + q2.x * tempQuat.w + q2.y * tempQuat.z - q2.z * tempQuat.y;
	q1->y = q2.w * tempQuat.y + q2.y * tempQuat.w + q2.z * tempQuat.x - q2.x * tempQuat.z;
	q1->z = q2.w * tempQuat.z + q2.z * tempQuat.w + q2.x * tempQuat.y - q2.y * tempQuat.x;
	q1->w = q2.w * tempQuat.w - q2.x * tempQuat.x - q2.y * tempQuat.y - q2.z * tempQuat.z;
}

// Multiply q1*q2 (apply a rotation of "q1" to "q2") and store the result in "out"!
void quatMultiplyByQuatOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	out->y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
	out->z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
	out->w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
}

// Multiply q1*q2 (apply a rotation of "q1" to "q2")!
quat quatMultiplyByQuatC(const quat q1, const quat q2){
	const quat out = {
		.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
	};
	return(out);
}

// Multiply conj(q2)*q1 (undo a rotation of "q2" from "q1")!
void quatMultiplyQuatConjBy(quat *const restrict q1, const quat q2){
	quat tempQuat = *q1;

	q1->x = q2.w * tempQuat.x - q2.x * tempQuat.w - q2.y * tempQuat.z + q2.z * tempQuat.y;
	q1->y = q2.w * tempQuat.y - q2.y * tempQuat.w - q2.z * tempQuat.x + q2.x * tempQuat.z;
	q1->z = q2.w * tempQuat.z - q2.z * tempQuat.w - q2.x * tempQuat.y + q2.y * tempQuat.x;
	q1->w = q2.w * tempQuat.w + q2.x * tempQuat.x + q2.y * tempQuat.y + q2.z * tempQuat.z;
}

// Multiply conj(q1)*q2 (undo a rotation of "q1" from "q2")!
void quatMultiplyConjByQuat(quat *const restrict q1, const quat q2){
	quat tempQuat = *q1;

	q1->x = tempQuat.w * q2.x - tempQuat.x * q2.w - tempQuat.y * q2.z + tempQuat.z * q2.y;
	q1->y = tempQuat.w * q2.y - tempQuat.y * q2.w - tempQuat.z * q2.x + tempQuat.x * q2.z;
	q1->z = tempQuat.w * q2.z - tempQuat.z * q2.w - tempQuat.x * q2.y + tempQuat.y * q2.x;
	q1->w = tempQuat.w * q2.w + tempQuat.x * q2.x + tempQuat.y * q2.y + tempQuat.z * q2.z;
}

// Multiply conj(q2)*q1 (undo a rotation of "q2" from "q1") and store the result in "out"!
void quatMultiplyQuatConjByOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = q2.w * q1.x - q2.x * q1.w - q2.y * q1.z + q2.z * q1.y;
	out->y = q2.w * q1.y - q2.y * q1.w - q2.z * q1.x + q2.x * q1.z;
	out->z = q2.w * q1.z - q2.z * q1.w - q2.x * q1.y + q2.y * q1.x;
	out->w = q2.w * q1.w + q2.x * q1.x + q2.y * q1.y + q2.z * q1.z;
}

// Multiply conj(q2)*q1 (undo a rotation of "q2" from "q1")!
quat quatMultiplyQuatConjByC(const quat q1, const quat q2){
	const quat out = {
		.x = q2.w * q1.x - q2.x * q1.w - q2.y * q1.z + q2.z * q1.y,
		.y = q2.w * q1.y - q2.y * q1.w - q2.z * q1.x + q2.x * q1.z,
		.z = q2.w * q1.z - q2.z * q1.w - q2.x * q1.y + q2.y * q1.x,
		.w = q2.w * q1.w + q2.x * q1.x + q2.y * q1.y + q2.z * q1.z
	};
	return(out);
}

// Multiply q1*conj(q2)!
void quatMultiplyByQuatConj(quat *const restrict q1, const quat q2){
	quat tempQuat = *q1;

	q1->x = -tempQuat.w * q2.x + tempQuat.x * q2.w - tempQuat.y * q2.z + tempQuat.z * q2.y;
	q1->y = -tempQuat.w * q2.y + tempQuat.y * q2.w - tempQuat.z * q2.x + tempQuat.x * q2.z;
	q1->z = -tempQuat.w * q2.z + tempQuat.z * q2.w - tempQuat.x * q2.y + tempQuat.y * q2.x;
	q1->w =  tempQuat.w * q2.w + tempQuat.x * q2.x + tempQuat.y * q2.y + tempQuat.z * q2.z;
}

// Multiply q2*conj(q1)!
void quatMultiplyQuatByConj(quat *const restrict q1, const quat q2){
	quat tempQuat = *q1;

	q1->x = -q2.w * tempQuat.x + q2.x * tempQuat.w - q2.y * tempQuat.z + q2.z * tempQuat.y;
	q1->y = -q2.w * tempQuat.y + q2.y * tempQuat.w - q2.z * tempQuat.x + q2.x * tempQuat.z;
	q1->z = -q2.w * tempQuat.z + q2.z * tempQuat.w - q2.x * tempQuat.y + q2.y * tempQuat.x;
	q1->w =  q2.w * tempQuat.w + q2.x * tempQuat.x + q2.y * tempQuat.y + q2.z * tempQuat.z;
}

// Multiply q1*conj(q2) and store the result in "out"!
void quatMultiplyByQuatConjOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = -q1.w * q2.x + q1.x * q2.w - q1.y * q2.z + q1.z * q2.y;
	out->y = -q1.w * q2.y + q1.y * q2.w - q1.z * q2.x + q1.x * q2.z;
	out->z = -q1.w * q2.z + q1.z * q2.w - q1.x * q2.y + q1.y * q2.x;
	out->w =  q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

// Multiply q1*conj(q2)!
quat quatMultiplyByQuatConjC(const quat q1, const quat q2){
	const quat out = {
		.x = -q1.w * q2.x + q1.x * q2.w - q1.y * q2.z + q1.z * q2.y,
		.y = -q1.w * q2.y + q1.y * q2.w - q1.z * q2.x + q1.x * q2.z,
		.z = -q1.w * q2.z + q1.z * q2.w - q1.x * q2.y + q1.y * q2.x,
		.w =  q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z
	};
	return(out);
}


// Apply a quaternion rotation to a vec3!
void quatRotateVec3(const quat *const restrict q, vec3 *const restrict v){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3NormVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	v->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	v->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	v->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Apply a quaternion rotation to a vec3!
void quatRotateVec3Out(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3NormVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Apply a quaternion rotation to a vec3!
vec3 quatRotateVec3C(const quat q, vec3 v){
	float qvDot = vec3DotVec3C(v, *((const vec3 *)&q.x));
	const float qNorm = q.w * q.w - vec3NormVec3C(*((const vec3 *)&q.x));
	const vec3 qvCross = vec3MultiplySC(vec3CrossVec3C(*((const vec3 *)&q.x), v), q.w + q.w);
	qvDot += qvDot;

	v.x = qvDot * q.x + qNorm * v.x + qvCross.x;
	v.y = qvDot * q.y + qNorm * v.y + qvCross.y;
	v.z = qvDot * q.z + qNorm * v.z + qvCross.z;

	return(v);
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3Inverse(const quat *const restrict q, vec3 *const restrict v){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3NormVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, -q->w - q->w);

	v->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	v->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	v->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3InverseOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3NormVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, -q->w - q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Undo a quaternion rotation on a vec3!
vec3 quatRotateVec3InverseC(const quat q, vec3 v){
	float qvDot = vec3DotVec3C(v, *((const vec3 *)&q.x));
	const float qNorm = q.w * q.w - vec3NormVec3C(*((const vec3 *)&q.x));
	const vec3 qvCross = vec3MultiplySC(vec3CrossVec3C(*((const vec3 *)&q.x), v), -q.w - q.w);
	qvDot += qvDot;

	v.x = qvDot * q.x + qNorm * v.x + qvCross.x;
	v.y = qvDot * q.y + qNorm * v.y + qvCross.y;
	v.z = qvDot * q.z + qNorm * v.z + qvCross.z;

	return(v);
}

/*
** Apply a quaternion rotation to a vec3!
**
** Note: This method is significantly faster, but it is not perfectly accurate.
*/
void quatRotateVec3Fast(const quat *const restrict q, vec3 *const restrict v){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	qvCross.x += q->w * v->x;
	qvCross.y += q->w * v->y;
	qvCross.z += q->w * v->z;
	vec3CrossVec3By((const vec3 *const restrict)&q->x, &qvCross);

	v->x = qvCross.x + qvCross.x + v->x;
	v->y = qvCross.y + qvCross.y + v->y;
	v->z = qvCross.z + qvCross.z + v->z;
}

// Apply a quaternion rotation to a vec3!
void quatRotateVec3FastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	qvCross.x += q->w * v->x;
	qvCross.y += q->w * v->y;
	qvCross.z += q->w * v->z;
	vec3CrossVec3By((const vec3 *const restrict)&q->x, &qvCross);

	out->x = qvCross.x + qvCross.x + v->x;
	out->y = qvCross.y + qvCross.y + v->y;
	out->z = qvCross.z + qvCross.z + v->z;
}

// Apply a quaternion rotation to a vec3!
vec3 quatRotateVec3FastC(const quat q, vec3 v){
	vec3 qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), v);
	qvCross.x += q.w * v.x;
	qvCross.y += q.w * v.y;
	qvCross.z += q.w * v.z;
	qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), qvCross);

	v.x = qvCross.x + qvCross.x + v.x;
	v.y = qvCross.y + qvCross.y + v.y;
	v.z = qvCross.z + qvCross.z + v.z;

	return(v);
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3InverseFast(const quat *const restrict q, vec3 *const restrict v){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	qvCross.x -= q->w * v->x;
	qvCross.y -= q->w * v->y;
	qvCross.z -= q->w * v->z;
	vec3CrossVec3By((const vec3 *const restrict)&q->x, &qvCross);

	v->x = qvCross.x + qvCross.x + v->x;
	v->y = qvCross.y + qvCross.y + v->y;
	v->z = qvCross.z + qvCross.z + v->z;
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3InverseFastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	qvCross.x -= q->w * v->x;
	qvCross.y -= q->w * v->y;
	qvCross.z -= q->w * v->z;
	vec3CrossVec3By((const vec3 *const restrict)&q->x, &qvCross);

	out->x = qvCross.x + qvCross.x + v->x;
	out->y = qvCross.y + qvCross.y + v->y;
	out->z = qvCross.z + qvCross.z + v->z;
}

// Undo a quaternion rotation on a vec3!
vec3 quatRotateVec3InverseFastC(const quat q, vec3 v){
	vec3 qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), v);
	qvCross.x -= q.w * v.x;
	qvCross.y -= q.w * v.y;
	qvCross.z -= q.w * v.z;
	qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), qvCross);

	v.x = qvCross.x + qvCross.x + v.x;
	v.y = qvCross.y + qvCross.y + v.y;
	v.z = qvCross.z + qvCross.z + v.z;

	return(v);
}


// Find the magnitude (length) of a quaternion stored as four floats!
float quatMagnitude(const float x, const float y, const float z, const float w){
	return(sqrtf(quatNorm(x, y, z, w)));
}

// Find the magnitude (length) of a quaternion!
float quatMagnitudeQuat(const quat *const restrict q){
	return(sqrtf(quatNormQuat(q)));
}

// Find the magnitude (length) of a quaternion!
float quatMagnitudeQuatC(const quat q){
	return(sqrtf(quatNormQuatC(q)));
}


// Find the dot product of two vec4s stored as four floats!
float quatDot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2){
	return(x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2);
}

// Find the dot product of a quat and a vec4 stored as four floats!
float quatDotQuatFloat(const quat *const restrict q, const float x, const float y, const float z, const float w){
	return(q->x * x + q->y * y + q->z * z + q->w * w);
}

// Find the dot product of a quat and a vec4 stored as four floats!
float quatDotQuatFloatC(const quat q, const float x, const float y, const float z, const float w){
	return(q.x * x + q.y * y + q.z * z + q.w * w);
}

// Find the dot product of two vec4s!
float quatDotQuat(const quat *const restrict q1, const quat *const restrict q2){
	return(q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w);
}

// Find the dot product of two vec4s!
float quatDotQuatC(const quat q1, const quat q2){
	return(q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w);
}

// Find the norm of a quaternion stored as four floats!
float quatNorm(const float x, const float y, const float z, const float w){
	return(x * x + y * y + z * z + w * w);
}

// Find the norm of a quaternion!
float quatNormQuat(const quat *const restrict q){
	return(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}

// Find the norm of a quaternion!
float quatNormQuatC(const quat q){
	return(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}


// Normalize a quaternion stored as four floats and store the result in "out"!
void quatNormalize(const float x, const float y, const float z, const float w, quat *const restrict out){
	const float magnitude = invSqrt(x * x + y * y + z * z + w * w);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

// Normalize a quaternion stored as four floats and store the result in "out"!
void quatNormalizeFast(const float x, const float y, const float z, const float w, quat *const restrict out){
	const float magnitude = invSqrtFast(x * x + y * y + z * z + w * w);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

// Normalize a quaternion stored as four floats!
quat quatNormalizeC(const float x, const float y, const float z, const float w){
	const float magnitude = invSqrt(x * x + y * y + z * z + w * w);
	quat q;

	q.x = x * magnitude;
	q.y = y * magnitude;
	q.z = z * magnitude;
	q.w = w * magnitude;

	return(q);
}

// Normalize a quaternion stored as four floats!
quat quatNormalizeFastC(const float x, const float y, const float z, const float w){
	const float magnitude = invSqrtFast(x * x + y * y + z * z + w * w);
	quat q;

	q.x = x * magnitude;
	q.y = y * magnitude;
	q.z = z * magnitude;
	q.w = w * magnitude;

	return(q);
}

// Normalize a quaternion!
void quatNormalizeQuat(quat *const restrict q){
	const float magnitude = invSqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);

	q->x *= magnitude;
	q->y *= magnitude;
	q->z *= magnitude;
	q->w *= magnitude;
}

// Normalize a quaternion!
void quatNormalizeQuatFast(quat *const restrict q){
	const float magnitude = invSqrtFast(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);

	q->x *= magnitude;
	q->y *= magnitude;
	q->z *= magnitude;
	q->w *= magnitude;
}

// Normalize a quaternion and store the result in "out"!
void quatNormalizeQuatOut(const quat *const restrict q, quat *const restrict out){
	const float magnitude = invSqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);

	out->x = q->x * magnitude;
	out->y = q->y * magnitude;
	out->z = q->z * magnitude;
	out->w = q->w * magnitude;
}

// Normalize a quaternion and store the result in "out"!
void quatNormalizeQuatFastOut(const quat *const restrict q, quat *const restrict out){
	const float magnitude = invSqrtFast(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);

	out->x = q->x * magnitude;
	out->y = q->y * magnitude;
	out->z = q->z * magnitude;
	out->w = q->w * magnitude;
}

// Normalize a quaternion!
quat quatNormalizeQuatC(quat q){
	const float magnitude = invSqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

	q.x *= magnitude;
	q.y *= magnitude;
	q.z *= magnitude;
	q.w *= magnitude;

	return(q);
}

// Normalize a quaternion!
quat quatNormalizeQuatFastC(quat q){
	const float magnitude = invSqrtFast(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

	q.x *= magnitude;
	q.y *= magnitude;
	q.z *= magnitude;
	q.w *= magnitude;

	return(q);
}

/*
** Find the conjugate of a quaternion! This is
** obtained by negating the imaginary parts.
*/
void quatConjugate(quat *const restrict q){
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
	q->w = q->w;
}

// Find the conjugate of a quaternion and store the result in "out"!
void quatConjugateOut(const quat *const restrict q, quat *const restrict out){
	out->x = -q->x;
	out->y = -q->y;
	out->z = -q->z;
	out->w = q->w;
}

// Find the conjugate of a quaternion!
quat quatConjugateC(quat q){
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	q.w = q.w;

	return(q);
}

/*
** Rather than negating the imaginary parts,
** negating the real part also seems to work.
*/
void quatConjugateFast(quat *const restrict q){
	q->w = -q->w;
}

// Find the conjugate of a quaternion and store the result in "out"!
void quatConjugateFastOut(const quat *const restrict q, quat *const restrict out){
	*out = *q;
	out->w = -q->w;
}

// Find the conjugate of a quaternion!
quat quatConjugateFastC(quat q){
	q.w = -q.w;

	return(q);
}

/*
** Negate every component of a quaternion! This effectively swaps
** the path taken to reach the orientation that it represents.
*/
void quatNegate(quat *const restrict q){
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
	q->w = -q->w;
}

// Negative every component of a quaternion and store the result in "out"!
void quatNegateOut(const quat *const restrict q, quat *const restrict out){
	out->x = -q->x;
	out->y = -q->y;
	out->z = -q->z;
	out->w = -q->w;
}

// Negative every component of a quaternion!
quat quatNegateC(quat q){
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	q.w = -q.w;

	return(q);
}


/*
** Find the Euler angles describing a quaternion and store the result in "out"!
** The output vector is given in the form (roll, pitch, yaw) and the ordering
** is ZYX, as usual.
**
** Implementation inspired by the one in Gino van den Bergen's Motion Toolkit.
*/
void quatToEulerAngles(const quat q, vec3 *const restrict out){
	const float xx = q.x*q.x;
	vec3InitSet(out,
		asinf(2.f*(q.x*q.w - q.y*q.z)),
		atan2f(2.f*(q.x*q.z + q.y*q.w), 1.f - 2.f*(xx + q.y*q.y)),
		atan2f(2.f*(q.x*q.y + q.z*q.w), 1.f - 2.f*(xx + q.z*q.z))
	);
}

/*
** Find the Euler angles describing a quaternion and return them as a vec3.
** The output vector is given in the form (roll, pitch, yaw) and the ordering
** is ZYX, as usual.
**
** Implementation inspired by the one in Gino van den Bergen's Motion Toolkit.
*/
vec3 quatToEulerAnglesC(const quat q){
	const float xx = q.x*q.x;
	return(vec3InitSetC(
		asinf(2.f*(q.x*q.w - q.y*q.z)),
		atan2f(2.f*(q.x*q.z + q.y*q.w), 1.f - 2.f*(xx + q.y*q.y)),
		atan2f(2.f*(q.x*q.y + q.z*q.w), 1.f - 2.f*(xx + q.z*q.z))
	));
}

/*
** Find the Euler angles describing a quaternion and store the result in "out"!
** The output vector is given in the form (pitch, yaw, roll) and the ordering
** is ZYX, as usual.
**
** This version checks for singularities, but I don't think it's strictly necessary.
** Thanks to Martin Baker at EuclideanSpace for this implementation!
*/
void quatToEulerAnglesAlt(const quat q, vec3 *const restrict out){
	const float sinp = q.x*q.y + q.z*q.w;
	// Handle the singularities at the poles.
	if(fabsf(sinp) > 0.5f - QUAT_SINGULARITY_THRESHOLD){
		vec3InitSet(out, 0.f, copySign(2.f*atan2f(q.x, q.w), sinp), copySign(M_PI_2, sinp));
	}else{
		const float zz = q.z*q.z;
		vec3InitSet(out,
			atan2f(2.f*(q.x*q.w - q.y*q.z), 1.f - 2.f*(q.x*q.x + zz)),
			atan2f(2.f*(q.y*q.w - q.x*q.z), 1.f - 2.f*(q.y*q.y + zz)),
			asinf(2.f*sinp)
		);
	}
}

/*
** Find the Euler angles describing a quaternion and return them as a vec3.
** The output vector is given in the form (pitch, yaw, roll) and the ordering
** is ZYX, as usual.
**
** This version checks for singularities, but I don't think it's strictly necessary.
** Thanks to Martin Baker at EuclideanSpace for this implementation!
*/
vec3 quatToEulerAnglesAltC(const quat q){
	const float sinp = q.x*q.y + q.z*q.w;
	// Handle the singularities at the poles.
	if(fabsf(sinp) > 0.5f - QUAT_SINGULARITY_THRESHOLD){
		return(vec3InitSetC(0.f, copySign(2.f*atan2f(q.x, q.w), sinp), copySign(M_PI_2, sinp)));
	}else{
		const float zz = q.z*q.z;
		return(vec3InitSetC(
			atan2f(2.f*(q.x*q.w - q.y*q.z), 1.f - 2.f*(q.x*q.x + zz)),
			atan2f(2.f*(q.y*q.w - q.x*q.z), 1.f - 2.f*(q.y*q.y + zz)),
			asinf(2.f*sinp)
		));
	}
}

// Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngle(const quat *const restrict q, vec4 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale < QUAT_SINGULARITY_THRESHOLD*QUAT_SINGULARITY_THRESHOLD){
		out->x = 1.f;
		out->y = 0.f;
		out->z = 0.f;
	}else{
		scale = invSqrt(scale);
		out->x = q->x*scale;
		out->y = q->y*scale;
		out->z = q->z*scale;
	}
	out->w = 2.f*acosf(q->w);
}

// Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngleFast(const quat *const restrict q, vec4 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale < QUAT_SINGULARITY_THRESHOLD*QUAT_SINGULARITY_THRESHOLD){
		out->x = 1.f;
		out->y = 0.f;
		out->z = 0.f;
	}else{
		scale = invSqrtFast(scale);
		out->x = q->x*scale;
		out->y = q->y*scale;
		out->z = q->z*scale;
	}
	out->w = 2.f*acosf(q->w);
}

// Convert a quaternion to an axis and an angle!
vec4 quatToAxisAngleC(const quat q){
	vec4 v;

	float scale = 1.f - q.w*q.w;
	if(scale < QUAT_SINGULARITY_THRESHOLD*QUAT_SINGULARITY_THRESHOLD){
		v.x = 1.f;
		v.y = 0.f;
		v.z = 0.f;
	}else{
		scale = invSqrt(scale);
		v.x = q.x*scale;
		v.y = q.y*scale;
		v.z = q.z*scale;
	}
	v.w = 2.f*acosf(q.w);

	return(v);
}

// Convert a quaternion to an axis and an angle!
vec4 quatToAxisAngleFastC(const quat q){
	vec4 v;

	float scale = 1.f - q.w*q.w;
	if(scale < QUAT_SINGULARITY_THRESHOLD*QUAT_SINGULARITY_THRESHOLD){
		v.x = 1.f;
		v.y = 0.f;
		v.z = 0.f;
	}else{
		scale = invSqrtFast(scale);
		v.x = q.x*scale;
		v.y = q.y*scale;
		v.z = q.z*scale;
	}
	v.w = 2.f*acosf(q.w);

	return(v);
}


// Rotate a quaternion (in radians)!
void quatRotateByRad(quat *const restrict q, const float x, const float y, const float z){
	quat rot;
	quatInitEulerRad(&rot, x, y, z);
	quatMultiplyQuatBy(q, rot);
}

// Rotate a quaternion (in radians)!
quat quatRotateByRadC(const quat q, const float x, const float y, const float z){
	return(quatMultiplyByQuatC(quatInitEulerRadC(x, y, z), q));
}

// Rotate a quaternion (in degrees)!
void quatRotateByDeg(quat *const restrict q, const float x, const float y, const float z){
	quatRotateByRad(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

// Rotate a quaternion (in degrees)!
quat quatRotateByDegC(const quat q, const float x, const float y, const float z){
	return(quatRotateByRadC(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD));
}

// Rotate a quaternion by a vec3 (in radians)!
void quatRotateByVec3Rad(quat *const restrict q, const vec3 *const restrict v){
	quatRotateByRad(q, v->x, v->y, v->z);
}

// Rotate a quaternion by a vec3 (in radians)!
quat quatRotateByVec3RadC(const quat q, const vec3 v){
	return(quatRotateByRadC(q, v.x, v.y, v.z));
}

// Rotate a quaternion by a vec3 (in degrees)!
void quatRotateByVec3Deg(quat *const restrict q, const vec3 *const restrict v){
	quatRotateByRad(q, v->x * DEG_TO_RAD, v->y * DEG_TO_RAD, v->z * DEG_TO_RAD);
}

// Rotate a quaternion by a vec3 (in degrees)!
quat quatRotateByVec3DegC(const quat q, const vec3 v){
	return(quatRotateByRadC(q, v.x * DEG_TO_RAD, v.y * DEG_TO_RAD, v.z * DEG_TO_RAD));
}


// Perform linear interpolation between two quaternions!
void quatLerp(quat *const restrict q1, const quat *const restrict q2, const float time){
	q1->x = lerpFloatFast(q1->x, q2->x, time);
	q1->y = lerpFloatFast(q1->y, q2->y, time);
	q1->z = lerpFloatFast(q1->z, q2->z, time);
	q1->w = lerpFloatFast(q1->w, q2->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(q1);
}

// Perform linear interpolation between two quaternions and store the result in "out"!
void quatLerpOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out){
	out->x = lerpFloatFast(q1->x, q2->x, time);
	out->y = lerpFloatFast(q1->y, q2->y, time);
	out->z = lerpFloatFast(q1->z, q2->z, time);
	out->w = lerpFloatFast(q1->w, q2->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(out);
}

// Perform linear interpolation between two quaternions!
quat quatLerpC(quat q1, const quat q2, const float time){
	q1.x = lerpFloatFast(q1.x, q2.x, time);
	q1.y = lerpFloatFast(q1.y, q2.y, time);
	q1.z = lerpFloatFast(q1.z, q2.z, time);
	q1.w = lerpFloatFast(q1.w, q2.w, time);

	// It's nice to be safe... but it isn't very fast.
	return(quatNormalizeQuatC(q1));
}

/*
** Perform linear interpolation between two quaternions!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
void quatLerpDiff(quat *const restrict q, const quat *const restrict offset, const float time){
	q->x = lerpDiffFast(q->x, offset->x, time);
	q->y = lerpDiffFast(q->y, offset->y, time);
	q->z = lerpDiffFast(q->z, offset->z, time);
	q->w = lerpDiffFast(q->w, offset->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(q);
}

/*
** Perform linear interpolation between two quaternions and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
void quatLerpDiffOut(const quat *const restrict q, const quat *const restrict offset, const float time, quat *const restrict out){
	out->x = lerpDiffFast(q->x, offset->x, time);
	out->y = lerpDiffFast(q->y, offset->y, time);
	out->z = lerpDiffFast(q->z, offset->z, time);
	out->w = lerpDiffFast(q->w, offset->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(out);
}

/*
** Perform linear interpolation between two quaternions!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
quat quatLerpDiffC(quat q, const quat offset, const float time){
	q.x = lerpDiffFast(q.x, offset.x, time);
	q.y = lerpDiffFast(q.y, offset.y, time);
	q.z = lerpDiffFast(q.z, offset.z, time);
	q.w = lerpDiffFast(q.w, offset.w, time);

	// It's nice to be safe... but it isn't very fast.
	return(quatNormalizeQuatC(q));
}

// Perform spherical linear interpolation between two quaternions!
void quatSlerp(quat *const restrict q1, quat q2, const float time){
	float cosTheta = quatDotQuat(q1, &q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		quatNegate(&q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		quatLerp(q1, &q2, time);
	}else{
		const float theta = acosf(cosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * invSinTheta;
		const float sinThetaT = sinf(theta * time) * invSinTheta;

		q1->x = q1->x * sinThetaInvT + q2.x * sinThetaT;
		q1->y = q1->y * sinThetaInvT + q2.y * sinThetaT;
		q1->z = q1->z * sinThetaInvT + q2.z * sinThetaT;
		q1->w = q1->w * sinThetaInvT + q2.w * sinThetaT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuat(q1);
	}
}

/*
** Perform spherical linear interpolation between
** two quaternions and store the result in "out"!
*/
void quatSlerpOut(const quat *const restrict q1, quat q2, const float time, quat *const restrict out){
	float cosTheta = quatDotQuat(q1, &q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		quatNegate(&q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		quatLerpOut(q1, &q2, time, out);
	}else{
		const float theta = acosf(cosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * invSinTheta;
		const float sinThetaT = sinf(theta * time) * invSinTheta;

		out->x = q1->x * sinThetaInvT + q2.x * sinThetaT;
		out->y = q1->y * sinThetaInvT + q2.y * sinThetaT;
		out->z = q1->z * sinThetaInvT + q2.z * sinThetaT;
		out->w = q1->w * sinThetaInvT + q2.w * sinThetaT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuat(out);
	}
}

// Perform spherical linear interpolation between two quaternions!
quat quatSlerpC(quat q1, quat q2, const float time){
	float cosTheta = quatDotQuatC(q1, q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		q2 = quatNegateC(q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		return(quatLerpC(q1, q2, time));
	}else{
		const float theta = acosf(cosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * invSinTheta;
		const float sinThetaT = sinf(theta * time) * invSinTheta;

		q1.x = q1.x * sinThetaInvT + q2.x * sinThetaT;
		q1.y = q1.y * sinThetaInvT + q2.y * sinThetaT;
		q1.z = q1.z * sinThetaInvT + q2.z * sinThetaT;
		q1.w = q1.w * sinThetaInvT + q2.w * sinThetaT;

		// It's nice to be safe... but it isn't very fast.
		return(quatNormalizeQuatC(q1));
	}
}

/*
** Slerp two quaternions using an optimized algorithm!
**
** Note: Although this should be significantly faster than
** the former method, it may also be slightly less accurate.
*/
void quatSlerpFast(quat *const restrict q1, quat q2, const float time){
	float cosTheta = quatDotQuat(q1, &q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		quatNegate(&q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		quatLerp(q1, &q2, time);
	}else{
		const float theta = acosf(cosTheta);
		const float sinTheta = invSqrtFast(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * sinTheta;
		const float sinThetaT = sinf(theta * time) * sinTheta;

		q1->x = q1->x * sinThetaInvT + q2.x * sinThetaT;
		q1->y = q1->y * sinThetaInvT + q2.y * sinThetaT;
		q1->z = q1->z * sinThetaInvT + q2.z * sinThetaT;
		q1->w = q1->w * sinThetaInvT + q2.w * sinThetaT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuatFast(q1);
	}
}

/*
** Perform spherical linear interpolation between
** two quaternions using an optimized algorithm
** and store the result in "out"!
*/
void quatSlerpFastOut(const quat *const restrict q1, quat q2, const float time, quat *const restrict out){
	float cosTheta = quatDotQuat(q1, &q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		quatNegate(&q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		quatLerpOut(q1, &q2, time, out);
	}else{
		const float theta = acosf(cosTheta);
		const float sinTheta = invSqrtFast(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * sinTheta;
		const float sinThetaT = sinf(theta * time) * sinTheta;

		out->x = q1->x * sinThetaInvT + q2.x * sinThetaT;
		out->y = q1->y * sinThetaInvT + q2.y * sinThetaT;
		out->z = q1->z * sinThetaInvT + q2.z * sinThetaT;
		out->w = q1->w * sinThetaInvT + q2.w * sinThetaT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuatFast(out);
	}
}

// Slerp two quaternions using an optimized algorithm!
quat quatSlerpFastC(quat q1, quat q2, const float time){
	float cosTheta = quatDotQuatC(q1, q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		q2 = quatNegateC(q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		return(quatLerpC(q1, q2, time));
	}else{
		const float theta = acosf(cosTheta);
		const float sinTheta = invSqrtFast(1.f - cosTheta * cosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * sinTheta;
		const float sinThetaT = sinf(theta * time) * sinTheta;

		q1.x = q1.x * sinThetaInvT + q2.x * sinThetaT;
		q1.y = q1.y * sinThetaInvT + q2.y * sinThetaT;
		q1.z = q1.z * sinThetaInvT + q2.z * sinThetaT;
		q1.w = q1.w * sinThetaInvT + q2.w * sinThetaT;

		// It's nice to be safe... but it isn't very fast.
		return(quatNormalizeQuatFastC(q1));
	}
}

/*
** Slerp two quaternions using the algorithm presented by David Eberly in his 2011 paper
** "A Fast and Accurate Algorithm for Computing SLERP". This method notably removes all
** trigonometric function calls and is still extremely accurate.
*/
void quatSlerpFaster(quat *const restrict q1, const quat *const restrict q2, const float time){
	float x = quatDotQuat(q1, q2);
	const float sign = (x >= 0.f) ? 1.f : (x = -x, -1.f);

	const float xm1 = x - 1.f;
	const float d = 1.f - time;
	const float sqrD = d * d;
	const float sqrT = time * time;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.f + bD[0] * (1.f + bD[1] * (1.f + bD[2] * (1.f + bD[3] * (
		1.f + bD[4] * (1.f + bD[5] * (1.f + bD[6] * (1.f + bD[7])))))))
	);
	const float cT = sign * time * (
		1.f + bT[0] * (1.f + bT[1] * (1.f + bT[2] * (1.f + bT[3] * (
		1.f + bT[4] * (1.f + bT[5] * (1.f + bT[6] * (1.f + bT[7])))))))
	);


	q1->x = q1->x * cD + q2->x * cT;
	q1->y = q1->y * cD + q2->y * cT;
	q1->z = q1->z * cD + q2->z * cT;
	q1->w = q1->w * cD + q2->w * cT;

	quatNormalizeQuatFast(q1);
}

/*
** Perform spherical linear interpolation between
** two quaternions using an even more optimized algorithm
** and store the result in "out"!
*/
void quatSlerpFasterOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out){
	float x = quatDotQuat(q1, q2);
	const float sign = (x >= 0.f) ? 1.f : (x = -x, -1.f);

	const float xm1 = x - 1.f;
	const float d = 1.f - time;
	const float sqrD = d * d;
	const float sqrT = time * time;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.f + bD[0] * (1.f + bD[1] * (1.f + bD[2] * (1.f + bD[3] * (
		1.f + bD[4] * (1.f + bD[5] * (1.f + bD[6] * (1.f + bD[7])))))))
	);
	const float cT = sign * time * (
		1.f + bT[0] * (1.f + bT[1] * (1.f + bT[2] * (1.f + bT[3] * (
		1.f + bT[4] * (1.f + bT[5] * (1.f + bT[6] * (1.f + bT[7])))))))
	);


	out->x = q1->x * cD + q2->x * cT;
	out->y = q1->y * cD + q2->y * cT;
	out->z = q1->z * cD + q2->z * cT;
	out->w = q1->w * cD + q2->w * cT;

	quatNormalizeQuatFast(out);
}

// Slerp two quaternions using an even more optimized algorithm!
quat quatSlerpFasterC(quat q1, const quat q2, const float time){
	float x = quatDotQuatC(q1, q2);
	const float sign = (x >= 0.f) ? 1.f : (x = -x, -1.f);

	const float xm1 = x - 1.f;
	const float d = 1.f - time;
	const float sqrD = d * d;
	const float sqrT = time * time;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.f + bD[0] * (1.f + bD[1] * (1.f + bD[2] * (1.f + bD[3] * (
		1.f + bD[4] * (1.f + bD[5] * (1.f + bD[6] * (1.f + bD[7])))))))
	);
	const float cT = sign * time * (
		1.f + bT[0] * (1.f + bT[1] * (1.f + bT[2] * (1.f + bT[3] * (
		1.f + bT[4] * (1.f + bT[5] * (1.f + bT[6] * (1.f + bT[7])))))))
	);


	q1.x = q1.x * cD + q2.x * cT;
	q1.y = q1.y * cD + q2.y * cT;
	q1.z = q1.z * cD + q2.z * cT;
	q1.w = q1.w * cD + q2.w * cT;

	return(quatNormalizeQuatFastC(q1));
}


/*
** Scale a quaternion by slerping between
** it and the identity quaternion!
*/
void quatScale(quat *const restrict q, const float x){
	const float absCosTheta = fabsf(q->w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q->w < 0.f){
			quatMultiplyS(q, -x);
		}else{
			quatMultiplyS(q, x);
		}
	}else{
		const float theta = acosf(absCosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * invSinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q->w >= 0.f) ? sinf(theta * x) * invSinTheta : -(sinf(theta * x) * invSinTheta);

		quatMultiplyS(q, sinThetaT);
		q->w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuatFast(q);
	}
}

/*
** Scale a quaternion by slerping between
** it and the identity quaternion and store
** the result in "out"!
*/
void quatScaleOut(const quat *const restrict q, const float x, quat *const restrict out){
	const float absCosTheta = fabsf(q->w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q->w < 0.f){
			quatMultiplySOut(q, -x, out);
		}else{
			quatMultiplySOut(q, x, out);
		}
	}else{
		const float theta = acosf(absCosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * invSinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q->w >= 0.f) ? sinf(theta * x) * invSinTheta : -(sinf(theta * x) * invSinTheta);

		quatMultiplySOut(q, sinThetaT, out);
		out->w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuat(out);
	}
}

/*
** Scale a quaternion by slerping between
** it and the identity quaternion!
*/
quat quatScaleC(quat q, const float x){
	const float absCosTheta = fabsf(q.w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q.w < 0.f){
			return(quatMultiplySC(q, -x));
		}
		return(quatMultiplySC(q, x));
	}else{
		const float theta = acosf(absCosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * invSinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q.w >= 0.f) ? sinf(theta * x) * invSinTheta : -(sinf(theta * x) * invSinTheta);

		q = quatMultiplySC(q, sinThetaT);
		q.w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		return(quatNormalizeQuatC(q));
	}
}

/*
** Scale a quaternion by slerping between
** it and the identity quaternion!
**
** Note: Although this should be significantly faster than
** the former method, it may also be slightly less accurate.
*/
void quatScaleFast(quat *const restrict q, const float x){
	const float absCosTheta = fabsf(q->w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q->w < 0.f){
			quatMultiplyS(q, -x);
		}else{
			quatMultiplyS(q, x);
		}
	}else{
		const float theta = acosf(absCosTheta);
		const float sinTheta = invSqrtFast(1.f - absCosTheta * absCosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * sinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q->w >= 0.f) ? sinf(theta * x) * sinTheta : -(sinf(theta * x) * sinTheta);

		quatMultiplyS(q, sinThetaT);
		q->w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuatFast(q);
	}
}

/*
** Scale a quaternion using an optimized
** algorithm and store the result in "out"!
*/
void quatScaleFastOut(const quat *const restrict q, const float x, quat *const restrict out){
	const float absCosTheta = fabsf(q->w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q->w < 0.f){
			quatMultiplySOut(q, -x, out);
		}else{
			quatMultiplySOut(q, x, out);
		}
	}else{
		const float theta = acosf(absCosTheta);
		const float sinTheta = invSqrtFast(1.f - absCosTheta * absCosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * sinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q->w >= 0.f) ? sinf(theta * x) * sinTheta : -(sinf(theta * x) * sinTheta);

		quatMultiplySOut(q, sinThetaT, out);
		out->w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		quatNormalizeQuatFast(out);
	}
}

// Scale a quaternion using an optimized algorithm!
quat quatScaleFastC(quat q, const float x){
	const float absCosTheta = fabsf(q.w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q.w < 0.f){
			return(quatMultiplySC(q, -x));
		}
		return(quatMultiplySC(q, x));
	}else{
		const float theta = acosf(absCosTheta);
		const float sinTheta = invSqrtFast(1.f - absCosTheta * absCosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * sinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q.w >= 0.f) ? sinf(theta * x) * sinTheta : -(sinf(theta * x) * sinTheta);

		q = quatMultiplySC(q, sinThetaT);
		q.w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		return(quatNormalizeQuatFastC(q));
	}
}

/*
** Scale a quaternion using the slerp algorithm presented by David Eberly in his 2011 paper
** "A Fast and Accurate Algorithm for Computing SLERP". This method notably removes all
** trigonometric function calls and is still extremely accurate.
*/
void quatScaleFaster(quat *const restrict q, const float time){
	float x = q->w;
	const float sign = (x >= 0.f) ? 1.f : (x = -x, -1.f);

	const float xm1 = x - 1.f;
	const float d = 1.f - time;
	const float sqrD = d * d;
	const float sqrT = time * time;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.f + bD[0] * (1.f + bD[1] * (1.f + bD[2] * (1.f + bD[3] * (
		1.f + bD[4] * (1.f + bD[5] * (1.f + bD[6] * (1.f + bD[7])))))))
	);
	const float cT = sign * time * (
		1.f + bT[0] * (1.f + bT[1] * (1.f + bT[2] * (1.f + bT[3] * (
		1.f + bT[4] * (1.f + bT[5] * (1.f + bT[6] * (1.f + bT[7])))))))
	);


	quatMultiplyS(q, cD);
	q->w += cT;

	quatNormalizeQuatFast(q);
}


/*
** Scale a quaternion using an even more optimized
** algorithm and store the result in "out"!
*/
void quatScaleFasterOut(const quat *const restrict q, const float time, quat *const restrict out){
	float x = q->w;
	const float sign = (x >= 0.f) ? 1.f : (x = -x, -1.f);

	const float xm1 = x - 1.f;
	const float d = 1.f - time;
	const float sqrD = d * d;
	const float sqrT = time * time;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.f + bD[0] * (1.f + bD[1] * (1.f + bD[2] * (1.f + bD[3] * (
		1.f + bD[4] * (1.f + bD[5] * (1.f + bD[6] * (1.f + bD[7])))))))
	);
	const float cT = sign * time * (
		1.f + bT[0] * (1.f + bT[1] * (1.f + bT[2] * (1.f + bT[3] * (
		1.f + bT[4] * (1.f + bT[5] * (1.f + bT[6] * (1.f + bT[7])))))))
	);


	quatMultiplySOut(q, cD, out);
	out->w += cT;

	quatNormalizeQuatFast(out);
}

// Scale a quaternion using an even more optimized algorithm!
quat quatScaleFasterC(quat q, const float time){
	float x = q.w;
	const float sign = (x >= 0.f) ? 1.f : (x = -x, -1.f);

	const float xm1 = x - 1.f;
	const float d = 1.f - time;
	const float sqrD = d * d;
	const float sqrT = time * time;

	const float bD[8] = {
		(u[0] * sqrD - v[0]) * xm1,
		(u[1] * sqrD - v[1]) * xm1,
		(u[2] * sqrD - v[2]) * xm1,
		(u[3] * sqrD - v[3]) * xm1,
		(u[4] * sqrD - v[4]) * xm1,
		(u[5] * sqrD - v[5]) * xm1,
		(u[6] * sqrD - v[6]) * xm1,
		(u[7] * sqrD - v[7]) * xm1
	};
	const float bT[8] = {
		(u[0] * sqrT - v[0]) * xm1,
		(u[1] * sqrT - v[1]) * xm1,
		(u[2] * sqrT - v[2]) * xm1,
		(u[3] * sqrT - v[3]) * xm1,
		(u[4] * sqrT - v[4]) * xm1,
		(u[5] * sqrT - v[5]) * xm1,
		(u[6] * sqrT - v[6]) * xm1,
		(u[7] * sqrT - v[7]) * xm1
	};

	const float cD = d * (
		1.f + bD[0] * (1.f + bD[1] * (1.f + bD[2] * (1.f + bD[3] * (
		1.f + bD[4] * (1.f + bD[5] * (1.f + bD[6] * (1.f + bD[7])))))))
	);
	const float cT = sign * time * (
		1.f + bT[0] * (1.f + bT[1] * (1.f + bT[2] * (1.f + bT[3] * (
		1.f + bT[4] * (1.f + bT[5] * (1.f + bT[6] * (1.f + bT[7])))))))
	);


	q = quatMultiplySC(q, cD);
	q.w += cT;

	return(quatNormalizeQuatFastC(q));
}


/*
** Decompose a quaternion into its swing and twist components.
** This results in a swing quaternion "s" and twist quaternion
** "t" such that q = s*t. We also assume that the twist axis
** "v" has been normalized.
**
** This implementation negates twist when the dot product is
** negative to ensure that it points in the same direction as "v".
**
** Based off Przemyslaw Dobrowolski's implementation given
** in Swing-Twist Decomposition in Clifford Algebra (2015).
*/
void quatSwingTwist(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	//
	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	const float l = (u < 0.f) ? -invSqrtFast(q->w*q->w + u*u) : invSqrtFast(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatInitSet(t, v->x*u, v->y*u, v->z*u, q->w*l);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatMultiplyByQuatConjOut(*q, *t, s);
}

void quatSwingTwistC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3C(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	//
	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	const float l = (u < 0.f) ? -invSqrtFast(q.w*q.w + u*u) : invSqrtFast(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	*t = quatInitSetC(v.x*u, v.y*u, v.z*u, q.w*l);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatMultiplyByQuatConjC(q, *t);
}

/*
** Decompose a quaternion into its swing and twist components.
** This results in a swing quaternion "s" and twist quaternion
** "t" such that q = s*t. We also assume that the twist axis
** "v" has been normalized.
**
** Based off Przemyslaw Dobrowolski's implementation given
** in Swing-Twist Decomposition in Clifford Algebra (2015).
*/
void quatSwingTwistFast(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = invSqrtFast(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatInitSet(t, v->x*u, v->y*u, v->z*u, q->w*l);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatMultiplyByQuatConjOut(*q, *t, s);
}

void quatSwingTwistFastC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3C(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = invSqrtFast(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	*t = quatInitSetC(v.x*u, v.y*u, v.z*u, q.w*l);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatMultiplyByQuatConjC(q, *t);
}


/*
** Differentiate the quaternion
** "q" with angular velocity "w".
**
** dq/dt = 0.5f * quat(w.xyz, 0.f) * q
*/
void quatDifferentiate(quat *const restrict q, const vec3 *const restrict w){
	const quat spin = {
		.x = w->x * 0.5f,
		.y = w->y * 0.5f,
		.z = w->z * 0.5f,
		.w = 0.f
	};

	quatMultiplyQuatBy(q, spin);
}

/*
** Differentiate the quaternion "q" with angular
** velocity "w", then store the result in "out".
**
** dq/dt = 0.5f * quat(w.xyz, 0.f) * q
*/
void quatDifferentiateOut(const quat *const restrict q, const vec3 *const restrict w, quat *const restrict out){
	const quat spin = {
		.x = w->x * 0.5f,
		.y = w->y * 0.5f,
		.z = w->z * 0.5f,
		.w = 0.f
	};

	quatMultiplyByQuatOut(spin, *q, out);
}

/*
** Differentiate the quaternion
** "q" with angular velocity "w".
**
** dq/dt = 0.5f * quat(w.xyz, 0.f) * q
*/
quat quatDifferentiateC(const quat q, const vec3 w){
	const quat spin = {
		.x = w.x * 0.5f,
		.y = w.y * 0.5f,
		.z = w.z * 0.5f,
		.w = 0.f
	};

	return(quatMultiplyByQuatC(spin, q));
}

/*
** Integrate the quaternion "q" with angular
** velocity "w" using the step size "dt".
**
** q_(n + 1) = q_n + dq/dt * dt
*/
void quatIntegrate(quat *const restrict q, const vec3 *const restrict w, float dt){
	quat spin;

	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	quatInitSet(&spin, w->x * dt, w->y * dt, w->z * dt, 0.f);

	quatMultiplyByQuat(&spin, *q);
	quatAddQuat(q, &spin);
}

/*
** Integrate the quaternion "q" with angular velocity "w"
** using the step size "dt" and store the result in "out".
**
** q_(n + 1) = q_n + dq/dt * dt
*/
void quatIntegrateOut(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict out){
	quat spin;

	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	quatInitSet(&spin, w->x * dt, w->y * dt, w->z * dt, 0.f);

	quatMultiplyByQuat(&spin, *q);
	quatAddQuatOut(q, &spin, out);
}

/*
** Integrate the quaternion "q" with angular
** velocity "w" using the step size "dt".
**
** q_(n + 1) = q_n + dq/dt * dt
*/
quat quatIntegrateC(const quat q, const vec3 w, float dt){
	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	return(quatAddQuatC(q, quatMultiplyByQuatC(quatInitSetC(w.x * dt, w.y * dt, w.z * dt, 0.f), q)));
}