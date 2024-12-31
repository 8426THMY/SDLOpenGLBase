#ifndef mat3x4_h
#define mat3x4_h


#include "utilTypes.h"

#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include "quat.h"


// Note: Our matrices are column-major, just the way OpenGL likes them.
// This matrix is rectangular, with four columns and three rows.
typedef struct mat3x4 {
	float m[4][3];
} mat3x4;


void mat3x4InitZero(mat3x4 *const restrict m);
mat3x4 mat3x4InitZeroC();
void mat3x4InitIdentity(mat3x4 *const restrict m);
mat3x4 mat3x4InitIdentityC();
void mat3x4InitTranslate(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4InitTranslateC(const float x, const float y, const float z);
void mat3x4InitTranslateVec3(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4InitTranslateVec3C(const vec3 v);
void mat3x4InitEulerXYZ(mat3x4 *const restrict m, const float x, const float y, const float z);
void mat3x4InitEulerZXY(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4InitEulerXYZC(const float x, const float y, const float z);
mat3x4 mat3x4InitEulerZXYC(const float x, const float y, const float z);
void mat3x4InitEulerVec3XYZ(mat3x4 *const restrict m, const vec3 *const restrict v);
void mat3x4InitEulerVec3ZXY(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4InitEulerVec3XYZC(const vec3 v);
mat3x4 mat3x4InitEulerVec3ZXYC(const vec3 v);
void mat3x4InitAxisAngle(mat3x4 *const restrict m, const vec4 *const restrict v);
mat3x4 mat3x4InitAxisAngleC(const vec4 v);
void mat3x4InitQuat(mat3x4 *const restrict m, const quat *const restrict q);
mat3x4 mat3x4InitQuatC(const quat q);
void mat3x4InitScale(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4InitScaleC(const float x, const float y, const float z);
void mat3x4InitScaleVec3(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4InitScaleVec3C(const vec3 v);
void mat3x4InitShearQuat(
	mat3x4 *const restrict m,
	const vec3 *const restrict v, const quat *const restrict q
);
mat3x4 mat3x4InitShearQuatC(const vec3 v, const quat q);

void mat3x4AddMat3x4(mat3x4 *const restrict m1, const mat3x4 *const restrict m2);
void mat3x4AddMat3x4Out(
	const mat3x4 *const restrict m1, const mat3x4 *const restrict m2,
	mat3x4 *const restrict out
);
mat3x4 mat3x4AddMat3x4C(const mat3x4 m1, const mat3x4 m2);

void mat3x4MultiplyVec3(const mat3x4 *const restrict m, vec3 *const restrict v);
void mat3x4MultiplyVec3Out(
	const mat3x4 *const restrict m, const vec3 *const restrict v,
	vec3 *const restrict out
);
vec3 mat3x4MultiplyVec3C(const mat3x4 m, const vec3 v);
void vec3MultiplyMat3x4(vec3 *const restrict v, const mat3x4 *const restrict m);
void vec3MultiplyMat3x4Out(
	const vec3 *const restrict v, const mat3x4 *const restrict m,
	vec3 *const restrict out
);
vec3 vec3MultiplyMat3x4C(const vec3 v, const mat3x4 m);
void mat3x4MultiplyVec4(const mat3x4 *const restrict m, vec4 *const restrict v);
void mat3x4MultiplyVec4Out(
	const mat3x4 *const restrict m, const vec4 *const restrict v,
	vec4 *const restrict out
);
vec4 mat3x4MultiplyVec4C(const mat3x4 m, const vec4 v);
void mat3x4MultiplyMat3(mat3x4 *const restrict m1, const mat3 m2);
void mat3x4MultiplyMat3Out(const mat3x4 m1, const mat3 m2, mat3x4 *const restrict out);
mat3x4 mat3x4MultiplyMat3C(const mat3x4 m1, const mat3 m2);
void mat3MultiplyMat3x4(const mat3 m1, mat3x4 *const restrict m2);
void mat3MultiplyMat3x4Out(const mat3 m1, const mat3x4 m2, mat3x4 *const restrict out);
mat3x4 mat3MultiplyMat3x4C(const mat3 m1, const mat3x4 m2);
void mat3x4MultiplyMat3x4P1(mat3x4 *const restrict m1, const mat3x4 m2);
void mat3x4MultiplyMat3x4P2(const mat3x4 m1, mat3x4 *const restrict m2);
void mat3x4MultiplyMat3x4Out(const mat3x4 m1, const mat3x4 m2, mat3x4 *const restrict out);
mat3x4 mat3x4MultiplyMat3x4C(const mat3x4 m1, const mat3x4 m2);
void mat3x4MultiplyMat3x4TransP1(mat3x4 *const restrict m1, const mat3x4 m2);
void mat3x4MultiplyMat3x4TransP2(const mat3x4 m1, mat3x4 *const restrict m2);
void mat3x4MultiplyMat3x4TransOut(const mat3x4 m1, const mat3x4 m2, mat3x4 *const restrict out);
mat3x4 mat3x4MultiplyMat3x4TransC(const mat3x4 m1, const mat3x4 m2);
void mat3x4TransMultiplyMat3x4P1(mat3x4 *const restrict m1, const mat3x4 m2);
void mat3x4TransMultiplyMat3x4P2(const mat3x4 m1, mat3x4 *const restrict m2);
void mat3x4TransMultiplyMat3x4Out(const mat3x4 m1, const mat3x4 m2, mat3x4 *const restrict out);
mat3x4 mat3x4TransMultiplyMat3x4C(const mat3x4 m1, const mat3x4 m2);

void mat3x4Translate(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4TranslateC(mat3x4 m, const float x, const float y, const float z);
void mat3x4TranslateVec3(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4TranslateVec3C(const mat3x4 m, const vec3 v);
void mat3x4TranslatePre(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4TranslatePreC(mat3x4 m, const float x, const float y, const float z);
void mat3x4TranslatePreVec3(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4TranslatePreVec3C(const mat3x4 m, const vec3 v);

void mat3x4RotateByEulerXYZ(mat3x4 *const restrict m, const float x, const float y, const float z);
void mat3x4RotateByEulerZXY(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4RotateByEulerXYZC(const mat3x4 m, const float x, const float y, const float z);
mat3x4 mat3x4RotateByEulerZXYC(const mat3x4 m, const float x, const float y, const float z);
void mat3x4RotateByVec3EulerXYZ(mat3x4 *const restrict m, const vec3 *const restrict v);
void mat3x4RotateByVec3EulerZXY(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4RotateByVec3EulerXYZC(const mat3x4 m, const vec3 v);
mat3x4 mat3x4RotateByVec3EulerZXYC(const mat3x4 m, const vec3 v);
void mat3x4RotateByAxisAngle(mat3x4 *const restrict m, const vec4 *const restrict v);
mat3x4 mat3x4RotateByAxisAngleC(const mat3x4 m, const vec4 v);
void mat3x4RotateByQuat(mat3x4 *const restrict m, const quat *const restrict q);
mat3x4 mat3x4RotateByQuatC(const mat3x4 m, const quat q);
void mat3x4RotateX(mat3x4 *const restrict m, const float x);
mat3x4 mat3x4RotateXC(const mat3x4 m, const float x);
void mat3x4RotateY(mat3x4 *const restrict m, const float y);
mat3x4 mat3x4RotateYC(const mat3x4 m, const float y);
void mat3x4RotateZ(mat3x4 *const restrict m, const float z);
mat3x4 mat3x4RotateZC(const mat3x4 m, const float z);
void mat3x4RotateForward(
	mat3x4 *const restrict m,
	const vec3 *const restrict forward,
	const vec3 *const restrict worldUp
);
mat3x4 mat3x4RotateForwardC(const vec3 forward, const vec3 worldUp);
void mat3x4RotateToFace(
	mat3x4 *const restrict m,
	const vec3 *const restrict eye, const vec3 *const restrict target,
	const vec3 *const restrict worldUp
);
mat3x4 mat3x4RotateToFaceC(const vec3 eye, const vec3 target, const vec3 worldUp);

void mat3x4Scale(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4ScaleC(mat3x4 m, const float x, const float y, const float z);
void mat3x4ScaleVec3(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4ScaleVec3C(mat3x4 m, const vec3 v);
void mat3x4ScalePre(mat3x4 *const restrict m, const float x, const float y, const float z);
mat3x4 mat3x4ScalePreC(mat3x4 m, const float x, const float y, const float z);
void mat3x4ScalePreVec3(mat3x4 *const restrict m, const vec3 *const restrict v);
mat3x4 mat3x4ScalePreVec3C(mat3x4 m, const vec3 v);

void mat3x4View(mat3x4 *const restrict m, const vec3 *const restrict pos, const mat3 *const restrict rot);
mat3x4 mat3x4ViewC(const vec3 pos, const mat3 rot);
void mat3x4ViewLookAt(
	mat3x4 *const restrict m,
	const vec3 *const restrict eye, const vec3 *const restrict target,
	const vec3 *const restrict worldUp
);
mat3x4 mat3x4ViewLookAtC(const vec3 eye, const vec3 target, const vec3 worldUp);

void mat3x4ToQuat(const mat3x4 *const restrict m, quat *const restrict out);
quat mat3x4ToQuatC(const mat3x4 m);
void mat3x4ToQuatAlt(const mat3x4 *const restrict m, quat *const restrict out);
quat mat3x4ToQuatAltC(const mat3x4 m);
void quatToMat3x4(const quat *const restrict q, mat3x4 *const restrict out);
mat3x4 quatToMat3x4C(const quat q);


extern mat3x4 g_mat3x4Identity;


#endif