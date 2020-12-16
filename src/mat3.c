#include "mat3.h"


#include <string.h>

#include "utilMath.h"


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


// Multiply a matrix by a vec3!
void mat3MultiplyByVec3(const mat3 *const restrict m, vec3 *const restrict v){
	vec3 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z;
}

// Multiply a matrix by a vec3 and store the result in "out"! This assumes that "out" isn't "v".
void mat3MultiplyByVec3Out(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;
}

// Multiply a matrix by a vec3!
vec3 mat3MultiplyByVec3C(const mat3 m, const vec3 v){
	const vec3 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z
	};

	return(out);
}

// Multiply a vec3 by a matrix!
void mat3MultiplyVec3By(mat3 *const restrict m, const vec3 *const restrict v){
	const mat3 tempMatrix = *m;

	m->m[0][0] =
	m->m[0][1] =
	m->m[0][2] = tempMatrix.m[0][0] * v->x + tempMatrix.m[0][1] * v->y + tempMatrix.m[0][2] * v->z;

	m->m[1][0] =
	m->m[1][1] =
	m->m[1][2] = tempMatrix.m[1][0] * v->x + tempMatrix.m[1][1] * v->y + tempMatrix.m[1][2] * v->z;

	m->m[2][0] =
	m->m[2][1] =
	m->m[2][2] = tempMatrix.m[2][0] * v->x + tempMatrix.m[2][1] * v->y + tempMatrix.m[2][2] * v->z;
}

// Multiply a vec3 by a matrix and store the result in "out"!
void mat3MultiplyVec3ByOut(const mat3 m, const vec3 *const restrict v, mat3 *const restrict out){
	out->m[0][0] =
	out->m[0][1] =
	out->m[0][2] = m.m[0][0] * v->x + m.m[0][1] * v->y + m.m[0][2] * v->z;

	out->m[1][0] =
	out->m[1][1] =
	out->m[1][2] = m.m[1][0] * v->x + m.m[1][1] * v->y + m.m[1][2] * v->z;

	out->m[2][0] =
	out->m[2][1] =
	out->m[2][2] = m.m[2][0] * v->x + m.m[2][1] * v->y + m.m[2][2] * v->z;
}

// Multiply a vec3 by a matrix!
mat3 mat3MultiplyVec3ByC(const mat3 m, const vec3 v){
	mat3 out;

	out.m[0][0] =
	out.m[0][1] =
	out.m[0][2] = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z;

	out.m[1][0] =
	out.m[1][1] =
	out.m[1][2] = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z;

	out.m[2][0] =
	out.m[2][1] =
	out.m[2][2] = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z;

	return(out);
}

