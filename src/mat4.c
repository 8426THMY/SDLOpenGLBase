#include "mat4.h"


#include <string.h>
#include <math.h>

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

// Initialise a matrix to a translation matrix using a vec4!
void mat4InitTranslateVec4(mat4 *const restrict m, const vec4 *const restrict v){
	mat4InitTranslate4(m, v->x, v->y, v->z, v->w);
}

// Initialise a matrix to a translation matrix using a vec4!
mat4 mat4InitTranslateVec4C(const vec4 v){
	return(mat4InitTranslate4C(v.x, v.y, v.z, v.w));
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

// Initialize a matrix from ZXY Euler angles (in radians)!
void mat4InitEulerZXY(mat4 *const restrict m, const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	m->m[0][0] = sxsy*sz + cy*cz;
	m->m[0][1] = cx*sz;
	m->m[0][2] = sxcy*sz - sy*cz;
	m->m[0][3] = 0.f;

	m->m[1][0] = sxsy*cz - cy*sz;
	m->m[1][1] = cx*cz;
	m->m[1][2] = sxcy*cz + sy*sz;
	m->m[1][3] = 0.f;

	m->m[2][0] = cx*sy;
	m->m[2][1] = -sx;
	m->m[2][2] = cx*cy;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Initialize a matrix from ZXY Euler angles (in radians)!
mat4 mat4InitEulerZXYC(const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	const mat4 m = {
		.m[0][0] = sxsy*sz + cy*cz,
		.m[0][1] = cx*sz,
		.m[0][2] = sxcy*sz - sy*cz,
		.m[0][3] = 0.f,

		.m[1][0] = sxsy*cz - cy*sz,
		.m[1][1] = cx*cz,
		.m[1][2] = sxcy*cz + sy*sz,
		.m[1][3] = 0.f,

		.m[2][0] = cx*sy,
		.m[2][1] = -sx,
		.m[2][2] = cx*cy,
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = 1.f
	};


	return(m);
}

// Initialize a matrix from XYZ Euler angles (in radians)!
void mat4InitEulerVec3XYZ(mat4 *const restrict m, const vec3 *const restrict v){
	const float cx = cosf(v->x);
	const float sx = sinf(v->x);
	const float cy = cosf(v->y);
	const float sy = sinf(v->y);
	const float cz = cosf(v->z);
	const float sz = sinf(v->z);
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
mat4 mat4InitEulerVec3XYZC(const vec3 v){
	const float cx = cosf(v.x);
	const float sx = sinf(v.x);
	const float cy = cosf(v.y);
	const float sy = sinf(v.y);
	const float cz = cosf(v.z);
	const float sz = sinf(v.z);
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

// Initialize a matrix from ZXY Euler angles (in radians)!
void mat4InitEulerVec3ZXY(mat4 *const restrict m, const vec3 *const restrict v){
	const float cx = cosf(v->x);
	const float sx = sinf(v->x);
	const float cy = cosf(v->y);
	const float sy = sinf(v->y);
	const float cz = cosf(v->z);
	const float sz = sinf(v->z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	m->m[0][0] = sxsy*sz + cy*cz;
	m->m[0][1] = cx*sz;
	m->m[0][2] = sxcy*sz - sy*cz;
	m->m[0][3] = 0.f;

	m->m[1][0] = sxsy*cz - cy*sz;
	m->m[1][1] = cx*cz;
	m->m[1][2] = sxcy*cz + sy*sz;
	m->m[1][3] = 0.f;

	m->m[2][0] = cx*sy;
	m->m[2][1] = -sx;
	m->m[2][2] = cx*cy;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Initialize a matrix from ZXY Euler angles (in radians)!
mat4 mat4InitEulerVec3ZXYC(const vec3 v){
	const float cx = cosf(v.x);
	const float sx = sinf(v.x);
	const float cy = cosf(v.y);
	const float sy = sinf(v.y);
	const float cz = cosf(v.z);
	const float sz = sinf(v.z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	const mat4 m = {
		.m[0][0] = sxsy*sz + cy*cz,
		.m[0][1] = cx*sz,
		.m[0][2] = sxcy*sz - sy*cz,
		.m[0][3] = 0.f,

		.m[1][0] = sxsy*cz - cy*sz,
		.m[1][1] = cx*cz,
		.m[1][2] = sxcy*cz + sy*sz,
		.m[1][3] = 0.f,

		.m[2][0] = cx*sy,
		.m[2][1] = -sx,
		.m[2][2] = cx*cy,
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = 1.f
	};


	return(m);
}

/*
** Initialize a matrix from an axis and an angle!
** We assume that the axis is already normalized.
*/
void mat4InitAxisAngle(mat4 *const restrict m, const vec4 *const restrict v){
	const float c = cosf(v->w);
	const float s = sinf(v->w);
	const float t = 1.f - c;
	const vec3 normalAxis = {
		.x = v->x,
		.y = v->y,
		.z = v->z
	};
	vec3 tempAxis;
	vec3MultiplySOut(&normalAxis, t, &tempAxis);

	// Convert the axis angle to a rotation matrix!
	m->m[0][0] = tempAxis.x * normalAxis.x + c;
	m->m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s;
	m->m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s;
	m->m[0][3] = 0.f;

	m->m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s;
	m->m[1][1] = tempAxis.y * normalAxis.y + c;
	m->m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s;
	m->m[1][3] = 0.f;

	m->m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s;
	m->m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s;
	m->m[2][2] = tempAxis.z * normalAxis.z + c;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

/*
** Initialize a matrix from an axis and an angle!
** We assume that the axis is already normalized.
*/
mat4 mat4InitAxisAngleC(const vec4 v){
	const float c = cosf(v.w);
	const float s = sinf(v.w);
	const float t = 1.f - c;
	const vec3 normalAxis = {
		.x = v.x,
		.y = v.y,
		.z = v.z
	};
	const vec3 tempAxis = vec3MultiplySC(normalAxis, t);

	// Convert the axis angle to a rotation matrix!
	const mat4 out = {
		.m[0][0] = tempAxis.x * normalAxis.x + c,
		.m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s,
		.m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s,
		.m[0][3] = 0.f,

		.m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s,
		.m[1][1] = tempAxis.y * normalAxis.y + c,
		.m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s,
		.m[1][3] = 0.f,

		.m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s,
		.m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s,
		.m[2][2] = tempAxis.z * normalAxis.z + c,
		.m[2][3] = 0.f,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f,
		.m[3][3] = 1.f
	};

	return(out);
}

/*
** Initialise a matrix to a rotation matrix!
** This assumes that "q" is normalized.
*/
void mat4InitQuat(mat4 *const restrict m, const quat *const restrict q){
	const float xx = q->x * q->x;
	const float xy = q->x * q->y;
	const float xz = q->x * q->z;
	const float xw = q->x * q->w;
	const float yy = q->y * q->y;
	const float yz = q->y * q->z;
	const float yw = q->y * q->w;
	const float zz = q->z * q->z;
	const float zw = q->z * q->w;

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

/*
** Initialise a matrix to a rotation matrix!
** This assumes that "q" is normalized.
*/
mat4 mat4InitQuatC(const quat q){
	const float xx = q.x * q.x;
	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float xw = q.x * q.w;
	const float yy = q.y * q.y;
	const float yz = q.y * q.z;
	const float yw = q.y * q.w;
	const float zz = q.z * q.z;
	const float zw = q.z * q.w;

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

// Given a quaternion and a vector, construct the shear matrix QVQ^T.
void mat4InitShearQuat(
	mat4 *const restrict m,
	const vec3 *const restrict v, const quat *const restrict q
){

	// Convert the quaternion Q to a rotation matrix!
	mat3 rotMatrix;
	mat3InitQuat(&rotMatrix, q);

	float cx = v->x*rotMatrix.m[0][0];
	float cy = v->y*rotMatrix.m[1][0];
	float cz = v->z*rotMatrix.m[2][0];

	// Return m = QVQ^T, where Q is our rotation matrix and V is
	// the scale matrix with elements of v along its diagonal.
	m->m[0][0] = rotMatrix.m[0][0]*cx + rotMatrix.m[1][0]*cy + rotMatrix.m[2][0]*cz;
	m->m[0][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m->m[0][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	m->m[0][3] = 0.f;
	cx = v->x*rotMatrix.m[0][1];
	cy = v->y*rotMatrix.m[1][1];
	cz = v->z*rotMatrix.m[2][1];
	m->m[1][0] = m->m[0][1];
	m->m[1][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m->m[1][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	m->m[1][3] = 0.f;
	m->m[2][0] = m->m[0][2];
	m->m[2][1] = m->m[1][2];
	m->m[2][2] = rotMatrix.m[0][2]*v->x*rotMatrix.m[0][2] +
	             rotMatrix.m[1][2]*v->y*rotMatrix.m[1][2] +
	             rotMatrix.m[2][2]*v->z*rotMatrix.m[2][2];
	m->m[2][3] = 0.f;
	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

// Given a quaternion and a vector, construct the shear matrix QVQ^T.
mat4 mat4InitShearQuatC(const vec3 v, const quat q){
	// Convert the quaternion Q to a rotation matrix!
	const mat3 rotMatrix =  mat3InitQuatC(q);

	float cx = v.x*rotMatrix.m[0][0];
	float cy = v.y*rotMatrix.m[1][0];
	float cz = v.z*rotMatrix.m[2][0];
	mat4 m;

	// Return m = QVQ^T, where Q is our rotation matrix and V is
	// the scale matrix with elements of v along its diagonal.
	m.m[0][0] = rotMatrix.m[0][0]*cx + rotMatrix.m[1][0]*cy + rotMatrix.m[2][0]*cz;
	m.m[0][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m.m[0][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	m.m[0][3] = 0.f;
	cx = v.x*rotMatrix.m[0][1];
	cy = v.y*rotMatrix.m[1][1];
	cz = v.z*rotMatrix.m[2][1];
	m.m[1][0] = m.m[0][1];
	m.m[1][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m.m[1][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	m.m[1][3] = 0.f;
	m.m[2][0] = m.m[0][2];
	m.m[2][1] = m.m[1][2];
	m.m[2][2] = rotMatrix.m[0][2]*v.x*rotMatrix.m[0][2] +
	            rotMatrix.m[1][2]*v.y*rotMatrix.m[1][2] +
	            rotMatrix.m[2][2]*v.z*rotMatrix.m[2][2];
	m.m[2][3] = 0.f;
	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = 0.f;
	m.m[3][3] = 1.f;

	return(m);
}

/*
** Construct a matrix that reflects by the specified plane.
** We assume that the plane's normal component is unit length.
** This should be multipled by matrices on the left!
*/
void mat4InitReflect(mat4 *const restrict m, const vec4 *const restrict plane){
	const float x2 = 2.f*plane->x;
	const float y2 = 2.f*plane->y;
	const float z2 = 2.f*plane->z;

	m->m[0][0] = 1.f - x2*plane->x;
	m->m[0][1] =     - x2*plane->y;
	m->m[0][2] =     - x2*plane->z;
	m->m[0][3] = 0.f;

	m->m[1][0] = m->m[0][1];
	m->m[1][1] = 1.f - y2*plane->y;
	m->m[1][2] =     - y2*plane->z;
	m->m[1][3] = 0.f;

	m->m[2][0] = m->m[0][2];
	m->m[2][1] = m->m[1][2];
	m->m[2][2] = 1.f - z2*plane->z;
	m->m[2][3] = 0.f;

	m->m[3][0] =     - x2*plane->w;
	m->m[3][1] =     - y2*plane->w;
	m->m[3][2] =     - z2*plane->w;
	m->m[3][3] = 1.f;
}

/*
** Construct a matrix that reflects by the specified plane.
** We assume that the plane's normal component is unit length.
** This should be multipled by matrices on the left!
*/
mat4 mat4InitReflectC(const vec4 plane){
	const float x2 = 2.f*plane.x;
	const float y2 = 2.f*plane.y;
	const float z2 = 2.f*plane.z;
	mat4 m;

	m.m[0][0] = 1.f - x2*plane.x;
	m.m[0][1] =     - x2*plane.y;
	m.m[0][2] =     - x2*plane.z;
	m.m[0][3] = 0.f;

	m.m[1][0] = m.m[0][1];
	m.m[1][1] = 1.f - y2*plane.y;
	m.m[1][2] =     - y2*plane.z;
	m.m[1][3] = 0.f;

	m.m[2][0] = m.m[0][2];
	m.m[2][1] = m.m[1][2];
	m.m[2][2] = 1.f - z2*plane.z;
	m.m[2][3] = 0.f;

	m.m[3][0] =     - x2*plane.w;
	m.m[3][1] =     - y2*plane.w;
	m.m[3][2] =     - z2*plane.w;
	m.m[3][3] = 1.f;

	return(m);
}

// Cast a 4x4 matrix down to a 3x3 one.
void mat3InitMat4(mat3 *const restrict m1, const mat4 *const restrict m2){
	memcpy(m1->m[0], m2->m[0], sizeof(m1->m[0]));
	memcpy(m1->m[1], m2->m[1], sizeof(m1->m[1]));
	memcpy(m1->m[2], m2->m[2], sizeof(m1->m[2]));
}

// Cast a 4x4 matrix down to a 3x3 one.
mat3 mat3InitMat4C(const mat4 m){
	const mat3 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = m.m[0][1],
		.m[0][2] = m.m[0][2],

		.m[1][0] = m.m[1][0],
		.m[1][1] = m.m[1][1],
		.m[1][2] = m.m[1][2],

		.m[2][0] = m.m[2][0],
		.m[2][1] = m.m[2][1],
		.m[2][2] = m.m[2][2]
	};

	return(out);
}

// Cast a 4x4 matrix down to a 3x4 one.
void mat3x4InitMat4(mat3x4 *const restrict m1, const mat4 *const restrict m2){
	memcpy(m1->m[0], m2->m[0], sizeof(m1->m[0]));
	memcpy(m1->m[1], m2->m[1], sizeof(m1->m[1]));
	memcpy(m1->m[2], m2->m[2], sizeof(m1->m[2]));
	memcpy(m1->m[3], m2->m[3], sizeof(m1->m[3]));
}

// Cast a 4x4 matrix down to a 3x4 one.
mat3x4 mat3x4InitMat4C(const mat4 m){
	const mat3x4 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = m.m[0][1],
		.m[0][2] = m.m[0][2],

		.m[1][0] = m.m[1][0],
		.m[1][1] = m.m[1][1],
		.m[1][2] = m.m[1][2],

		.m[2][0] = m.m[2][0],
		.m[2][1] = m.m[2][1],
		.m[2][2] = m.m[2][2],

		.m[3][0] = m.m[3][0],
		.m[3][1] = m.m[3][1],
		.m[3][2] = m.m[3][2]
	};

	return(out);
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


// Multiply "m" by "x"!
void mat4MultiplyS(mat4 *const restrict m, const float x){
	m->m[0][0] *= x;
	m->m[0][1] *= x;
	m->m[0][2] *= x;
	m->m[0][3] *= x;
	m->m[1][0] *= x;
	m->m[1][1] *= x;
	m->m[1][2] *= x;
	m->m[1][3] *= x;
	m->m[2][0] *= x;
	m->m[2][1] *= x;
	m->m[2][2] *= x;
	m->m[2][3] *= x;
	m->m[3][0] *= x;
	m->m[3][1] *= x;
	m->m[3][2] *= x;
	m->m[3][3] *= x;
}

// Multiply "m" by "x" and store the result in "out"!
void mat4MultiplySOut(const mat4 *const restrict m, const float x, mat4 *const restrict out){
	out->m[0][0] = m->m[0][0] * x;
	out->m[0][1] = m->m[0][1] * x;
	out->m[0][2] = m->m[0][2] * x;
	out->m[0][3] = m->m[0][3] * x;
	out->m[1][0] = m->m[1][0] * x;
	out->m[1][1] = m->m[1][1] * x;
	out->m[1][2] = m->m[1][2] * x;
	out->m[1][3] = m->m[1][3] * x;
	out->m[2][0] = m->m[2][0] * x;
	out->m[2][1] = m->m[2][1] * x;
	out->m[2][2] = m->m[2][2] * x;
	out->m[2][3] = m->m[2][3] * x;
	out->m[3][0] = m->m[3][0] * x;
	out->m[3][1] = m->m[3][1] * x;
	out->m[3][2] = m->m[3][2] * x;
	out->m[3][3] = m->m[3][3] * x;
}

// Multiply "m" by "x"!
mat4 mat4MultiplySOutC(mat4 m, const float x){
	m.m[0][0] *= x;
	m.m[0][1] *= x;
	m.m[0][2] *= x;
	m.m[0][3] *= x;
	m.m[1][0] *= x;
	m.m[1][1] *= x;
	m.m[1][2] *= x;
	m.m[1][3] *= x;
	m.m[2][0] *= x;
	m.m[2][1] *= x;
	m.m[2][2] *= x;
	m.m[2][3] *= x;
	m.m[3][0] *= x;
	m.m[3][1] *= x;
	m.m[3][2] *= x;
	m.m[3][3] *= x;

	return(m);
}

/*
** Left-multiply a 3x1 column vector by a matrix (m*v)!
** Note that this is really a 4x1 vector with '1' in the last component.
*/
void mat4MultiplyVec3(const mat4 *const restrict m, vec3 *const restrict v){
	const vec3 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z + m->m[3][0];
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z + m->m[3][1];
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z + m->m[3][2];
}

/*
** Left-multiply a 3x1 column vector by a matrix (m*v) and store the result in "out"!
** Note that this is really a 4x1 vector with '1' in the last component.
** This assumes that "out" isn't "v".
*/
void mat4MultiplyVec3Out(const mat4 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0];
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1];
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2];
}

/*
** Left-multiply a 3x1 column vector by a matrix (m*v) and return the result!
** Note that this is really a 4x1 vector with '1' in the last component.
** This assumes that "out" isn't "v".
*/
vec3 mat4MultiplyVec3C(const mat4 m, const vec3 v){
	const vec3 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0],
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1],
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2]
	};

	return(out);
}

/*
** Right-multiply a 1x3 row vector by a matrix (v^T*m)!
** Note that this is really a 1x4 vector with '1' in the last component.
*/
void vec3MultiplyMat4(vec3 *const restrict v, const mat4 *const restrict m){
	const vec3 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1] + temp.z * m->m[0][2] + m->m[0][3];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1] + temp.z * m->m[1][2] + m->m[1][3];
	v->z = temp.x * m->m[2][0] + temp.y * m->m[2][1] + temp.z * m->m[2][2] + m->m[2][3];
}

/*
** Right-multiply a 1x3 row vector by a matrix (v^T*m) and store the result in "out"!
** Note that this is really a 1x4 vector with '1' in the last component.
** This assumes that "out" isn't "v".
*/
void vec3MultiplyMat4Out(const vec3 *const restrict v, const mat4 *const restrict m, vec3 *const restrict out){
	out->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + m->m[0][3];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + m->m[1][3];
	out->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + m->m[2][3];
}

/*
** Right-multiply a 1x3 row vector by a matrix (v^T*m) and return the result!
** Note that this is really a 1x4 vector with '1' in the last component.
*/
vec3 vec3MultiplyMat4C(const vec3 v, const mat4 m){
	const vec3 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3],
		.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3]
	};

	return(out);
}

// Left-multiply a 4x1 column vector by a matrix (m*v)!
void mat4MultiplyVec4(const mat4 *const restrict m, vec4 *const restrict v){
	const vec4 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z + m->m[3][0] * temp.w;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z + m->m[3][1] * temp.w;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z + m->m[3][2] * temp.w;
	v->w = m->m[0][3] * temp.x + m->m[1][3] * temp.y + m->m[2][3] * temp.z + m->m[3][3] * temp.w;
}

/*
** Left-multiply a 4x1 column vector by a matrix (m*v) and store the result in "out"!
** This assumes that "out" isn't "v".
*/
void mat4MultiplyVec4Out(const mat4 *const restrict m, const vec4 *const restrict v, vec4 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0] * v->w;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1] * v->w;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2] * v->w;
	out->w = m->m[0][3] * v->x + m->m[1][3] * v->y + m->m[2][3] * v->z + m->m[3][3] * v->w;
}

// Left-multiply a 4x1 column vector by a matrix (m*v) and return the result!
vec4 mat4MultiplyVec4C(const mat4 m, const vec4 v){
	const vec4 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w,
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w,
		.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w
	};

	return(out);
}

// Right-multiply a 1x4 row vector by a matrix (v^T*m)!
void vec4MultiplyMat4(vec4 *const restrict v, const mat4 *const restrict m){
	const vec4 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1] + temp.z * m->m[0][2] + temp.w * m->m[0][3];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1] + temp.z * m->m[1][2] + temp.w * m->m[1][3];
	v->z = temp.x * m->m[2][0] + temp.y * m->m[2][1] + temp.z * m->m[2][2] + temp.w * m->m[2][3];
	v->w = temp.x * m->m[3][0] + temp.y * m->m[3][1] + temp.z * m->m[3][2] + temp.w * m->m[3][3];
}

