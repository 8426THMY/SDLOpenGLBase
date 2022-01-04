#include "mat3.h"


#include <string.h>

#include "utilMath.h"


// We use this value when comparing non-negative numbers with 0.
#define MAT3_ZERO_EPSILON 0.000001f
#define TWO_PI_ON_THREE 2.09439510239319549f


// Forward-declare any helper functions!
static void ComputeEigenvector0(
	const float a00, const float a01, const float a02,
	const float a11, const float a12, const float a22,
	const float eval0, vec3 *const evec0
);
static void ComputeEigenvector1(
	const float a00, const float a01, const float a02,
	const float a11, const float a12, const float a22,
	const vec3 *const evec0, const float eval1, vec3 *const evec1
);


mat3 g_mat3Identity = {
	.m[0][0] = 1.f, .m[0][1] = 0.f, .m[0][2] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f, .m[1][2] = 0.f,
	.m[2][0] = 0.f, .m[2][1] = 0.f, .m[2][2] = 1.f
};


// Initialize the matrix's values to 0!
void mat3InitZero(mat3 *const restrict m){
	memset(m, 0.f, sizeof(*m));
}

// Initialize the matrix's values to 0!
mat3 mat3InitZeroC(){
	mat3 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

// Initialize the matrix to an identity matrix!
void mat3InitIdentity(mat3 *const restrict m){
	*m = g_mat3Identity;
}

// Initialize the matrix to an identity matrix!
mat3 mat3InitIdentityC(){
	return(g_mat3Identity);
}

// Initialize a matrix from XYZ Euler angles (in radians)!
void mat3InitEulerXYZ(mat3 *const restrict m, const float x, const float y, const float z){
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
}

// Initialize a matrix from ZXY Euler angles (in radians)!
void mat3InitEulerZXY(mat3 *const restrict m, const float x, const float y, const float z){
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
}

// Initialize a matrix from XYZ Euler angles (in radians)!
mat3 mat3InitEulerXYZC(const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;

	const mat3 m = {
		.m[0][0] = cy*cz,
		.m[0][1] = cy*sz,
		.m[0][2] = -sy,

		.m[1][0] = sxsy*cz - cx*sz,
		.m[1][1] = sxsy*sz + cx*cz,
		.m[1][2] = sx*cy,

		.m[2][0] = cxsy*cz + sx*sz,
		.m[2][1] = cxsy*sz - sx*cz,
		.m[2][2] = cx*cy
	};


	return(m);
}

// Initialize a matrix from ZXY Euler angles (in radians)!
mat3 mat3InitEulerZXYC(const float x, const float y, const float z){
	const float cx = cosf(x);
	const float sx = sinf(x);
	const float cy = cosf(y);
	const float sy = sinf(y);
	const float cz = cosf(z);
	const float sz = sinf(z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	const mat3 m = {
		.m[0][0] = sxsy*sz + cy*cz,
		.m[0][1] = cx*sz,
		.m[0][2] = sxcy*sz - sy*cz,

		.m[1][0] = sxsy*cz - cy*sz,
		.m[1][1] = cx*cz,
		.m[1][2] = sxcy*cz + sy*sz,

		.m[2][0] = cx*sy,
		.m[2][1] = -sx,
		.m[2][2] = cx*cy
	};


	return(m);
}

// Initialize a matrix from XYZ Euler angles (in radians)!
void mat3InitEulerVec3XYZ(mat3 *const restrict m, const vec3 *const restrict v){
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
}

// Initialize a matrix from ZXY Euler angles (in radians)!
void mat3InitEulerVec3ZXY(mat3 *const restrict m, const vec3 *const restrict v){
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
}

// Initialize a matrix from XYZ Euler angles (in radians)!
mat3 mat3InitEulerVec3XYZC(const vec3 v){
	const float cx = cosf(v.x);
	const float sx = sinf(v.x);
	const float cy = cosf(v.y);
	const float sy = sinf(v.y);
	const float cz = cosf(v.z);
	const float sz = sinf(v.z);
	const float sxsy = sx * sy;
	const float cxsy = cx * sy;

	const mat3 m = {
		.m[0][0] = cy*cz,
		.m[0][1] = cy*sz,
		.m[0][2] = -sy,

		.m[1][0] = sxsy*cz - cx*sz,
		.m[1][1] = sxsy*sz + cx*cz,
		.m[1][2] = sx*cy,

		.m[2][0] = cxsy*cz + sx*sz,
		.m[2][1] = cxsy*sz - sx*cz,
		.m[2][2] = cx*cy
	};


	return(m);
}

// Initialize a matrix from ZXY Euler angles (in radians)!
mat3 mat3InitEulerVec3ZXYC(const vec3 v){
	const float cx = cosf(v.x);
	const float sx = sinf(v.x);
	const float cy = cosf(v.y);
	const float sy = sinf(v.y);
	const float cz = cosf(v.z);
	const float sz = sinf(v.z);
	const float sxsy = sx * sy;
	const float sxcy = sx * cy;

	const mat3 m = {
		.m[0][0] = sxsy*sz + cy*cz,
		.m[0][1] = cx*sz,
		.m[0][2] = sxcy*sz - sy*cz,

		.m[1][0] = sxsy*cz - cy*sz,
		.m[1][1] = cx*cz,
		.m[1][2] = sxcy*cz + sy*sz,

		.m[2][0] = cx*sy,
		.m[2][1] = -sx,
		.m[2][2] = cx*cy
	};


	return(m);
}

// Initialize a matrix from an axis and an angle!
void mat3InitAxisAngle(mat3 *const restrict m, const vec4 *const restrict v){
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
}

// Initialize a matrix from an axis and an angle!
mat3 mat3InitAxisAngleC(const vec4 v){
	const float c = cosf(v.w);
	const float s = sinf(v.w);
	const float t = 1.f - c;

	// Normalize the axis!
	const vec3 normalAxis = vec3NormalizeFastC(v.x, v.y, v.z);
	const vec3 tempAxis = vec3MultiplySC(normalAxis, t);

	// Convert the axis angle to a rotation matrix!
	const mat3 out = {
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

	return(out);
}

// Initialise a matrix to a rotation matrix!
void mat3InitQuat(mat3 *const restrict m, const quat *const restrict q){
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
}

// Initialise a matrix to a rotation matrix!
mat3 mat3InitQuatC(const quat q){
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
	const mat3 m = {
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


	return(m);
}

// Initialise a matrix to a scale matrix!
void mat3InitScale(mat3 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = y;
	m->m[1][2] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = z;
}

// Initialise a matrix to a scale matrix!
mat3 mat3InitScaleC(const float x, const float y, const float z){
	const mat3 m = {
		.m[0][0] = x,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = y,
		.m[1][2] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = z
	};

	return(m);
}

// Initialise a matrix to a scale matrix using a vec3!
void mat3InitScaleVec3(mat3 *const restrict m, const vec3 *const restrict v){
	mat3InitScale(m, v->x, v->y, v->z);
}

// Initialise a matrix to a scale matrix using a vec3!
mat3 mat3InitScaleVec3C(const vec3 v){
	return(mat3InitScaleC(v.x, v.y, v.z));
}

// Initialize the matrix to a uniform scale matrix!
void mat3InitDiagonal(mat3 *const restrict m, const float x){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = x;
	m->m[1][2] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = x;
}

// Initialize the matrix to a uniform scale matrix!
mat3 mat3InitDiagonalC(const float x){
	const mat3 out = {
		.m[0][0] = x,
		.m[0][1] = 0.f,
		.m[0][2] = 0.f,

		.m[1][0] = 0.f,
		.m[1][1] = x,
		.m[1][2] = 0.f,

		.m[2][0] = 0.f,
		.m[2][1] = 0.f,
		.m[2][2] = x
	};

	return(out);
}

// Initialize the matrix to a skew-symmetric cross product matrix!
void mat3InitSkew(mat3 *const restrict m, const vec3 *const restrict v){
	m->m[0][0] = 0.f;
	m->m[0][1] = v->z;
	m->m[0][2] = -v->y;

	m->m[1][0] = -v->z;
	m->m[1][1] = 0.f;
	m->m[1][2] = v->x;

	m->m[2][0] = v->y;
	m->m[2][1] = -v->x;
	m->m[2][2] = 0.f;
}

// Initialize the matrix to a skew-symmetric cross product matrix!
mat3 mat3InitSkewC(const vec3 v){
	const mat3 out = {
		.m[0][0] = 0.f,
		.m[0][1] = v.z,
		.m[0][2] = -v.y,

		.m[1][0] = -v.z,
		.m[1][1] = 0.f,
		.m[1][2] = v.x,

		.m[2][0] = v.y,
		.m[2][1] = -v.x,
		.m[2][2] = 0.f
	};

	return(out);
}

// Given a quaternion and a vector, construct the shear matrix QVQ^T.
void mat3InitShearQuat(
	mat3 *const restrict m,
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
}

// Given a quaternion and a vector, construct the shear matrix QVQ^T.
mat3 mat3InitShearQuatC(const vec3 v, const quat q){
	// Convert the quaternion Q to a rotation matrix!
	const mat3 rotMatrix = mat3InitQuatC(q);

	float cx = v.x*rotMatrix.m[0][0];
	float cy = v.y*rotMatrix.m[1][0];
	float cz = v.z*rotMatrix.m[2][0];
	mat3 m;

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

	return(m);
}


// Add the matrix "m2" to "m1"!
void mat3AddMat3(mat3 *const restrict m1, const mat3 *const restrict m2){
	m1->m[0][0] += m2->m[0][0];
	m1->m[0][1] += m2->m[0][1];
	m1->m[0][2] += m2->m[0][2];

	m1->m[1][0] += m2->m[1][0];
	m1->m[1][1] += m2->m[1][1];
	m1->m[1][2] += m2->m[1][2];

	m1->m[2][0] += m2->m[2][0];
	m1->m[2][1] += m2->m[2][1];
	m1->m[2][2] += m2->m[2][2];
}

// Add the matrix "m2" to "m1" and store the result in "out"!
void mat3AddMat3Out(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict out){
	out->m[0][0] = m1->m[0][0] + m2->m[0][0];
	out->m[0][1] = m1->m[0][1] + m2->m[0][1];
	out->m[0][2] = m1->m[0][2] + m2->m[0][2];

	out->m[1][0] = m1->m[1][0] + m2->m[1][0];
	out->m[1][1] = m1->m[1][1] + m2->m[1][1];
	out->m[1][2] = m1->m[1][2] + m2->m[1][2];

	out->m[2][0] = m1->m[2][0] + m2->m[2][0];
	out->m[2][1] = m1->m[2][1] + m2->m[2][1];
	out->m[2][2] = m1->m[2][2] + m2->m[2][2];
}

// Add the matrix "m2" to "m1" and return the result!
mat3 mat3AddMat3C(const mat3 m1, const mat3 m2){
	const mat3 out = {
		.m[0][0] = m1.m[0][0] + m2.m[0][0],
		.m[0][1] = m1.m[0][1] + m2.m[0][1],
		.m[0][2] = m1.m[0][2] + m2.m[0][2],

		.m[1][0] = m1.m[1][0] + m2.m[1][0],
		.m[1][1] = m1.m[1][1] + m2.m[1][1],
		.m[1][2] = m1.m[1][2] + m2.m[1][2],

		.m[2][0] = m1.m[2][0] + m2.m[2][0],
		.m[2][1] = m1.m[2][1] + m2.m[2][1],
		.m[2][2] = m1.m[2][2] + m2.m[2][2]
	};

	return(out);
}


// Left-multiply a column vector by a matrix (m*v)!
void mat3MultiplyVec3(const mat3 *const restrict m, vec3 *const restrict v){
	const vec3 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z;
}

/*
** Left-multiply a column vector by a matrix (m*v) and store the result in "out"!
** This assumes that "out" isn't "v".
*/
void mat3MultiplyVec3Out(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;
}

// Left-multiply a column vector by a matrix (m*v) and return the result!
vec3 mat3MultiplyVec3C(const mat3 m, const vec3 v){
	const vec3 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z
	};

	return(out);
}

// Right-multiply a row vector by a matrix (v^T*m)!
void vec3MultiplyMat3(vec3 *const restrict v, const mat3 *const restrict m){
	const vec3 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1] + temp.z * m->m[0][2];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1] + temp.z * m->m[1][2];
	v->z = temp.x * m->m[2][0] + temp.y * m->m[2][1] + temp.z * m->m[2][2];
}

// Right-multiply a row vector by a matrix (v^T*m) and store the result in "out"!
void vec3MultiplyMat3Out(const vec3 *const restrict v, const mat3 *const restrict m, vec3 *const restrict out){
	out->x = v->x * m->m[0][0] + v->y * m->m[0][1] + v->z * m->m[0][2];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1] + v->z * m->m[1][2];
	out->z = v->x * m->m[2][0] + v->y * m->m[2][1] + v->z * m->m[2][2];
}