// Right-multiply "m1" by "m2" (m1*m2)!
void mat3MultiplyByMat3(mat3 *const restrict m1, const mat3 m2){
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

// Left-multiply "m1" by "m2" (m2*m1)!
void mat3MultiplyMat3By(mat3 *const restrict m1, const mat3 m2){
	const mat3 tempMatrix = *m1;

	m1->m[0][0] = m2.m[0][0] * tempMatrix.m[0][0] + m2.m[1][0] * tempMatrix.m[0][1] + m2.m[2][0] * tempMatrix.m[0][2];
	m1->m[0][1] = m2.m[0][1] * tempMatrix.m[0][0] + m2.m[1][1] * tempMatrix.m[0][1] + m2.m[2][1] * tempMatrix.m[0][2];
	m1->m[0][2] = m2.m[0][2] * tempMatrix.m[0][0] + m2.m[1][2] * tempMatrix.m[0][1] + m2.m[2][2] * tempMatrix.m[0][2];

	m1->m[1][0] = m2.m[0][0] * tempMatrix.m[1][0] + m2.m[1][0] * tempMatrix.m[1][1] + m2.m[2][0] * tempMatrix.m[1][2];
	m1->m[1][1] = m2.m[0][1] * tempMatrix.m[1][0] + m2.m[1][1] * tempMatrix.m[1][1] + m2.m[2][1] * tempMatrix.m[1][2];
	m1->m[1][2] = m2.m[0][2] * tempMatrix.m[1][0] + m2.m[1][2] * tempMatrix.m[1][1] + m2.m[2][2] * tempMatrix.m[1][2];

	m1->m[2][0] = m2.m[0][0] * tempMatrix.m[2][0] + m2.m[1][0] * tempMatrix.m[2][1] + m2.m[2][0] * tempMatrix.m[2][2];
	m1->m[2][1] = m2.m[0][1] * tempMatrix.m[2][0] + m2.m[1][1] * tempMatrix.m[2][1] + m2.m[2][1] * tempMatrix.m[2][2];
	m1->m[2][2] = m2.m[0][2] * tempMatrix.m[2][0] + m2.m[1][2] * tempMatrix.m[2][1] + m2.m[2][2] * tempMatrix.m[2][2];
}

// Right-multiply "m1" by "m2" (m1*m2) and store the result in "out"!
void mat3MultiplyByMat3Out(const mat3 m1, const mat3 m2, mat3 *const restrict out){
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

// Right-multiply "m1" by "m2" (m1*m2)!
mat3 mat3MultiplyByMat3C(const mat3 m1, const mat3 m2){
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


// Left-multiply a matrix by a scale matrix stored as three floats!
void mat3ScalePre(mat3 *const restrict m, const float x, const float y, const float z){
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
mat3 mat3ScalePreC(mat3 m, const float x, const float y, const float z){
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
void mat3ScalePreVec3(mat3 *const restrict m, const vec3 *const restrict v){
	mat3ScalePre(m, v->x, v->y, v->z);
}

// Left-multiply a matrix by a scale matrix stored as a vec3!
mat3 mat3ScalePreVec3C(mat3 m, const vec3 v){
	return(mat3ScalePreC(m, v.x, v.y, v.z));
}

// Right-multiply a matrix by a scale matrix stored as three floats!
void mat3Scale(mat3 *const restrict m, const float x, const float y, const float z){
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
mat3 mat3ScaleC(mat3 m, const float x, const float y, const float z){
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
void mat3ScaleVec3(mat3 *const restrict m, const vec3 *const restrict v){
	mat3Scale(m, v->x, v->y, v->z);
}

// Right-multiply a matrix by a scale matrix stored as a vec3!
mat3 mat3ScaleVec3C(mat3 m, const vec3 v){
	return(mat3ScaleC(m, v.x, v.y, v.z));
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

// Invert a matrix!
void mat3Invert(mat3 *const restrict m){
	const mat3 tempMatrix = *m;

	// We need to use these values twice, but we only need to calculate them once.
	const float f0 = tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[1][2] * tempMatrix.m[2][1];
	const float f1 = tempMatrix.m[2][1] * tempMatrix.m[0][2] - tempMatrix.m[0][1] * tempMatrix.m[2][2];
	const float f2 = tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[1][1] * tempMatrix.m[0][2];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * f0 +
	               tempMatrix.m[1][0] * f1 +
	               tempMatrix.m[2][0] * f2;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		m->m[0][0] = f0 * invDet;
		m->m[0][1] = f1 * invDet;
		m->m[0][2] = f2 * invDet;
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
	const float f0 = m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1];
	const float f1 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float f2 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * f0 +
	               m.m[1][0] * f1 +
	               m.m[2][0] * f2;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		out->m[0][0] = f0 * invDet;
		out->m[0][1] = f1 * invDet;
		out->m[0][2] = f2 * invDet;
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
	const float f0 = m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1];
	const float f1 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float f2 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * f0 +
	               m.m[1][0] * f1 +
	               m.m[2][0] * f2;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		mat3 out;

		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		out.m[0][0] = f0 * invDet;
		out.m[0][1] = f1 * invDet;
		out.m[0][2] = f2 * invDet;
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
	const float f0 = tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[1][2] * tempMatrix.m[2][1];
	const float f1 = tempMatrix.m[2][1] * tempMatrix.m[0][2] - tempMatrix.m[0][1] * tempMatrix.m[2][2];
	const float f2 = tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[1][1] * tempMatrix.m[0][2];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * f0 +
	               tempMatrix.m[1][0] * f1 +
	               tempMatrix.m[2][0] * f2;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		m->m[0][0] = f0 * invDet;
		m->m[0][1] = f1 * invDet;
		m->m[0][2] = f2 * invDet;
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
	const float f0 = tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[1][2] * tempMatrix.m[2][1];
	const float f1 = tempMatrix.m[2][1] * tempMatrix.m[0][2] - tempMatrix.m[0][1] * tempMatrix.m[2][2];
	const float f2 = tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[1][1] * tempMatrix.m[0][2];
	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * f0 +
	               tempMatrix.m[1][0] * f1 +
	               tempMatrix.m[2][0] * f2;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		out->m[0][0] = f0 * invDet;
		out->m[0][1] = f1 * invDet;
		out->m[0][2] = f2 * invDet;
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
** Invert a matrix, storing the result in "out"
** and returning whether or not we were successful!
*/
return_t mat3CanInvertC(const mat3 m, mat3 *const restrict out){
	// We need to use these values twice, but we only need to calculate them once.
	const float f0 = m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1];
	const float f1 = m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2];
	const float f2 = m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2];
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * f0 +
	               m.m[1][0] * f1 +
	               m.m[2][0] * f2;

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// Now use the determinant to find the inverse of the matrix!
		out->m[0][0] = f0 * invDet;
		out->m[0][1] = f1 * invDet;
		out->m[0][2] = f2 * invDet;
		out->m[1][0] = (m.m[2][0] * m.m[1][2] - m.m[1][0] * m.m[2][2]) * invDet;
		out->m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[2][0] * m.m[0][2]) * invDet;
		out->m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * invDet;
		out->m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]) * invDet;
		out->m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * invDet;
		out->m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * invDet;


		return(1);
	}


	return(0);
}


