#include "mat4.h"


#include <string.h>
#include <math.h>

#include "utilMath.h"
#include "mat3.h"


static mat4 identityMat4 = {
	.m[0][0] = 1.f, .m[0][1] = 0.f, .m[0][2] = 0.f, .m[0][3] = 0.f,
	.m[1][0] = 0.f, .m[1][1] = 1.f, .m[1][2] = 0.f, .m[1][3] = 0.f,
	.m[2][0] = 0.f, .m[2][1] = 0.f, .m[2][2] = 1.f, .m[2][3] = 0.f,
	.m[3][0] = 0.f, .m[3][1] = 0.f, .m[3][2] = 0.f, .m[3][3] = 1.f
};


//Initialize the matrix's values to 0!
void mat4InitZero(mat4 *m){
	memset(m, 0.f, sizeof(*m));
}

//Initialize the matrix's values to 0!
mat4 mat4InitZeroR(){
	mat4 m;
	memset(&m, 0.f, sizeof(m));

	return(m);
}

//Initialize the matrix to an identity matrix!
void mat4InitIdentity(mat4 *m){
	*m = identityMat4;
}

//Initialize the matrix to an identity matrix!
mat4 mat4InitIdentityR(){
	return(identityMat4);
}

//Initialise a matrix to a translation matrix!
void mat4InitTranslate(mat4 *m, const float x, const float y, const float z){
	m->m[0][0] = 1.f;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 1.f;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = 1.f;
	m->m[2][3] = 0.f;

	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
	m->m[3][3] = 1.f;
}

//Initialise a matrix to a translation matrix!
mat4 mat4InitTranslateR(const float x, const float y, const float z){
	mat4 m;

	m.m[0][0] = 1.f;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][1] = 1.f;
	m.m[1][2] = 0.f;
	m.m[1][3] = 0.f;

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = 1.f;
	m.m[2][3] = 0.f;

	m.m[3][0] = x;
	m.m[3][1] = y;
	m.m[3][2] = z;
	m.m[3][3] = 1.f;

	return(m);
}

//Initialise a matrix to a translation matrix!
void mat4InitTranslate4(mat4 *m, const float x, const float y, const float z, const float w){
	m->m[0][0] = 1.f;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = 1.f;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = 1.f;
	m->m[2][3] = 0.f;

	m->m[3][0] = x;
	m->m[3][1] = y;
	m->m[3][2] = z;
	m->m[3][3] = w;
}

//Initialise a matrix to a translation matrix!
mat4 mat4InitTranslate4R(const float x, const float y, const float z, const float w){
	mat4 m;

	m.m[0][0] = 1.f;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][1] = 1.f;
	m.m[1][2] = 0.f;
	m.m[1][3] = 0.f;

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = 1.f;
	m.m[2][3] = 0.f;

	m.m[3][0] = x;
	m.m[3][1] = y;
	m.m[3][2] = z;
	m.m[3][3] = w;

	return(m);
}

//Initialise a matrix to a translation matrix using a vec3!
void mat4InitTranslateVec3(mat4 *m, const vec3 *v){
	mat4InitTranslate(m, v->x, v->y, v->z);
}

//Initialise a matrix to a translation matrix using a vec3!
mat4 mat4InitTranslateVec3R(const vec3 v){
	return(mat4InitTranslateR(v.x, v.y, v.z));
}

//Initialise a matrix to a translation matrix using a vec4!
void mat4InitTranslateVec4(mat4 *m, const vec4 *v){
	mat4InitTranslate4(m, v->x, v->y, v->z, v->w);
}

//Initialise a matrix to a translation matrix using a vec4!
mat4 mat4InitTranslateVec4R(const vec4 v){
	return(mat4InitTranslate4R(v.x, v.y, v.z, v.w));
}

//Initialise a matrix to a rotation matrix!
void mat4InitRotateQuat(mat4 *m, const quat *q){
	const float xx = q->x * q->x;
	const float xy = q->x * q->y;
	const float xz = q->x * q->z;
	const float xw = q->x * q->w;
	const float yy = q->y * q->y;
	const float yz = q->y * q->z;
	const float yw = q->y * q->w;
	const float zz = q->z * q->z;
	const float zw = q->z * q->w;

	//Rotate our matrix by the quaternion!
	m->m[0][0] = 1.f - 2.f * (yy + zz);
	m->m[0][1] = 2.f * (xy + zw);
	m->m[0][2] = 2.f * (xz - yw);
	m->m[0][3] = 0.f;

	m->m[1][0] = 2.f * (xy - zw);
	m->m[1][1] = 1.f - 2.f * (xx + zz);
	m->m[1][2] = 2.f * (yz + xw);
	m->m[1][3] = 0.f;

	m->m[2][0] = 2.f * (xz + yw);
	m->m[2][1] = 2.f * (yz - xw);
	m->m[2][2] = 1.f - 2.f * (xx + yy);
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

//Initialise a matrix to a rotation matrix!
mat4 mat4InitRotateQuatR(const quat q){
	const float xx = q.x * q.x;
	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float xw = q.x * q.w;
	const float yy = q.y * q.y;
	const float yz = q.y * q.z;
	const float yw = q.y * q.w;
	const float zz = q.z * q.z;
	const float zw = q.z * q.w;
	mat4 m;

	//Rotate our matrix by the quaternion!
	m.m[0][0] = 1.f - 2.f * (yy + zz);
	m.m[0][1] = 2.f * (xy + zw);
	m.m[0][2] = 2.f * (xz - yw);
	m.m[0][3] = 0.f;

	m.m[1][0] = 2.f * (xy - zw);
	m.m[1][1] = 1.f - 2.f * (xx + zz);
	m.m[1][2] = 2.f * (yz + xw);
	m.m[1][3] = 0.f;

	m.m[2][0] = 2.f * (xz + yw);
	m.m[2][1] = 2.f * (yz - xw);
	m.m[2][2] = 1.f - 2.f * (xx + yy);
	m.m[2][3] = 0.f;

	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = 0.f;
	m.m[3][3] = 1.f;

	return(m);
}

//Initialise a matrix to a scale matrix!
void mat4InitScale(mat4 *m, const float x, const float y, const float z){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = y;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = z;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = 1.f;
}

//Initialise a matrix to a scale matrix!
mat4 mat4InitScaleR(const float x, const float y, const float z){
	mat4 m;

	m.m[0][0] = x;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][1] = y;
	m.m[1][2] = 0.f;
	m.m[1][3] = 0.f;

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = z;
	m.m[2][3] = 0.f;

	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = 0.f;
	m.m[3][3] = 1.f;

	return(m);
}

//Initialise a matrix to a scale matrix!
void mat4InitScale4(mat4 *m, const float x, const float y, const float z, const float w){
	m->m[0][0] = x;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = y;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = z;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = 0.f;
	m->m[3][3] = w;
}

//Initialise a matrix to a scale matrix!
mat4 mat4InitScale4R(const float x, const float y, const float z, const float w){
	mat4 m;

	m.m[0][0] = x;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][1] = y;
	m.m[1][2] = 0.f;
	m.m[1][3] = 0.f;

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = z;
	m.m[2][3] = 0.f;

	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = 0.f;
	m.m[3][3] = w;

	return(m);
}

//Initialise a matrix to a scale matrix using a vec3!
void mat4InitScaleVec3(mat4 *m, const vec3 *v){
	mat4InitScale(m, v->x, v->y, v->z);
}

//Initialise a matrix to a scale matrix using a vec3!
mat4 mat4InitScaleVec3R(const vec3 v){
	return(mat4InitScaleR(v.x, v.y, v.z));
}

//Initialise a matrix to a scale matrix using a vec4!
void mat4InitScaleVec4(mat4 *m, const vec4 *v){
	mat4InitScale4(m, v->x, v->y, v->z, v->w);
}

//Initialise a matrix to a scale matrix using a vec4!
mat4 mat4InitScaleVec4R(const vec4 v){
	return(mat4InitScale4R(v.x, v.y, v.z, v.w));
}


//Multiply a matrix by a vec3!
void mat4MultiplyByVec3(const mat4 *m, vec3 *v){
	vec3 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z + m->m[3][0];
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z + m->m[3][1];
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z + m->m[3][2];
}

//Multiply a matrix by a vec3 and store the result in "out"! This assumes that "out" isn't "v".
void mat4MultiplyByVec3Out(const mat4 *m, const vec3 *v, vec3 *out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0];
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1];
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2];
}

//Multiply a matrix by a vec3!
vec3 mat4MultiplyByVec3R(const mat4 m, const vec3 v){
	vec3 out;

	out.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
	out.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
	out.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];

	return(out);
}

//Multiply a vec4 by a matrix!
void mat4MultiplyVec4By(mat4 *m, const vec4 *v){
	const mat4 tempMatrix = *m;

	m->m[0][0] = v->x * tempMatrix.m[0][0] + v->y * tempMatrix.m[0][1] + v->z * tempMatrix.m[0][2] + v->w * tempMatrix.m[0][3];
	m->m[0][1] = v->x * tempMatrix.m[0][0] + v->y * tempMatrix.m[0][1] + v->z * tempMatrix.m[0][2] + v->w * tempMatrix.m[0][3];
	m->m[0][2] = v->x * tempMatrix.m[0][0] + v->y * tempMatrix.m[0][1] + v->z * tempMatrix.m[0][2] + v->w * tempMatrix.m[0][3];
	m->m[0][3] = v->x * tempMatrix.m[0][0] + v->y * tempMatrix.m[0][1] + v->z * tempMatrix.m[0][2] + v->w * tempMatrix.m[0][3];

	m->m[1][0] = v->x * tempMatrix.m[1][0] + v->y * tempMatrix.m[1][1] + v->z * tempMatrix.m[1][2] + v->w * tempMatrix.m[1][3];
	m->m[1][1] = v->x * tempMatrix.m[1][0] + v->y * tempMatrix.m[1][1] + v->z * tempMatrix.m[1][2] + v->w * tempMatrix.m[1][3];
	m->m[1][2] = v->x * tempMatrix.m[1][0] + v->y * tempMatrix.m[1][1] + v->z * tempMatrix.m[1][2] + v->w * tempMatrix.m[1][3];
	m->m[1][3] = v->x * tempMatrix.m[1][0] + v->y * tempMatrix.m[1][1] + v->z * tempMatrix.m[1][2] + v->w * tempMatrix.m[1][3];

	m->m[2][0] = v->x * tempMatrix.m[2][0] + v->y * tempMatrix.m[2][1] + v->z * tempMatrix.m[2][2] + v->w * tempMatrix.m[2][3];
	m->m[2][1] = v->x * tempMatrix.m[2][0] + v->y * tempMatrix.m[2][1] + v->z * tempMatrix.m[2][2] + v->w * tempMatrix.m[2][3];
	m->m[2][2] = v->x * tempMatrix.m[2][0] + v->y * tempMatrix.m[2][1] + v->z * tempMatrix.m[2][2] + v->w * tempMatrix.m[2][3];
	m->m[2][3] = v->x * tempMatrix.m[2][0] + v->y * tempMatrix.m[2][1] + v->z * tempMatrix.m[2][2] + v->w * tempMatrix.m[2][3];

	m->m[3][0] = v->x * tempMatrix.m[3][0] + v->y * tempMatrix.m[3][1] + v->z * tempMatrix.m[3][2] + v->w * tempMatrix.m[3][3];
	m->m[3][1] = v->x * tempMatrix.m[3][0] + v->y * tempMatrix.m[3][1] + v->z * tempMatrix.m[3][2] + v->w * tempMatrix.m[3][3];
	m->m[3][2] = v->x * tempMatrix.m[3][0] + v->y * tempMatrix.m[3][1] + v->z * tempMatrix.m[3][2] + v->w * tempMatrix.m[3][3];
	m->m[3][3] = v->x * tempMatrix.m[3][0] + v->y * tempMatrix.m[3][1] + v->z * tempMatrix.m[3][2] + v->w * tempMatrix.m[3][3];
}

