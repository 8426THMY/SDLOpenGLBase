#include "quat.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


// We'll do linear interpolation if the angle between
// the two quaternions is less than half a degree.
#define QUAT_LERP_THRESHOLD cos(1.f*DEG_TO_RAD)


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
quat quatInitZeroR(){
	quat q;
	memset(&q, 0.f, sizeof(q));

	return(q);
}

// Initialize the quaternion to an identity quaternion!
void quatInitIdentity(quat *const restrict q){
	*q = g_quatIdentity;
}

// Initialize the quaternion to an identity quaternion!
quat quatInitIdentityR(){
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
quat quatInitSetR(const float x, const float y, const float z, const float w){
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

	q->x = cz * cy * sx - sz * sy * cx;
	q->y = cz * sy * cx + sz * cy * sx;
	q->z = sz * cy * cx - cz * sy * sx;
	q->w = cz * cy * cx + sz * sy * sx;
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZYX.
*/
quat quatInitEulerRadR(const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	quat q;

	q.x = cz * cy * sx - sz * sy * cx;
	q.y = cz * sy * cx + sz * cy * sx;
	q.z = sz * cy * cx - cz * sy * sx;
	q.w = cz * cy * cx + sz * sy * sx;

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
quat quatInitEulerDegR(const float x, const float y, const float z){
	return(quatInitEulerRadR(x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD));
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
quat quatInitEulerVec3RadR(const vec3 v){
	return(quatInitEulerRadR(v.x, v.y, v.z));
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
quat quatInitEulerVec3DegR(const vec3 v){
	return(quatInitEulerRadR(v.x * DEG_TO_RAD, v.y * DEG_TO_RAD, v.z * DEG_TO_RAD));
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
quat quatAddR(quat q, const float x, const float y, const float z, const float w){
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
quat quatAddSR(quat q, const float x){
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
quat quatAddQuatR(quat q, const vec4 v){
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
quat quatSubtractR(quat q, const float x, const float y, const float z, const float w){
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
quat quatSubtractFromR(quat q, const float x, const float y, const float z, const float w){
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
quat quatSubtractSFromR(quat q, const float x){
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
quat quatSubtractFromSR(quat q, const float x){
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
quat quatSubtractQuatFromR(quat q, const vec4 v){
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
quat quatMultiplySR(quat q, const float x){
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
quat quatMultiplyVec4R(quat q, const vec4 v){
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
quat quatDivideBySR(quat q, const float x){
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
quat quatDivideSByR(quat q, const float x){
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
quat quatDivideSByFastR(quat q, const float x){
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
quat quatDivideByVec4R(quat q, const vec4 v){
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
quat quatDivideVec4ByR(quat q, const vec4 v){
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
quat quatDivideByVec4FastR(quat q, const vec4 v){
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
quat quatDivideVec4ByFastR(quat q, const vec4 v){
	q.x = v.x / q.x;
	q.y = v.y / q.y;
	q.z = v.z / q.z;
	q.w = v.w / q.w;

	return(q);
}


// Multiply "q1" by "q2" (apply a rotation of "q1" to "q2")!
void quatMultiplyByQuat(quat *const restrict q1, const quat *const restrict q2){
	quat tempQuat = *q1;

	q1->x = tempQuat.w * q2->x + tempQuat.x * q2->w + tempQuat.y * q2->z - tempQuat.z * q2->y;
	q1->y = tempQuat.w * q2->y + tempQuat.y * q2->w + tempQuat.z * q2->x - tempQuat.x * q2->z;
	q1->z = tempQuat.w * q2->z + tempQuat.z * q2->w + tempQuat.x * q2->y - tempQuat.y * q2->x;
	q1->w = tempQuat.w * q2->w - tempQuat.x * q2->x - tempQuat.y * q2->y - tempQuat.z * q2->z;
}

// Multiply "q2" by "q1" (apply a rotation of "q2" to "q1")!
void quatMultiplyQuatBy(quat *const restrict q1, const quat *const restrict q2){
	quat tempQuat = *q1;

	q1->x = q2->w * tempQuat.x + q2->x * tempQuat.w + q2->y * tempQuat.z - q2->z * tempQuat.y;
	q1->y = q2->w * tempQuat.y + q2->y * tempQuat.w + q2->z * tempQuat.x - q2->x * tempQuat.z;
	q1->z = q2->w * tempQuat.z + q2->z * tempQuat.w + q2->x * tempQuat.y - q2->y * tempQuat.x;
	q1->w = q2->w * tempQuat.w - q2->x * tempQuat.x - q2->y * tempQuat.y - q2->z * tempQuat.z;
}

// Multiply "q1" by "q2" (apply a rotation of "q1" to "q2") and store the result in "out"!
void quatMultiplyByQuatOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	out->y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
	out->z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
	out->w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
}

// Multiply "q1" by "q2" (apply a rotation of "q1" to "q2") and store the result in "out"! This assumes that "out" isn't "q1" or "q2".
void quatMultiplyByQuatFastOut(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict out){
	out->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
	out->y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
	out->z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
	out->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
}

// Multiply "q1" by "q2" (apply a rotation of "q1" to "q2")!
quat quatMultiplyByQuatR(const quat q1, const quat q2){
	quat out;

	out.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	out.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
	out.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
	out.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

	return(out);
}


// Apply a quaternion rotation to a vec3!
void quatRotateVec3(const quat *const restrict q, vec3 *const restrict v){
	float qvDot = vec3DotVec3Float(v, q->x, q->y, q->z);
	const float qNorm = q->w * q->w - vec3Norm(q->x, q->y, q->z);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	v->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	v->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	v->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Apply a quaternion rotation to a vec3!
void quatRotateVec3Out(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	float qvDot = vec3DotVec3Float(v, q->x, q->y, q->z);
	const float qNorm = q->w * q->w - vec3Norm(q->x, q->y, q->z);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Apply a quaternion rotation to a vec3!
vec3 quatRotateVec3R(const quat q, vec3 v){
	float qvDot = vec3DotVec3FloatR(v, q.x, q.y, q.z);
	const float qNorm = q.w * q.w - vec3Norm(q.x, q.y, q.z);
	const vec3 qvCross = vec3MultiplySR(vec3CrossFloatVec3R(q.x, q.y, q.z, v), q.w + q.w);
	qvDot += qvDot;

	v.x = qvDot * q.x + qNorm * v.x + qvCross.x;
	v.y = qvDot * q.y + qNorm * v.y + qvCross.y;
	v.z = qvDot * q.z + qNorm * v.z + qvCross.z;

	return(v);
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3Inverse(const quat *const restrict q, vec3 *const restrict v){
	float qvDot = vec3DotVec3Float(v, q->x, q->y, q->z);
	const float qNorm = q->w * q->w - vec3Norm(q->x, q->y, q->z);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	vec3MultiplyS(&qvCross, -q->w - q->w);

	v->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	v->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	v->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3InverseOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	float qvDot = vec3DotVec3Float(v, q->x, q->y, q->z);
	const float qNorm = q->w * q->w - vec3Norm(q->x, q->y, q->z);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	vec3MultiplyS(&qvCross, -q->w - q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

// Undo a quaternion rotation on a vec3!
vec3 quatRotateVec3InverseR(const quat q, vec3 v){
	float qvDot = vec3DotVec3FloatR(v, q.x, q.y, q.z);
	const float qNorm = q.w * q.w - vec3Norm(q.x, q.y, q.z);
	const vec3 qvCross = vec3MultiplySR(vec3CrossFloatVec3R(q.x, q.y, q.z, v), -q.w - q.w);
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
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	qvCross.x += q->w * v->x;
	qvCross.y += q->w * v->y;
	qvCross.z += q->w * v->z;
	vec3Cross(q->x, q->y, q->z, qvCross.x, qvCross.y, qvCross.z, &qvCross);

	v->x = qvCross.x + qvCross.x + v->x;
	v->y = qvCross.y + qvCross.y + v->y;
	v->z = qvCross.z + qvCross.z + v->z;
}

// Apply a quaternion rotation to a vec3!
void quatRotateVec3FastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	vec3 qvCross;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	qvCross.x += q->w * v->x;
	qvCross.y += q->w * v->y;
	qvCross.z += q->w * v->z;
	vec3Cross(q->x, q->y, q->z, qvCross.x, qvCross.y, qvCross.z, &qvCross);

	out->x = qvCross.x + qvCross.x + v->x;
	out->y = qvCross.y + qvCross.y + v->y;
	out->z = qvCross.z + qvCross.z + v->z;
}

// Apply a quaternion rotation to a vec3!
vec3 quatRotateVec3FastR(const quat q, vec3 v){
	vec3 qvCross = vec3CrossFloatVec3R(q.x, q.y, q.z, v);
	qvCross.x += q.w * v.x;
	qvCross.y += q.w * v.y;
	qvCross.z += q.w * v.z;
	qvCross = vec3CrossFloatVec3R(q.x, q.y, q.z, qvCross);

	v.x = qvCross.x + qvCross.x + v.x;
	v.y = qvCross.y + qvCross.y + v.y;
	v.z = qvCross.z + qvCross.z + v.z;

	return(v);
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3InverseFast(const quat *const restrict q, vec3 *const restrict v){
	vec3 qvCross;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	qvCross.x -= q->w * v->x;
	qvCross.y -= q->w * v->y;
	qvCross.z -= q->w * v->z;
	vec3Cross(q->x, q->y, q->z, qvCross.x, qvCross.y, qvCross.z, &qvCross);

	v->x = qvCross.x + qvCross.x + v->x;
	v->y = qvCross.y + qvCross.y + v->y;
	v->z = qvCross.z + qvCross.z + v->z;
}

// Undo a quaternion rotation on a vec3!
void quatRotateVec3InverseFastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	vec3 qvCross;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	qvCross.x -= q->w * v->x;
	qvCross.y -= q->w * v->y;
	qvCross.z -= q->w * v->z;
	vec3Cross(q->x, q->y, q->z, qvCross.x, qvCross.y, qvCross.z, &qvCross);

	out->x = qvCross.x + qvCross.x + v->x;
	out->y = qvCross.y + qvCross.y + v->y;
	out->z = qvCross.z + qvCross.z + v->z;
}

// Undo a quaternion rotation on a vec3!
vec3 quatRotateVec3InverseFastR(const quat q, vec3 v){
	vec3 qvCross = vec3CrossFloatVec3R(q.x, q.y, q.z, v);
	qvCross.x -= q.w * v.x;
	qvCross.y -= q.w * v.y;
	qvCross.z -= q.w * v.z;
	qvCross = vec3CrossR(q.x, q.y, q.z, qvCross.x, qvCross.y, qvCross.z);

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
float quatMagnitudeQuatR(const quat q){
	return(sqrtf(quatNormQuatR(q)));
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
float quatDotQuatFloatR(const quat q, const float x, const float y, const float z, const float w){
	return(q.x * x + q.y * y + q.z * z + q.w * w);
}

// Find the dot product of two vec4s!
float quatDotQuat(const quat *const restrict q1, const quat *const restrict q2){
	return(q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w);
}

// Find the dot product of two vec4s!
float quatDotQuatR(const quat q1, const quat q2){
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
float quatNormQuatR(const quat q){
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
quat quatNormalizeR(const float x, const float y, const float z, const float w){
	const float magnitude = invSqrt(x * x + y * y + z * z + w * w);
	quat q;

	q.x = x * magnitude;
	q.y = y * magnitude;
	q.z = z * magnitude;
	q.w = w * magnitude;

	return(q);
}

// Normalize a quaternion stored as four floats!
quat quatNormalizeFastR(const float x, const float y, const float z, const float w){
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
quat quatNormalizeQuatR(quat q){
	const float magnitude = invSqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

	q.x *= magnitude;
	q.y *= magnitude;
	q.z *= magnitude;
	q.w *= magnitude;

	return(q);
}

// Normalize a quaternion!
quat quatNormalizeQuatFastR(quat q){
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
quat quatConjugateR(quat q){
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
quat quatConjugateFastR(quat q){
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
quat quatNegateR(quat q){
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	q.w = -q.w;

	return(q);
}



// Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngle(const quat *const restrict q, vec4 *const restrict out){
	if(q->w > -1.f && q->w < 1.f){
		const float scale = invSqrt(1.f - q->w * q->w);

		out->x = q->x * scale;
		out->y = q->y * scale;
		out->z = q->z * scale;
		out->w = acosf(q->w);
		out->w += out->w;
	}else{
		out->x = out->y = out->z = 1.f;
		out->w = 0.f;
	}
}

// Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngleFast(const quat *const restrict q, vec4 *const restrict out){
	if(q->w > -1.f && q->w < 1.f){
		const float scale = invSqrtFast(1.f - q->w * q->w);

		out->x = q->x * scale;
		out->y = q->y * scale;
		out->z = q->z * scale;
		out->w = acosf(q->w);
		out->w += out->w;
	}else{
		out->x = out->y = out->z = 1.f;
		out->w = 0.f;
	}
}

// Convert a quaternion to an axis and an angle!
vec4 quatToAxisAngleR(const quat q){
	vec4 v;

	if(q.w > -1.f && q.w < 1.f){
		const float scale = invSqrt(1.f - q.w * q.w);

		v.x = q.x * scale;
		v.y = q.y * scale;
		v.z = q.z * scale;
		v.w = acosf(q.w);
		v.w += v.w;
	}else{
		v.x = v.y = v.z = 1.f;
		v.w = 0.f;
	}

	return(v);
}

// Convert a quaternion to an axis and an angle!
vec4 quatToAxisAngleFastR(const quat q){
	vec4 v;

	if(q.w > -1.f && q.w < 1.f){
		const float scale = invSqrtFast(1.f - q.w * q.w);

		v.x = q.x * scale;
		v.y = q.y * scale;
		v.z = q.z * scale;
		v.w = acosf(q.w);
		v.w += v.w;
	}else{
		v.x = v.y = v.z = 1.f;
		v.w = 0.f;
	}

	return(v);
}


// Rotate a quaternion (in radians)!
void quatRotateByRad(quat *const restrict q, const float x, const float y, const float z){
	quat rot;
	quatInitEulerRad(&rot, x, y, z);
	quatMultiplyQuatBy(q, &rot);
}

// Rotate a quaternion (in radians)!
quat quatRotateByRadR(const quat q, const float x, const float y, const float z){
	return(quatMultiplyByQuatR(quatInitEulerRadR(x, y, z), q));
}

// Rotate a quaternion (in degrees)!
void quatRotateByDeg(quat *const restrict q, const float x, const float y, const float z){
	quatRotateByRad(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

// Rotate a quaternion (in degrees)!
quat quatRotateByDegR(const quat q, const float x, const float y, const float z){
	return(quatRotateByRadR(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD));
}

// Rotate a quaternion by a vec3 (in radians)!
void quatRotateByVec3Rad(quat *const restrict q, const vec3 *const restrict v){
	quatRotateByRad(q, v->x, v->y, v->z);
}

// Rotate a quaternion by a vec3 (in radians)!
quat quatRotateByVec3RadR(const quat q, const vec3 v){
	return(quatRotateByRadR(q, v.x, v.y, v.z));
}

// Rotate a quaternion by a vec3 (in degrees)!
void quatRotateByVec3Deg(quat *const restrict q, const vec3 *const restrict v){
	quatRotateByRad(q, v->x * DEG_TO_RAD, v->y * DEG_TO_RAD, v->z * DEG_TO_RAD);
}

// Rotate a quaternion by a vec3 (in degrees)!
quat quatRotateByVec3DegR(const quat q, const vec3 v){
	return(quatRotateByRadR(q, v.x * DEG_TO_RAD, v.y * DEG_TO_RAD, v.z * DEG_TO_RAD));
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
quat quatLerpR(quat q1, const quat q2, const float time){
	q1.x = lerpFloatFast(q1.x, q2.x, time);
	q1.y = lerpFloatFast(q1.y, q2.y, time);
	q1.z = lerpFloatFast(q1.z, q2.z, time);
	q1.w = lerpFloatFast(q1.w, q2.w, time);

	// It's nice to be safe... but it isn't very fast.
	return(quatNormalizeQuatR(q1));
}

/*
** Perform linear interpolation between two quaternions!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
void quatLerpFast(quat *const restrict q, const quat *const restrict offset, const float time){
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
void quatLerpFastOut(const quat *const restrict q, const quat *const restrict offset, const float time, quat *const restrict out){
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
quat quatLerpFastR(quat q, const quat offset, const float time){
	q.x = lerpDiffFast(q.x, offset.x, time);
	q.y = lerpDiffFast(q.y, offset.y, time);
	q.z = lerpDiffFast(q.z, offset.z, time);
	q.w = lerpDiffFast(q.w, offset.w, time);

	// It's nice to be safe... but it isn't very fast.
	return(quatNormalizeQuatR(q));
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
quat quatSlerpR(quat q1, quat q2, const float time){
	float cosTheta = quatDotQuatR(q1, q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		q2 = quatNegateR(q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		return(quatLerpR(q1, q2, time));
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
		return(quatNormalizeQuatR(q1));
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
quat quatSlerpFastR(quat q1, quat q2, const float time){
	float cosTheta = quatDotQuatR(q1, q2);

	// Prevent the interpolation from taking the longer path.
	if(cosTheta < 0.f){
		cosTheta = -cosTheta;
		q2 = quatNegateR(q2);
	}

	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(cosTheta > QUAT_LERP_THRESHOLD){
		return(quatLerpR(q1, q2, time));
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
		return(quatNormalizeQuatFastR(q1));
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
quat quatSlerpFasterR(quat q1, const quat q2, const float time){
	float x = quatDotQuatR(q1, q2);
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

	return(quatNormalizeQuatFastR(q1));
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
quat quatScaleR(quat q, const float x){
	const float absCosTheta = fabsf(q.w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q.w < 0.f){
			return(quatMultiplySR(q, -x));
		}
		return(quatMultiplySR(q, x));
	}else{
		const float theta = acosf(absCosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * invSinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q.w >= 0.f) ? sinf(theta * x) * invSinTheta : -(sinf(theta * x) * invSinTheta);

		q = quatMultiplySR(q, sinThetaT);
		q.w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		return(quatNormalizeQuatR(q));
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
quat quatScaleFastR(quat q, const float x){
	const float absCosTheta = fabsf(q.w);
	// We can perform linear interpolation if the angle is
	// small enough (in this case, less than half a degree).
	if(absCosTheta > QUAT_LERP_THRESHOLD){
		// Prevent the interpolation from taking the longer path.
		if(q.w < 0.f){
			return(quatMultiplySR(q, -x));
		}
		return(quatMultiplySR(q, x));
	}else{
		const float theta = acosf(absCosTheta);
		const float sinTheta = invSqrtFast(1.f - absCosTheta * absCosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - x)) * sinTheta;
		// Prevent the interpolation from taking the longer path.
		const float sinThetaT = (q.w >= 0.f) ? sinf(theta * x) * sinTheta : -(sinf(theta * x) * sinTheta);

		q = quatMultiplySR(q, sinThetaT);
		q.w += sinThetaInvT;

		// It's nice to be safe... but it isn't very fast.
		return(quatNormalizeQuatFastR(q));
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
quat quatScaleFasterR(quat q, const float time){
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


	q = quatMultiplySR(q, cD);
	q.w += cT;

	return(quatNormalizeQuatFastR(q));
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

	quatMultiplyQuatBy(q, &spin);
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

	quatMultiplyByQuatFastOut(&spin, q, out);
}

/*
** Differentiate the quaternion
** "q" with angular velocity "w".
**
** dq/dt = 0.5f * quat(w.xyz, 0.f) * q
*/
quat quatDifferentiateR(const quat q, const vec3 w){
	const quat spin = {
		.x = w.x * 0.5f,
		.y = w.y * 0.5f,
		.z = w.z * 0.5f,
		.w = 0.f
	};

	return(quatMultiplyByQuatR(spin, q));
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

	quatMultiplyByQuat(&spin, q);
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

	quatMultiplyByQuat(&spin, q);
	quatAddQuatOut(q, &spin, out);
}

/*
** Integrate the quaternion "q" with angular
** velocity "w" using the step size "dt".
**
** q_(n + 1) = q_n + dq/dt * dt
*/
quat quatIntegrateR(const quat q, const vec3 w, float dt){
	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	return(quatAddQuatR(q, quatMultiplyByQuatR(quatInitSetR(w.x * dt, w.y * dt, w.z * dt, 0.f), q)));
}