#include "mat3x4.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


mat3x4 g_mat3x4Identity = {
	.m[0][0] = 1.f, .m[0][1] = 0.f, .m[0][2] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f, .m[1][2] = 0.f,
	.m[2][0] = 0.f, .m[2][1] = 0.f, .m[2][2] = 1.f,
	.m[3][0] = 0.f, .m[3][1] = 0.f, .m[3][2] = 0.f
};


// Initialize the matrix's values to 0!
void mat3x4InitZero(mat3x4 *const restrict m){
	memset(m, 0.f, sizeof(*m));
}

// Initialize the matrix's values to 0!
mat3x4 mat3x4InitZeroC(){
	mat3x4 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

// Initialize the matrix to an identity matrix!
void mat3x4InitIdentity(mat3x4 *const restrict m){
	*m = g_mat3x4Identity;
}

// Initialize the matrix to an identity matrix!
mat3x4 mat3x4InitIdentityC(){
	return(g_mat3x4Identity);
}

// Initialise a matrix to a translation matrix!
void mat3x4InitTranslate(mat3x4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = 1.f;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 1.f;
	m->m[1][2] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = 1.f;

	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
}

// Initialise a matrix to a translation matrix!
mat3x4 mat3x4InitTranslateC(const float x, const float y, const float z){
	const mat3x4 m = {
		.m[0][0] = 1.f,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = 1.f,
		.m[1][2] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = 1.f,

		.m[3][0] = x,
		.m[3][1] = y,
		.m[3][2] = z
	};

	return(m);
}

// Initialise a matrix to a translation matrix using a vec3!
void mat3x4InitTranslateVec3(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4InitTranslate(m, v->x, v->y, v->z);
}

// Initialise a matrix to a translation matrix using a vec3!
mat3x4 mat3x4InitTranslateVec3C(const vec3 v){
	return(mat3x4InitTranslateC(v.x, v.y, v.z));
}

// Initialize a matrix from XYZ Euler angles (in radians)!
void mat3x4InitEulerXYZ(mat3x4 *const restrict m, const float x, const float y, const float z){
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

	m->m[1][0] = sxsy*cz - cx*sz;
	m->m[1][1] = sxsy*sz + cx*cz;
	m->m[1][2] = sx*cy;

	m->m[2][0] = cxsy*cz + sx*sz;
	m->m[2][1] = cxsy*sz - sx*cz;
	m->m[2][2] = cx*cy;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Initialize a matrix from ZXY Euler angles (in radians)!
void mat3x4InitEulerZXY(mat3x4 *const restrict m, const float x, const float y, const float z){
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

	m->m[1][0] = sxsy*cz - cy*sz;
	m->m[1][1] = cx*cz;
	m->m[1][2] = sxcy*cz + sy*sz;

	m->m[2][0] = cx*sy;
	m->m[2][1] = -sx;
	m->m[2][2] = cx*cy;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Initialize a matrix from XYZ Euler angles (in radians)!
mat3x4 mat3x4InitEulerXYZC(const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;

	const mat3x4 m = {
		.m[0][0] = cy*cz,
		.m[0][1] = cy*sz,
		.m[0][2] = -sy,

		.m[1][0] = sxsy*cz - cx*sz,
		.m[1][1] = sxsy*sz + cx*cz,
		.m[1][2] = sx*cy,

		.m[2][0] = cxsy*cz + sx*sz,
		.m[2][1] = cxsy*sz - sx*cz,
		.m[2][2] = cx*cy,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};


	return(m);
}

// Initialize a matrix from ZXY Euler angles (in radians)!
mat3x4 mat3x4InitEulerZXYC(const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	const mat3x4 m = {
		.m[0][0] = sxsy*sz + cy*cz,
		.m[0][1] = cx*sz,
		.m[0][2] = sxcy*sz - sy*cz,

		.m[1][0] = sxsy*cz - cy*sz,
		.m[1][1] = cx*cz,
		.m[1][2] = sxcy*cz + sy*sz,

		.m[2][0] = cx*sy,
		.m[2][1] = -sx,
		.m[2][2] = cx*cy,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};


	return(m);
}

// Initialize a matrix from XYZ Euler angles (in radians)!
void mat3x4InitEulerVec3XYZ(mat3x4 *const restrict m, const vec3 *const restrict v){
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

	m->m[1][0] = sxsy*cz - cx*sz;
	m->m[1][1] = sxsy*sz + cx*cz;
	m->m[1][2] = sx*cy;

	m->m[2][0] = cxsy*cz + sx*sz;
	m->m[2][1] = cxsy*sz - sx*cz;
	m->m[2][2] = cx*cy;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Initialize a matrix from ZXY Euler angles (in radians)!
void mat3x4InitEulerVec3ZXY(mat3x4 *const restrict m, const vec3 *const restrict v){
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

	m->m[1][0] = sxsy*cz - cy*sz;
	m->m[1][1] = cx*cz;
	m->m[1][2] = sxcy*cz + sy*sz;

	m->m[2][0] = cx*sy;
	m->m[2][1] = -sx;
	m->m[2][2] = cx*cy;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Initialize a matrix from XYZ Euler angles (in radians)!
mat3x4 mat3x4InitEulerVec3XYZC(const vec3 v){
	const float cx = cosf(v.x);
	const float sx = sinf(v.x);
	const float cy = cosf(v.y);
	const float sy = sinf(v.y);
	const float cz = cosf(v.z);
	const float sz = sinf(v.z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;

	const mat3x4 m = {
		.m[0][0] = cy*cz,
		.m[0][1] = cy*sz,
		.m[0][2] = -sy,

		.m[1][0] = sxsy*cz - cx*sz,
		.m[1][1] = sxsy*sz + cx*cz,
		.m[1][2] = sx*cy,

		.m[2][0] = cxsy*cz + sx*sz,
		.m[2][1] = cxsy*sz - sx*cz,
		.m[2][2] = cx*cy,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};


	return(m);
}

// Initialize a matrix from ZXY Euler angles (in radians)!
mat3x4 mat3x4InitEulerVec3ZXYC(const vec3 v){
	const float cx = cosf(v.x);
	const float sx = sinf(v.x);
	const float cy = cosf(v.y);
	const float sy = sinf(v.y);
	const float cz = cosf(v.z);
	const float sz = sinf(v.z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	const mat3x4 m = {
		.m[0][0] = sxsy*sz + cy*cz,
		.m[0][1] = cx*sz,
		.m[0][2] = sxcy*sz - sy*cz,

		.m[1][0] = sxsy*cz - cy*sz,
		.m[1][1] = cx*cz,
		.m[1][2] = sxcy*cz + sy*sz,

		.m[2][0] = cx*sy,
		.m[2][1] = -sx,
		.m[2][2] = cx*cy,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};


	return(m);
}

// Initialize a matrix from an axis and an angle!
void mat3x4InitAxisAngle(mat3x4 *const restrict m, const vec4 *const restrict v){
	const float c = cosf(v->w);
	const float s = sinf(v->w);
	const float t = 1.f - c;
	vec3 normalAxis;
	vec3 tempAxis;

	// Normalize the axis!
	vec3NormalizeFast(v->x, v->y, v->z, &normalAxis);
	vec3MultiplySOut(&normalAxis, t, &tempAxis);

	// Convert the axis angle to a rotation matrix!
	m->m[0][0] = tempAxis.x * normalAxis.x + c;
	m->m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s;
	m->m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s;

	m->m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s;
	m->m[1][1] = tempAxis.y * normalAxis.y + c;
	m->m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s;

	m->m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s;
	m->m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s;
	m->m[2][2] = tempAxis.z * normalAxis.z + c;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Initialize a matrix from an axis and an angle!
mat3x4 mat3x4InitAxisAngleC(const vec4 v){
	const float c = cosf(v.w);
	const float s = sinf(v.w);
	const float t = 1.f - c;

	// Normalize the axis!
	const vec3 normalAxis = vec3NormalizeFastC(v.x, v.y, v.z);
	const vec3 tempAxis = vec3MultiplySC(normalAxis, t);

	// Convert the axis angle to a rotation matrix!
	const mat3x4 out = {
		.m[0][0] = tempAxis.x * normalAxis.x + c,
		.m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s,
		.m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s,

		.m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s,
		.m[1][1] = tempAxis.y * normalAxis.y + c,
		.m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s,

		.m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s,
		.m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s,
		.m[2][2] = tempAxis.z * normalAxis.z + c,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};

	return(out);
}

/*
** Initialise a matrix to a rotation matrix!
** This assumes that "q" is normalized.
*/
void mat3x4InitQuat(mat3x4 *const restrict m, const quat *const restrict q){
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

	m->m[1][0] = 2.f * (xy - zw);
	m->m[1][1] = 1.f - 2.f * (xx + zz);
	m->m[1][2] = 2.f * (yz + xw);

	m->m[2][0] = 2.f * (xz + yw);
	m->m[2][1] = 2.f * (yz - xw);
	m->m[2][2] = 1.f - 2.f * (xx + yy);

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

/*
** Initialise a matrix to a rotation matrix!
** This assumes that "q" is normalized.
*/
mat3x4 mat3x4InitQuatC(const quat q){
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
	const mat3x4 m = {
		.m[0][0] = 1.f - 2.f * (yy + zz),
		.m[0][1] = 2.f * (xy + zw),
		.m[0][2] = 2.f * (xz - yw),

		.m[1][0] = 2.f * (xy - zw),
		.m[1][1] = 1.f - 2.f * (xx + zz),
		.m[1][2] = 2.f * (yz + xw),

		.m[2][0] = 2.f * (xz + yw),
		.m[2][1] = 2.f * (yz - xw),
		.m[2][2] = 1.f - 2.f * (xx + yy),

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};


	return(m);
}

// Initialise a matrix to a scale matrix!
void mat3x4InitScale(mat3x4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = y;
	m->m[1][2] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = z;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Initialise a matrix to a scale matrix!
mat3x4 mat3x4InitScaleC(const float x, const float y, const float z){
	const mat3x4 m = {
		.m[0][0] = x,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = y,
		.m[1][2] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = z,

		.m[3][0] = 0.f,
		.m[3][1] = 0.f,
		.m[3][2] = 0.f
	};

	return(m);
}

// Initialise a matrix to a scale matrix using a vec3!
void mat3x4InitScaleVec3(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4InitScale(m, v->x, v->y, v->z);
}

// Initialise a matrix to a scale matrix using a vec3!
mat3x4 mat3x4InitScaleVec3C(const vec3 v){
	return(mat3x4InitScaleC(v.x, v.y, v.z));
}

// Given a quaternion and a vector, construct the shear matrix QVQ^T.
void mat3x4InitShearQuat(
	mat3x4 *const restrict m,
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
	cx = v->x*rotMatrix.m[0][1];
	cy = v->y*rotMatrix.m[1][1];
	cz = v->z*rotMatrix.m[2][1];
	m->m[1][0] = m->m[0][1];
	m->m[1][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m->m[1][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	m->m[2][0] = m->m[0][2];
	m->m[2][1] = m->m[1][2];
	m->m[2][2] = rotMatrix.m[0][2]*v->x*rotMatrix.m[0][2] +
	             rotMatrix.m[1][2]*v->y*rotMatrix.m[1][2] +
	             rotMatrix.m[2][2]*v->z*rotMatrix.m[2][2];
	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Given a quaternion and a vector, construct the shear matrix QVQ^T.
mat3x4 mat3x4InitShearQuatC(const vec3 v, const quat q){
	// Convert the quaternion Q to a rotation matrix!
	const mat3 rotMatrix =  mat3InitQuatC(q);

	float cx = v.x*rotMatrix.m[0][0];
	float cy = v.y*rotMatrix.m[1][0];
	float cz = v.z*rotMatrix.m[2][0];
	mat3x4 m;

	// Return m = QVQ^T, where Q is our rotation matrix and V is
	// the scale matrix with elements of v along its diagonal.
	m.m[0][0] = rotMatrix.m[0][0]*cx + rotMatrix.m[1][0]*cy + rotMatrix.m[2][0]*cz;
	m.m[0][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m.m[0][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	cx = v.x*rotMatrix.m[0][1];
	cy = v.y*rotMatrix.m[1][1];
	cz = v.z*rotMatrix.m[2][1];
	m.m[1][0] = m.m[0][1];
	m.m[1][1] = rotMatrix.m[0][1]*cx + rotMatrix.m[1][1]*cy + rotMatrix.m[2][1]*cz;
	m.m[1][2] = rotMatrix.m[0][2]*cx + rotMatrix.m[1][2]*cy + rotMatrix.m[2][2]*cz;
	m.m[2][0] = m.m[0][2];
	m.m[2][1] = m.m[1][2];
	m.m[2][2] = rotMatrix.m[0][2]*v.x*rotMatrix.m[0][2] +
	            rotMatrix.m[1][2]*v.y*rotMatrix.m[1][2] +
	            rotMatrix.m[2][2]*v.z*rotMatrix.m[2][2];
	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = 0.f;

	return(m);
}


// Add the matrix "m2" to "m1"!
void mat3x4AddMat3x4(mat3x4 *const restrict m1, const mat3x4 *const restrict m2){
	m1->m[0][0] += m2->m[0][0];
	m1->m[0][1] += m2->m[0][1];
	m1->m[0][2] += m2->m[0][2];

	m1->m[1][0] += m2->m[1][0];
	m1->m[1][1] += m2->m[1][1];
	m1->m[1][2] += m2->m[1][2];

	m1->m[2][0] += m2->m[2][0];
	m1->m[2][1] += m2->m[2][1];
	m1->m[2][2] += m2->m[2][2];

	m1->m[3][0] += m2->m[3][0];
	m1->m[3][1] += m2->m[3][1];
	m1->m[3][2] += m2->m[3][2];
}

// Add the matrix "m2" to "m1" and store the result in "out"!
void mat3x4AddMat3x4Out(
	const mat3x4 *const restrict m1, const mat3x4 *const restrict m2,
	mat3x4 *const restrict out
){

	out->m[0][0] = m1->m[0][0] + m2->m[0][0];
	out->m[0][1] = m1->m[0][1] + m2->m[0][1];
	out->m[0][2] = m1->m[0][2] + m2->m[0][2];

	out->m[1][0] = m1->m[1][0] + m2->m[1][0];
	out->m[1][1] = m1->m[1][1] + m2->m[1][1];
	out->m[1][2] = m1->m[1][2] + m2->m[1][2];

	out->m[2][0] = m1->m[2][0] + m2->m[2][0];
	out->m[2][1] = m1->m[2][1] + m2->m[2][1];
	out->m[2][2] = m1->m[2][2] + m2->m[2][2];

	out->m[3][0] = m1->m[3][0] + m2->m[3][0];
	out->m[3][1] = m1->m[3][1] + m2->m[3][1];
	out->m[3][2] = m1->m[3][2] + m2->m[3][2];
}

// Add the matrix "m2" to "m1" and return the result!
mat3x4 mat3x4AddMat3x4C(const mat3x4 m1, const mat3x4 m2){
	const mat3x4 out = {
		.m[0][0] = m1.m[0][0] + m2.m[0][0],
		.m[0][1] = m1.m[0][1] + m2.m[0][1],
		.m[0][2] = m1.m[0][2] + m2.m[0][2],

		.m[1][0] = m1.m[1][0] + m2.m[1][0],
		.m[1][1] = m1.m[1][1] + m2.m[1][1],
		.m[1][2] = m1.m[1][2] + m2.m[1][2],

		.m[2][0] = m1.m[2][0] + m2.m[2][0],
		.m[2][1] = m1.m[2][1] + m2.m[2][1],
		.m[2][2] = m1.m[2][2] + m2.m[2][2],

		.m[3][0] = m1.m[3][0] + m2.m[3][0],
		.m[3][1] = m1.m[3][1] + m2.m[3][1],
		.m[3][2] = m1.m[3][2] + m2.m[3][2]
	};

	return(out);
}


/*
** Left-multiply a 3x1 column vector by a matrix (m*v)!
** Note that this is really a 4x1 vector with '1' in the last component.
*/
void mat3x4MultiplyVec3(const mat3x4 *const restrict m, vec3 *const restrict v){
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
void mat3x4MultiplyVec3Out(
	const mat3x4 *const restrict m, const vec3 *const restrict v,
	vec3 *const restrict out
){

	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0];
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1];
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2];
}

/*
** Left-multiply a 3x1 column vector by a matrix (m*v) and return the result!
** Note that this is really a 4x1 vector with '1' in the last component.
** This assumes that "out" isn't "v".
*/
vec3 mat3x4MultiplyVec3C(const mat3x4 m, const vec3 v){
	const vec3 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0],
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1],
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2]
	};

	return(out);
}

// Right-multiply a 1x3 row vector by a matrix (v^T*m)!
void vec3MultiplyMat3x4(vec3 *const restrict v, const mat3x4 *const restrict m){
	const vec3 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1] + temp.z * m->m[0][2];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1] + temp.z * m->m[1][2];
	v->z = temp.x * m->m[2][0] + temp.y * m->m[2][1] + temp.z * m->m[2][2];
}

/*
** Right-multiply a 1x3 row vector by a matrix (v^T*m) and store the result in "out"!
** This assumes that "out" isn't "v".
*/
void vec3MultiplyMat3x4Out(
	const vec3 *const restrict v, const mat3x4 *const restrict m,
	vec3 *const restrict out
){

	out->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	out->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}

// Right-multiply a 1x3 row vector by a matrix (v^T*m) and return the result!
vec3 vec3MultiplyMat3x4C(const vec3 v, const mat3x4 m){
	const vec3 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
		.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]
	};

	return(out);
}

// Left-multiply a 4x1 column vector by a matrix (m*v)!
void mat3x4MultiplyVec4(const mat3x4 *const restrict m, vec4 *const restrict v){
	const vec4 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z;
	v->w = m->m[0][3] * temp.x + m->m[1][3] * temp.y + m->m[2][3] * temp.z + temp.w;
}

/*
** Left-multiply a 4x1 column vector by a matrix (m*v) and store the result in "out"!
** This assumes that "out" isn't "v".
*/
void mat3x4MultiplyVec4Out(
	const mat3x4 *const restrict m, const vec4 *const restrict v,
	vec4 *const restrict out
){

	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;
	out->w = m->m[0][3] * v->x + m->m[1][3] * v->y + m->m[2][3] * v->z + v->w;
}

// Left-multiply a 4x1 column vector by a matrix (m*v) and return the result!
vec4 mat3x4MultiplyVec4C(const mat3x4 m, const vec4 v){
	const vec4 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z,
		.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + v.w
	};

	return(out);
}

/*
** Right-multiply the 3x4 matrix "m1" by the 3x3 transformation matrix "m2" (m1*m2)!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat3x4MultiplyMat3(mat3x4 *const restrict m1, const mat3 m2){
	const mat3x4 tempMatrix1 = *m1;

	m1->m[0][0] = tempMatrix1.m[0][0] * m2.m[0][0] + tempMatrix1.m[1][0] * m2.m[0][1] +
	              tempMatrix1.m[2][0] * m2.m[0][2];
	m1->m[0][1] = tempMatrix1.m[0][1] * m2.m[0][0] + tempMatrix1.m[1][1] * m2.m[0][1] +
	              tempMatrix1.m[2][1] * m2.m[0][2];
	m1->m[0][2] = tempMatrix1.m[0][2] * m2.m[0][0] + tempMatrix1.m[1][2] * m2.m[0][1] +
	              tempMatrix1.m[2][2] * m2.m[0][2];

	m1->m[1][0] = tempMatrix1.m[0][0] * m2.m[1][0] + tempMatrix1.m[1][0] * m2.m[1][1] +
	              tempMatrix1.m[2][0] * m2.m[1][2];
	m1->m[1][1] = tempMatrix1.m[0][1] * m2.m[1][0] + tempMatrix1.m[1][1] * m2.m[1][1] +
	              tempMatrix1.m[2][1] * m2.m[1][2];
	m1->m[1][2] = tempMatrix1.m[0][2] * m2.m[1][0] + tempMatrix1.m[1][2] * m2.m[1][1] +
	              tempMatrix1.m[2][2] * m2.m[1][2];

	m1->m[2][0] = tempMatrix1.m[0][0] * m2.m[2][0] + tempMatrix1.m[1][0] * m2.m[2][1] +
	              tempMatrix1.m[2][0] * m2.m[2][2];
	m1->m[2][1] = tempMatrix1.m[0][1] * m2.m[2][0] + tempMatrix1.m[1][1] * m2.m[2][1] +
	              tempMatrix1.m[2][1] * m2.m[2][2];
	m1->m[2][2] = tempMatrix1.m[0][2] * m2.m[2][0] + tempMatrix1.m[1][2] * m2.m[2][1] +
	              tempMatrix1.m[2][2] * m2.m[2][2];
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x3 transformation matrix "m2" (m1*m2) and store the result in "out"!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
void mat3x4MultiplyMat3Out(const mat3x4 m1, const mat3 m2, mat3x4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2];

	out->m[3][0] = m1.m[3][0];
	out->m[3][1] = m1.m[3][1];
	out->m[3][2] = m1.m[3][2];
}

/*
** Right-multiply the 4x4 matrix "m1" by the 3x3 transformation matrix "m2" (m1*m2) and return the result!
** We pad "m2" out to a 4x4 matrix using zeroes on the off-diagonals and one on the diagonal.
*/
mat3x4 mat3x4MultiplyMat3C(const mat3x4 m1, const mat3 m2){
	const mat3x4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2],

		.m[3][0] = m1.m[3][0],
		.m[3][1] = m1.m[3][1],
		.m[3][2] = m1.m[3][2]
	};

	return(out);
}

// Left-multiply the 3x4 matrix "m2" by the 3x3 transformation matrix "m1" (m1*m2) and store the result in "m2"!
void mat3MultiplyMat3x4(const mat3 m1, mat3x4 *const restrict m2){
	const mat3x4 tempMatrix1 = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix1.m[0][0] + m1.m[1][0] * tempMatrix1.m[0][1] +
	              m1.m[2][0] * tempMatrix1.m[0][2];
	m2->m[0][1] = m1.m[0][1] * tempMatrix1.m[0][0] + m1.m[1][1] * tempMatrix1.m[0][1] +
	              m1.m[2][1] * tempMatrix1.m[0][2];
	m2->m[0][2] = m1.m[0][2] * tempMatrix1.m[0][0] + m1.m[1][2] * tempMatrix1.m[0][1] +
	              m1.m[2][2] * tempMatrix1.m[0][2];

	m2->m[1][0] = m1.m[0][0] * tempMatrix1.m[1][0] + m1.m[1][0] * tempMatrix1.m[1][1] +
	              m1.m[2][0] * tempMatrix1.m[1][2];
	m2->m[1][1] = m1.m[0][1] * tempMatrix1.m[1][0] + m1.m[1][1] * tempMatrix1.m[1][1] +
	              m1.m[2][1] * tempMatrix1.m[1][2];
	m2->m[1][2] = m1.m[0][2] * tempMatrix1.m[1][0] + m1.m[1][2] * tempMatrix1.m[1][1] +
	              m1.m[2][2] * tempMatrix1.m[1][2];

	m2->m[2][0] = m1.m[0][0] * tempMatrix1.m[2][0] + m1.m[1][0] * tempMatrix1.m[2][1] +
	              m1.m[2][0] * tempMatrix1.m[2][2];
	m2->m[2][1] = m1.m[0][1] * tempMatrix1.m[2][0] + m1.m[1][1] * tempMatrix1.m[2][1] +
	              m1.m[2][1] * tempMatrix1.m[2][2];
	m2->m[2][2] = m1.m[0][2] * tempMatrix1.m[2][0] + m1.m[1][2] * tempMatrix1.m[2][1] +
	              m1.m[2][2] * tempMatrix1.m[2][2];

	m2->m[3][0] = m1.m[0][0] * tempMatrix1.m[3][0] + m1.m[1][0] * tempMatrix1.m[3][1] +
	              m1.m[2][0] * tempMatrix1.m[3][2];
	m2->m[3][1] = m1.m[0][1] * tempMatrix1.m[3][0] + m1.m[1][1] * tempMatrix1.m[3][1] +
	              m1.m[2][1] * tempMatrix1.m[3][2];
	m2->m[3][2] = m1.m[0][2] * tempMatrix1.m[3][0] + m1.m[1][2] * tempMatrix1.m[3][1] +
	              m1.m[2][2] * tempMatrix1.m[3][2];
}

// Left-multiply the 3x4 matrix "m2" by the 3x3 transformation matrix "m1" (m1*m2) and store the result in "out"!
void mat3MultiplyMat3x4Out(const mat3 m1, const mat3x4 m2, mat3x4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2];
}

// Left-multiply the 4x4 matrix "m2" by the 3x3 transformation matrix "m1" (m1*m2) and return the result!
mat3x4 mat3MultiplyMat3x4C(const mat3 m1, const mat3x4 m2){
	const mat3x4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2]
	};

	return(out);
}

