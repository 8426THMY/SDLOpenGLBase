#include "quat.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


#define MATH_NORMALIZE_EPSILON 0.000001f

#define QUAT_SINGULARITY_THRESHOLD 0.0001f
#define QUAT_SINGULARITY_THRESHOLD_SQUARED (QUAT_SINGULARITY_THRESHOLD*QUAT_SINGULARITY_THRESHOLD)
// We'll do linear interpolation if the angle between
// the two quaternions is less than half a degree.
#define QUAT_LERP_THRESHOLD cosf(DEG_TO_RAD(1.f))


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
void quatInitSet(quat *const restrict q, const float w, const float x, const float y, const float z){
	q->x = x;
	q->y = y;
	q->z = z;
	q->w = w;
}

// Initialize the quaternion's values to the ones specified!
quat quatInitSetC(const float w, const float x, const float y, const float z){
	quat q;

	q.x = x;
	q.y = y;
	q.z = z;
	q.w = w;

	return(q);
}

// Initialize a quaternion from a vector and a float!
void quatInitVec3F(quat *const restrict q, const float w, const vec3 *const restrict v){
	*((vec3 *)&q->x) = *v;
	q->w = w;
}

// Initialize a quaternion from a vector and a float!
quat quatInitVec3FC(const float w, const vec3 v){
	const quat q = {.x = v.x, .y = v.y, .z = v.z, .w = w};
	return(q);
}

// Initialize a quaternion from an axis-angle representation!
void quatInitAxisAngle(quat *const restrict q, const vec3 *const restrict v, const float t){
	const float tHalf = t*0.5f;
	const float st = sinf(tHalf);
	q->x = st*v->x;
	q->y = st*v->y;
	q->z = st*v->z;
	q->w = cosf(tHalf);
}