// Right-multiply a row vector by a matrix (v^T*m) and return the result!
vec3 vec3MultiplyMat3C(const vec3 v, const mat3 m){
	const vec3 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2],
		.z = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2]
	};

	return(out);
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "m1"!
void mat3MultiplyMat3P1(mat3 *const restrict m1, const mat3 m2){
	const mat3 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[0][1] + tempMatrix.m[2][0] * m2.m[0][2];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1] + tempMatrix.m[2][1] * m2.m[0][2];
	m1->m[0][2] = tempMatrix.m[0][2] * m2.m[0][0] + tempMatrix.m[1][2] * m2.m[0][1] + tempMatrix.m[2][2] * m2.m[0][2];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[1][0] * m2.m[1][1] + tempMatrix.m[2][0] * m2.m[1][2];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1] + tempMatrix.m[2][1] * m2.m[1][2];
	m1->m[1][2] = tempMatrix.m[0][2] * m2.m[1][0] + tempMatrix.m[1][2] * m2.m[1][1] + tempMatrix.m[2][2] * m2.m[1][2];

	m1->m[2][0] = tempMatrix.m[0][0] * m2.m[2][0] + tempMatrix.m[1][0] * m2.m[2][1] + tempMatrix.m[2][0] * m2.m[2][2];
	m1->m[2][1] = tempMatrix.m[0][1] * m2.m[2][0] + tempMatrix.m[1][1] * m2.m[2][1] + tempMatrix.m[2][1] * m2.m[2][2];
	m1->m[2][2] = tempMatrix.m[0][2] * m2.m[2][0] + tempMatrix.m[1][2] * m2.m[2][1] + tempMatrix.m[2][2] * m2.m[2][2];
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "m2"!
void mat3MultiplyMat3P2(const mat3 m1, mat3 *const restrict m2){
	const mat3 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[1][0] * tempMatrix.m[0][1] + m1.m[2][0] * tempMatrix.m[0][2];
	m2->m[0][1] = m1.m[0][1] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[0][1] + m1.m[2][1] * tempMatrix.m[0][2];
	m2->m[0][2] = m1.m[0][2] * tempMatrix.m[0][0] + m1.m[1][2] * tempMatrix.m[0][1] + m1.m[2][2] * tempMatrix.m[0][2];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[1][0] + m1.m[1][0] * tempMatrix.m[1][1] + m1.m[2][0] * tempMatrix.m[1][2];
	m2->m[1][1] = m1.m[0][1] * tempMatrix.m[1][0] + m1.m[1][1] * tempMatrix.m[1][1] + m1.m[2][1] * tempMatrix.m[1][2];
	m2->m[1][2] = m1.m[0][2] * tempMatrix.m[1][0] + m1.m[1][2] * tempMatrix.m[1][1] + m1.m[2][2] * tempMatrix.m[1][2];

	m2->m[2][0] = m1.m[0][0] * tempMatrix.m[2][0] + m1.m[1][0] * tempMatrix.m[2][1] + m1.m[2][0] * tempMatrix.m[2][2];
	m2->m[2][1] = m1.m[0][1] * tempMatrix.m[2][0] + m1.m[1][1] * tempMatrix.m[2][1] + m1.m[2][1] * tempMatrix.m[2][2];
	m2->m[2][2] = m1.m[0][2] * tempMatrix.m[2][0] + m1.m[1][2] * tempMatrix.m[2][1] + m1.m[2][2] * tempMatrix.m[2][2];
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "out"!
void mat3MultiplyMat3Out(const mat3 m1, const mat3 m2, mat3 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2];
}

