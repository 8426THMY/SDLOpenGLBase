#ifndef mat3_h
#define mat3_h


#include "utilTypes.h"

#include "vec3.h"
#include "quat.h"


// Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat3 {
	float m[3][3];
} mat3;


void mat3InitZero(mat3 *const restrict m);
mat3 mat3InitZeroC();
void mat3InitIdentity(mat3 *const restrict m);
mat3 mat3InitIdentityC();
void mat3InitDiagonal(mat3 *const restrict m, const float x);
mat3 mat3InitDiagonalC(const float x);
void mat3InitSkew(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3InitSkewC(const vec3 v);

void mat3AddMat3(mat3 *const restrict m1, const mat3 *const restrict m2);
void mat3AddMat3Out(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict out);
mat3 mat3AddMat3C(const mat3 m1, const mat3 m2);

void mat3MultiplyByVec3(const mat3 *const restrict m, vec3 *const restrict v);
void mat3MultiplyByVec3Out(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out);
vec3 mat3MultiplyByVec3C(const mat3 m, vec3 v);
void mat3MultiplyVec3By(mat3 *const restrict m, const vec3 *const restrict v);
void mat3MultiplyVec3ByOut(const mat3 m, const vec3 *const restrict v, mat3 *const restrict out);
mat3 mat3MultiplyVec3ByC(mat3 m, const vec3 v);
void mat3MultiplyByMat3(mat3 *const restrict m1, const mat3 m2);
void mat3MultiplyMat3By(mat3 *const restrict m1, const mat3 m2);
void mat3MultiplyByMat3Out(const mat3 m1, const mat3 m2, mat3 *const restrict out);
mat3 mat3MultiplyByMat3C(const mat3 m1, const mat3 m2);

void mat3ScalePre(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3ScalePreC(mat3 m, const float x, const float y, const float z);
void mat3ScalePreVec3(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3ScalePreVec3C(mat3 m, const vec3 v);
void mat3Scale(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3ScaleC(mat3 m, const float x, const float y, const float z);
void mat3ScaleVec3(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3ScaleVec3C(mat3 m, const vec3 v);

void mat3Transpose(mat3 *const restrict m);
void mat3TransposeOut(const mat3 m, mat3 *const restrict out);
mat3 mat3TransposeC(const mat3 m);
void mat3Invert(mat3 *const restrict m);
void mat3InvertOut(const mat3 m, mat3 *const restrict out);
mat3 mat3InvertC(const mat3 m);
return_t mat3CanInvert(mat3 *const restrict m);
return_t mat3CanInvertOut(const mat3 m, mat3 *const restrict out);

void mat3ToQuat(const mat3 *const restrict m, quat *const restrict out);
quat mat3ToQuatC(const mat3 m);
void mat3ToQuatAlt(const mat3 *const restrict m, quat *const restrict out);
quat mat3ToQuatAltC(const mat3 m);
void quatToMat3(const quat *const restrict q, mat3 *const restrict out);
mat3 quatToMat3C(const quat q);


extern mat3 g_mat3Identity;


#endif