//Multiply a vec4 by a matrix and store the result in "out"!
void mat4MultiplyVec4ByOut(const mat4 m, const vec4 *v, mat4 *out){
	out->m[0][0] = v->x * m.m[0][0] + v->y * m.m[0][1] + v->z * m.m[0][2] + v->w * m.m[0][3];
	out->m[0][1] = v->x * m.m[0][0] + v->y * m.m[0][1] + v->z * m.m[0][2] + v->w * m.m[0][3];
	out->m[0][2] = v->x * m.m[0][0] + v->y * m.m[0][1] + v->z * m.m[0][2] + v->w * m.m[0][3];
	out->m[0][3] = v->x * m.m[0][0] + v->y * m.m[0][1] + v->z * m.m[0][2] + v->w * m.m[0][3];

	out->m[1][0] = v->x * m.m[1][0] + v->y * m.m[1][1] + v->z * m.m[1][2] + v->w * m.m[1][3];
	out->m[1][1] = v->x * m.m[1][0] + v->y * m.m[1][1] + v->z * m.m[1][2] + v->w * m.m[1][3];
	out->m[1][2] = v->x * m.m[1][0] + v->y * m.m[1][1] + v->z * m.m[1][2] + v->w * m.m[1][3];
	out->m[1][3] = v->x * m.m[1][0] + v->y * m.m[1][1] + v->z * m.m[1][2] + v->w * m.m[1][3];

	out->m[2][0] = v->x * m.m[2][0] + v->y * m.m[2][1] + v->z * m.m[2][2] + v->w * m.m[2][3];
	out->m[2][1] = v->x * m.m[2][0] + v->y * m.m[2][1] + v->z * m.m[2][2] + v->w * m.m[2][3];
	out->m[2][2] = v->x * m.m[2][0] + v->y * m.m[2][1] + v->z * m.m[2][2] + v->w * m.m[2][3];
	out->m[2][3] = v->x * m.m[2][0] + v->y * m.m[2][1] + v->z * m.m[2][2] + v->w * m.m[2][3];

	out->m[3][0] = v->x * m.m[3][0] + v->y * m.m[3][1] + v->z * m.m[3][2] + v->w * m.m[3][3];
	out->m[3][1] = v->x * m.m[3][0] + v->y * m.m[3][1] + v->z * m.m[3][2] + v->w * m.m[3][3];
	out->m[3][2] = v->x * m.m[3][0] + v->y * m.m[3][1] + v->z * m.m[3][2] + v->w * m.m[3][3];
	out->m[3][3] = v->x * m.m[3][0] + v->y * m.m[3][1] + v->z * m.m[3][2] + v->w * m.m[3][3];
}

//Multiply a vec4 by a matrix!
mat4 mat4MultiplyVec4ByR(const mat4 m, const vec4 v){
	mat4 out;

	out.m[0][0] = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3];
	out.m[0][1] = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3];
	out.m[0][2] = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3];
	out.m[0][3] = v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + v.w * m.m[0][3];

	out.m[1][0] = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3];
	out.m[1][1] = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3];
	out.m[1][2] = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3];
	out.m[1][3] = v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + v.w * m.m[1][3];

	out.m[2][0] = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3];
	out.m[2][1] = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3];
	out.m[2][2] = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3];
	out.m[2][3] = v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + v.w * m.m[2][3];

	out.m[3][0] = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3];
	out.m[3][1] = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3];
	out.m[3][2] = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3];
	out.m[3][3] = v.x * m.m[3][0] + v.y * m.m[3][1] + v.z * m.m[3][2] + v.w * m.m[3][3];

	return(out);
}

//Multiply a matrix by a vec4!
void mat4MultiplyByVec4(const mat4 *m, vec4 *v){
	vec4 temp = *v;

	v->x = m->m[0][0] * temp.x + m->m[1][0] * temp.y + m->m[2][0] * temp.z + m->m[3][0] * temp.w;
	v->y = m->m[0][1] * temp.x + m->m[1][1] * temp.y + m->m[2][1] * temp.z + m->m[3][1] * temp.w;
	v->z = m->m[0][2] * temp.x + m->m[1][2] * temp.y + m->m[2][2] * temp.z + m->m[3][2] * temp.w;
	v->w = m->m[0][3] * temp.x + m->m[1][3] * temp.y + m->m[2][3] * temp.z + m->m[3][3] * temp.w;
}

//Multiply a matrix by a vec4 and store the result in "out"! This assumes that "out" isn't "v".
void mat4MultiplyByVec4Out(const mat4 *m, const vec4 *v, vec4 *out){
	out->x = m->m[0][0] * v->x + m->m[1][0] * v->y + m->m[2][0] * v->z + m->m[3][0] * v->w;
	out->y = m->m[0][1] * v->x + m->m[1][1] * v->y + m->m[2][1] * v->z + m->m[3][1] * v->w;
	out->z = m->m[0][2] * v->x + m->m[1][2] * v->y + m->m[2][2] * v->z + m->m[3][2] * v->w;
	out->w = m->m[0][3] * v->x + m->m[1][3] * v->y + m->m[2][3] * v->z + m->m[3][3] * v->w;
}

//Multiply a matrix by a vec4!
vec4 mat4MultiplyByVec4R(const mat4 m, const vec4 v){
	vec4 out;

	out.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w;
	out.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w;
	out.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w;
	out.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w;

	return(out);
}

//Multiply "m1" by "m2"!
void mat4MultiplyByMat4(mat4 *m1, const mat4 m2, mat4 *out){
	const mat4 tempMatrix1 = *m1;

	m1->m[0][0] = tempMatrix1.m[0][0] * m2.m[0][0] + tempMatrix1.m[1][0] * m2.m[0][1] + tempMatrix1.m[2][0] * m2.m[0][2] + tempMatrix1.m[3][0] * m2.m[0][3];
	m1->m[0][1] = tempMatrix1.m[0][1] * m2.m[0][0] + tempMatrix1.m[1][1] * m2.m[0][1] + tempMatrix1.m[2][1] * m2.m[0][2] + tempMatrix1.m[3][1] * m2.m[0][3];
	m1->m[0][2] = tempMatrix1.m[0][2] * m2.m[0][0] + tempMatrix1.m[1][2] * m2.m[0][1] + tempMatrix1.m[2][2] * m2.m[0][2] + tempMatrix1.m[3][2] * m2.m[0][3];
	m1->m[0][3] = tempMatrix1.m[0][3] * m2.m[0][0] + tempMatrix1.m[1][3] * m2.m[0][1] + tempMatrix1.m[2][3] * m2.m[0][2] + tempMatrix1.m[3][3] * m2.m[0][3];

	m1->m[1][0] = tempMatrix1.m[0][0] * m2.m[1][0] + tempMatrix1.m[1][0] * m2.m[1][1] + tempMatrix1.m[2][0] * m2.m[1][2] + tempMatrix1.m[3][0] * m2.m[1][3];
	m1->m[1][1] = tempMatrix1.m[0][1] * m2.m[1][0] + tempMatrix1.m[1][1] * m2.m[1][1] + tempMatrix1.m[2][1] * m2.m[1][2] + tempMatrix1.m[3][1] * m2.m[1][3];
	m1->m[1][2] = tempMatrix1.m[0][2] * m2.m[1][0] + tempMatrix1.m[1][2] * m2.m[1][1] + tempMatrix1.m[2][2] * m2.m[1][2] + tempMatrix1.m[3][2] * m2.m[1][3];
	m1->m[1][3] = tempMatrix1.m[0][3] * m2.m[1][0] + tempMatrix1.m[1][3] * m2.m[1][1] + tempMatrix1.m[2][3] * m2.m[1][2] + tempMatrix1.m[3][3] * m2.m[1][3];

	m1->m[2][0] = tempMatrix1.m[0][0] * m2.m[2][0] + tempMatrix1.m[1][0] * m2.m[2][1] + tempMatrix1.m[2][0] * m2.m[2][2] + tempMatrix1.m[3][0] * m2.m[2][3];
	m1->m[2][1] = tempMatrix1.m[0][1] * m2.m[2][0] + tempMatrix1.m[1][1] * m2.m[2][1] + tempMatrix1.m[2][1] * m2.m[2][2] + tempMatrix1.m[3][1] * m2.m[2][3];
	m1->m[2][2] = tempMatrix1.m[0][2] * m2.m[2][0] + tempMatrix1.m[1][2] * m2.m[2][1] + tempMatrix1.m[2][2] * m2.m[2][2] + tempMatrix1.m[3][2] * m2.m[2][3];
	m1->m[2][3] = tempMatrix1.m[0][3] * m2.m[2][0] + tempMatrix1.m[1][3] * m2.m[2][1] + tempMatrix1.m[2][3] * m2.m[2][2] + tempMatrix1.m[3][3] * m2.m[2][3];

	m1->m[3][0] = tempMatrix1.m[0][0] * m2.m[3][0] + tempMatrix1.m[1][0] * m2.m[3][1] + tempMatrix1.m[2][0] * m2.m[3][2] + tempMatrix1.m[3][0] * m2.m[3][3];
	m1->m[3][1] = tempMatrix1.m[0][1] * m2.m[3][0] + tempMatrix1.m[1][1] * m2.m[3][1] + tempMatrix1.m[2][1] * m2.m[3][2] + tempMatrix1.m[3][1] * m2.m[3][3];
	m1->m[3][2] = tempMatrix1.m[0][2] * m2.m[3][0] + tempMatrix1.m[1][2] * m2.m[3][1] + tempMatrix1.m[2][2] * m2.m[3][2] + tempMatrix1.m[3][2] * m2.m[3][3];
	m1->m[3][3] = tempMatrix1.m[0][3] * m2.m[3][0] + tempMatrix1.m[1][3] * m2.m[3][1] + tempMatrix1.m[2][3] * m2.m[3][2] + tempMatrix1.m[3][3] * m2.m[3][3];
}

