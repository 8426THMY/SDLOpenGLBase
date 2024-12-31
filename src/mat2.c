#include "mat2.h"


#include <string.h>

#include "utilMath.h"


mat2 g_mat2Identity = {
	.m[0][0] = 1.f, .m[0][1] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f
};


// Initialize the matrix's values to 0!
void mat2InitZero(mat2 *const restrict m){
	memset(m, 0.f, sizeof(*m));
}

// Initialize the matrix's values to 0!
mat2 mat2InitZeroC(){
	mat2 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

// Initialize the matrix to an identity matrix!
void mat2InitIdentity(mat2 *const restrict m){
	*m = g_mat2Identity;
}

// Initialize the matrix to an identity matrix!
mat2 mat2InitIdentityC(){
	return(g_mat2Identity);
}


// Add the matrix "m2" to "m1"!
void mat2AddMat2(mat2 *const restrict m1, const mat2 *const restrict m2){
	m1->m[0][0] += m2->m[0][0];
	m1->m[0][1] += m2->m[0][1];

	m1->m[1][0] += m2->m[1][0];
	m1->m[1][1] += m2->m[1][1];
}

// Add the matrix "m2" to "m1" and store the result in "out"!
void mat2AddMat2Out(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict out){
	out->m[0][0] = m1->m[0][0] + m2->m[0][0];
	out->m[0][1] = m1->m[0][1] + m2->m[0][1];

	out->m[1][0] = m1->m[1][0] + m2->m[1][0];
	out->m[1][1] = m1->m[1][1] + m2->m[1][1];
}

// Add the matrix "m2" to "m1" and return the result!
mat2 mat2AddMat2C(const mat2 m1, const mat2 m2){
	const mat2 out = {
		.m[0][0] = m1.m[0][0] + m2.m[0][0],
		.m[0][1] = m1.m[0][1] + m2.m[0][1],

		.m[1][0] = m1.m[1][0] + m2.m[1][0],
		.m[1][1] = m1.m[1][1] + m2.m[1][1]
	};

	return(out);
}


// Left-multiply a column vector by a matrix (m*v)!
void mat2MultiplyVec2(const mat2 *const restrict m, vec2 *const restrict v){
	const vec2 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y;
}

/*
** Left-multiply a column vector by a matrix (m*v) and store the result in "out"!
** This assumes that "out" isn't "v".
*/
void mat2MultiplyVec2Out(const mat2 *const restrict m, const vec2 *const restrict v, vec2 *const restrict out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y;
}

// Left-multiply a column vector by a matrix (m*v) and return the result!
vec2 mat2MultiplyVec2C(const mat2 m, const vec2 v){
	const vec2 out = {
		.x = m.m[0][0] * v.x + m.m[1][0] * v.y,
		.y = m.m[0][1] * v.x + m.m[1][1] * v.y
	};

	return(out);
}

// Right-multiply a row vector by a matrix (v^T*m)!
void vec2MultiplyMat2(vec2 *const restrict v, const mat2 *const restrict m){
	const vec2 temp = *v;

	v->x = temp.x * m->m[0][0] + temp.y * m->m[0][1];
	v->y = temp.x * m->m[1][0] + temp.y * m->m[1][1];
}

// Right-multiply a row vector by a matrix (v^T*m) and store the result in "out"!
void vec2MultiplyMat2Out(const vec2 *const restrict v, const mat2 *const restrict m, vec2 *const restrict out){
	out->x = v->x * m->m[0][0] + v->y * m->m[0][1];
	out->y = v->x * m->m[1][0] + v->y * m->m[1][1];
}

// Right-multiply a row vector by a matrix (v^T*m) and return the result!
vec2 vec2MultiplyMat2C(const vec2 v, const mat2 m){
	const vec2 out = {
		.x = v.x * m.m[0][0] + v.y * m.m[0][1],
		.y = v.x * m.m[1][0] + v.y * m.m[1][1]
	};

	return(out);
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "m1"!
void mat2MultiplyMat2P1(mat2 *const restrict m1, const mat2 m2){
	const mat2 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[0][1];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[1][0] * m2.m[1][1];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1];
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "m2"!
void mat2MultiplyMat2P2(const mat2 m1, mat2 *const restrict m2){
	const mat2 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[1][0] * tempMatrix.m[0][1];
	m2->m[0][1] = m1.m[0][1] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[0][1];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[1][0] + m1.m[1][0] * tempMatrix.m[1][1];
	m2->m[1][1] = m1.m[0][1] * tempMatrix.m[1][0] + m1.m[1][1] * tempMatrix.m[1][1];
}

// Left-multiply "m2" by "m1" (m1*m2) and store the result in "out"!
void mat2MultiplyMat2Out(const mat2 m1, const mat2 m2, mat2 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1];
}

// Left-multiply "m2" by "m1" (m1*m2) and return the result!
mat2 mat2MultiplyMat2C(const mat2 m1, const mat2 m2){
	const mat2 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1],
		.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1]
	};

	return(out);
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and store the result in "m1"!
void mat2MultiplyMat2TransP1(mat2 *const restrict m1, const mat2 m2){
	const mat2 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[1][0] * m2.m[1][0];
	m1->m[0][1] = tempMatrix.m[0][1] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[1][0];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[0][1] + tempMatrix.m[1][0] * m2.m[1][1];
	m1->m[1][1] = tempMatrix.m[0][1] * m2.m[0][1] + tempMatrix.m[1][1] * m2.m[1][1];
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and store the result in "m2"!
void mat2MultiplyMat2TransP2(const mat2 m1, mat2 *const restrict m2){
	const mat2 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[1][0] * tempMatrix.m[1][0];
	m2->m[0][1] = m1.m[0][1] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[1][0];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[0][1] + m1.m[1][0] * tempMatrix.m[1][1];
	m2->m[1][1] = m1.m[0][1] * tempMatrix.m[0][1] + m1.m[1][1] * tempMatrix.m[1][1];
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and store the result in "out"!
void mat2MultiplyMat2TransOut(const mat2 m1, const mat2 m2, mat2 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[1][0];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0];

	out->m[1][0] = m1.m[0][0] * m2.m[0][1] + m1.m[1][0] * m2.m[1][1];
	out->m[1][1] = m1.m[0][1] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1];
}