// Initialize a quaternion from an axis-angle representation!
quat quatInitAxisAngleC(const vec3 v, const float t){
	const float tHalf = t*0.5f;
	const float st = sinf(tHalf);
	const quat q = {.x = st*v.x, .y = st*v.y, .z = st*v.z, .w = cosf(tHalf)};
	return(q);
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is XYZ.
*/
void quatInitEulerXYZ(quat *const restrict q, const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const quat qxy = {
		.x = cy * sx,
		.y = sy * cx,
		.z = -sy * sx,
		.w = cy * cx
	};

	q->x = cz * qxy.x - sz * qxy.y;
	q->y = cz * qxy.y + sz * qxy.x;
	q->z = cz * qxy.z + sz * qxy.w;
	q->w = cz * qxy.w - sz * qxy.z;
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZXY, which is useful for cameras.
*/
void quatInitEulerZXY(quat *const restrict q, const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const quat qzx = {
		.x = sx * cz,
		.y = -sx * sz,
		.z = cx * sz,
		.w = cx * cz
	};

	q->x = cy * qzx.x + sy * qzx.z;
	q->y = cy * qzx.y + sy * qzx.w;
	q->z = cy * qzx.z - sy * qzx.x;
	q->w = cy * qzx.w - sy * qzx.y;
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is XYZ.
*/
quat quatInitEulerXYZC(const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const quat qxy = {
		.x = cy * sx,
		.y = sy * cx,
		.z = -sy * sx,
		.w = cy * cx
	};

	const quat q = {
		.x = cz * qxy.x - sz * qxy.y,
		.y = cz * qxy.y + sz * qxy.x,
		.z = cz * qxy.z + sz * qxy.w,
		.w = cz * qxy.w - sz * qxy.z
	};

	return(q);
}

/*
** Initialize the quaternion's values to the Euler angles specified (in radians)!
** The order of rotations is ZXY, which is useful for cameras.
*/
quat quatInitEulerZXYC(const float x, const float y, const float z){
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;
	const float cx = cosf(hx);
	const float cy = cosf(hy);
	const float cz = cosf(hz);
	const float sx = sinf(hx);
	const float sy = sinf(hy);
	const float sz = sinf(hz);
	const quat qzx = {
		.x = sx * cz,
		.y = -sx * sz,
		.z = cx * sz,
		.w = cx * cz
	};

	const quat q = {
		.x = cy * qzx.x + sy * qzx.z,
		.y = cy * qzx.y + sy * qzx.w,
		.z = cy * qzx.z - sy * qzx.x,
		.w = cy * qzx.w - sy * qzx.y
	};

	return(q);
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is XYZ.
*/
void quatInitEulerVec3XYZ(quat *const restrict q, const vec3 *const restrict v){
	quatInitEulerXYZ(q, v->x, v->y, v->z);
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is ZXY, which is useful for cameras.
*/
void quatInitEulerVec3ZXY(quat *const restrict q, const vec3 *const restrict v){
	quatInitEulerZXY(q, v->x, v->y, v->z);
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is XYZ.
*/
quat quatInitEulerVec3XYZC(const vec3 v){
	return(quatInitEulerXYZC(v.x, v.y, v.z));
}

/*
** Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
** The order of rotations is ZXY, which is useful for cameras.
*/
quat quatInitEulerVec3ZXYC(const vec3 v){
	return(quatInitEulerZXYC(v.x, v.y, v.z));
}


/*
** Return whether a unit quaternion is sufficiently close to identity.
** Note that we only really need the 'w' component to check this.
*/
return_t quatIsIdentity(const float w){
	return(w > 1.f - MATH_NORMALIZE_EPSILON);
}


// Add a quat stored as four floats to "q"!
void quatAdd(quat *const restrict q, const float w, const float x, const float y, const float z){
	q->x += x;
	q->y += y;
	q->z += z;
	q->w += w;
}

// Add a quat stored as four floats to "q" and store the result in "out"!
void quatAddOut(const quat *const restrict q, const float w, const float x, const float y, const float z, quat *const restrict out){
	out->x = q->x + x;
	out->y = q->y + y;
	out->z = q->z + z;
	out->w = q->w + w;
}

// Add a quat stored as four floats to "q"!
quat quatAddC(quat q, const float w, const float x, const float y, const float z){
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
void quatAddVec4(quat *const restrict q, const vec4 *const restrict v){
	q->x += v->x;
	q->y += v->y;
	q->z += v->z;
	q->w += v->w;
}

// Add "v" to "q" and store the result in "out"!
void quatAddVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x + v->x;
	out->y = q->y + v->y;
	out->z = q->z + v->z;
	out->w = q->w + v->w;
}

// Add "v" to "q" and return the result!
quat quatAddVec4C(quat q, const vec4 v){
	q.x += v.x;
	q.y += v.y;
	q.z += v.z;
	q.w += v.w;

	return(q);
}

// Add "q2" to "q1"!
void quatAddQuat(quat *const restrict q1, const quat *const restrict q2){
	q1->x += q2->x;
	q1->y += q2->y;
	q1->z += q2->z;
	q1->w += q2->w;
}

// Add "q2" to "q1" and store the result in "out"!
void quatAddQuatOut(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict out){
	out->x = q1->x + q2->x;
	out->y = q1->y + q2->y;
	out->z = q1->z + q2->z;
	out->w = q1->w + q2->w;
}

// Add "q2" to "q1" and return the result!
quat quatAddQuatC(quat q1, const quat q2){
	q1.x += q2.x;
	q1.y += q2.y;
	q1.z += q2.z;
	q1.w += q2.w;

	return(q1);
}

// Subtract a quat stored as four floats from "q"!
void quatSubtract(quat *const restrict q, const float w, const float x, const float y, const float z){
	q->x -= x;
	q->y -= y;
	q->z -= z;
	q->w -= w;
}

// Subtract a quat stored as four floats from "q" and store the result in "out"!
void quatSubtractOut(const quat *const restrict q, const float w, const float x, const float y, const float z, quat *const restrict out){
	out->x = q->x - x;
	out->y = q->y - y;
	out->z = q->z - z;
	out->w = q->w - w;
}

// Subtract a quat stored as four floats from "q"!
quat quatSubtractC(quat q, const float w, const float x, const float y, const float z){
	q.x -= x;
	q.y -= y;
	q.z -= z;
	q.w -= w;

	return(q);
}

// Subtract "q" from a quat stored as four floats!
void quatSubtractFrom(quat *const restrict q, const float w, const float x, const float y, const float z){
	q->x = x - q->x;
	q->y = y - q->y;
	q->z = z - q->z;
	q->w = w - q->w;
}

// Subtract "q" from a quat stored as four floats and store the result in "out"!
void quatSubtractFromOut(const quat *const restrict q, const float w, const float x, const float y, const float z, quat *const restrict out){
	out->x = x - q->x;
	out->y = y - q->y;
	out->z = z - q->z;
	out->w = w - q->w;
}

// Subtract "q" from a quat stored as four floats!
quat quatSubtractFromC(quat q, const float w, const float x, const float y, const float z){
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

// Subtract "v" from "q" and store the result in "q"!
void quatSubtractVec4P1(quat *const restrict q, const vec4 *const restrict v){
	q->x -= v->x;
	q->y -= v->y;
	q->z -= v->z;
	q->w -= v->w;
}

// Subtract "v" from "q" and store the result in "v"!
void quatSubtractVec4P2(const quat *const restrict q, vec4 *const restrict v){
	v->x = q->x - v->x;
	v->y = q->y - v->y;
	v->z = q->z - v->z;
	v->w = q->w - v->w;
}

// Subtract "v" from "q" and store the result in "out"!
void quatSubtractVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x - v->x;
	out->y = q->y - v->y;
	out->z = q->z - v->z;
	out->w = q->w - v->w;
}

// Subtract "v" from "q" and return the result!
quat quatSubtractVec4C(quat q, const vec4 v){
	q.x -= v.x;
	q.y -= v.y;
	q.z -= v.z;
	q.w -= v.w;

	return(q);
}

// Subtract "q" from "v" and store the result in "v"!
void vec4SubtractQuatP1(vec4 *const restrict v, const quat *const restrict q){
	v->x -= q->x;
	v->y -= q->y;
	v->z -= q->z;
	v->w -= q->w;
}

// Subtract "q" from "v" and store the result in "v"!
void vec4SubtractQuatP2(const vec4 *const restrict v, quat *const restrict q){
	q->x = v->x - q->x;
	q->y = v->y - q->y;
	q->z = v->z - q->z;
	q->w = v->w - q->w;
}

// Subtract "q" from "v" and store the result in "out"!
void vec4SubtractQuatOut(const vec4 *const restrict v, const quat *const restrict q, vec4 *const restrict out){
	out->x = v->x - q->x;
	out->y = v->y - q->y;
	out->z = v->z - q->z;
	out->w = v->w - q->w;
}

// Subtract "q" from "v" and return the result!
vec4 vec4SubtractQuatC(vec4 v, const quat q){
	v.x -= q.x;
	v.y -= q.y;
	v.z -= q.z;
	v.w -= q.w;

	return(v);
}

// Subtract "q2" from "q1" and store the result in "q1"!
void quatSubtractQuatP1(quat *const restrict q1, const quat *const restrict q2){
	q1->x -= q2->x;
	q1->y -= q2->y;
	q1->z -= q2->z;
	q1->w -= q2->w;
}

// Subtract "q2" from "q1" and store the result in "q2"!
void quatSubtractQuatP2(const quat *const restrict q1, quat *const restrict q2){
	q2->x = q1->x - q2->x;
	q2->y = q1->y - q2->y;
	q2->z = q1->z - q2->z;
	q2->w = q1->w - q2->w;
}

// Subtract "q2" from "q1" and store the result in "out"!
void quatSubtractQuatOut(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict out){
	out->x = q1->x - q2->x;
	out->y = q1->y - q2->y;
	out->z = q1->z - q2->z;
	out->w = q1->w - q2->w;
}

// Subtract "q2" from "q1" and return the result!
quat quatSubtractQuatC(quat q1, const quat q2){
	q1.x -= q2.x;
	q1.y -= q2.y;
	q1.z -= q2.z;
	q1.w -= q2.w;

	return(q1);
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

// Divide "q" by "v" and store the result in "q"!
void quatDivideVec4P1(quat *const restrict q, const vec4 *const restrict v){
	q->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	q->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	q->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	q->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

// Divide "q" by "v" and store the result in "v"!
void quatDivideVec4P2(const quat *const restrict q, vec4 *const restrict v){
	v->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	v->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	v->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	v->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

// Divide "q" by "v" and store the result in "out"!
void quatDivideVec4Out(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	out->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	out->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	out->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

// Divide "q" by "v" and return the result!
quat quatDivideVec4C(quat q, const vec4 v){
	q.x = (v.x != 0.f) ? q.x / v.x : 0.f;
	q.y = (v.y != 0.f) ? q.y / v.y : 0.f;
	q.z = (v.z != 0.f) ? q.z / v.z : 0.f;
	q.w = (v.w != 0.f) ? q.w / v.w : 0.f;

	return(q);
}

// Divide "v" by "q" and store the result in "v"!
void vec4DivideQuatP1(vec4 *const restrict v, const quat *const restrict q){
	v->x = (q->x != 0.f) ? v->x / q->x : 0.f;
	v->y = (q->y != 0.f) ? v->y / q->y : 0.f;
	v->z = (q->z != 0.f) ? v->z / q->z : 0.f;
	v->w = (q->w != 0.f) ? v->w / q->w : 0.f;
}

// Divide "v" by "q" and store the result in "q"!
void vec4DivideQuatP2(const vec4 *const restrict v, quat *const restrict q){
	q->x = (q->x != 0.f) ? v->x / q->x : 0.f;
	q->y = (q->y != 0.f) ? v->y / q->y : 0.f;
	q->z = (q->z != 0.f) ? v->z / q->z : 0.f;
	q->w = (q->w != 0.f) ? v->w / q->w : 0.f;
}

// Divide "v" by "q" and store the result in "out"!
void vec4DivideQuatOut(const vec4 *const restrict v, const quat *const restrict q, vec4 *const restrict out){
	out->x = (q->x != 0.f) ? v->x / q->x : 0.f;
	out->y = (q->y != 0.f) ? v->y / q->y : 0.f;
	out->z = (q->z != 0.f) ? v->z / q->z : 0.f;
	out->w = (q->w != 0.f) ? v->w / q->w : 0.f;
}

// Divide "v" by "q" and return the result!
vec4 vec4DivideQuatC(vec4 v, const quat q){
	v.x = (q.x != 0.f) ? v.x / q.x : 0.f;
	v.y = (q.y != 0.f) ? v.y / q.y : 0.f;
	v.z = (q.z != 0.f) ? v.z / q.z : 0.f;
	v.w = (q.w != 0.f) ? v.w / q.w : 0.f;

	return(v);
}

// Divide "q1" by "q2" and store the result in "q1"!
void quatDivideQuatP1(quat *const restrict q1, const quat *const restrict q2){
	q1->x = (q2->x != 0.f) ? q1->x / q2->x : 0.f;
	q1->y = (q2->y != 0.f) ? q1->y / q2->y : 0.f;
	q1->z = (q2->z != 0.f) ? q1->z / q2->z : 0.f;
	q1->w = (q2->w != 0.f) ? q1->w / q2->w : 0.f;
}

// Divide "q1" by "q2" and store the result in "q2"!
void quatDivideQuatP2(const quat *const restrict q1, quat *const restrict q2){
	q2->x = (q2->x != 0.f) ? q1->x / q2->x : 0.f;
	q2->y = (q2->y != 0.f) ? q1->y / q2->y : 0.f;
	q2->z = (q2->z != 0.f) ? q1->z / q2->z : 0.f;
	q2->w = (q2->w != 0.f) ? q1->w / q2->w : 0.f;
}

// Divide "q1" by "q2" and store the result in "out"!
void quatDivideQuatOut(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict out){
	out->x = (q2->x != 0.f) ? q1->x / q2->x : 0.f;
	out->y = (q2->y != 0.f) ? q1->y / q2->y : 0.f;
	out->z = (q2->z != 0.f) ? q1->z / q2->z : 0.f;
	out->w = (q2->w != 0.f) ? q1->w / q2->w : 0.f;
}

// Divide "q1" by "q2" and return the result!
quat quatDivideQuatC(quat q1, const quat q2){
	q1.x = (q2.x != 0.f) ? q1.x / q2.x : 0.f;
	q1.y = (q2.y != 0.f) ? q1.y / q2.y : 0.f;
	q1.z = (q2.z != 0.f) ? q1.z / q2.z : 0.f;
	q1.w = (q2.w != 0.f) ? q1.w / q2.w : 0.f;

	return(q1);
}

/*
** Divide "q" by "v" and store the result in "q"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideVec4FastP1(quat *const restrict q, const vec4 *const restrict v){
	q->x /= v->x;
	q->y /= v->y;
	q->z /= v->z;
	q->w /= v->w;
}

/*
** Divide "q" by "v" and store the result in "v"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideVec4FastP2(const quat *const restrict q, vec4 *const restrict v){
	v->x = q->x / v->x;
	v->y = q->y / v->y;
	v->z = q->z / v->z;
	v->w = q->w / v->w;
}

/*
** Divide "q" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideVec4FastOut(const quat *const restrict q, const vec4 *const restrict v, quat *const restrict out){
	out->x = q->x / v->x;
	out->y = q->y / v->y;
	out->z = q->z / v->z;
	out->w = q->w / v->w;
}

/*
** Divide "q" by "v" and return the result!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
quat quatDivideVec4FastC(quat q, const vec4 v){
	q.x /= v.x;
	q.y /= v.y;
	q.z /= v.z;
	q.w /= v.w;

	return(q);
}

/*
** Divide "v" by "q" and store the result in "v"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec4DivideQuatFastP1(vec4 *const restrict v, const quat *const restrict q){
	v->x /= q->x;
	v->y /= q->y;
	v->z /= q->z;
	v->w /= q->w;
}

/*
** Divide "v" by "q" and store the result in "q"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void vec4DivideQuatFastP2(const vec4 *const restrict v, quat *const restrict q){
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
void vec4DivideQuatFastOut(const vec4 *const restrict v, const quat *const restrict q, vec4 *const restrict out){
	out->x = v->x / q->x;
	out->y = v->y / q->y;
	out->z = v->z / q->z;
	out->w = v->w / q->w;
}

/*
** Divide "q" by "v" and return the result!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
vec4 vec4DivideQuatFastC(vec4 v, const quat q){
	v.x /= q.x;
	v.y /= q.y;
	v.z /= q.z;
	v.w /= q.w;

	return(v);
}

/*
** Divide "q1" by "q2" and store the result in "q1"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideQuatFastP1(quat *const restrict q1, const quat *const restrict q2){
	q1->x /= q2->x;
	q1->y /= q2->y;
	q1->z /= q2->z;
	q1->w /= q2->w;
}

/*
** Divide "q1" by "q2" and store the result in "q2"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideQuatFastP2(const quat *const restrict q1, quat *const restrict q2){
	q2->x = q1->x / q2->x;
	q2->y = q1->y / q2->y;
	q2->z = q1->z / q2->z;
	q2->w = q1->w / q2->w;
}

/*
** Divide "q1" by "q2" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideQuatFastOut(const quat *const restrict q1, const quat *const restrict q2, quat *const restrict out){
	out->x = q1->x / q2->x;
	out->y = q1->y / q2->y;
	out->z = q1->z / q2->z;
	out->w = q1->w / q2->w;
}

/*
** Divide "q1" by "q2" and return the result!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
quat quatDivideQuatFastC(quat q1, const quat q2){
	q1.x /= q2.x;
	q1.y /= q2.y;
	q1.z /= q2.z;
	q1.w /= q2.w;

	return(q1);
}


// Multiply q1*q2 (apply a rotation of "q1" to "q2")!
void quatMultiplyQuatP1(quat *const restrict q1, const quat q2){
	const quat tempQuat = *q1;

	q1->x = tempQuat.w * q2.x + tempQuat.x * q2.w + tempQuat.y * q2.z - tempQuat.z * q2.y;
	q1->y = tempQuat.w * q2.y + tempQuat.y * q2.w + tempQuat.z * q2.x - tempQuat.x * q2.z;
	q1->z = tempQuat.w * q2.z + tempQuat.z * q2.w + tempQuat.x * q2.y - tempQuat.y * q2.x;
	q1->w = tempQuat.w * q2.w - tempQuat.x * q2.x - tempQuat.y * q2.y - tempQuat.z * q2.z;
}

// Multiply q1*q2 (apply a rotation of "q1" to "q2") and store the result in "q2"!
void quatMultiplyQuatP2(const quat q1, quat *const restrict q2){
	const quat tempQuat = *q2;

	q2->x = q1.w * tempQuat.x + q1.x * tempQuat.w + q1.y * tempQuat.z - q1.z * tempQuat.y;
	q2->y = q1.w * tempQuat.y + q1.y * tempQuat.w + q1.z * tempQuat.x - q1.x * tempQuat.z;
	q2->z = q1.w * tempQuat.z + q1.z * tempQuat.w + q1.x * tempQuat.y - q1.y * tempQuat.x;
	q2->w = q1.w * tempQuat.w - q1.x * tempQuat.x - q1.y * tempQuat.y - q1.z * tempQuat.z;
}

// Multiply q1*q2 (apply a rotation of "q1" to "q2") and store the result in "out"!
void quatMultiplyQuatOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	out->y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
	out->z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
	out->w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
}

// Multiply q1*q2 (apply a rotation of "q1" to "q2")!
quat quatMultiplyQuatC(const quat q1, const quat q2){
	const quat out = {
		.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
	};
	return(out);
}

/*
** Multiply q1*conj(q2) and store the result in "q1"!
** This is the rotation that must be applied to q2 to bring it to q1.
*/
void quatMultiplyQuatConjP1(quat *const restrict q1, const quat q2){
	const quat tempQuat = *q1;

	q1->x = -tempQuat.w * q2.x + tempQuat.x * q2.w - tempQuat.y * q2.z + tempQuat.z * q2.y;
	q1->y = -tempQuat.w * q2.y + tempQuat.y * q2.w - tempQuat.z * q2.x + tempQuat.x * q2.z;
	q1->z = -tempQuat.w * q2.z + tempQuat.z * q2.w - tempQuat.x * q2.y + tempQuat.y * q2.x;
	q1->w =  tempQuat.w * q2.w + tempQuat.x * q2.x + tempQuat.y * q2.y + tempQuat.z * q2.z;
}

/*
** Multiply q1*conj(q2) and store the result in "q2"!
** This is the rotation that must be applied to q2 to bring it to q1.
*/
void quatMultiplyQuatConjP2(const quat q1, quat *const restrict q2){
	const quat tempQuat = *q2;

	q2->x = -q1.w * tempQuat.x + q1.x * tempQuat.w - q1.y * tempQuat.z + q1.z * tempQuat.y;
	q2->y = -q1.w * tempQuat.y + q1.y * tempQuat.w - q1.z * tempQuat.x + q1.x * tempQuat.z;
	q2->z = -q1.w * tempQuat.z + q1.z * tempQuat.w - q1.x * tempQuat.y + q1.y * tempQuat.x;
	q2->w =  q1.w * tempQuat.w + q1.x * tempQuat.x + q1.y * tempQuat.y + q1.z * tempQuat.z;
}

/*
** Multiply q1*conj(q2) and store the result in "out"!
** This is the rotation that must be applied to q2 to bring it to q1.
*/
void quatMultiplyQuatConjOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = -q1.w * q2.x + q1.x * q2.w - q1.y * q2.z + q1.z * q2.y;
	out->y = -q1.w * q2.y + q1.y * q2.w - q1.z * q2.x + q1.x * q2.z;
	out->z = -q1.w * q2.z + q1.z * q2.w - q1.x * q2.y + q1.y * q2.x;
	out->w =  q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

/*
** Multiply q1*conj(q2) and return the result!
** This is the rotation that must be applied to "q2" to bring it to "q1".
*/
quat quatMultiplyQuatConjC(const quat q1, const quat q2){
	const quat out = {
		.x = -q1.w * q2.x + q1.x * q2.w - q1.y * q2.z + q1.z * q2.y,
		.y = -q1.w * q2.y + q1.y * q2.w - q1.z * q2.x + q1.x * q2.z,
		.z = -q1.w * q2.z + q1.z * q2.w - q1.x * q2.y + q1.y * q2.x,
		.w =  q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z
	};
	return(out);
}

/*
** Multiply conj(q1)*q2 and store the result in "q1"!
** This is the orientation that, when rotated by "q1", is taken to "q2".
*/
void quatConjMultiplyQuatP1(quat *const restrict q1, const quat q2){
	const quat tempQuat = *q1;

	q1->x = tempQuat.w * q2.x - tempQuat.x * q2.w - tempQuat.y * q2.z + tempQuat.z * q2.y;
	q1->y = tempQuat.w * q2.y - tempQuat.y * q2.w - tempQuat.z * q2.x + tempQuat.x * q2.z;
	q1->z = tempQuat.w * q2.z - tempQuat.z * q2.w - tempQuat.x * q2.y + tempQuat.y * q2.x;
	q1->w = tempQuat.w * q2.w + tempQuat.x * q2.x + tempQuat.y * q2.y + tempQuat.z * q2.z;
}

/*
** Multiply conj(q1)*q2 and store the result in "q2"!
** This is the orientation that, when rotated by "q1", is taken to "q2".
*/
void quatConjMultiplyQuatP2(const quat q1, quat *const restrict q2){
	const quat tempQuat = *q2;

	q2->x = q1.w * tempQuat.x - q1.x * tempQuat.w - q1.y * tempQuat.z + q1.z * tempQuat.y;
	q2->y = q1.w * tempQuat.y - q1.y * tempQuat.w - q1.z * tempQuat.x + q1.x * tempQuat.z;
	q2->z = q1.w * tempQuat.z - q1.z * tempQuat.w - q1.x * tempQuat.y + q1.y * tempQuat.x;
	q2->w = q1.w * tempQuat.w + q1.x * tempQuat.x + q1.y * tempQuat.y + q1.z * tempQuat.z;
}

/*
** Multiply conj(q1)*q2 and store the result in "out"!
** This is the orientation that, when rotated by "q1", is taken to "q2".
*/
void quatConjMultiplyQuatOut(const quat q1, const quat q2, quat *const restrict out){
	out->x = q1.w * q2.x - q1.x * q2.w - q1.y * q2.z + q1.z * q2.y;
	out->y = q1.w * q2.y - q1.y * q2.w - q1.z * q2.x + q1.x * q2.z;
	out->z = q1.w * q2.z - q1.z * q2.w - q1.x * q2.y + q1.y * q2.x;
	out->w = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

/*
** Multiply conj(q1)*q2 and return the result!
** This is the orientation that, when rotated by "q1", is taken to "q2".
*/
quat quatConjMultiplyQuatC(const quat q1, const quat q2){
	const quat out = {
		.x = q1.w * q2.x - q1.x * q2.w - q1.y * q2.z + q1.z * q2.y,
		.y = q1.w * q2.y - q1.y * q2.w - q1.z * q2.x + q1.x * q2.z,
		.z = q1.w * q2.z - q1.z * q2.w - q1.x * q2.y + q1.y * q2.x,
		.w = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z
	};
	return(out);
}


/*
** Apply a quaternion rotation to a vec3! Normally, this is done
** by computing v' = q*v*conj(q), where v is the quaternion with
** imaginary part v, and the rotated vector is the imaginary part
** of v'. However, we use an optimized method instead.
*/
void quatRotateVec3(const quat *const restrict q, vec3 *const restrict v){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3MagnitudeSquaredVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	v->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	v->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	v->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

/*
** Apply a quaternion rotation to a vec3! Normally, this is done
** by computing v' = q*v*conj(q), where v is the quaternion with
** imaginary part v, and the rotated vector is the imaginary part
** of v'. However, we use an optimized method instead.
*/
void quatRotateVec3Out(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3MagnitudeSquaredVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

/*
** Apply a quaternion rotation to a vec3! Normally, this is done
** by computing v' = q*v*conj(q), where v is the quaternion with
** imaginary part v, and the rotated vector is the imaginary part
** of v'. However, we use an optimized method instead.
*/
vec3 quatRotateVec3C(const quat q, vec3 v){
	float qvDot = vec3DotVec3C(v, *((const vec3 *)&q.x));
	const float qNorm = q.w * q.w - vec3MagnitudeSquaredVec3C(*((const vec3 *)&q.x));
	const vec3 qvCross = vec3MultiplySC(vec3CrossVec3C(*((const vec3 *)&q.x), v), q.w + q.w);
	qvDot += qvDot;

	v.x = qvDot * q.x + qNorm * v.x + qvCross.x;
	v.y = qvDot * q.y + qNorm * v.y + qvCross.y;
	v.z = qvDot * q.z + qNorm * v.z + qvCross.z;

	return(v);
}

/*
** Undo a quaternion rotation on a vec3! Normally, this is done
** by computing v' = conj(q)*v*q, where v is the quaternion with
** imaginary part v, and the rotated vector is the imaginary part
** of v'. However, we use an optimized method instead.
*/
void quatConjRotateVec3(const quat *const restrict q, vec3 *const restrict v){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3MagnitudeSquaredVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, -q->w - q->w);

	v->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	v->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	v->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

/*
** Undo a quaternion rotation on a vec3! Normally, this is done
** by computing v' = conj(q)*v*q, where v is the quaternion with
** imaginary part v, and the rotated vector is the imaginary part
** of v'. However, we use an optimized method instead.
*/
void quatConjRotateVec3Out(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	float qvDot = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	const float qNorm = q->w * q->w - vec3MagnitudeSquaredVec3((const vec3 *const restrict)&q->x);
	vec3 qvCross;

	qvDot += qvDot;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	vec3MultiplyS(&qvCross, -q->w - q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

/*
** Undo a quaternion rotation on a vec3! Normally, this is done
** by computing v' = conj(q)*v*q, where v is the quaternion with
** imaginary part v, and the rotated vector is the imaginary part
** of v'. However, we use an optimized method instead.
*/
vec3 quatConjRotateVec3C(const quat q, vec3 v){
	float qvDot = vec3DotVec3C(v, *((const vec3 *)&q.x));
	const float qNorm = q.w * q.w - vec3MagnitudeSquaredVec3C(*((const vec3 *)&q.x));
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
	vec3CrossVec3P2((const vec3 *const restrict)&q->x, &qvCross);

	v->x += 2.f*qvCross.x;
	v->y += 2.f*qvCross.y;
	v->z += 2.f*qvCross.z;
}

/*
** Apply a quaternion rotation to a vec3!
**
** Note: This method is significantly faster, but it is not perfectly accurate.
*/
void quatRotateVec3FastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	qvCross.x += q->w * v->x;
	qvCross.y += q->w * v->y;
	qvCross.z += q->w * v->z;
	vec3CrossVec3P2((const vec3 *const restrict)&q->x, &qvCross);

	out->x = 2.f*qvCross.x + v->x;
	out->y = 2.f*qvCross.y + v->y;
	out->z = 2.f*qvCross.z + v->z;
}

/*
** Apply a quaternion rotation to a vec3!
**
** Note: This method is significantly faster, but it is not perfectly accurate.
*/
vec3 quatRotateVec3FastC(const quat q, vec3 v){
	vec3 qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), v);
	qvCross.x += q.w * v.x;
	qvCross.y += q.w * v.y;
	qvCross.z += q.w * v.z;
	qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), qvCross);

	v.x += 2.f*qvCross.x;
	v.y += 2.f*qvCross.y;
	v.z += 2.f*qvCross.z;

	return(v);
}

/*
** Undo a quaternion rotation on a vec3!
**
** Note: This method is significantly faster, but it is not perfectly accurate.
*/
void quatConjRotateVec3Fast(const quat *const restrict q, vec3 *const restrict v){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	// It would perhaps be clearer if we reversed the order
	// of the cross products, but it actually suffices to
	// treat the quaternion's conjugate as (-w, x, y, z).
	// The final expression can be shown to be identical.
	qvCross.x -= q->w * v->x;
	qvCross.y -= q->w * v->y;
	qvCross.z -= q->w * v->z;
	vec3CrossVec3P2((const vec3 *const restrict)&q->x, &qvCross);

	v->x += 2.f*qvCross.x;
	v->y += 2.f*qvCross.y;
	v->z += 2.f*qvCross.z;
}

/*
** Undo a quaternion rotation on a vec3!
**
** Note: This method is significantly faster, but it is not perfectly accurate.
*/
void quatConjRotateVec3FastOut(const quat *const restrict q, const vec3 *const restrict v, vec3 *const restrict out){
	vec3 qvCross;
	vec3CrossVec3Out((const vec3 *const restrict)&q->x, v, &qvCross);
	// It would perhaps be clearer if we reversed the order
	// of the cross products, but it actually suffices to
	// treat the quaternion's conjugate as (-w, x, y, z).
	// The final expression can be shown to be identical.
	qvCross.x -= q->w * v->x;
	qvCross.y -= q->w * v->y;
	qvCross.z -= q->w * v->z;
	vec3CrossVec3P2((const vec3 *const restrict)&q->x, &qvCross);

	out->x = 2.f*qvCross.x + v->x;
	out->y = 2.f*qvCross.y + v->y;
	out->z = 2.f*qvCross.z + v->z;
}

/*
** Undo a quaternion rotation on a vec3!
**
** Note: This method is significantly faster, but it is not perfectly accurate.
*/
vec3 quatConjRotateVec3FastC(const quat q, vec3 v){
	vec3 qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), v);
	// It would perhaps be clearer if we reversed the order
	// of the cross products, but it actually suffices to
	// treat the quaternion's conjugate as (-w, x, y, z).
	// The final expression can be shown to be identical.
	qvCross.x -= q.w * v.x;
	qvCross.y -= q.w * v.y;
	qvCross.z -= q.w * v.z;
	qvCross = vec3CrossVec3C(*((const vec3 *)&q.x), qvCross);

	v.x += 2.f*qvCross.x;
	v.y += 2.f*qvCross.y;
	v.z += 2.f*qvCross.z;

	return(v);
}


// Rotate the x-axis by a quaternion and store the result in "out"!
void quatBasisX(const quat *const restrict q, vec3 *const restrict out){
	const float x2 = 2.f * q->x;
	const float w2 = 2.f * q->w;
	out->x = q->x * x2 + q->w * w2 - 1.f;
	out->y = q->y * x2 + q->z * w2;
	out->z = q->z * x2 - q->y * w2;
}

// Rotate the x-axis by a quaternion!
vec3 quatBasisXC(const quat q){
	const float x2 = 2.f * q.x;
	const float w2 = 2.f * q.w;
	const vec3 v = {
		.x = q.x * x2 + q.w * w2 - 1.f,
		.y = q.y * x2 + q.z * w2,
		.z = q.z * x2 - q.y * w2
	};

	return(v);
}

// Rotate the y-axis by a quaternion and store the result in "out"!
void quatBasisY(const quat *const restrict q, vec3 *const restrict out){
	const float y2 = 2.f * q->y;
	const float w2 = 2.f * q->w;
	out->x = q->x * y2 - q->z * w2;
	out->y = q->y * y2 + q->w * w2 - 1.f;
	out->z = q->z * y2 + q->x * w2;
}

// Rotate the y-axis by a quaternion!
vec3 quatBasisYC(const quat q){
	const float y2 = 2.f * q.y;
	const float w2 = 2.f * q.w;
	const vec3 v = {
		.x = q.x * y2 - q.z * w2,
		.y = q.y * y2 + q.w * w2 - 1.f,
		.z = q.z * y2 + q.x * w2
	};

	return(v);
}

// Rotate the z-axis by a quaternion and store the result in "out"!
void quatBasisZ(const quat *const restrict q, vec3 *const restrict out){
	const float z2 = 2.f * q->z;
	const float w2 = 2.f * q->w;
	//2xz + 2yw, 2yz - 2xw, 2zz + 2ww - 1
	out->x = q->x * z2 + q->y * w2;
	out->y = q->y * z2 - q->x * w2;
	out->z = q->z * z2 + q->w * w2 - 1.f;
}

// Rotate the z-axis by a quaternion!
vec3 quatBasisZC(const quat q){
	const float z2 = 2.f * q.z;
	const float w2 = 2.f * q.w;
	const vec3 v = {
		.x = q.x * z2 + q.y * w2,
		.y = q.y * z2 - q.x * w2,
		.z = q.z * z2 + q.w * w2 - 1.f
	};

	return(v);
}


// Find a quaternion that rotates "v1" to "v2" and store the result in "out"!
void quatShortestArc(const vec3 *const restrict v1, const vec3 *const restrict v2, quat *const restrict out){
	vec3CrossVec3Out(v1, v2, (vec3 *)&out->x);
	out->w = 1.f + vec3DotVec3(v1, v2);
	quatNormalizeQuatFast(out);
}

// Find a quaternion that rotates "v1" to "v2"!
quat quatShortestArcC(const vec3 v1, const vec3 v2){
	const vec3 axis = vec3CrossVec3C(v1, v2);
	return(quatNormalizeQuatFastC(quatInitSetC(1.f + vec3DotVec3C(v1, v2), axis.x, axis.y, axis.z)));
}

/*
** Find a quaternion that rotates "v1" to "v2"!
** This implementation is based on Stan Melax's article in Game Programming Gems 2.
**
** The main difference is that this implementation
** handles the singularity when the two vectors are
** antiparallel.
*/
void quatShortestArcAlt(const vec3 *const restrict v1, const vec3 *const restrict v2, quat *const restrict out){
	const float d = vec3DotVec3(v1, v2);
	// Handle the case where the vectors are
	// antiparallel to avoid dividing by zero.
	if(d < -1.f + QUAT_SINGULARITY_THRESHOLD){
		vec3OrthonormalFast(v1, (vec3 *const restrict)&out->x);
		out->w = 0.f;
	}else{
		const float s = sqrtf(2.f * (1.f + d));
		const float rs = 1.f/s;
		vec3 axis;
		vec3CrossVec3Out(v1, v2, &axis);
		quatInitSet(out, s*0.5f, rs*axis.x, rs*axis.y, rs*axis.z);
	}
}

/*
** Find a quaternion that rotates "v1" to "v2"!
** This implementation is based on Stan Melax's article in Game Programming Gems 2.
**
** The main difference is that this implementation
** handles the singularity when the two vectors are
** antiparallel.
*/
quat quatShortestArcAltC(const vec3 v1, const vec3 v2){
	const float d = vec3DotVec3C(v1, v2);
	// Handle the case where the vectors are
	// antiparallel to avoid dividing by zero.
	if(d < -1.f + QUAT_SINGULARITY_THRESHOLD){
		quat q;
		*((vec3 *const restrict)&q.x) = vec3OrthonormalFastC(v1);
		q.w = 0.f;
		return(q);
	}else{
		const float s = sqrtf(2.f * (1.f + d));
		const float rs = 1.f/s;
		const vec3 axis = vec3CrossVec3C(v1, v2);
		return(quatInitSetC(s*0.5f, rs*axis.x, rs*axis.y, rs*axis.z));
	}
}


// Find the magnitude (length) of a quaternion stored as four floats!
float quatMagnitude(const float w, const float x, const float y, const float z){
	return(sqrtf(quatMagnitudeSquared(w, x, y, z)));
}

// Find the magnitude (length) of a quaternion!
float quatMagnitudeQuat(const quat *const restrict q){
	return(sqrtf(quatMagnitudeSquaredQuat(q)));
}

// Find the magnitude (length) of a quaternion!
float quatMagnitudeQuatC(const quat q){
	return(sqrtf(quatMagnitudeSquaredQuatC(q)));
}

// Find the squared magnitude of a quaternion stored as four floats!
float quatMagnitudeSquared(const float w, const float x, const float y, const float z){
	return(w * w + x * x + y * y + z * z);
}

// Find the squared magnitude of a quaternion!
float quatMagnitudeSquaredQuat(const quat *const restrict q){
	return(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}

// Find the squared magnitude of a quaternion!
float quatMagnitudeSquaredQuatC(const quat q){
	return(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}


// Find the dot product of two vec4s stored as four floats!
float quatDot(const float w1, const float x1, const float y1, const float z1, const float w2, const float x2, const float y2, const float z2){
	return(w1 * w2 + x1 * x2 + y1 * y2 + z1 * z2);
}

// Find the dot product of a quat and a vec4 stored as four floats!
float quatDotQuatFloat(const quat *const restrict q, const float w, const float x, const float y, const float z){
	return(q->x * x + q->y * y + q->z * z + q->w * w);
}

// Find the dot product of a quat and a vec4 stored as four floats!
float quatDotQuatFloatC(const quat q, const float w, const float x, const float y, const float z){
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


// Normalize a quaternion stored as four floats and store the result in "out"!
void quatNormalize(const float w, const float x, const float y, const float z, quat *const restrict out){
	const float magnitude = invSqrt(w * w + x * x + y * y + z * z);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

// Normalize a quaternion stored as four floats!
quat quatNormalizeC(const float w, const float x, const float y, const float z){
	const float magnitude = invSqrt(w * w + x * x + y * y + z * z);
	quat q;

	q.x = x * magnitude;
	q.y = y * magnitude;
	q.z = z * magnitude;
	q.w = w * magnitude;

	return(q);
}

// Normalize a quaternion stored as four floats and store the result in "out"!
void quatNormalizeFast(const float w, const float x, const float y, const float z, quat *const restrict out){
	const float magnitude = invSqrtFast(w * w + x * x + y * y + z * z);

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

// Normalize a quaternion stored as four floats!
quat quatNormalizeFastC(const float w, const float x, const float y, const float z){
	const float magnitude = invSqrtFast(w * w + x * x + y * y + z * z);
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

// Normalize a quaternion and store the result in "out"!
void quatNormalizeQuatOut(const quat *const restrict q, quat *const restrict out){
	const float magnitude = invSqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);

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
void quatNormalizeQuatFast(quat *const restrict q){
	const float magnitude = invSqrtFast(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);

	q->x *= magnitude;
	q->y *= magnitude;
	q->z *= magnitude;
	q->w *= magnitude;
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
quat quatNormalizeQuatFastC(quat q){
	const float magnitude = invSqrtFast(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

	q.x *= magnitude;
	q.y *= magnitude;
	q.z *= magnitude;
	q.w *= magnitude;

	return(q);
}

/*
** Try to normalize a quat stored as four floats
** and return whether or not we were successful!
*/
return_t quatCanNormalize(const float w, const float x, const float y, const float z, quat *const restrict out){
	float magnitude = w * w + x * x + y * y + z * z + w * w;
	if(magnitude > MATH_NORMALIZE_EPSILON){
		magnitude = invSqrt(magnitude);
		out->x = x * magnitude;
		out->y = y * magnitude;
		out->z = z * magnitude;
		out->w = w * magnitude;

		return(1);
	}

	return(0);
}

// Try to normalize a quat and return whether or not we were successful!
return_t quatCanNormalizeQuat(quat *const restrict v){
	float magnitude = v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w;
	if(magnitude > MATH_NORMALIZE_EPSILON){
		magnitude = invSqrt(magnitude);
		v->x *= magnitude;
		v->y *= magnitude;
		v->z *= magnitude;
		v->w *= magnitude;

		return(1);
	}

	return(0);
}

// Try to normalize a quat and return whether or not we were successful!
return_t quatCanNormalizeQuatOut(const quat *const restrict v, quat *const restrict out){
	float magnitude = v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w;
	if(magnitude > MATH_NORMALIZE_EPSILON){
		magnitude = invSqrt(magnitude);
		out->x = v->x * magnitude;
		out->y = v->y * magnitude;
		out->z = v->z * magnitude;
		out->w = v->w * magnitude;

		return(1);
	}

	return(0);
}

/*
** Try to normalize a quat stored as four floats
** and return whether or not we were successful!
*/
return_t quatCanNormalizeFast(const float w, const float x, const float y, const float z, quat *const restrict out){
	float magnitude = w * w + x * x + y * y + z * z;
	if(magnitude > MATH_NORMALIZE_EPSILON){
		magnitude = invSqrtFast(magnitude);
		out->x = x * magnitude;
		out->y = y * magnitude;
		out->z = z * magnitude;
		out->w = w * magnitude;

		return(1);
	}

	return(0);
}

// Try to normalize a quat and return whether or not we were successful!
return_t quatCanNormalizeQuatFast(quat *const restrict v){
	float magnitude = v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w;
	if(magnitude > MATH_NORMALIZE_EPSILON){
		magnitude = invSqrtFast(magnitude);
		v->x *= magnitude;
		v->y *= magnitude;
		v->z *= magnitude;
		v->w *= magnitude;

		return(1);
	}

	return(0);
}

// Try to normalize a quat and return whether or not we were successful!
return_t quatCanNormalizeQuatFastOut(const quat *const restrict v, quat *const restrict out){
	float magnitude = v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w;
	if(magnitude > MATH_NORMALIZE_EPSILON){
		magnitude = invSqrtFast(magnitude);
		out->x = v->x * magnitude;
		out->y = v->y * magnitude;
		out->z = v->z * magnitude;
		out->w = v->w * magnitude;

		return(1);
	}

	return(0);
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


// Find a quaternion's normalized axis and store the result in "out"!
void quatAxis(const quat *const restrict q, vec3 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale < MATH_NORMALIZE_EPSILON){
		out->x = 1.f;
		out->y = 0.f;
		out->z = 0.f;
	}else{
		scale = invSqrt(scale);
		out->x = q->x*scale;
		out->y = q->y*scale;
		out->z = q->z*scale;
	}
}

// Find a quaternion's normalized axis!
vec3 quatAxisC(const quat q){
	vec3 v;

	float scale = 1.f - q.w*q.w;
	if(scale < MATH_NORMALIZE_EPSILON){
		v.x = 1.f;
		v.y = 0.f;
		v.z = 0.f;
	}else{
		scale = invSqrt(scale);
		v.x = q.x*scale;
		v.y = q.y*scale;
		v.z = q.z*scale;
	}

	return(v);
}

// Find a quaternion's normalized axis and store the result in "out"!
void quatAxisAlt(const quat *const restrict q, vec3 *const restrict out){
	const float scale = invSqrt(1.f - q->w*q->w);
	out->x = q->x*scale;
	out->y = q->y*scale;
	out->z = q->z*scale;
}

// Find a quaternion's normalized axis!
vec3 quatAxisAltC(const quat q){
	const float scale = invSqrtFast(1.f - q.w*q.w);
	const vec3 v = vec3InitSetC(q.x*scale, q.y*scale, q.z*scale);

	return(v);
}

// Find a quaternion's normalized axis and store the result in "out"!
void quatAxisFast(const quat *const restrict q, vec3 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale < MATH_NORMALIZE_EPSILON){
		out->x = 1.f;
		out->y = 0.f;
		out->z = 0.f;
	}else{
		scale = invSqrtFast(scale);
		out->x = q->x*scale;
		out->y = q->y*scale;
		out->z = q->z*scale;
	}
}

// Find a quaternion's normalized axis!
vec3 quatAxisFastC(const quat q){
	vec3 v;

	float scale = 1.f - q.w*q.w;
	if(scale < MATH_NORMALIZE_EPSILON){
		v.x = 1.f;
		v.y = 0.f;
		v.z = 0.f;
	}else{
		scale = invSqrtFast(scale);
		v.x = q.x*scale;
		v.y = q.y*scale;
		v.z = q.z*scale;
	}

	return(v);
}

// Find a quaternion's normalized axis and store the result in "out"!
void quatAxisAltFast(const quat *const restrict q, vec3 *const restrict out){
	const float scale = invSqrtFast(1.f - q->w*q->w);
	out->x = q->x*scale;
	out->y = q->y*scale;
	out->z = q->z*scale;
}

// Find a quaternion's normalized axis!
vec3 quatAxisAltFastC(const quat q){
	const float scale = invSqrtFast(1.f - q.w*q.w);
	const vec3 v = vec3InitSetC(q.x*scale, q.y*scale, q.z*scale);

	return(v);
}

/*
** Try and get a quaternion's normalized axis, and return whether we
** are successful! This should only fail if "q" is close to identity.
*/
return_t quatCanGetAxis(const quat *const restrict q, vec3 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale > MATH_NORMALIZE_EPSILON){
		scale = invSqrt(scale);
		out->x = q->x*scale;
		out->y = q->y*scale;
		out->z = q->z*scale;

		return(1);
	}

	return(0);
}

/*
** Try and get a quaternion's normalized axis, and return whether we
** are successful! This should only fail if "q" is close to identity.
*/
return_t quatCanGetAxisFast(const quat *const restrict q, vec3 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale > MATH_NORMALIZE_EPSILON){
		scale = invSqrtFast(scale);
		out->x = q->x*scale;
		out->y = q->y*scale;
		out->z = q->z*scale;

		return(1);
	}

	return(0);
}

// Return a quaternion's angle!
float quatAngle(const quat *const restrict q){
	return(2.f * acosf(q->w));
}

// Return a quaternion's angle!
float quatAngleC(const quat q){
	return(2.f * acosf(q.w));
}


/*
** Return a quaternion's rotation about an arbitrary axis!
** Note that "qa" is the dot product of the quaternion's
** axis with the unit axis of interest and "w" is the
** quaternion's real part.
*/
float quatAngleAboutAxis(const float qa, const float w){
	// tan(t/4) = sin(t/2)/(1 + cos(t/2))
	return(4.f * atan2f(qa, 1.f + w));
}

/*
** Assuming "v" is a vector given by
**     v = (tanf(tx * 0.25f), tanf(ty * 0.25f), tanf(tz * 0.25f)),
** where
**     tk = quatAngleAboutAxis(q.k, q.w),
** this function reconstructs "q" and stores it in "out".
**
** This function was inspired by the one used by NVIDIA in PhysX.
*/
void quatFromTanVector(const vec3 *const restrict v, quat *const restrict out){
	const float v2 = vec3MagnitudeSquaredVec3(v);
	if(v2 < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		quatInitIdentity(out);
	}else{
		const float d = 2.f / (1.f + v2);
		quatInitSet(out, d - 1.f, v->x * d, v->y * d, v->z * d);
	}
}

quat quatFromTanVectorC(const vec3 v){
	const float v2 = vec3MagnitudeSquaredVec3C(v);
	if(v2 < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		return(quatInitIdentityC());
	}else{
		const float d = 2.f / (1.f + v2);
		return(quatInitSetC(d - 1.f, v.x * d, v.y * d, v.z * d));
	}
}


/*
** Find the XYZ Euler angles describing a quaternion and store the result
** in "out"! The output vector is given in the form (pitch, yaw, roll).
**
** Implementation inspired by the one in Gino van den Bergen's Motion Toolkit.
*/
void quatToEulerAnglesXYZ(const quat q, vec3 *const restrict out){
	const float yy = q.y*q.y;
	vec3InitSet(out,
		atan2f(2.f*(q.y*q.z + q.x*q.w), 1.f - 2.f*(yy + q.x*q.x)),
		asinf(2.f*(q.y*q.w - q.x*q.z)),
		atan2f(2.f*(q.x*q.y + q.z*q.w), 1.f - 2.f*(yy + q.z*q.z))
	);
}

/*
** Find the ZXY Euler angles describing a quaternion and store the result
** in "out"! The output vector is given in the form (pitch, yaw, roll).
**
** Implementation inspired by the one in Gino van den Bergen's Motion Toolkit.
*/
void quatToEulerAnglesZXY(const quat q, vec3 *const restrict out){
	const float xx = q.x*q.x;
	vec3InitSet(out,
		asinf(2.f*(q.x*q.w - q.y*q.z)),
		atan2f(2.f*(q.x*q.z + q.y*q.w), 1.f - 2.f*(xx + q.y*q.y)),
		atan2f(2.f*(q.x*q.y + q.z*q.w), 1.f - 2.f*(xx + q.z*q.z))
	);
}

/*
** Find the XYZ Euler angles describing a quaternion and return them
** as a vec3. The output vector is given in the form (pitch, yaw, roll).
**
** Implementation inspired by the one in Gino van den Bergen's Motion Toolkit.
*/
vec3 quatToEulerAnglesXYZC(const quat q){
	const float yy = q.y*q.y;
	return(vec3InitSetC(
		atan2f(2.f*(q.y*q.z + q.x*q.w), 1.f - 2.f*(yy + q.x*q.x)),
		asinf(2.f*(q.y*q.w - q.x*q.z)),
		atan2f(2.f*(q.x*q.y + q.z*q.w), 1.f - 2.f*(yy + q.z*q.z))
	));
}

/*
** Find the ZXY Euler angles describing a quaternion and return them
** as a vec3. The output vector is given in the form (pitch, yaw, roll).
**
** Implementation inspired by the one in Gino van den Bergen's Motion Toolkit.
*/
vec3 quatToEulerAnglesZXYC(const quat q){
	const float xx = q.x*q.x;
	return(vec3InitSetC(
		asinf(2.f*(q.x*q.w - q.y*q.z)),
		atan2f(2.f*(q.x*q.z + q.y*q.w), 1.f - 2.f*(xx + q.y*q.y)),
		atan2f(2.f*(q.x*q.y + q.z*q.w), 1.f - 2.f*(xx + q.z*q.z))
	));
}

// Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngle(const quat *const restrict q, vec4 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale < MATH_NORMALIZE_EPSILON){
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

// Convert a quaternion to an axis and an angle!
vec4 quatToAxisAngleC(const quat q){
	vec4 v;

	float scale = 1.f - q.w*q.w;
	if(scale < MATH_NORMALIZE_EPSILON){
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

// Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngleFast(const quat *const restrict q, vec4 *const restrict out){
	float scale = 1.f - q->w*q->w;
	if(scale < MATH_NORMALIZE_EPSILON){
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
vec4 quatToAxisAngleFastC(const quat q){
	vec4 v;

	float scale = 1.f - q.w*q.w;
	if(scale < MATH_NORMALIZE_EPSILON){
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
void quatRotateByEulerXYZ(quat *const restrict q, const float x, const float y, const float z){
	quat rot;
	quatInitEulerXYZ(&rot, x, y, z);
	quatMultiplyQuatP2(rot, q);
}

// Rotate a quaternion (in radians)!
void quatRotateByEulerZXY(quat *const restrict q, const float x, const float y, const float z){
	quat rot;
	quatInitEulerZXY(&rot, x, y, z);
	quatMultiplyQuatP2(rot, q);
}

// Rotate a quaternion (in radians)!
quat quatRotateByEulerXYZC(const quat q, const float x, const float y, const float z){
	return(quatMultiplyQuatC(quatInitEulerXYZC(x, y, z), q));
}

// Rotate a quaternion (in radians)!
quat quatRotateByEulerZXYC(const quat q, const float x, const float y, const float z){
	return(quatMultiplyQuatC(quatInitEulerZXYC(x, y, z), q));
}

// Rotate a quaternion by a vec3 (in radians)!
void quatRotateByVec3EulerXYZ(quat *const restrict q, const vec3 *const restrict v){
	quatRotateByEulerXYZ(q, v->x, v->y, v->z);
}

// Rotate a quaternion by a vec3 (in radians)!
void quatRotateByVec3EulerZXY(quat *const restrict q, const vec3 *const restrict v){
	quatRotateByEulerZXY(q, v->x, v->y, v->z);
}

// Rotate a quaternion by a vec3 (in radians)!
quat quatRotateByVec3EulerXYZC(const quat q, const vec3 v){
	return(quatRotateByEulerXYZC(q, v.x, v.y, v.z));
}

// Rotate a quaternion by a vec3 (in radians)!
quat quatRotateByVec3EulerZXYC(const quat q, const vec3 v){
	return(quatRotateByEulerZXYC(q, v.x, v.y, v.z));
}


// Perform linear interpolation between two quaternions and store the result in "q1"!
void quatLerp(quat *const restrict q1, const quat *const restrict q2, const float time){
	q1->x = floatLerpFast(q1->x, q2->x, time);
	q1->y = floatLerpFast(q1->y, q2->y, time);
	q1->z = floatLerpFast(q1->z, q2->z, time);
	q1->w = floatLerpFast(q1->w, q2->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(q1);
}

// Perform linear interpolation between two quaternions and store the result in "out"!
void quatLerpOut(const quat *const restrict q1, const quat *const restrict q2, const float time, quat *const restrict out){
	out->x = floatLerpFast(q1->x, q2->x, time);
	out->y = floatLerpFast(q1->y, q2->y, time);
	out->z = floatLerpFast(q1->z, q2->z, time);
	out->w = floatLerpFast(q1->w, q2->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(out);
}

// Perform linear interpolation between two quaternions!
quat quatLerpC(quat q1, const quat q2, const float time){
	q1.x = floatLerpFast(q1.x, q2.x, time);
	q1.y = floatLerpFast(q1.y, q2.y, time);
	q1.z = floatLerpFast(q1.z, q2.z, time);
	q1.w = floatLerpFast(q1.w, q2.w, time);

	// It's nice to be safe... but it isn't very fast.
	return(quatNormalizeQuatC(q1));
}

/*
** Perform linear interpolation between two quaternions and store the result in "q"!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
void quatLerpDiff(quat *const restrict q, const quat *const restrict offset, const float time){
	q->x = floatLerpDiffFast(q->x, offset->x, time);
	q->y = floatLerpDiffFast(q->y, offset->y, time);
	q->z = floatLerpDiffFast(q->z, offset->z, time);
	q->w = floatLerpDiffFast(q->w, offset->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(q);
}

/*
** Perform linear interpolation between two quaternions and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
void quatLerpDiffOut(const quat *const restrict q, const quat *const restrict offset, const float time, quat *const restrict out){
	out->x = floatLerpDiffFast(q->x, offset->x, time);
	out->y = floatLerpDiffFast(q->y, offset->y, time);
	out->z = floatLerpDiffFast(q->z, offset->z, time);
	out->w = floatLerpDiffFast(q->w, offset->w, time);

	// It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(out);
}

/*
** Perform linear interpolation between two quaternions!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
quat quatLerpDiffC(quat q, const quat offset, const float time){
	q.x = floatLerpDiffFast(q.x, offset.x, time);
	q.y = floatLerpDiffFast(q.y, offset.y, time);
	q.z = floatLerpDiffFast(q.z, offset.z, time);
	q.w = floatLerpDiffFast(q.w, offset.w, time);

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
** We also check for and handle the singularity.
**
** Based off Przemyslaw Dobrowolski's implementation given
** in Swing-Twist Decomposition in Clifford Algebra (2015).
*/
void quatSwingTwist(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	float l = q->w*q->w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		quatInitIdentity(t);
	}else{
		l = invSqrtFast(l);
		u *= l;
		quatInitSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	}

	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatMultiplyQuatConjOut(*q, *t, s);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	if(u < 0.f){
		quatNegate(t);
	}
}

void quatSwingTwistC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3C(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	float l = q.w*q.w + u*u;

	// Handle the singularity at twist rotations close to pi.
	if(l < QUAT_SINGULARITY_THRESHOLD_SQUARED){
		*t = quatInitIdentityC();
	}else{
		l = invSqrtFast(l);
		u *= l;
		*t = quatInitSetC(q.w*l, v.x*u, v.y*u, v.z*u);
	}

	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatMultiplyQuatConjC(q, *t);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	if(u < 0.f){
		quatNegate(t);
	}
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
void quatSwingTwistFast(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = invSqrtFast(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatInitSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatMultiplyQuatConjOut(*q, *t, s);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	if(u < 0.f){
		quatNegate(t);
	}
}

void quatSwingTwistFastC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3C(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = invSqrtFast(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	*t = quatInitSetC(q.w*l, v.x*u, v.y*u, v.z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatMultiplyQuatConjC(q, *t);

	// Note that if the dot product is negative, we need to invert
	// the twist quaternion to keep the direction consistent.
	if(u < 0.f){
		quatNegate(t);
	}
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
void quatSwingTwistFaster(const quat *const restrict q, const vec3 *const restrict v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3(v, (const vec3 *const restrict)&q->x);
	// Quickly compute the inverse magnitude of the projection.
	const float l = invSqrtFast(q->w*q->w + u*u);
	u *= l;

	// Set the twist quaternion.
	quatInitSet(t, q->w*l, v->x*u, v->y*u, v->z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	quatMultiplyQuatConjOut(*q, *t, s);
}

void quatSwingTwistFasterC(const quat q, const vec3 v, quat *const restrict t, quat *const restrict s){
	// Project the q's rotation axis onto the twist axis "v".
	float u = vec3DotVec3C(v, *((const vec3 *)&q.x));
	// Quickly compute the inverse magnitude of the projection.
	const float l = invSqrtFast(q.w*q.w + u*u);
	u *= l;

	// Set the twist quaternion.
	*t = quatInitSetC(q.w*l, v.x*u, v.y*u, v.z*u);
	// By construction, q = s*t. We have "t", so to find
	// "s" we can just multiply "q" by the conjugate of "t".
	*s = quatMultiplyQuatConjC(q, *t);
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

	quatMultiplyQuatP2(spin, q);
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

	quatMultiplyQuatOut(spin, *q, out);
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

	return(quatMultiplyQuatC(spin, q));
}

/*
** Integrate the quaternion "q" with angular
** velocity "w" using the step size "dt".
**
** q_{n + 1} = q_n + dq/dt * dt
*/
void quatIntegrate(quat *const restrict q, const vec3 *const restrict w, float dt){
	quat spin;

	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	quatInitSet(&spin, 0.f, w->x * dt, w->y * dt, w->z * dt);

	quatMultiplyQuatP1(&spin, *q);
	quatAddQuat(q, &spin);
}

/*
** Integrate the quaternion "q" with angular velocity "w"
** using the step size "dt" and store the result in "out".
**
** q_{n + 1} = q_n + dq/dt * dt
*/
void quatIntegrateOut(const quat *const restrict q, const vec3 *const restrict w, float dt, quat *const restrict out){
	quat spin;

	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	quatInitSet(&spin, 0.f, w->x * dt, w->y * dt, w->z * dt);

	quatMultiplyQuatP1(&spin, *q);
	quatAddQuatOut(q, &spin, out);
}

/*
** Integrate the quaternion "q" with angular
** velocity "w" using the step size "dt".
**
** q_{n + 1} = q_n + dq/dt * dt
*/
quat quatIntegrateC(const quat q, const vec3 w, float dt){
	dt *= 0.5f;
	// Multiply by half the timestep to
	// save multiplications later on.
	return(quatAddQuatC(q, quatMultiplyQuatC(quatInitSetC(0.f, w.x * dt, w.y * dt, w.z * dt), q)));
}