// Right-multiply a 1x4 row vector by a matrix (v^T*m) and store the result in "out"!
void vec4MultiplyMat4Out(const vec4 *const restrict v, const mat4 *const restrict m, vec4 *const restrict out){
	out->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2] + v->w * m->m[0][3];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2] + v->w * m->m[1][3];
	out->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2] + v->w * m->m[2][3];
	out->w = v->x * m->m[3][0] + v->y * m->m[3][1] + v->z * m->m[3][2] + v->w * m->m[3][3];
}

// Right-multiply a 1x4 row vector by a matrix (v^T*m) and return the result!
vec4 vec4MultiplyMat4C(const vec4 v, const mat4 m){
	const vec4 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3],
		.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3],
		.w = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3]
	};

	return(out);
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x3 transformation matrix "m2" (m1*m2)!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat4MultiplyMat3(mat4 *const restrict m1, const mat3 m2){
	const mat4 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[0][1] +
	              tempMatrix.m[2][0] * m2.m[0][2];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1] +
	              tempMatrix.m[2][1] * m2.m[0][2];
	m1->m[0][2] = tempMatrix.m[0][2] * m2.m[0][0] + tempMatrix.m[1][2] * m2.m[0][1] +
	              tempMatrix.m[2][2] * m2.m[0][2];
	m1->m[0][3] = tempMatrix.m[0][3] * m2.m[0][0] + tempMatrix.m[1][3] * m2.m[0][1] +
	              tempMatrix.m[2][3] * m2.m[0][2];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[1][0] * m2.m[1][1] +
	              tempMatrix.m[2][0] * m2.m[1][2];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1] +
	              tempMatrix.m[2][1] * m2.m[1][2];
	m1->m[1][2] = tempMatrix.m[0][2] * m2.m[1][0] + tempMatrix.m[1][2] * m2.m[1][1] +
	              tempMatrix.m[2][2] * m2.m[1][2];
	m1->m[1][3] = tempMatrix.m[0][3] * m2.m[1][0] + tempMatrix.m[1][3] * m2.m[1][1] +
	              tempMatrix.m[2][3] * m2.m[1][2];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[2][0] + tempMatrix.m[1][0] * m2.m[2][1] +
	              tempMatrix.m[2][0] * m2.m[2][2];
	m1->m[2][1] = tempMatrix.m[0][1] * m2.m[2][0] + tempMatrix.m[1][1] * m2.m[2][1] +
	              tempMatrix.m[2][1] * m2.m[2][2];
	m1->m[2][2] = tempMatrix.m[0][2] * m2.m[2][0] + tempMatrix.m[1][2] * m2.m[2][1] +
	              tempMatrix.m[2][2] * m2.m[2][2];
	m1->m[2][3] = tempMatrix.m[0][3] * m2.m[2][0] + tempMatrix.m[1][3] * m2.m[2][1] +
	              tempMatrix.m[2][3] * m2.m[2][2];
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x3 transformation matrix "m2" (m1*m2) and store the result in "out"!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat4MultiplyMat3Out(const mat4 m1, const mat3 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2];
	out->m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2];
	out->m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2];
	out->m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2];

	out->m[3][0] = m1.m[3][0];
	out->m[3][1] = m1.m[3][1];
	out->m[3][2] = m1.m[3][2];
	out->m[3][3] = m1.m[3][3];
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x3 transformation matrix "m2" (m1*m2) and return the result!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
mat4 mat4MultiplyMat3C(const mat4 m1, const mat3 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2],
		.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2],
		.m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2],
		.m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2],

		.m[3][0] = m1.m[3][0],
		.m[3][1] = m1.m[3][1],
		.m[3][2] = m1.m[3][2],
		.m[3][3] = m1.m[3][3]
	};

	return(out);
}