// Left-multiply "m2" by "m1" (m1*m2) and return the result!
mat3 mat3MultiplyMat3C(const mat3 m1, const mat3 m2){
	const mat3 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2],
		.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2],
		.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2],

		.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2],
		.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2],
		.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2]
	};

	return(out);
}


// Rotate a matrix using XYZ Euler angles!
void mat3RotateByEulerXYZ(mat3 *const restrict m, const float x, const float y, const float z){
	mat3 rotMatrix;
	mat3InitEulerXYZ(&rotMatrix, x, y, z);
	mat3MultiplyMat3P2(rotMatrix, m);
}

// Rotate a matrix using ZXY Euler angles!
void mat3RotateByEulerZXY(mat3 *const restrict m, const float x, const float y, const float z){
	mat3 rotMatrix;
	mat3InitEulerZXY(&rotMatrix, x, y, z);
	mat3MultiplyMat3P2(rotMatrix, m);
}

// Rotate a matrix using XYZ Euler angles!
mat3 mat3RotateByEulerXYZC(const mat3 m, const float x, const float y, const float z){
	const mat3 rotMatrix = mat3InitEulerXYZC(x, y, z);
	return(mat3MultiplyMat3C(rotMatrix, m));
}

// Rotate a matrix using ZXY Euler angles!
mat3 mat3RotateByEulerZXYC(const mat3 m, const float x, const float y, const float z){
	const mat3 rotMatrix = mat3InitEulerZXYC(x, y, z);
	return(mat3MultiplyMat3C(rotMatrix, m));
}

// Rotate a matrix by a vec3 using XYZ Euler angles!
void mat3RotateByVec3EulerXYZ(mat3 *const restrict m, const vec3 *const restrict v){
	mat3RotateByEulerXYZ(m, v->x, v->y, v->z);
}

// Rotate a matrix by a vec3 using ZXY Euler angles!
void mat3RotateByVec3EulerZXY(mat3 *const restrict m, const vec3 *const restrict v){
	mat3RotateByEulerZXY(m, v->x, v->y, v->z);
}

// Rotate a matrix by a vec3 using XYZ Euler angles!
mat3 mat3RotateByVec3EulerXYZC(const mat3 m, const vec3 v){
	return(mat3RotateByEulerXYZC(m, v.x, v.y, v.z));
}

// Rotate a matrix by a vec3 using ZXY Euler angles!
mat3 mat3RotateByVec3EulerZXYC(const mat3 m, const vec3 v){
	return(mat3RotateByEulerZXYC(m, v.x, v.y, v.z));
}