/*
** Left-multiply "m2" by "m1" (m1*m2) and store the result in "m1"!
** We pad "m2" out to a 4x4 matrix by adding the row [0, 0, 0, 1].
*/
void mat3x4MultiplyMat3x4P1(mat3x4 *const restrict m1, const mat3x4 m2){
	const mat3x4 tempMatrix1 = *m1;

	m1->m[0][0] = tempMatrix1.m[0][0] * m2.m[0][0] + tempMatrix1.m[1][0] * m2.m[0][1] +
	              tempMatrix1.m[2][0] * m2.m[0][2];
	m1->m[0][1] = tempMatrix1.m[0][1] * m2.m[0][0] + tempMatrix1.m[1][1] * m2.m[0][1] +
	              tempMatrix1.m[2][1] * m2.m[0][2];
	m1->m[0][2] = tempMatrix1.m[0][2] * m2.m[0][0] + tempMatrix1.m[1][2] * m2.m[0][1] +
	              tempMatrix1.m[2][2] * m2.m[0][2];

	m1->m[1][0] = tempMatrix1.m[0][0] * m2.m[1][0] + tempMatrix1.m[1][0] * m2.m[1][1] +
	              tempMatrix1.m[2][0] * m2.m[1][2];
	m1->m[1][1] = tempMatrix1.m[0][1] * m2.m[1][0] + tempMatrix1.m[1][1] * m2.m[1][1] +
	              tempMatrix1.m[2][1] * m2.m[1][2];
	m1->m[1][2] = tempMatrix1.m[0][2] * m2.m[1][0] + tempMatrix1.m[1][2] * m2.m[1][1] +
	              tempMatrix1.m[2][2] * m2.m[1][2];

	m1->m[2][0] = tempMatrix1.m[0][0] * m2.m[2][0] + tempMatrix1.m[1][0] * m2.m[2][1] +
	              tempMatrix1.m[2][0] * m2.m[2][2];
	m1->m[2][1] = tempMatrix1.m[0][1] * m2.m[2][0] + tempMatrix1.m[1][1] * m2.m[2][1] +
	              tempMatrix1.m[2][1] * m2.m[2][2];
	m1->m[2][2] = tempMatrix1.m[0][2] * m2.m[2][0] + tempMatrix1.m[1][2] * m2.m[2][1] +
	              tempMatrix1.m[2][2] * m2.m[2][2];

	m1->m[3][0] = tempMatrix1.m[0][0] * m2.m[3][0] + tempMatrix1.m[1][0] * m2.m[3][1] +
	              tempMatrix1.m[2][0] * m2.m[3][2] + tempMatrix1.m[3][0];
	m1->m[3][1] = tempMatrix1.m[0][1] * m2.m[3][0] + tempMatrix1.m[1][1] * m2.m[3][1] +
	              tempMatrix1.m[2][1] * m2.m[3][2] + tempMatrix1.m[3][1];
	m1->m[3][2] = tempMatrix1.m[0][2] * m2.m[3][0] + tempMatrix1.m[1][2] * m2.m[3][1] +
	              tempMatrix1.m[2][2] * m2.m[3][2] + tempMatrix1.m[3][2];
}

