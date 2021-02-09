#include "mat4.h"


#include <string.h>
#include <math.h>

#include "mat3.h"

#include "utilMath.h"


mat4 g_mat4Identity = {
	.m[0][0] = 1.f, .m[0][1] = 0.f, .m[0][2] = 0.f, .m[0][3] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f, .m[1][2] = 0.f, .m[1][3] = 0.f,
	.m[2][0] = 0.f, .m[2][1] = 0.f, .m[2][2] = 1.f, .m[2][3] = 0.f,
	.m[3][0] = 0.f, .m[3][1] = 0.f, .m[3][2] = 0.f, .m[3][3] = 1.f
};


// Initialize the matrix's values to 0!
void mat4InitZero(mat4 *const restrict m){
	memset(m, 0.f, sizeof(*m));
}

// Initialize the matrix's values to 0!
mat4 mat4InitZeroC(){
	mat4 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

// Initialize the matrix to an identity matrix!
void mat4InitIdentity(mat4 *const restrict m){
	*m = g_mat4Identity;
}

// Initialize the matrix to an identity matrix!
mat4 mat4InitIdentityC(){
	return(g_mat4Identity);
}

// Initialise a matrix to a translation matrix!
void mat4InitTranslate(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = 1.f;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 1.f;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = 1.f;
	m->m[2][3] = 0.f;

	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
	m->m[3][3] = 1.f;
}

// Initialise a matrix to a translation matrix!
mat4 mat4InitTranslateC(const float x, const float y, const float z){
	const mat4 m = {
		.m[0][0] = 1.f,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,
		.m[0][3] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = 1.f,
		.m[1][2] = 0.f,
		.m[1][3] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = 1.f,
		.m[2][3] = 0.f,

		.m[3][0] = x,
		.m[3][1] = y,
		.m[3][2] = z,
		.m[3][3] = 1.f
	};

	return(m);
}

// Initialise a matrix to a translation matrix!
void mat4InitTranslate4(mat4 *const restrict m, const float x, const float y, const float z, const float w){
	m->m[0][0] = 1.f;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 1.f;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = 1.f;
	m->m[2][3] = 0.f;

	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
	m->m[3][3] = w;
}

// Initialise a matrix to a translation matrix!
mat4 mat4InitTranslate4C(const float x, const float y, const float z, const float w){
	const mat4 m = {
		.m[0][0] = 1.f,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,
		.m[0][3] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = 1.f,
		.m[1][2] = 0.f,
		.m[1][3] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = 1.f,
		.m[2][3] = 0.f,

		.m[3][0] = x,
		.m[3][1] = y,
		.m[3][2] = z,
		.m[3][3] = w
	};

	return(m);
}

// Initialise a matrix to a translation matrix using a vec3!
void mat4InitTranslateVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4InitTranslate(m, v->x, v->y, v->z);
}

// Initialise a matrix to a translation matrix using a vec3!
mat4 mat4InitTranslateVec3C(const vec3 v){
	return(mat4InitTranslateC(v.x, v.y, v.z));
}

// Initialize a matrix from XYZ Euler angles (in radians)!
void mat4InitEulerXYZ(mat4 *const restrict m, const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;

	m->m[0][0] = cy*cz;
	m->m[0][1] = cy*sz;
	m->m[0][2] = -sy;
	m->m[0][3] = 0.f;

	m->m[1][0] = sxsy*cz - cx*sz;
	m->m[1][1] = sxsy*sz + cx*cz;
	m->m[1][2] = sx*cy;
	m->m[1][3] = 0.f;

	m->m[2][0] = cxsy*cz + sx*sz;
	m->m[2][1] = cxsy*sz - sx*cz;
	m->m[2][2] = cx*cy;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Initialize a matrix from XYZ Euler angles (in radians)!
mat4 mat4InitEulerXYZC(const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;

	const mat4 m = {
		.m[0][0] = cy*cz,
		.m[0][1] = cy*sz,
		.m[0][2] = -sy,
		.m[0][3] = 0.f,

		.m[1][0] = sxsy*cz - cx*sz,
		.m[1][1] = sxsy*sz + cx*cz,
		.m[1][2] = sx*cy,
		.m[1][3] = 0.f,

		.m[2][0] = cxsy*cz + sx*sz,
		.m[2][1] = cxsy*sz - sx*cz,
		.m[2][2] = cx*cy,
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = 1.f
	};


	return(m);
}

// Initialise a matrix to a rotation matrix!
void mat4InitRotateQuat(mat4 *const restrict m, const quat *const restrict q){
	const float xx = q->x * q->x;
	const float xy = q->x * q->y;
	const float xz = q->x * q->z;
	const float xw = q->x * q->w;
	const float yy = q->y * q->y;
	const float yz = q->y * q->z;
	const float yw = q->y * q->w;
	const float zz = q->z * q->z;
	const float zw = q->z * q->w;

	// Rotate our matrix by the quaternion!
	m->m[0][0] = 1.f - 2.f * (yy + zz);
	m->m[0][1] = 2.f * (xy + zw);
	m->m[0][2] = 2.f * (xz - yw);
	m->m[0][3] = 0.f;

	m->m[1][0] = 2.f * (xy - zw);
	m->m[1][1] = 1.f - 2.f * (xx + zz);
	m->m[1][2] = 2.f * (yz + xw);
	m->m[1][3] = 0.f;

	m->m[2][0] = 2.f * (xz + yw);
	m->m[2][1] = 2.f * (yz - xw);
	m->m[2][2] = 1.f - 2.f * (xx + yy);
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Initialise a matrix to a rotation matrix!
mat4 mat4InitRotateQuatC(const quat q){
	const float xx = q.x * q.x;
	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float xw = q.x * q.w;
	const float yy = q.y * q.y;
	const float yz = q.y * q.z;
	const float yw = q.y * q.w;
	const float zz = q.z * q.z;
	const float zw = q.z * q.w;

	// Rotate our matrix by the quaternion!
	const mat4 m = {
		.m[0][0] = 1.f - 2.f * (yy + zz),
		.m[0][1] = 2.f * (xy + zw),
		.m[0][2] = 2.f * (xz - yw),
		.m[0][3] = 0.f,

		.m[1][0] = 2.f * (xy - zw),
		.m[1][1] = 1.f - 2.f * (xx + zz),
		.m[1][2] = 2.f * (yz + xw),
		.m[1][3] = 0.f,

		.m[2][0] = 2.f * (xz + yw),
		.m[2][1] = 2.f * (yz - xw),
		.m[2][2] = 1.f - 2.f * (xx + yy),
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = 1.f
	};


	return(m);
}

// Initialise a matrix to a scale matrix!
void mat4InitScale(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = y;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = z;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Initialise a matrix to a scale matrix!
mat4 mat4InitScaleC(const float x, const float y, const float z){
	const mat4 m = {
		.m[0][0] = x,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,
		.m[0][3] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = y,
		.m[1][2] = 0.f,
		.m[1][3] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = z,
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = 1.f
	};

	return(m);
}

// Initialise a matrix to a scale matrix!
void mat4InitScale4(mat4 *const restrict m, const float x, const float y, const float z, const float w){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = y;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = z;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = w;
}

// Initialise a matrix to a scale matrix!
mat4 mat4InitScale4C(const float x, const float y, const float z, const float w){
	const mat4 m = {
		.m[0][0] = x,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,
		.m[0][3] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = y,
		.m[1][2] = 0.f,
		.m[1][3] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = z,
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = w
	};

	return(m);
}

// Initialise a matrix to a scale matrix using a vec3!
void mat4InitScaleVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4InitScale(m, v->x, v->y, v->z);
}

// Initialise a matrix to a scale matrix using a vec3!
mat4 mat4InitScaleVec3C(const vec3 v){
	return(mat4InitScaleC(v.x, v.y, v.z));
}

// Initialise a matrix to a scale matrix using a vec4!
void mat4InitScaleVec4(mat4 *const restrict m, const vec4 *const restrict v){
	mat4InitScale4(m, v->x, v->y, v->z, v->w);
}

// Initialise a matrix to a scale matrix using a vec4!
mat4 mat4InitScaleVec4C(const vec4 v){
	return(mat4InitScale4C(v.x, v.y, v.z, v.w));
}


// Add the matrix "m2" to "m1"!
void mat4AddMat4(mat4 *const restrict m1, const mat4 *const restrict m2){
	m1->m[0][0] += m2->m[0][0];
	m1->m[0][1] += m2->m[0][1];
	m1->m[0][2] += m2->m[0][2];
	m1->m[0][3] += m2->m[0][3];

	m1->m[1][0] += m2->m[1][0];
	m1->m[1][1] += m2->m[1][1];
	m1->m[1][2] += m2->m[1][2];
	m1->m[1][3] += m2->m[1][3];

	m1->m[2][0] += m2->m[2][0];
	m1->m[2][1] += m2->m[2][1];
	m1->m[2][2] += m2->m[2][2];
	m1->m[2][3] += m2->m[2][3];

	m1->m[3][0] += m2->m[3][0];
	m1->m[3][1] += m2->m[3][1];
	m1->m[3][2] += m2->m[3][2];
	m1->m[3][3] += m2->m[3][3];
}

// Add the matrix "m2" to "m1" and store the result in "out"!
void mat4AddMat4Out(const mat4 *const restrict m1, const mat4 *const restrict m2, mat4 *const restrict out){
	out->m[0][0] = m1->m[0][0] + m2->m[0][0];
	out->m[0][1] = m1->m[0][1] + m2->m[0][1];
	out->m[0][2] = m1->m[0][2] + m2->m[0][2];
	out->m[0][3] = m1->m[0][3] + m2->m[0][3];

	out->m[1][0] = m1->m[1][0] + m2->m[1][0];
	out->m[1][1] = m1->m[1][1] + m2->m[1][1];
	out->m[1][2] = m1->m[1][2] + m2->m[1][2];
	out->m[1][3] = m1->m[1][3] + m2->m[1][3];

	out->m[2][0] = m1->m[2][0] + m2->m[2][0];
	out->m[2][1] = m1->m[2][1] + m2->m[2][1];
	out->m[2][2] = m1->m[2][2] + m2->m[2][2];
	out->m[2][3] = m1->m[2][3] + m2->m[2][3];

	out->m[3][0] = m1->m[3][0] + m2->m[3][0];
	out->m[3][1] = m1->m[3][1] + m2->m[3][1];
	out->m[3][2] = m1->m[3][2] + m2->m[3][2];
	out->m[3][3] = m1->m[3][3] + m2->m[3][3];
}

// Add the matrix "m2" to "m1" and return the result!
mat4 mat4AddMat4C(const mat4 m1, const mat4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] + m2.m[0][0],
		.m[0][1] = m1.m[0][1] + m2.m[0][1],
		.m[0][2] = m1.m[0][2] + m2.m[0][2],
		.m[0][3] = m1.m[0][3] + m2.m[0][3],

		.m[1][0] = m1.m[1][0] + m2.m[1][0],
		.m[1][1] = m1.m[1][1] + m2.m[1][1],
		.m[1][2] = m1.m[1][2] + m2.m[1][2],
		.m[1][3] = m1.m[1][3] + m2.m[1][3],

		.m[2][0] = m1.m[2][0] + m2.m[2][0],
		.m[2][1] = m1.m[2][1] + m2.m[2][1],
		.m[2][2] = m1.m[2][2] + m2.m[2][2],
		.m[2][3] = m1.m[2][3] + m2.m[2][3],

		.m[3][0] = m1.m[3][0] + m2.m[3][0],
		.m[3][1] = m1.m[3][1] + m2.m[3][1],
		.m[3][2] = m1.m[3][2] + m2.m[3][2],
		.m[3][3] = m1.m[3][3] + m2.m[3][3]
	};

	return(out);
}