// Left-multiply "m2" transpose by "m1" (m1*m2^T) and return the result!
mat2 mat2MultiplyMat2TransC(const mat2 m1, const mat2 m2){
	const mat2 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[1][0],
		.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0],

		.m[1][0] = m1.m[0][0] * m2.m[0][1] + m1.m[1][0] * m2.m[1][1],
		.m[1][1] = m1.m[0][1] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1]
	};

	return(out);
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and store the result in "m1"!
void mat2TransMultiplyMat2P1(mat2 *const restrict m1, const mat2 m2){
	const mat2 tempMatrix = *m1;

	m1->m[0][0] = tempMatrix.m[0][0] * m2.m[0][0] + tempMatrix.m[0][1] * m2.m[0][1];
	m1->m[0][1] = tempMatrix.m[1][0] * m2.m[0][0] + tempMatrix.m[1][1] * m2.m[0][1];

	m1->m[1][0] = tempMatrix.m[0][0] * m2.m[1][0] + tempMatrix.m[0][1] * m2.m[1][1];
	m1->m[1][1] = tempMatrix.m[1][0] * m2.m[1][0] + tempMatrix.m[1][1] * m2.m[1][1];
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and store the result in "m2"!
void mat2TransMultiplyMat2P2(const mat2 m1, mat2 *const restrict m2){
	const mat2 tempMatrix = *m2;

	m2->m[0][0] = m1.m[0][0] * tempMatrix.m[0][0] + m1.m[0][1] * tempMatrix.m[0][1];
	m2->m[0][1] = m1.m[1][0] * tempMatrix.m[0][0] + m1.m[1][1] * tempMatrix.m[0][1];

	m2->m[1][0] = m1.m[0][0] * tempMatrix.m[1][0] + m1.m[0][1] * tempMatrix.m[1][1];
	m2->m[1][1] = m1.m[1][0] * tempMatrix.m[1][0] + m1.m[1][1] * tempMatrix.m[1][1];
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and store the result in "out"!
void mat2TransMultiplyMat2Out(const mat2 m1, const mat2 m2, mat2 *const restrict out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[0][1];
	out->m[0][1] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[0][1] * m2.m[1][1];
	out->m[1][1] = m1.m[1][0] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1];
}

// Left-multiply "m2" by "m1" transpose (m1^T*m2) and return the result!
mat2 mat2TransMultiplyMat2C(const mat2 m1, const mat2 m2){
	const mat2 out = {
		.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[0][1],
		.m[0][1] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1],

		.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[0][1] * m2.m[1][1],
		.m[1][1] = m1.m[1][0] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1]
	};

	return(out);
}