// Rotate a matrix by an axis and an angle!
void mat3RotateByAxisAngle(mat3 *const restrict m, const vec4 *const restrict v){
	mat3 rotMatrix;
	// Convert the axis angle to a rotation matrix!
	mat3InitAxisAngle(&rotMatrix, v);
	// Now rotate our matrix by it!
	mat3MultiplyMat3P2(rotMatrix, m);
}

// Rotate a matrix by an axis and an angle!
mat3 mat3RotateByAxisAngleC(const mat3 m, const vec4 v){
	return(mat3MultiplyMat3C(mat3InitAxisAngleC(v), m));
}

// Rotate a matrix by a quaternion!
void mat3RotateByQuat(mat3 *const restrict m, const quat *const restrict q){
	mat3 rotMatrix;
	// Convert the quaternion to a rotation matrix!
	mat3InitQuat(&rotMatrix, q);
	// Now rotate our matrix by it!
	mat3MultiplyMat3P2(rotMatrix, m);
}

// Rotate a matrix by a quaternion!
mat3 mat3RotateByQuatC(const mat3 m, const quat q){
	return(mat3MultiplyMat3C(mat3InitQuatC(q), m));
}

// Rotate a matrix on the X-axis!
void mat3RotateX(mat3 *const restrict m, const float x){
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
}

// Rotate a matrix on the X-axis!
mat3 mat3RotateXC(const mat3 m, const float x){
	const float cx = cosf(x);
	const float sx = sinf(x);

	const mat3 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = cx * m.m[0][1] - sx * m.m[0][2],
		.m[0][2] = sx * m.m[0][1] + cx * m.m[0][2],

		.m[1][0] = m.m[1][0],
		.m[1][1] = cx * m.m[1][1] - sx * m.m[1][2],
		.m[1][2] = sx * m.m[1][1] + cx * m.m[1][2],

		.m[2][0] = m.m[2][0],
		.m[2][1] = cx * m.m[2][1] - sx * m.m[2][2],
		.m[2][2] = sx * m.m[2][1] + cx * m.m[2][2]
	};

	return(out);
}

// Rotate a matrix on the Y-axis!
void mat3RotateY(mat3 *const restrict m, const float y){
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
}

// Rotate a matrix on the Y-axis!
mat3 mat3RotateYC(const mat3 m, const float y){
	const float cy = cosf(y);
	const float sy = sinf(y);

	const mat3 out = {
		.m[0][0] = cy * m.m[0][0] + sy * m.m[0][2],
		.m[0][1] = m.m[0][1],
		.m[0][2] = -sy * m.m[0][0] + cy * m.m[0][2],

		.m[1][0] = cy * m.m[1][0] + sy * m.m[1][2],
		.m[1][1] = m.m[1][1],
		.m[1][2] = -sy * m.m[1][0] + cy * m.m[1][2],

		.m[2][0] = cy * m.m[2][0] + sy * m.m[2][2],
		.m[2][1] = m.m[2][1],
		.m[2][2] = -sy * m.m[2][0] + cy * m.m[2][2]
	};

	return(out);
}

// Rotate a matrix on the Z-axis!
void mat3RotateZ(mat3 *const restrict m, const float z){
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
}

// Rotate a matrix on the Z-axis!
mat3 mat3RotateZC(const mat3 m, const float z){
	const float cz = cosf(z);
	const float sz = sinf(z);

	const mat3 out = {
		.m[0][0] = cz * m.m[0][0] - sz * m.m[0][1],
		.m[0][1] = sz * m.m[0][0] + cz * m.m[0][1],
		.m[0][2] = m.m[0][2],

		.m[1][0] = cz * m.m[1][0] - sz * m.m[1][1],
		.m[1][1] = sz * m.m[1][0] + cz * m.m[1][1],
		.m[1][2] = m.m[1][2],

		.m[2][0] = cz * m.m[2][0] - sz * m.m[2][1],
		.m[2][1] = sz * m.m[2][0] + cz * m.m[2][1],
		.m[2][2] = m.m[2][2]
	};

	return(out);
}

// Generate a rotation matrix that faces a target!
void mat3RotateToFace(mat3 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict worldUp){
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

	m->m[1][0] = up.x;
	m->m[1][1] = up.y;
	m->m[1][2] = up.z;

	m->m[2][0] = forward.x;
	m->m[2][1] = forward.y;
	m->m[2][2] = forward.z;
}

// Generate a rotation matrix that faces a target!
mat3 mat3RotateToFaceC(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3FastC(vec3SubtractVec3FromC(target, eye));
	const vec3 right   = vec3NormalizeVec3FastC(vec3CrossVec3C(worldUp, forward));
	const vec3 up      = vec3NormalizeVec3FastC(vec3CrossVec3C(forward, right));

	// Rotate the matrix to look at "target"!
	const mat3 m = {
		.m[0][0] = right.x,   .m[0][1] = right.y,   .m[0][2] = right.z,
		.m[1][0] = up.x,      .m[1][1] = up.y,      .m[1][2] = up.z,
		.m[2][0] = forward.x, .m[2][1] = forward.y, .m[2][2] = forward.z
	};

	return(m);
}


// Left-multiply a matrix by a scale matrix stored as three floats!
void mat3Scale(mat3 *const restrict m, const float x, const float y, const float z){
	m->m[0][0] *= x;
	m->m[0][1] *= y;
	m->m[0][2] *= z;

	m->m[1][0] *= x;
	m->m[1][1] *= y;
	m->m[1][2] *= z;

	m->m[2][0] *= x;
	m->m[2][1] *= y;
	m->m[2][2] *= z;
}

// Left-multiply a matrix by a scale matrix stored as three floats!
mat3 mat3ScaleC(mat3 m, const float x, const float y, const float z){
	m.m[0][0] *= x;
	m.m[0][1] *= y;
	m.m[0][2] *= z;

	m.m[1][0] *= x;
	m.m[1][1] *= y;
	m.m[1][2] *= z;

	m.m[2][0] *= x;
	m.m[2][1] *= y;
	m.m[2][2] *= z;

	return(m);
}

// Left-multiply a matrix by a scale matrix stored as a vec3!
void mat3ScaleVec3(mat3 *const restrict m, const vec3 *const restrict v){
	mat3Scale(m, v->x, v->y, v->z);
}

// Left-multiply a matrix by a scale matrix stored as a vec3!
mat3 mat3ScaleVec3C(mat3 m, const vec3 v){
	return(mat3ScaleC(m, v.x, v.y, v.z));
}

