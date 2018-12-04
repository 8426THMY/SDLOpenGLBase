#include "mat3.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


//Initialize the matrix's values to 0!
void mat3InitZero(mat3 *m){
	memset(m, 0.f, sizeof(*m));
}

//Initialize the matrix to an identity matrix!
void mat3InitIdentity(mat3 *m){
	m->m[0][0] = 1.f;
	m->m[0][1] =
	m->m[0][2] =

	m->m[1][0] = 0.f;
	m->m[1][1] = 1.f;
	m->m[1][2] =

	m->m[2][0] =
	m->m[2][1] = 0.f;
	m->m[2][2] = 1.f;
}


//Multiply a vec3 with a matrix and store the result in "out"!
void vec3MultiplyMat3(const vec3 *v, const mat3 *m, mat3 *out){
	float result[9];

	result[0] = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z;
	result[1] = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z;
	result[2] = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z;

	result[3] = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z;
	result[4] = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z;
	result[5] = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z;

	result[6] = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z;
	result[7] = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z;
	result[8] = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z;

	memcpy(out, result, sizeof(result));
}

//Multiply a vec3 with a matrix and store the result in "out"! This assumes that "out" isn't "m".
void vec3MultiplyMat3R(const vec3 *v, const mat3 *m, mat3 *out){
	out->m[0][0] = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z;
	out->m[0][1] = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z;
	out->m[0][2] = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z;

	out->m[1][0] = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z;
	out->m[1][1] = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z;
	out->m[1][2] = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z;

	out->m[2][0] = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z;
	out->m[2][1] = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z;
	out->m[2][2] = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z;
}

//Multiply a matrix with a vec3 and store the result in "out"!
void mat3MultiplyVec3(const mat3 *m, const vec3 *v, vec3 *out){
	vec3 result;

	result.x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
	result.y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
	result.z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;

	*out = result;
}

//Multiply a matrix with a vec3 and store the result in "out"! This assumes that "out" isn't "v".
void mat3MultiplyVec3R(const mat3 *m, const vec3 *v, vec3 *out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z;
}

//Multiply two matrices and store the result in "out"!
void mat3MultiplyMat3(const mat3 *m1, const mat3 *m2, mat3 *out){
	float result[9];

	result[0] = m1->m[0][0] * m2->m[0][0] + m1->m[1][0] * m2->m[0][1] + m1->m[2][0] * m2->m[0][2];
	result[1] = m1->m[0][1] * m2->m[0][0] + m1->m[1][1] * m2->m[0][1] + m1->m[2][1] * m2->m[0][2];
	result[2] = m1->m[0][2] * m2->m[0][0] + m1->m[1][2] * m2->m[0][1] + m1->m[2][2] * m2->m[0][2];

	result[3] = m1->m[0][0] * m2->m[1][0] + m1->m[1][0] * m2->m[1][1] + m1->m[2][0] * m2->m[1][2];
	result[4] = m1->m[0][1] * m2->m[1][0] + m1->m[1][1] * m2->m[1][1] + m1->m[2][1] * m2->m[1][2];
	result[5] = m1->m[0][2] * m2->m[1][0] + m1->m[1][2] * m2->m[1][1] + m1->m[2][2] * m2->m[1][2];

	result[6] = m1->m[0][0] * m2->m[2][0] + m1->m[1][0] * m2->m[2][1] + m1->m[2][0] * m2->m[2][2];
	result[7] = m1->m[0][1] * m2->m[2][0] + m1->m[1][1] * m2->m[2][1] + m1->m[2][1] * m2->m[2][2];
	result[8] = m1->m[0][2] * m2->m[2][0] + m1->m[1][2] * m2->m[2][1] + m1->m[2][2] * m2->m[2][2];

	memcpy(out, result, sizeof(result));
}