//Multiply "m2" by "m1"!
void mat4MultiplyMat4By(mat4 *m1, const mat4 m2, mat4 *out){
	const mat4 tempMatrix1 = *m1;

	m1->m[0][0] = m2.m[0][0] * tempMatrix1.m[0][0] + m2.m[1][0] * tempMatrix1.m[0][1] + m2.m[2][0] * tempMatrix1.m[0][2] + m2.m[3][0] * tempMatrix1.m[0][3];
	m1->m[0][1] = m2.m[0][1] * tempMatrix1.m[0][0] + m2.m[1][1] * tempMatrix1.m[0][1] + m2.m[2][1] * tempMatrix1.m[0][2] + m2.m[3][1] * tempMatrix1.m[0][3];
	m1->m[0][2] = m2.m[0][2] * tempMatrix1.m[0][0] + m2.m[1][2] * tempMatrix1.m[0][1] + m2.m[2][2] * tempMatrix1.m[0][2] + m2.m[3][2] * tempMatrix1.m[0][3];
	m1->m[0][3] = m2.m[0][3] * tempMatrix1.m[0][0] + m2.m[1][3] * tempMatrix1.m[0][1] + m2.m[2][3] * tempMatrix1.m[0][2] + m2.m[3][3] * tempMatrix1.m[0][3];

	m1->m[1][0] = m2.m[0][0] * tempMatrix1.m[1][0] + m2.m[1][0] * tempMatrix1.m[1][1] + m2.m[2][0] * tempMatrix1.m[1][2] + m2.m[3][0] * tempMatrix1.m[1][3];
	m1->m[1][1] = m2.m[0][1] * tempMatrix1.m[1][0] + m2.m[1][1] * tempMatrix1.m[1][1] + m2.m[2][1] * tempMatrix1.m[1][2] + m2.m[3][1] * tempMatrix1.m[1][3];
	m1->m[1][2] = m2.m[0][2] * tempMatrix1.m[1][0] + m2.m[1][2] * tempMatrix1.m[1][1] + m2.m[2][2] * tempMatrix1.m[1][2] + m2.m[3][2] * tempMatrix1.m[1][3];
	m1->m[1][3] = m2.m[0][3] * tempMatrix1.m[1][0] + m2.m[1][3] * tempMatrix1.m[1][1] + m2.m[2][3] * tempMatrix1.m[1][2] + m2.m[3][3] * tempMatrix1.m[1][3];

	m1->m[2][0] = m2.m[0][0] * tempMatrix1.m[2][0] + m2.m[1][0] * tempMatrix1.m[2][1] + m2.m[2][0] * tempMatrix1.m[2][2] + m2.m[3][0] * tempMatrix1.m[2][3];
	m1->m[2][1] = m2.m[0][1] * tempMatrix1.m[2][0] + m2.m[1][1] * tempMatrix1.m[2][1] + m2.m[2][1] * tempMatrix1.m[2][2] + m2.m[3][1] * tempMatrix1.m[2][3];
	m1->m[2][2] = m2.m[0][2] * tempMatrix1.m[2][0] + m2.m[1][2] * tempMatrix1.m[2][1] + m2.m[2][2] * tempMatrix1.m[2][2] + m2.m[3][2] * tempMatrix1.m[2][3];
	m1->m[2][3] = m2.m[0][3] * tempMatrix1.m[2][0] + m2.m[1][3] * tempMatrix1.m[2][1] + m2.m[2][3] * tempMatrix1.m[2][2] + m2.m[3][3] * tempMatrix1.m[2][3];

	m1->m[3][0] = m2.m[0][0] * tempMatrix1.m[3][0] + m2.m[1][0] * tempMatrix1.m[3][1] + m2.m[2][0] * tempMatrix1.m[3][2] + m2.m[3][0] * tempMatrix1.m[3][3];
	m1->m[3][1] = m2.m[0][1] * tempMatrix1.m[3][0] + m2.m[1][1] * tempMatrix1.m[3][1] + m2.m[2][1] * tempMatrix1.m[3][2] + m2.m[3][1] * tempMatrix1.m[3][3];
	m1->m[3][2] = m2.m[0][2] * tempMatrix1.m[3][0] + m2.m[1][2] * tempMatrix1.m[3][1] + m2.m[2][2] * tempMatrix1.m[3][2] + m2.m[3][2] * tempMatrix1.m[3][3];
	m1->m[3][3] = m2.m[0][3] * tempMatrix1.m[3][0] + m2.m[1][3] * tempMatrix1.m[3][1] + m2.m[2][3] * tempMatrix1.m[3][2] + m2.m[3][3] * tempMatrix1.m[3][3];
}

//Multiply "m1" by "m2" and store the result in "out"! This assumes that "out" isn't "m1" or "m2".
void mat4MultiplyByMat4Out(const mat4 m1, const mat4 m2, mat4 *out){
	out->m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
	out->m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
	out->m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];
	out->m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

	out->m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
	out->m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
	out->m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];
	out->m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

	out->m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
	out->m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
	out->m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];
	out->m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

	out->m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
	out->m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
	out->m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
	out->m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] + m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];
}

//Multiply "m1" by "m2" and store the result in "out"!
mat4 mat4MultiplyByMat4R(const mat4 m1, const mat4 m2){
	mat4 out;

	out.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
	out.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
	out.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];
	out.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

	out.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
	out.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
	out.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];
	out.m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

	out.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
	out.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
	out.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];
	out.m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

	out.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
	out.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
	out.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
	out.m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] + m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];

	return(out);
}


//Translate a matrix!
void mat4Translate(mat4 *m, const float x, const float y, const float z){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0];
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1];
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2];
	m->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3];
}

//Translate a matrix!
mat4 mat4TranslateR(mat4 m, const float x, const float y, const float z){
	m.m[3][0] = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0];
	m.m[3][1] = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1];
	m.m[3][2] = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2];
	m.m[3][3] = m.m[0][3] * x + m.m[1][3] * y + m.m[2][3] * z + m.m[3][3];

	return(m);
}

//Translate a matrix!
void mat4Translate4(mat4 *m, const float x, const float y, const float z, const float w){
	m->m[3][0] = m->m[0][0] * x + m->m[1][0] * y + m->m[2][0] * z + m->m[3][0] * w;
	m->m[3][1] = m->m[0][1] * x + m->m[1][1] * y + m->m[2][1] * z + m->m[3][1] * w;
	m->m[3][2] = m->m[0][2] * x + m->m[1][2] * y + m->m[2][2] * z + m->m[3][2] * w;
	m->m[3][3] = m->m[0][3] * x + m->m[1][3] * y + m->m[2][3] * z + m->m[3][3] * w;
}

//Translate a matrix!
mat4 mat4Translate4R(mat4 m, const float x, const float y, const float z, const float w){
	m.m[3][0] = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0] * w;
	m.m[3][1] = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1] * w;
	m.m[3][2] = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2] * w;
	m.m[3][3] = m.m[0][3] * x + m.m[1][3] * y + m.m[2][3] * z + m.m[3][3] * w;

	return(m);
}

//Translate a matrix by a vec3!
void mat4TranslateVec3(mat4 *m, const vec3 *v){
	mat4Translate(m, v->x, v->y, v->z);
}

//Translate a matrix by a vec3!
mat4 mat4TranslateVec3R(const mat4 m, const vec3 v){
	return(mat4TranslateR(m, v.x, v.y, v.z));
}

//Translate a matrix by a vec4!
void mat4TranslateVec4(mat4 *m, const vec4 *v){
	mat4Translate4(m, v->x, v->y, v->z, v->w);
}

//Translate a matrix by a vec4!
mat4 mat4TranslateVec4R(const mat4 m, const vec4 v){
	return(mat4Translate4R(m, v.x, v.y, v.z, v.w));
}


//Rotate a matrix!
//The order of rotations is ZYX.
void mat4RotateRad(mat4 *m, const float x, const float y, const float z){
	const float a = cosf(x);
	const float b = sinf(x);
	const float c = cosf(y);
	const float d = sinf(y);
	const float e = cosf(z);
	const float f = sinf(z);
	const float db = d * b;
	const float da = d * a;
	float temp[3];
	const mat4 tempMatrix = *m;

	temp[0] = e * c; temp[1] = -(f * a) + (e * db); temp[2] = (f * b) + (e * da);
	m->m[0][0] = temp[0] * tempMatrix.m[0][0] + temp[1] * tempMatrix.m[1][0] + temp[2] * tempMatrix.m[2][0];
	m->m[1][0] = temp[0] * tempMatrix.m[0][1] + temp[1] * tempMatrix.m[1][1] + temp[2] * tempMatrix.m[2][1];
	m->m[2][0] = temp[0] * tempMatrix.m[0][2] + temp[1] * tempMatrix.m[1][2] + temp[2] * tempMatrix.m[2][2];
	m->m[3][0] = temp[0] * tempMatrix.m[0][3] + temp[1] * tempMatrix.m[1][3] + temp[2] * tempMatrix.m[2][3];

	temp[0] = f * c; temp[1] = (e * a) + (f * db); temp[2] = -(e * b) + (f * da);
	m->m[0][1] = temp[0] * tempMatrix.m[0][0] + temp[1] * tempMatrix.m[1][0] + temp[2] * tempMatrix.m[2][0];
	m->m[1][1] = temp[0] * tempMatrix.m[0][1] + temp[1] * tempMatrix.m[1][1] + temp[2] * tempMatrix.m[2][1];
	m->m[2][1] = temp[0] * tempMatrix.m[0][2] + temp[1] * tempMatrix.m[1][2] + temp[2] * tempMatrix.m[2][2];
	m->m[3][1] = temp[0] * tempMatrix.m[0][3] + temp[1] * tempMatrix.m[1][3] + temp[2] * tempMatrix.m[2][3];

	temp[0] = -d; temp[1] = c * b; temp[2] = c * a;
	m->m[0][2] = temp[0] * tempMatrix.m[0][0] + temp[1] * tempMatrix.m[1][0] + temp[2] * tempMatrix.m[2][0];
	m->m[1][2] = temp[0] * tempMatrix.m[0][1] + temp[1] * tempMatrix.m[1][1] + temp[2] * tempMatrix.m[2][1];
	m->m[2][2] = temp[0] * tempMatrix.m[0][2] + temp[1] * tempMatrix.m[1][2] + temp[2] * tempMatrix.m[2][2];
	m->m[3][2] = temp[0] * tempMatrix.m[0][3] + temp[1] * tempMatrix.m[1][3] + temp[2] * tempMatrix.m[2][3];
}

