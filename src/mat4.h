#ifndef mat4_h
#define mat4_h


#include "utilTypes.h"

#include "vec3.h"
#include "vec4.h"
#include "quat.h"


// Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat4 {
	float m[4][4];
} mat4;


void mat4InitZero(mat4 *const restrict m);
mat4 mat4InitZeroC();
void mat4InitIdentity(mat4 *const restrict m);
mat4 mat4InitIdentityC();
void mat4InitTranslate(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4InitTranslateC(const float x, const float y, const float z);
void mat4InitTranslate4(mat4 *const restrict m, const float x, const float y, const float z, const float w);
mat4 mat4InitTranslate4C(const float x, const float y, const float z, const float w);
void mat4InitTranslateVec3(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4InitTranslateVec3C(const vec3 v);
void mat4InitTranslateVec4(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4InitTranslateVec4C(const vec4 v);
void mat4InitRotateQuat(mat4 *const restrict m, const quat *const restrict q);
mat4 mat4InitRotateQuatC(const quat q);
void mat4InitScale(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4InitScaleC(const float x, const float y, const float z);
void mat4InitScale4(mat4 *const restrict m, const float x, const float y, const float z, const float w);
mat4 mat4InitScale4C(const float x, const float y, const float z, const float w);
void mat4InitScaleVec3(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4InitScaleVec3C(const vec3 v);
void mat4InitScaleVec4(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4InitScaleVec4C(const vec4 v);

void mat4AddMat4(mat4 *const restrict m1, const mat4 *const restrict m2);
void mat4AddMat4Out(const mat4 *const restrict m1, const mat4 *const restrict m2, mat4 *const restrict out);
mat4 mat4AddMat4C(const mat4 m1, const mat4 m2);

void mat4MultiplyByVec3(const mat4 *const restrict m, vec3 *const restrict v);
void mat4MultiplyByVec3Out(const mat4 *const restrict m, const vec3 *const restrict v, vec3 *const restrict out);
vec3 mat4MultiplyByVec3C(const mat4 m, const vec3 v);
void mat4MultiplyVec4By(mat4 *const restrict m, const vec4 *const restrict v);
void mat4MultiplyVec4ByOut(const mat4 m, const vec4 *const restrict v, mat4 *const restrict out);
mat4 mat4MultiplyVec4ByC(const mat4 m, const vec4 v);
void mat4MultiplyByVec4(const mat4 *const restrict m, vec4 *const restrict v);
void mat4MultiplyByVec4Out(const mat4 *const restrict m, const vec4 *const restrict v, vec4 *const restrict out);
vec4 mat4MultiplyByVec4C(const mat4 m, const vec4 v);
void mat4MultiplyByMat4(mat4 *const restrict m1, const mat4 m2);
void mat4MultiplyMat4By(mat4 *const restrict m1, const mat4 m2);
void mat4MultiplyByMat4Out(const mat4 m1, const mat4 m2, mat4 *const restrict out);
mat4 mat4MultiplyByMat4C(const mat4 m1, const mat4 m2);

void mat4TranslatePre(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4TranslatePreC(mat4 m, const float x, const float y, const float z);
void mat4TranslatePre4(mat4 *const restrict m, const float x, const float y, const float z, const float w);
mat4 mat4TranslatePre4C(mat4 m, const float x, const float y, const float z, const float w);
void mat4TranslatePreVec3(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4TranslatePreVec3C(const mat4 m, const vec3 v);
void mat4TranslatePreVec4(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4TranslatePreVec4C(const mat4 m, const vec4 v);
void mat4Translate(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4TranslateC(mat4 m, const float x, const float y, const float z);
void mat4Translate4(mat4 *const restrict m, const float x, const float y, const float z, const float w);
mat4 mat4Translate4C(mat4 m, const float x, const float y, const float z, const float w);
void mat4TranslateVec3(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4TranslateVec3C(const mat4 m, const vec3 v);
void mat4TranslateVec4(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4TranslateVec4C(const mat4 m, const vec4 v);

void mat4RotateRad(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4RotateRadC(const mat4 m, const float x, const float y, const float z);
void mat4RotateDeg(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4RotateDegC(const mat4 m, const float x, const float y, const float z);
void mat4RotateByVec3Rad(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4RotateByVec3RadC(const mat4 m, const vec3 v);
void mat4RotateByVec3Deg(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4RotateByVec3DegC(const mat4 m, const vec3 v);
void mat4RotateAxisAngle(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4RotateAxisAngleC(const mat4 m, const vec4 v);
void mat4RotateQuat(mat4 *const restrict m, const quat *const restrict q);
mat4 mat4RotateQuatC(const mat4 m, const quat q);
void mat4RotateXRad(mat4 *const restrict m, const float x);
mat4 mat4RotateXRadC(const mat4 m, const float x);
void mat4RotateXDeg(mat4 *const restrict m, const float x);
mat4 mat4RotateXDegC(const mat4 m, const float x);
void mat4RotateYRad(mat4 *const restrict m, const float y);
mat4 mat4RotateYRadC(const mat4 m, const float y);
void mat4RotateYDeg(mat4 *const restrict m, const float y);
mat4 mat4RotateYDegC(const mat4 m, const float y);
void mat4RotateZRad(mat4 *const restrict m, const float z);
mat4 mat4RotateZRadC(const mat4 m, const float z);
void mat4RotateZDeg(mat4 *const restrict m, const float z);
mat4 mat4RotateZDegC(const mat4 m, const float z);

void mat4ScalePre(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4ScalePreC(mat4 m, const float x, const float y, const float z);
void mat4ScalePre4(mat4 *const restrict m, const float x, const float y, const float z, const float w);
mat4 mat4ScalePre4C(mat4 m, const float x, const float y, const float z, const float w);
void mat4ScalePreVec3(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4ScalePreVec3C(mat4 m, const vec3 v);
void mat4ScalePreVec4(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4ScalePreVec4C(mat4 m, const vec4 v);
void mat4Scale(mat4 *const restrict m, const float x, const float y, const float z);
mat4 mat4ScaleC(mat4 m, const float x, const float y, const float z);
void mat4Scale4(mat4 *const restrict m, const float x, const float y, const float z, const float w);
mat4 mat4Scale4C(mat4 m, const float x, const float y, const float z, const float w);
void mat4ScaleVec3(mat4 *const restrict m, const vec3 *const restrict v);
mat4 mat4ScaleVec3C(mat4 m, const vec3 v);
void mat4ScaleVec4(mat4 *const restrict m, const vec4 *const restrict v);
mat4 mat4ScaleVec4C(mat4 m, const vec4 v);

void mat4Transpose(mat4 *const restrict m);
void mat4TransposeOut(const mat4 m, mat4 *const restrict out);
mat4 mat4TransposeC(const mat4 m);
void mat4Invert(mat4 *const restrict m);
void mat4InvertOut(const mat4 m, mat4 *const restrict out);
mat4 mat4InvertC(const mat4 m);
return_t mat4CanInvert(mat4 *const restrict m);
return_t mat4CanInvertOut(const mat4 m, mat4 *const restrict out);

void mat4Orthographic(mat4 *const restrict m, const float right, const float left, const float top, const float bottom, const float near, const float far);
mat4 mat4OrthographicC(const float right, const float left, const float top, const float bottom, const float near, const float far);
void mat4Perspective(mat4 *const restrict m, const float fov, const float aspectRatio, const float near, const float far);
mat4 mat4PerspectiveC(const float fov, const float aspectRatio, const float near, const float far);
void mat4PerspectiveOld(mat4 *const restrict m, const float fov, const float aspectRatio, const float near, const float far);
mat4 mat4PerspectiveOldC(const float fov, const float aspectRatio, const float near, const float far);
void mat4RotateToFace(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict worldUp);
mat4 mat4RotateToFaceC(const vec3 eye, const vec3 target, const vec3 worldUp);
void mat4LookAt(mat4 *const restrict m, const vec3 *const restrict eye, const vec3 *const restrict target, const vec3 *const restrict worldUp);
mat4 mat4LookAtC(const vec3 eye, const vec3 target, const vec3 worldUp);

void mat4ToQuat(const mat4 *const restrict m, quat *const restrict out);
quat mat4ToQuatC(const mat4 m);
void mat4ToQuatAlt(const mat4 *const restrict m, quat *const restrict out);
quat mat4ToQuatAltC(const mat4 m);
void quatToMat4(const quat *const restrict q, mat4 *const restrict out);
mat4 quatToMat4C(const quat q);


extern mat4 g_mat4Identity;


#endif