/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat2Transpose(mat2 *const restrict m){
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
void mat2TransposeOut(const mat2 m, mat2 *const restrict out){
	out->m[0][0] = m.m[0][0];
	out->m[0][1] = m.m[1][0];

	out->m[1][0] = m.m[0][1];
	out->m[1][1] = m.m[1][1];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
mat2 mat2TransposeC(const mat2 m){
	const mat2 out = {
		.m[0][0] = m.m[0][0],
		.m[0][1] = m.m[1][0],

		.m[1][0] = m.m[0][1],
		.m[1][1] = m.m[1][1]
	};

	return(out);
}

// Calculate the determinant of a matrix!
float mat2Determinant(const mat2 *const restrict m){
	return(m->m[0][0]*m->m[1][1] - m->m[1][0]*m->m[0][1]);
}

// Calculate the determinant of a matrix!
float mat2DeterminantC(const mat2 m){
	return(m.m[0][0]*m.m[1][1] - m.m[1][0]*m.m[0][1]);
}

// Calculate the determinant of a matrix from its rows!
float mat2DeterminantColumns(const float *const restrict r0, const float *const restrict r1){
	return(r0[0]*r1[1] - r1[0]*r0[1]);
}

// Invert a matrix!
void mat2Invert(mat2 *const restrict m){
	const mat2 tempMatrix = *m;

	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[1][0] * tempMatrix.m[0][1];

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		m->m[0][0] =  tempMatrix.m[1][1] * invDet;
		m->m[0][1] = -tempMatrix.m[0][1] * invDet;
		m->m[1][0] = -tempMatrix.m[1][0] * invDet;
		m->m[1][1] =  tempMatrix.m[0][0] * invDet;
	}
}

// Invert a matrix and store the result in "out"!
void mat2InvertOut(const mat2 m, mat2 *const restrict out){
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] =  m.m[1][1] * invDet;
		out->m[0][1] = -m.m[0][1] * invDet;
		out->m[1][0] = -m.m[1][0] * invDet;
		out->m[1][1] =  m.m[0][0] * invDet;
	}
}

// Invert a matrix!
mat2 mat2InvertC(const mat2 m){
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		mat2 out;

		invDet = 1.f / invDet;

		out.m[0][0] =  m.m[1][1] * invDet;
		out.m[0][1] = -m.m[0][1] * invDet;
		out.m[1][0] = -m.m[1][0] * invDet;
		out.m[1][1] =  m.m[0][0] * invDet;

		return(out);
	}

	// If we couldn't invert the matrix, return the original one!
	return(m);
}

// Invert a matrix and return whether or not we were successful!
return_t mat2CanInvert(mat2 *const restrict m){
	const mat2 tempMatrix = *m;

	// Find the determinant of the matrix!
	float invDet = tempMatrix.m[0][0] * tempMatrix.m[1][1] - tempMatrix.m[1][0] * tempMatrix.m[0][1];

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		m->m[0][0] =  tempMatrix.m[1][1] * invDet;
		m->m[0][1] = -tempMatrix.m[0][1] * invDet;
		m->m[1][0] = -tempMatrix.m[1][0] * invDet;
		m->m[1][1] =  tempMatrix.m[0][0] * invDet;


		return(1);
	}


	return(0);
}