//Rotate a matrix!
//The order of rotations is ZYX.
mat4 mat4RotateRadR(const mat4 m, const float x, const float y, const float z){
	const float a = cosf(x);
	const float b = sinf(x);
	const float c = cosf(y);
	const float d = sinf(y);
	const float e = cosf(z);
	const float f = sinf(z);
	const float db = d * b;
	const float da = d * a;
	float temp[3];
	mat4 out;

	temp[0] = e * c; temp[1] = -(f * a) + (e * db); temp[2] = (f * b) + (e * da);
	out.m[0][0] = temp[0] * m.m[0][0] + temp[1] * m.m[1][0] + temp[2] * m.m[2][0];
	out.m[1][0] = temp[0] * m.m[0][1] + temp[1] * m.m[1][1] + temp[2] * m.m[2][1];
	out.m[2][0] = temp[0] * m.m[0][2] + temp[1] * m.m[1][2] + temp[2] * m.m[2][2];
	out.m[3][0] = temp[0] * m.m[0][3] + temp[1] * m.m[1][3] + temp[2] * m.m[2][3];

	temp[0] = f * c; temp[1] = (e * a) + (f * db); temp[2] = -(e * b) + (f * da);
	out.m[0][1] = temp[0] * m.m[0][0] + temp[1] * m.m[1][0] + temp[2] * m.m[2][0];
	out.m[1][1] = temp[0] * m.m[0][1] + temp[1] * m.m[1][1] + temp[2] * m.m[2][1];
	out.m[2][1] = temp[0] * m.m[0][2] + temp[1] * m.m[1][2] + temp[2] * m.m[2][2];
	out.m[3][1] = temp[0] * m.m[0][3] + temp[1] * m.m[1][3] + temp[2] * m.m[2][3];

	temp[0] = -d; temp[1] = c * b; temp[2] = c * a;
	out.m[0][2] = temp[0] * m.m[0][0] + temp[1] * m.m[1][0] + temp[2] * m.m[2][0];
	out.m[1][2] = temp[0] * m.m[0][1] + temp[1] * m.m[1][1] + temp[2] * m.m[2][1];
	out.m[2][2] = temp[0] * m.m[0][2] + temp[1] * m.m[1][2] + temp[2] * m.m[2][2];
	out.m[3][2] = temp[0] * m.m[0][3] + temp[1] * m.m[1][3] + temp[2] * m.m[2][3];

	return(out);
}