// Multiply a vec3 by a matrix (m*v)!
void mat4MultiplyVec3By(const mat4 *const restrict m, vec3 *const restrict v){
	const vec3 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z + m->m[3][0];
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z + m->m[3][1];
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z + m->m[3][2];
}

// Multiply a vec3 by a matrix (m*v) and store the result in "out"! This assumes that "out" isn't "v".
void mat4MultiplyVec3ByOut(const mat4 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0];
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1];
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2];
}

// Multiply a vec3 by a matrix (m*v)!
vec3 mat4MultiplyVec3ByC(const mat4 m, const vec3 v){
	const vec3 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0],
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1],
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2]
	};

	return(out);
}

// Multiply a matrix by a vec3 (v*m)!
void mat4MultiplyByVec3(const mat4 *const restrict m, vec3 *const restrict v){
	const vec3 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1] + temp.z * m->m[0][2];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1] + temp.z * m->m[1][2];
	v->z = temp.x * m->m[2][0] + temp.y * m->m[2][1] + temp.z * m->m[2][2];
}

// Multiply a matrix by a vec3 (v*m) and store the result in "out"! This assumes that "out" isn't "v".
void mat4MultiplyByVec3Out(const mat4 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out){
	out->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	out->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}

// Multiply a matrix by a vec3 (v*m)!
vec3 mat4MultiplyByVec3C(const mat4 m, const vec3 v){
	const vec3 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
		.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]
	};

	return(out);
}

// Multiply a vec4 by a matrix (m*v)!
void mat4MultiplyVec4By(const mat4 *const restrict m, vec4 *const restrict v){
	const vec4 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z + m->m[3][0] * temp.w;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z + m->m[3][1] * temp.w;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z + m->m[3][2] * temp.w;
	v->w = m->m[0][3] * temp.x + m->m[1][3] * temp.y + m->m[2][3] * temp.z + m->m[3][3] * temp.w;
}

// Multiply a vec4 by a matrix (m*v) and store the result in "out"! This assumes that "out" isn't "v".
void mat4MultiplyVec4ByOut(const mat4 *const restrict m, const vec4 *const restrict v, vec4 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0] * v->w;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1] * v->w;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2] * v->w;
	out->w = m->m[0][3] * v->x + m->m[1][3] * v->y + m->m[2][3] * v->z + m->m[3][3] * v->w;
}

// Multiply a vec4 by a matrix (m*v)!
vec4 mat4MultiplyVec4ByC(const mat4 m, const vec4 v){
	const vec4 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w,
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w,
		.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w
	};

	return(out);
}

// Multiply a matrix by a vec4 (v*m)!
void mat4MultiplyByVec4(const mat4 *const restrict m, vec4 *const restrict v){
	const vec4 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1] + temp.z * m->m[0][2] + temp.w * m->m[0][3];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1] + temp.z * m->m[1][2] + temp.w * m->m[1][3];
	v->z = temp.x * m->m[2][0] + temp.y * m->m[2][1] + temp.z * m->m[2][2] + temp.w * m->m[2][3];
	v->w = temp.x * m->m[3][0] + temp.y * m->m[3][1] + temp.z * m->m[3][2] + temp.w * m->m[3][3];
}

// Multiply a matrix by a vec4 (v*m) and store the result in "out"!
void mat4MultiplyByVec4Out(const mat4 *const restrict m, const vec4 *const restrict v, vec4 *const restrict out){
	out->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + v->w * m->m[0][3];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + v->w * m->m[1][3];
	out->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + v->w * m->m[2][3];
	out->w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w * m->m[3][3];
}

// Multiply a matrix by a vec4 (v*m)!
vec4 mat4MultiplyByVec4C(const mat4 m, const vec4 v){
	const vec4 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3],
		.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3],
		.w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3]
	};

	return(out);
}

// Right-multiply "m1" by "m2" (m1*m2)!
void mat4MultiplyMat4By(mat4 *const restrict m1, const mat4 m2){
	const mat4 tempMatrix1 = *m1;

	m1->m[0][0] = tempMatrix1.m[0][0] * m2.m[0][0] + tempMatrix1.m[1][0] * m2.m[0][1] + tempMatrix1.m[2][0] * m2.m[0][2] + tempMatrix1.m[3][0] * m2.m[0][3];
	m1->m[0][1] = tempMatrix1.m[0][1] * m2.m[0][0] + tempMatrix1.m[1][1] * m2.m[0][1] + tempMatrix1.m[2][1] * m2.m[0][2] + tempMatrix1.m[3][1] * m2.m[0][3];
	m1->m[0][2] = tempMatrix1.m[0][2] * m2.m[0][0] + tempMatrix1.m[1][2] * m2.m[0][1] + tempMatrix1.m[2][2] * m2.m[0][2] + tempMatrix1.m[3][2] * m2.m[0][3];
	m1->m[0][3] = tempMatrix1.m[0][3] * m2.m[0][0] + tempMatrix1.m[1][3] * m2.m[0][1] + tempMatrix1.m[2][3] * m2.m[0][2] + tempMatrix1.m[3][3] * m2.m[0][3];

	m1->m[1][0] = tempMatrix1.m[0][0] * m2.m[1][0] + tempMatrix1.m[1][0] * m2.m[1][1] + tempMatrix1.m[2][0] * m2.m[1][2] + tempMatrix1.m[3][0] * m2.m[1][3];
	m1->m[1][1] = tempMatrix1.m[0][1] * m2.m[1][0] + tempMatrix1.m[1][1] * m2.m[1][1] + tempMatrix1.m[2][1] * m2.m[1][2] + tempMatrix1.m[3][1] * m2.m[1][3];
	m1->m[1][2] = tempMatrix1.m[0][2] * m2.m[1][0] + tempMatrix1.m[1][2] * m2.m[1][1] + tempMatrix1.m[2][2] * m2.m[1][2] + tempMatrix1.m[3][2] * m2.m[1][3];
	m1->m[1][3] = tempMatrix1.m[0][3] * m2.m[1][0] + tempMatrix1.m[1][3] * m2.m[1][1] + tempMatrix1.m[2][3] * m2.m[1][2] + tempMatrix1.m[3][3] * m2.m[1][3];

	m1->m[2][0] = tempMatrix1.m[0][0] * m2.m[2][0] + tempMatrix1.m[1][0] * m2.m[2][1] + tempMatrix1.m[2][0] * m2.m[2][2] + tempMatrix1.m[3][0] * m2.m[2][3];
	m1->m[2][1] = tempMatrix1.m[0][1] * m2.m[2][0] + tempMatrix1.m[1][1] * m2.m[2][1] + tempMatrix1.m[2][1] * m2.m[2][2] + tempMatrix1.m[3][1] * m2.m[2][3];
	m1->m[2][2] = tempMatrix1.m[0][2] * m2.m[2][0] + tempMatrix1.m[1][2] * m2.m[2][1] + tempMatrix1.m[2][2] * m2.m[2][2] + tempMatrix1.m[3][2] * m2.m[2][3];
	m1->m[2][3] = tempMatrix1.m[0][3] * m2.m[2][0] + tempMatrix1.m[1][3] * m2.m[2][1] + tempMatrix1.m[2][3] * m2.m[2][2] + tempMatrix1.m[3][3] * m2.m[2][3];

	m1->m[3][0] = tempMatrix1.m[0][0] * m2.m[3][0] + tempMatrix1.m[1][0] * m2.m[3][1] + tempMatrix1.m[2][0] * m2.m[3][2] + tempMatrix1.m[3][0] * m2.m[3][3];
	m1->m[3][1] = tempMatrix1.m[0][1] * m2.m[3][0] + tempMatrix1.m[1][1] * m2.m[3][1] + tempMatrix1.m[2][1] * m2.m[3][2] + tempMatrix1.m[3][1] * m2.m[3][3];
	m1->m[3][2] = tempMatrix1.m[0][2] * m2.m[3][0] + tempMatrix1.m[1][2] * m2.m[3][1] + tempMatrix1.m[2][2] * m2.m[3][2] + tempMatrix1.m[3][2] * m2.m[3][3];
	m1->m[3][3] = tempMatrix1.m[0][3] * m2.m[3][0] + tempMatrix1.m[1][3] * m2.m[3][1] + tempMatrix1.m[2][3] * m2.m[3][2] + tempMatrix1.m[3][3] * m2.m[3][3];
}