/*
** Left-multiply "m2" by "m1" (m1*m2) and store the result in "m2"!
** We pad "m2" out to a 4x4 matrix by adding the row [0, 0, 0, 1].
*/
void mat3x4MultiplyMat3x4P2(const mat3x4 m1, mat3x4 *const restrict m2){
	const mat3x4 tempMatrix1 = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix1.m[0][0] + m1.m[1][0] * tempMatrix1.m[0][1] +
	              m1.m[2][0] * tempMatrix1.m[0][2];
	m2->m[0][1] = m1.m[0][1] * tempMatrix1.m[0][0] + m1.m[1][1] * tempMatrix1.m[0][1] +
	              m1.m[2][1] * tempMatrix1.m[0][2];
	m2->m[0][2] = m1.m[0][2] * tempMatrix1.m[0][0] + m1.m[1][2] * tempMatrix1.m[0][1] +
	              m1.m[2][2] * tempMatrix1.m[0][2];

	m2->m[1][0] = m1.m[0][0] * tempMatrix1.m[1][0] + m1.m[1][0] * tempMatrix1.m[1][1] +
	              m1.m[2][0] * tempMatrix1.m[1][2];
	m2->m[1][1] = m1.m[0][1] * tempMatrix1.m[1][0] + m1.m[1][1] * tempMatrix1.m[1][1] +
	              m1.m[2][1] * tempMatrix1.m[1][2];
	m2->m[1][2] = m1.m[0][2] * tempMatrix1.m[1][0] + m1.m[1][2] * tempMatrix1.m[1][1] +
	              m1.m[2][2] * tempMatrix1.m[1][2];

	m2->m[2][0] = m1.m[0][0] * tempMatrix1.m[2][0] + m1.m[1][0] * tempMatrix1.m[2][1] +
	              m1.m[2][0] * tempMatrix1.m[2][2];
	m2->m[2][1] = m1.m[0][1] * tempMatrix1.m[2][0] + m1.m[1][1] * tempMatrix1.m[2][1] +
	              m1.m[2][1] * tempMatrix1.m[2][2];
	m2->m[2][2] = m1.m[0][2] * tempMatrix1.m[2][0] + m1.m[1][2] * tempMatrix1.m[2][1] +
	              m1.m[2][2] * tempMatrix1.m[2][2];

	m2->m[3][0] = m1.m[0][0] * tempMatrix1.m[3][0] + m1.m[1][0] * tempMatrix1.m[3][1] +
	              m1.m[2][0] * tempMatrix1.m[3][2] + m1.m[3][0];
	m2->m[3][1] = m1.m[0][1] * tempMatrix1.m[3][0] + m1.m[1][1] * tempMatrix1.m[3][1] +
	              m1.m[2][1] * tempMatrix1.m[3][2] + m1.m[3][1];
	m2->m[3][2] = m1.m[0][2] * tempMatrix1.m[3][0] + m1.m[1][2] * tempMatrix1.m[3][1] +
	              m1.m[2][2] * tempMatrix1.m[3][2] + m1.m[3][2];
}

