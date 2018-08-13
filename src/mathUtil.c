#include "mathUtil.h"


//Fast inverse square root, literally copy-pasted from the Quake III Arena source.
float fastInvSqrt(const float x){
	const float halfX = x * 0.5f;
	//Don't dereference here, we don't want any compiler warnings.
	const long *xr = (long *)&x;
	const long i = 0x5F3759DF - (*xr >> 1);
	//We avoid dereferencing here for the same reason as before.
	const float *ir = (float *)&i;
	float y = *ir;
	//Newton-Raphson method.
	y *= 1.5f - halfX * y * y;
	//A second iteration provides a more accurate result.
	//y *= 1.5f - halfX * y * y;

	return(y);
}