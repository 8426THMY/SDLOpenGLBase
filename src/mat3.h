#ifndef mat3_h
#define mat3_h


#include "utilTypes.h"

#include "vec3.h"
#include "quat.h"


// Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat3 {
	float m[3][3];
} mat3;


void mat3InitZero(mat3 *m);
mat3 mat3InitZeroR();
void mat3InitIdentity(mat3 *m);
mat3 mat3InitIdentityR();
void mat3InitDiagonal(mat3 *m, const float x);
mat3 mat3InitDiagonalR(const float x);
void mat3InitSkew(mat3 *m, const vec3 *v);
mat3 mat3InitSkewR(const vec3 v);

void mat3AddMat3(mat3 *m1, const mat3 *m2);
void mat3AddMat3Out(const mat3 *m1, const mat3 *m2, mat3 *out);
mat3 mat3AddMat3R(const mat3 m1, const mat3 m2);

void mat3MultiplyByVec3(const mat3 *m, vec3 *v);
void mat3MultiplyByVec3Out(const mat3 *m, const vec3 *v, vec3 *out);
vec3 mat3MultiplyByVec3R(const mat3 m, vec3 v);
void mat3MultiplyVec3By(mat3 *m, const vec3 *v);
void mat3MultiplyVec3ByOut(const mat3 m, const vec3 *v, mat3 *out);
mat3 mat3MultiplyVec3ByR(mat3 m, const vec3 v);
void mat3MultiplyByMat3(mat3 *m1, const mat3 *m2);
void mat3MultiplyMat3By(mat3 *m1, const mat3 *m2);
void mat3MultiplyByMat3Out(const mat3 m1, const mat3 m2, mat3 *out);
mat3 mat3MultiplyByMat3R(const mat3 m1, const mat3 m2);

void mat3Transpose(mat3 *m);
void mat3TransposeOut(const mat3 m, mat3 *out);
mat3 mat3TransposeR(const mat3 m);
void mat3Invert(mat3 *m);
void mat3InvertOut(const mat3 m, mat3 *out);
mat3 mat3InvertR(const mat3 m);
return_t mat3CanInvert(mat3 *m);
return_t mat3CanInvertOut(const mat3 m, mat3 *out);

void mat3ToQuat(const mat3 *m, quat *out);
quat mat3ToQuatR(const mat3 m);
void mat3ToQuatAlt(const mat3 *m, quat *out);
quat mat3ToQuatAltR(const mat3 m);
void quatToMat3(const quat *q, mat3 *out);
mat3 quatToMat3R(const quat q);


extern mat3 g_mat3Identity;


#endif