/*
** Left-multiply "m2" by "m1" (m1*m2) and store the result in "out"!
** We pad "m2" out to a 4x4 matrix by adding the row [0, 0, 0, 1].
*/
void mat3x4MultiplyMat3x4Out(const mat3x4 m1, const mat3x4 m2, mat3x4 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
	               m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
	               m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
	               m1.m[2][2] * m2.m[0][2];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
	               m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
	               m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
	               m1.m[2][2] * m2.m[1][2];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
	               m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
	               m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
	               m1.m[2][2] * m2.m[2][2];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
	               m1.m[2][0] * m2.m[3][2] + m1.m[3][0];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
	               m1.m[2][1] * m2.m[3][2] + m1.m[3][1];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
	               m1.m[2][2] * m2.m[3][2] + m1.m[3][2];
}

/*
** Left-multiply "m2" by "m1" (m1*m2) and return the result!
** We pad "m2" out to a 4x4 matrix by adding the row [0, 0, 0, 1].
*/
mat3x4 mat3x4MultiplyMat3x4C(const mat3x4 m1, const mat3x4 m2){
	const mat3x4 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] +
		           m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] +
		           m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] +
		           m1.m[2][2] * m2.m[0][2],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] +
		           m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] +
		           m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] +
		           m1.m[2][2] * m2.m[1][2],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] +
		           m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] +
		           m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] +
		           m1.m[2][2] * m2.m[2][2],

		.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] +
		           m1.m[2][0] * m2.m[3][2] + m1.m[3][0],
		.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] +
		           m1.m[2][1] * m2.m[3][2] + m1.m[3][1],
		.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] +
		           m1.m[2][2] * m2.m[3][2] + m1.m[3][2]
	};

	return(out);
}


