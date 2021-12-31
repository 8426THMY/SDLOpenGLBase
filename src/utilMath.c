#include "utilMath.h"


#define MATH_FLOAT_SIGN      0x80000000
#define MATH_FLOAT_MAGNITUDE 0x7FFFFFFF

// We use this value when comparing non-negative numbers with 0.
#define MATH_ZERO_EPSILON 0.000001f

#define TWO_THIRDS_PI 2.09439510239319549f


#include "mat3.h"


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


#warning "We initialize a lot of vectors to a difference here. It'd be better to do it using the function for when we add SSE support. Check colliderHull.c for more details."


float floatMin(const float x, const float y){
	return(x < y ? x : y);
}

float floatMax(const float x, const float y){
	return(x > y ? x : y);
}

float floatClamp(const float x, const float min, const float max){
	const float t = x < min ? min : x;
	return(t > max ? max : t);
}

unsigned int uintMin(const unsigned int x, const unsigned int y){
	return(x < y ? x : y);
}

unsigned int uintMax(const unsigned int x, const unsigned int y){
	return(x > y ? x : y);
}

unsigned int uintClamp(const unsigned int x, const unsigned int min, const unsigned int max){
	const unsigned int t = x < min ? min : x;
	return(t > max ? max : t);
}


float floatLerp(const float x, const float y, const float t){
	#ifdef FP_FAST_FMAF
		return(fmaf(t, y, fmaf(-t, x, x)));
	#else
		return(x + t*(y - x));
	#endif
}

float floatLerpDiff(const float x, const float y, const float t){
	#ifdef FP_FAST_FMAF
		return(fmaf(t, y, x));
	#else
		return(x + t*y);
	#endif
}


// Return a number that uses the magnitude of x and the sign of y.
float copySign(const float x, const float y){
	const bitFloat i = {.f = x};
	const bitFloat j = {.f = y};
	const bitFloat k = {.l = (i.l & MATH_FLOAT_MAGNITUDE) | (j.l & MATH_FLOAT_SIGN)};
	return(k.f);
}

/*
** Return a number that uses the magnitude of x
** and the sign of y. If y is 0, then we return 0.
*/
float copySignZero(const float x, const float y){
	if(y != 0.f){
		const bitFloat i = {.f = x};
		const bitFloat j = {.f = y};
		const bitFloat k = {.l = (i.l & MATH_FLOAT_MAGNITUDE) | (j.l & MATH_FLOAT_SIGN)};
		return(k.f);
	}
	return(0.f);
}


/*
** Quickly approximates "1.f / sqrtf(x)". This is almost completely
** copy-pasted from the Quake III Arena source, albeit with some
** small changes in order to increase accuracy and avoid triggering
** any compiler warnings.
*/
float fastInvSqrt(const float x){
	const float x2 = x * 0.5f;
	// By using a union here, we can avoid the
	// compiler warnings that the original had.
	bitFloat i = {.f = x};
	// The original magic number, "0x5F3759DF", is supposedly an
	// approximation of the square root of 2 to the power of 127.
	// I have found that the magic number "0x5F3504F3", being a
	// more accurate approximation of this value, provides far more
	// accurate results after any number of Newton-Raphson iterations.
	i.l = 0x5F3504F3 - (i.l >> 1);

	// Use the Newton-Raphson method to
	// gain a more accurate approximation.
	i.f *= 1.5f - x2 * i.f * i.f;


	return(i.f);
}

float fastInvSqrtAccurate(const float x){
	const float x2 = x * 0.5f;
	// By using a union here, we can avoid the
	// compiler warnings that the original had.
	bitFloat i = {.f = x};
	// The original magic number, "0x5F3759DF", is supposedly an
	// approximation of the square root of 2 to the power of 127.
	// I have found that the magic number "0x5F3504F3", being a
	// more accurate approximation of this value, provides far more
	// accurate results after any number of Newton-Raphson iterations.
	i.l = 0x5F3504F3 - (i.l >> 1);

	// Use the Newton-Raphson method to
	// gain a more accurate approximation.
	i.f *= 1.5f - x2 * i.f * i.f;
	// A second iteration provides
	// an even more accurate result.
	i.f *= 1.5f - x2 * i.f * i.f;


	return(i.f);
}


/*
** Compute the barycentric coordinates of the
** point "p" with respect to the triangle "abc".
*/
void pointBarycentric(
	const vec3 *const restrict a, const vec3 *const restrict b,
	const vec3 *const restrict c, const vec3 *const restrict p,
	vec3 *const restrict out
){

	// b - a
	const vec3 v1 = {
		.x = b->x - a->x,
		.y = b->y - a->y,
		.z = b->z - a->z
	};
	// c - a
	const vec3 v2 = {
		.x = c->x - a->x,
		.y = c->y - a->y,
		.z = c->z - a->z
	};
	// p - a
	const vec3 v3 = {
		.x = p->x - a->x,
		.y = p->y - a->y,
		.z = p->z - a->z
	};
	const float d11 = vec3DotVec3(&v1, &v1);
	const float d12 = vec3DotVec3(&v1, &v2);
	const float d22 = vec3DotVec3(&v2, &v2);
	const float d31 = vec3DotVec3(&v3, &v1);
	const float d32 = vec3DotVec3(&v3, &v2);
	const float invDenom = 1.f / (d11 * d22 - d12 * d12);

	out->y = (d22 * d31 - d12 * d32) * invDenom;
	out->z = (d11 * d32 - d12 * d31) * invDenom;
	out->x = 1.f - out->y - out->z;
}