/*
** Invert a matrix, storing the result in "out"
** and returning whether or not we were successful!
*/
return_t mat2CanInvertOut(const mat2 m, mat2 *const restrict out){
	// Find the determinant of the matrix!
	float invDet = m.m[0][0] * m.m[1][1] - m.m[1][0] * m.m[0][1];

	// Make sure we don't divide by 0!
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] =  m.m[1][1] * invDet;
		out->m[0][1] = -m.m[0][1] * invDet;
		out->m[1][0] = -m.m[1][0] * invDet;
		out->m[1][1] =  m.m[0][0] * invDet;


		return(1);
	}


	return(0);
}


/*
** Solves the system Ax = b using Cramer's rule.
** Cramer's rule states that the solution x is given by
**
** x = (det(A_0)/det(A), det(A_1)/det(A))^T,
**
** where A_i is the matrix A with the ith column replace by b.
** This of course does not work if det(A) = 0.
*/
void mat2Solve(const mat2 *const restrict A, const vec2 *const restrict b, vec2 *const restrict x){
	float invDet = mat2Determinant(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// x_0 = det(A_0)/det(A)
		x->x = mat2DeterminantColumns((const float *const)b, A->m[1]) * invDet;
		// x_1 = det(A_1)/det(A)
		x->y = mat2DeterminantColumns(A->m[0], (const float *const)b) * invDet;
	}else{
		vec2InitZero(x);
	}
}

vec2 mat2SolveC(const mat2 A, const vec2 b){
	float invDet = mat2DeterminantC(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		return(vec2InitSetC(
			// x_0 = det(A_0)/det(A)
			mat2DeterminantColumns((const float *const)&b, A.m[1]) * invDet,
			// x_1 = det(A_1)/det(A)
			mat2DeterminantColumns(A.m[0], (const float *const)&b) * invDet
		));
	}

	return(vec2InitZeroC());
}

/*
** Solves the system Ax = b using Cramer's
** rule and return whether we were successful.
*/
return_t mat2CanSolve(const mat2 *const restrict A, const vec2 *const restrict b, vec2 *const restrict x){
	float invDet = mat2Determinant(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		// x_0 = det(A_0)/det(A)
		x->x = mat2DeterminantColumns((const float *const)b, A->m[1]) * invDet;
		// x_1 = det(A_1)/det(A)
		x->y = mat2DeterminantColumns(A->m[0], (const float *const)b) * invDet;

		return(1);
	}else{
		vec2InitZero(x);
	}

	return(0);
}

return_t mat2CanSolveC(const mat2 A, const vec2 b, vec2 *const restrict x){
	float invDet = mat2DeterminantC(A);
	if(invDet != 0.f){
		invDet = 1.f / invDet;

		*x = vec2InitSetC(
			// x_0 = det(A_0)/det(A)
			mat2DeterminantColumns((const float *const)&b, A.m[1]) * invDet,
			// x_1 = det(A_1)/det(A)
			mat2DeterminantColumns(A.m[0], (const float *const)&b) * invDet
		);

		return(1);
	}else{
		*x = vec2InitZeroC();
	}

	return(0);
}


void mat2Lerp(const mat2 *const restrict m1, const mat2 *const restrict m2, const float time, mat2 *const restrict out){
	out->m[0][0] = floatLerpFast(m1->m[0][0], m2->m[0][0], time);
	out->m[0][1] = floatLerpFast(m1->m[0][1], m2->m[0][1], time);
	out->m[1][0] = floatLerpFast(m1->m[1][0], m2->m[1][0], time);
	out->m[1][1] = floatLerpFast(m1->m[1][1], m2->m[1][1], time);
}

mat2 mat2LerpC(mat2 m1, const mat2 m2, const float time){
	m1.m[0][0] = floatLerpFast(m1.m[0][0], m2.m[0][0], time);
	m1.m[0][1] = floatLerpFast(m1.m[0][1], m2.m[0][1], time);
	m1.m[1][0] = floatLerpFast(m1.m[1][0], m2.m[1][0], time);
	m1.m[1][1] = floatLerpFast(m1.m[1][1], m2.m[1][1], time);

	return(m1);
}