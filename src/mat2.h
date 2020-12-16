#ifndef mat2_h
#define mat2_h


#include "utilTypes.h"

#include "vec2.h"


// Note: Our matrices are column-major, just the way OpenGL likes them.
typedef struct mat2 {
	float m[2][2];
} mat2;


void mat2InitZero(mat2 *const restrict m);
mat2 mat2InitZeroC();
void mat2InitIdentity(mat2 *const restrict m);
mat2 mat2InitIdentityC();

void mat2AddMat2(mat2 *const restrict m1, const mat2 *const restrict m2);
void mat2AddMat2Out(const mat2 *const restrict m1, const mat2 *const restrict m2, mat2 *const restrict out);
mat2 mat2AddMat2C(const mat2 m1, const mat2 m2);

void mat2MultiplyByVec2(const mat2 *const restrict m, vec2 *const restrict v);
void mat2MultiplyByVec2Out(const mat2 *const restrict m, const vec2 *const restrict v, vec2 *const restrict out);
vec2 mat2MultiplyByVec2C(const mat2 m, const vec2 v);
void mat2MultiplyVec2By(mat2 *const restrict m, const vec2 *const restrict v);
void mat2MultiplyVec2ByOut(const mat2 m, const vec2 *const restrict v, mat2 *const restrict out);
mat2 mat2MultiplyVec2ByC(const mat2 m, const vec2 v);
void mat2MultiplyByMat2(mat2 *const restrict m1, const mat2 m2);
void mat2MultiplyMat2By(mat2 *const restrict m1, const mat2 m2);
void mat2MultiplyByMat2Out(const mat2 m1, const mat2 m2, mat2 *const restrict out);
mat2 mat2MultiplyByMat2C(const mat2 m1, const mat2 m2);

void mat2Transpose(mat2 *const restrict m);
void mat2TransposeOut(const mat2 m, mat2 *const restrict out);
mat2 mat2TransposeC(const mat2 m);
float mat2Determinant(const mat2 *const restrict m);
float mat2DeterminantC(const mat2 m);
void mat2Invert(mat2 *const restrict m);
void mat2InvertOut(const mat2 m, mat2 *const restrict out);
mat2 mat2InvertC(const mat2 m);
return_t mat2CanInvert(mat2 *const restrict m);
return_t mat2CanInvertOut(const mat2 m, mat2 *const restrict out);

void mat2Solve(const mat2 *const restrict A, const vec2 *const restrict b, vec2 *const restrict x);
vec2 mat2SolveC(const mat2 A, const vec2 b);
return_t mat2CanSolve(const mat2 *const restrict A, const vec2 *const restrict b, vec2 *const restrict x);
return_t mat2CanSolveC(const mat2 A, const vec2 b, vec2 *const restrict x);


extern mat2 g_g_mat2Identity;


#endif