/*
** Left-multiply the 4x4 matrix "m2" by the 3x3 transformation matrix "m1" (m1*m2) and store the result in "m2"!
** We pad "m1" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat3MultiplyMat4(const mat3 m1, mat4 *const restrict m2){
	const mat4 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[1][0] * tempMatrix.m[0][1] +
	              m1.m[2][0] * tempMatrix.m[0][2];
	m2->m[0][1] = m1.m[0][1] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[0][1] +
	              m1.m[2][1] * tempMatrix.m[0][2];
	m2->m[0][2] = m1.m[0][2] * tempMatrix.m[0][0] + m1.m[1][2] * tempMatrix.m[0][1] +
	              m1.m[2][2] * tempMatrix.m[0][2];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[1][0] + m1.m[1][0] * tempMatrix.m[1][1] +
	              m1.m[2][0] * tempMatrix.m[1][2];
	m2->m[1][1] = m1.m[0][1] * tempMatrix.m[1][0] + m1.m[1][1] * tempMatrix.m[1][1] +
	              m1.m[2][1] * tempMatrix.m[1][2];
	m2->m[1][2] = m1.m[0][2] * tempMatrix.m[1][0] + m1.m[1][2] * tempMatrix.m[1][1] +
	              m1.m[2][2] * tempMatrix.m[1][2];

	m2->m[2][0] = m1.m[0][0] * tempMatrix.m[2][0] + m1.m[1][0] * tempMatrix.m[2][1] +
	              m1.m[2][0] * tempMatrix.m[2][2];
	m2->m[2][1] = m1.m[0][1] * tempMatrix.m[2][0] + m1.m[1][1] * tempMatrix.m[2][1] +
	              m1.m[2][1] * tempMatrix.m[2][2];
	m2->m[2][2] = m1.m[0][2] * tempMatrix.m[2][0] + m1.m[1][2] * tempMatrix.m[2][1] +
	              m1.m[2][2] * tempMatrix.m[2][2];

	m2->m[3][0] = m1.m[0][0] * tempMatrix.m[3][0] + m1.m[1][0] * tempMatrix.m[3][1] +
	              m1.m[2][0] * tempMatrix.m[3][2];
	m2->m[3][1] = m1.m[0][1] * tempMatrix.m[3][0] + m1.m[1][1] * tempMatrix.m[3][1] +
	              m1.m[2][1] * tempMatrix.m[3][2];
	m2->m[3][2] = m1.m[0][2] * tempMatrix.m[3][0] + m1.m[1][2] * tempMatrix.m[3][1] +
	              m1.m[2][2] * tempMatrix.m[3][2];
}

/*
** Left-multiply the 4x4 matrix "m2" by the 3x3 transformation matrix "m1" (m1*m2) and store the result in "out"!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat3MultiplyMat4Out(const mat3 m1, const mat4 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2];
	out->m[0][3] = m2.m[0][3];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2];
	out->m[1][3] = m2.m[1][3];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2];
	out->m[2][3] = m2.m[2][3];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2];
	out->m[3][3] = m2.m[3][3];
}

/*
** Left-multiply the 4x4 matrix "m2" by the 3x3 transformation matrix "m1" (m1*m2) and return the result!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
mat4 mat3MultiplyMat4C(const mat3 m1, const mat4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2],
		.m[0][3] = m2.m[0][3],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2],
		.m[1][3] = m2.m[1][3],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2],
		.m[2][3] = m2.m[2][3],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2],
		.m[3][3] = m2.m[3][3]
	};

	return(out);
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x4 transformation matrix "m2" (m1*m2) and store the result in "m1"!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat4MultiplyMat3x4(mat4 *const restrict m1, const mat3x4 m2){
	const mat4 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[0][1] +
	              tempMatrix.m[2][0] * m2.m[0][2];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1] +
	              tempMatrix.m[2][1] * m2.m[0][2];
	m1->m[0][2] = tempMatrix.m[0][2] * m2.m[0][0] + tempMatrix.m[1][2] * m2.m[0][1] +
	              tempMatrix.m[2][2] * m2.m[0][2];
	m1->m[0][3] = tempMatrix.m[0][3] * m2.m[0][0] + tempMatrix.m[1][3] * m2.m[0][1] +
	              tempMatrix.m[2][3] * m2.m[0][2];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[1][0] * m2.m[1][1] +
	              tempMatrix.m[2][0] * m2.m[1][2];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1] +
	              tempMatrix.m[2][1] * m2.m[1][2];
	m1->m[1][2] = tempMatrix.m[0][2] * m2.m[1][0] + tempMatrix.m[1][2] * m2.m[1][1] +
	              tempMatrix.m[2][2] * m2.m[1][2];
	m1->m[1][3] = tempMatrix.m[0][3] * m2.m[1][0] + tempMatrix.m[1][3] * m2.m[1][1] +
	              tempMatrix.m[2][3] * m2.m[1][2];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[2][0] + tempMatrix.m[1][0] * m2.m[2][1] +
	              tempMatrix.m[2][0] * m2.m[2][2];
	m1->m[2][1] = tempMatrix.m[0][1] * m2.m[2][0] + tempMatrix.m[1][1] * m2.m[2][1] +
	              tempMatrix.m[2][1] * m2.m[2][2];
	m1->m[2][2] = tempMatrix.m[0][2] * m2.m[2][0] + tempMatrix.m[1][2] * m2.m[2][1] +
	              tempMatrix.m[2][2] * m2.m[2][2];
	m1->m[2][3] = tempMatrix.m[0][3] * m2.m[2][0] + tempMatrix.m[1][3] * m2.m[2][1] +
	              tempMatrix.m[2][3] * m2.m[2][2];

	m1->m[3][0] = tempMatrix.m[0][0] * m2.m[3][0] + tempMatrix.m[1][0] * m2.m[3][1] +
	              tempMatrix.m[2][0] * m2.m[3][2] + tempMatrix.m[3][0];
	m1->m[3][1] = tempMatrix.m[0][1] * m2.m[3][0] + tempMatrix.m[1][1] * m2.m[3][1] +
	              tempMatrix.m[2][1] * m2.m[3][2] + tempMatrix.m[3][1];
	m1->m[3][2] = tempMatrix.m[0][2] * m2.m[3][0] + tempMatrix.m[1][2] * m2.m[3][1] +
	              tempMatrix.m[2][2] * m2.m[3][2] + tempMatrix.m[3][2];
	m1->m[3][3] = tempMatrix.m[0][3] * m2.m[3][0] + tempMatrix.m[1][3] * m2.m[3][1] +
	              tempMatrix.m[2][3] * m2.m[3][2] + tempMatrix.m[3][3];
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x4 transformation matrix "m2" (m1*m2) and store the result in "out"!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat4MultiplyMat3x4Out(const mat4 m1, const mat3x4 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
	               m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
	               m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
	               m1.m[2][2] * m2.m[0][2];
	out->m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] +
	               m1.m[2][3] * m2.m[0][2];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
	               m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
	               m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
	               m1.m[2][2] * m2.m[1][2];
	out->m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] +
	               m1.m[2][3] * m2.m[1][2];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
	               m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
	               m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
	               m1.m[2][2] * m2.m[2][2];
	out->m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] +
	               m1.m[2][3] * m2.m[2][2];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
	               m1.m[2][0] * m2.m[3][2] + m1.m[3][0];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
	               m1.m[2][1] * m2.m[3][2] + m1.m[3][1];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
	               m1.m[2][2] * m2.m[3][2] + m1.m[3][2];
	out->m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] +
	               m1.m[2][3] * m2.m[3][2] + m1.m[3][3];
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x4 transformation matrix "m2" (m1*m2) and return the result!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
mat4 mat4MultiplyMat3x4C(const mat4 m1, const mat3x4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
		           m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
		           m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
		           m1.m[2][2] * m2.m[0][2],
		.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] +
		           m1.m[2][3] * m2.m[0][2],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
		           m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
		           m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
		           m1.m[2][2] * m2.m[1][2],
		.m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] +
		           m1.m[2][3] * m2.m[1][2],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
		           m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
		           m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
		           m1.m[2][2] * m2.m[2][2],
		.m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] +
		           m1.m[2][3] * m2.m[2][2],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
		           m1.m[2][0] * m2.m[3][2] + m1.m[3][0],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
		           m1.m[2][1] * m2.m[3][2] + m1.m[3][1],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
		           m1.m[2][2] * m2.m[3][2] + m1.m[3][2],
		.m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] +
		           m1.m[2][3] * m2.m[3][2] + m1.m[3][3]
	};

	return(out);
}

// Right-multiply the 4x4 matrix "m1" by the 3x4 transformation matrix "m2" (m1*m2) and store the result in "m1"!
void mat3x4MultiplyMat4(mat3x4 *const restrict m1, const mat4 m2){
	const mat3x4 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[0][1] +
	              tempMatrix.m[2][0] * m2.m[0][2] + tempMatrix.m[3][0] * m2.m[0][3];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1] +
	              tempMatrix.m[2][1] * m2.m[0][2] + tempMatrix.m[3][1] * m2.m[0][3];
	m1->m[0][2] = tempMatrix.m[0][2] * m2.m[0][0] + tempMatrix.m[1][2] * m2.m[0][1] +
	              tempMatrix.m[2][2] * m2.m[0][2] + tempMatrix.m[3][2] * m2.m[0][3];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[1][0] * m2.m[1][1] +
	              tempMatrix.m[2][0] * m2.m[1][2] + tempMatrix.m[3][0] * m2.m[1][3];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1] +
	              tempMatrix.m[2][1] * m2.m[1][2] + tempMatrix.m[3][1] * m2.m[1][3];
	m1->m[1][2] = tempMatrix.m[0][2] * m2.m[1][0] + tempMatrix.m[1][2] * m2.m[1][1] +
	              tempMatrix.m[2][2] * m2.m[1][2] + tempMatrix.m[3][2] * m2.m[1][3];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[2][0] + tempMatrix.m[1][0] * m2.m[2][1] +
	              tempMatrix.m[2][0] * m2.m[2][2] + tempMatrix.m[3][0] * m2.m[2][3];
	m1->m[2][1] = tempMatrix.m[0][1] * m2.m[2][0] + tempMatrix.m[1][1] * m2.m[2][1] +
	              tempMatrix.m[2][1] * m2.m[2][2] + tempMatrix.m[3][1] * m2.m[2][3];
	m1->m[2][2] = tempMatrix.m[0][2] * m2.m[2][0] + tempMatrix.m[1][2] * m2.m[2][1] +
	              tempMatrix.m[2][2] * m2.m[2][2] + tempMatrix.m[3][2] * m2.m[2][3];

	m1->m[3][0] = tempMatrix.m[0][0] * m2.m[3][0] + tempMatrix.m[1][0] * m2.m[3][1] +
	              tempMatrix.m[2][0] * m2.m[3][2] + tempMatrix.m[3][0] * m2.m[3][3];
	m1->m[3][1] = tempMatrix.m[0][1] * m2.m[3][0] + tempMatrix.m[1][1] * m2.m[3][1] +
	              tempMatrix.m[2][1] * m2.m[3][2] + tempMatrix.m[3][1] * m2.m[3][3];
	m1->m[3][2] = tempMatrix.m[0][2] * m2.m[3][0] + tempMatrix.m[1][2] * m2.m[3][1] +
	              tempMatrix.m[2][2] * m2.m[3][2] + tempMatrix.m[3][2] * m2.m[3][3];
}

// Right-multiply the 4x4 matrix "m1" by the 3x4 transformation matrix "m2" (m1*m2) and store the result in "out"!
void mat3x4MultiplyMat4Out(const mat3x4 m1, const mat4 m2, mat3x4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
	               m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
	               m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
	               m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
	               m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
	               m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
	               m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
	               m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
	               m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
	               m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
	               m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
	               m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
	               m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
}

// Right-multiply the 4x4 matrix "m1" by the 3x4 transformation matrix "m2" (m1*m2) and return the result!
mat3x4 mat3x4MultiplyMat4C(const mat3x4 m1, const mat4 m2){
	const mat3x4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
		           m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
		           m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
		           m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
		           m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
		           m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
		           m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
		           m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
		           m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
		           m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
		           m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
		           m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
		           m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3]
	};

	return(out);
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "m1"!
void mat4MultiplyMat4P1(mat4 *const restrict m1, const mat4 m2){
	const mat4 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[0][1] +
	              tempMatrix.m[2][0] * m2.m[0][2] + tempMatrix.m[3][0] * m2.m[0][3];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1] +
	              tempMatrix.m[2][1] * m2.m[0][2] + tempMatrix.m[3][1] * m2.m[0][3];
	m1->m[0][2] = tempMatrix.m[0][2] * m2.m[0][0] + tempMatrix.m[1][2] * m2.m[0][1] +
	              tempMatrix.m[2][2] * m2.m[0][2] + tempMatrix.m[3][2] * m2.m[0][3];
	m1->m[0][3] = tempMatrix.m[0][3] * m2.m[0][0] + tempMatrix.m[1][3] * m2.m[0][1] +
	              tempMatrix.m[2][3] * m2.m[0][2] + tempMatrix.m[3][3] * m2.m[0][3];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[1][0] * m2.m[1][1] +
	              tempMatrix.m[2][0] * m2.m[1][2] + tempMatrix.m[3][0] * m2.m[1][3];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1] +
	              tempMatrix.m[2][1] * m2.m[1][2] + tempMatrix.m[3][1] * m2.m[1][3];
	m1->m[1][2] = tempMatrix.m[0][2] * m2.m[1][0] + tempMatrix.m[1][2] * m2.m[1][1] +
	              tempMatrix.m[2][2] * m2.m[1][2] + tempMatrix.m[3][2] * m2.m[1][3];
	m1->m[1][3] = tempMatrix.m[0][3] * m2.m[1][0] + tempMatrix.m[1][3] * m2.m[1][1] +
	              tempMatrix.m[2][3] * m2.m[1][2] + tempMatrix.m[3][3] * m2.m[1][3];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[2][0] + tempMatrix.m[1][0] * m2.m[2][1] +
	              tempMatrix.m[2][0] * m2.m[2][2] + tempMatrix.m[3][0] * m2.m[2][3];
	m1->m[2][1] = tempMatrix.m[0][1] * m2.m[2][0] + tempMatrix.m[1][1] * m2.m[2][1] +
	              tempMatrix.m[2][1] * m2.m[2][2] + tempMatrix.m[3][1] * m2.m[2][3];
	m1->m[2][2] = tempMatrix.m[0][2] * m2.m[2][0] + tempMatrix.m[1][2] * m2.m[2][1] +
	              tempMatrix.m[2][2] * m2.m[2][2] + tempMatrix.m[3][2] * m2.m[2][3];
	m1->m[2][3] = tempMatrix.m[0][3] * m2.m[2][0] + tempMatrix.m[1][3] * m2.m[2][1] +
	              tempMatrix.m[2][3] * m2.m[2][2] + tempMatrix.m[3][3] * m2.m[2][3];

	m1->m[3][0] = tempMatrix.m[0][0] * m2.m[3][0] + tempMatrix.m[1][0] * m2.m[3][1] +
	              tempMatrix.m[2][0] * m2.m[3][2] + tempMatrix.m[3][0] * m2.m[3][3];
	m1->m[3][1] = tempMatrix.m[0][1] * m2.m[3][0] + tempMatrix.m[1][1] * m2.m[3][1] +
	              tempMatrix.m[2][1] * m2.m[3][2] + tempMatrix.m[3][1] * m2.m[3][3];
	m1->m[3][2] = tempMatrix.m[0][2] * m2.m[3][0] + tempMatrix.m[1][2] * m2.m[3][1] +
	              tempMatrix.m[2][2] * m2.m[3][2] + tempMatrix.m[3][2] * m2.m[3][3];
	m1->m[3][3] = tempMatrix.m[0][3] * m2.m[3][0] + tempMatrix.m[1][3] * m2.m[3][1] +
	              tempMatrix.m[2][3] * m2.m[3][2] + tempMatrix.m[3][3] * m2.m[3][3];
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "m2"!
void mat4MultiplyMat4P2(const mat4 m1, mat4 *const restrict m2){
	const mat4 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[1][0] * tempMatrix.m[0][1] +
	              m1.m[2][0] * tempMatrix.m[0][2] + m1.m[3][0] * tempMatrix.m[0][3];
	m2->m[0][1] = m1.m[0][1] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[0][1] +
	              m1.m[2][1] * tempMatrix.m[0][2] + m1.m[3][1] * tempMatrix.m[0][3];
	m2->m[0][2] = m1.m[0][2] * tempMatrix.m[0][0] + m1.m[1][2] * tempMatrix.m[0][1] +
	              m1.m[2][2] * tempMatrix.m[0][2] + m1.m[3][2] * tempMatrix.m[0][3];
	m2->m[0][3] = m1.m[0][3] * tempMatrix.m[0][0] + m1.m[1][3] * tempMatrix.m[0][1] +
	              m1.m[2][3] * tempMatrix.m[0][2] + m1.m[3][3] * tempMatrix.m[0][3];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[1][0] + m1.m[1][0] * tempMatrix.m[1][1] +
	              m1.m[2][0] * tempMatrix.m[1][2] + m1.m[3][0] * tempMatrix.m[1][3];
	m2->m[1][1] = m1.m[0][1] * tempMatrix.m[1][0] + m1.m[1][1] * tempMatrix.m[1][1] +
	              m1.m[2][1] * tempMatrix.m[1][2] + m1.m[3][1] * tempMatrix.m[1][3];
	m2->m[1][2] = m1.m[0][2] * tempMatrix.m[1][0] + m1.m[1][2] * tempMatrix.m[1][1] +
	              m1.m[2][2] * tempMatrix.m[1][2] + m1.m[3][2] * tempMatrix.m[1][3];
	m2->m[1][3] = m1.m[0][3] * tempMatrix.m[1][0] + m1.m[1][3] * tempMatrix.m[1][1] +
	              m1.m[2][3] * tempMatrix.m[1][2] + m1.m[3][3] * tempMatrix.m[1][3];

	m2->m[2][0] = m1.m[0][0] * tempMatrix.m[2][0] + m1.m[1][0] * tempMatrix.m[2][1] +
	              m1.m[2][0] * tempMatrix.m[2][2] + m1.m[3][0] * tempMatrix.m[2][3];
	m2->m[2][1] = m1.m[0][1] * tempMatrix.m[2][0] + m1.m[1][1] * tempMatrix.m[2][1] +
	              m1.m[2][1] * tempMatrix.m[2][2] + m1.m[3][1] * tempMatrix.m[2][3];
	m2->m[2][2] = m1.m[0][2] * tempMatrix.m[2][0] + m1.m[1][2] * tempMatrix.m[2][1] +
	              m1.m[2][2] * tempMatrix.m[2][2] + m1.m[3][2] * tempMatrix.m[2][3];
	m2->m[2][3] = m1.m[0][3] * tempMatrix.m[2][0] + m1.m[1][3] * tempMatrix.m[2][1] +
	              m1.m[2][3] * tempMatrix.m[2][2] + m1.m[3][3] * tempMatrix.m[2][3];

	m2->m[3][0] = m1.m[0][0] * tempMatrix.m[3][0] + m1.m[1][0] * tempMatrix.m[3][1] +
	              m1.m[2][0] * tempMatrix.m[3][2] + m1.m[3][0] * tempMatrix.m[3][3];
	m2->m[3][1] = m1.m[0][1] * tempMatrix.m[3][0] + m1.m[1][1] * tempMatrix.m[3][1] +
	              m1.m[2][1] * tempMatrix.m[3][2] + m1.m[3][1] * tempMatrix.m[3][3];
	m2->m[3][2] = m1.m[0][2] * tempMatrix.m[3][0] + m1.m[1][2] * tempMatrix.m[3][1] +
	              m1.m[2][2] * tempMatrix.m[3][2] + m1.m[3][2] * tempMatrix.m[3][3];
	m2->m[3][3] = m1.m[0][3] * tempMatrix.m[3][0] + m1.m[1][3] * tempMatrix.m[3][1] +
	              m1.m[2][3] * tempMatrix.m[3][2] + m1.m[3][3] * tempMatrix.m[3][3];
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "out"!
void mat4MultiplyMat4Out(const mat4 m1, const mat4 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
	               m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
	               m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
	               m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];
	out->m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] +
	               m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
	               m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
	               m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
	               m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];
	out->m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] +
	               m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
	               m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
	               m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
	               m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];
	out->m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] +
	               m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
	               m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
	               m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
	               m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
	out->m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] +
	               m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];
}

// Left-multiply "m2" by "m1" (m1*m2) and return the result!
mat4 mat4MultiplyMat4C(const mat4 m1, const mat4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
		           m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
		           m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
		           m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3],
		.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] +
		           m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
		           m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
		           m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
		           m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3],
		.m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] +
		           m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
		           m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
		           m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
		           m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3],
		.m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] +
		           m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
		           m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
		           m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
		           m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3],
		.m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] +
		           m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3]
	};

	return(out);
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and store the result in "m1"!
void mat4MultiplyMat4TransP1(mat4 *const restrict m1, const mat4 m2){
	const mat4 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[1][0] +
	              tempMatrix.m[2][0] * m2.m[2][0] + tempMatrix.m[3][0] * m2.m[3][0];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[1][0] +
	              tempMatrix.m[2][1] * m2.m[2][0] + tempMatrix.m[3][1] * m2.m[3][0];
	m1->m[0][2] = tempMatrix.m[0][2] * m2.m[0][0] + tempMatrix.m[1][2] * m2.m[1][0] +
	              tempMatrix.m[2][2] * m2.m[2][0] + tempMatrix.m[3][2] * m2.m[3][0];
	m1->m[0][3] = tempMatrix.m[0][3] * m2.m[0][0] + tempMatrix.m[1][3] * m2.m[1][0] +
	              tempMatrix.m[2][3] * m2.m[2][0] + tempMatrix.m[3][3] * m2.m[3][0];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[0][1] + tempMatrix.m[1][0] * m2.m[1][1] +
	              tempMatrix.m[2][0] * m2.m[2][1] + tempMatrix.m[3][0] * m2.m[3][1];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[0][1] + tempMatrix.m[1][1] * m2.m[1][1] +
	              tempMatrix.m[2][1] * m2.m[2][1] + tempMatrix.m[3][1] * m2.m[3][1];
	m1->m[1][2] = tempMatrix.m[0][2] * m2.m[0][1] + tempMatrix.m[1][2] * m2.m[1][1] +
	              tempMatrix.m[2][2] * m2.m[2][1] + tempMatrix.m[3][2] * m2.m[3][1];
	m1->m[1][3] = tempMatrix.m[0][3] * m2.m[0][1] + tempMatrix.m[1][3] * m2.m[1][1] +
	              tempMatrix.m[2][3] * m2.m[2][1] + tempMatrix.m[3][3] * m2.m[3][1];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[0][2] + tempMatrix.m[1][0] * m2.m[1][2] +
	              tempMatrix.m[2][0] * m2.m[2][2] + tempMatrix.m[3][0] * m2.m[3][2];
	m1->m[2][1] = tempMatrix.m[0][1] * m2.m[0][2] + tempMatrix.m[1][1] * m2.m[1][2] +
	              tempMatrix.m[2][1] * m2.m[2][2] + tempMatrix.m[3][1] * m2.m[3][2];
	m1->m[2][2] = tempMatrix.m[0][2] * m2.m[0][2] + tempMatrix.m[1][2] * m2.m[1][2] +
	              tempMatrix.m[2][2] * m2.m[2][2] + tempMatrix.m[3][2] * m2.m[3][2];
	m1->m[2][3] = tempMatrix.m[0][3] * m2.m[0][2] + tempMatrix.m[1][3] * m2.m[1][2] +
	              tempMatrix.m[2][3] * m2.m[2][2] + tempMatrix.m[3][3] * m2.m[3][2];

	m1->m[3][0] = tempMatrix.m[0][0] * m2.m[0][3] + tempMatrix.m[1][0] * m2.m[1][3] +
	              tempMatrix.m[2][0] * m2.m[2][3] + tempMatrix.m[3][0] * m2.m[3][3];
	m1->m[3][1] = tempMatrix.m[0][1] * m2.m[0][3] + tempMatrix.m[1][1] * m2.m[1][3] +
	              tempMatrix.m[2][1] * m2.m[2][3] + tempMatrix.m[3][1] * m2.m[3][3];
	m1->m[3][2] = tempMatrix.m[0][2] * m2.m[0][3] + tempMatrix.m[1][2] * m2.m[1][3] +
	              tempMatrix.m[2][2] * m2.m[2][3] + tempMatrix.m[3][2] * m2.m[3][3];
	m1->m[3][3] = tempMatrix.m[0][3] * m2.m[0][3] + tempMatrix.m[1][3] * m2.m[1][3] +
	              tempMatrix.m[2][3] * m2.m[2][3] + tempMatrix.m[3][3] * m2.m[3][3];
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and store the result in "m2"!
void mat4MultiplyMat4TransP2(const mat4 m1, mat4 *const restrict m2){
	const mat4 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[1][0] * tempMatrix.m[1][0] +
	              m1.m[2][0] * tempMatrix.m[2][0] + m1.m[3][0] * tempMatrix.m[3][0];
	m2->m[0][1] = m1.m[0][1] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[1][0] +
	              m1.m[2][1] * tempMatrix.m[2][0] + m1.m[3][1] * tempMatrix.m[3][0];
	m2->m[0][2] = m1.m[0][2] * tempMatrix.m[0][0] + m1.m[1][2] * tempMatrix.m[1][0] +
	              m1.m[2][2] * tempMatrix.m[2][0] + m1.m[3][2] * tempMatrix.m[3][0];
	m2->m[0][3] = m1.m[0][3] * tempMatrix.m[0][0] + m1.m[1][3] * tempMatrix.m[1][0] +
	              m1.m[2][3] * tempMatrix.m[2][0] + m1.m[3][3] * tempMatrix.m[3][0];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[0][1] + m1.m[1][0] * tempMatrix.m[1][1] +
	              m1.m[2][0] * tempMatrix.m[2][1] + m1.m[3][0] * tempMatrix.m[3][1];
	m2->m[1][1] = m1.m[0][1] * tempMatrix.m[0][1] + m1.m[1][1] * tempMatrix.m[1][1] +
	              m1.m[2][1] * tempMatrix.m[2][1] + m1.m[3][1] * tempMatrix.m[3][1];
	m2->m[1][2] = m1.m[0][2] * tempMatrix.m[0][1] + m1.m[1][2] * tempMatrix.m[1][1] +
	              m1.m[2][2] * tempMatrix.m[2][1] + m1.m[3][2] * tempMatrix.m[3][1];
	m2->m[1][3] = m1.m[0][3] * tempMatrix.m[0][1] + m1.m[1][3] * tempMatrix.m[1][1] +
	              m1.m[2][3] * tempMatrix.m[2][1] + m1.m[3][3] * tempMatrix.m[3][1];

	m2->m[2][0] = m1.m[0][0] * tempMatrix.m[0][2] + m1.m[1][0] * tempMatrix.m[1][2] +
	              m1.m[2][0] * tempMatrix.m[2][2] + m1.m[3][0] * tempMatrix.m[3][2];
	m2->m[2][1] = m1.m[0][1] * tempMatrix.m[0][2] + m1.m[1][1] * tempMatrix.m[1][2] +
	              m1.m[2][1] * tempMatrix.m[2][2] + m1.m[3][1] * tempMatrix.m[3][2];
	m2->m[2][2] = m1.m[0][2] * tempMatrix.m[0][2] + m1.m[1][2] * tempMatrix.m[1][2] +
	              m1.m[2][2] * tempMatrix.m[2][2] + m1.m[3][2] * tempMatrix.m[3][2];
	m2->m[2][3] = m1.m[0][3] * tempMatrix.m[0][2] + m1.m[1][3] * tempMatrix.m[1][2] +
	              m1.m[2][3] * tempMatrix.m[2][2] + m1.m[3][3] * tempMatrix.m[3][2];

	m2->m[3][0] = m1.m[0][0] * tempMatrix.m[0][3] + m1.m[1][0] * tempMatrix.m[1][3] +
	              m1.m[2][0] * tempMatrix.m[2][3] + m1.m[3][0] * tempMatrix.m[3][3];
	m2->m[3][1] = m1.m[0][1] * tempMatrix.m[0][3] + m1.m[1][1] * tempMatrix.m[1][3] +
	              m1.m[2][1] * tempMatrix.m[2][3] + m1.m[3][1] * tempMatrix.m[3][3];
	m2->m[3][2] = m1.m[0][2] * tempMatrix.m[0][3] + m1.m[1][2] * tempMatrix.m[1][3] +
	              m1.m[2][2] * tempMatrix.m[2][3] + m1.m[3][2] * tempMatrix.m[3][3];
	m2->m[3][3] = m1.m[0][3] * tempMatrix.m[0][3] + m1.m[1][3] * tempMatrix.m[1][3] +
	              m1.m[2][3] * tempMatrix.m[2][3] + m1.m[3][3] * tempMatrix.m[3][3];
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and store the result in "out"!
void mat4MultiplyMat4TransOut(const mat4 m1, const mat4 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[1][0] +
	               m1.m[2][0] * m2.m[2][0] + m1.m[3][0] * m2.m[3][0];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] +
	               m1.m[2][1] * m2.m[2][0] + m1.m[3][1] * m2.m[3][0];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[1][0] +
	               m1.m[2][2] * m2.m[2][0] + m1.m[3][2] * m2.m[3][0];
	out->m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[1][0] +
	               m1.m[2][3] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];

	out->m[1][0] = m1.m[0][0] * m2.m[0][1] + m1.m[1][0] * m2.m[1][1] +
	               m1.m[2][0] * m2.m[2][1] + m1.m[3][0] * m2.m[3][1];
	out->m[1][1] = m1.m[0][1] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] +
	               m1.m[2][1] * m2.m[2][1] + m1.m[3][1] * m2.m[3][1];
	out->m[1][2] = m1.m[0][2] * m2.m[0][1] + m1.m[1][2] * m2.m[1][1] +
	               m1.m[2][2] * m2.m[2][1] + m1.m[3][2] * m2.m[3][1];
	out->m[1][3] = m1.m[0][3] * m2.m[0][1] + m1.m[1][3] * m2.m[1][1] +
	               m1.m[2][3] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];

	out->m[2][0] = m1.m[0][0] * m2.m[0][2] + m1.m[1][0] * m2.m[1][2] +
	               m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[3][2];
	out->m[2][1] = m1.m[0][1] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] +
	               m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[3][2];
	out->m[2][2] = m1.m[0][2] * m2.m[0][2] + m1.m[1][2] * m2.m[1][2] +
	               m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[3][2];
	out->m[2][3] = m1.m[0][3] * m2.m[0][2] + m1.m[1][3] * m2.m[1][2] +
	               m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];

	out->m[3][0] = m1.m[0][0] * m2.m[0][3] + m1.m[1][0] * m2.m[1][3] +
	               m1.m[2][0] * m2.m[2][3] + m1.m[3][0] * m2.m[3][3];
	out->m[3][1] = m1.m[0][1] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] +
	               m1.m[2][1] * m2.m[2][3] + m1.m[3][1] * m2.m[3][3];
	out->m[3][2] = m1.m[0][2] * m2.m[0][3] + m1.m[1][2] * m2.m[1][3] +
	               m1.m[2][2] * m2.m[2][3] + m1.m[3][2] * m2.m[3][3];
	out->m[3][3] = m1.m[0][3] * m2.m[0][3] + m1.m[1][3] * m2.m[1][3] +
	               m1.m[2][3] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and return the result!
mat4 mat4MultiplyMat4TransC(const mat4 m1, const mat4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[1][0] +
		           m1.m[2][0] * m2.m[2][0] + m1.m[3][0] * m2.m[3][0],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] +
		           m1.m[2][1] * m2.m[2][0] + m1.m[3][1] * m2.m[3][0],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[1][0] +
		           m1.m[2][2] * m2.m[2][0] + m1.m[3][2] * m2.m[3][0],
		.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[1][0] +
		           m1.m[2][3] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],

		.m[1][0] = m1.m[0][0] * m2.m[0][1] + m1.m[1][0] * m2.m[1][1] +
		           m1.m[2][0] * m2.m[2][1] + m1.m[3][0] * m2.m[3][1],
		.m[1][1] = m1.m[0][1] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] +
		           m1.m[2][1] * m2.m[2][1] + m1.m[3][1] * m2.m[3][1],
		.m[1][2] = m1.m[0][2] * m2.m[0][1] + m1.m[1][2] * m2.m[1][1] +
		           m1.m[2][2] * m2.m[2][1] + m1.m[3][2] * m2.m[3][1],
		.m[1][3] = m1.m[0][3] * m2.m[0][1] + m1.m[1][3] * m2.m[1][1] +
		           m1.m[2][3] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],

		.m[2][0] = m1.m[0][0] * m2.m[0][2] + m1.m[1][0] * m2.m[1][2] +
		           m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[3][2],
		.m[2][1] = m1.m[0][1] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] +
		           m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[3][2],
		.m[2][2] = m1.m[0][2] * m2.m[0][2] + m1.m[1][2] * m2.m[1][2] +
		           m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[3][2],
		.m[2][3] = m1.m[0][3] * m2.m[0][2] + m1.m[1][3] * m2.m[1][2] +
		           m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],

		.m[3][0] = m1.m[0][0] * m2.m[0][3] + m1.m[1][0] * m2.m[1][3] +
		           m1.m[2][0] * m2.m[2][3] + m1.m[3][0] * m2.m[3][3],
		.m[3][1] = m1.m[0][1] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] +
		           m1.m[2][1] * m2.m[2][3] + m1.m[3][1] * m2.m[3][3],
		.m[3][2] = m1.m[0][2] * m2.m[0][3] + m1.m[1][2] * m2.m[1][3] +
		           m1.m[2][2] * m2.m[2][3] + m1.m[3][2] * m2.m[3][3],
		.m[3][3] = m1.m[0][3] * m2.m[0][3] + m1.m[1][3] * m2.m[1][3] +
		           m1.m[2][3] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3]
	};

	return(out);
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and store the result in "m1"!
void mat4TransMultiplyMat4P1(mat4 *const restrict m1, const mat4 m2){
	const mat4 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[0][1] * m2.m[0][1] +
	              tempMatrix.m[0][2] * m2.m[0][2] + tempMatrix.m[0][3] * m2.m[0][3];
	m1->m[0][1] = tempMatrix.m[1][0] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1] +
	              tempMatrix.m[1][2] * m2.m[0][2] + tempMatrix.m[1][3] * m2.m[0][3];
	m1->m[0][2] = tempMatrix.m[2][0] * m2.m[0][0] + tempMatrix.m[2][1] * m2.m[0][1] +
	              tempMatrix.m[2][2] * m2.m[0][2] + tempMatrix.m[2][3] * m2.m[0][3];
	m1->m[0][3] = tempMatrix.m[3][0] * m2.m[0][0] + tempMatrix.m[3][1] * m2.m[0][1] +
	              tempMatrix.m[3][2] * m2.m[0][2] + tempMatrix.m[3][3] * m2.m[0][3];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[0][1] * m2.m[1][1] +
	              tempMatrix.m[0][2] * m2.m[1][2] + tempMatrix.m[0][3] * m2.m[1][3];
	m1->m[1][1] = tempMatrix.m[1][0] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1] +
	              tempMatrix.m[1][2] * m2.m[1][2] + tempMatrix.m[1][3] * m2.m[1][3];
	m1->m[1][2] = tempMatrix.m[2][0] * m2.m[1][0] + tempMatrix.m[2][1] * m2.m[1][1] +
	              tempMatrix.m[2][2] * m2.m[1][2] + tempMatrix.m[2][3] * m2.m[1][3];
	m1->m[1][3] = tempMatrix.m[3][0] * m2.m[1][0] + tempMatrix.m[3][1] * m2.m[1][1] +
	              tempMatrix.m[3][2] * m2.m[1][2] + tempMatrix.m[3][3] * m2.m[1][3];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[2][0] + tempMatrix.m[0][1] * m2.m[2][1] +
	              tempMatrix.m[0][2] * m2.m[2][2] + tempMatrix.m[0][3] * m2.m[2][3];
	m1->m[2][1] = tempMatrix.m[1][0] * m2.m[2][0] + tempMatrix.m[1][1] * m2.m[2][1] +
	              tempMatrix.m[1][2] * m2.m[2][2] + tempMatrix.m[1][3] * m2.m[2][3];
	m1->m[2][2] = tempMatrix.m[2][0] * m2.m[2][0] + tempMatrix.m[2][1] * m2.m[2][1] +
	              tempMatrix.m[2][2] * m2.m[2][2] + tempMatrix.m[2][3] * m2.m[2][3];
	m1->m[2][3] = tempMatrix.m[3][0] * m2.m[2][0] + tempMatrix.m[3][1] * m2.m[2][1] +
	              tempMatrix.m[3][2] * m2.m[2][2] + tempMatrix.m[3][3] * m2.m[2][3];

	m1->m[3][0] = tempMatrix.m[0][0] * m2.m[3][0] + tempMatrix.m[0][1] * m2.m[3][1] +
	              tempMatrix.m[0][2] * m2.m[3][2] + tempMatrix.m[0][3] * m2.m[3][3];
	m1->m[3][1] = tempMatrix.m[1][0] * m2.m[3][0] + tempMatrix.m[1][1] * m2.m[3][1] +
	              tempMatrix.m[1][2] * m2.m[3][2] + tempMatrix.m[1][3] * m2.m[3][3];
	m1->m[3][2] = tempMatrix.m[2][0] * m2.m[3][0] + tempMatrix.m[2][1] * m2.m[3][1] +
	              tempMatrix.m[2][2] * m2.m[3][2] + tempMatrix.m[2][3] * m2.m[3][3];
	m1->m[3][3] = tempMatrix.m[3][0] * m2.m[3][0] + tempMatrix.m[3][1] * m2.m[3][1] +
	              tempMatrix.m[3][2] * m2.m[3][2] + tempMatrix.m[3][3] * m2.m[3][3];
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and store the result in "m2"!
void mat4TransMultiplyMat4P2(const mat4 m1, mat4 *const restrict m2){
	const mat4 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[0][1] * tempMatrix.m[0][1] +
	              m1.m[0][2] * tempMatrix.m[0][2] + m1.m[0][3] * tempMatrix.m[0][3];
	m2->m[0][1] = m1.m[1][0] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[0][1] +
	              m1.m[1][2] * tempMatrix.m[0][2] + m1.m[1][3] * tempMatrix.m[0][3];
	m2->m[0][2] = m1.m[2][0] * tempMatrix.m[0][0] + m1.m[2][1] * tempMatrix.m[0][1] +
	              m1.m[2][2] * tempMatrix.m[0][2] + m1.m[2][3] * tempMatrix.m[0][3];
	m2->m[0][3] = m1.m[3][0] * tempMatrix.m[0][0] + m1.m[3][1] * tempMatrix.m[0][1] +
	              m1.m[3][2] * tempMatrix.m[0][2] + m1.m[3][3] * tempMatrix.m[0][3];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[1][0] + m1.m[0][1] * tempMatrix.m[1][1] +
	              m1.m[0][2] * tempMatrix.m[1][2] + m1.m[0][3] * tempMatrix.m[1][3];
	m2->m[1][1] = m1.m[1][0] * tempMatrix.m[1][0] + m1.m[1][1] * tempMatrix.m[1][1] +
	              m1.m[1][2] * tempMatrix.m[1][2] + m1.m[1][3] * tempMatrix.m[1][3];
	m2->m[1][2] = m1.m[2][0] * tempMatrix.m[1][0] + m1.m[2][1] * tempMatrix.m[1][1] +
	              m1.m[2][2] * tempMatrix.m[1][2] + m1.m[2][3] * tempMatrix.m[1][3];
	m2->m[1][3] = m1.m[3][0] * tempMatrix.m[1][0] + m1.m[3][1] * tempMatrix.m[1][1] +
	              m1.m[3][2] * tempMatrix.m[1][2] + m1.m[3][3] * tempMatrix.m[1][3];

	m2->m[2][0] = m1.m[0][0] * tempMatrix.m[2][0] + m1.m[0][1] * tempMatrix.m[2][1] +
	              m1.m[0][2] * tempMatrix.m[2][2] + m1.m[0][3] * tempMatrix.m[2][3];
	m2->m[2][1] = m1.m[1][0] * tempMatrix.m[2][0] + m1.m[1][1] * tempMatrix.m[2][1] +
	              m1.m[1][2] * tempMatrix.m[2][2] + m1.m[1][3] * tempMatrix.m[2][3];
	m2->m[2][2] = m1.m[2][0] * tempMatrix.m[2][0] + m1.m[2][1] * tempMatrix.m[2][1] +
	              m1.m[2][2] * tempMatrix.m[2][2] + m1.m[2][3] * tempMatrix.m[2][3];
	m2->m[2][3] = m1.m[3][0] * tempMatrix.m[2][0] + m1.m[3][1] * tempMatrix.m[2][1] +
	              m1.m[3][2] * tempMatrix.m[2][2] + m1.m[3][3] * tempMatrix.m[2][3];

	m2->m[3][0] = m1.m[0][0] * tempMatrix.m[3][0] + m1.m[0][1] * tempMatrix.m[3][1] +
	              m1.m[0][2] * tempMatrix.m[3][2] + m1.m[0][3] * tempMatrix.m[3][3];
	m2->m[3][1] = m1.m[1][0] * tempMatrix.m[3][0] + m1.m[1][1] * tempMatrix.m[3][1] +
	              m1.m[1][2] * tempMatrix.m[3][2] + m1.m[1][3] * tempMatrix.m[3][3];
	m2->m[3][2] = m1.m[2][0] * tempMatrix.m[3][0] + m1.m[2][1] * tempMatrix.m[3][1] +
	              m1.m[2][2] * tempMatrix.m[3][2] + m1.m[2][3] * tempMatrix.m[3][3];
	m2->m[3][3] = m1.m[3][0] * tempMatrix.m[3][0] + m1.m[3][1] * tempMatrix.m[3][1] +
	              m1.m[3][2] * tempMatrix.m[3][2] + m1.m[3][3] * tempMatrix.m[3][3];
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and store the result in "out"!
void mat4TransMultiplyMat4Out(const mat4 m1, const mat4 m2, mat4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[0][1] +
	               m1.m[0][2] * m2.m[0][2] + m1.m[0][3] * m2.m[0][3];
	out->m[0][1] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
	               m1.m[1][2] * m2.m[0][2] + m1.m[1][3] * m2.m[0][3];
	out->m[0][2] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[0][1] +
	               m1.m[2][2] * m2.m[0][2] + m1.m[2][3] * m2.m[0][3];
	out->m[0][3] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[0][1] +
	               m1.m[3][2] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[0][1] * m2.m[1][1] +
	               m1.m[0][2] * m2.m[1][2] + m1.m[0][3] * m2.m[1][3];
	out->m[1][1] = m1.m[1][0] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
	               m1.m[1][2] * m2.m[1][2] + m1.m[1][3] * m2.m[1][3];
	out->m[1][2] = m1.m[2][0] * m2.m[1][0] + m1.m[2][1] * m2.m[1][1] +
	               m1.m[2][2] * m2.m[1][2] + m1.m[2][3] * m2.m[1][3];
	out->m[1][3] = m1.m[3][0] * m2.m[1][0] + m1.m[3][1] * m2.m[1][1] +
	               m1.m[3][2] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[0][1] * m2.m[2][1] +
	               m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[2][3];
	out->m[2][1] = m1.m[1][0] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
	               m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[2][3];
	out->m[2][2] = m1.m[2][0] * m2.m[2][0] + m1.m[2][1] * m2.m[2][1] +
	               m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[2][3];
	out->m[2][3] = m1.m[3][0] * m2.m[2][0] + m1.m[3][1] * m2.m[2][1] +
	               m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[0][1] * m2.m[3][1] +
	               m1.m[0][2] * m2.m[3][2] + m1.m[0][3] * m2.m[3][3];
	out->m[3][1] = m1.m[1][0] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
	               m1.m[1][2] * m2.m[3][2] + m1.m[1][3] * m2.m[3][3];
	out->m[3][2] = m1.m[2][0] * m2.m[3][0] + m1.m[2][1] * m2.m[3][1] +
	               m1.m[2][2] * m2.m[3][2] + m1.m[2][3] * m2.m[3][3];
	out->m[3][3] = m1.m[3][0] * m2.m[3][0] + m1.m[3][1] * m2.m[3][1] +
	               m1.m[3][2] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and return the result!
mat4 mat4TransMultiplyMat4C(const mat4 m1, const mat4 m2){
	const mat4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[0][1] +
		           m1.m[0][2] * m2.m[0][2] + m1.m[0][3] * m2.m[0][3],
		.m[0][1] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
		           m1.m[1][2] * m2.m[0][2] + m1.m[1][3] * m2.m[0][3],
		.m[0][2] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[0][1] +
		           m1.m[2][2] * m2.m[0][2] + m1.m[2][3] * m2.m[0][3],
		.m[0][3] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[0][1] +
		           m1.m[3][2] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[0][1] * m2.m[1][1] +
		           m1.m[0][2] * m2.m[1][2] + m1.m[0][3] * m2.m[1][3],
		.m[1][1] = m1.m[1][0] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
		           m1.m[1][2] * m2.m[1][2] + m1.m[1][3] * m2.m[1][3],
		.m[1][2] = m1.m[2][0] * m2.m[1][0] + m1.m[2][1] * m2.m[1][1] +
		           m1.m[2][2] * m2.m[1][2] + m1.m[2][3] * m2.m[1][3],
		.m[1][3] = m1.m[3][0] * m2.m[1][0] + m1.m[3][1] * m2.m[1][1] +
		           m1.m[3][2] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[0][1] * m2.m[2][1] +
		           m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[2][3],
		.m[2][1] = m1.m[1][0] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
		           m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[2][3],
		.m[2][2] = m1.m[2][0] * m2.m[2][0] + m1.m[2][1] * m2.m[2][1] +
		           m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[2][3],
		.m[2][3] = m1.m[3][0] * m2.m[2][0] + m1.m[3][1] * m2.m[2][1] +
		           m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[0][1] * m2.m[3][1] +
		           m1.m[0][2] * m2.m[3][2] + m1.m[0][3] * m2.m[3][3],
		.m[3][1] = m1.m[1][0] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
		           m1.m[1][2] * m2.m[3][2] + m1.m[1][3] * m2.m[3][3],
		.m[3][2] = m1.m[2][0] * m2.m[3][0] + m1.m[2][1] * m2.m[3][1] +
		           m1.m[2][2] * m2.m[3][2] + m1.m[2][3] * m2.m[3][3],
		.m[3][3] = m1.m[3][0] * m2.m[3][0] + m1.m[3][1] * m2.m[3][1] +
		           m1.m[3][2] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3]
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

// Right-multiply a transformation matrix by a translation matrix stored as three floats!
void mat4TranslateTransformPre(mat4 *const restrict m, const float x, const float y, const float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
}

// Right-multiply a transformation matrix by a translation matrix stored as three floats!
mat4 mat4TranslateTransformPreC(mat4 m, const float x, const float y, const float z){
	m.m[3][0] = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0];
	m.m[3][1] = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1];
	m.m[3][2] = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2];

	return(m);
}

// Right-multiply a transformation matrix by a translation matrix stored as a vec3!
void mat4TranslateTransformPreVec3(mat4 *const restrict m, const vec3 *const restrict v){
	mat4TranslateTransformPre(m, v->x, v->y, v->z);
}

// Right-multiply a transformation matrix by a translation matrix stored as a vec3!
mat4 mat4TranslateTransformPreVec3C(const mat4 m, const vec3 v){
	return(mat4TranslateTransformPreC(m, v.x, v.y, v.z));
}


// Rotate a matrix using XYZ Euler angles!
void mat4RotateByEulerXYZ(mat4 *const restrict m, const float x, const float y, const float z){
	mat4 rotMatrix;
	mat4InitEulerXYZ(&rotMatrix, x, y, z);
	mat4MultiplyMat4P2(rotMatrix, m);
}

// Rotate a matrix using XYZ Euler angles!
mat4 mat4RotateByEulerXYZC(const mat4 m, const float x, const float y, const float z){
	const mat4 rotMatrix = mat4InitEulerXYZC(x, y, z);
	return(mat4MultiplyMat4C(rotMatrix, m));
}

// Rotate a matrix using ZXY Euler angles!
void mat4RotateByEulerZXY(mat4 *const restrict m, const float x, const float y, const float z){
	mat4 rotMatrix;
	mat4InitEulerZXY(&rotMatrix, x, y, z);
	mat4MultiplyMat4P2(rotMatrix, m);
}

// Rotate a matrix using ZXY Euler angles!
mat4 mat4RotateByEulerZXYC(const mat4 m, const float x, const float y, const float z){
	const mat4 rotMatrix = mat4InitEulerZXYC(x, y, z);
	return(mat4MultiplyMat4C(rotMatrix, m));
}

// Rotate a matrix by a vec3 using XYZ Euler angles!
void mat4RotateByVec3EulerXYZ(mat4 *const restrict m, const vec3 *const restrict v){
	mat4RotateByEulerXYZ(m, v->x, v->y, v->z);
}

// Rotate a matrix by a vec3 using XYZ Euler angles!
mat4 mat4RotateByVec3EulerXYZC(const mat4 m, const vec3 v){
	return(mat4RotateByEulerXYZC(m, v.x, v.y, v.z));
}

// Rotate a matrix by a vec3 using ZXY Euler angles!
void mat4RotateByVec3EulerZXY(mat4 *const restrict m, const vec3 *const restrict v){
	mat4RotateByEulerZXY(m, v->x, v->y, v->z);
}

// Rotate a matrix by a vec3 using ZXY Euler angles!
mat4 mat4RotateByVec3EulerZXYC(const mat4 m, const vec3 v){
	return(mat4RotateByEulerZXYC(m, v.x, v.y, v.z));
}

// Rotate a matrix by an axis and an angle!
void mat4RotateByAxisAngle(mat4 *const restrict m, const vec4 *const restrict v){
	mat3 rotMatrix;
	// Convert the axis angle to a rotation matrix!
	mat3InitAxisAngle(&rotMatrix, v);
	// Now rotate our matrix by it!
	mat3MultiplyMat4(rotMatrix, m);
}

// Rotate a matrix by an axis and an angle!
mat4 mat4RotateByAxisAngleC(const mat4 m, const vec4 v){
	return(mat3MultiplyMat4C(mat3InitAxisAngleC(v), m));
}

// Rotate a matrix by a quaternion!
void mat4RotateByQuat(mat4 *const restrict m, const quat *const restrict q){
	mat3 rotMatrix;
	// Convert the quaternion to a rotation matrix!
	mat3InitQuat(&rotMatrix, q);
	// Now rotate our matrix by it!
	mat3MultiplyMat4(rotMatrix, m);
}

// Rotate a matrix by a quaternion!
mat4 mat4RotateByQuatC(const mat4 m, const quat q){
	return(mat3MultiplyMat4C(mat3InitQuatC(q), m));
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

/*
** Generate a rotation matrix that looks in the
** direction of the normalized vector "forward"!
*/
void mat4RotateForward(
	mat4 *const restrict m,
	const vec3 *const restrict forward,
	const vec3 *const restrict worldUp
){

	vec3 right, up;
	// Get the right vector!
	vec3CrossVec3Out(worldUp, forward, &right);
	vec3NormalizeVec3Fast(&right);
	// Get the up vector!
	vec3CrossVec3Out(forward, &right, &up);
	vec3NormalizeVec3Fast(&up);

	// Rotate the matrix to look forward!
	m->m[0][0] = right.x;
	m->m[0][1] = right.y;
	m->m[0][2] = right.z;
	m->m[0][3] = 0.f;

	m->m[1][0] = up.x;
	m->m[1][1] = up.y;
	m->m[1][2] = up.z;
	m->m[1][3] = 0.f;

	m->m[2][0] = forward->x;
	m->m[2][1] = forward->y;
	m->m[2][2] = forward->z;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

/*
** Generate a rotation matrix that looks in the
** direction of the normalized vector "forward"!
*/
mat4 mat4RotateForwardC(const vec3 forward, const vec3 worldUp){
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));

	// Rotate the matrix to look forward!
	const mat4 m = {
		.m[0][0] = right.x,   .m[0][1] = right.y,   .m[0][2] = right.z,   .m[0][3] = 0.f,
		.m[1][0] = up.x,      .m[1][1] = up.y,      .m[1][2] = up.z,      .m[1][3] = 0.f,
		.m[2][0] = forward.x, .m[2][1] = forward.y, .m[2][2] = forward.z, .m[2][3] = 0.f,
		.m[3][0] = 0.f,       .m[3][1] = 0.f,       .m[3][2] = 0.f,       .m[3][3] = 1.f
	};

	return(m);
}