// Left-multiply "m1" by "m2" (m2*m1)!
void mat4MultiplyByMat4(mat4 *const restrict m1, const mat4 m2){
	const mat4 tempMatrix1 = *m1;

	m1->m[0][0] = m2.m[0][0] * tempMatrix1.m[0][0] + m2.m[1][0] * tempMatrix1.m[0][1] + m2.m[2][0] * tempMatrix1.m[0][2] + m2.m[3][0] * tempMatrix1.m[0][3];
	m1->m[0][1] = m2.m[0][1] * tempMatrix1.m[0][0] + m2.m[1][1] * tempMatrix1.m[0][1] + m2.m[2][1] * tempMatrix1.m[0][2] + m2.m[3][1] * tempMatrix1.m[0][3];
	m1->m[0][2] = m2.m[0][2] * tempMatrix1.m[0][0] + m2.m[1][2] * tempMatrix1.m[0][1] + m2.m[2][2] * tempMatrix1.m[0][2] + m2.m[3][2] * tempMatrix1.m[0][3];
	m1->m[0][3] = m2.m[0][3] * tempMatrix1.m[0][0] + m2.m[1][3] * tempMatrix1.m[0][1] + m2.m[2][3] * tempMatrix1.m[0][2] + m2.m[3][3] * tempMatrix1.m[0][3];

	m1->m[1][0] = m2.m[0][0] * tempMatrix1.m[1][0] + m2.m[1][0] * tempMatrix1.m[1][1] + m2.m[2][0] * tempMatrix1.m[1][2] + m2.m[3][0] * tempMatrix1.m[1][3];
	m1->m[1][1] = m2.m[0][1] * tempMatrix1.m[1][0] + m2.m[1][1] * tempMatrix1.m[1][1] + m2.m[2][1] * tempMatrix1.m[1][2] + m2.m[3][1] * tempMatrix1.m[1][3];
	m1->m[1][2] = m2.m[0][2] * tempMatrix1.m[1][0] + m2.m[1][2] * tempMatrix1.m[1][1] + m2.m[2][2] * tempMatrix1.m[1][2] + m2.m[3][2] * tempMatrix1.m[1][3];
	m1->m[1][3] = m2.m[0][3] * tempMatrix1.m[1][0] + m2.m[1][3] * tempMatrix1.m[1][1] + m2.m[2][3] * tempMatrix1.m[1][2] + m2.m[3][3] * tempMatrix1.m[1][3];

	m1->m[2][0] = m2.m[0][0] * tempMatrix1.m[2][0] + m2.m[1][0] * tempMatrix1.m[2][1] + m2.m[2][0] * tempMatrix1.m[2][2] + m2.m[3][0] * tempMatrix1.m[2][3];
	m1->m[2][1] = m2.m[0][1] * tempMatrix1.m[2][0] + m2.m[1][1] * tempMatrix1.m[2][1] + m2.m[2][1] * tempMatrix1.m[2][2] + m2.m[3][1] * tempMatrix1.m[2][3];
	m1->m[2][2] = m2.m[0][2] * tempMatrix1.m[2][0] + m2.m[1][2] * tempMatrix1.m[2][1] + m2.m[2][2] * tempMatrix1.m[2][2] + m2.m[3][2] * tempMatrix1.m[2][3];
	m1->m[2][3] = m2.m[0][3] * tempMatrix1.m[2][0] + m2.m[1][3] * tempMatrix1.m[2][1] + m2.m[2][3] * tempMatrix1.m[2][2] + m2.m[3][3] * tempMatrix1.m[2][3];

	m1->m[3][0] = m2.m[0][0] * tempMatrix1.m[3][0] + m2.m[1][0] * tempMatrix1.m[3][1] + m2.m[2][0] * tempMatrix1.m[3][2] + m2.m[3][0] * tempMatrix1.m[3][3];
	m1->m[3][1] = m2.m[0][1] * tempMatrix1.m[3][0] + m2.m[1][1] * tempMatrix1.m[3][1] + m2.m[2][1] * tempMatrix1.m[3][2] + m2.m[3][1] * tempMatrix1.m[3][3];
	m1->m[3][2] = m2.m[0][2] * tempMatrix1.m[3][0] + m2.m[1][2] * tempMatrix1.m[3][1] + m2.m[2][2] * tempMatrix1.m[3][2] + m2.m[3][2] * tempMatrix1.m[3][3];
	m1->m[3][3] = m2.m[0][3] * tempMatrix1.m[3][0] + m2.m[1][3] * tempMatrix1.m[3][1] + m2.m[2][3] * tempMatrix1.m[3][2] + m2.m[3][3] * tempMatrix1.m[3][3];
}

// Right-multiply "m1" by "m2" (m1*m2) and store the result in "out"! This assumes that "out" isn't "m1" or "m2".
void mat4MultiplyMat4ByOut(const mat4 m1, const mat4 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];
	out->m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];
	out->m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];
	out->m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
	out->m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] + m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];
}

// Right-multiply "m1" by "m2" (m1*m2) and return the result!
mat4 mat4MultiplyMat4ByC(const mat4 m1, const mat4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3],
		.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3],
		.m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3],
		.m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3],
		.m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] + m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3]
	};

	return(out);
}


// Left-multiply a matrix by a translation matrix stored as three floats!
void mat4Translate(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] += m->m[0][3] * x;
	m->m[0][1] += m->m[0][3] * y;
	m->m[0][2] += m->m[0][3] * z;

	m->m[1][0] += m->m[1][3] * x;
	m->m[1][1] += m->m[1][3] * y;
	m->m[1][2] += m->m[1][3] * z;

	m->m[2][0] += m->m[2][3] * x;
	m->m[2][1] += m->m[2][3] * y;
	m->m[2][2] += m->m[2][3] * z;

	m->m[3][0] += m->m[3][3] * x;
	m->m[3][1] += m->m[3][3] * y;
	m->m[3][2] += m->m[3][3] * z;
}

// Left-multiply a matrix by a translation matrix stored as three floats!
mat4 mat4TranslateC(mat4 m, const float x, const float y, const float z){
	m.m[0][0] += m.m[0][3] * x;
	m.m[0][1] += m.m[0][3] * y;
	m.m[0][2] += m.m[0][3] * z;

	m.m[1][0] += m.m[1][3] * x;
	m.m[1][1] += m.m[1][3] * y;
	m.m[1][2] += m.m[1][3] * z;

	m.m[2][0] += m.m[2][3] * x;
	m.m[2][1] += m.m[2][3] * y;
	m.m[2][2] += m.m[2][3] * z;

	m.m[3][0] += m.m[3][3] * x;
	m.m[3][1] += m.m[3][3] * y;
	m.m[3][2] += m.m[3][3] * z;

	return(m);
}

// Left-multiply a matrix by a translation matrix stored as a vec3!
void mat4TranslateVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4Translate(m, v->x, v->y, v->z);
}

// Left-multiply a matrix by a translation matrix stored as a vec3!
mat4 mat4TranslateVec3C(const mat4 m, const vec3 v){
	return(mat4TranslateC(m, v.x, v.y, v.z));
}

// Right-multiply a matrix by a translation matrix stored as three floats!
void mat4TranslatePre(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
	m->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3];
}

// Right-multiply a matrix by a translation matrix stored as three floats!
mat4 mat4TranslatePreC(mat4 m, const float x, const float y, const float z){
	m.m[3][0] = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0];
	m.m[3][1] = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1];
	m.m[3][2] = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2];
	m.m[3][3] = m.m[0][3] * x + m.m[1][3] * y + m.m[2][3] * z + m.m[3][3];

	return(m);
}

// Right-multiply a matrix by a translation matrix stored as a vec3!
void mat4TranslatePreVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4TranslatePre(m, v->x, v->y, v->z);
}

// Right-multiply a matrix by a translation matrix stored as a vec3!
mat4 mat4TranslatePreVec3C(const mat4 m, const vec3 v){
	return(mat4TranslatePreC(m, v.x, v.y, v.z));
}

// Left-multiply a transformation matrix by a translation matrix stored as three floats!
void mat4TranslateTransform(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[3][0] += x;
	m->m[3][1] += y;
	m->m[3][2] += z;
}

// Left-multiply a transformation matrix by a translation matrix stored as three floats!
mat4 mat4TranslateTransformC(mat4 m, const float x, const float y, const float z){
	m.m[3][0] += x;
	m.m[3][1] += y;
	m.m[3][2] += z;

	return(m);
}

// Left-multiply a transformation matrix by a translation matrix stored as a vec3!
void mat4TranslateTransformVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4TranslateTransform(m, v->x, v->y, v->z);
}

// Left-multiply a transformation matrix by a translation matrix stored as a vec3!
mat4 mat4TranslateTransformVec3C(const mat4 m, const vec3 v){
	return(mat4TranslateTransformC(m, v.x, v.y, v.z));
}


/*
** Rotate a matrix!
** The order of rotations is XYZ.
*/
void mat4RotateXYZ(mat4 *const restrict m, const float x, const float y, const float z){
	mat4 rotMatrix;
	mat4InitEulerXYZ(&rotMatrix, x, y, z);
	mat4MultiplyByMat4(m, rotMatrix);
}

/*
** Rotate a matrix!
** The order of rotations is XYZ.
*/
mat4 mat4RotateXYZC(const mat4 m, const float x, const float y, const float z){
	const mat4 rotMatrix = mat4InitEulerXYZC(x, y, z);
	return(mat4MultiplyMat4ByC(rotMatrix, m));
}

/*
** Rotate a matrix by a vec3 (using radians)!
** The order of rotations is XYZ.
*/
void mat4RotateByVec3XYZ(mat4 *const restrict m, const vec3 *const restrict v){
	mat4RotateXYZ(m, v->x, v->y, v->z);
}

/*
** Rotate a matrix by a vec3 (using radians)!
** The order of rotations is ZYX.
*/
mat4 mat4RotateByVec3XYZC(const mat4 m, const vec3 v){
	return(mat4RotateXYZC(m, v.x, v.y, v.z));
}