// Right-multiply a matrix by a scale matrix stored as three floats!
void mat3ScalePre(mat3 *const restrict m, const float x, const float y, const float z){
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
mat3 mat3ScalePreC(mat3 m, const float x, const float y, const float z){
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
void mat3ScalePreVec3(mat3 *const restrict m, const vec3 *const restrict v){
	mat3ScalePre(m, v->x, v->y, v->z);
}

// Right-multiply a matrix by a scale matrix stored as a vec3!
mat3 mat3ScalePreVec3C(mat3 m, const vec3 v){
	return(mat3ScalePreC(m, v.x, v.y, v.z));
}


/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat3Transpose(mat3 *const restrict m){
	const mat3 tempMatrix = *m;

	m->m[0][0] = tempMatrix.m[0][0];
	m->m[0][1] = tempMatrix.m[1][0];
	m->m[0][2] = tempMatrix.m[2][0];

	m->m[1][0] = tempMatrix.m[0][1];
	m->m[1][1] = tempMatrix.m[1][1];
	m->m[1][2] = tempMatrix.m[2][1];

	m->m[2][0] = tempMatrix.m[0][2];
	m->m[2][1] = tempMatrix.m[1][2];
	m->m[2][2] = tempMatrix.m[2][2];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat3TransposeOut(const mat3 m, mat3 *const restrict out){
	out->m[0][0] = m.m[0][0];
	out->m[0][1] = m.m[1][0];
	out->m[0][2] = m.m[2][0];

	out->m[1][0] = m.m[0][1];
	out->m[1][1] = m.m[1][1];
	out->m[1][2] = m.m[2][1];

	out->m[2][0] = m.m[0][2];
	out->m[2][1] = m.m[1][2];
	out->m[2][2] = m.m[2][2];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
mat3 mat3TransposeC(const mat3 m){
	const mat3 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = m.m[1][0],
		.m[0][2] = m.m[2][0],

		.m[1][0] = m.m[0][1],
		.m[1][1] = m.m[1][1],
		.m[1][2] = m.m[2][1],

		.m[2][0] = m.m[0][2],
		.m[2][1] = m.m[1][2],
		.m[2][2] = m.m[2][2]
	};

	return(out);
}

// Calculate the determinant of a matrix!
float mat3Determinant(const mat3 *const restrict m){
	return(
		m->m[0][0] * (m->m[1][1] * m->m[2][2] - m->m[1][2] * m->m[2][1]) +
		m->m[1][0] * (m->m[2][1] * m->m[0][2] - m->m[0][1] * m->m[2][2]) +
		m->m[2][0] * (m->m[0][1] * m->m[1][2] - m->m[1][1] * m->m[0][2])
	);
}

// Calculate the determinant of a matrix!
float mat3DeterminantC(const mat3 m){
	return(
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) +
		m.m[1][0] * (m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2]) +
		m.m[2][0] * (m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2])
	);
}

// Calculate the determinant of a matrix from its rows!
float mat3DeterminantColumns(const float *const restrict r0, const float *const restrict r1, const float *const restrict r2){
	return(
		r0[0] * (r1[1] * r2[2] - r1[2] * r2[1]) +
		r1[0] * (r2[1] * r0[2] - r0[1] * r2[2]) +
		r2[0] * (r0[1] * r1[2] - r1[1] * r0[2])
	);
}

// Invert a matrix!
void mat3Invert(mat3 *const restrict m){
	const mat3 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	// These values represent the cofactors c_(0,0), c_(0,1) and c_(0,2).
	const float c00 = tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[2][1] * tempMatrix.m[1][2];
	const float c01 = tempMatrix.m[2][1] * tempMatrix.m[0][2] - tempMatrix.m[0][1] * tempMatrix.m[2][2];
	const float c02 = tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[1][1] * tempMatrix.m[0][2];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * c00 + tempMatrix.m[1][0] * c01 + tempMatrix.m[2][0] * c02;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^(-1) = adj(m)/det(m), where
		//     adj(m)_(i,j) = c_(j,i).
		m->m[0][0] = c00 * invDet;
		m->m[0][1] = c01 * invDet;
		m->m[0][2] = c02 * invDet;
		m->m[1][0] = (tempMatrix.m[2][0] * tempMatrix.m[1][2] - tempMatrix.m[1][0] * tempMatrix.m[2][2]) * invDet;
		m->m[1][1] = (tempMatrix.m[0][0] * tempMatrix.m[2][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2]) * invDet;
		m->m[1][2] = (tempMatrix.m[0][2] * tempMatrix.m[1][0] - tempMatrix.m[0][0] * tempMatrix.m[1][2]) * invDet;
		m->m[2][0] = (tempMatrix.m[1][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[1][1]) * invDet;
		m->m[2][1] = (tempMatrix.m[0][1] * tempMatrix.m[2][0] - tempMatrix.m[0][0] * tempMatrix.m[2][1]) * invDet;
		m->m[2][2] = (tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[0][1] * tempMatrix.m[1][0]) * invDet;
	}
}

// Invert a matrix and store the result in "out"!
void mat3InvertOut(const mat3 m, mat3 *const restrict out){
	// We need to use these values twice, but we only need to calculate them once.
	// These values represent the cofactors c_(0,0), c_(0,1) and c_(0,2).
	const float c00 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];
	const float c01 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float c02 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * c00 + m.m[1][0] * c01 + m.m[2][0] * c02;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^(-1) = adj(m)/det(m), where
		//     adj(m)_(i,j) = c_(j,i).
		out->m[0][0] = c00 * invDet;
		out->m[0][1] = c01 * invDet;
		out->m[0][2] = c02 * invDet;
		out->m[1][0] = (m.m[2][0] * m.m[1][2] - m.m[1][0] * m.m[2][2]) * invDet;
		out->m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[2][0] * m.m[0][2]) * invDet;
		out->m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDet;
		out->m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]) * invDet;
		out->m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDet;
		out->m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDet;
	}
}

// Invert a matrix!
mat3 mat3InvertC(const mat3 m){
	// We need to use these values twice, but we only need to calculate them once.
	// These values represent the cofactors c_(0,0), c_(0,1) and c_(0,2).
	const float c00 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];
	const float c01 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float c02 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * c00 + m.m[1][0] * c01 + m.m[2][0] * c02;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		mat3 out;

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^(-1) = adj(m)/det(m), where
		//     adj(m)_(i,j) = c_(j,i).
		out.m[0][0] = c00 * invDet;
		out.m[0][1] = c01 * invDet;
		out.m[0][2] = c02 * invDet;
		out.m[1][0] = (m.m[2][0] * m.m[1][2] - m.m[1][0] * m.m[2][2]) * invDet;
		out.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[2][0] * m.m[0][2]) * invDet;
		out.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDet;
		out.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]) * invDet;
		out.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDet;
		out.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDet;

		return(out);
	}

	// If we couldn't invert the matrix, return the original one!
	return(m);
}