/*
** Compute an orthonormal basis from the normalised vector
** "v1" and store the other two vectors in "v2" and "v3".
**
** Special thanks to Erin Catto for this implementation!
*/
void orthonormalBasis(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3){
	// The magic number "0x3F13CD3A" is approximately equivalent to the
	// square root of 1 over 3. In three dimensions, at least one component
	// of any unit vector must be greater than or equal to this number.
	if(fabsf(v1->x) >= SQRT_ONE_THIRD){
		const float invLength = invSqrt(v1->x*v1->x + v1->y*v1->y);
		vec3InitSet(v2, v1->y*invLength, -v1->x*invLength, 0.f);
	}else{
		const float invLength = invSqrt(v1->y*v1->y + v1->z*v1->z);
		vec3InitSet(v2, 0.f, v1->z*invLength, -v1->y*invLength);
	}
	vec3CrossVec3Out(v1, v2, v3);
}

/*
** Compute an orthonormal basis from the normalised vector
** "v1" and store the other two vectors in "v2" and "v3".
**
** Special thanks to Erin Catto for this implementation!
*/
void orthonormalBasisFast(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3){
	// The magic number "0x3F13CD3A" is approximately equivalent to the
	// square root of 1 over 3. In three dimensions, at least one component
	// of any unit vector must be greater than or equal to this number.
	if(fabsf(v1->x) >= SQRT_ONE_THIRD){
		const float invLength = invSqrtFast(v1->x*v1->x + v1->y*v1->y);
		vec3InitSet(v2, v1->y*invLength, -v1->x*invLength, 0.f);
	}else{
		const float invLength = invSqrtFast(v1->y*v1->y + v1->z*v1->z);
		vec3InitSet(v2, 0.f, v1->z*invLength, -v1->y*invLength);
	}
	vec3CrossVec3Out(v1, v2, v3);
}

/*
** Compute an orthonormal basis from the normalised vector
** "v1" and store the other two vectors in "v2" and "v3".
**
** Special thanks to Pixar for this implementation, which was
** given in Building an Orthonormal Basis, Revisited (2017).
*/
void orthonormalBasisFaster(const vec3 *const restrict v1, vec3 *const restrict v2, vec3 *const restrict v3){
	const float sign = copySign(1.f, v1->z);
	const float a = -1.f/(sign + v1->z);
	const float b = v1->x*v1->y*a;
	vec3InitSet(v2, 1.f + sign*v1->x*v1->x*a, sign*b, -sign*v1->x);
	vec3InitSet(v3, b, sign + v1->y*v1->y*a, -v1->y);
}


/*
** Find the two closest points on two line segments,
** with starting points "sn" and ending points "en",
** and store the new points in the variables "pn".
*/
void segmentClosestPoints(
	const vec3 *const restrict s1, const vec3 *const restrict e1, const vec3 *const restrict s2,
	const vec3 *const restrict e2, vec3 *const restrict p1, vec3 *const restrict p2
){

	// e1 - s1
	const vec3 v1 = {
		.x = e1->x - s1->x,
		.y = e1->y - s1->y,
		.z = e1->z - s1->z
	};
	// s1 - s2
	const vec3 v2 = {
		.x = s1->x - s2->x,
		.y = s1->y - s2->y,
		.z = s1->z - s2->z
	};
	// e2 - s2
	const vec3 v3 = {
		.x = e2->x - s2->x,
		.y = e2->y - s2->y,
		.z = e2->z - s2->z
	};

	const float d11 = vec3MagnitudeSquaredVec3(&v1);
	const float d21 = vec3DotVec3(&v2, &v1);
	const float d23 = vec3DotVec3(&v2, &v3);
	const float d31 = vec3DotVec3(&v3, &v1);
	const float d33 = vec3MagnitudeSquaredVec3(&v3);
	const float denom = d11 * d33 - d31 * d31;
	// If the two edges are perfectly parallel, the closest
	// points should be in the middle of the first segment.
	const float m1 = (denom != 0.f) ? (d23 * d31 - d21 * d33) / denom : 0.5f;
	const float m2 = (d23 + d31 * m1) / d33;

	// Find the closest point on the first line.
	vec3LerpDiff(s1, &v1, m1, p1);
	// Find the closest point on the second line.
	vec3LerpDiff(s2, &v2, m2, p2);
}


