#include "mat3.h"


#include <string.h>

#include "utilMath.h"


mat3 identityMat3 = {
	.m[0][0] = 1.f, .m[0][1] = 0.f, .m[0][2] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f, .m[1][2] = 0.f,
	.m[2][0] = 0.f, .m[2][1] = 0.f, .m[2][2] = 1.f
};


// Initialize the matrix's values to 0!
void mat3InitZero(mat3 *m){
	memset(m, 0.f, sizeof(*m));
}

// Initialize the matrix's values to 0!
mat3 mat3InitZeroR(){
	mat3 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

// Initialize the matrix to an identity matrix!
void mat3InitIdentity(mat3 *m){
	*m = identityMat3;
}

// Initialize the matrix to an identity matrix!
mat3 mat3InitIdentityR(){
	return(identityMat3);
}

// Initialize the matrix to a uniform scale matrix!
void mat3InitDiagonal(mat3 *m, const float x){
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
mat3 mat3InitDiagonalR(const float x){
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
void mat3InitSkew(mat3 *m, const vec3 *v){
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
mat3 mat3InitSkewR(const vec3 v){
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
void mat3AddMat3(mat3 *m1, const mat3 *m2){
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
void mat3AddMat3Out(const mat3 *m1, const mat3 *m2, mat3 *out){
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
mat3 mat3AddMat3R(const mat3 m1, const mat3 m2){
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
void mat3MultiplyByVec3(const mat3 *m, vec3 *v){
	vec3 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z;
}

// Multiply a matrix by a vec3 and store the result in "out"! This assumes that "out" isn't "v".
void mat3MultiplyByVec3Out(const mat3 *m, const vec3 *v, vec3 *out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;
}

// Multiply a matrix by a vec3!
vec3 mat3MultiplyByVec3R(const mat3 m, const vec3 v){
	const vec3 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
		.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z
	};

	return(out);
}

// Multiply a vec3 by a matrix!
void mat3MultiplyVec3By(mat3 *m, const vec3 *v){
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
void mat3MultiplyVec3ByOut(const mat3 m, const vec3 *v, mat3 *out){
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
mat3 mat3MultiplyVec3ByR(const mat3 m, const vec3 v){
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

// Multiply "m1" by "m2"!
void mat3MultiplyByMat3(mat3 *m1, const mat3 *m2){
	const mat3 tempMatrix1 = *m1;
	const mat3 tempMatrix2 = *m2;

	m1->m[0][0] = tempMatrix1.m[0][0] * tempMatrix2.m[0][0] + tempMatrix1.m[1][0] * tempMatrix2.m[0][1] + tempMatrix1.m[2][0] * tempMatrix2.m[0][2];
	m1->m[0][1] = tempMatrix1.m[0][1] * tempMatrix2.m[0][0] + tempMatrix1.m[1][1] * tempMatrix2.m[0][1] + tempMatrix1.m[2][1] * tempMatrix2.m[0][2];
	m1->m[0][2] = tempMatrix1.m[0][2] * tempMatrix2.m[0][0] + tempMatrix1.m[1][2] * tempMatrix2.m[0][1] + tempMatrix1.m[2][2] * tempMatrix2.m[0][2];

	m1->m[1][0] = tempMatrix1.m[0][0] * tempMatrix2.m[1][0] + tempMatrix1.m[1][0] * tempMatrix2.m[1][1] + tempMatrix1.m[2][0] * tempMatrix2.m[1][2];
	m1->m[1][1] = tempMatrix1.m[0][1] * tempMatrix2.m[1][0] + tempMatrix1.m[1][1] * tempMatrix2.m[1][1] + tempMatrix1.m[2][1] * tempMatrix2.m[1][2];
	m1->m[1][2] = tempMatrix1.m[0][2] * tempMatrix2.m[1][0] + tempMatrix1.m[1][2] * tempMatrix2.m[1][1] + tempMatrix1.m[2][2] * tempMatrix2.m[1][2];

	m1->m[2][0] = tempMatrix1.m[0][0] * tempMatrix2.m[2][0] + tempMatrix1.m[1][0] * tempMatrix2.m[2][1] + tempMatrix1.m[2][0] * tempMatrix2.m[2][2];
	m1->m[2][1] = tempMatrix1.m[0][1] * tempMatrix2.m[2][0] + tempMatrix1.m[1][1] * tempMatrix2.m[2][1] + tempMatrix1.m[2][1] * tempMatrix2.m[2][2];
	m1->m[2][2] = tempMatrix1.m[0][2] * tempMatrix2.m[2][0] + tempMatrix1.m[1][2] * tempMatrix2.m[2][1] + tempMatrix1.m[2][2] * tempMatrix2.m[2][2];
}

// Multiply "m2" by "m1"!
void mat3MultiplyMat3By(mat3 *m1, const mat3 *m2){
	const mat3 tempMatrix1 = *m1;
	const mat3 tempMatrix2 = *m2;

	m1->m[0][0] = tempMatrix2.m[0][0] * tempMatrix1.m[0][0] + tempMatrix2.m[1][0] * tempMatrix1.m[0][1] + tempMatrix2.m[2][0] * tempMatrix1.m[0][2];
	m1->m[0][1] = tempMatrix2.m[0][1] * tempMatrix1.m[0][0] + tempMatrix2.m[1][1] * tempMatrix1.m[0][1] + tempMatrix2.m[2][1] * tempMatrix1.m[0][2];
	m1->m[0][2] = tempMatrix2.m[0][2] * tempMatrix1.m[0][0] + tempMatrix2.m[1][2] * tempMatrix1.m[0][1] + tempMatrix2.m[2][2] * tempMatrix1.m[0][2];

	m1->m[1][0] = tempMatrix2.m[0][0] * tempMatrix1.m[1][0] + tempMatrix2.m[1][0] * tempMatrix1.m[1][1] + tempMatrix2.m[2][0] * tempMatrix1.m[1][2];
	m1->m[1][1] = tempMatrix2.m[0][1] * tempMatrix1.m[1][0] + tempMatrix2.m[1][1] * tempMatrix1.m[1][1] + tempMatrix2.m[2][1] * tempMatrix1.m[1][2];
	m1->m[1][2] = tempMatrix2.m[0][2] * tempMatrix1.m[1][0] + tempMatrix2.m[1][2] * tempMatrix1.m[1][1] + tempMatrix2.m[2][2] * tempMatrix1.m[1][2];

	m1->m[2][0] = tempMatrix2.m[0][0] * tempMatrix1.m[2][0] + tempMatrix2.m[1][0] * tempMatrix1.m[2][1] + tempMatrix2.m[2][0] * tempMatrix1.m[2][2];
	m1->m[2][1] = tempMatrix2.m[0][1] * tempMatrix1.m[2][0] + tempMatrix2.m[1][1] * tempMatrix1.m[2][1] + tempMatrix2.m[2][1] * tempMatrix1.m[2][2];
	m1->m[2][2] = tempMatrix2.m[0][2] * tempMatrix1.m[2][0] + tempMatrix2.m[1][2] * tempMatrix1.m[2][1] + tempMatrix2.m[2][2] * tempMatrix1.m[2][2];
}

// Multiply "m1" by "m2" and store the result in "out"!
void mat3MultiplyByMat3Out(const mat3 m1, const mat3 m2, mat3 *out){
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

// Multiply "m1" by "m2"!
mat3 mat3MultiplyByMat3R(const mat3 m1, const mat3 m2){
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


/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat3Transpose(mat3 *m){
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
void mat3TransposeOut(const mat3 m, mat3 *out){
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
mat3 mat3TransposeR(const mat3 m){
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

// Invert a matrix!
void mat3Invert(mat3 *m){
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
void mat3InvertOut(const mat3 m, mat3 *out){
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
mat3 mat3InvertR(const mat3 m){
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
return_t mat3CanInvert(mat3 *m){
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
return_t mat3CanInvertOut(const mat3 m, mat3 *out){
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


// Convert a quaternion to a 3x3 matrix and store the result in "out"!
void quatToMat3(const quat *q, mat3 *out){
	const float xx = q->x*q->x;
	const float yy = q->y*q->y;
	const float zz = q->z*q->z;
	float temp1;
	float temp2;

	out->m[0][0] = 1 - 2*(yy + zz);
	out->m[1][1] = 1 - 2*(xx - zz);
	out->m[2][2] = 1 - 2*(xx - yy);

	temp1 = q->x*q->y;
	temp2 = q->w*q->z;
	out->m[0][1] = 2*(temp1 - temp2);
	out->m[1][0] = 2*(temp1 + temp2);

	temp1 = q->x*q->z;
	temp2 = q->w*q->y;
	out->m[0][2] = 2*(temp1 + temp2);
	out->m[2][0] = 2*(temp1 - temp2);

	temp1 = q->y*q->z;
	temp2 = q->w*q->x;
	out->m[1][2] = 2*(temp1 - temp2);
	out->m[2][1] = 2*(temp1 + temp2);
}

// Convert a quaternion to a 3x3 matrix!
mat3 quatToMat3R(const quat q){
	const float xx = q.x*q.x;
	const float yy = q.y*q.y;
	const float zz = q.z*q.z;
	float temp1;
	float temp2;
	mat3 out;

	out.m[0][0] = 1 - 2*(yy + zz);
	out.m[1][1] = 1 - 2*(xx - zz);
	out.m[2][2] = 1 - 2*(xx - yy);

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

	return(out);
}