// Generate a rotation matrix that faces a target!
void mat4RotateToFace(
	mat4 *const restrict m,
	const vec3 *const restrict eye, const vec3 *const restrict target,
	const vec3 *const restrict worldUp
){

	vec3 right, up, forward;
	// Get the forward vector!
	vec3SubtractVec3Out(target, eye, &forward);
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
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3C(target, eye));
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

// Compute the squared Frobenius norm!
float mat4FrobeniusNormSquared(const mat4 *const restrict m){
	return(
		m->m[0][0]*m->m[0][0] + m->m[0][1]*m->m[0][1] + m->m[0][2]*m->m[0][2] + m->m[0][3]*m->m[0][3] +
		m->m[1][0]*m->m[1][0] + m->m[1][1]*m->m[1][1] + m->m[1][2]*m->m[1][2] + m->m[1][3]*m->m[1][3] +
		m->m[2][0]*m->m[2][0] + m->m[2][1]*m->m[2][1] + m->m[2][2]*m->m[2][2] + m->m[2][3]*m->m[2][3] +
		m->m[3][0]*m->m[3][0] + m->m[3][1]*m->m[3][1] + m->m[3][2]*m->m[3][2] + m->m[3][3]*m->m[3][3]
	);
}

// Compute the squared Frobenius norm!
float mat4FrobeniusNormSquaredC(const mat4 m){
	return(
		m.m[0][0]*m.m[0][0] + m.m[0][1]*m.m[0][1] + m.m[0][2]*m.m[0][2] + m.m[0][3]*m.m[0][3] +
		m.m[1][0]*m.m[1][0] + m.m[1][1]*m.m[1][1] + m.m[1][2]*m.m[1][2] + m.m[1][3]*m.m[1][3] +
		m.m[2][0]*m.m[2][0] + m.m[2][1]*m.m[2][1] + m.m[2][2]*m.m[2][2] + m.m[2][3]*m.m[2][3] +
		m.m[3][0]*m.m[3][0] + m.m[3][1]*m.m[3][1] + m.m[3][2]*m.m[3][2] + m.m[3][3]*m.m[3][3]
	);
}