/*
** Convert the rotation from degrees to radians!
** The order of rotations is ZYX.
*/
void mat4RotateDeg(mat4 *m, const float x, const float y, const float z){
	mat4RotateRad(m, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

/*
** Convert the rotation from degrees to radians!
** The order of rotations is ZYX.
*/
mat4 mat4RotateDegR(const mat4 m, const float x, const float y, const float z){
	return(mat4RotateRadR(m, x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD));
}

/*
** Rotate a matrix by a vec3 (using radians)!
** The order of rotations is ZYX.
*/
void mat4RotateByVec3Rad(mat4 *m, const vec3 *v){
	mat4RotateRad(m, v->x, v->y, v->z);
}

/*
** Rotate a matrix by a vec3 (using radians)!
** The order of rotations is ZYX.
*/
mat4 mat4RotateByVec3RadR(const mat4 m, const vec3 v){
	return(mat4RotateRadR(m, v.x, v.y, v.z));
}

/*
** Rotate a matrix by a vec3 (using degrees)!
** The order of rotations is ZYX.
*/
void mat4RotateByVec3Deg(mat4 *m, const vec3 *v){
	mat4RotateRad(m, v->x * DEG_TO_RAD, v->y * DEG_TO_RAD, v->z * DEG_TO_RAD);
}

/*
** Rotate a matrix by a vec3 (using degrees)!
** The order of rotations is ZYX.
*/
mat4 mat4RotateByVec3DegR(const mat4 m, const vec3 v){
	return(mat4RotateRadR(m, v.x * DEG_TO_RAD, v.y * DEG_TO_RAD, v.z * DEG_TO_RAD));
}

//Rotate a matrix by an axis and an angle!
void mat4RotateAxisAngle(mat4 *m, const vec4 *v){
	const float c = cosf(v->w);
	const float s = sinf(v->w);
	const float t = 1.f - c;
	mat3 rotMatrix;
	const mat4 tempMatrix = *m;

	//Normalize the axis!
	vec3 normalAxis;
	vec3Normalize(v->x, v->y, v->z, &normalAxis);
	vec3 tempAxis;
	vec3MultiplySOut(&normalAxis, t, &tempAxis);

	//Convert the axis angle to a rotation matrix! Note that this is
	//a row-major matrix as opposed to our usual column-major format.
	rotMatrix.m[0][0] = tempAxis.x * normalAxis.x + c;
	rotMatrix.m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s;
	rotMatrix.m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s;

	rotMatrix.m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s;
	rotMatrix.m[1][1] = tempAxis.y * normalAxis.y + c;
	rotMatrix.m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s;

	rotMatrix.m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s;
	rotMatrix.m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s;
	rotMatrix.m[2][2] = tempAxis.z * normalAxis.z + c;

	//Now rotate our matrix by it!
	m->m[0][0] = rotMatrix.m[0][0] * tempMatrix.m[0][0] + rotMatrix.m[0][1] * tempMatrix.m[1][0] + rotMatrix.m[0][2] * tempMatrix.m[2][0];
	m->m[0][1] = rotMatrix.m[1][0] * tempMatrix.m[0][0] + rotMatrix.m[1][1] * tempMatrix.m[1][0] + rotMatrix.m[1][2] * tempMatrix.m[2][0];
	m->m[0][2] = rotMatrix.m[2][0] * tempMatrix.m[0][0] + rotMatrix.m[2][1] * tempMatrix.m[1][0] + rotMatrix.m[2][2] * tempMatrix.m[2][0];

	m->m[1][0] = rotMatrix.m[0][0] * tempMatrix.m[0][1] + rotMatrix.m[0][1] * tempMatrix.m[1][1] + rotMatrix.m[0][2] * tempMatrix.m[2][1];
	m->m[1][1] = rotMatrix.m[1][0] * tempMatrix.m[0][1] + rotMatrix.m[1][1] * tempMatrix.m[1][1] + rotMatrix.m[1][2] * tempMatrix.m[2][1];
	m->m[1][2] = rotMatrix.m[2][0] * tempMatrix.m[0][1] + rotMatrix.m[2][1] * tempMatrix.m[1][1] + rotMatrix.m[2][2] * tempMatrix.m[2][1];

	m->m[2][0] = rotMatrix.m[0][0] * tempMatrix.m[0][2] + rotMatrix.m[0][1] * tempMatrix.m[1][2] + rotMatrix.m[0][2] * tempMatrix.m[2][2];
	m->m[2][1] = rotMatrix.m[1][0] * tempMatrix.m[0][2] + rotMatrix.m[1][1] * tempMatrix.m[1][2] + rotMatrix.m[1][2] * tempMatrix.m[2][2];
	m->m[2][2] = rotMatrix.m[2][0] * tempMatrix.m[0][2] + rotMatrix.m[2][1] * tempMatrix.m[1][2] + rotMatrix.m[2][2] * tempMatrix.m[2][2];

	m->m[3][0] = rotMatrix.m[0][0] * tempMatrix.m[0][3] + rotMatrix.m[0][1] * tempMatrix.m[1][3] + rotMatrix.m[0][2] * tempMatrix.m[2][3];
	m->m[3][1] = rotMatrix.m[1][0] * tempMatrix.m[0][3] + rotMatrix.m[1][1] * tempMatrix.m[1][3] + rotMatrix.m[1][2] * tempMatrix.m[2][3];
	m->m[3][2] = rotMatrix.m[2][0] * tempMatrix.m[0][3] + rotMatrix.m[2][1] * tempMatrix.m[1][3] + rotMatrix.m[2][2] * tempMatrix.m[2][3];
}

//Rotate a matrix by an axis and an angle!
mat4 mat4RotateAxisAngleR(const mat4 m, const vec4 v){
	const float c = cosf(v.w);
	const float s = sinf(v.w);
	const float t = 1.f - c;
	mat3 rotMatrix;
	mat4 out;

	//Normalize the axis!
	const vec3 normalAxis = vec3NormalizeR(v.x, v.y, v.z);
	const vec3 tempAxis = vec3MultiplySR(normalAxis, t);

	//Convert the axis angle to a rotation matrix! Note that this is
	//a row-major matrix as opposed to our usual column-major format.
	rotMatrix.m[0][0] = tempAxis.x * normalAxis.x + c;
	rotMatrix.m[0][1] = tempAxis.x * normalAxis.y + normalAxis.z * s;
	rotMatrix.m[0][2] = tempAxis.x * normalAxis.z - normalAxis.y * s;

	rotMatrix.m[1][0] = tempAxis.y * normalAxis.x - normalAxis.z * s;
	rotMatrix.m[1][1] = tempAxis.y * normalAxis.y + c;
	rotMatrix.m[1][2] = tempAxis.y * normalAxis.z + normalAxis.x * s;

	rotMatrix.m[2][0] = tempAxis.z * normalAxis.x + normalAxis.y * s;
	rotMatrix.m[2][1] = tempAxis.z * normalAxis.y - normalAxis.x * s;
	rotMatrix.m[2][2] = tempAxis.z * normalAxis.z + c;

	//Now rotate our matrix by it!
	out.m[0][0] = rotMatrix.m[0][0] * m.m[0][0] + rotMatrix.m[0][1] * m.m[1][0] + rotMatrix.m[0][2] * m.m[2][0];
	out.m[0][1] = rotMatrix.m[1][0] * m.m[0][0] + rotMatrix.m[1][1] * m.m[1][0] + rotMatrix.m[1][2] * m.m[2][0];
	out.m[0][2] = rotMatrix.m[2][0] * m.m[0][0] + rotMatrix.m[2][1] * m.m[1][0] + rotMatrix.m[2][2] * m.m[2][0];

	out.m[1][0] = rotMatrix.m[0][0] * m.m[0][1] + rotMatrix.m[0][1] * m.m[1][1] + rotMatrix.m[0][2] * m.m[2][1];
	out.m[1][1] = rotMatrix.m[1][0] * m.m[0][1] + rotMatrix.m[1][1] * m.m[1][1] + rotMatrix.m[1][2] * m.m[2][1];
	out.m[1][2] = rotMatrix.m[2][0] * m.m[0][1] + rotMatrix.m[2][1] * m.m[1][1] + rotMatrix.m[2][2] * m.m[2][1];

	out.m[2][0] = rotMatrix.m[0][0] * m.m[0][2] + rotMatrix.m[0][1] * m.m[1][2] + rotMatrix.m[0][2] * m.m[2][2];
	out.m[2][1] = rotMatrix.m[1][0] * m.m[0][2] + rotMatrix.m[1][1] * m.m[1][2] + rotMatrix.m[1][2] * m.m[2][2];
	out.m[2][2] = rotMatrix.m[2][0] * m.m[0][2] + rotMatrix.m[2][1] * m.m[1][2] + rotMatrix.m[2][2] * m.m[2][2];

	out.m[3][0] = rotMatrix.m[0][0] * m.m[0][3] + rotMatrix.m[0][1] * m.m[1][3] + rotMatrix.m[0][2] * m.m[2][3];
	out.m[3][1] = rotMatrix.m[1][0] * m.m[0][3] + rotMatrix.m[1][1] * m.m[1][3] + rotMatrix.m[1][2] * m.m[2][3];
	out.m[3][2] = rotMatrix.m[2][0] * m.m[0][3] + rotMatrix.m[2][1] * m.m[1][3] + rotMatrix.m[2][2] * m.m[2][3];

	return(out);
}

//Rotate a matrix by a quaternion!
void mat4RotateQuat(mat4 *m, const quat *q){
	const float xx = q->x * q->x;
	const float xy = q->x * q->y;
	const float xz = q->x * q->z;
	const float xw = q->x * q->w;
	const float yy = q->y * q->y;
	const float yz = q->y * q->z;
	const float yw = q->y * q->w;
	const float zz = q->z * q->z;
	const float zw = q->z * q->w;
	mat3 rotMatrix;
	const mat4 tempMatrix = *m;

	//Convert the quaternion to a rotation matrix!
	rotMatrix.m[0][0] = 1.f - 2.f * (yy + zz);
	rotMatrix.m[0][1] = 2.f * (xy + zw);
	rotMatrix.m[0][2] = 2.f * (xz - yw);

	rotMatrix.m[1][0] = 2.f * (xy - zw);
	rotMatrix.m[1][1] = 1.f - 2.f * (xx + zz);
	rotMatrix.m[1][2] = 2.f * (yz + xw);

	rotMatrix.m[2][0] = 2.f * (xz + yw);
	rotMatrix.m[2][1] = 2.f * (yz - xw);
	rotMatrix.m[2][2] = 1.f - 2.f * (xx + yy);

	//Now rotate our matrix by it!
	m->m[0][0] = rotMatrix.m[0][0] * tempMatrix.m[0][0] + rotMatrix.m[0][1] * tempMatrix.m[1][0] + rotMatrix.m[0][2] * tempMatrix.m[2][0];
	m->m[0][1] = rotMatrix.m[0][0] * tempMatrix.m[0][1] + rotMatrix.m[0][1] * tempMatrix.m[1][1] + rotMatrix.m[0][2] * tempMatrix.m[2][1];
	m->m[0][2] = rotMatrix.m[0][0] * tempMatrix.m[0][2] + rotMatrix.m[0][1] * tempMatrix.m[1][2] + rotMatrix.m[0][2] * tempMatrix.m[2][2];
	m->m[0][3] = rotMatrix.m[0][0] * tempMatrix.m[0][3] + rotMatrix.m[0][1] * tempMatrix.m[1][3] + rotMatrix.m[0][2] * tempMatrix.m[2][3];

	m->m[1][0] = rotMatrix.m[1][0] * tempMatrix.m[0][0] + rotMatrix.m[1][1] * tempMatrix.m[1][0] + rotMatrix.m[1][2] * tempMatrix.m[2][0];
	m->m[1][1] = rotMatrix.m[1][0] * tempMatrix.m[0][1] + rotMatrix.m[1][1] * tempMatrix.m[1][1] + rotMatrix.m[1][2] * tempMatrix.m[2][1];
	m->m[1][2] = rotMatrix.m[1][0] * tempMatrix.m[0][2] + rotMatrix.m[1][1] * tempMatrix.m[1][2] + rotMatrix.m[1][2] * tempMatrix.m[2][2];
	m->m[1][3] = rotMatrix.m[1][0] * tempMatrix.m[0][3] + rotMatrix.m[1][1] * tempMatrix.m[1][3] + rotMatrix.m[1][2] * tempMatrix.m[2][3];

	m->m[2][0] = rotMatrix.m[2][0] * tempMatrix.m[0][0] + rotMatrix.m[2][1] * tempMatrix.m[1][0] + rotMatrix.m[2][2] * tempMatrix.m[2][0];
	m->m[2][1] = rotMatrix.m[2][0] * tempMatrix.m[0][1] + rotMatrix.m[2][1] * tempMatrix.m[1][1] + rotMatrix.m[2][2] * tempMatrix.m[2][1];
	m->m[2][2] = rotMatrix.m[2][0] * tempMatrix.m[0][2] + rotMatrix.m[2][1] * tempMatrix.m[1][2] + rotMatrix.m[2][2] * tempMatrix.m[2][2];
	m->m[2][3] = rotMatrix.m[2][0] * tempMatrix.m[0][3] + rotMatrix.m[2][1] * tempMatrix.m[1][3] + rotMatrix.m[2][2] * tempMatrix.m[2][3];
}

//Rotate a matrix by a quaternion!
mat4 mat4RotateQuatR(const mat4 m, const quat q){
	const float xx = q.x * q.x;
	const float xy = q.x * q.y;
	const float xz = q.x * q.z;
	const float xw = q.x * q.w;
	const float yy = q.y * q.y;
	const float yz = q.y * q.z;
	const float yw = q.y * q.w;
	const float zz = q.z * q.z;
	const float zw = q.z * q.w;
	mat3 rotMatrix;
	mat4 out;

	//Convert the quaternion to a rotation matrix!
	rotMatrix.m[0][0] = 1.f - 2.f * (yy + zz);
	rotMatrix.m[0][1] = 2.f * (xy + zw);
	rotMatrix.m[0][2] = 2.f * (xz - yw);

	rotMatrix.m[1][0] = 2.f * (xy - zw);
	rotMatrix.m[1][1] = 1.f - 2.f * (xx + zz);
	rotMatrix.m[1][2] = 2.f * (yz + xw);

	rotMatrix.m[2][0] = 2.f * (xz + yw);
	rotMatrix.m[2][1] = 2.f * (yz - xw);
	rotMatrix.m[2][2] = 1.f - 2.f * (xx + yy);

	//Now rotate our matrix by it!
	out.m[0][0] = rotMatrix.m[0][0] * m.m[0][0] + rotMatrix.m[0][1] * m.m[1][0] + rotMatrix.m[0][2] * m.m[2][0];
	out.m[0][1] = rotMatrix.m[0][0] * m.m[0][1] + rotMatrix.m[0][1] * m.m[1][1] + rotMatrix.m[0][2] * m.m[2][1];
	out.m[0][2] = rotMatrix.m[0][0] * m.m[0][2] + rotMatrix.m[0][1] * m.m[1][2] + rotMatrix.m[0][2] * m.m[2][2];
	out.m[0][3] = rotMatrix.m[0][0] * m.m[0][3] + rotMatrix.m[0][1] * m.m[1][3] + rotMatrix.m[0][2] * m.m[2][3];

	out.m[1][0] = rotMatrix.m[1][0] * m.m[0][0] + rotMatrix.m[1][1] * m.m[1][0] + rotMatrix.m[1][2] * m.m[2][0];
	out.m[1][1] = rotMatrix.m[1][0] * m.m[0][1] + rotMatrix.m[1][1] * m.m[1][1] + rotMatrix.m[1][2] * m.m[2][1];
	out.m[1][2] = rotMatrix.m[1][0] * m.m[0][2] + rotMatrix.m[1][1] * m.m[1][2] + rotMatrix.m[1][2] * m.m[2][2];
	out.m[1][3] = rotMatrix.m[1][0] * m.m[0][3] + rotMatrix.m[1][1] * m.m[1][3] + rotMatrix.m[1][2] * m.m[2][3];

	out.m[2][0] = rotMatrix.m[2][0] * m.m[0][0] + rotMatrix.m[2][1] * m.m[1][0] + rotMatrix.m[2][2] * m.m[2][0];
	out.m[2][1] = rotMatrix.m[2][0] * m.m[0][1] + rotMatrix.m[2][1] * m.m[1][1] + rotMatrix.m[2][2] * m.m[2][1];
	out.m[2][2] = rotMatrix.m[2][0] * m.m[0][2] + rotMatrix.m[2][1] * m.m[1][2] + rotMatrix.m[2][2] * m.m[2][2];
	out.m[2][3] = rotMatrix.m[2][0] * m.m[0][3] + rotMatrix.m[2][1] * m.m[1][3] + rotMatrix.m[2][2] * m.m[2][3];

	return(out);
}

//Rotate a matrix on the X axis!
void mat4RotateXRad(mat4 *m, const float x){
	const float a = cosf(x);
	const float b = sinf(x);
	const mat4 tempMatrix = *m;

	m->m[1][0] = a * tempMatrix.m[1][0] + b * tempMatrix.m[2][0];
	m->m[1][1] = a * tempMatrix.m[1][1] + b * tempMatrix.m[2][1];
	m->m[1][2] = a * tempMatrix.m[1][2] + b * tempMatrix.m[2][2];
	m->m[1][3] = a * tempMatrix.m[1][3] + b * tempMatrix.m[2][3];

	m->m[2][0] = a * tempMatrix.m[2][0] - b * tempMatrix.m[1][0];
	m->m[2][1] = a * tempMatrix.m[2][1] - b * tempMatrix.m[1][1];
	m->m[2][2] = a * tempMatrix.m[2][2] - b * tempMatrix.m[1][2];
	m->m[2][3] = a * tempMatrix.m[2][3] - b * tempMatrix.m[1][3];
}

//Rotate a matrix on the X axis!
mat4 mat4RotateXRadR(const mat4 m, const float x){
	const float a = cosf(x);
	const float b = sinf(x);
	mat4 out;

	out.m[1][0] = a * m.m[1][0] + b * m.m[2][0];
	out.m[1][1] = a * m.m[1][1] + b * m.m[2][1];
	out.m[1][2] = a * m.m[1][2] + b * m.m[2][2];
	out.m[1][3] = a * m.m[1][3] + b * m.m[2][3];

	out.m[2][0] = a * m.m[2][0] - b * m.m[1][0];
	out.m[2][1] = a * m.m[2][1] - b * m.m[1][1];
	out.m[2][2] = a * m.m[2][2] - b * m.m[1][2];
	out.m[2][3] = a * m.m[2][3] - b * m.m[1][3];

	return(out);
}

//Convert the X rotation from degrees to radians!
void mat4RotateXDeg(mat4 *m, const float x){
	mat4RotateXRad(m, x * DEG_TO_RAD);
}

//Convert the X rotation from degrees to radians!
mat4 mat4RotateXDegR(const mat4 m, const float x){
	return(mat4RotateXRadR(m, x * DEG_TO_RAD));
}

//Rotate a matrix on the Y axis!
void mat4RotateYRad(mat4 *m, const float y){
	const float c = cosf(y);
	const float d = sinf(y);
	const mat4 tempMatrix = *m;

	m->m[0][0] = c * tempMatrix.m[0][0] - d * tempMatrix.m[2][0];
	m->m[0][1] = c * tempMatrix.m[0][1] - d * tempMatrix.m[2][1];
	m->m[0][2] = c * tempMatrix.m[0][2] - d * tempMatrix.m[2][2];
	m->m[0][3] = c * tempMatrix.m[0][3] - d * tempMatrix.m[2][3];

	m->m[2][0] = d * tempMatrix.m[0][0] + c * tempMatrix.m[2][0];
	m->m[2][1] = d * tempMatrix.m[0][1] + c * tempMatrix.m[2][1];
	m->m[2][2] = d * tempMatrix.m[0][2] + c * tempMatrix.m[2][2];
	m->m[2][3] = d * tempMatrix.m[0][3] + c * tempMatrix.m[2][3];
}

//Rotate a matrix on the Y axis!
mat4 mat4RotateYRadR(const mat4 m, const float y){
	const float c = cosf(y);
	const float d = sinf(y);
	mat4 out;

	out.m[0][0] = c * m.m[0][0] - d * m.m[2][0];
	out.m[0][1] = c * m.m[0][1] - d * m.m[2][1];
	out.m[0][2] = c * m.m[0][2] - d * m.m[2][2];
	out.m[0][3] = c * m.m[0][3] - d * m.m[2][3];

	out.m[2][0] = d * m.m[0][0] + c * m.m[2][0];
	out.m[2][1] = d * m.m[0][1] + c * m.m[2][1];
	out.m[2][2] = d * m.m[0][2] + c * m.m[2][2];
	out.m[2][3] = d * m.m[0][3] + c * m.m[2][3];

	return(out);
}

//Convert the Y rotation from degrees to radians!
void mat4RotateYDeg(mat4 *m, const float y){
	mat4RotateYRad(m, y * DEG_TO_RAD);
}

//Convert the Y rotation from degrees to radians!
mat4 mat4RotateYDegR(const mat4 m, const float y){
	return(mat4RotateYRadR(m, y * DEG_TO_RAD));
}

//Rotate a matrix on the Z axis!
void mat4RotateZRad(mat4 *m, const float z){
	const float e = cosf(z);
	const float f = sinf(z);
	const mat4 tempMatrix = *m;

	m->m[0][0] = e * tempMatrix.m[0][0] + f * tempMatrix.m[1][0];
	m->m[0][1] = e * tempMatrix.m[0][1] + f * tempMatrix.m[1][1];
	m->m[0][2] = e * tempMatrix.m[0][2] + f * tempMatrix.m[1][2];
	m->m[0][3] = e * tempMatrix.m[0][3] + f * tempMatrix.m[1][3];

	m->m[1][0] = e * tempMatrix.m[1][0] - f * tempMatrix.m[0][0];
	m->m[1][1] = e * tempMatrix.m[1][1] - f * tempMatrix.m[0][1];
	m->m[1][2] = e * tempMatrix.m[1][2] - f * tempMatrix.m[0][2];
	m->m[1][3] = e * tempMatrix.m[1][3] - f * tempMatrix.m[0][3];
}

//Rotate a matrix on the Z axis!
mat4 mat4RotateZRadR(const mat4 m, const float z){
	const float e = cosf(z);
	const float f = sinf(z);
	mat4 out;

	out.m[0][0] = e * m.m[0][0] + f * m.m[1][0];
	out.m[0][1] = e * m.m[0][1] + f * m.m[1][1];
	out.m[0][2] = e * m.m[0][2] + f * m.m[1][2];
	out.m[0][3] = e * m.m[0][3] + f * m.m[1][3];

	out.m[1][0] = e * m.m[1][0] - f * m.m[0][0];
	out.m[1][1] = e * m.m[1][1] - f * m.m[0][1];
	out.m[1][2] = e * m.m[1][2] - f * m.m[0][2];
	out.m[1][3] = e * m.m[1][3] - f * m.m[0][3];

	return(out);
}

//Convert the Z rotation from degrees to radians!
void mat4RotateZDeg(mat4 *m, const float z){
	mat4RotateZRad(m, z * DEG_TO_RAD);
}

//Convert the Z rotation from degrees to radians!
mat4 mat4RotateZDegR(const mat4 m, const float z){
	return(mat4RotateZRadR(m, z * DEG_TO_RAD));
}


//Scale a matrix by three floats!
void mat4Scale(mat4 *m, const float x, const float y, const float z){
	m->m[0][0] *= x;
	m->m[0][1] *= x;
	m->m[0][2] *= x;
	m->m[0][3] *= x;

	m->m[1][0] *= y;
	m->m[1][1] *= y;
	m->m[1][2] *= y;
	m->m[1][3] *= y;

	m->m[2][0] *= z;
	m->m[2][1] *= z;
	m->m[2][2] *= z;
	m->m[2][3] *= z;
}

//Scale a matrix by three floats!
mat4 mat4ScaleR(mat4 m, const float x, const float y, const float z){
	m.m[0][0] *= x;
	m.m[0][1] *= x;
	m.m[0][2] *= x;
	m.m[0][3] *= x;

	m.m[1][0] *= y;
	m.m[1][1] *= y;
	m.m[1][2] *= y;
	m.m[1][3] *= y;

	m.m[2][0] *= z;
	m.m[2][1] *= z;
	m.m[2][2] *= z;
	m.m[2][3] *= z;

	return(m);
}

//Scale a matrix by four floats!
void mat4Scale4(mat4 *m, const float x, const float y, const float z, const float w){
	m->m[0][0] *= x;
	m->m[0][1] *= x;
	m->m[0][2] *= x;
	m->m[0][3] *= x;

	m->m[1][0] *= y;
	m->m[1][1] *= y;
	m->m[1][2] *= y;
	m->m[1][3] *= y;

	m->m[2][0] *= z;
	m->m[2][1] *= z;
	m->m[2][2] *= z;
	m->m[2][3] *= z;

	m->m[3][0] *= w;
	m->m[3][1] *= w;
	m->m[3][2] *= w;
	m->m[3][3] *= w;
}

//Scale a matrix by four floats!
mat4 mat4Scale4R(mat4 m, const float x, const float y, const float z, const float w){
	m.m[0][0] *= x;
	m.m[0][1] *= x;
	m.m[0][2] *= x;
	m.m[0][3] *= x;

	m.m[1][0] *= y;
	m.m[1][1] *= y;
	m.m[1][2] *= y;
	m.m[1][3] *= y;

	m.m[2][0] *= z;
	m.m[2][1] *= z;
	m.m[2][2] *= z;
	m.m[2][3] *= z;

	m.m[3][0] *= w;
	m.m[3][1] *= w;
	m.m[3][2] *= w;
	m.m[3][3] *= w;

	return(m);
}

//Scale a matrix by a vec3!
void mat4ScaleVec3(mat4 *m, const vec3 *v){
	m->m[0][0] *= v->x;
	m->m[0][1] *= v->x;
	m->m[0][2] *= v->x;
	m->m[0][3] *= v->x;

	m->m[1][0] *= v->y;
	m->m[1][1] *= v->y;
	m->m[1][2] *= v->y;
	m->m[1][3] *= v->y;

	m->m[2][0] *= v->z;
	m->m[2][1] *= v->z;
	m->m[2][2] *= v->z;
	m->m[2][3] *= v->z;
}

//Scale a matrix by a vec3!
mat4 mat4ScaleVec3R(mat4 m, const vec3 v){
	m.m[0][0] *= v.x;
	m.m[0][1] *= v.x;
	m.m[0][2] *= v.x;
	m.m[0][3] *= v.x;

	m.m[1][0] *= v.y;
	m.m[1][1] *= v.y;
	m.m[1][2] *= v.y;
	m.m[1][3] *= v.y;

	m.m[2][0] *= v.z;
	m.m[2][1] *= v.z;
	m.m[2][2] *= v.z;
	m.m[2][3] *= v.z;

	return(m);
}

//Scale a matrix by a vec4!
void mat4ScaleVec4(mat4 *m, const vec4 *v){
	m->m[0][0] *= v->x;
	m->m[0][1] *= v->x;
	m->m[0][2] *= v->x;
	m->m[0][3] *= v->x;

	m->m[1][0] *= v->y;
	m->m[1][1] *= v->y;
	m->m[1][2] *= v->y;
	m->m[1][3] *= v->y;

	m->m[2][0] *= v->z;
	m->m[2][1] *= v->z;
	m->m[2][2] *= v->z;
	m->m[2][3] *= v->z;

	m->m[3][0] *= v->w;
	m->m[3][1] *= v->w;
	m->m[3][2] *= v->w;
	m->m[3][3] *= v->w;
}

//Scale a matrix by a vec4!
mat4 mat4ScaleVec4R(mat4 m, const vec4 v){
	m.m[0][0] *= v.x;
	m.m[0][1] *= v.x;
	m.m[0][2] *= v.x;
	m.m[0][3] *= v.x;

	m.m[1][0] *= v.y;
	m.m[1][1] *= v.y;
	m.m[1][2] *= v.y;
	m.m[1][3] *= v.y;

	m.m[2][0] *= v.z;
	m.m[2][1] *= v.z;
	m.m[2][2] *= v.z;
	m.m[2][3] *= v.z;

	m.m[3][0] *= v.w;
	m.m[3][1] *= v.w;
	m.m[3][2] *= v.w;
	m.m[3][3] *= v.w;

	return(m);
}


/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat4Transpose(mat4 *m){
	const mat4 tempMatrix = *m;

	m->m[0][0] = tempMatrix.m[0][0];
	m->m[0][1] = tempMatrix.m[1][0];
	m->m[0][2] = tempMatrix.m[2][0];
	m->m[0][3] = tempMatrix.m[3][0];

	m->m[1][0] = tempMatrix.m[0][1];
	m->m[1][1] = tempMatrix.m[1][1];
	m->m[1][2] = tempMatrix.m[2][1];
	m->m[1][3] = tempMatrix.m[3][1];

	m->m[2][0] = tempMatrix.m[0][2];
	m->m[2][1] = tempMatrix.m[1][2];
	m->m[2][2] = tempMatrix.m[2][2];
	m->m[2][3] = tempMatrix.m[3][2];

	m->m[3][0] = tempMatrix.m[0][3];
	m->m[3][1] = tempMatrix.m[1][3];
	m->m[3][2] = tempMatrix.m[2][3];
	m->m[3][3] = tempMatrix.m[3][3];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
void mat4TransposeOut(const mat4 m, mat4 *out){
	out->m[0][0] = m.m[0][0];
	out->m[0][1] = m.m[1][0];
	out->m[0][2] = m.m[2][0];
	out->m[0][3] = m.m[3][0];

	out->m[1][0] = m.m[0][1];
	out->m[1][1] = m.m[1][1];
	out->m[1][2] = m.m[2][1];
	out->m[1][3] = m.m[3][1];

	out->m[2][0] = m.m[0][2];
	out->m[2][1] = m.m[1][2];
	out->m[2][2] = m.m[2][2];
	out->m[2][3] = m.m[3][2];

	out->m[3][0] = m.m[0][3];
	out->m[3][1] = m.m[1][3];
	out->m[3][2] = m.m[2][3];
	out->m[3][3] = m.m[3][3];
}

/*
** Find the transpose of a matrix! For column-major matrices, this effectively
** translates it to a row-major matrix. The reverse is true for row-major matrices.
*/
mat4 mat4TransposeR(const mat4 m){
	mat4 out;

	out.m[0][0] = m.m[0][0];
	out.m[0][1] = m.m[1][0];
	out.m[0][2] = m.m[2][0];
	out.m[0][3] = m.m[3][0];

	out.m[1][0] = m.m[0][1];
	out.m[1][1] = m.m[1][1];
	out.m[1][2] = m.m[2][1];
	out.m[1][3] = m.m[3][1];

	out.m[2][0] = m.m[0][2];
	out.m[2][1] = m.m[1][2];
	out.m[2][2] = m.m[2][2];
	out.m[2][3] = m.m[3][2];

	out.m[3][0] = m.m[0][3];
	out.m[3][1] = m.m[1][3];
	out.m[3][2] = m.m[2][3];
	out.m[3][3] = m.m[3][3];

	return(out);
}

//Invert a matrix!
void mat4Invert(mat4 *m){
	const mat4 tempMatrix = *m;

	const float f0 =  tempMatrix.m[1][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[1][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[2][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[2][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[3][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[3][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f1 = -tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f2 =  tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] -
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] +
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1];
	const float f3 = -tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] + tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] - tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] + tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1];
	float invDet = tempMatrix.m[0][0] * f0 + tempMatrix.m[0][1] * f1 + tempMatrix.m[0][2] * f2 + tempMatrix.m[0][3] * f3;

	if(invDet != 0.f){
		invDet = 1.f / invDet;

		m->m[0][0] =  f0 * invDet;
		m->m[0][1] = -tempMatrix.m[0][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[0][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[0][2] =  tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[0][3] = -tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][0] =  f1 * invDet;
		m->m[1][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[1][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[2][0] =  f2 * invDet;
		m->m[2][1] = -tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1];
		m->m[2][2] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[2][3] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[3][0] =  f3 * invDet;
		m->m[3][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] - tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] + tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] - tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1];
		m->m[3][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] + tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] - tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] + tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];
		m->m[3][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] + tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];
	}
}

