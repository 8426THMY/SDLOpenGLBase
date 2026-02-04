#ifndef mat3_h
#define mat3_h


#include "vec3.h"
#include "vec4.h"
#include "quat.h"

#include "utilTypes.h"


// Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat3 {
	float m[3][3];
} mat3;


void mat3InitZero(mat3 *const restrict m);
mat3 mat3InitZeroC();
void mat3InitIdentity(mat3 *const restrict m);
mat3 mat3InitIdentityC();
void mat3InitEulerXYZ(mat3 *const restrict m, const float x, const float y, const float z);
void mat3InitEulerZXY(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3InitEulerXYZC(const float x, const float y, const float z);
mat3 mat3InitEulerZXYC(const float x, const float y, const float z);
void mat3InitEulerVec3XYZ(mat3 *const restrict m, const vec3 *const restrict v);
void mat3InitEulerVec3ZXY(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3InitEulerVec3XYZC(const vec3 v);
mat3 mat3InitEulerVec3ZXYC(const vec3 v);
void mat3InitAxisAngle(mat3 *const restrict m, const vec4 *const restrict v);
mat3 mat3InitAxisAngleC(const vec4 v);
void mat3InitQuat(mat3 *const restrict m, const quat *const restrict q);
mat3 mat3InitQuatC(const quat q);
void mat3InitScale(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3InitScaleC(const float x, const float y, const float z);
void mat3InitScaleVec3(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3InitScaleVec3C(const vec3 v);
void mat3InitDiagonal(mat3 *const restrict m, const float x);
mat3 mat3InitDiagonalC(const float x);
void mat3InitSkew(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3InitSkewC(const vec3 v);
void mat3InitShearQuat(
	mat3 *const restrict m,
	const vec3 *const restrict v, const quat *const restrict q
);
mat3 mat3InitShearQuatC(const vec3 v, const quat q);

void mat3AddMat3(mat3 *const restrict m1, const mat3 *const restrict m2);
void mat3AddMat3Out(const mat3 *const restrict m1, const mat3 *const restrict m2, mat3 *const restrict out);
mat3 mat3AddMat3C(const mat3 m1, const mat3 m2);

void mat3MultiplyS(mat3 *const restrict m, const float x);
void mat3MultiplySOut(const mat3 *const restrict m, const float x, mat3 *const restrict out);
mat3 mat3MultiplySOutC(mat3 m, const float x);
void mat3MultiplyVec3(const mat3 *const restrict m, vec3 *const restrict v);
void mat3MultiplyVec3Out(const mat3 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out);
vec3 mat3MultiplyVec3C(const mat3 m, vec3 v);
void vec3MultiplyMat3(vec3 *const restrict v, const mat3 *const restrict m);
void vec3MultiplyMat3Out(const vec3 *const restrict v, const mat3 *const restrict m, vec3 *const restrict out);
vec3 vec3MultiplyMat3C(const vec3 v, const mat3 m);
void mat3MultiplyMat3P1(mat3 *const restrict m1, const mat3 m2);
void mat3MultiplyMat3P2(const mat3 m1, mat3 *const restrict m2);
void mat3MultiplyMat3Out(const mat3 m1, const mat3 m2, mat3 *const restrict out);
mat3 mat3MultiplyMat3C(const mat3 m1, const mat3 m2);
void mat3MultiplyMat3TransP1(mat3 *const restrict m1, const mat3 m2);
void mat3MultiplyMat3TransP2(const mat3 m1, mat3 *const restrict m2);
void mat3MultiplyMat3TransOut(const mat3 m1, const mat3 m2, mat3 *const restrict out);
mat3 mat3MultiplyMat3TransC(const mat3 m1, const mat3 m2);
void mat3TransMultiplyMat3P1(mat3 *const restrict m1, const mat3 m2);
void mat3TransMultiplyMat3P2(const mat3 m1, mat3 *const restrict m2);
void mat3TransMultiplyMat3Out(const mat3 m1, const mat3 m2, mat3 *const restrict out);
mat3 mat3TransMultiplyMat3C(const mat3 m1, const mat3 m2);

void mat3RotateByEulerXYZ(mat3 *const restrict m, const float x, const float y, const float z);
void mat3RotateByEulerZXY(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3RotateByEulerXYZC(const mat3 m, const float x, const float y, const float z);
mat3 mat3RotateByEulerZXYC(const mat3 m, const float x, const float y, const float z);
void mat3RotateByVec3EulerXYZ(mat3 *const restrict m, const vec3 *const restrict v);
void mat3RotateByVec3EulerZXY(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3RotateByVec3EulerXYZC(const mat3 m, const vec3 v);
mat3 mat3RotateByVec3EulerZXYC(const mat3 m, const vec3 v);
void mat3RotateByAxisAngle(mat3 *const restrict m, const vec4 *const restrict v);
mat3 mat3RotateByAxisAngleC(const mat3 m, const vec4 v);
void mat3RotateByQuat(mat3 *const restrict m, const quat *const restrict q);
mat3 mat3RotateByQuatC(const mat3 m, const quat q);
void mat3RotateX(mat3 *const restrict m, const float x);
mat3 mat3RotateXC(const mat3 m, const float x);
void mat3RotateY(mat3 *const restrict m, const float y);
mat3 mat3RotateYC(const mat3 m, const float y);
void mat3RotateZ(mat3 *const restrict m, const float z);
mat3 mat3RotateZC(const mat3 m, const float z);
void mat3RotateToFace(mat3 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict worldUp);
mat3 mat3RotateToFaceC(const vec3 eye, const vec3 target, const vec3 worldUp);

void mat3Scale(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3ScaleC(mat3 m, const float x, const float y, const float z);
void mat3ScaleVec3(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3ScaleVec3C(mat3 m, const vec3 v);
void mat3ScalePre(mat3 *const restrict m, const float x, const float y, const float z);
mat3 mat3ScalePreC(mat3 m, const float x, const float y, const float z);
void mat3ScalePreVec3(mat3 *const restrict m, const vec3 *const restrict v);
mat3 mat3ScalePreVec3C(mat3 m, const vec3 v);

void mat3Transpose(mat3 *const restrict m);
void mat3TransposeOut(const mat3 m, mat3 *const restrict out);
mat3 mat3TransposeC(const mat3 m);
float mat3FrobeniusNormSquared(const mat3 *const restrict m);
float mat3FrobeniusNormSquaredC(const mat3 m);
float mat3Trace(const mat3 *const restrict m);
float mat3TraceC(const mat3 m);
float mat3Determinant(const mat3 *const restrict m);
float mat3DeterminantC(const mat3 m);
float mat3DeterminantColumns(const float *const restrict r0, const float *const restrict r1, const float *const restrict r2);
void mat3AdjugateTrans(const mat3 *const restrict m, mat3 *const restrict out);
void mat3Invert(mat3 *const restrict m);
void mat3InvertOut(const mat3 m, mat3 *const restrict out);
mat3 mat3InvertC(const mat3 m);
return_t mat3CanInvert(mat3 *const restrict m);
return_t mat3CanInvertOut(const mat3 m, mat3 *const restrict out);

void mat3Solve(const mat3 *const restrict A, const vec3 *const restrict b, vec3 *const restrict x);
vec3 mat3SolveC(const mat3 A, const vec3 b);
return_t mat3CanSolve(const mat3 *const restrict A, const vec3 *const restrict b, vec3 *const restrict x);
return_t mat3CanSolveC(const mat3 A, const vec3 b, vec3 *const restrict x);

void mat3PolarDecompose(mat3 A, mat3 *const restrict R);
void mat3DiagonalizeSymmetric(
	float a00, float a01, float a02,
	float a11, float a12, float a22,
	vec3 *const restrict evals, quat *const restrict Q
);

void mat3ToQuat(const mat3 *const restrict m, quat *const restrict out);
quat mat3ToQuatC(const mat3 m);
void mat3ToQuatAlt(const mat3 *const restrict m, quat *const restrict out);
quat mat3ToQuatAltC(const mat3 m);
void quatToMat3(const quat *const restrict q, mat3 *const restrict out);
mat3 quatToMat3C(const quat q);

void mat3Lerp(const mat3 *const restrict m1, const mat3 *const restrict m2, const float time, mat3 *const restrict out);
mat3 mat3LerpC(mat3 m1, const mat3 m2, const float time);


extern mat3 g_mat3Identity;


#endif