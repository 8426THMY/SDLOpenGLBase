#include "cubicSpline.h"


#include "memoryManager.h"


/*
** Initialize a spline's coefficients using
** the array 'y' of 'n' control points.
*/
void cubicSplineInit(
	cubicSpline *const spline, const vec3 *const y, const size_t n
){

	//
}

/*
** Calculate the value of a spline at time 't' after
** control point 'i'. We assume that 0 <= t <= 1.
*/
void cubicSplineEvaluate(
	const cubicSpline *const spline,
	const size_t i, const float t, vec3 *const out
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

void cubicSplineDelete(cubicSpline *const spline){
	// Rather than allocating a chunk for each array,
	// we do a single big allocation. This probably
	// isn't a great idea, as we're done for if there
	// isn't a contiguous chunk that's big enough.
	memoryManagerGlobalfree(spline->a);
}