//Multiply two matrices and store the result in "out"! This assumes that "out" isn't "m1" or "m2".
void mat3MultiplyMat3R(const mat3 *m1, const mat3 *m2, mat3 *out){
	out->m[0][0] = m1->m[0][0] * m2->m[0][0] + m1->m[1][0] * m2->m[0][1] + m1->m[2][0] * m2->m[0][2];
	out->m[0][1] = m1->m[0][1] * m2->m[0][0] + m1->m[1][1] * m2->m[0][1] + m1->m[2][1] * m2->m[0][2];
	out->m[0][2] = m1->m[0][2] * m2->m[0][0] + m1->m[1][2] * m2->m[0][1] + m1->m[2][2] * m2->m[0][2];

	out->m[1][0] = m1->m[0][0] * m2->m[1][0] + m1->m[1][0] * m2->m[1][1] + m1->m[2][0] * m2->m[1][2];
	out->m[1][1] = m1->m[0][1] * m2->m[1][0] + m1->m[1][1] * m2->m[1][1] + m1->m[2][1] * m2->m[1][2];
	out->m[1][2] = m1->m[0][2] * m2->m[1][0] + m1->m[1][2] * m2->m[1][1] + m1->m[2][2] * m2->m[1][2];

	out->m[2][0] = m1->m[0][0] * m2->m[2][0] + m1->m[1][0] * m2->m[2][1] + m1->m[2][0] * m2->m[2][2];
	out->m[2][1] = m1->m[0][1] * m2->m[2][0] + m1->m[1][1] * m2->m[2][1] + m1->m[2][1] * m2->m[2][2];
	out->m[2][2] = m1->m[0][2] * m2->m[2][0] + m1->m[1][2] * m2->m[2][1] + m1->m[2][2] * m2->m[2][2];
}


//Find the transpose of a matrix! For column-major matrices, this effectively
//translates it to a row-major matrix. The reverse is true for row-major matrices.
void mat3Transpose(const mat3 *m, mat3 *out){
	float tempMatrix[9];
	memcpy(tempMatrix, m, sizeof(tempMatrix));

	out->m[0][0] = tempMatrix[0];
	out->m[0][1] = tempMatrix[3];
	out->m[0][2] = tempMatrix[6];

	out->m[1][0] = tempMatrix[1];
	out->m[1][1] = tempMatrix[4];
	out->m[1][2] = tempMatrix[7];

	out->m[2][0] = tempMatrix[2];
	out->m[2][1] = tempMatrix[5];
	out->m[2][2] = tempMatrix[8];
}

//Invert a matrix!
return_t mat3Invert(const mat3 *m, mat3 *out){
	float tempMatrix[9];
	memcpy(tempMatrix, m, sizeof(tempMatrix));

	//We need to use these values twice, but we only need to calculate them once.
	out->m[0][0] = tempMatrix[4] * tempMatrix[8] - tempMatrix[5] * tempMatrix[7];
	out->m[0][1] = tempMatrix[7] * tempMatrix[2] - tempMatrix[1] * tempMatrix[8];
	out->m[0][2] = tempMatrix[1] * tempMatrix[5] - tempMatrix[4] * tempMatrix[2];
	//Find the inverse determinant of the matrix!
	float invDet = tempMatrix[0] * out->m[0][0] +
	               tempMatrix[3] * out->m[0][1] +
	               tempMatrix[6] * out->m[0][2];

	//Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		//Now use the determinant to find the inverse of the matrix!
		out->m[0][0] *= invDet;
		out->m[0][1] *= invDet;
		out->m[0][2] *= invDet;
		out->m[1][0] = (tempMatrix[6] * tempMatrix[5] - tempMatrix[3] * tempMatrix[8]) * invDet;
		out->m[1][1] = (tempMatrix[0] * tempMatrix[8] - tempMatrix[6] * tempMatrix[2]) * invDet;
		out->m[1][2] = (tempMatrix[2] * tempMatrix[3] - tempMatrix[0] * tempMatrix[5]) * invDet;
		out->m[2][0] = (tempMatrix[3] * tempMatrix[7] - tempMatrix[6] * tempMatrix[4]) * invDet;
		out->m[2][1] = (tempMatrix[1] * tempMatrix[6] - tempMatrix[0] * tempMatrix[7]) * invDet;
		out->m[2][2] = (tempMatrix[0] * tempMatrix[4] - tempMatrix[1] * tempMatrix[3]) * invDet;


		return(1);
	}

	memcpy(out, tempMatrix, sizeof(tempMatrix));


	return(0);
}