/*
** Calculate the normal of the triangle with vertices
** "a", "b" and "c" and store it in the variable "out".
*/
void triangleNormal(const vec3 *const restrict a, const vec3 *const restrict b, const vec3 *const restrict c, vec3 *const restrict out){
	// b - a
	const vec3 v1 = {
		.x = b->x - a->x,
		.y = b->y - a->y,
		.z = b->z - a->z
	};
	// c - a
	const vec3 v2 = {
		.x = c->x - a->x,
		.y = c->y - a->y,
		.z = c->z - a->z
	};

	vec3CrossVec3Out(&v1, &v2, out);
}


/*
** Return the distance between a point and a plane.
** The variable "vertex" can be any point on the
** plane, but the plane's normal must be a unit vector.
*/
float pointPlaneDist(const vec3 *const restrict point, const vec3 *const restrict vertex, const vec3 *const restrict normal){
	// point - vertex
	const vec3 offset = {
		.x = point->x - vertex->x,
		.y = point->y - vertex->y,
		.z = point->z - vertex->z
	};

	return(vec3DotVec3(normal, &offset));
}

/*
** Project a point onto a plane. The variable
** "vertex" can be any point on the plane.
*/
void pointPlaneProject(const vec3 *const restrict point, const vec3 *const restrict vertex, const vec3 *const restrict normal, vec3 *const restrict out){
	// point - vertex
	const vec3 offset = {
		.x = point->x - vertex->x,
		.y = point->y - vertex->y,
		.z = point->z - vertex->z
	};
	const float dist = vec3DotVec3(normal, &offset);

	out->x = point->x - dist * normal->x;
	out->y = point->y - dist * normal->y;
	out->z = point->z - dist * normal->z;
}


/*
** Clamp a quaternion's swing to an ellipse.
**
*/
/**#warning "Should maybe do something like PhysX or MotionToolkit."
void clampEllipse(const float Ex, const float Ey, const float Ea, const float Eb){
	//
}**/

/*
** Clamp a quaternion's swing to an ellipse using a fast, approximate algorithm.
** That is, rather than finding the actual closest point, we find the closest
** point along the line intersecting the point generated by the swing axis and
** the ellipse's origin.
**
** This function returns the distance of this closest point from the ellipse's
** centre. This distance is the maximum allowable angle about the swing axis.
**
** Note that we assume that "Ea" and "Eb" are non-negative.
*/
float clampEllipseDistanceFast(const float Ex, const float Ey, const float Ea, const float Eb){
	if(Ex != 0.f){
		// Consider the ellipse "x^2/a^2 + y^2/b^2 = 1",
		// and let (x0, y0) be a point satisfying
		//
		// x0^2/a^2 + y0^2/b^2 = L^2,
		// L = ab/sqrt(b^2*x0^2 + a^2*y0^2).
		//
		// Then the closest point on the ellipse to (x0, y0)
		// is given by (1/L)(x0, y0). The distance is then
		// given simply by d = ||(x0, y0)||/L.
		const float Ebx = Eb*Ex;
		const float Eay = Ea*Ey;
		return((Ea*Eb) * sqrtf((Ex*Ex + Ey*Ey)/(Ebx*Ebx + Eay*Eay)));
	}else{
		return(Eb);
	}
}

/*
** Clamp a quaternion's swing to an ellipse using a fast, approximate algorithm.
** Besides returning the distance, this also computes the unit normal of the intersection point.
*/
float clampEllipseDistanceNormalFast(const float Ex, const float Ey, const float Ea, const float Eb, vec2 *const restrict normal){
	if(Ex != 0.f){
		// Consider the ellipse "x^2/a^2 + y^2/b^2 = 1",
		// and let (x0, y0) be a point satisfying
		//
		// x0^2/a^2 + y0^2/b^2 = L^2,
		// L = ab/sqrt(b^2*x0^2 + a^2*y0^2).
		//
		// Then the closest point on the ellipse to (x0, y0)
		// is given by (1/L)(x0, y0). The distance is then
		// given simply by d = ||(x0, y0)||/L.
		const float Ebx = Eb*Ex;
		const float Eay = Ea*Ey;

		// Using the formulae above, we can show that the
		// normal at the intersection point is given by
		//
		// (b^2*t_x, a^2*t_y).
		vec2NormalizeFast(Eb*Ebx, Ea*Eay, normal);

		return((Ea*Eb) * sqrtf((Ex*Ex + Ey*Ey)/(Ebx*Ebx + Eay*Eay)));
	}else{
		normal->x = 0.f;
		normal->y = 1.f;

		return(Eb);
	}
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
** given in A Robust Eigensolver for 3x3 Symmetric Matrices (2014).
*/
void diagonalizeMat3Symmetric(
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
	if(maxAbsElement <= MATH_ZERO_EPSILON){
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
			if(norm <= MATH_ZERO_EPSILON){
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
				const float beta0 = 2.f*cosf(angle + TWO_THIRDS_PI);
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
			if(m00abs > MATH_ZERO_EPSILON || m01abs > MATH_ZERO_EPSILON){
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
			if(m11abs > MATH_ZERO_EPSILON || m01abs > MATH_ZERO_EPSILON){
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