// Compute the trace of a matrix!
float mat4Trace(const mat4 *const restrict m){
	return(m->m[0][0] + m->m[1][1] + m->m[2][2] + m->m[3][3]);
}

// Compute the trace of a matrix!
float mat4TraceC(const mat4 m){
	return(m.m[0][0] + m.m[1][1] + m.m[2][2] + m.m[3][3]);
}

// Invert a matrix!
void mat4Invert(mat4 *const restrict m){
	const mat4 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	// These values represent 2x2 minors on the lower half of our matrix.
	const float f0 = tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[3][2] * tempMatrix.m[2][3];
	const float f1 = tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[3][2] * tempMatrix.m[1][3];
	const float f2 = tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[2][2] * tempMatrix.m[1][3];
	const float f3 = tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[3][2] * tempMatrix.m[0][3];
	const float f4 = tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[2][2] * tempMatrix.m[0][3];
	const float f5 = tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[1][2] * tempMatrix.m[0][3];
	// These values represent the cofactors c_{0,0}, c_{0,1}, c_{0,2} and c_{0,3}.
	const float c00 =   tempMatrix.m[1][1] * f0 - tempMatrix.m[2][1] * f1 + tempMatrix.m[3][1] * f2;
	const float c01 = -(tempMatrix.m[0][1] * f0 - tempMatrix.m[2][1] * f3 + tempMatrix.m[3][1] * f4);
	const float c02 =   tempMatrix.m[0][1] * f1 - tempMatrix.m[1][1] * f3 + tempMatrix.m[3][1] * f5;
	const float c03 = -(tempMatrix.m[0][1] * f2 - tempMatrix.m[1][1] * f4 + tempMatrix.m[2][1] * f5);
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * c00 + tempMatrix.m[1][0] * c01 + tempMatrix.m[2][0] * c02 + tempMatrix.m[3][0] * c03;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		// These values represent 2x2 minors on the upper half of our matrix.
		const float g0 = tempMatrix.m[2][0] * tempMatrix.m[3][1] - tempMatrix.m[3][0] * tempMatrix.m[2][1];
		const float g1 = tempMatrix.m[1][0] * tempMatrix.m[3][1] - tempMatrix.m[3][0] * tempMatrix.m[1][1];
		const float g2 = tempMatrix.m[1][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[1][1];
		const float g3 = tempMatrix.m[0][0] * tempMatrix.m[3][1] - tempMatrix.m[3][0] * tempMatrix.m[0][1];
		const float g4 = tempMatrix.m[0][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[0][1];
		const float g5 = tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[1][0] * tempMatrix.m[0][1];

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^{-1} = adj(m)/det(m), where
		//     adj(m)_{i,j} = c_{j,i}.
		m->m[0][0] = c00 * invDet;
		m->m[0][1] = c01 * invDet;
		m->m[0][2] = c02 * invDet;
		m->m[0][3] = c03 * invDet;
		m->m[1][0] = -(tempMatrix.m[1][0] * f0 - tempMatrix.m[2][0] * f1 + tempMatrix.m[3][0] * f2) * invDet;
		m->m[1][1] =  (tempMatrix.m[0][0] * f0 - tempMatrix.m[2][0] * f3 + tempMatrix.m[3][0] * f4) * invDet;
		m->m[1][2] = -(tempMatrix.m[0][0] * f1 - tempMatrix.m[1][0] * f3 + tempMatrix.m[3][0] * f5) * invDet;
		m->m[1][3] =  (tempMatrix.m[0][0] * f2 - tempMatrix.m[1][0] * f4 + tempMatrix.m[2][0] * f5) * invDet;
		// For these entries, we now expand along the bottom row of the 3x3 submatrix!
		m->m[2][0] =  (tempMatrix.m[1][3] * g0 - tempMatrix.m[2][3] * g1 + tempMatrix.m[3][3] * g2) * invDet;
		m->m[2][1] = -(tempMatrix.m[0][3] * g0 - tempMatrix.m[2][3] * g3 + tempMatrix.m[3][3] * g4) * invDet;
		m->m[2][2] =  (tempMatrix.m[0][3] * g1 - tempMatrix.m[1][3] * g3 + tempMatrix.m[3][3] * g5) * invDet;
		m->m[2][3] = -(tempMatrix.m[0][3] * g2 - tempMatrix.m[1][3] * g4 + tempMatrix.m[2][3] * g5) * invDet;
		m->m[3][0] = -(tempMatrix.m[1][2] * g0 - tempMatrix.m[2][2] * g1 + tempMatrix.m[3][2] * g2) * invDet;
		m->m[3][1] =  (tempMatrix.m[0][2] * g0 - tempMatrix.m[2][2] * g3 + tempMatrix.m[3][2] * g4) * invDet;
		m->m[3][2] = -(tempMatrix.m[0][2] * g1 - tempMatrix.m[1][2] * g3 + tempMatrix.m[3][2] * g5) * invDet;
		m->m[3][3] =  (tempMatrix.m[0][2] * g2 - tempMatrix.m[1][2] * g4 + tempMatrix.m[2][2] * g5) * invDet;
	}
}

// Invert a matrix and store the result in "out"!
void mat4InvertOut(const mat4 m, mat4 *const restrict out){
	// We need to use these values twice, but we only need to calculate them once.
	// These values represent 2x2 minors on the lower half of our matrix.
	const float f0 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3];
	const float f1 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3];
	const float f2 = m.m[1][2] * m.m[2][3] - m.m[2][2] * m.m[1][3];
	const float f3 = m.m[0][2] * m.m[3][3] - m.m[3][2] * m.m[0][3];
	const float f4 = m.m[0][2] * m.m[2][3] - m.m[2][2] * m.m[0][3];
	const float f5 = m.m[0][2] * m.m[1][3] - m.m[1][2] * m.m[0][3];
	// These values represent the cofactors c_{0,0}, c_{0,1}, c_{0,2} and c_{0,3}.
	const float c00 =   m.m[1][1] * f0 - m.m[2][1] * f1 + m.m[3][1] * f2;
	const float c01 = -(m.m[0][1] * f0 - m.m[2][1] * f3 + m.m[3][1] * f4);
	const float c02 =   m.m[0][1] * f1 - m.m[1][1] * f3 + m.m[3][1] * f5;
	const float c03 = -(m.m[0][1] * f2 - m.m[1][1] * f4 + m.m[2][1] * f5);
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * c00 + m.m[1][0] * c01 + m.m[2][0] * c02 + m.m[3][0] * c03;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		// These values represent 2x2 minors on the upper half of our matrix.
		const float g0 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1];
		const float g1 = m.m[1][0] * m.m[3][1] - m.m[3][0] * m.m[1][1];
		const float g2 = m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1];
		const float g3 = m.m[0][0] * m.m[3][1] - m.m[3][0] * m.m[0][1];
		const float g4 = m.m[0][0] * m.m[2][1] - m.m[2][0] * m.m[0][1];
		const float g5 = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^{-1} = adj(m)/det(m), where
		//     adj(m)_{i,j} = c_{j,i}.
		out->m[0][0] = c00 * invDet;
		out->m[0][1] = c01 * invDet;
		out->m[0][2] = c02 * invDet;
		out->m[0][3] = c03 * invDet;
		out->m[1][0] = -(m.m[1][0] * f0 - m.m[2][0] * f1 + m.m[3][0] * f2) * invDet;
		out->m[1][1] =  (m.m[0][0] * f0 - m.m[2][0] * f3 + m.m[3][0] * f4) * invDet;
		out->m[1][2] = -(m.m[0][0] * f1 - m.m[1][0] * f3 + m.m[3][0] * f5) * invDet;
		out->m[1][3] =  (m.m[0][0] * f2 - m.m[1][0] * f4 + m.m[2][0] * f5) * invDet;
		// For these entries, we now expand along the bottom row of the 3x3 submatrix!
		out->m[2][0] =  (m.m[1][3] * g0 - m.m[2][3] * g1 + m.m[3][3] * g2) * invDet;
		out->m[2][1] = -(m.m[0][3] * g0 - m.m[2][3] * g3 + m.m[3][3] * g4) * invDet;
		out->m[2][2] =  (m.m[0][3] * g1 - m.m[1][3] * g3 + m.m[3][3] * g5) * invDet;
		out->m[2][3] = -(m.m[0][3] * g2 - m.m[1][3] * g4 + m.m[2][3] * g5) * invDet;
		out->m[3][0] = -(m.m[1][2] * g0 - m.m[2][2] * g1 + m.m[3][2] * g2) * invDet;
		out->m[3][1] =  (m.m[0][2] * g0 - m.m[2][2] * g3 + m.m[3][2] * g4) * invDet;
		out->m[3][2] = -(m.m[0][2] * g1 - m.m[1][2] * g3 + m.m[3][2] * g5) * invDet;
		out->m[3][3] =  (m.m[0][2] * g2 - m.m[1][2] * g4 + m.m[2][2] * g5) * invDet;
	}
}