// Left-multiply a matrix by a translation matrix stored as three floats!
void mat3x4Translate(mat3x4 *const restrict m, const float x, const float y, const float z){
	m->m[3][0] += x;
	m->m[3][1] += y;
	m->m[3][2] += z;
}

// Left-multiply a matrix by a translation matrix stored as three floats!
mat3x4 mat3x4TranslateC(mat3x4 m, const float x, const float y, const float z){
	m.m[3][0] += x;
	m.m[3][1] += y;
	m.m[3][2] += z;

	return(m);
}

// Left-multiply a matrix by a translation matrix stored as a vec3!
void mat3x4TranslateVec3(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4Translate(m, v->x, v->y, v->z);
}

// Left-multiply a matrix by a translation matrix stored as a vec3!
mat3x4 mat3x4TranslateVec3C(const mat3x4 m, const vec3 v){
	return(mat3x4TranslateC(m, v.x, v.y, v.z));
}

// Right-multiply a matrix by a translation matrix stored as three floats!
void mat3x4TranslatePre(mat3x4 *const restrict m, const float x, const float y, const float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
}

// Right-multiply a matrix by a translation matrix stored as three floats!
mat3x4 mat3x4TranslatePreC(mat3x4 m, const float x, const float y, const float z){
	m.m[3][0] = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0];
	m.m[3][1] = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1];
	m.m[3][2] = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2];

	return(m);
}