//Invert a matrix and store the result in "out"!
void mat4InvertOut(const mat4 m, mat4 *out){
	const float f0 =  m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                  m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
	                  m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
	const float f1 = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                  m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
	                  m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];
	const float f2 =  m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                  m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
	                  m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];
	const float f3 = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                  m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
	                  m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2 + m.m[0][3] * f3;

	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] =  f0 * invDet;
		out->m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
		                m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
		                m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
		out->m[0][2] =  m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		                m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
		                m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
		out->m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
		                m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
		                m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
		out->m[1][0] =  f1 * invDet;
		out->m[1][1] =  m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		                m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
		                m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];
		out->m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
		                m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
		                m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];
		out->m[1][3] =  m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		                m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
		                m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];
		out->m[2][0] =  f2 * invDet;
		out->m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
		                m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];
		out->m[2][2] =  m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		                m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];
		out->m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
		                m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
		                m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];
		out->m[3][0] =  f3 * invDet;
		out->m[3][1] =  m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		                m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];
		out->m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
		                m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];
		out->m[3][3] =  m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		                m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
		                m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];
	}
}

//Invert a matrix!
mat4 mat4InvertR(const mat4 m){
	const float f0 =  m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                  m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
	                  m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
	const float f1 = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                  m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
	                  m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];
	const float f2 =  m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                  m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
	                  m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];
	const float f3 = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                  m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
	                  m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2 + m.m[0][3] * f3;

	if(invDet != 0.f){
		mat4 out;

		invDet = 1.f / invDet;

		out.m[0][0] =  f0 * invDet;
		out.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
		                m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
		                m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
		out.m[0][2] =  m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		                m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
		                m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
		out.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
		                m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
		                m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
		out.m[1][0] =  f1 * invDet;
		out.m[1][1] =  m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		                m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
		                m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];
		out.m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
		                m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
		                m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];
		out.m[1][3] =  m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		                m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
		                m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];
		out.m[2][0] =  f2 * invDet;
		out.m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
		                m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];
		out.m[2][2] =  m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		                m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];
		out.m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
		                m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
		                m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];
		out.m[3][0] =  f3 * invDet;
		out.m[3][1] =  m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		                m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];
		out.m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
		                m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];
		out.m[3][3] =  m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		                m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
		                m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];

		return(out);
	}

	//If we couldn't invert the matrix, return the original one!
	return(m);
}