// Invert a matrix!
mat4 mat4InvertC(const mat4 m){
	// We need to use these values twice, but we only need to calculate them once.
	// These values represent 2x2 minors on the lower half of our matrix.
	const float f0 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3];
	const float f1 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3];
	const float f2 = m.m[1][2] * m.m[2][3] - m.m[2][2] * m.m[1][3];
	const float f3 = m.m[0][2] * m.m[3][3] - m.m[3][2] * m.m[0][3];
	const float f4 = m.m[0][2] * m.m[2][3] - m.m[2][2] * m.m[0][3];
	const float f5 = m.m[0][2] * m.m[1][3] - m.m[1][2] * m.m[0][3];
	// These values represent the cofactors c_{0,0}, c_{0,1}, c_{0,2} and c_{0,3}.
	const float c00 =   m.m[1][1] * f0 - m.m[2][1] * f1 + m.m[3][1] * f2;
	const float c01 = -(m.m[0][1] * f0 - m.m[2][1] * f3 + m.m[3][1] * f4);
	const float c02 =   m.m[0][1] * f1 - m.m[1][1] * f3 + m.m[3][1] * f5;
	const float c03 = -(m.m[0][1] * f2 - m.m[1][1] * f4 + m.m[2][1] * f5);
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * c00 + m.m[1][0] * c01 + m.m[2][0] * c02 + m.m[3][0] * c03;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		// These values represent 2x2 minors on the upper half of our matrix.
		const float g0 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1];
		const float g1 = m.m[1][0] * m.m[3][1] - m.m[3][0] * m.m[1][1];
		const float g2 = m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1];
		const float g3 = m.m[0][0] * m.m[3][1] - m.m[3][0] * m.m[0][1];
		const float g4 = m.m[0][0] * m.m[2][1] - m.m[2][0] * m.m[0][1];
		const float g5 = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];
		mat4 out;

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^{-1} = adj(m)/det(m), where
		//     adj(m)_{i,j} = c_{j,i}.
		out.m[0][0] = c00 * invDet;
		out.m[0][1] = c01 * invDet;
		out.m[0][2] = c02 * invDet;
		out.m[0][3] = c03 * invDet;
		out.m[1][0] = -(m.m[1][0] * f0 - m.m[2][0] * f1 + m.m[3][0] * f2) * invDet;
		out.m[1][1] =  (m.m[0][0] * f0 - m.m[2][0] * f3 + m.m[3][0] * f4) * invDet;
		out.m[1][2] = -(m.m[0][0] * f1 - m.m[1][0] * f3 + m.m[3][0] * f5) * invDet;
		out.m[1][3] =  (m.m[0][0] * f2 - m.m[1][0] * f4 + m.m[2][0] * f5) * invDet;
		// For these entries, we now expand along the bottom row of the 3x3 submatrix!
		out.m[2][0] =  (m.m[1][3] * g0 - m.m[2][3] * g1 + m.m[3][3] * g2) * invDet;
		out.m[2][1] = -(m.m[0][3] * g0 - m.m[2][3] * g3 + m.m[3][3] * g4) * invDet;
		out.m[2][2] =  (m.m[0][3] * g1 - m.m[1][3] * g3 + m.m[3][3] * g5) * invDet;
		out.m[2][3] = -(m.m[0][3] * g2 - m.m[1][3] * g4 + m.m[2][3] * g5) * invDet;
		out.m[3][0] = -(m.m[1][2] * g0 - m.m[2][2] * g1 + m.m[3][2] * g2) * invDet;
		out.m[3][1] =  (m.m[0][2] * g0 - m.m[2][2] * g3 + m.m[3][2] * g4) * invDet;
		out.m[3][2] = -(m.m[0][2] * g1 - m.m[1][2] * g3 + m.m[3][2] * g5) * invDet;
		out.m[3][3] =  (m.m[0][2] * g2 - m.m[1][2] * g4 + m.m[2][2] * g5) * invDet;

		return(out);
	}

	// If we couldn't invert the matrix, return the original one!
	return(m);
}