// Rotate a matrix by an axis and an angle!
void mat4RotateAxisAngle(mat4 *const restrict m, const vec4 *const restrict v){
	const float c = cosf(v->w);
	const float s = sinf(v->w);
	const float t = 1.f - c;
	mat3 rotMatrix;
	const mat4 tempMatrix = *m;

	// Normalize the axis!
	vec3 normalAxis;
	vec3NormalizeFast(v->x, v->y, v->z, &normalAxis);
	vec3 tempAxis;
	vec3MultiplySOut(&normalAxis, t, &tempAxis);

	// Convert the axis angle to a rotation matrix! Note that this is
	// a row-major matrix as opposed to our usual column-major format.
	rotMatrix.m[0][0] = tempAxis.x * normalAxis.x + c;
	rotMatrix.m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s;
	rotMatrix.m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s;

	rotMatrix.m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s;
	rotMatrix.m[1][1] = tempAxis.y * normalAxis.y + c;
	rotMatrix.m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s;

	rotMatrix.m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s;
	rotMatrix.m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s;
	rotMatrix.m[2][2] = tempAxis.z * normalAxis.z + c;

	// Now rotate our matrix by it!
	m->m[0][0] = rotMatrix.m[0][0] * tempMatrix.m[0][0] + rotMatrix.m[0][1] * tempMatrix.m[1][0] + rotMatrix.m[0][2] * tempMatrix.m[2][0];
	m->m[0][1] = rotMatrix.m[1][0] * tempMatrix.m[0][0] + rotMatrix.m[1][1] * tempMatrix.m[1][0] + rotMatrix.m[1][2] * tempMatrix.m[2][0];
	m->m[0][2] = rotMatrix.m[2][0] * tempMatrix.m[0][0] + rotMatrix.m[2][1] * tempMatrix.m[1][0] + rotMatrix.m[2][2] * tempMatrix.m[2][0];

	m->m[1][0] = rotMatrix.m[0][0] * tempMatrix.m[0][1] + rotMatrix.m[0][1] * tempMatrix.m[1][1] + rotMatrix.m[0][2] * tempMatrix.m[2][1];
	m->m[1][1] = rotMatrix.m[1][0] * tempMatrix.m[0][1] + rotMatrix.m[1][1] * tempMatrix.m[1][1] + rotMatrix.m[1][2] * tempMatrix.m[2][1];
	m->m[1][2] = rotMatrix.m[2][0] * tempMatrix.m[0][1] + rotMatrix.m[2][1] * tempMatrix.m[1][1] + rotMatrix.m[2][2] * tempMatrix.m[2][1];

	m->m[2][0] = rotMatrix.m[0][0] * tempMatrix.m[0][2] + rotMatrix.m[0][1] * tempMatrix.m[1][2] + rotMatrix.m[0][2] * tempMatrix.m[2][2];
	m->m[2][1] = rotMatrix.m[1][0] * tempMatrix.m[0][2] + rotMatrix.m[1][1] * tempMatrix.m[1][2] + rotMatrix.m[1][2] * tempMatrix.m[2][2];
	m->m[2][2] = rotMatrix.m[2][0] * tempMatrix.m[0][2] + rotMatrix.m[2][1] * tempMatrix.m[1][2] + rotMatrix.m[2][2] * tempMatrix.m[2][2];

	m->m[3][0] = rotMatrix.m[0][0] * tempMatrix.m[0][3] + rotMatrix.m[0][1] * tempMatrix.m[1][3] + rotMatrix.m[0][2] * tempMatrix.m[2][3];
	m->m[3][1] = rotMatrix.m[1][0] * tempMatrix.m[0][3] + rotMatrix.m[1][1] * tempMatrix.m[1][3] + rotMatrix.m[1][2] * tempMatrix.m[2][3];
	m->m[3][2] = rotMatrix.m[2][0] * tempMatrix.m[0][3] + rotMatrix.m[2][1] * tempMatrix.m[1][3] + rotMatrix.m[2][2] * tempMatrix.m[2][3];
}

// Rotate a matrix by an axis and an angle!
mat4 mat4RotateAxisAngleC(const mat4 m, const vec4 v){
	const float c = cosf(v.w);
	const float s = sinf(v.w);
	const float t = 1.f - c;

	// Normalize the axis!
	const vec3 normalAxis = vec3NormalizeFastC(v.x, v.y, v.z);
	const vec3 tempAxis = vec3MultiplySC(normalAxis, t);

	// Convert the axis angle to a rotation matrix! Note that this is
	// a row-major matrix as opposed to our usual column-major format.
	const mat3 rotMatrix = {
		.m[0][0] = tempAxis.x * normalAxis.x + c,
		.m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s,
		.m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s,

		.m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s,
		.m[1][1] = tempAxis.y * normalAxis.y + c,
		.m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s,

		.m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s,
		.m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s,
		.m[2][2] = tempAxis.z * normalAxis.z + c
	};

	// Now rotate our matrix by it!
	const mat4 out = {
		.m[0][0] = rotMatrix.m[0][0] * m.m[0][0] + rotMatrix.m[0][1] * m.m[1][0] + rotMatrix.m[0][2] * m.m[2][0],
		.m[0][1] = rotMatrix.m[1][0] * m.m[0][0] + rotMatrix.m[1][1] * m.m[1][0] + rotMatrix.m[1][2] * m.m[2][0],
		.m[0][2] = rotMatrix.m[2][0] * m.m[0][0] + rotMatrix.m[2][1] * m.m[1][0] + rotMatrix.m[2][2] * m.m[2][0],
		.m[0][3] = m.m[0][3],

		.m[1][0] = rotMatrix.m[0][0] * m.m[0][1] + rotMatrix.m[0][1] * m.m[1][1] + rotMatrix.m[0][2] * m.m[2][1],
		.m[1][1] = rotMatrix.m[1][0] * m.m[0][1] + rotMatrix.m[1][1] * m.m[1][1] + rotMatrix.m[1][2] * m.m[2][1],
		.m[1][2] = rotMatrix.m[2][0] * m.m[0][1] + rotMatrix.m[2][1] * m.m[1][1] + rotMatrix.m[2][2] * m.m[2][1],
		.m[1][3] = m.m[1][3],

		.m[2][0] = rotMatrix.m[0][0] * m.m[0][2] + rotMatrix.m[0][1] * m.m[1][2] + rotMatrix.m[0][2] * m.m[2][2],
		.m[2][1] = rotMatrix.m[1][0] * m.m[0][2] + rotMatrix.m[1][1] * m.m[1][2] + rotMatrix.m[1][2] * m.m[2][2],
		.m[2][2] = rotMatrix.m[2][0] * m.m[0][2] + rotMatrix.m[2][1] * m.m[1][2] + rotMatrix.m[2][2] * m.m[2][2],
		.m[2][3] = m.m[2][3],

		.m[3][0] = rotMatrix.m[0][0] * m.m[0][3] + rotMatrix.m[0][1] * m.m[1][3] + rotMatrix.m[0][2] * m.m[2][3],
		.m[3][1] = rotMatrix.m[1][0] * m.m[0][3] + rotMatrix.m[1][1] * m.m[1][3] + rotMatrix.m[1][2] * m.m[2][3],
		.m[3][2] = rotMatrix.m[2][0] * m.m[0][3] + rotMatrix.m[2][1] * m.m[1][3] + rotMatrix.m[2][2] * m.m[2][3],
		.m[3][3] = m.m[3][3]
	};


	return(out);
}

// Rotate a matrix by a quaternion!
void mat4RotateQuat(mat4 *const restrict m, const quat *const restrict q){
	const float xx = q->x * q->x;
	const float xy = q->x * q->y;
	const float xz = q->x * q->z;
	const float xw = q->x * q->w;
	const float yy = q->y * q->y;
	const float yz = q->y * q->z;
	const float yw = q->y * q->w;
	const float zz = q->z * q->z;
	const float zw = q->z * q->w;
	const mat4 tempMatrix = *m;

	// Convert the quaternion to a rotation matrix!
	const mat3 rotMatrix = {
		.m[0][0] = 1.f - 2.f * (yy + zz),
		.m[0][1] = 2.f * (xy + zw),
		.m[0][2] = 2.f * (xz - yw),

		.m[1][0] = 2.f * (xy - zw),
		.m[1][1] = 1.f - 2.f * (xx + zz),
		.m[1][2] = 2.f * (yz + xw),

		.m[2][0] = 2.f * (xz + yw),
		.m[2][1] = 2.f * (yz - xw),
		.m[2][2] = 1.f - 2.f * (xx + yy)
	};

	// Now rotate our matrix by it!
	m->m[0][0] = rotMatrix.m[0][0] * tempMatrix.m[0][0] + rotMatrix.m[0][1] * tempMatrix.m[1][0] + rotMatrix.m[0][2] * tempMatrix.m[2][0];
	m->m[0][1] = rotMatrix.m[0][0] * tempMatrix.m[0][1] + rotMatrix.m[0][1] * tempMatrix.m[1][1] + rotMatrix.m[0][2] * tempMatrix.m[2][1];
	m->m[0][2] = rotMatrix.m[0][0] * tempMatrix.m[0][2] + rotMatrix.m[0][1] * tempMatrix.m[1][2] + rotMatrix.m[0][2] * tempMatrix.m[2][2];
	m->m[0][3] = rotMatrix.m[0][0] * tempMatrix.m[0][3] + rotMatrix.m[0][1] * tempMatrix.m[1][3] + rotMatrix.m[0][2] * tempMatrix.m[2][3];

	m->m[1][0] = rotMatrix.m[1][0] * tempMatrix.m[0][0] + rotMatrix.m[1][1] * tempMatrix.m[1][0] + rotMatrix.m[1][2] * tempMatrix.m[2][0];
	m->m[1][1] = rotMatrix.m[1][0] * tempMatrix.m[0][1] + rotMatrix.m[1][1] * tempMatrix.m[1][1] + rotMatrix.m[1][2] * tempMatrix.m[2][1];
	m->m[1][2] = rotMatrix.m[1][0] * tempMatrix.m[0][2] + rotMatrix.m[1][1] * tempMatrix.m[1][2] + rotMatrix.m[1][2] * tempMatrix.m[2][2];
	m->m[1][3] = rotMatrix.m[1][0] * tempMatrix.m[0][3] + rotMatrix.m[1][1] * tempMatrix.m[1][3] + rotMatrix.m[1][2] * tempMatrix.m[2][3];

	m->m[2][0] = rotMatrix.m[2][0] * tempMatrix.m[0][0] + rotMatrix.m[2][1] * tempMatrix.m[1][0] + rotMatrix.m[2][2] * tempMatrix.m[2][0];
	m->m[2][1] = rotMatrix.m[2][0] * tempMatrix.m[0][1] + rotMatrix.m[2][1] * tempMatrix.m[1][1] + rotMatrix.m[2][2] * tempMatrix.m[2][1];
	m->m[2][2] = rotMatrix.m[2][0] * tempMatrix.m[0][2] + rotMatrix.m[2][1] * tempMatrix.m[1][2] + rotMatrix.m[2][2] * tempMatrix.m[2][2];
	m->m[2][3] = rotMatrix.m[2][0] * tempMatrix.m[0][3] + rotMatrix.m[2][1] * tempMatrix.m[1][3] + rotMatrix.m[2][2] * tempMatrix.m[2][3];
}