// Right-multiply a matrix by a translation matrix stored as a vec3!
void mat3x4TranslatePreVec3(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4TranslatePre(m, v->x, v->y, v->z);
}

// Right-multiply a matrix by a translation matrix stored as a vec3!
mat3x4 mat3x4TranslatePreVec3C(const mat3x4 m, const vec3 v){
	return(mat3x4TranslatePreC(m, v.x, v.y, v.z));
}


// Rotate a matrix using XYZ Euler angles!
void mat3x4RotateByEulerXYZ(mat3x4 *const restrict m, const float x, const float y, const float z){
	mat3x4 rotMatrix;
	mat3x4InitEulerXYZ(&rotMatrix, x, y, z);
	mat3x4MultiplyMat3x4P2(rotMatrix, m);
}

// Rotate a matrix using ZXY Euler angles!
void mat3x4RotateByEulerZXY(mat3x4 *const restrict m, const float x, const float y, const float z){
	mat3x4 rotMatrix;
	mat3x4InitEulerZXY(&rotMatrix, x, y, z);
	mat3x4MultiplyMat3x4P2(rotMatrix, m);
}

// Rotate a matrix using XYZ Euler angles!
mat3x4 mat3x4RotateByEulerXYZC(const mat3x4 m, const float x, const float y, const float z){
	const mat3x4 rotMatrix = mat3x4InitEulerXYZC(x, y, z);
	return(mat3x4MultiplyMat3x4C(rotMatrix, m));
}

// Rotate a matrix using ZXY Euler angles!
mat3x4 mat3x4RotateByEulerZXYC(const mat3x4 m, const float x, const float y, const float z){
	const mat3x4 rotMatrix = mat3x4InitEulerZXYC(x, y, z);
	return(mat3x4MultiplyMat3x4C(rotMatrix, m));
}

// Rotate a matrix by a vec3 using XYZ Euler angles!
void mat3x4RotateByVec3EulerXYZ(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4RotateByEulerXYZ(m, v->x, v->y, v->z);
}

// Rotate a matrix by a vec3 using ZXY Euler angles!
void mat3x4RotateByVec3EulerZXY(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4RotateByEulerZXY(m, v->x, v->y, v->z);
}

// Rotate a matrix by a vec3 using XYZ Euler angles!
mat3x4 mat3x4RotateByVec3EulerXYZC(const mat3x4 m, const vec3 v){
	return(mat3x4RotateByEulerXYZC(m, v.x, v.y, v.z));
}

// Rotate a matrix by a vec3 using ZXY Euler angles!
mat3x4 mat3x4RotateByVec3EulerZXYC(const mat3x4 m, const vec3 v){
	return(mat3x4RotateByEulerZXYC(m, v.x, v.y, v.z));
}

// Rotate a matrix by an axis and an angle!
void mat3x4RotateByAxisAngle(mat3x4 *const restrict m, const vec4 *const restrict v){
	mat3 rotMatrix;
	// Convert the axis angle to a rotation matrix!
	mat3InitAxisAngle(&rotMatrix, v);
	// Now rotate our matrix by it!
	mat3MultiplyMat3x4(rotMatrix, m);
}

// Rotate a matrix by an axis and an angle!
mat3x4 mat3x4RotateByAxisAngleC(const mat3x4 m, const vec4 v){
	return(mat3MultiplyMat3x4C(mat3InitAxisAngleC(v), m));
}

// Rotate a matrix by a quaternion!
void mat3x4RotateByQuat(mat3x4 *const restrict m, const quat *const restrict q){
	mat3 rotMatrix;
	// Convert the quaternion to a rotation matrix!
	mat3InitQuat(&rotMatrix, q);
	// Now rotate our matrix by it!
	mat3MultiplyMat3x4(rotMatrix, m);
}

// Rotate a matrix by a quaternion!
mat3x4 mat3x4RotateByQuatC(const mat3x4 m, const quat q){
	return(mat3MultiplyMat3x4C(mat3InitQuatC(q), m));
}

// Rotate a matrix on the X-axis!
void mat3x4RotateX(mat3x4 *const restrict m, const float x){
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
mat3x4 mat3x4RotateXC(const mat3x4 m, const float x){
	const float cx = cosf(x);
	const float sx = sinf(x);

	const mat3x4 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = cx * m.m[0][1] - sx * m.m[0][2],
		.m[0][2] = sx * m.m[0][1] + cx * m.m[0][2],

		.m[1][0] = m.m[1][0],
		.m[1][1] = cx * m.m[1][1] - sx * m.m[1][2],
		.m[1][2] = sx * m.m[1][1] + cx * m.m[1][2],

		.m[2][0] = m.m[2][0],
		.m[2][1] = cx * m.m[2][1] - sx * m.m[2][2],
		.m[2][2] = sx * m.m[2][1] + cx * m.m[2][2],

		.m[3][0] = m.m[3][0],
		.m[3][1] = cx * m.m[3][1] - sx * m.m[3][2],
		.m[3][2] = sx * m.m[3][1] + cx * m.m[3][2]
	};

	return(out);
}

