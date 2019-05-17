#include "quat.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


//We'll do linear interpolation if the angle between the two quaternions is less than 1 degree.
#define QUAT_LERP_EPSILON cos(DEG_TO_RAD)


static quat identityQuat = {
	.x = 0.f,
	.y = 0.f,
	.z = 0.f,
	.w = 1.f
};


//Initialize the quaternion's values to 0!
void quatInitZero(quat *q){
	memset(q, 0.f, sizeof(*q));
}

//Initialize the quaternion to an identity quaternion!
void quatInitIdentity(quat *q){
	*q = identityQuat;
}

//Initialize the quaternion's values to the ones specified!
void quatInitSet(quat *q, const float x, const float y, const float z, const float w){
	q->x = x;
	q->y = y;
	q->z = z;
	q->w = w;
}

//Initialize the quaternion's values to the Euler angles specified (in radians)!
//The order of rotations is ZYX.
void quatInitEulerRad(quat *q, const float x, const float y, const float z){
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

//Initialize the quaternion's values to the Euler angles specified (in degrees)!
//The order of rotations is ZYX.
void quatInitEulerDeg(quat *q, const float x, const float y, const float z){
	quatInitEulerRad(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

//Initialize the quaternion's values to the Euler angles specified in "v" (in radians)!
//The order of rotations is ZYX.
void quatInitEulerVec3Rad(quat *q, const vec3 *v){
	quatInitEulerRad(q, v->x, v->y, v->z);
}

//Initialize the quaternion's values to the Euler angles specified in "v" (in degrees)!
//The order of rotations is ZYX.
void quatInitEulerVec3Deg(quat *q, const vec3 *v){
	quatInitEulerRad(q, v->x * DEG_TO_RAD, v->y * DEG_TO_RAD, v->z * DEG_TO_RAD);
}


//Add a quat stored as four floats to "q"!
void quatAdd(quat *q, const float x, const float y, const float z, const float w){
	q->x += x;
	q->y += y;
	q->z += z;
	q->w += w;
}

//Add a quat stored as four floats to "q" and store the result in "out"!
void quatAddOut(const quat *q, const float x, const float y, const float z, const float w, quat *out){
	out->x = q->x + x;
	out->y = q->y + y;
	out->z = q->z + z;
	out->w = q->w + w;
}

//Add "x" to "q"!
void quatAddS(quat *q, const float x){
	q->x += x;
	q->y += x;
	q->z += x;
	q->w += x;
}

//Add "x" to "q" and store the result in "out"!
void quatAddSOut(const quat *q, const float x, quat *out){
	out->x = q->x + x;
	out->y = q->y + x;
	out->z = q->z + x;
	out->w = q->w + x;
}

//Add "v" to "q"!
void quatAddVec4(quat *q, const vec4 *v){
	q->x += v->x;
	q->y += v->y;
	q->z += v->z;
	q->w += v->w;
}

//Add "v" to "q" and store the result in "out"!
void quatAddVec4Out(const quat *q, const vec4 *v, quat *out){
	out->x = q->x + v->x;
	out->y = q->y + v->y;
	out->z = q->z + v->z;
	out->w = q->w + v->w;
}

//Subtract a quat stored as four floats from "q"!
void quatSubtract(quat *q, const float x, const float y, const float z, const float w){
	q->x -= x;
	q->y -= y;
	q->z -= z;
	q->w -= w;
}

//Subtract a quat stored as four floats from "q" and store the result in "out"!
void quatSubtractOut(const quat *q, const float x, const float y, const float z, const float w, quat *out){
	out->x = q->x - x;
	out->y = q->y - y;
	out->z = q->z - z;
	out->w = q->w - w;
}

//Subtract "q" from a quat stored as four floats!
void quatSubtractFrom(quat *q, const float x, const float y, const float z, const float w){
	q->x = x - q->x;
	q->y = y - q->y;
	q->z = z - q->z;
	q->w = w - q->w;
}

//Subtract "q" from a quat stored as four floats and store the result in "out"!
void quatSubtractFromOut(const quat *q, const float x, const float y, const float z, const float w, quat *out){
	out->x = x - q->x;
	out->y = y - q->y;
	out->z = z - q->z;
	out->w = w - q->w;
}

//Subtract "x" from "q"!
void quatSubtractSFrom(quat *q, const float x){
	q->x -= x;
	q->y -= x;
	q->z -= x;
	q->w -= x;
}

//Subtract "x" from "q" and store the result in "out"!
void quatSubtractSFromOut(const quat *q, const float x, quat *out){
	out->x = q->x - x;
	out->y = q->y - x;
	out->z = q->z - x;
	out->w = q->w - x;
}

//Subtract "q" from "x"!
void quatSubtractFromS(quat *q, const float x){
	q->x = x - q->x;
	q->y = x - q->y;
	q->z = x - q->z;
	q->w = x - q->w;
}

//Subtract "q" from "x" and store the result in "out"!
void quatSubtractFromSOut(const quat *q, const float x, quat *out){
	out->x = x - q->x;
	out->y = x - q->y;
	out->z = x - q->z;
	out->w = x - q->w;
}

//Subtract "v" from "q"!
void quatSubtractVec4From(quat *q, const vec4 *v){
	q->x -= v->x;
	q->y -= v->y;
	q->z -= v->z;
	q->w -= v->w;
}

//Subtract "q" from "v"!
void quatSubtractFromVec4(quat *q, const vec4 *v){
	q->x = v->x - q->x;
	q->y = v->y - q->y;
	q->z = v->z - q->z;
	q->w = v->w - q->w;
}

//Subtract "v" from "q" and store the result in "out"!
void quatSubtractVec4FromOut(const quat *q, const vec4 *v, quat *out){
	out->x = q->x - v->x;
	out->y = q->y - v->y;
	out->z = q->z - v->z;
	out->w = q->w - v->w;
}


//Multiply "q" by "x"!
void quatMultiplyS(quat *q, const float x){
	q->x *= x;
	q->y *= x;
	q->z *= x;
	q->w *= x;
}

//Multiply "q" by "x" and store the result in "out"!
void quatMultiplySOut(const quat *q, const float x, quat *out){
	out->x = q->x * x;
	out->y = q->y * x;
	out->z = q->z * x;
	out->w = q->w * x;
}

//Multiply "q" by "v"!
void quatMultiplyVec4(quat *q, const vec4 *v){
	q->x *= v->x;
	q->y *= v->y;
	q->z *= v->z;
	q->w *= v->w;
}

//Multiply "q" by "v" and store the result in "out"!
void quatMultiplyVec4Out(const quat *q, const vec4 *v, quat *out){
	out->x = q->x * v->x;
	out->y = q->y * v->y;
	out->z = q->z * v->z;
	out->w = q->w * v->w;
}

//Divide "q" by "x"!
void quatDivideByS(quat *q, const float x){
	const float invX = 1.f / x;

	q->x *= invX;
	q->y *= invX;
	q->z *= invX;
	q->w *= invX;
}

//Divide "q" by "x" and store the result in "out"!
void quatDivideBySOut(const quat *q, const float x, quat *out){
	const float invX = 1.f / x;

	out->x = q->x * invX;
	out->y = q->y * invX;
	out->z = q->z * invX;
	out->w = q->w * invX;
}

//Divide "x" by "q"!
void quatDivideSBy(quat *q, const float x){
	q->x = (q->x != 0.f) ? x / q->x : 0.f;
	q->y = (q->y != 0.f) ? x / q->y : 0.f;
	q->z = (q->z != 0.f) ? x / q->z : 0.f;
	q->w = (q->w != 0.f) ? x / q->w : 0.f;
}

//Divide "x" by "q" and store the result in "out"!
void quatDivideSByOut(const quat *q, const float x, quat *out){
	out->x = (q->x != 0.f) ? x / q->x : 0.f;
	out->y = (q->y != 0.f) ? x / q->y : 0.f;
	out->z = (q->z != 0.f) ? x / q->z : 0.f;
	out->w = (q->w != 0.f) ? x / q->w : 0.f;
}

/*
** Divide "x" by "q"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void quatDivideSByFast(quat *q, const float x){
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
void quatDivideSByFastOut(const quat *q, const float x, quat *out){
	out->x = x / q->x;
	out->y = x / q->y;
	out->z = x / q->z;
	out->w = x / q->w;
}

//Divide "q" by "v"!
void quatDivideByVec4(quat *q, const vec4 *v){
	q->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	q->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	q->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	q->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

//Divide "v" by "q"!
void quatDivideVec4By(quat *q, const vec4 *v){
	q->x = (q->x != 0.f) ? v->x / q->x : 0.f;
	q->y = (q->y != 0.f) ? v->y / q->y : 0.f;
	q->z = (q->z != 0.f) ? v->z / q->z : 0.f;
	q->w = (q->w != 0.f) ? v->w / q->w : 0.f;
}

//Divide "q" by "v" and store the result in "out"!
void quatDivideByVec4Out(const quat *q, const vec4 *v, quat *out){
	out->x = (v->x != 0.f) ? q->x / v->x : 0.f;
	out->y = (v->y != 0.f) ? q->y / v->y : 0.f;
	out->z = (v->z != 0.f) ? q->z / v->z : 0.f;
	out->w = (v->w != 0.f) ? q->w / v->w : 0.f;
}

/*
** Divide "q" by "v"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void quatDivideByVec4Fast(quat *q, const vec4 *v){
	q->x /= v->x;
	q->y /= v->y;
	q->z /= v->z;
	q->w /= v->w;
}

/*
** Divide "v" by "q"! Unlike the regular version, this
** does not check to prevent against divide-by-zero errors.
*/
void quatDivideVec4ByFast(quat *q, const vec4 *v){
	q->x = v->x / q->x;
	q->y = v->y / q->y;
	q->z = v->z / q->z;
	q->w = v->w / q->w;
}

/*
** Divide "q" by "v" and store the result in "out"!
** Unlike the regular version, this does not check
** to prevent against divide-by-zero errors.
*/
void quatDivideByVec4FastOut(const quat *q, const vec4 *v, quat *out){
	out->x = q->x / v->x;
	out->y = q->y / v->y;
	out->z = q->z / v->z;
	out->w = q->w / v->w;
}


//Multiply "q1" by "q2" (apply a rotation of "q1" to "q2")!
void quatMultiplyByQuat(quat *q1, const quat *q2){
	quat result;

	result.x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
	result.y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
	result.z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
	result.w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;

	*q1 = result;
}

//Multiply "q2" by "q1" (apply a rotation of "q2" to "q1")!
void quatMultiplyQuatBy(quat *q1, const quat *q2){
	quat result;

	result.x = q2->w * q1->x + q2->x * q1->w + q2->y * q1->z - q2->z * q1->y;
	result.y = q2->w * q1->y + q2->y * q1->w + q2->z * q1->x - q2->x * q1->z;
	result.z = q2->w * q1->z + q2->z * q1->w + q2->x * q1->y - q2->y * q1->x;
	result.w = q2->w * q1->w - q2->x * q1->x - q2->y * q1->y - q2->z * q1->z;

	*q1 = result;
}

//Multiply "q1" by "q2" (apply a rotation of "q1" to "q2") and store the result in "q1"! This assumes that "out" isn't "q1" or "q2".
void quatMultiplyQuatOut(const quat *q1, const quat *q2, quat *out){
	out->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
	out->y = q1->w * q2->y + q1->y * q2->w + q1->z * q2->x - q1->x * q2->z;
	out->z = q1->w * q2->z + q1->z * q2->w + q1->x * q2->y - q1->y * q2->x;
	out->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
}


//Apply a quaternion rotation to a vec3!
void quatApplyRotation(const quat *q, const vec3 *v, vec3 *out){
	float qvDot = vec3DotVec3Float(v, q->x, q->y, q->z);
	qvDot += qvDot;
	float qNorm = q->w * q->w - vec3Norm(q->x, q->y, q->z);
	vec3 qvCross;
	vec3CrossFloatVec3(q->x, q->y, q->z, v, &qvCross);
	vec3MultiplyS(&qvCross, q->w + q->w);

	out->x = qvDot * q->x + qNorm * v->x + qvCross.x;
	out->y = qvDot * q->y + qNorm * v->y + qvCross.y;
	out->z = qvDot * q->z + qNorm * v->z + qvCross.z;
}

//Apply a quaternion rotation to a vec3!
//Note: This should be a straight upgrade to the former method, but I'm not sure.
void quatApplyRotationFast(const quat *q, const vec3 *v, vec3 *out){
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


//Find the norm of a quaternion stored as four floats!
float quatNorm(const float x, const float y, const float z, const float w){
	return(x * x + y * y + z * z + w * w);
}

//Find the norm of a quaternion!
float quatNormQuat(const quat *q){
	return(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
}


//Find the magnitude (length) of a quaternion stored as four floats!
float quatMagnitude(const float x, const float y, const float z, const float w){
	return(sqrtf(quatNorm(x, y, z, w)));
}

//Find the magnitude (length) of a quaternion!
float quatMagnitudeQuat(const quat *q){
	return(sqrtf(quatNormQuat(q)));
}


//Find the dot product of two vec4s stored as four floats!
float quatDot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2){
	return(x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2);
}

//Find the dot product of a quat and a vec4 stored as four floats!
float quatDotQuatFloat(const quat *q, const float x, const float y, const float z, const float w){
	return(q->x * x + q->y * y + q->z * z + q->w * w);
}

//Find the dot product of two vec4s!
float quatDotQuat(const quat *q1, const quat *q2){
	return(q1->x * q2->x + q1->y * q2->y + q1->z * q2->z + q1->w * q2->w);
}


//Normalize a quaternion stored as four floats and store the result in "out"!
void quatNormalize(const float x, const float y, const float z, const float w, quat *out){
	const float magnitude = fastInvSqrt(quatNorm(x, y, z, w));

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

//Normalize a quaternion!
void quatNormalizeQuat(quat *q){
	const float magnitude = fastInvSqrt(quatNormQuat(q));

	q->x = q->x * magnitude;
	q->y = q->y * magnitude;
	q->z = q->z * magnitude;
	q->w = q->w * magnitude;
}

//Normalize a quaternion and store the result in "out"!
void quatNormalizeQuatOut(const quat *q, quat *out){
	const float magnitude = fastInvSqrt(quatNormQuat(q));

	out->x = q->x * magnitude;
	out->y = q->y * magnitude;
	out->z = q->z * magnitude;
	out->w = q->w * magnitude;
}


//Find the conjugate of a quaternion and store the result in "out"!
void quatConjugate(const quat *q, quat *out){
	out->x = -q->x;
	out->y = -q->y;
	out->z = -q->z;
	out->w = q->w;
}

//Find the conjugate of a quaternion and store the result in "out"!
//Note: This seems to work, but the fact that people only suggest
//      the former method makes me wonder if it's for a reason.
void quatConjugateFast(const quat *q, quat *out){
	out->w = -q->w;
}


//Convert a quaternion to an axis and an angle and store the result in "out"!
void quatToAxisAngle(const quat *q, vec4 *out){
	if(q->w > -1.f && q->w < 1.f){
		const float scale = fastInvSqrt(1.f - q->w * q->w);

		out->x = q->x * scale;
		out->y = q->y * scale;
		out->z = q->z * scale;
		//We normally multiply by 2, but it's faster to just do an addition.
		out->w = acosf(q->w);
		out->w += out->w;
	}else{
		out->x = out->y = out->z = 1.f;
		out->w = 0.f;
	}
}


//Rotate a quaternion (in radians)!
void quatRotateRad(quat *q, const float x, const float y, const float z){
	quat rot;
	quatInitEulerRad(&rot, x, y, z);
	quatMultiplyQuatBy(q, &rot);
}

//Rotate a quaternion (in degrees)!
void quatRotateDeg(quat *q, const float x, const float y, const float z){
	quatRotateRad(q, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

//Rotate a quaternion by a vec3 (in radians)!
void quatRotateVec3Rad(quat *q, const vec3 *v){
	quatRotateRad(q, v->x, v->y, v->z);
}

//Rotate a quaternion by a vec3 (in degrees)!
void quatRotateVec3Deg(quat *q, const vec3 *v){
	quatRotateRad(q, v->x * DEG_TO_RAD, v->y * DEG_TO_RAD, v->z * DEG_TO_RAD);
}


//Perform linear interpolation between two quaternions and store the result in "out"!
void quatLerp(const quat *q1, const quat *q2, const float time, quat *out){
	out->x = lerpNum(q1->x, q2->x, time);
	out->y = lerpNum(q1->y, q2->y, time);
	out->z = lerpNum(q1->z, q2->z, time);
	out->w = lerpNum(q1->w, q2->w, time);

	//It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(out);
}

/*
** Perform linear interpolation between two quaternions and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function accepts
** the starting orientation and the difference between it and the ending orientation.
*/
void quatLerpFast(const quat *q, const quat *offset, const float time, quat *out){
	out->x = lerpNumFast(q->x, offset->x, time);
	out->y = lerpNumFast(q->y, offset->y, time);
	out->z = lerpNumFast(q->z, offset->z, time);
	out->w = lerpNumFast(q->w, offset->w, time);

	//It's nice to be safe... but it isn't very fast.
	quatNormalizeQuat(out);
}

/*
** Perform spherical linear interpolation between
** two quaternions and store the result in "out"!
*/
void quatSlerp(const quat *q1, const quat *q2, const float time, quat *out){
	const float cosTheta = quatDotQuat(q1, q2);
	const float absCosTheta = fabsf(cosTheta);
	//We can perform linear interpolation of the angle is small enough (in this case, less than 1 degree).
	if(absCosTheta > QUAT_LERP_EPSILON){
		quatLerp(q1, q2, time, out);
	}else{
		const float theta = acosf(absCosTheta);
		const float invSinTheta = 1.f / sinf(theta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * invSinTheta;
		//If the dot product is negative, the interpolation won't take the shortest path.
		//We can fix this easily by negating the second quaternion.
		float sinThetaT;
		if(cosTheta >= 0.f){
			sinThetaT = sinf(theta * time) * invSinTheta;
		}else{
			sinThetaT = -(sinf(theta * time) * invSinTheta);
		}

		out->x = q1->x * sinThetaInvT + q2->x * sinThetaT;
		out->y = q1->y * sinThetaInvT + q2->y * sinThetaT;
		out->z = q1->z * sinThetaInvT + q2->z * sinThetaT;
		out->w = q1->w * sinThetaInvT + q2->w * sinThetaT;

		//It's nice to be safe... but it isn't very fast.
		quatNormalizeQuat(out);
	}
}

/*
** Slerp two quaternions using an optimized algorithm!
**
** Note: Although this should be significantly faster than
** the former method, it may also be slightly less accurate.
*/
void quatSlerpFast(const quat *q1, const quat *q2, const float time, quat *out){
	const float cosTheta = quatDotQuat(q1, q2);
	const float absCosTheta = fabsf(cosTheta);
	//We can perform linear interpolation of the angle is small enough (in this case, less than 1 degree).
	if(absCosTheta > QUAT_LERP_EPSILON){
		quatLerp(q1, q2, time, out);
	}else{
		const float theta = acosf(absCosTheta);
		const float sinTheta = fastInvSqrt(1.f - absCosTheta * absCosTheta);
		const float sinThetaInvT = sinf(theta * (1.f - time)) * sinTheta;
		//If the dot product is negative, the interpolation won't take the shortest path.
		//We can fix this easily by negating the second quaternion.
		float sinThetaT;
		if(cosTheta >= 0.f){
			sinThetaT = sinf(theta * time) * sinTheta;
		}else{
			sinThetaT = -(sinf(theta * time) * sinTheta);
		}

		out->x = q1->x * sinThetaInvT + q2->x * sinThetaT;
		out->y = q1->y * sinThetaInvT + q2->y * sinThetaT;
		out->z = q1->z * sinThetaInvT + q2->z * sinThetaT;
		out->w = q1->w * sinThetaInvT + q2->w * sinThetaT;

		//It's nice to be safe... but it isn't very fast.
		quatNormalizeQuat(out);
	}
}


/*
** Differentiate the quaternion
** "q" with angular velocity "w".
**
** dq/dt = 0.5f * quat(w.xyz, 0.f) * q
*/
void quatDifferentiate(quat *q, const vec3 *w){
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
void quatDifferentiateOut(const quat *q, const vec3 *w, quat *out){
	const quat spin = {
		.x = w->x * 0.5f,
		.y = w->y * 0.5f,
		.z = w->z * 0.5f,
		.w = 0.f
	};

	quatMultiplyQuatOut(&spin, q, out);
}

/*
** Integrate the quaternion "q" with angular
** velocity "w" using the step size "dt".
**
** q^(n + 1) = q^n + dq/dt * dt
*/
void quatIntegrate(quat *q, const vec3 *w, float dt){
	quat spin;

	dt *= 0.5f;
	//Multiply by half the timestep to
	//save multiplications later on.
	quatInitSet(&spin, w->x * dt, w->y * dt, w->z * dt, 0.f);

	quatMultiplyByQuat(&spin, q);
	quatAddVec4(q, &spin);
}

/*
** Integrate the quaternion "q" with angular velocity "w"
** using the step size "dt" and store the result in "out".
**
** q^(n + 1) = q^n + dq/dt * dt
*/
void quatIntegrateOut(const quat *q, const vec3 *w, float dt, quat *out){
	quat spin;

	dt *= 0.5f;
	//Multiply by half the timestep to
	//save multiplications later on.
	quatInitSet(&spin, w->x * dt, w->y * dt, w->z * dt, 0.f);

	quatMultiplyByQuat(&spin, q);
	quatAddVec4Out(q, &spin, out);
}