// Invert a matrix and return whether or not we were successful!
return_t mat4CanInvert(mat4 *const restrict m){
	const mat4 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	// These values represent 2x2 minors on the lower half of our matrix.
	const float f0 = tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[3][2] * tempMatrix.m[2][3];
	const float f1 = tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[3][2] * tempMatrix.m[1][3];
	const float f2 = tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[2][2] * tempMatrix.m[1][3];
	const float f3 = tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[3][2] * tempMatrix.m[0][3];
	const float f4 = tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[2][2] * tempMatrix.m[0][3];
	const float f5 = tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[1][2] * tempMatrix.m[0][3];
	// These values represent the cofactors c_{0,0}, c_{0,1}, c_{0,2} and c_{0,3}.
	const float c00 =   tempMatrix.m[1][1] * f0 - tempMatrix.m[2][1] * f1 + tempMatrix.m[3][1] * f2;
	const float c01 = -(tempMatrix.m[0][1] * f0 - tempMatrix.m[2][1] * f3 + tempMatrix.m[3][1] * f4);
	const float c02 =   tempMatrix.m[0][1] * f1 - tempMatrix.m[1][1] * f3 + tempMatrix.m[3][1] * f5;
	const float c03 = -(tempMatrix.m[0][1] * f2 - tempMatrix.m[1][1] * f4 + tempMatrix.m[2][1] * f5);
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * c00 + tempMatrix.m[1][0] * c01 + tempMatrix.m[2][0] * c02 + tempMatrix.m[3][0] * c03;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		// These values represent 2x2 minors on the upper half of our matrix.
		const float g0 = tempMatrix.m[2][0] * tempMatrix.m[3][1] - tempMatrix.m[3][0] * tempMatrix.m[2][1];
		const float g1 = tempMatrix.m[1][0] * tempMatrix.m[3][1] - tempMatrix.m[3][0] * tempMatrix.m[1][1];
		const float g2 = tempMatrix.m[1][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[1][1];
		const float g3 = tempMatrix.m[0][0] * tempMatrix.m[3][1] - tempMatrix.m[3][0] * tempMatrix.m[0][1];
		const float g4 = tempMatrix.m[0][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[0][1];
		const float g5 = tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[1][0] * tempMatrix.m[0][1];

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^{-1} = adj(m)/det(m), where
		//     adj(m)_{i,j} = c_{j,i}.
		m->m[0][0] = c00 * invDet;
		m->m[0][1] = c01 * invDet;
		m->m[0][2] = c02 * invDet;
		m->m[0][3] = c03 * invDet;
		m->m[1][0] = -(tempMatrix.m[1][0] * f0 - tempMatrix.m[2][0] * f1 + tempMatrix.m[3][0] * f2) * invDet;
		m->m[1][1] =  (tempMatrix.m[0][0] * f0 - tempMatrix.m[2][0] * f3 + tempMatrix.m[3][0] * f4) * invDet;
		m->m[1][2] = -(tempMatrix.m[0][0] * f1 - tempMatrix.m[1][0] * f3 + tempMatrix.m[3][0] * f5) * invDet;
		m->m[1][3] =  (tempMatrix.m[0][0] * f2 - tempMatrix.m[1][0] * f4 + tempMatrix.m[2][0] * f5) * invDet;
		// For these entries, we now expand along the bottom row of the 3x3 submatrix!
		m->m[2][0] =  (tempMatrix.m[1][3] * g0 - tempMatrix.m[2][3] * g1 + tempMatrix.m[3][3] * g2) * invDet;
		m->m[2][1] = -(tempMatrix.m[0][3] * g0 - tempMatrix.m[2][3] * g3 + tempMatrix.m[3][3] * g4) * invDet;
		m->m[2][2] =  (tempMatrix.m[0][3] * g1 - tempMatrix.m[1][3] * g3 + tempMatrix.m[3][3] * g5) * invDet;
		m->m[2][3] = -(tempMatrix.m[0][3] * g2 - tempMatrix.m[1][3] * g4 + tempMatrix.m[2][3] * g5) * invDet;
		m->m[3][0] = -(tempMatrix.m[1][2] * g0 - tempMatrix.m[2][2] * g1 + tempMatrix.m[3][2] * g2) * invDet;
		m->m[3][1] =  (tempMatrix.m[0][2] * g0 - tempMatrix.m[2][2] * g3 + tempMatrix.m[3][2] * g4) * invDet;
		m->m[3][2] = -(tempMatrix.m[0][2] * g1 - tempMatrix.m[1][2] * g3 + tempMatrix.m[3][2] * g5) * invDet;
		m->m[3][3] =  (tempMatrix.m[0][2] * g2 - tempMatrix.m[1][2] * g4 + tempMatrix.m[2][2] * g5) * invDet;


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
	// These values represent 2x2 minors on the lower half of our matrix.
	const float f0 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3];
	const float f1 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3];
	const float f2 = m.m[1][2] * m.m[2][3] - m.m[2][2] * m.m[1][3];
	const float f3 = m.m[0][2] * m.m[3][3] - m.m[3][2] * m.m[0][3];
	const float f4 = m.m[0][2] * m.m[2][3] - m.m[2][2] * m.m[0][3];
	const float f5 = m.m[0][2] * m.m[1][3] - m.m[1][2] * m.m[0][3];
	// These values represent the cofactors c_{0,0}, c_{0,1}, c_{0,2} and c_{0,3}.
	const float c00 =   m.m[1][1] * f0 - m.m[2][1] * f1 + m.m[3][1] * f2;
	const float c01 = -(m.m[0][1] * f0 - m.m[2][1] * f3 + m.m[3][1] * f4);
	const float c02 =   m.m[0][1] * f1 - m.m[1][1] * f3 + m.m[3][1] * f5;
	const float c03 = -(m.m[0][1] * f2 - m.m[1][1] * f4 + m.m[2][1] * f5);
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * c00 + m.m[1][0] * c01 + m.m[2][0] * c02 + m.m[3][0] * c03;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		// These values represent 2x2 minors on the upper half of our matrix.
		const float g0 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1];
		const float g1 = m.m[1][0] * m.m[3][1] - m.m[3][0] * m.m[1][1];
		const float g2 = m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1];
		const float g3 = m.m[0][0] * m.m[3][1] - m.m[3][0] * m.m[0][1];
		const float g4 = m.m[0][0] * m.m[2][1] - m.m[2][0] * m.m[0][1];
		const float g5 = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^{-1} = adj(m)/det(m), where
		//     adj(m)_{i,j} = c_{j,i}.
		out->m[0][0] = c00 * invDet;
		out->m[0][1] = c01 * invDet;
		out->m[0][2] = c02 * invDet;
		out->m[0][3] = c03 * invDet;
		out->m[1][0] = -(m.m[1][0] * f0 - m.m[2][0] * f1 + m.m[3][0] * f2) * invDet;
		out->m[1][1] =  (m.m[0][0] * f0 - m.m[2][0] * f3 + m.m[3][0] * f4) * invDet;
		out->m[1][2] = -(m.m[0][0] * f1 - m.m[1][0] * f3 + m.m[3][0] * f5) * invDet;
		out->m[1][3] =  (m.m[0][0] * f2 - m.m[1][0] * f4 + m.m[2][0] * f5) * invDet;
		// For these entries, we now expand along the bottom row of the 3x3 submatrix!
		out->m[2][0] =  (m.m[1][3] * g0 - m.m[2][3] * g1 + m.m[3][3] * g2) * invDet;
		out->m[2][1] = -(m.m[0][3] * g0 - m.m[2][3] * g3 + m.m[3][3] * g4) * invDet;
		out->m[2][2] =  (m.m[0][3] * g1 - m.m[1][3] * g3 + m.m[3][3] * g5) * invDet;
		out->m[2][3] = -(m.m[0][3] * g2 - m.m[1][3] * g4 + m.m[2][3] * g5) * invDet;
		out->m[3][0] = -(m.m[1][2] * g0 - m.m[2][2] * g1 + m.m[3][2] * g2) * invDet;
		out->m[3][1] =  (m.m[0][2] * g0 - m.m[2][2] * g3 + m.m[3][2] * g4) * invDet;
		out->m[3][2] = -(m.m[0][2] * g1 - m.m[1][2] * g3 + m.m[3][2] * g5) * invDet;
		out->m[3][3] =  (m.m[0][2] * g2 - m.m[1][2] * g4 + m.m[2][2] * g5) * invDet;


		return(1);
	}


	return(0);
}


/*
** Initialize a view matrix from a position and a rotation!
** Recall that a view matrix can be generally written as
**
** V = (TR)^{-1},
**
** where R is a rotation to whatever we want to look at and
** T is a translation to the camera's position.
*/
void mat4View(
	mat4 *const restrict m,
	const vec3 *const restrict pos,
	const mat3 *const restrict rot
){

	// We take the transpose of the rotation to invert it.
	m->m[0][0] = rot->m[0][0];
	m->m[0][1] = rot->m[1][0];
	m->m[0][2] = rot->m[2][0];
	m->m[0][3] = 0.f;

	m->m[1][0] = rot->m[0][1];
	m->m[1][1] = rot->m[1][1];
	m->m[1][2] = rot->m[2][1];
	m->m[1][3] = 0.f;

	m->m[2][0] = rot->m[0][2];
	m->m[2][1] = rot->m[1][2];
	m->m[2][2] = rot->m[2][2];
	m->m[2][3] = 0.f;

	// Our matrices are column-major, so the rows are the basis vectors.
	// We negate the dot products to effectively invert the translation.
	m->m[3][0] = -vec3DotVec3((vec3 *)rot->m[0], pos);
	m->m[3][1] = -vec3DotVec3((vec3 *)rot->m[1], pos);
	m->m[3][2] = -vec3DotVec3((vec3 *)rot->m[2], pos);
	m->m[3][3] = 1.f;
}

/*
** Initialize a view matrix from a position and a rotation!
** Recall that a view matrix can be generally written as
**
** V = (TR)^{-1},
**
** where R is a rotation to whatever we want to look at and
** T is a translation to the camera's position.
*/
mat4 mat4ViewC(const vec3 pos, const mat3 rot){
	const mat4 m = {
		// We take the transpose of the rotation to invert it.
		.m[0][0] = rot.m[0][0], .m[0][1] = rot.m[1][0], .m[0][2] = rot.m[2][0], .m[0][3] = 0.f,
		.m[1][0] = rot.m[0][1], .m[1][1] = rot.m[1][1], .m[1][2] = rot.m[2][1], .m[1][3] = 0.f,
		.m[2][0] = rot.m[0][2], .m[2][1] = rot.m[1][2], .m[2][2] = rot.m[2][2], .m[2][3] = 0.f,
		// Our matrices are column-major, so the rows are the basis vectors.
		// We negate the dot products to effectively invert the translation.
		.m[3][0] = -vec3DotVec3C(*((vec3 *)rot.m[0]), pos),
		.m[3][1] = -vec3DotVec3C(*((vec3 *)rot.m[1]), pos),
		.m[3][2] = -vec3DotVec3C(*((vec3 *)rot.m[2]), pos),
		.m[3][3] = 1.f
	};
	return(m);
}

/*
** Initialize a view matrix from a position and a rotation!
** Recall that a view matrix can be generally written as
**
** V = (TR)^{-1},
**
** where R is a rotation to whatever we want to look at and
** T is a translation to the camera's position.
*/
void mat4ViewQuat(
	mat4 *const restrict m,
	const vec3 *const restrict pos,
	const quat *const restrict q
){

	const float xx = q->x*q->x;
	const float yy = q->y*q->y;
	const float zz = q->z*q->z;
	float temp1;
	float temp2;

	m->m[0][0] = 1.f - 2.f*(yy + zz);
	m->m[1][1] = 1.f - 2.f*(xx + zz);
	m->m[2][2] = 1.f - 2.f*(xx + yy);

	// We want the conjugate quaternion,
	// hence the negative signs here.
	temp1 =  q->x*q->y;
	temp2 = -q->z*q->w;
	m->m[0][1] = 2.f*(temp1 + temp2);
	m->m[1][0] = 2.f*(temp1 - temp2);

	temp1 =  q->x*q->z;
	temp2 = -q->y*q->w;
	m->m[0][2] = 2.f*(temp1 - temp2);
	m->m[2][0] = 2.f*(temp1 + temp2);

	temp1 =  q->y*q->z;
	temp2 = -q->x*q->w;
	m->m[1][2] = 2.f*(temp1 + temp2);
	m->m[2][1] = 2.f*(temp1 - temp2);

	m->m[0][3] = 0.f;
	m->m[1][3] = 0.f;
	m->m[2][3] = 0.f;

	// Our matrices are column-major, so the rows are the basis vectors.
	// We negate the dot products to effectively invert the translation.
	m->m[3][0] = -vec3DotVec3((vec3 *)m->m[0], pos);
	m->m[3][1] = -vec3DotVec3((vec3 *)m->m[1], pos);
	m->m[3][2] = -vec3DotVec3((vec3 *)m->m[2], pos);
	m->m[3][3] = 1.f;
}

/*
** Initialize a view matrix from a position and a rotation!
** Recall that a view matrix can be generally written as
**
** V = (TR)^{-1},
**
** where R is a rotation to whatever we want to look at and
** T is a translation to the camera's position.
*/
mat4 mat4ViewQuatC(const vec3 pos, const quat q){
	const float xx = q.x*q.x;
	const float yy = q.y*q.y;
	const float zz = q.z*q.z;
	float temp1;
	float temp2;
	mat4 out;

	out.m[0][0] = 1.f - 2.f*(yy + zz);
	out.m[1][1] = 1.f - 2.f*(xx + zz);
	out.m[2][2] = 1.f - 2.f*(xx + yy);

	// We want the conjugate quaternion,
	// hence the negative signs here.
	temp1 =  q.x*q.y;
	temp2 = -q.z*q.w;
	out.m[0][1] = 2.f*(temp1 + temp2);
	out.m[1][0] = 2.f*(temp1 - temp2);

	temp1 =  q.x*q.z;
	temp2 = -q.y*q.w;
	out.m[0][2] = 2.f*(temp1 - temp2);
	out.m[2][0] = 2.f*(temp1 + temp2);

	temp1 =  q.y*q.z;
	temp2 = -q.x*q.w;
	out.m[1][2] = 2.f*(temp1 + temp2);
	out.m[2][1] = 2.f*(temp1 - temp2);

	out.m[0][3] = 0.f;
	out.m[1][3] = 0.f;
	out.m[2][3] = 0.f;

	out.m[3][0] = -vec3DotVec3C(*((vec3 *)out.m[0]), pos);
	out.m[3][1] = -vec3DotVec3C(*((vec3 *)out.m[1]), pos);
	out.m[3][2] = -vec3DotVec3C(*((vec3 *)out.m[2]), pos);
	out.m[3][3] = 1.f;

	return(out);
}

