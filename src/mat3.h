#ifndef mat3_h
#define mat3_h


#include "vec3.h"
#include "vec4.h"
#include "quat.h"


//Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat3 {
	float m[3][3];
} mat3;


void mat3InitZero(mat3 *m);
void mat3InitIdentity(mat3 *m);

void vec3MultiplyMat3(const vec3 *v, const mat3 *m, mat3 *out);
void vec3MultiplyMat3R(const vec3 *v, const mat3 *m, mat3 *out);
void mat3MultiplyVec3(const mat3 *m, const vec3 *v, vec3 *out);
void mat3MultiplyVec3R(const mat3 *m, const vec3 *v, vec3 *out);
void mat3MultiplyMat3(const mat3 *m1, const mat3 *m2, mat3 *out);
void mat3MultiplyMat3R(const mat3 *m1, const mat3 *m2, mat3 *out);

void mat3Transpose(const mat3 *m, mat3 *out);
unsigned char mat3Invert(const mat3 *m, mat3 *out);


#endif