// Rotate a matrix by a quaternion!
mat4 mat4RotateQuatC(const mat4 m, const quat q){
	const float xx = q.x * q.x;
	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float xw = q.x * q.w;
	const float yy = q.y * q.y;
	const float yz = q.y * q.z;
	const float yw = q.y * q.w;
	const float zz = q.z * q.z;
	const float zw = q.z * q.w;

	// Convert the quaternion to a rotation matrix!
	const mat3 rotMatrix = {
		.m[0][0] = 1.f - 2.f * (yy + zz),
		.m[0][1] = 2.f * (xy + zw),
		.m[0][2] = 2.f * (xz - yw),

		.m[1][0] = 2.f * (xy - zw),
		.m[1][1] = 1.f - 2.f * (xx + zz),
		.m[1][2] = 2.f * (yz + xw),

		.m[2][0] = 2.f * (xz + yw),
		.m[2][1] = 2.f * (yz - xw),
		.m[2][2] = 1.f - 2.f * (xx + yy)
	};

	// Now rotate our matrix by it!
	const mat4 out = {
		.m[0][0] = rotMatrix.m[0][0] * m.m[0][0] + rotMatrix.m[0][1] * m.m[1][0] + rotMatrix.m[0][2] * m.m[2][0],
		.m[0][1] = rotMatrix.m[0][0] * m.m[0][1] + rotMatrix.m[0][1] * m.m[1][1] + rotMatrix.m[0][2] * m.m[2][1],
		.m[0][2] = rotMatrix.m[0][0] * m.m[0][2] + rotMatrix.m[0][1] * m.m[1][2] + rotMatrix.m[0][2] * m.m[2][2],
		.m[0][3] = rotMatrix.m[0][0] * m.m[0][3] + rotMatrix.m[0][1] * m.m[1][3] + rotMatrix.m[0][2] * m.m[2][3],

		.m[1][0] = rotMatrix.m[1][0] * m.m[0][0] + rotMatrix.m[1][1] * m.m[1][0] + rotMatrix.m[1][2] * m.m[2][0],
		.m[1][1] = rotMatrix.m[1][0] * m.m[0][1] + rotMatrix.m[1][1] * m.m[1][1] + rotMatrix.m[1][2] * m.m[2][1],
		.m[1][2] = rotMatrix.m[1][0] * m.m[0][2] + rotMatrix.m[1][1] * m.m[1][2] + rotMatrix.m[1][2] * m.m[2][2],
		.m[1][3] = rotMatrix.m[1][0] * m.m[0][3] + rotMatrix.m[1][1] * m.m[1][3] + rotMatrix.m[1][2] * m.m[2][3],

		.m[2][0] = rotMatrix.m[2][0] * m.m[0][0] + rotMatrix.m[2][1] * m.m[1][0] + rotMatrix.m[2][2] * m.m[2][0],
		.m[2][1] = rotMatrix.m[2][0] * m.m[0][1] + rotMatrix.m[2][1] * m.m[1][1] + rotMatrix.m[2][2] * m.m[2][1],
		.m[2][2] = rotMatrix.m[2][0] * m.m[0][2] + rotMatrix.m[2][1] * m.m[1][2] + rotMatrix.m[2][2] * m.m[2][2],
		.m[2][3] = rotMatrix.m[2][0] * m.m[0][3] + rotMatrix.m[2][1] * m.m[1][3] + rotMatrix.m[2][2] * m.m[2][3],

		.m[3][0] = m.m[3][0],
		.m[3][1] = m.m[3][1],
		.m[3][2] = m.m[3][2],
		.m[3][3] = m.m[3][3]
	};


	return(out);
}

// Rotate a matrix on the X-axis!
void mat4RotateX(mat4 *const restrict m, const float x){
	const float cx = cosf(x);
	const float sx = sinf(x);
	float temp[2];

	temp[0] = m->m[0][1];
	temp[1] = m->m[0][2];
	m->m[0][1] = cx * temp[0] - sx * temp[1];
	m->m[0][2] = sx * temp[0] + cx * temp[1];

	temp[0] = m->m[1][1];
	temp[1] = m->m[1][2];
	m->m[1][1] = cx * temp[0] - sx * temp[1];
	m->m[1][2] = sx * temp[0] + cx * temp[1];

	temp[0] = m->m[2][1];
	temp[1] = m->m[2][2];
	m->m[2][1] = cx * temp[0] - sx * temp[1];
	m->m[2][2] = sx * temp[0] + cx * temp[1];

	temp[0] = m->m[3][1];
	temp[1] = m->m[3][2];
	m->m[3][1] = cx * temp[0] - sx * temp[1];
	m->m[3][2] = sx * temp[0] + cx * temp[1];
}

// Rotate a matrix on the X-axis!
mat4 mat4RotateXC(const mat4 m, const float x){
	const float cx = cosf(x);
	const float sx = sinf(x);

	const mat4 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = cx * m.m[0][1] - sx * m.m[0][2],
		.m[0][2] = sx * m.m[0][1] + cx * m.m[0][2],
		.m[0][3] = m.m[0][3],

		.m[1][0] = m.m[1][0],
		.m[1][1] = cx * m.m[1][1] - sx * m.m[1][2],
		.m[1][2] = sx * m.m[1][1] + cx * m.m[1][2],
		.m[1][3] = m.m[1][3],

		.m[2][0] = m.m[2][0],
		.m[2][1] = cx * m.m[2][1] - sx * m.m[2][2],
		.m[2][2] = sx * m.m[2][1] + cx * m.m[2][2],
		.m[2][3] = m.m[2][3],

		.m[3][0] = m.m[3][0],
		.m[3][1] = cx * m.m[3][1] - sx * m.m[3][2],
		.m[3][2] = sx * m.m[3][1] + cx * m.m[3][2],
		.m[3][3] = m.m[3][3]
	};

	return(out);
}

// Rotate a matrix on the Y-axis!
void mat4RotateY(mat4 *const restrict m, const float y){
	const float cy = cosf(y);
	const float sy = sinf(y);
	float temp[2];

	temp[0] = m->m[0][0];
	temp[1] = m->m[0][2];
	m->m[0][0] = cy * temp[0] + sy * temp[1];
	m->m[0][2] = -sy * temp[0] + cy * temp[1];

	temp[0] = m->m[1][0];
	temp[1] = m->m[1][2];
	m->m[1][0] = cy * temp[0] + sy * temp[1];
	m->m[1][2] = -sy * temp[0] + cy * temp[1];

	temp[0] = m->m[2][0];
	temp[1] = m->m[2][2];
	m->m[2][0] = cy * temp[0] + sy * temp[1];
	m->m[2][2] = -sy * temp[0] + cy * temp[1];

	temp[0] = m->m[3][0];
	temp[1] = m->m[3][2];
	m->m[3][0] = cy * temp[0] + sy * temp[1];
	m->m[3][2] = -sy * temp[0] + cy * temp[1];
}

// Rotate a matrix on the Y-axis!
mat4 mat4RotateYC(const mat4 m, const float y){
	const float cy = cosf(y);
	const float sy = sinf(y);

	const mat4 out = {
		.m[0][0] = cy * m.m[0][0] + sy * m.m[0][2],
		.m[0][1] = m.m[0][1],
		.m[0][2] = -sy * m.m[0][0] + cy * m.m[0][2],
		.m[0][3] = m.m[0][3],

		.m[1][0] = cy * m.m[1][0] + sy * m.m[1][2],
		.m[1][1] = m.m[1][1],
		.m[1][2] = -sy * m.m[1][0] + cy * m.m[1][2],
		.m[1][3] = m.m[1][3],

		.m[2][0] = cy * m.m[2][0] + sy * m.m[2][2],
		.m[2][1] = m.m[2][1],
		.m[2][2] = -sy * m.m[2][0] + cy * m.m[2][2],
		.m[2][3] = m.m[2][3],

		.m[3][0] = cy * m.m[3][0] + sy * m.m[3][2],
		.m[3][1] = m.m[3][1],
		.m[3][2] = -sy * m.m[3][0] + cy * m.m[3][2],
		.m[3][3] = m.m[3][3]
	};

	return(out);
}

// Rotate a matrix on the Z-axis!
void mat4RotateZ(mat4 *const restrict m, const float z){
	const float cz = cosf(z);
	const float sz = sinf(z);
	float temp[2];

	temp[0] = m->m[0][0];
	temp[1] = m->m[0][1];
	m->m[0][0] = cz * temp[0] - sz * temp[1];
	m->m[0][1] = sz * temp[0] + cz * temp[1];

	temp[0] = m->m[1][0];
	temp[1] = m->m[1][1];
	m->m[1][0] = cz * temp[0] - sz * temp[1];
	m->m[1][1] = sz * temp[0] + cz * temp[1];

	temp[0] = m->m[2][0];
	temp[1] = m->m[2][1];
	m->m[2][0] = cz * temp[0] - sz * temp[1];
	m->m[2][1] = sz * temp[0] + cz * temp[1];

	temp[0] = m->m[3][0];
	temp[1] = m->m[3][1];
	m->m[3][0] = cz * temp[0] - sz * temp[1];
	m->m[3][1] = sz * temp[0] + cz * temp[1];
}

// Rotate a matrix on the Z-axis!
mat4 mat4RotateZC(const mat4 m, const float z){
	const float cz = cosf(z);
	const float sz = sinf(z);

	const mat4 out = {
		.m[0][0] = cz * m.m[0][0] - sz * m.m[0][1],
		.m[0][1] = sz * m.m[0][0] + cz * m.m[0][1],
		.m[0][2] = m.m[0][2],
		.m[0][3] = m.m[0][3],

		.m[1][0] = cz * m.m[1][0] - sz * m.m[1][1],
		.m[1][1] = sz * m.m[1][0] + cz * m.m[1][1],
		.m[1][2] = m.m[1][2],
		.m[1][3] = m.m[1][3],

		.m[2][0] = cz * m.m[2][0] - sz * m.m[2][1],
		.m[2][1] = sz * m.m[2][0] + cz * m.m[2][1],
		.m[2][2] = m.m[2][2],
		.m[2][3] = m.m[2][3],

		.m[3][0] = cz * m.m[3][0] - sz * m.m[3][1],
		.m[3][1] = sz * m.m[3][0] + cz * m.m[3][1],
		.m[3][2] = m.m[3][2],
		.m[3][3] = m.m[3][3]
	};

	return(out);
}