// Rotate a matrix on the Y-axis!
void mat3x4RotateY(mat3x4 *const restrict m, const float y){
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
mat3x4 mat3x4RotateYC(const mat3x4 m, const float y){
	const float cy = cosf(y);
	const float sy = sinf(y);

	const mat3x4 out = {
		.m[0][0] = cy * m.m[0][0] + sy * m.m[0][2],
		.m[0][1] = m.m[0][1],
		.m[0][2] = -sy * m.m[0][0] + cy * m.m[0][2],

		.m[1][0] = cy * m.m[1][0] + sy * m.m[1][2],
		.m[1][1] = m.m[1][1],
		.m[1][2] = -sy * m.m[1][0] + cy * m.m[1][2],

		.m[2][0] = cy * m.m[2][0] + sy * m.m[2][2],
		.m[2][1] = m.m[2][1],
		.m[2][2] = -sy * m.m[2][0] + cy * m.m[2][2],

		.m[3][0] = cy * m.m[3][0] + sy * m.m[3][2],
		.m[3][1] = m.m[3][1],
		.m[3][2] = -sy * m.m[3][0] + cy * m.m[3][2]
	};

	return(out);
}

// Rotate a matrix on the Z-axis!
void mat3x4RotateZ(mat3x4 *const restrict m, const float z){
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
mat3x4 mat3x4RotateZC(const mat3x4 m, const float z){
	const float cz = cosf(z);
	const float sz = sinf(z);

	const mat3x4 out = {
		.m[0][0] = cz * m.m[0][0] - sz * m.m[0][1],
		.m[0][1] = sz * m.m[0][0] + cz * m.m[0][1],
		.m[0][2] = m.m[0][2],

		.m[1][0] = cz * m.m[1][0] - sz * m.m[1][1],
		.m[1][1] = sz * m.m[1][0] + cz * m.m[1][1],
		.m[1][2] = m.m[1][2],

		.m[2][0] = cz * m.m[2][0] - sz * m.m[2][1],
		.m[2][1] = sz * m.m[2][0] + cz * m.m[2][1],
		.m[2][2] = m.m[2][2],

		.m[3][0] = cz * m.m[3][0] - sz * m.m[3][1],
		.m[3][1] = sz * m.m[3][0] + cz * m.m[3][1],
		.m[3][2] = m.m[3][2]
	};

	return(out);
}

/*
** Generate a rotation matrix that looks in the
** direction of the normalized vector "forward"!
*/
void mat3x4RotateForward(
	mat3x4 *const restrict m,
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

	m->m[1][0] = up.x;
	m->m[1][1] = up.y;
	m->m[1][2] = up.z;

	m->m[2][0] = forward->x;
	m->m[2][1] = forward->y;
	m->m[2][2] = forward->z;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

/*
** Generate a rotation matrix that looks in the
** direction of the normalized vector "forward"!
*/
mat3x4 mat3x4RotateForwardC(const vec3 forward, const vec3 worldUp){
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));

	// Rotate the matrix to look forward!
	const mat3x4 m = {
		.m[0][0] = right.x,   .m[0][1] = right.y,   .m[0][2] = right.z,
		.m[1][0] = up.x,      .m[1][1] = up.y,      .m[1][2] = up.z,
		.m[2][0] = forward.x, .m[2][1] = forward.y, .m[2][2] = forward.z,
		.m[3][0] = 0.f,       .m[3][1] = 0.f,       .m[3][2] = 0.f
	};

	return(m);
}

// Generate a rotation matrix that faces a target!
void mat3x4RotateToFace(
	mat3x4 *const restrict m,
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

	m->m[1][0] = up.x;
	m->m[1][1] = up.y;
	m->m[1][2] = up.z;

	m->m[2][0] = forward.x;
	m->m[2][1] = forward.y;
	m->m[2][2] = forward.z;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
}

// Generate a rotation matrix that faces a target!
mat3x4 mat3x4RotateToFaceC(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3C(target, eye));
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));

	// Rotate the matrix to look at "target"!
	const mat3x4 m = {
		.m[0][0] = right.x,   .m[0][1] = right.y,   .m[0][2] = right.z,
		.m[1][0] = up.x,      .m[1][1] = up.y,      .m[1][2] = up.z,
		.m[2][0] = forward.x, .m[2][1] = forward.y, .m[2][2] = forward.z,
		.m[3][0] = 0.f,       .m[3][1] = 0.f,       .m[3][2] = 0.f
	};

	return(m);
}