/*
** Solves the system Ax = b using Cramer's rule.
** Cramer's rule states that the solution x is given by
**
** x = (det(A_1)/det(A), det(A_2)/det(A), det(A_3)/det(A))^T,
**
** where A_i is the matrix A with the ith column replace by b.
** This of course does not work if det(A) = 0.
*/
void mat3Solve(const mat3 *const restrict A, const vec3 *const restrict b, vec3 *const restrict x){
	float invDet = mat3Determinant(A);
	if(invDet != 0.f){
		mat3 Ai;

		invDet = 1.f / invDet;

		// x_1 = det(A_1)/det(A)
		memcpy(Ai.m[0], b, sizeof(vec3));
		memcpy(Ai.m[1], A->m[1], sizeof(vec3)+sizeof(vec3));
		x->x = mat3Determinant(&Ai) * invDet;

		// x_2 = det(A_2)/det(A)
		memcpy(Ai.m[0], A->m[0], sizeof(vec3));
		memcpy(Ai.m[1], b, sizeof(vec3));
		x->y = mat3Determinant(&Ai) * invDet;

		// x_3 = det(A_3)/det(A)
		memcpy(Ai.m[1], A->m[1], sizeof(vec3));
		memcpy(Ai.m[2], b, sizeof(vec3));
		x->z = mat3Determinant(&Ai) * invDet;
	}
}

vec3 mat3SolveC(const mat3 A, const vec3 b){
	float invDet = mat3DeterminantC(A);
	if(invDet != 0.f){
		mat3 Ai;
		vec3 x;

		invDet = 1.f / invDet;

		// x_1 = det(A_1)/det(A)
		memcpy(Ai.m[0], &b, sizeof(vec3));
		memcpy(Ai.m[1], A.m[1], sizeof(vec3)+sizeof(vec3));
		x.x = mat3DeterminantC(Ai) * invDet;

		// x_2 = det(A_2)/det(A)
		memcpy(Ai.m[0], A.m[0], sizeof(vec3));
		memcpy(Ai.m[1], &b, sizeof(vec3));
		x.y = mat3DeterminantC(Ai) * invDet;

		// x_3 = det(A_3)/det(A)
		memcpy(Ai.m[1], A.m[1], sizeof(vec3));
		memcpy(Ai.m[2], &b, sizeof(vec3));
		x.z = mat3DeterminantC(Ai) * invDet;

		return(x);
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
		mat3 Ai;

		invDet = 1.f / invDet;

		// x_1 = det(A_1)/det(A)
		memcpy(Ai.m[0], b, sizeof(vec3));
		memcpy(Ai.m[1], A->m[1], sizeof(vec3)+sizeof(vec3));
		x->x = mat3Determinant(&Ai) * invDet;

		// x_2 = det(A_2)/det(A)
		memcpy(Ai.m[0], A->m[0], sizeof(vec3));
		memcpy(Ai.m[1], b, sizeof(vec3));
		x->y = mat3Determinant(&Ai) * invDet;

		// x_3 = det(A_3)/det(A)
		memcpy(Ai.m[1], A->m[1], sizeof(vec3));
		memcpy(Ai.m[2], b, sizeof(vec3));
		x->z = mat3Determinant(&Ai) * invDet;

		return(1);
	}

	return(0);
}