// Left-multiply a matrix by a scale matrix stored as three floats!
void mat4Scale(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] *= x;
	m->m[0][1] *= y;
	m->m[0][2] *= z;

	m->m[1][0] *= x;
	m->m[1][1] *= y;
	m->m[1][2] *= z;

	m->m[2][0] *= x;
	m->m[2][1] *= y;
	m->m[2][2] *= z;

	m->m[3][0] *= x;
	m->m[3][1] *= y;
	m->m[3][2] *= z;
}

// Left-multiply a matrix by a scale matrix stored as three floats!
mat4 mat4ScaleC(mat4 m, const float x, const float y, const float z){
	m.m[0][0] *= x;
	m.m[0][1] *= y;
	m.m[0][2] *= z;

	m.m[1][0] *= x;
	m.m[1][1] *= y;
	m.m[1][2] *= z;

	m.m[2][0] *= x;
	m.m[2][1] *= y;
	m.m[2][2] *= z;

	m.m[3][0] *= x;
	m.m[3][1] *= y;
	m.m[3][2] *= z;

	return(m);
}

// Left-multiply a matrix by a scale matrix stored as four floats!
void mat4Scale4(mat4 *const restrict m, const float x, const float y, const float z, const float w){
	m->m[0][0] *= x;
	m->m[0][1] *= y;
	m->m[0][2] *= z;
	m->m[0][3] *= w;

	m->m[1][0] *= x;
	m->m[1][1] *= y;
	m->m[1][2] *= z;
	m->m[1][3] *= w;

	m->m[2][0] *= x;
	m->m[2][1] *= y;
	m->m[2][2] *= z;
	m->m[2][3] *= w;

	m->m[3][0] *= x;
	m->m[3][1] *= y;
	m->m[3][2] *= z;
	m->m[3][3] *= w;
}

// Left-multiply a matrix by a scale matrix stored as four floats!
mat4 mat4Scale4C(mat4 m, const float x, const float y, const float z, const float w){
	m.m[0][0] *= x;
	m.m[0][1] *= y;
	m.m[0][2] *= z;
	m.m[0][3] *= w;

	m.m[1][0] *= x;
	m.m[1][1] *= y;
	m.m[1][2] *= z;
	m.m[1][3] *= w;

	m.m[2][0] *= x;
	m.m[2][1] *= y;
	m.m[2][2] *= z;
	m.m[2][3] *= w;

	m.m[3][0] *= x;
	m.m[3][1] *= y;
	m.m[3][2] *= z;
	m.m[3][3] *= w;

	return(m);
}

// Left-multiply a matrix by a scale matrix stored as a vec3!
void mat4ScaleVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4Scale(m, v->x, v->y, v->z);
}

// Left-multiply a matrix by a scale matrix stored as a vec3!
mat4 mat4ScaleVec3C(mat4 m, const vec3 v){
	return(mat4ScaleC(m, v.x, v.y, v.z));
}

// Left-multiply a matrix by a scale matrix stored as a vec4!
void mat4ScaleVec4(mat4 *const restrict m, const vec4 *const restrict v){
	mat4Scale4(m, v->x, v->y, v->z, v->w);
}

// Left-multiply a matrix by a scale matrix stored as a vec4!
mat4 mat4ScaleVec4C(mat4 m, const vec4 v){
	return(mat4Scale4C(m, v.x, v.y, v.z, v.w));
}

// Right-multiply a matrix by a scale matrix stored as three floats!
void mat4ScalePre(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] *= x;
	m->m[0][1] *= x;
	m->m[0][2] *= x;
	m->m[0][3] *= x;

	m->m[1][0] *= y;
	m->m[1][1] *= y;
	m->m[1][2] *= y;
	m->m[1][3] *= y;

	m->m[2][0] *= z;
	m->m[2][1] *= z;
	m->m[2][2] *= z;
	m->m[2][3] *= z;
}

// Right-multiply a matrix by a scale matrix stored as three floats!
mat4 mat4ScalePreC(mat4 m, const float x, const float y, const float z){
	m.m[0][0] *= x;
	m.m[0][1] *= x;
	m.m[0][2] *= x;
	m.m[0][3] *= x;

	m.m[1][0] *= y;
	m.m[1][1] *= y;
	m.m[1][2] *= y;
	m.m[1][3] *= y;

	m.m[2][0] *= z;
	m.m[2][1] *= z;
	m.m[2][2] *= z;
	m.m[2][3] *= z;

	return(m);
}

// Right-multiply a matrix by a scale matrix stored as four floats!
void mat4ScalePre4(mat4 *const restrict m, const float x, const float y, const float z, const float w){
	m->m[0][0] *= x;
	m->m[0][1] *= x;
	m->m[0][2] *= x;
	m->m[0][3] *= x;

	m->m[1][0] *= y;
	m->m[1][1] *= y;
	m->m[1][2] *= y;
	m->m[1][3] *= y;

	m->m[2][0] *= z;
	m->m[2][1] *= z;
	m->m[2][2] *= z;
	m->m[2][3] *= z;

	m->m[3][0] *= w;
	m->m[3][1] *= w;
	m->m[3][2] *= w;
	m->m[3][3] *= w;
}

// Right-multiply a matrix by a scale matrix stored as four floats!
mat4 mat4ScalePre4C(mat4 m, const float x, const float y, const float z, const float w){
	m.m[0][0] *= x;
	m.m[0][1] *= x;
	m.m[0][2] *= x;
	m.m[0][3] *= x;

	m.m[1][0] *= y;
	m.m[1][1] *= y;
	m.m[1][2] *= y;
	m.m[1][3] *= y;

	m.m[2][0] *= z;
	m.m[2][1] *= z;
	m.m[2][2] *= z;
	m.m[2][3] *= z;

	m.m[3][0] *= w;
	m.m[3][1] *= w;
	m.m[3][2] *= w;
	m.m[3][3] *= w;

	return(m);
}

// Right-multiply a matrix by a scale matrix stored as a vec3!
void mat4ScalePreVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4ScalePre(m, v->x, v->y, v->z);
}

// Right-multiply a matrix by a scale matrix stored as a vec3!
mat4 mat4ScalePreVec3C(mat4 m, const vec3 v){
	return(mat4ScalePreC(m, v.x, v.y, v.z));
}

// Right-multiply a matrix by a scale matrix stored as a vec4!
void mat4ScalePreVec4(mat4 *const restrict m, const vec4 *const restrict v){
	mat4ScalePre4(m, v->x, v->y, v->z, v->w);
}

// Right-multiply a matrix by a scale matrix stored as a vec4!
mat4 mat4ScalePreVec4C(mat4 m, const vec4 v){
	return(mat4ScalePre4C(m, v.x, v.y, v.z, v.w));
}


/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat4Transpose(mat4 *const restrict m){
	const mat4 tempMatrix = *m;

	m->m[0][0] = tempMatrix.m[0][0];
	m->m[0][1] = tempMatrix.m[1][0];
	m->m[0][2] = tempMatrix.m[2][0];
	m->m[0][3] = tempMatrix.m[3][0];

	m->m[1][0] = tempMatrix.m[0][1];
	m->m[1][1] = tempMatrix.m[1][1];
	m->m[1][2] = tempMatrix.m[2][1];
	m->m[1][3] = tempMatrix.m[3][1];

	m->m[2][0] = tempMatrix.m[0][2];
	m->m[2][1] = tempMatrix.m[1][2];
	m->m[2][2] = tempMatrix.m[2][2];
	m->m[2][3] = tempMatrix.m[3][2];

	m->m[3][0] = tempMatrix.m[0][3];
	m->m[3][1] = tempMatrix.m[1][3];
	m->m[3][2] = tempMatrix.m[2][3];
	m->m[3][3] = tempMatrix.m[3][3];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat4TransposeOut(const mat4 m, mat4 *const restrict out){
	out->m[0][0] = m.m[0][0];
	out->m[0][1] = m.m[1][0];
	out->m[0][2] = m.m[2][0];
	out->m[0][3] = m.m[3][0];

	out->m[1][0] = m.m[0][1];
	out->m[1][1] = m.m[1][1];
	out->m[1][2] = m.m[2][1];
	out->m[1][3] = m.m[3][1];

	out->m[2][0] = m.m[0][2];
	out->m[2][1] = m.m[1][2];
	out->m[2][2] = m.m[2][2];
	out->m[2][3] = m.m[3][2];

	out->m[3][0] = m.m[0][3];
	out->m[3][1] = m.m[1][3];
	out->m[3][2] = m.m[2][3];
	out->m[3][3] = m.m[3][3];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
mat4 mat4TransposeC(const mat4 m){
	const mat4 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = m.m[1][0],
		.m[0][2] = m.m[2][0],
		.m[0][3] = m.m[3][0],

		.m[1][0] = m.m[0][1],
		.m[1][1] = m.m[1][1],
		.m[1][2] = m.m[2][1],
		.m[1][3] = m.m[3][1],

		.m[2][0] = m.m[0][2],
		.m[2][1] = m.m[1][2],
		.m[2][2] = m.m[2][2],
		.m[2][3] = m.m[3][2],

		.m[3][0] = m.m[0][3],
		.m[3][1] = m.m[1][3],
		.m[3][2] = m.m[2][3],
		.m[3][3] = m.m[3][3]
	};

	return(out);
}

// Invert a matrix!
void mat4Invert(mat4 *const restrict m){
	const mat4 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	const float f0 =  tempMatrix.m[1][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[1][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[2][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[2][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[3][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[3][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f1 = -tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f2 =  tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] -
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] +
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1];
	const float f3 = -tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] + tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] - tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] + tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * f0 + tempMatrix.m[0][1] * f1 + tempMatrix.m[0][2] * f2 + tempMatrix.m[0][3] * f3;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		m->m[0][0] =  f0 * invDet;
		m->m[0][1] = -tempMatrix.m[0][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[0][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[0][2] =  tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[0][3] = -tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][0] =  f1 * invDet;
		m->m[1][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[1][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[2][0] =  f2 * invDet;
		m->m[2][1] = -tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1];
		m->m[2][2] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[2][3] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[3][0] =  f3 * invDet;
		m->m[3][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] - tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] + tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] - tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1];
		m->m[3][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] + tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] - tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] + tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];
		m->m[3][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] + tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];
	}
}