//Invert a matrix and return whether or not we were successful!
return_t mat4CanInvert(mat4 *m){
	const mat4 tempMatrix = *m;

	const float f0 =  tempMatrix.m[1][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[1][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[2][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[2][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[3][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[3][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f1 = -tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2];
	const float f2 =  tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] -
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] +
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1];
	const float f3 = -tempMatrix.m[1][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] + tempMatrix.m[1][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] +
	                  tempMatrix.m[2][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] - tempMatrix.m[2][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] -
	                  tempMatrix.m[3][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] + tempMatrix.m[3][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1];
	float invDet = tempMatrix.m[0][0] * f0 + tempMatrix.m[0][1] * f1 + tempMatrix.m[0][2] * f2 + tempMatrix.m[0][3] * f3;

	if(invDet != 0.f){
		invDet = 1.f / invDet;

		m->m[0][0] =  f0 * invDet;
		m->m[0][1] = -tempMatrix.m[0][1] * tempMatrix.m[2][2] * tempMatrix.m[3][3] + tempMatrix.m[0][1] * tempMatrix.m[2][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[0][2] =  tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[3][3] - tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[3][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[0][3] = -tempMatrix.m[0][1] * tempMatrix.m[1][2] * tempMatrix.m[2][3] + tempMatrix.m[0][1] * tempMatrix.m[1][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[1][1] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[1][1] * tempMatrix.m[0][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[2][1] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[2][1] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][0] =  f1 * invDet;
		m->m[1][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2];
		m->m[1][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][2] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][2] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[1][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][2] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][2] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][2];
		m->m[2][0] =  f2 * invDet;
		m->m[2][1] = -tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][3] + tempMatrix.m[0][0] * tempMatrix.m[2][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] - tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] + tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1];
		m->m[2][2] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][3] - tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[3][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][3] + tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] - tempMatrix.m[3][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[2][3] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][3] + tempMatrix.m[0][0] * tempMatrix.m[1][3] * tempMatrix.m[2][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][3] - tempMatrix.m[1][0] * tempMatrix.m[0][3] * tempMatrix.m[2][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][3] + tempMatrix.m[2][0] * tempMatrix.m[0][3] * tempMatrix.m[1][1];
		m->m[3][0] =  f3 * invDet;
		m->m[3][1] =  tempMatrix.m[0][0] * tempMatrix.m[2][1] * tempMatrix.m[3][2] - tempMatrix.m[0][0] * tempMatrix.m[2][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] + tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] - tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1];
		m->m[3][2] = -tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[3][2] + tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[3][1] +
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[3][2] - tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[3][1] -
		              tempMatrix.m[3][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] + tempMatrix.m[3][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];
		m->m[3][3] =  tempMatrix.m[0][0] * tempMatrix.m[1][1] * tempMatrix.m[2][2] - tempMatrix.m[0][0] * tempMatrix.m[1][2] * tempMatrix.m[2][1] -
		              tempMatrix.m[1][0] * tempMatrix.m[0][1] * tempMatrix.m[2][2] + tempMatrix.m[1][0] * tempMatrix.m[0][2] * tempMatrix.m[2][1] +
		              tempMatrix.m[2][0] * tempMatrix.m[0][1] * tempMatrix.m[1][2] - tempMatrix.m[2][0] * tempMatrix.m[0][2] * tempMatrix.m[1][1];


		return(1);
	}


	return(0);
}