// Invert a matrix and return whether or not we were successful!
return_t mat3CanInvert(mat3 *const restrict m){
	const mat3 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	// These values represent the cofactors c_(0,0), c_(0,1) and c_(0,2).
	const float c00 = tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[2][1] * tempMatrix.m[1][2];
	const float c01 = tempMatrix.m[2][1] * tempMatrix.m[0][2] - tempMatrix.m[0][1] * tempMatrix.m[2][2];
	const float c02 = tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[1][1] * tempMatrix.m[0][2];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * c00 + tempMatrix.m[1][0] * c01 + tempMatrix.m[2][0] * c02;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^(-1) = adj(m)/det(m), where
		//     adj(m)_(i,j) = c_(j,i).
		m->m[0][0] = c00 * invDet;
		m->m[0][1] = c01 * invDet;
		m->m[0][2] = c02 * invDet;
		m->m[1][0] = (tempMatrix.m[2][0] * tempMatrix.m[1][2] - tempMatrix.m[1][0] * tempMatrix.m[2][2]) * invDet;
		m->m[1][1] = (tempMatrix.m[0][0] * tempMatrix.m[2][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2]) * invDet;
		m->m[1][2] = (tempMatrix.m[0][2] * tempMatrix.m[1][0] - tempMatrix.m[0][0] * tempMatrix.m[1][2]) * invDet;
		m->m[2][0] = (tempMatrix.m[1][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[1][1]) * invDet;
		m->m[2][1] = (tempMatrix.m[0][1] * tempMatrix.m[2][0] - tempMatrix.m[0][0] * tempMatrix.m[2][1]) * invDet;
		m->m[2][2] = (tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[0][1] * tempMatrix.m[1][0]) * invDet;


		return(1);
	}


	return(0);
}

/*
** Invert a matrix, storing the result in "out"
** and returning whether or not we were successful!
*/
return_t mat3CanInvertOut(const mat3 *const restrict m, mat3 *const restrict out){
	const mat3 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	// These values represent the cofactors c_(0,0), c_(0,1) and c_(0,2).
	const float c00 = tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[2][1] * tempMatrix.m[1][2];
	const float c01 = tempMatrix.m[2][1] * tempMatrix.m[0][2] - tempMatrix.m[0][1] * tempMatrix.m[2][2];
	const float c02 = tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[1][1] * tempMatrix.m[0][2];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * c00 + tempMatrix.m[1][0] * c01 + tempMatrix.m[2][0] * c02;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		// In terms of the adjugate, m^(-1) = adj(m)/det(m), where
		//     adj(m)_(i,j) = c_(j,i).
		out->m[0][0] = c00 * invDet;
		out->m[0][1] = c01 * invDet;
		out->m[0][2] = c02 * invDet;
		out->m[1][0] = (tempMatrix.m[2][0] * tempMatrix.m[1][2] - tempMatrix.m[1][0] * tempMatrix.m[2][2]) * invDet;
		out->m[1][1] = (tempMatrix.m[0][0] * tempMatrix.m[2][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2]) * invDet;
		out->m[1][2] = (tempMatrix.m[0][2] * tempMatrix.m[1][0] - tempMatrix.m[0][0] * tempMatrix.m[1][2]) * invDet;
		out->m[2][0] = (tempMatrix.m[1][0] * tempMatrix.m[2][1] - tempMatrix.m[2][0] * tempMatrix.m[1][1]) * invDet;
		out->m[2][1] = (tempMatrix.m[0][1] * tempMatrix.m[2][0] - tempMatrix.m[0][0] * tempMatrix.m[2][1]) * invDet;
		out->m[2][2] = (tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[0][1] * tempMatrix.m[1][0]) * invDet;


		return(1);
	}


	return(0);
}


/*
** Solves the system Ax = b using Cramer's rule.
** Cramer's rule states that the solution x is given by
**
** x = (det(A_0)/det(A), det(A_1)/det(A), det(A_2)/det(A))^T,
**
** where A_i is the matrix A with the ith column replace by b.
** This of course does not work if det(A) = 0.
*/
void mat3Solve(const mat3 *const restrict A, const vec3 *const restrict b, vec3 *const restrict x){
	float invDet = mat3Determinant(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// x_0 = det(A_0)/det(A)
		x->x = mat3DeterminantColumns((const float *const restrict)b, A->m[1], A->m[2]) * invDet;
		// x_1 = det(A_1)/det(A)
		x->y = mat3DeterminantColumns(A->m[0], (const float *const restrict)b, A->m[2]) * invDet;
		// x_2 = det(A_2)/det(A)
		x->z = mat3DeterminantColumns(A->m[0], A->m[1], (const float *const restrict)b) * invDet;
	}else{
		vec3InitZero(x);
	}
}

vec3 mat3SolveC(const mat3 A, const vec3 b){
	float invDet = mat3DeterminantC(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		return(vec3InitSetC(
			// x_0 = det(A_0)/det(A)
			mat3DeterminantColumns((const float *const restrict)&b, A.m[1], A.m[2]) * invDet,
			// x_1 = det(A_1)/det(A)
			mat3DeterminantColumns(A.m[0], (const float *const restrict)&b, A.m[2]) * invDet,
			// x_2 = det(A_2)/det(A)
			mat3DeterminantColumns(A.m[0], A.m[1], (const float *const restrict)&b) * invDet
		));
	}

	return(vec3InitZeroC());
}

/*
** Solves the system Ax = b using Cramer's
** rule and return whether we were successful.
*/
return_t mat3CanSolve(const mat3 *const restrict A, const vec3 *const restrict b, vec3 *const restrict x){
	float invDet = mat3Determinant(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// x_0 = det(A_0)/det(A)
		x->x = mat3DeterminantColumns((const float *const restrict)b, A->m[1], A->m[2]) * invDet;
		// x_1 = det(A_1)/det(A)
		x->y = mat3DeterminantColumns(A->m[0], (const float *const restrict)b, A->m[2]) * invDet;
		// x_2 = det(A_2)/det(A)
		x->z = mat3DeterminantColumns(A->m[0], A->m[1], (const float *const restrict)b) * invDet;

		return(1);
	}else{
		vec3InitZero(x);
	}

	return(0);
}

