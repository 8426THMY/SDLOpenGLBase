#ifndef mat4_h
#define mat4_h


#include "utilTypes.h"

#include "vec3.h"
#include "vec4.h"
#include "quat.h"


//Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat4 {
	float m[4][4];
} mat4;


void mat4InitZero(mat4 *m);
void mat4InitIdentity(mat4 *m);
void mat4InitTranslate(mat4 *m, const float x, const float y, const float z);
void mat4InitTranslate4(mat4 *m, const float x, const float y, const float z, const float w);
void mat4InitTranslateVec3(mat4 *m, const vec3 *v);
void mat4InitTranslateVec4(mat4 *m, const vec4 *v);
void mat4InitRotateQuat(mat4 *m, const quat *q);
void mat4InitScale(mat4 *m, const float x, const float y, const float z);
void mat4InitScale4(mat4 *m, const float x, const float y, const float z, const float w);
void mat4InitScaleVec3(mat4 *m, const vec3 *v);
void mat4InitScaleVec4(mat4 *m, const vec4 *v);

void mat4MultiplyByVec3(const mat4 *m, vec3 *v);
void mat4MultiplyByVec3Out(const mat4 *m, const vec3 *v, vec3 *out);
void mat4MultiplyVec4By(mat4 *m, const vec4 *v);
void mat4MultiplyVec4ByOut(const mat4 *m, const vec4 *v, mat4 *out);
void mat4MultiplyByVec4(const mat4 *m, vec4 *v);
void mat4MultiplyByVec4Out(const mat4 *m, const vec4 *v, vec4 *out);
void mat4MultiplyByMat4(mat4 *m1, const mat4 *m2, mat4 *out);
void mat4MultiplyMat4By(mat4 *m1, const mat4 *m2, mat4 *out);
void mat4MultiplyByMat4Out(const mat4 *m1, const mat4 *m2, mat4 *out);

void mat4Translate(mat4 *m, const float x, const float y, const float z);
void mat4Translate4(mat4 *m, const float x, const float y, const float z, const float w);
void mat4TranslateVec3(mat4 *m, const vec3 *v);
void mat4TranslateVec4(mat4 *m, const vec4 *v);

void mat4RotateRad(mat4 *m, const float x, const float y, const float z);
void mat4RotateDeg(mat4 *m, const float x, const float y, const float z);
void mat4RotateVec3Rad(mat4 *m, const vec3 *v);
void mat4RotateVec3Deg(mat4 *m, const vec3 *v);
void mat4RotateAxisAngle(mat4 *m, const vec4 *v);
void mat4RotateQuat(mat4 *m, const quat *q);
void mat4RotateXRad(mat4 *m, const float x);
void mat4RotateXDeg(mat4 *m, const float x);
void mat4RotateYRad(mat4 *m, const float y);
void mat4RotateYDeg(mat4 *m, const float y);
void mat4RotateZRad(mat4 *m, const float z);
void mat4RotateZDeg(mat4 *m, const float z);

void mat4Scale(mat4 *m, const float x, const float y, const float z);
void mat4Scale4(mat4 *m, const float x, const float y, const float z, const float w);
void mat4ScaleVec3(mat4 *m, const vec3 *v);
void mat4ScaleVec4(mat4 *m, const vec4 *v);

void mat4Transpose(mat4 *m);
void mat4TransposeOut(const mat4 *m, mat4 *out);
void mat4Invert(mat4 *m);
void mat4InvertOut(const mat4 *m, mat4 *out);
return_t mat4InvertR(mat4 *m);
return_t mat4InvertROut(const mat4 *m, mat4 *out);

void mat4Orthographic(mat4 *m, const float right, const float left, const float bottom, const float top, const float near, const float far);
void mat4Perspective(mat4 *m, const float fov, const float aspectRatio, const float near, const float far);
void mat4PerspectiveOld(mat4 *m, const float fov, const float aspectRatio, const float near, const float far);
void mat4LookAt(mat4 *m, const vec3 *eye, const vec3 *target, const vec3 *wolrdUp);


#endif