// Invert a matrix and store the result in "out"!
void mat4InvertOut(const mat4 m, mat4 *const restrict out){
	// We need to use these values twice, but we only need to calculate them once.
	const float f0 =  m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                  m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
	                  m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
	const float f1 = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                  m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
	                  m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];
	const float f2 =  m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                  m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
	                  m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];
	const float f3 = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                  m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
	                  m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2 + m.m[0][3] * f3;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		out->m[0][0] =  f0 * invDet;
		out->m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
		                m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
		                m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
		out->m[0][2] =  m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		                m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
		                m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
		out->m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
		                m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
		                m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
		out->m[1][0] =  f1 * invDet;
		out->m[1][1] =  m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		                m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
		                m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];
		out->m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
		                m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
		                m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];
		out->m[1][3] =  m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		                m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
		                m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];
		out->m[2][0] =  f2 * invDet;
		out->m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
		                m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];
		out->m[2][2] =  m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		                m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];
		out->m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
		                m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
		                m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];
		out->m[3][0] =  f3 * invDet;
		out->m[3][1] =  m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		                m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];
		out->m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
		                m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];
		out->m[3][3] =  m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		                m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
		                m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];
	}
}

// Invert a matrix!
mat4 mat4InvertC(const mat4 m){
	// We need to use these values twice, but we only need to calculate them once.
	const float f0 =  m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                  m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
	                  m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
	const float f1 = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                  m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
	                  m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];
	const float f2 =  m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                  m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
	                  m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];
	const float f3 = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                  m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
	                  m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2 + m.m[0][3] * f3;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		mat4 out;

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		out.m[0][0] =  f0 * invDet;
		out.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
		               m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
		               m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
		out.m[0][2] =  m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		               m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
		               m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
		out.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
		               m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
		               m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
		out.m[1][0] =  f1 * invDet;
		out.m[1][1] =  m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		               m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
		               m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];
		out.m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
		               m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
		               m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];
		out.m[1][3] =  m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		               m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
		               m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];
		out.m[2][0] =  f2 * invDet;
		out.m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
		               m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
		               m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];
		out.m[2][2] =  m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		               m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
		               m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];
		out.m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
		               m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
		               m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];
		out.m[3][0] =  f3 * invDet;
		out.m[3][1] =  m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		               m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
		               m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];
		out.m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
		               m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
		               m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];
		out.m[3][3] =  m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		               m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
		               m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];

		return(out);
	}

	// If we couldn't invert the matrix, return the original one!
	return(m);
}

// Invert a matrix and return whether or not we were successful!
return_t mat4CanInvert(mat4 *const restrict m){
	const mat4 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	const float f0 =  tempMatrix.m[1][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[1][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[2][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[2][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[3][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[3][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f1 = -tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f2 =  tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] -
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] +
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1];
	const float f3 = -tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] + tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] - tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] + tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * f0 + tempMatrix.m[0][1] * f1 + tempMatrix.m[0][2] * f2 + tempMatrix.m[0][3] * f3;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		m->m[0][0] =  f0 * invDet;
		m->m[0][1] = -tempMatrix.m[0][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[0][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[0][2] =  tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[0][3] = -tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][0] =  f1 * invDet;
		m->m[1][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[1][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[2][0] =  f2 * invDet;
		m->m[2][1] = -tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1];
		m->m[2][2] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[2][3] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[3][0] =  f3 * invDet;
		m->m[3][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] - tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] + tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] - tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1];
		m->m[3][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] + tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] - tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] + tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];
		m->m[3][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] + tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];


		return(1);
	}


	return(0);
}

/*
** Invert a matrix, storing the result in "out"
** and returning whether or not we were successful!
*/
return_t mat4CanInvertOut(const mat4 m, mat4 *const restrict out){
	// We need to use these values twice, but we only need to calculate them once.
	const float f0 =  m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                  m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
	                  m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
	const float f1 = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                  m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
	                  m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];
	const float f2 =  m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                  m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
	                  m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];
	const float f3 = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                  m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
	                  m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2 + m.m[0][3] * f3;

	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] =  f0 * invDet;
		out->m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
		                m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
		                m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
		out->m[0][2] =  m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		                m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
		                m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
		out->m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
		                m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
		                m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
		out->m[1][0] =  f1 * invDet;
		out->m[1][1] =  m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		                m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
		                m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];
		out->m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
		                m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
		                m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];
		out->m[1][3] =  m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		                m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
		                m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];
		out->m[2][0] =  f2 * invDet;
		out->m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
		                m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];
		out->m[2][2] =  m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		                m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];
		out->m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
		                m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
		                m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];
		out->m[3][0] =  f3 * invDet;
		out->m[3][1] =  m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		                m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];
		out->m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
		                m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];
		out->m[3][3] =  m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		                m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
		                m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];


		return(1);
	}


	return(0);
}


// Generate an orthographic matrix!
void mat4Orthographic(mat4 *const restrict m, const float right, const float left, const float top, const float bottom, const float nearVal, const float farVal){
	const float widthScale  = 1.f/(right - left);
	const float heightScale = 1.f/(top - bottom);
	const float depthScale  = 1.f/(nearVal - farVal);

	m->m[0][0] = 2.f*widthScale;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 2.f*heightScale;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = 2.f*depthScale;
	m->m[2][3] = 0.f;

	m->m[3][0] = -((right + left) * widthScale);
	m->m[3][1] = -((top + bottom) * heightScale);
	m->m[3][2] = ((nearVal + farVal) * depthScale);
	m->m[3][3] = 1.f;
}

// Generate an orthographic matrix!
mat4 mat4OrthographicC(const float right, const float left, const float top, const float bottom, const float nearVal, const float farVal){
	const float widthScale  = 1.f/(right - left);
	const float heightScale = 1.f/(top - bottom);
	const float depthScale  = 1.f/(nearVal - farVal);
	const mat4 m = {
		.m[0][0] =                 2.f*widthScale, .m[0][1] =                             0.f, .m[0][2] =                         0.f, .m[0][3] = 0.f,
		.m[1][0] =                            0.f, .m[1][1] =                 2.f*heightScale, .m[1][2] =                         0.f, .m[1][3] = 0.f,
		.m[2][0] =                            0.f, .m[2][1] =                             0.f, .m[2][2] =              2.f*depthScale, .m[2][3] = 0.f,
		.m[3][0] = -((right + left) * widthScale), .m[3][1] = -((top + bottom) * heightScale), .m[3][2] = ((nearVal + farVal) * depthScale), .m[3][3] = 1.f
	};

	return(m);
}

// Generate a perspective matrix!
void mat4Perspective(mat4 *const restrict m, const float fov, const float aspectRatio, const float nearVal, const float farVal){
	const float invScale = 1.f/(aspectRatio * tan(fov * 0.5f));
	const float depthScale = 1.f/(nearVal - farVal);

	m->m[0][0] = invScale;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = aspectRatio * invScale;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = (farVal + nearVal) * depthScale;
	m->m[2][3] = -1.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 2.f * farVal * nearVal * depthScale;
	m->m[3][3] = 0.f;
}

// Generate a perspective matrix!
mat4 mat4PerspectiveC(const float fov, const float aspectRatio, const float nearVal, const float farVal){
	const float invScale = 1.f/(aspectRatio * tan(fov * 0.5f));
	const float depthScale = 1.f/(nearVal - farVal);
	const mat4 m = {
		.m[0][0] = invScale, .m[0][1] = 0.f,                    .m[0][2] = 0.f,                                 .m[0][3] =  0.f,
		.m[1][0] = 0.f,      .m[1][1] = aspectRatio * invScale, .m[1][2] = 0.f,                                 .m[1][3] =  0.f,
		.m[2][0] = 0.f,      .m[2][1] = 0.f,                    .m[2][2] = (farVal + nearVal) * depthScale,     .m[2][3] = -1.f,
		.m[3][0] = 0.f,      .m[3][1] = 0.f,                    .m[3][2] = 2.f * farVal * nearVal * depthScale, .m[3][3] =  0.f
	};

	return(m);
}

// Generate a perspective matrix using the plain, unoptimized method!
void mat4PerspectiveOld(mat4 *const restrict m, const float fov, const float aspectRatio, const float nearVal, const float farVal){
	const float top    = nearVal * tanf(fov * 0.5f);
	const float right  = top * aspectRatio;
	const float widthScale = 1.f/right;
	const float heightScale = 1.f/top;
	const float depthScale = 1.f/(nearVal - farVal);

	m->m[0][0] = nearVal * widthScale;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = nearVal * heightScale;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = (farVal + nearVal) * depthScale;
	m->m[2][3] = -1.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 2.f * farVal * nearVal * depthScale;
	m->m[3][3] = 0.f;
}

// Generate a perspective matrix using the plain, unoptimized method!
mat4 mat4PerspectiveOldC(const float fov, const float aspectRatio, const float nearVal, const float farVal){
	const float top    = nearVal * tanf(fov * 0.5f);
	const float right  = top * aspectRatio;
	const float widthScale = 1.f/right;
	const float heightScale = 1.f/top;
	const float depthScale = 1.f/(nearVal - farVal);
	const mat4 m = {
		.m[0][0] = nearVal * widthScale, .m[0][1] = 0.f,                   .m[0][2] = 0.f,                                 .m[0][3] =  0.f,
		.m[1][0] = 0.f,                  .m[1][1] = nearVal * heightScale, .m[1][2] = 0.f,                                 .m[1][3] =  0.f,
		.m[2][0] = 0.f,                  .m[2][1] = 0.f,                   .m[2][2] = (farVal + nearVal) * depthScale,     .m[2][3] = -1.f,
		.m[3][0] = 0.f,                  .m[3][1] = 0.f,                   .m[3][2] = 2.f * farVal * nearVal * depthScale, .m[3][3] =  0.f
	};

	return(m);
}

