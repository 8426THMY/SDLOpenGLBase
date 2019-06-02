#include "mat2.h"


#include <string.h>

#include "utilMath.h"


static mat2 identityMat2 = {
	.m[0][0] = 1.f, .m[0][1] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f
};


//Initialize the matrix's values to 0!
void mat2InitZero(mat2 *m){
	memset(m, 0.f, sizeof(*m));
}

//Initialize the matrix's values to 0!
mat2 mat2InitZeroR(){
	mat2 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

//Initialize the matrix to an identity matrix!
void mat2InitIdentity(mat2 *m){
	*m = identityMat2;
}

//Initialize the matrix to an identity matrix!
mat2 mat2InitIdentityR(){
	return(identityMat2);
}


//Multiply a matrix by a vec2!
void mat2MultiplyByVec2(const mat2 *m, vec2 *v){
	vec2 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y;
}

//Multiply a matrix by a vec2 and store the result in "out"! This assumes that "out" isn't "v".
void mat2MultiplyByVec2Out(const mat2 *m, const vec2 *v, vec2 *out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y;
}

//Multiply a matrix by a vec2!
vec2 mat2MultiplyByVec2R(const mat2 m, const vec2 v){
	vec2 out;

	out.x = m.m[0][0] * v.x + m.m[1][0] * v.y;
	out.y = m.m[0][1] * v.x + m.m[1][1] * v.y;

	return(out);
}

//Multiply a vec2 by a matrix!
void mat2MultiplyVec2By(mat2 *m, const vec2 *v){
	const mat2 tempMatrix = *m;

	m->m[0][0] =
	m->m[0][1] = tempMatrix.m[0][0] * v->x + tempMatrix.m[0][1] * v->y;

	m->m[1][0] =
	m->m[1][1] = tempMatrix.m[1][0] * v->x + tempMatrix.m[1][1] * v->y;
}

//Multiply a vec2 by a matrix and store the result in "out"!
void mat2MultiplyVec2ByOut(const mat2 m, const vec2 *v, mat2 *out){
	out->m[0][0] =
	out->m[0][1] = m.m[0][0] * v->x + m.m[0][1] * v->y;

	out->m[1][0] =
	out->m[1][1] = m.m[1][0] * v->x + m.m[1][1] * v->y;
}

//Multiply a vec2 by a matrix!
mat2 mat2MultiplyVec2ByR(const mat2 m, const vec2 v){
	mat2 out = m;

	out.m[0][0] =
	out.m[0][1] = m.m[0][0] * v.x + m.m[0][1] * v.y;

	out.m[1][0] =
	out.m[1][1] = m.m[1][0] * v.x + m.m[1][1] * v.y;

	return(out);
}

//Multiply "m1" by "m2"!
void mat2MultiplyByMat2(mat2 *m1, const mat2 *m2){
	const mat2 tempMatrix1 = *m1;
	const mat2 tempMatrix2 = *m2;

	m1->m[0][0] = tempMatrix1.m[0][0] * tempMatrix2.m[0][0] + tempMatrix1.m[1][0] * tempMatrix2.m[0][1];
	m1->m[0][1] = tempMatrix1.m[0][1] * tempMatrix2.m[0][0] + tempMatrix1.m[1][1] * tempMatrix2.m[0][1];

	m1->m[1][0] = tempMatrix1.m[0][0] * tempMatrix2.m[1][0] + tempMatrix1.m[1][0] * tempMatrix2.m[1][1];
	m1->m[1][1] = tempMatrix1.m[0][1] * tempMatrix2.m[1][0] + tempMatrix1.m[1][1] * tempMatrix2.m[1][1];
}

//Multiply "m2" by "m1"!
void mat2MultiplyMat2By(mat2 *m1, const mat2 *m2){
	const mat2 tempMatrix1 = *m1;
	const mat2 tempMatrix2 = *m2;

	m1->m[0][0] = tempMatrix2.m[0][0] * tempMatrix1.m[0][0] + tempMatrix2.m[1][0] * tempMatrix1.m[0][1];
	m1->m[0][1] = tempMatrix2.m[0][1] * tempMatrix1.m[0][0] + tempMatrix2.m[1][1] * tempMatrix1.m[0][1];

	m1->m[1][0] = tempMatrix2.m[0][0] * tempMatrix1.m[1][0] + tempMatrix2.m[1][0] * tempMatrix1.m[1][1];
	m1->m[1][1] = tempMatrix2.m[0][1] * tempMatrix1.m[1][0] + tempMatrix2.m[1][1] * tempMatrix1.m[1][1];
}

//Multiply "m1" by "m2" and store the result in "out"!
void mat2MultiplyByMat2Out(const mat2 m1, const mat2 m2, mat2 *out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1];
}

