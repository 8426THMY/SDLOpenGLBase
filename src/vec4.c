#include "vec4.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"


//Initialize the vec4's values to 0!
void vec4InitZero(vec4 *v){
	memset(v, 0.f, sizeof(*v));
}

//Initialize the vec4's values using a vec3!
void vec4InitVec3(vec4 *v1, const vec3 *v2, const float w){
	v1->x = v1->x;
	v1->y = v1->y;
	v1->z = v1->z;
	v1->w = w;
}

//Initialize the vec4's values to the ones specified!
void vec4InitSet(vec4 *v, const float x, const float y, const float z, const float w){
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}


//Add a vec4 stored as four floats to "v" and store the result in "out"!
void vec4Add(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out){
	out->x = v->x + x;
	out->y = v->y + y;
	out->z = v->z + z;
	out->w = v->w + w;
}

//Add "x" to "v" and store the result in "out"!
void vec4AddS(const vec4 *v, const float x, vec4 *out){
	out->x = v->x + x;
	out->y = v->y + x;
	out->z = v->z + x;
	out->w = v->w + x;
}

//Add "v2" to "v1" and store the result in "out"!
void vec4AddVec4(const vec4 *v1, const vec4 *v2, vec4 *out){
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
	out->w = v1->w + v2->w;
}

//Subtract "v" from a vec4 stored as four floats and store the result in "out"!
void vec4SubtractFrom(const vec4 *v, const float x, const float y, const float z, const float w, vec4 *out){
	out->x = x - v->x;
	out->y = y - v->y;
	out->z = z - v->z;
	out->w = w - v->w;
}

//Subtract "x" from "v" and store the result in "out"!
void vec4SubtractSFrom(const vec4 *v, const float x, vec4 *out){
	out->x = v->x - x;
	out->y = v->y - x;
	out->z = v->z - x;
	out->w = v->w - x;
}

//Subtract "v" from "x" and store the result in "out"!
void vec4SubtractFromS(const vec4 *v, const float x, vec4 *out){
	out->x = x - v->x;
	out->y = x - v->y;
	out->z = x - v->z;
	out->w = x - v->w;
}

//Subtract "v2" from "v1" and store the result in "out"!
void vec4SubtractVec4From(const vec4 *v1, const vec4 *v2, vec4 *out){
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
	out->w = v1->w - v2->w;
}

//Multiply "v" by "x" and store the result in "out"!
void vec4MultiplyS(const vec4 *v, const float x, vec4 *out){
	out->x = v->x * x;
	out->y = v->y * x;
	out->z = v->z * x;
	out->w = v->w * x;
}

//Multiply "v1" by "v2" and store the result in "out"!
void vec4MultiplyVec4(const vec4 *v1, const vec4 *v2, vec4 *out){
	out->x = v1->x * v2->x;
	out->y = v1->y * v2->y;
	out->z = v1->z * v2->z;
	out->w = v1->w * v2->w;
}

//Divide "v" by "x" and store the result in "out"!
void vec4DivideByS(const vec4 *v, const float x, vec4 *out){
	//Make sure we don't divide by 0!
	if(x != 0.f){
		const float invX = 1.f / x;

		out->x = v->x * invX;
		out->y = v->y * invX;
		out->z = v->z * invX;
		out->w = v->w * invX;
	}else{
		vec4InitZero(out);
	}
}

//Divide "x" by "v" and store the result in "out"!
void vec4DivideSBy(const vec4 *v, const float x, vec4 *out){
	//Make sure we don't divide by 0!
	if(v->x != 0.f && v->y != 0.f && v->z != 0.f && v->w != 0.f){
		out->x = x / v->x;
		out->y = x / v->y;
		out->z = x / v->z;
		out->w = x / v->w;
	}else{
		vec4InitZero(out);
	}
}

//Divide "v1" by "v2" and store the result in "out"!
void vec4DivideByVec4(const vec4 *v1, const vec4 *v2, vec4 *out){
	//Make sure we don't divide by 0!
	if(v2->x != 0.f && v2->y != 0.f && v2->z != 0.f && v2->w != 0.f){
		out->x = v1->x / v2->x;
		out->y = v1->y / v2->y;
		out->z = v1->z / v2->z;
		out->w = v1->w / v2->w;
	}else{
		vec4InitZero(out);
	}
}