return_t mat3CanSolveC(const mat3 A, const vec3 b, vec3 *const restrict x){
	float invDet = mat3DeterminantC(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		*x = vec3InitSetC(
			// x_0 = det(A_0)/det(A)
			mat3DeterminantColumns((const float *const restrict)&b, A.m[1], A.m[2]) * invDet,
			// x_1 = det(A_1)/det(A)
			mat3DeterminantColumns(A.m[0], (const float *const restrict)&b, A.m[2]) * invDet,
			// x_2 = det(A_2)/det(A)
			mat3DeterminantColumns(A.m[0], A.m[1], (const float *const restrict)&b) * invDet
		);

		return(1);
	}else{
		*x = vec3InitZeroC();
	}

	return(0);
}


/*
** Diagonalize a symmetric 3x3 matrix A and return its eigenvalues
** and eigenvectors. Because our input matrix is symmetric, we need
** only specify the unique values.
**
** Rather than using an approximate iterative algorithm, we compute
** an exact analytic solution. Benchmarks seem to indicate that this
** is significantly faster and more accurate for the 3x3 case.
**
** This is used for non-uniform scaling in our affine transformation
** structure. Shears are stored as QSQ^T, where Q is a pure rotation
** and S is a scale along the x, y and z axes. This function returns
** Q as a quaternion and S as a vector.
**
** Special thanks to David Eberly for this implementation, which was
** given in "A Robust Eigensolver for 3x3 Symmetric Matrices" (2014).
*/
void mat3DiagonalizeSymmetric(
	float a00, float a01, float a02,
	float a11, float a12, float a22,
	vec3 *const restrict evals, quat *const restrict Q
){

	// Factor out the maximum absolute value of the matrix entries to
	// prevent floating-point overflow when computing the eigenvalues.
	const float max0 = floatMax(fabsf(a00), fabsf(a01));
	const float max1 = floatMax(fabsf(a02), fabsf(a11));
	const float max2 = floatMax(fabsf(a12), fabsf(a22));
	const float maxAbsElement = floatMax(floatMax(max0, max1), max2);
	// If the maximum is 0, A is the zero matrix.
	if(maxAbsElement <= MAT3_ZERO_EPSILON){
		evals->x = 0.f;
		evals->y = 0.f;
		evals->z = 0.f;
		quatInitIdentity(Q);
	}else{
		const float invMaxAbsElement = 1.f/maxAbsElement;
		a00 *= invMaxAbsElement;
		a01 *= invMaxAbsElement;
		a02 *= invMaxAbsElement;
		a11 *= invMaxAbsElement;
		a12 *= invMaxAbsElement;
		a22 *= invMaxAbsElement;

		{
			const float norm = a01*a01 + a02*a02 + a12*a12;
			// Early exit if A is diagonal.
			if(norm <= MAT3_ZERO_EPSILON){
				evals->x = a00;
				evals->y = a11;
				evals->z = a22;
				quatInitIdentity(Q);
			}else{
				mat3 Qmat;

				// q = tr(A)/3
				const float q = (a00 + a11 + a22)/3.f;
				// B = (A - qI)/p
				const float b00 = a00 - q;
				const float b11 = a11 - q;
				const float b22 = a22 - q;
				// p = sqrt(tr((A - qI)^2)/6)
				const float p = sqrtf((b00*b00 + b11*b11 + b22*b22 + 2.f*norm)/6.f);

				const float c00 = b11*b22 - a12*a12;
				const float c01 = a01*b22 - a12*a02;
				const float c02 = a01*a12 - b11*a02;
				// Compute the determinant of B. When we calculated the diagonals
				// of B, we did not divide by p: that's why we divide by p^3 here.
				const float det = (b00*c00 - a01*c01 + a02*c02)/(p*p*p);

				// Recall that det(B)/2 = cos(3theta). Because det(B)/2 may
				// lie outside the domain of acos, that is [-1, 1], due to
				// rounding errors, we clamp it to this interval.
				const float halfDet = floatClamp(0.5f*det, -1.f, 1.f);

				// Compute the (increasing) ordered eigenvalues of B.
				const float angle = acosf(halfDet)/3.f;
				const float beta2 = 2.f*cosf(angle);
				const float beta0 = 2.f*cosf(angle + TWO_PI_ON_THREE);
				const float beta1 = -(beta0 + beta2);
				// Compute the (increasing) ordered eigenvalues of A.
				evals->x = q + p*beta0;
				evals->y = q + p*beta1;
				evals->z = q + p*beta2;

				// Compute the eigenvectors so that they form
				// an ordered, right-handed, orthonormal set.
				if(halfDet >= 0.f){
					ComputeEigenvector0(a00, a01, a02, a11, a12, a22, evals->z, (vec3 *)Qmat.m[2]);
					ComputeEigenvector1(a00, a01, a02, a11, a12, a22, (vec3 *)Qmat.m[2], evals->y, (vec3 *)Qmat.m[1]);
					vec3CrossVec3Out((vec3 *)Qmat.m[1], (vec3 *)Qmat.m[2], (vec3 *)Qmat.m[0]);
				}else{
					ComputeEigenvector0(a00, a01, a02, a11, a12, a22, evals->x, (vec3 *)Qmat.m[0]);
					ComputeEigenvector1(a00, a01, a02, a11, a12, a22, (vec3 *)Qmat.m[0], evals->y, (vec3 *)Qmat.m[1]);
					vec3CrossVec3Out((vec3 *)Qmat.m[0], (vec3 *)Qmat.m[1], (vec3 *)Qmat.m[2]);
				}
				// Convert the matrix for Q to a quaternion.
				mat3ToQuat(&Qmat, Q);
			}
		}

		// Undo the preconditioning.
		evals->x *= maxAbsElement;
		evals->y *= maxAbsElement;
		evals->z *= maxAbsElement;
	}
}


