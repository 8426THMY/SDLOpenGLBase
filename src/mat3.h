#ifndef mat3_h
#define mat3_h


#include "utilTypes.h"

#include "vec3.h"
#include "vec4.h"
#include "quat.h"


//Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat3 {
	float m[3][3];
} mat3;


void mat3InitZero(mat3 *m);
void mat3InitIdentity(mat3 *m);

void mat3MultiplyByVec3(const mat3 *m, vec3 *v);
void mat3MultiplyByVec3Out(const mat3 *m, const vec3 *v, vec3 *out);
void mat3MultiplyVec3By(mat3 *m, const vec3 *v);
void mat3MultiplyVec3ByOut(const mat3 *m, const vec3 *v, mat3 *out);
void mat3MultiplyByMat3(mat3 *m1, const mat3 *m2);
void mat3MultiplyMat3By(mat3 *m1, const mat3 *m2);
void mat3MultiplyByMat3Out(const mat3 *m1, const mat3 *m2, mat3 *out);

void mat3Transpose(mat3 *m);
void mat3TransposeOut(const mat3 *m, mat3 *out);
void mat3Invert(mat3 *m);
void mat3InvertOut(const mat3 *m, mat3 *out);
return_t mat3InvertR(mat3 *m);
return_t mat3InvertROut(const mat3 *m, mat3 *out);


#endif