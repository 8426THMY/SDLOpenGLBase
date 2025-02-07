#include "cubicSpline.h"


#include "memoryManager.h"


/*
** Initialize a spline's coefficients using
** the array 'y' of 'n' control points.
*/
void cubicSplineInit(
	cubicSpline *const restrict spline,
	const vec3 *const restrict y, const size_t n
){

	const size_t numFuncs = n - 1;
	// Given n control points, we have only n-1 spline functions.
	// However, each spline function has 4 coefficients, hence we
	// must allocate memory for 4(n-1) coefficients.
	spline->a = memoryManagerGlobalAlloc(4*numFuncs*sizeof(*y));
	if(spline->a == NULL){
		/** MALLOC FAILED **/
	}
	spline->b = &spline->a[numFuncs];
	spline->c = &spline->b[numFuncs];
	spline->d = &spline->c[numFuncs];

	// Compute each c_i using the Thomas algorithm.
	{
		float cPrimePrev;
		// Store the values of c_i' in memory
		// that we don't quite need yet.
		float *cPrime = (float *)spline->a;

		const vec3 *yNext = &y[1];
		const vec3 *yPrev = &y[0];
		const vec3 *cPrev = spline->c;
		vec3 *c           = cPrev;
		const vec3 *cLast = &cPrev[numFuncs];

		// Compute c_0' and d_0'. We need to do this outside
		// the loop, since they don't follow the same pattern.
		cPrimePrev = 1.f/2.f;
		*cPrime = cPrimePrev;
		++cPrime;
		vec3SubtractVec3Out(yNext, yPrev, c);
		vec3MultiplyS(c, 3.f/2.f);
		++c;
		// Forward sweep: compute c_i' and d_i'.
		for(; c < cLast; ++c){
			// c_i' = 1/(4 - c_{i-1}')
			cPrimePrev = 1.f/(4.f - cPrimePrev);
			*cPrime = cPrimePrev;
			++cPrime;

			++yNext;
			// d_i' = c_i' (3(y_{i+1} - y_{i-1}) - d_{i-1}')
			vec3SubtractVec3Out(yNext, yPrev, c);
			vec3MultiplyS(c, 3.f);
			vec3SubtractVec3P1(c, cPrev);
			vec3MultiplyS(c, cPrimePrev);
			++yPrev;
			cPrev = c;
		}
		// Compute d_{n-1}'. We need to do this outside the loop,
		// since it doesn't follow the same pattern. This value
		// will run into the 'd' array, but that's fine for now.
		if(n >= 2){
			// d_{n-1}' = c_{n-1}' (3(y_{n-1} - y_{n-2}) - d_{n-2}')
			vec3SubtractVec3Out(yNext, yPrev, c);
			vec3MultiplyS(c, 3.f);
			vec3SubtractVec3P1(c, cPrev);
			vec3MultiplyS(c, 1.f/(2.f - cPrimePrev));
		}

		cPrev = c;
		--c;
		cLast = spline->c;
		// Back substitution: compute each c_i.
		for(; c >= cLast; --c){
			--cPrime;
			// c_i = d_i' - c_i' c_{i+1}
			vec3FmaP2(-*cPrime, cPrev, c);
			cPrev = c;
		}
	}

	// Now that we've calculated each c_i,
	// we can compute the other coefficients.
	{
		const vec3 *yCur  = &y[0];
		const vec3 *yNext = &y[1];
		const vec3 *cCur  = &spline->c[0];
		const vec3 *cNext = &spline->c[1];
		vec3 *a = spline->a;
		const vec3 *const aLast = &spline->a[numFuncs];
		vec3 *b = spline->b;

		// Note that we're still storing c_{n-1} in d_1, so
		// we don't need to do anything special in this loop.
		while(a < aLast){
			vec3 ydiff;
			vec3SubtractVec3Out(yCur, yNext, &ydiff);

			// a_i = 2(y_i - y_{i+1}) + c_i + c_{i+1}
			vec3MultiplySOut(&ydiff, 2.f, a);
			vec3AddVec3(a, cCur);
			vec3AddVec3(a, cNext);

			// b_i = 3(y_{i+1} - y_i) - 2c_i - c_{i+1}
			//     = -a_i - (y_i - y_{i+1}) - c_i
			vec3NegateOut(a, b);
			vec3SubtractVec3(b, cCur);
			vec3SubtractVec3(b, cNext);

			yCur = yNext;
			++yNext
			cCur = cNext;
			++cNext;
			++a;
			++b;
		}

		// d_i = y_i
		memcpy(spline->d, y, numFuncs);
	}
}

/*
** Calculate the value of a spline at time 't' after
** control point 'i'. We assume that 0 <= t <= 1.
*/
void cubicSplineEvaluate(
	const cubicSpline *const restrict spline,
	const size_t i, const float t, vec3 *const restrict out
){

	// out = at
	vec3MultiplySOut(&spline->a[i], t, out);
	// out = at^2 + bt
	vec3AddVec3(out, &spline->b[i]);
	vec3MultiplyS(out, t);
	// out = a_i t^3 + b_i t^2 + c_i t
	vec3AddVec3(out, &spline->c[i]);
	vec3MultiplyS(out, t);
	// out = a_i t^3 + b_i t^2 + c_i t + d_i
	vec3AddVec3(out, &spline->d[i]);
}

void cubicSplineDelete(cubicSpline *const restrict spline){
	// Rather than allocating a chunk for each array,
	// we do a single big allocation. This probably
	// isn't a great idea, as we're done for if there
	// isn't a contiguous chunk that's big enough.
	//
	// Currently, we rely on the fact that the chunck
	// is contiguous when we calculate the spline
	// coefficients, so this will have to do for now.
	memoryManagerGlobalfree(spline->a);
}