/*
** Convert a 3x3 matrix to a quaternion and store the result in "out"!
** For this to work, we assume that "m" is a special orthogonal matrix.
** Implementation by Mike Day from Converting a Rotation Matrix to a Quaternion.
*/
void mat3ToQuat(const mat3 *const restrict m, quat *const restrict out){
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
** Convert a 3x3 pure rotation matrix to a quaternion!
** Implementation by Mike Day from Converting a Rotation Matrix to a Quaternion.
*/
quat mat3ToQuatC(const mat3 m){
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
void mat3ToQuatAlt(const mat3 *const restrict m, quat *const restrict out){
	out->x = copySignZero(0.5f * sqrtf( m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f), m->m[1][2] - m->m[2][1]);
	out->y = copySignZero(0.5f * sqrtf(-m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f), m->m[2][0] - m->m[0][2]);
	out->z = copySignZero(0.5f * sqrtf(-m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f), m->m[0][1] - m->m[1][0]);
	out->w = 0.5f * sqrtf(m->m[0][0] + m->m[1][1] + m->m[2][2] + 1.f);
}

quat mat3ToQuatAltC(const mat3 m){
	const quat q = {
		.x = copySignZero(0.5f * sqrtf( m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f), m.m[1][2] - m.m[2][1]),
		.y = copySignZero(0.5f * sqrtf(-m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f), m.m[2][0] - m.m[0][2]),
		.z = copySignZero(0.5f * sqrtf(-m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f), m.m[0][1] - m.m[1][0]),
		.w = 0.5f * sqrtf(m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.f)
	};
	return(q);
}

// Convert a quaternion to a 3x3 matrix and store the result in "out"!
void quatToMat3(const quat *const restrict q, mat3 *const restrict out){
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
}

// Convert a quaternion to a 3x3 matrix!
mat3 quatToMat3C(const quat q){
	const float xx = q.x*q.x;
	const float yy = q.y*q.y;
	const float zz = q.z*q.z;
	float temp1;
	float temp2;
	mat3 out;

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

	return(out);
}


// Compute a normalized eigenvector for eigenvalue 0.
static void ComputeEigenvector0(
	const float a00, const float a01, const float a02,
	const float a11, const float a12, const float a22,
	const float eval0, vec3 *const evec0
){

	// We know that (A - eval0*I) has row rank 2, but we don't know which two rows
	// are linearly independent. To get the best results for the widest range of
	// inputs, we use the two rows whose cross product has the greatest magnitude.
	const vec3 r0 = {.x = a00 - eval0, .y = a01,         .z = a02        };
	const vec3 r1 = {.x = a01,         .y = a11 - eval0, .z = a12        };
	const vec3 r2 = {.x = a02,         .y = a12,         .z = a22 - eval0};
	vec3 r01, r12, r20;
	vec3CrossVec3Out(&r0, &r1, &r01);
	vec3CrossVec3Out(&r1, &r2, &r12);
	vec3CrossVec3Out(&r2, &r0, &r20);

	{
		const float d01 = vec3DotVec3(&r01, &r01);
		const float d12 = vec3DotVec3(&r12, &r12);
		const float d20 = vec3DotVec3(&r20, &r20);
		if(d01 >= d12 && d01 >= d20){
			vec3MultiplySOut(&r01, invSqrtFast(d01), evec0);
		}else if(d12 >= d20){
			vec3MultiplySOut(&r12, invSqrtFast(d12), evec0);
		}else{
			vec3MultiplySOut(&r20, invSqrtFast(d20), evec0);
		}
	}
}

/*
** Compute a normalized eigenvector for eigenvalue 1 that
** is orthogonal to the one computed for eigenvalue 0.
*/
static void ComputeEigenvector1(
	const float a00, const float a01, const float a02,
	const float a11, const float a12, const float a22,
	const vec3 *const evec0, const float eval1, vec3 *const evec1
){

	vec3 u, v;
	// Because A is symmetric, it is orthogonally diagonalizable,
	// so we can choose two eigenvectors to be the orthogonal, even
	// if their eigenvalues are the same. Here, u and v define the
	// orthogonal complement of evec0, so evec1 is of the form
	//     evec1 = x_0 u + x_1 v,
	// where x_0^2 + x_1^2 = 1 since u, v, evec1 are unit length.
	orthonormalBasisFaster(evec0, &u, &v);

	// Using the above terminology, suppose we define
	//     x = (x_0, x_1)^T,
	//     J = [u, v],
	//     M = J^T*(A - eval1*I)*J.
	// Then evec1 = Jx and 0 = (A - eval1*I)*evec1 = Mx.
	// Here, M is a 2x2 matrix. If eval0 = eval1, then M
	// has rank 0, and we can choose any valid x for evec1.
	// Otherwise, it has rank 1, and x is easily solvable.
	{
		const vec3 Au = {
			.x = a00*u.x + a01*u.y + a02*u.z,
			.y = a01*u.x + a11*u.y + a12*u.z,
			.z = a02*u.x + a12*u.y + a22*u.z
		};
		const vec3 Av = {
			.x = a00*v.x + a01*v.y + a02*v.z,
			.y = a01*v.x + a11*v.y + a12*v.z,
			.z = a02*v.x + a12*v.y + a22*v.z
		};
		float m00 = vec3DotVec3(&u, &Au) - eval1;
		float m01 = vec3DotVec3(&u, &Av);
		float m11 = vec3DotVec3(&v, &Av) - eval1;

		// The eigenvector is perpendicular to the (non-zero) row of M.
		// Choose the row with the greatest magnitude for the best results.
		const float m00abs = fabsf(m00);
		const float m01abs = fabsf(m01);
		const float m11abs = fabsf(m11);
		if(m00abs >= m11abs){
			if(m00abs > MAT3_ZERO_EPSILON || m01abs > MAT3_ZERO_EPSILON){
				if(m00abs >= m01abs){
					m01 /= m00;
					m00 = 1.f/sqrtf(1.f + m01*m01);
					m01 *= m00;
				}else{
					m00 /= m01;
					m01 = 1.f/sqrtf(1.f + m00*m00);
					m00 *= m01;
				}
				evec1->x = m01*u.x - m00*v.x;
				evec1->y = m01*u.y - m00*v.y;
				evec1->z = m01*u.z - m00*v.z;
			}else{
				*evec1 = u;
			}
		}else{
			if(m11abs > MAT3_ZERO_EPSILON || m01abs > MAT3_ZERO_EPSILON){
				if(m11abs >= m01abs){
					m01 /= m11;
					m11 = 1.f/sqrtf(1.f + m01*m01);
					m01 *= m11;
				}else{
					m11 /= m01;
					m01 = 1.f/sqrtf(1.f + m11*m11);
					m11 *= m01;
				}
				evec1->x = m11*u.x - m01*v.x;
				evec1->y = m11*u.y - m01*v.y;
				evec1->z = m11*u.z - m01*v.z;
			}else{
				*evec1 = u;
			}
		}
	}
}