// Generate a rotation matrix that faces a target!
void mat4RotateToFace(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict worldUp){
	vec3 right, up, forward;
	// Get the forward vector!
	vec3SubtractVec3FromOut(target, eye, &forward);
	vec3NormalizeVec3Fast(&forward);
	// Get the right vector!
	vec3CrossVec3Out(worldUp, &forward, &right);
	vec3NormalizeVec3Fast(&right);
	// Get the up vector!
	vec3CrossVec3Out(&forward, &right, &up);
	vec3NormalizeVec3Fast(&up);

	// Rotate the matrix to look at "target"!
	m->m[0][0] = right.x;
	m->m[0][1] = right.y;
	m->m[0][2] = right.z;
	m->m[0][3] = 0.f;

	m->m[1][0] = up.x;
	m->m[1][1] = up.y;
	m->m[1][2] = up.z;
	m->m[1][3] = 0.f;

	m->m[2][0] = forward.x;
	m->m[2][1] = forward.y;
	m->m[2][2] = forward.z;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Generate a rotation matrix that faces a target!
mat4 mat4RotateToFaceC(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3FromC(target, eye));
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));
	// Rotate the matrix to look at "target"!
	const mat4 m = {
		.m[0][0] = right.x,   .m[0][1] = right.y,   .m[0][2] = right.z,   .m[0][3] = 0.f,
		.m[1][0] = up.x,      .m[1][1] = up.y,      .m[1][2] = up.z,      .m[1][3] = 0.f,
		.m[2][0] = forward.x, .m[2][1] = forward.y, .m[2][2] = forward.z, .m[2][3] = 0.f,
		.m[3][0] = 0.f,       .m[3][1] = 0.f,       .m[3][2] = 0.f,       .m[3][3] = 1.f
	};

	return(m);
}

// Generate a look-at matrix!
void mat4LookAt(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict worldUp){
	vec3 right, up, forward;
	// Get the forward vector!
	vec3SubtractVec3FromOut(eye, target, &forward);
	vec3NormalizeVec3Fast(&forward);
	// Get the right vector!
	vec3CrossVec3Out(worldUp, &forward, &right);
	vec3NormalizeVec3Fast(&right);
	// Get the up vector!
	vec3CrossVec3Out(&forward, &right, &up);
	vec3NormalizeVec3Fast(&up);

	// Translate the matrix to "eye" and make it look at "target"!
	m->m[0][0] = right.x;
	m->m[0][1] = up.x;
	m->m[0][2] = forward.x;
	m->m[0][3] = 0.f;

	m->m[1][0] = right.y;
	m->m[1][1] = up.y;
	m->m[1][2] = forward.y;
	m->m[1][3] = 0.f;

	m->m[2][0] = right.z;
	m->m[2][1] = up.z;
	m->m[2][2] = forward.z;
	m->m[2][3] = 0.f;

	m->m[3][0] = -vec3DotVec3(&right, eye);
	m->m[3][1] = -vec3DotVec3(&up, eye);
	m->m[3][2] = -vec3DotVec3(&forward, eye);
	m->m[3][3] = 1.f;
}

// Generate a look-at matrix!
mat4 mat4LookAtC(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3FromC(eye, target));
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));
	// Translate the matrix to "eye" and make it look at "target"!
	const mat4 m = {
		.m[0][0] = right.x,                   .m[0][1] = up.x,                   .m[0][2] = forward.x,                   .m[0][3] = 0.f,
		.m[1][0] = right.y,                   .m[1][1] = up.y,                   .m[1][2] = forward.y,                   .m[1][3] = 0.f,
		.m[2][0] = right.z,                   .m[2][1] = up.z,                   .m[2][2] = forward.z,                   .m[2][3] = 0.f,
		.m[3][0] = -vec3DotVec3C(right, eye), .m[3][1] = -vec3DotVec3C(up, eye), .m[3][2] = -vec3DotVec3C(forward, eye), .m[3][3] = 1.f
	};

	return(m);
}


/*
** Convert a 4x4 matrix to a quaternion and store the result in "out"!
** For this to work, we assume that "m" is a special orthogonal matrix.
** Implementation derived by Mike Day in Converting a Rotation Matrix to a Quaternion.
*/
void mat4ToQuat(const mat4 *const restrict m, quat *const restrict out){
	float t;
	if(m->m[2][2] < 0.f){
		if(m->m[0][0] > m->m[1][1]){
			t = m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f;
			quatInitSet(out, t, m->m[0][1] + m->m[1][0], m->m[2][0] + m->m[0][2], m->m[1][2] - m->m[2][1]);
		}else{
			t = -m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f;
			quatInitSet(out, m->m[0][1] + m->m[1][0], t, m->m[1][2] + m->m[2][1], m->m[2][0] - m->m[0][2]);
		}
	}else{
		if(m->m[0][0] < -m->m[1][1]){
			t = -m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f;
			quatInitSet(out, m->m[2][0] + m->m[0][2], m->m[1][2] + m->m[2][1], t, m->m[0][1] - m->m[1][0]);
		}else{
			t = m->m[0][0] + m->m[1][1] + m->m[2][2] + 1.f;
			quatInitSet(out, m->m[1][2] - m->m[2][1], m->m[2][0] - m->m[0][2], m->m[0][1] - m->m[1][0], t);
		}
	}
	quatMultiplyS(out, 0.5f*invSqrtFast(t));
}

/*
** Convert a 4x4 matrix to a quaternion!
** Implementation derived by Mike Day in Converting a Rotation Matrix to a Quaternion.
*/
quat mat4ToQuatC(const mat4 m){
	float t;
	quat q;
	if(m.m[2][2] < 0.f){
		if(m.m[0][0] > m.m[1][1]){
			t = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f;
			q = quatInitSetC(t, m.m[0][1] + m.m[1][0], m.m[2][0] + m.m[0][2], m.m[1][2] - m.m[2][1]);
		}else{
			t = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f;
			q = quatInitSetC(m.m[0][1] + m.m[1][0], t, m.m[1][2] + m.m[2][1], m.m[2][0] - m.m[0][2]);
		}
	}else{
		if(m.m[0][0] < -m.m[1][1]){
			t = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f;
			q = quatInitSetC(m.m[2][0] + m.m[0][2], m.m[1][2] + m.m[2][1], t, m.m[0][1] - m.m[1][0]);
		}else{
			t = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f;
			q = quatInitSetC(m.m[1][2] - m.m[2][1], m.m[2][0] - m.m[0][2], m.m[0][1] - m.m[1][0], t);
		}
	}
	return(quatMultiplySC(q, 0.5f*invSqrtFast(t)));
}

/*
** An alternative implementation that forgoes
** branching at the cost of more square roots.
*/
void mat4ToQuatAlt(const mat4 *const restrict m, quat *const restrict out){
	out->x = copySignZero(0.5f * sqrtf( m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f), m->m[1][2] - m->m[2][1]);
	out->y = copySignZero(0.5f * sqrtf(-m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f), m->m[2][0] - m->m[0][2]);
	out->z = copySignZero(0.5f * sqrtf(-m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f), m->m[0][1] - m->m[1][0]);
	out->w = 0.5f * sqrtf(m->m[0][0] + m->m[1][1] + m->m[2][2] + 1.f);
}

quat mat4ToQuatAltC(const mat4 m){
	const quat q = {
		.x = copySignZero(0.5f * sqrtf( m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f), m.m[1][2] - m.m[2][1]),
		.y = copySignZero(0.5f * sqrtf(-m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f), m.m[2][0] - m.m[0][2]),
		.z = copySignZero(0.5f * sqrtf(-m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f), m.m[0][1] - m.m[1][0]),
		.w = 0.5f * sqrtf(m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f)
	};
	return(q);
}

// Convert a quaternion to a 4x4 matrix and store the result in "out"!
void quatToMat4(const quat *const restrict q, mat4 *const restrict out){
	const float xx = q->x*q->x;
	const float yy = q->y*q->y;
	const float zz = q->z*q->z;
	float temp1;
	float temp2;

	out->m[0][0] = 1 - 2*(yy + zz);
	out->m[1][1] = 1 - 2*(xx - zz);
	out->m[2][2] = 1 - 2*(xx - yy);
	out->m[3][3] = 1.f;

	temp1 = q->x*q->y;
	temp2 = q->w*q->z;
	out->m[0][1] = 2*(temp1 - temp2);
	out->m[1][0] = 2*(temp1 + temp2);

	temp1 = q->x*q->z;
	temp2 = q->w*q->y;
	out->m[0][2] = 2*(temp1 + temp2);
	out->m[2][0] = 2*(temp1 - temp2);

	temp1 = q->y*q->z;
	temp2 = q->w*q->w;
	out->m[1][2] = 2*(temp1 - temp2);
	out->m[2][1] = 2*(temp1 + temp2);

	out->m[0][3] =
	out->m[1][3] =
	out->m[2][3] =
	out->m[3][0] =
	out->m[3][1] =
	out->m[3][2] = 0.f;
}

// Convert a quaternion to a 4x4 matrix!
mat4 quatToMat4C(const quat q){
	const float xx = q.x*q.x;
	const float yy = q.y*q.y;
	const float zz = q.z*q.z;
	float temp1;
	float temp2;
	mat4 out;

	out.m[0][0] = 1 - 2*(yy + zz);
	out.m[1][1] = 1 - 2*(xx - zz);
	out.m[2][2] = 1 - 2*(xx - yy);
	out.m[3][3] = 1.f;

	temp1 = q.x*q.y;
	temp2 = q.w*q.z;
	out.m[0][1] = 2*(temp1 - temp2);
	out.m[1][0] = 2*(temp1 + temp2);

	temp1 = q.x*q.z;
	temp2 = q.w*q.y;
	out.m[0][2] = 2*(temp1 + temp2);
	out.m[2][0] = 2*(temp1 - temp2);

	temp1 = q.y*q.z;
	temp2 = q.w*q.x;
	out.m[1][2] = 2*(temp1 - temp2);
	out.m[2][1] = 2*(temp1 + temp2);

	out.m[0][3] =
	out.m[1][3] =
	out.m[2][3] =
	out.m[3][0] =
	out.m[3][1] =
	out.m[3][2] = 0.f;

	return(out);
}