// Generate a look-at matrix!
void mat4ViewLookAt(
	mat4 *const restrict m,
	const vec3 *const restrict eye, const vec3 *const restrict target,
	const vec3 *const restrict worldUp
){

	vec3 right, up, forward;
	// Get the forward vector!
	vec3SubtractVec3Out(eye, target, &forward);
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
mat4 mat4ViewLookAtC(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3C(eye, target));
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
** Reflect the matrix "m" by the plane specified.
** We assume that the plane's normal component is unit length.
** This changes the handedness of our coordinate system!
*/
void mat4ViewReflect(mat4 *const restrict m, const vec4 *const restrict plane){
	mat4 reflect;
	mat4InitReflect(&reflect, plane);
	// If M is the mirror matrix and V = (TR)^{-1} is the
	// old view matrix, the new view matrix is given by
	//     V' = (MTR)^{-1} = VM.
	// Here, we use the fact that reflections are involutory.
	mat4MultiplyMat4P1(m, reflect);
}

/*
** Reflect a matrix by a plane and store the result in "out".
** We assume that the plane's normal component is unit length.
** This changes the handedness of our coordinate system!
*/
void mat4ViewReflectOut(
	const mat4 *const restrict m,
	const vec4 *const restrict plane,
	mat4 *const restrict out
){

	mat4 reflect;
	mat4InitReflect(&reflect, plane);
	// If M is the mirror matrix and V = (TR)^{-1} is the
	// old view matrix, the new view matrix is given by
	//     V' = (MTR)^{-1} = VM.
	// Here, we use the fact that reflections are involutory.
	mat4MultiplyMat4Out(*m, reflect, out);
}

/*
** Return the result of "m" reflected by the plane specified.
** We assume that the plane's normal component is unit length.
** This changes the handedness of our coordinate system!
*/
mat4 mat4ViewReflectC(const mat4 m, const vec4 plane){
	// If M is the mirror matrix and V = (TR)^{-1} is the
	// old view matrix, the new view matrix is given by
	//     V' = (MTR)^{-1} = VM.
	// Here, we use the fact that reflections are involutory.
	return(mat4MultiplyMat4C(m, mat4InitReflectC(plane)));
}


// Generate an orthographic matrix!
void mat4Orthographic(
	mat4 *const restrict m,
	const float right, const float left,
	const float top, const float bottom,
	const float near, const float far
){

	const float widthScale  = 1.f/(right - left);
	const float heightScale = 1.f/(top - bottom);
	const float depthScale  = 1.f/(near - far);

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
	m->m[3][2] = ((near + far) * depthScale);
	m->m[3][3] = 1.f;
}

// Generate an orthographic matrix!
mat4 mat4OrthographicC(
	const float right, const float left,
	const float top, const float bottom,
	const float near, const float far
){

	const float widthScale  = 1.f/(right - left);
	const float heightScale = 1.f/(top - bottom);
	const float depthScale  = 1.f/(near - far);

	const mat4 m = {
		.m[0][0] = 2.f*widthScale,                 .m[0][1] = 0.f,                             .m[0][2] = 0.f,                       .m[0][3] = 0.f,
		.m[1][0] = 0.f,                            .m[1][1] = 2.f*heightScale,                 .m[1][2] = 0.f,                       .m[1][3] = 0.f,
		.m[2][0] = 0.f,                            .m[2][1] = 0.f,                             .m[2][2] = 2.f*depthScale,            .m[2][3] = 0.f,
		.m[3][0] = -((right + left) * widthScale), .m[3][1] = -((top + bottom) * heightScale), .m[3][2] = (near + far) * depthScale, .m[3][3] = 1.f
	};

	return(m);
}

// Generate a perspective matrix!
void mat4Frustum(
	mat4 *const restrict m,
	const float left, const float right,
	const float bottom, const float top,
	const float near, const float far
){

	const float widthScale  = 1.f/(right - left);
	const float heightScale = 1.f/(top - bottom);
	const float depthScale  = 1.f/(near - far);

	m->m[0][0] = 2.f * near * widthScale;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 2.f * near * heightScale;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = (right + left) * widthScale;
	m->m[2][1] = (top + bottom) * heightScale;
	m->m[2][2] = (near + far) * depthScale;
	m->m[2][3] = -1.f,

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 2.f * far * near * depthScale;
	m->m[3][3] = 0.f;
}

// Generate a perspective matrix!
mat4 mat4FrustumC(
	const float left, const float right,
	const float bottom, const float top,
	const float near, const float far
){

	const float widthScale  = 1.f/(right - left);
	const float heightScale = 1.f/(top - bottom);
	const float depthScale  = 1.f/(near - far);

	const mat4 m = {
		.m[0][0] = 2.f * near * widthScale,     .m[0][1] = 0.f,                          .m[0][2] = 0.f,                           .m[0][3] =  0.f,
		.m[1][0] = 0.f,                         .m[1][1] = 2.f * near * heightScale,     .m[1][2] = 0.f,                           .m[1][3] =  0.f,
		.m[2][0] = (right + left) * widthScale, .m[2][1] = (top + bottom) * heightScale, .m[2][2] = (near + far) * depthScale,     .m[2][3] = -1.f,
		.m[3][0] = 0.f,                         .m[3][1] = 0.f,                          .m[3][2] = 2.f * far * near * depthScale, .m[3][3] =  0.f
	};

	return(m);
}

// Generate a perspective matrix from the field of view and aspect ratio!
void mat4Perspective(
	mat4 *const restrict m,
	const float fov, const float aspectRatio,
	const float near, const float far
){

	const float invScale = 1.f/(aspectRatio * tanf(fov * 0.5f));
	const float depthScale = 1.f/(near - far);

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
	m->m[2][2] = (far + near) * depthScale;
	m->m[2][3] = -1.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 2.f * far * near * depthScale;
	m->m[3][3] = 0.f;
}

// Generate a perspective matrix!
mat4 mat4PerspectiveC(
	const float fov, const float aspectRatio,
	const float near, const float far
){

	const float invScale = 1.f/(aspectRatio * tanf(fov * 0.5f));
	const float depthScale = 1.f/(near - far);

	const mat4 m = {
		.m[0][0] = invScale, .m[0][1] = 0.f,                    .m[0][2] = 0.f,                           .m[0][3] =  0.f,
		.m[1][0] = 0.f,      .m[1][1] = aspectRatio * invScale, .m[1][2] = 0.f,                           .m[1][3] =  0.f,
		.m[2][0] = 0.f,      .m[2][1] = 0.f,                    .m[2][2] = (far + near) * depthScale,     .m[2][3] = -1.f,
		.m[3][0] = 0.f,      .m[3][1] = 0.f,                    .m[3][2] = 2.f * far * near * depthScale, .m[3][3] =  0.f
	};

	return(m);
}


/*
** Convert a 4x4 matrix to a quaternion and store the result in "out"!
** For this to work, we assume that "m" is a special orthogonal matrix.
** Implementation by Mike Day from Converting a Rotation Matrix to a Quaternion.
*/
void mat4ToQuat(const mat4 *const restrict m, quat *const restrict out){
	float t;
	if(m->m[2][2] < 0.f){
		if(m->m[0][0] > m->m[1][1]){
			t = m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f;
			quatInitSet(out, m->m[1][2] - m->m[2][1], t, m->m[0][1] + m->m[1][0], m->m[2][0] + m->m[0][2]);
		}else{
			t = -m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f;
			quatInitSet(out, m->m[2][0] - m->m[0][2], m->m[0][1] + m->m[1][0], t, m->m[1][2] + m->m[2][1]);
		}
	}else{
		if(m->m[0][0] < -m->m[1][1]){
			t = -m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f;
			quatInitSet(out, m->m[0][1] - m->m[1][0], m->m[2][0] + m->m[0][2], m->m[1][2] + m->m[2][1], t);
		}else{
			t = m->m[0][0] + m->m[1][1] + m->m[2][2] + 1.f;
			quatInitSet(out, t, m->m[1][2] - m->m[2][1], m->m[2][0] - m->m[0][2], m->m[0][1] - m->m[1][0]);
		}
	}
	quatMultiplyS(out, 0.5f*invSqrtFast(t));
}

/*
** Convert a 4x4 pure rotation matrix to a quaternion!
** Implementation by Mike Day from Converting a Rotation Matrix to a Quaternion.
*/
quat mat4ToQuatC(const mat4 m){
	float t;
	quat q;
	if(m.m[2][2] < 0.f){
		if(m.m[0][0] > m.m[1][1]){
			t = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f;
			q = quatInitSetC(m.m[1][2] - m.m[2][1], t, m.m[0][1] + m.m[1][0], m.m[2][0] + m.m[0][2]);
		}else{
			t = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f;
			q = quatInitSetC(m.m[2][0] - m.m[0][2], m.m[0][1] + m.m[1][0], t, m.m[1][2] + m.m[2][1]);
		}
	}else{
		if(m.m[0][0] < -m.m[1][1]){
			t = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f;
			q = quatInitSetC(m.m[0][1] - m.m[1][0], m.m[2][0] + m.m[0][2], m.m[1][2] + m.m[2][1], t);
		}else{
			t = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f;
			q = quatInitSetC(t, m.m[1][2] - m.m[2][1], m.m[2][0] - m.m[0][2], m.m[0][1] - m.m[1][0]);
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

/*
** Convert a quaternion to a 4x4 matrix and store the result in "out"!
** This assumes that "q" is normalized.
*/
void quatToMat4(const quat *const restrict q, mat4 *const restrict out){
	const float xx = q->x*q->x;
	const float yy = q->y*q->y;
	const float zz = q->z*q->z;
	float temp1;
	float temp2;

	out->m[0][0] = 1.f - 2.f*(yy + zz);
	out->m[1][1] = 1.f - 2.f*(xx + zz);
	out->m[2][2] = 1.f - 2.f*(xx + yy);

	temp1 = q->x*q->y;
	temp2 = q->z*q->w;
	out->m[0][1] = 2.f*(temp1 + temp2);
	out->m[1][0] = 2.f*(temp1 - temp2);

	temp1 = q->x*q->z;
	temp2 = q->y*q->w;
	out->m[0][2] = 2.f*(temp1 - temp2);
	out->m[2][0] = 2.f*(temp1 + temp2);

	temp1 = q->y*q->z;
	temp2 = q->x*q->w;
	out->m[1][2] = 2.f*(temp1 + temp2);
	out->m[2][1] = 2.f*(temp1 - temp2);

	out->m[0][3] = 0.f;
	out->m[1][3] = 0.f;
	out->m[2][3] = 0.f;

	out->m[3][0] = 0.f;
	out->m[3][1] = 0.f;
	out->m[3][2] = 0.f;
	out->m[3][3] = 1.f;
}

/*
** Convert a quaternion to a 4x4 matrix!
** This assumes that "q" is normalized.
*/
mat4 quatToMat4C(const quat q){
	const float xx = q.x*q.x;
	const float yy = q.y*q.y;
	const float zz = q.z*q.z;
	float temp1;
	float temp2;
	mat4 out;

	out.m[0][0] = 1.f - 2.f*(yy + zz);
	out.m[1][1] = 1.f - 2.f*(xx + zz);
	out.m[2][2] = 1.f - 2.f*(xx + yy);

	temp1 = q.x*q.y;
	temp2 = q.z*q.w;
	out.m[0][1] = 2.f*(temp1 + temp2);
	out.m[1][0] = 2.f*(temp1 - temp2);

	temp1 = q.x*q.z;
	temp2 = q.y*q.w;
	out.m[0][2] = 2.f*(temp1 - temp2);
	out.m[2][0] = 2.f*(temp1 + temp2);

	temp1 = q.y*q.z;
	temp2 = q.x*q.w;
	out.m[1][2] = 2.f*(temp1 + temp2);
	out.m[2][1] = 2.f*(temp1 - temp2);

	out.m[0][3] = 0.f;
	out.m[1][3] = 0.f;
	out.m[2][3] = 0.f;

	out.m[3][0] = 0.f;
	out.m[3][1] = 0.f;
	out.m[3][2] = 0.f;
	out.m[3][3] = 1.f;

	return(out);
}


void mat4Lerp(const mat4 *const restrict m1, const mat4 *const restrict m2, const float time, mat4 *const restrict out){
	out->m[0][0] = floatLerpFast(m1->m[0][0], m2->m[0][0], time);
	out->m[0][1] = floatLerpFast(m1->m[0][1], m2->m[0][1], time);
	out->m[0][2] = floatLerpFast(m1->m[0][2], m2->m[0][2], time);
	out->m[0][3] = floatLerpFast(m1->m[0][3], m2->m[0][3], time);
	out->m[1][0] = floatLerpFast(m1->m[1][0], m2->m[1][0], time);
	out->m[1][1] = floatLerpFast(m1->m[1][1], m2->m[1][1], time);
	out->m[1][2] = floatLerpFast(m1->m[1][2], m2->m[1][2], time);
	out->m[1][3] = floatLerpFast(m1->m[1][3], m2->m[1][3], time);
	out->m[2][0] = floatLerpFast(m1->m[2][0], m2->m[2][0], time);
	out->m[2][1] = floatLerpFast(m1->m[2][1], m2->m[2][1], time);
	out->m[2][2] = floatLerpFast(m1->m[2][2], m2->m[2][2], time);
	out->m[2][3] = floatLerpFast(m1->m[2][3], m2->m[2][3], time);
	out->m[3][0] = floatLerpFast(m1->m[3][0], m2->m[3][0], time);
	out->m[3][1] = floatLerpFast(m1->m[3][1], m2->m[3][1], time);
	out->m[3][2] = floatLerpFast(m1->m[3][2], m2->m[3][2], time);
	out->m[3][3] = floatLerpFast(m1->m[3][3], m2->m[3][3], time);
}

mat4 mat4LerpC(mat4 m1, const mat4 m2, const float time){
	m1.m[0][0] = floatLerpFast(m1.m[0][0], m2.m[0][0], time);
	m1.m[0][1] = floatLerpFast(m1.m[0][1], m2.m[0][1], time);
	m1.m[0][2] = floatLerpFast(m1.m[0][2], m2.m[0][2], time);
	m1.m[0][3] = floatLerpFast(m1.m[0][3], m2.m[0][3], time);
	m1.m[1][0] = floatLerpFast(m1.m[1][0], m2.m[1][0], time);
	m1.m[1][1] = floatLerpFast(m1.m[1][1], m2.m[1][1], time);
	m1.m[1][2] = floatLerpFast(m1.m[1][2], m2.m[1][2], time);
	m1.m[1][3] = floatLerpFast(m1.m[1][3], m2.m[1][3], time);
	m1.m[2][0] = floatLerpFast(m1.m[2][0], m2.m[2][0], time);
	m1.m[2][1] = floatLerpFast(m1.m[2][1], m2.m[2][1], time);
	m1.m[2][2] = floatLerpFast(m1.m[2][2], m2.m[2][2], time);
	m1.m[2][3] = floatLerpFast(m1.m[2][3], m2.m[2][3], time);
	m1.m[3][0] = floatLerpFast(m1.m[3][0], m2.m[3][0], time);
	m1.m[3][1] = floatLerpFast(m1.m[3][1], m2.m[3][1], time);
	m1.m[3][2] = floatLerpFast(m1.m[3][2], m2.m[3][2], time);
	m1.m[3][3] = floatLerpFast(m1.m[3][3], m2.m[3][3], time);

	return(m1);
}