// Left-multiply a matrix by a scale matrix stored as three floats!
void mat3x4Scale(mat3x4 *const restrict m, const float x, const float y, const float z){
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
mat3x4 mat3x4ScaleC(mat3x4 m, const float x, const float y, const float z){
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

// Left-multiply a matrix by a scale matrix stored as a vec3!
void mat3x4ScaleVec3(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4Scale(m, v->x, v->y, v->z);
}

// Left-multiply a matrix by a scale matrix stored as a vec3!
mat3x4 mat3x4ScaleVec3C(mat3x4 m, const vec3 v){
	return(mat3x4ScaleC(m, v.x, v.y, v.z));
}

// Right-multiply a matrix by a scale matrix stored as three floats!
void mat3x4ScalePre(mat3x4 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] *= x;
	m->m[0][1] *= x;
	m->m[0][2] *= x;

	m->m[1][0] *= y;
	m->m[1][1] *= y;
	m->m[1][2] *= y;

	m->m[2][0] *= z;
	m->m[2][1] *= z;
	m->m[2][2] *= z;
}

// Right-multiply a matrix by a scale matrix stored as three floats!
mat3x4 mat3x4ScalePreC(mat3x4 m, const float x, const float y, const float z){
	m.m[0][0] *= x;
	m.m[0][1] *= x;
	m.m[0][2] *= x;

	m.m[1][0] *= y;
	m.m[1][1] *= y;
	m.m[1][2] *= y;

	m.m[2][0] *= z;
	m.m[2][1] *= z;
	m.m[2][2] *= z;

	return(m);
}

// Right-multiply a matrix by a scale matrix stored as a vec3!
void mat3x4ScalePreVec3(mat3x4 *const restrict m, const vec3 *const restrict v){
	mat3x4ScalePre(m, v->x, v->y, v->z);
}

// Right-multiply a matrix by a scale matrix stored as a vec3!
mat3x4 mat3x4ScalePreVec3C(mat3x4 m, const vec3 v){
	return(mat3x4ScalePreC(m, v.x, v.y, v.z));
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
void mat3x4View(mat3x4 *const restrict m, const vec3 *const restrict pos, const mat3 *const restrict rot){
	// We take the transpose of the rotation to invert it.
	m->m[0][0] = rot->m[0][0];
	m->m[0][1] = rot->m[1][0];
	m->m[0][2] = rot->m[2][0];

	m->m[1][0] = rot->m[0][1];
	m->m[1][1] = rot->m[1][1];
	m->m[1][2] = rot->m[2][1];

	m->m[2][0] = rot->m[0][2];
	m->m[2][1] = rot->m[1][2];
	m->m[2][2] = rot->m[2][2];

	// Our matrices are column-major, so the rows are the basis vectors.
	// We negate the dot products to effectively invert the translation.
	m->m[3][0] = -vec3DotVec3((vec3 *)&rot->m[0], pos);
	m->m[3][1] = -vec3DotVec3((vec3 *)&rot->m[1], pos);
	m->m[3][2] = -vec3DotVec3((vec3 *)&rot->m[2], pos);
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
mat3x4 mat3x4ViewC(const vec3 pos, const mat3 rot){
	const mat3x4 m = {
		// We take the transpose of the rotation to invert it.
		.m[0][0] = rot.m[0][0], .m[0][1] = rot.m[1][0], .m[0][2] = rot.m[2][0],
		.m[1][0] = rot.m[0][1], .m[1][1] = rot.m[1][1], .m[1][2] = rot.m[2][1],
		.m[2][0] = rot.m[0][2], .m[2][1] = rot.m[1][2], .m[2][2] = rot.m[2][2],
		// Our matrices are column-major, so the rows are the basis vectors.
		// We negate the dot products to effectively invert the translation.
		.m[3][0] = -vec3DotVec3C(*((vec3 *)&rot.m[0]), pos),
		.m[3][1] = -vec3DotVec3C(*((vec3 *)&rot.m[1]), pos),
		.m[3][2] = -vec3DotVec3C(*((vec3 *)&rot.m[2]), pos),
	};
	return(m);
}

// Generate a look-at matrix!
void mat3x4ViewLookAt(
	mat3x4 *const restrict m,
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

	m->m[1][0] = right.y;
	m->m[1][1] = up.y;
	m->m[1][2] = forward.y;

	m->m[2][0] = right.z;
	m->m[2][1] = up.z;
	m->m[2][2] = forward.z;

	m->m[3][0] = -vec3DotVec3(&right, eye);
	m->m[3][1] = -vec3DotVec3(&up, eye);
	m->m[3][2] = -vec3DotVec3(&forward, eye);
}

// Generate a look-at matrix!
mat3x4 mat3x4ViewLookAtC(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3C(eye, target));
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));

	// Translate the matrix to "eye" and make it look at "target"!
	const mat3x4 m = {
		.m[0][0] = right.x,                   .m[0][1] = up.x,                   .m[0][2] = forward.x,
		.m[1][0] = right.y,                   .m[1][1] = up.y,                   .m[1][2] = forward.y,
		.m[2][0] = right.z,                   .m[2][1] = up.z,                   .m[2][2] = forward.z,
		.m[3][0] = -vec3DotVec3C(right, eye), .m[3][1] = -vec3DotVec3C(up, eye), .m[3][2] = -vec3DotVec3C(forward, eye)
	};

	return(m);
}


/*
** Convert a 4x4 matrix to a quaternion and store the result in "out"!
** For this to work, we assume that "m" is a special orthogonal matrix.
** Implementation by Mike Day from Converting a Rotation Matrix to a Quaternion.
*/
void mat3x4ToQuat(const mat3x4 *const restrict m, quat *const restrict out){
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
quat mat3x4ToQuatC(const mat3x4 m){
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
void mat3x4ToQuatAlt(const mat3x4 *const restrict m, quat *const restrict out){
	out->x = copySignZero(0.5f * sqrtf( m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f), m->m[1][2] - m->m[2][1]);
	out->y = copySignZero(0.5f * sqrtf(-m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f), m->m[2][0] - m->m[0][2]);
	out->z = copySignZero(0.5f * sqrtf(-m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f), m->m[0][1] - m->m[1][0]);
	out->w = 0.5f * sqrtf(m->m[0][0] + m->m[1][1] + m->m[2][2] + 1.f);
}

quat mat3x4ToQuatAltC(const mat3x4 m){
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
void quatToMat3x4(const quat *const restrict q, mat3x4 *const restrict out){
	const float xx = q->x*q->x;
	const float yy = q->y*q->y;
	const float zz = q->z*q->z;
	float temp1;
	float temp2;

	out->m[0][0] = 1 - 2*(yy + zz);
	out->m[1][1] = 1 - 2*(xx + zz);
	out->m[2][2] = 1 - 2*(xx + yy);

	temp1 = q->x*q->y;
	temp2 = q->z*q->w;
	out->m[0][1] = 2*(temp1 + temp2);
	out->m[1][0] = 2*(temp1 - temp2);

	temp1 = q->x*q->z;
	temp2 = q->y*q->w;
	out->m[0][2] = 2*(temp1 - temp2);
	out->m[2][0] = 2*(temp1 + temp2);

	temp1 = q->y*q->z;
	temp2 = q->x*q->w;
	out->m[1][2] = 2*(temp1 + temp2);
	out->m[2][1] = 2*(temp1 - temp2);

	out->m[3][0] =
	out->m[3][1] =
	out->m[3][2] = 0.f;
}

/*
** Convert a quaternion to a 4x4 matrix!
** This assumes that "q" is normalized.
*/
mat3x4 quatToMat3x4C(const quat q){
	const float xx = q.x*q.x;
	const float yy = q.y*q.y;
	const float zz = q.z*q.z;
	float temp1;
	float temp2;
	mat3x4 out;

	out.m[0][0] = 1 - 2*(yy + zz);
	out.m[1][1] = 1 - 2*(xx + zz);
	out.m[2][2] = 1 - 2*(xx + yy);

	temp1 = q.x*q.y;
	temp2 = q.z*q.w;
	out.m[0][1] = 2*(temp1 + temp2);
	out.m[1][0] = 2*(temp1 - temp2);

	temp1 = q.x*q.z;
	temp2 = q.y*q.w;
	out.m[0][2] = 2*(temp1 - temp2);
	out.m[2][0] = 2*(temp1 + temp2);

	temp1 = q.y*q.z;
	temp2 = q.x*q.w;
	out.m[1][2] = 2*(temp1 + temp2);
	out.m[2][1] = 2*(temp1 - temp2);

	out.m[3][0] =
	out.m[3][1] =
	out.m[3][2] = 0.f;

	return(out);
}