//Multiply "m1" by "m2"!
mat2 mat2MultiplyByMat2R(const mat2 m1, const mat2 m2){
	mat2 out;

	out.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1];
	out.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1];

	out.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1];
	out.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1];

	return(out);
}


/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat2Transpose(mat2 *m){
	const mat2 tempMatrix = *m;

	m->m[0][0] = tempMatrix.m[0][0];
	m->m[0][1] = tempMatrix.m[1][0];

	m->m[1][0] = tempMatrix.m[0][1];
	m->m[1][1] = tempMatrix.m[1][1];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat2TransposeOut(const mat2 m, mat2 *out){
	out->m[0][0] = m.m[0][0];
	out->m[0][1] = m.m[1][0];

	out->m[1][0] = m.m[0][1];
	out->m[1][1] = m.m[1][1];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
mat2 mat2TransposeR(const mat2 m){
	mat2 out;

	out.m[0][0] = m.m[0][0];
	out.m[0][1] = m.m[1][0];

	out.m[1][0] = m.m[0][1];
	out.m[1][1] = m.m[1][1];

	return(out);
}

//Invert a matrix!
void mat2Invert(mat2 *m){
	const mat2 tempMatrix = *m;

	//Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[1][0] * tempMatrix.m[0][1];

	//Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		m->m[0][0] = tempMatrix.m[1][1] * invDet;
		m->m[0][1] = tempMatrix.m[0][1] * -invDet;
		m->m[1][0] = tempMatrix.m[1][0] * -invDet;
		m->m[1][1] = tempMatrix.m[0][0] * invDet;
	}
}

//Invert a matrix and store the result in "out"!
void mat2InvertOut(const mat2 m, mat2 *out){
	//Find the determinant of the matrix!
	float invDet = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

	//Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] = m.m[1][1] * invDet;
		out->m[0][1] = m.m[0][1] * -invDet;
		out->m[1][0] = m.m[1][0] * -invDet;
		out->m[1][1] = m.m[0][0] * invDet;
	}
}

//Invert a matrix!
mat2 mat2InvertR(const mat2 m){
	//Find the determinant of the matrix!
	float invDet = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

	//Make sure we don't divide by 0!
	if(invDet != 0.f){
		mat2 out;

		invDet = 1.f / invDet;

		out.m[0][0] = m.m[1][1] * invDet;
		out.m[0][1] = m.m[0][1] * -invDet;
		out.m[1][0] = m.m[1][0] * -invDet;
		out.m[1][1] = m.m[0][0] * invDet;

		return(out);
	}

	//If we couldn't invert the matrix, return the original one!
	return(m);
}

//Invert a matrix and return whether or not we were successful!
return_t mat2CanInvert(mat2 *m){
	const mat2 tempMatrix = *m;

	//Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[1][0] * tempMatrix.m[0][1];

	//Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		m->m[0][0] = tempMatrix.m[1][1] * invDet;
		m->m[0][1] = tempMatrix.m[0][1] * -invDet;
		m->m[1][0] = tempMatrix.m[1][0] * -invDet;
		m->m[1][1] = tempMatrix.m[0][0] * invDet;


		return(1);
	}


	return(0);
}

/*
** Invert a matrix, storing the result in "out"
** and returning whether or not we were successful!
*/
return_t mat2CanInvertOut(const mat2 m, mat2 *out){
	//Find the determinant of the matrix!
	float invDet = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

	//Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] = m.m[1][1] * invDet;
		out->m[0][1] = m.m[0][1] * -invDet;
		out->m[1][0] = m.m[1][0] * -invDet;
		out->m[1][1] = m.m[0][0] * invDet;


		return(1);
	}


	return(0);
}