//Find the norm of a vec4 stored as four floats!
float vec4Norm(const float x, const float y, const float z, const float w){
	return(x * x + y * y + z * z + w * w);
}

//Find the norm of a vec4!
float vec4NormVec4(const vec4 *v){
	return(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}


//Find the magnitude (length) of a vec4 stored as four floats!
float vec4Magnitude(const float x, const float y, const float z, const float w){
	return(sqrtf(vec4Norm(x, y, z, w)));
}

//Find the magnitude (length) of a vec4!
float vec4MagnitudeVec4(const vec4 *v){
	return(sqrtf(vec4NormVec4(v)));
}

//Find the distance between a vec4 and one stored as four floats!
float vec4DistanceSquared(const vec4 *v, const float x, const float y, const float z, const float w){
	return(vec4Norm(x - v->x, y - v->y, z - v->z, w - v->w));
}

//Find the distance between two vec4s!
float vec4DistanceSquaredVec4(const vec4 *v1, const vec4 *v2){
	return(vec4Norm(v2->x - v1->x, v2->y - v1->y, v2->z - v1->z, v2->w - v1->w));
}


//Find the dot product of two vec4s stored as four floats!
float vec4Dot(const float x1, const float y1, const float z1, const float w1, const float x2, const float y2, const float z2, const float w2){
	return(x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2);
}

//Find the dot product of a vec4 and one stored as four floats!
float vec4DotVec4Float(const vec4 *v, const float x, const float y, const float z, const float w){
	return(v->x * x + v->y * y + v->z * z + v->w * w);
}

//Find the dot product of two vec4s!
float vec4DotVec4(const vec4 *v1, const vec4 *v2){
	return(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w);
}


//Normalize a vec4 stored as four floats and store the result in "out"!
void vec4Normalize(const float x, const float y, const float z, const float w, vec4 *out){
	const float magnitude = fastInvSqrt(vec4Norm(x, y, z, w));

	out->x = x * magnitude;
	out->y = y * magnitude;
	out->z = z * magnitude;
	out->w = w * magnitude;
}

//Normalize a vec4 and store the result in "out"!
void vec4NormalizeVec4(const vec4 *v, vec4 *out){
	const float magnitude = fastInvSqrt(vec4NormVec4(v));

	out->x = v->x * magnitude;
	out->y = v->y * magnitude;
	out->z = v->z * magnitude;
	out->w = v->w * magnitude;
}

//Negate the values of a vec4!
void vec4Negate(const vec4 *v, vec4 *out){
	out->x = -v->x;
	out->y = -v->y;
	out->z = -v->z;
	out->w = -v->w;
}


//Convert a vec4 from degrees to radians!
void vec4DegToRad(vec4 *v){
	v->x *= DEG_TO_RAD;
	v->y *= DEG_TO_RAD;
	v->z *= DEG_TO_RAD;
	v->w *= DEG_TO_RAD;
}

//Convert a vec4 from radians to degrees!
void vec4RadToDeg(vec4 *v){
	v->x *= RAD_TO_DEG;
	v->y *= RAD_TO_DEG;
	v->z *= RAD_TO_DEG;
	v->w *= RAD_TO_DEG;
}


//Perform linear interpolation between two vec4s and store the result in "out"!
void vec4Lerp(const vec4 *v1, const vec4 *v2, const float time, vec4 *out){
	out->x = floatLerp(v1->x, v2->x, time);
	out->y = floatLerp(v1->y, v2->y, time);
	out->z = floatLerp(v1->z, v2->z, time);
	out->w = floatLerp(v1->w, v2->w, time);
}

/*
** Perform linear interpolation between two vec4s and store the result in "out"!
** Instead of entering the two vectors to interpolate between, this function
** accepts the starting point and the difference between it and the ending point.
*/
void vec4LerpFast(const vec4 *v, const vec4 *offset, const float time, vec4 *out){
	out->x = floatLerpFast(v->x, offset->x, time);
	out->y = floatLerpFast(v->y, offset->y, time);
	out->z = floatLerpFast(v->z, offset->z, time);
	out->w = floatLerpFast(v->w, offset->w, time);
}