return_t mat3CanSolveC(const mat3 A, const vec3 b, vec3 *const restrict x){
	float invDet = mat3DeterminantC(A);
	if(invDet != 0.f){
		mat3 Ai;

		invDet = 1.f / invDet;

		// x_1 = det(A_1)/det(A)
		memcpy(Ai.m[0], &b, sizeof(vec3));
		memcpy(Ai.m[1], A.m[1], sizeof(vec3)+sizeof(vec3));
		x->x = mat3DeterminantC(Ai) * invDet;

		// x_2 = det(A_2)/det(A)
		memcpy(Ai.m[0], A.m[0], sizeof(vec3));
		memcpy(Ai.m[1], &b, sizeof(vec3));
		x->y = mat3DeterminantC(Ai) * invDet;

		// x_3 = det(A_3)/det(A)
		memcpy(Ai.m[1], A.m[1], sizeof(vec3));
		memcpy(Ai.m[2], &b, sizeof(vec3));
		x->z = mat3DeterminantC(Ai) * invDet;

		return(1);
	}

	return(0);
}


/*
** Convert a 3x3 matrix to a quaternion and store the result in "out"!
** For this to work, we assume that "m" is a special orthogonal matrix.
*/
void mat3ToQuat(const mat3 *const restrict m, quat *const restrict out){
	const float trace = m->m[0][0] + m->m[1][1] + m->m[2][2];

	if(trace > 0){
		const float S = 0.5f * invSqrtFast(trace + 1.f);
		quatInitSet(out,
			(m->m[1][2] - m->m[2][1]) * S,
			(m->m[2][0] - m->m[0][2]) * S,
			(m->m[0][1] - m->m[1][0]) * S,
			0.25f / S
		);
	}else if(m->m[0][0] > m->m[1][1] && m->m[0][0] > m->m[2][2]){
		const float S = 0.5f * invSqrtFast(m->m[0][0] - m->m[1][1] - m->m[2][2] + 1.f);
		quatInitSet(out,
			0.25f / S,
			(m->m[0][1] + m->m[1][0]) * S,
			(m->m[2][0] + m->m[0][2]) * S,
			(m->m[1][2] - m->m[2][1]) * S
		);
	}else if(m->m[1][1] > m->m[2][2]){
		const float S = 0.5f * invSqrtFast(-m->m[0][0] + m->m[1][1] - m->m[2][2] + 1.f);
		quatInitSet(out,
			(m->m[0][1] + m->m[1][0]) * S,
			0.25f / S,
			(m->m[1][2] - m->m[2][1]) * S,
			(m->m[2][0] + m->m[0][2]) * S
		);
	}else{
		const float S = 0.5f * invSqrtFast(-m->m[0][0] - m->m[1][1] + m->m[2][2] + 1.f);
		quatInitSet(out,
			(m->m[2][0] + m->m[0][2]) * S,
			(m->m[1][2] - m->m[2][1]) * S,
			0.25f / S,
			(m->m[0][1] + m->m[1][0]) * S
		);
	}
}

// Convert a 3x3 matrix to a quaternion!
quat mat3ToQuatC(const mat3 m){
	const float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	if(trace > 0){
		const float S = 0.5f * invSqrtFast(trace + 1.f);
		const quat q = {
			.x = (m.m[1][2] - m.m[2][1]) * S,
			.y = (m.m[2][0] - m.m[0][2]) * S,
			.z = (m.m[0][1] - m.m[1][0]) * S,
			.w = 0.25f / S
		};
		return(q);
	}else if(m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]){
		const float S = 0.5f * invSqrtFast(m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.f);
		const quat q = {
			.x = 0.25f / S,
			.y = (m.m[0][1] + m.m[1][0]) * S,
			.z = (m.m[2][0] + m.m[0][2]) * S,
			.w = (m.m[1][2] - m.m[2][1]) * S
		};
		return(q);
	}else if(m.m[1][1] > m.m[2][2]){
		const float S = 0.5f * invSqrtFast(-m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.f);
		const quat q = {
			.x = (m.m[0][1] + m.m[1][0]) * S,
			.y = 0.25f / S,
			.z = (m.m[1][2] - m.m[2][1]) * S,
			.w = (m.m[2][0] + m.m[0][2]) * S
		};
		return(q);
	}else{
		const float S = 0.5f * invSqrtFast(-m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.f);
		const quat q = {
			.x = (m.m[2][0] + m.m[0][2]) * S,
			.y = (m.m[1][2] - m.m[2][1]) * S,
			.z = 0.25f / S,
			.w = (m.m[0][1] + m.m[1][0]) * S
		};
		return(q);
	}
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