/*
** Invert a matrix, storing the result in "out"
** and returning whether or not we were successful!
*/
return_t mat4CanInvertOut(const mat4 m, mat4 *out){
	const float f0 =  m.m[1][1] * m.m[2][2] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                  m.m[2][1] * m.m[1][2] * m.m[3][3] + m.m[2][1] * m.m[1][3] * m.m[3][2] +
	                  m.m[3][1] * m.m[1][2] * m.m[2][3] - m.m[3][1] * m.m[1][3] * m.m[2][2];
	const float f1 = -m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                  m.m[2][0] * m.m[1][2] * m.m[3][3] - m.m[2][0] * m.m[1][3] * m.m[3][2] -
	                  m.m[3][0] * m.m[1][2] * m.m[2][3] + m.m[3][0] * m.m[1][3] * m.m[2][2];
	const float f2 =  m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                  m.m[2][0] * m.m[1][1] * m.m[3][3] + m.m[2][0] * m.m[1][3] * m.m[3][1] +
	                  m.m[3][0] * m.m[1][1] * m.m[2][3] - m.m[3][0] * m.m[1][3] * m.m[2][1];
	const float f3 = -m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                  m.m[2][0] * m.m[1][1] * m.m[3][2] - m.m[2][0] * m.m[1][2] * m.m[3][1] -
	                  m.m[3][0] * m.m[1][1] * m.m[2][2] + m.m[3][0] * m.m[1][2] * m.m[2][1];
	float invDet = m.m[0][0] * f0 + m.m[0][1] * f1 + m.m[0][2] * f2 + m.m[0][3] * f3;

	if(invDet != 0.f){
		invDet = 1.f / invDet;

		out->m[0][0] =  f0 * invDet;
		out->m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2] +
		                m.m[2][1] * m.m[0][2] * m.m[3][3] - m.m[2][1] * m.m[0][3] * m.m[3][2] -
		                m.m[3][1] * m.m[0][2] * m.m[2][3] + m.m[3][1] * m.m[0][3] * m.m[2][2];
		out->m[0][2] =  m.m[0][1] * m.m[1][2] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2] -
		                m.m[1][1] * m.m[0][2] * m.m[3][3] + m.m[1][1] * m.m[0][3] * m.m[3][2] +
		                m.m[3][1] * m.m[0][2] * m.m[1][3] - m.m[3][1] * m.m[0][3] * m.m[1][2];
		out->m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2] +
		                m.m[1][1] * m.m[0][2] * m.m[2][3] - m.m[1][1] * m.m[0][3] * m.m[2][2] -
		                m.m[2][1] * m.m[0][2] * m.m[1][3] + m.m[2][1] * m.m[0][3] * m.m[1][2];
		out->m[1][0] =  f1 * invDet;
		out->m[1][1] =  m.m[0][0] * m.m[2][2] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2] -
		                m.m[2][0] * m.m[0][2] * m.m[3][3] + m.m[2][0] * m.m[0][3] * m.m[3][2] +
		                m.m[3][0] * m.m[0][2] * m.m[2][3] - m.m[3][0] * m.m[0][3] * m.m[2][2];
		out->m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2] +
		                m.m[1][0] * m.m[0][2] * m.m[3][3] - m.m[1][0] * m.m[0][3] * m.m[3][2] -
		                m.m[3][0] * m.m[0][2] * m.m[1][3] + m.m[3][0] * m.m[0][3] * m.m[1][2];
		out->m[1][3] =  m.m[0][0] * m.m[1][2] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2] -
		                m.m[1][0] * m.m[0][2] * m.m[2][3] + m.m[1][0] * m.m[0][3] * m.m[2][2] +
		                m.m[2][0] * m.m[0][2] * m.m[1][3] - m.m[2][0] * m.m[0][3] * m.m[1][2];
		out->m[2][0] =  f2 * invDet;
		out->m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1] +
		                m.m[2][0] * m.m[0][1] * m.m[3][3] - m.m[2][0] * m.m[0][3] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[2][3] + m.m[3][0] * m.m[0][3] * m.m[2][1];
		out->m[2][2] =  m.m[0][0] * m.m[1][1] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1] -
		                m.m[1][0] * m.m[0][1] * m.m[3][3] + m.m[1][0] * m.m[0][3] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[1][3] - m.m[3][0] * m.m[0][3] * m.m[1][1];
		out->m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1] +
		                m.m[1][0] * m.m[0][1] * m.m[2][3] - m.m[1][0] * m.m[0][3] * m.m[2][1] -
		                m.m[2][0] * m.m[0][1] * m.m[1][3] + m.m[2][0] * m.m[0][3] * m.m[1][1];
		out->m[3][0] =  f3 * invDet;
		out->m[3][1] =  m.m[0][0] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1] -
		                m.m[2][0] * m.m[0][1] * m.m[3][2] + m.m[2][0] * m.m[0][2] * m.m[3][1] +
		                m.m[3][0] * m.m[0][1] * m.m[2][2] - m.m[3][0] * m.m[0][2] * m.m[2][1];
		out->m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1] +
		                m.m[1][0] * m.m[0][1] * m.m[3][2] - m.m[1][0] * m.m[0][2] * m.m[3][1] -
		                m.m[3][0] * m.m[0][1] * m.m[1][2] + m.m[3][0] * m.m[0][2] * m.m[1][1];
		out->m[3][3] =  m.m[0][0] * m.m[1][1] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1] -
		                m.m[1][0] * m.m[0][1] * m.m[2][2] + m.m[1][0] * m.m[0][2] * m.m[2][1] +
		                m.m[2][0] * m.m[0][1] * m.m[1][2] - m.m[2][0] * m.m[0][2] * m.m[1][1];


		return(1);
	}


	return(0);
}


//Generate an orthographic matrix!
void mat4Orthographic(mat4 *m, const float right, const float left, const float bottom, const float top, const float near, const float far){
	const float invRightMinLeft = 1.f / (right - left);
	const float invTopMinBottom = 1.f / (top - bottom);
	const float invFarMinNear   = 1.f / (far - near);

	m->m[0][0] = -invRightMinLeft - invRightMinLeft;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = -((right + left) * invRightMinLeft);

	m->m[1][0] = 0.f;
	m->m[1][1] = invTopMinBottom + invTopMinBottom;
	m->m[1][2] = 0.f;
	m->m[1][3] = -((top + bottom) * invTopMinBottom);

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = -invFarMinNear - invFarMinNear;
	m->m[2][3] = 0.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = -((far + near) * invFarMinNear);
	m->m[3][3] = 1.f;
}

//Generate an orthographic matrix!
mat4 mat4OrthographicR(const float right, const float left, const float bottom, const float top, const float near, const float far){
	const float invRightMinLeft = 1.f / (right - left);
	const float invTopMinBottom = 1.f / (top - bottom);
	const float invFarMinNear   = 1.f / (far - near);
	mat4 m;

	m.m[0][0] = -invRightMinLeft - invRightMinLeft;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = -((right + left) * invRightMinLeft);

	m.m[1][0] = 0.f;
	m.m[1][1] = invTopMinBottom + invTopMinBottom;
	m.m[1][2] = 0.f;
	m.m[1][3] = -((top + bottom) * invTopMinBottom);

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = -invFarMinNear - invFarMinNear;
	m.m[2][3] = 0.f;

	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = -((far + near) * invFarMinNear);
	m.m[3][3] = 1.f;

	return(m);
}

//Generate a perspective matrix!
void mat4Perspective(mat4 *m, const float fov, const float aspectRatio, const float near, const float far){
	const float invScale = 1.f / tan(fov * 0.5f);
	const float invFarMinNear = 1.f / (far - near);

	m->m[0][0] = invScale / aspectRatio;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = invScale;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = -((far + near) * invFarMinNear);
	m->m[2][3] = -1.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = -((far + far * near) * invFarMinNear);
	m->m[3][3] = 0.f;
}

//Generate a perspective matrix!
mat4 mat4PerspectiveR(const float fov, const float aspectRatio, const float near, const float far){
	const float invScale = 1.f / tan(fov * 0.5f);
	const float invFarMinNear = 1.f / (far - near);
	mat4 m;

	m.m[0][0] = invScale / aspectRatio;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][1] = invScale;
	m.m[1][2] = 0.f;
	m.m[1][3] = 0.f;

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = -((far + near) * invFarMinNear);
	m.m[2][3] = -1.f;

	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = -((far + far * near) * invFarMinNear);
	m.m[3][3] = 0.f;

	return(m);
}

//Generate a perspective matrix using the plain, unoptimized method!
void mat4PerspectiveOld(mat4 *m, const float fov, const float aspectRatio, const float near, const float far){
	const float top    = near * tanf(fov * 0.5f);
	const float right  = top * aspectRatio;
	const float invRightMinLeft = 1.f / (right + right);
	const float invTopMinBottom = 1.f / (top + top);
	const float invFarMinNear   = 1.f / (far - near);

	m->m[0][0] = (near + near) * invRightMinLeft;
	m->m[0][1] = 0.f;
	m->m[0][2] = 0.f;
	m->m[0][3] = 0.f;

	m->m[1][0] = 0.f;
	m->m[1][1] = (near + near) * invTopMinBottom;
	m->m[1][2] = 0.f;
	m->m[1][3] = 0.f;

	m->m[2][0] = 0.f;
	m->m[2][1] = 0.f;
	m->m[2][2] = -((far + near) * invFarMinNear);
	m->m[2][3] = -1.f;

	m->m[3][0] = 0.f;
	m->m[3][1] = 0.f;
	m->m[3][2] = -((far + far * near) * invFarMinNear);
	m->m[3][3] = 0.f;
}

//Generate a perspective matrix using the plain, unoptimized method!
mat4 mat4PerspectiveOldR(const float fov, const float aspectRatio, const float near, const float far){
	const float top    = near * tanf(fov * 0.5f);
	const float right  = top * aspectRatio;
	const float invRightMinLeft = 1.f / (right + right);
	const float invTopMinBottom = 1.f / (top + top);
	const float invFarMinNear   = 1.f / (far - near);
	mat4 m;

	m.m[0][0] = (near + near) * invRightMinLeft;
	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;
	m.m[0][3] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][1] = (near + near) * invTopMinBottom;
	m.m[1][2] = 0.f;
	m.m[1][3] = 0.f;

	m.m[2][0] = 0.f;
	m.m[2][1] = 0.f;
	m.m[2][2] = -((far + near) * invFarMinNear);
	m.m[2][3] = -1.f;

	m.m[3][0] = 0.f;
	m.m[3][1] = 0.f;
	m.m[3][2] = -((far + far * near) * invFarMinNear);
	m.m[3][3] = 0.f;

	return(m);
}

//Generate a look-at matrix!
void mat4LookAt(mat4 *m, const vec3 *eye, const vec3 *target, const vec3 *worldUp){
	vec3 right, up, forward;
	//Get the forward vector!
	vec3Normalize(eye->x - target->x, eye->y - target->y, eye->z - target->z, &forward);
	//Get the right vector!
	vec3CrossVec3Out(worldUp, &forward, &right);
	vec3NormalizeVec3(&right);
	//Get the up vector!
	vec3CrossVec3Out(&forward, &right, &up);

	//Translate the matrix to eye and make it look at target!
	m->m[0][0] = right.x;
	m->m[0][1] = up.x;
	m->m[0][2] = forward.x;
	m->m[0][3] = 0.f;

	m->m[1][0] = right.y;
	m->m[1][1] = up.y;
	m->m[1][2] = forward.y;
	m->m[1][3] = 0.f;

	m->m[2][0] = right.z;
	m->m[2][1] = up.z;
	m->m[2][2] = forward.z;
	m->m[2][3] = 0.f;

	m->m[3][0] = -vec3DotVec3(&right, eye);
	m->m[3][1] = -vec3DotVec3(&up, eye);
	m->m[3][2] = -vec3DotVec3(&forward, eye);
	m->m[3][3] = 1.f;
}

//Generate a look-at matrix!
mat4 mat4LookAtR(const vec3 eye, const vec3 target, const vec3 worldUp){
	const vec3 forward = vec3NormalizeVec3R(vec3SubtractVec3FromR(eye, target));
	const vec3 right = vec3NormalizeVec3R(vec3CrossVec3R(worldUp, forward));
	const vec3 up = vec3CrossVec3R(forward, right);
	mat4 m;

	//Translate the matrix to eye and make it look at target!
	m.m[0][0] = right.x;
	m.m[0][1] = up.x;
	m.m[0][2] = forward.x;
	m.m[0][3] = 0.f;

	m.m[1][0] = right.y;
	m.m[1][1] = up.y;
	m.m[1][2] = forward.y;
	m.m[1][3] = 0.f;

	m.m[2][0] = right.z;
	m.m[2][1] = up.z;
	m.m[2][2] = forward.z;
	m.m[2][3] = 0.f;

	m.m[3][0] = -vec3DotVec3R(right, eye);
	m.m[3][1] = -vec3DotVec3R(up, eye);
	m.m[3][2] = -vec3DotVec3R(forward, eye);
	m.m[3][3] = 1.f;

	return(m);
}