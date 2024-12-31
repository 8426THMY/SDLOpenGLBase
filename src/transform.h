#ifndef transform_h
#define transform_h


#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "mat3x4.h"
#include "mat4.h"


/*
** Rather than storing a shear as a quaternion and a scale vector,
** we can instead just store it as a 3x3 matrix. It turns out that
** interpolating a shear matrix surprisingly looks a lot better
** than interpolating a quaternion-vector pair, and it also has
** the added benefit of eliminating those pesky diagonalizations.
*/
#define TRANSFORM_MATRIX_SHEAR


/*
** To correctly handle non-uniform scaling, we introduce
** a "shear quaternion" that rotates us so that we can
** easily perform our scaling along the x, y and z axes.
**
** Our transformation is equivalent to the matrix
**     A = TRQSQ^T,
** where
**     T = translation,
**     R = rotation,
**     Q = shearing,
**     S = scaling.
** This is usually only necessary for things like skeletons,
** where we may want to add extra flare to their animations.
** However, it cannot be used with most colliders, such as
** spheres and capsules. The one exception to this is hulls.
*/
typedef struct transform {
	vec3 pos;
	quat rot;
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 shear;
	#else
	vec3 scale;
	quat shear;
	#endif
} transform;


void transformInit(transform *const restrict trans);
transform transformInitC();

void transformAppendPositionP1(transform *const restrict trans1, const transform *const restrict trans2);
void transformAppendPositionP2(const transform *const restrict trans1, transform *const restrict trans2);
void transformAppendPositionOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
);
void transformAppendScaleP1(transform *const restrict trans1, const transform *const restrict trans2);
void transformAppendScaleP2(const transform *const restrict trans1, transform *const restrict trans2);
void transformAppendScaleOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
);
void transformAppendRotationP1(transform *const restrict trans1, const transform *const restrict trans2);
void transformAppendRotationP2(const transform *const restrict trans1, transform *const restrict trans2);
void transformAppendRotationOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
);

void transformMultiplyP1(transform *const restrict trans1, const transform *const restrict trans2);
void transformMultiplyP2(const transform *const restrict trans1, transform *const restrict trans2);
void transformMultiplyOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
);
transform transformMultiplyC(const transform trans1, const transform trans2);

void transformInvert(transform *const restrict trans);
void transformInvertOut(const transform *const restrict trans, transform *const restrict out);
transform transformInvertC(const transform trans);

void transformInterpSet(
	const transform *const trans1, const transform *const trans2,
	const float time, transform *const out
);
transform transformInterpSetC(const transform trans1, const transform trans2, const float time);
void transformInterpAdd(
	const transform *const trans1, const transform *const trans2,
	const float time, transform *const out
);
transform transformInterpAddC(const transform trans1, const transform trans2, const float time);

void transformToMat3(const transform *const restrict trans, mat3 *const restrict out);
mat3 transformToMat3C(const transform trans);
void transformToMat3x4(const transform *const restrict trans, mat3x4 *const restrict out);
mat3x4 transformToMat3x4C(const transform trans);
void transformToMat4(const transform *const restrict trans, mat4 *const restrict out);
mat4 transformToMat4C(const transform trans);

void transformPoint(const transform *const restrict trans, vec3 *const restrict v);
void transformPointOut(const transform *const restrict trans, const vec3 *const v, vec3 *const out);
vec3 transformPointC(const transform trans, const vec3 v);
void transformDirection(const transform *const restrict trans, vec3 *const restrict v);
void transformDirectionOut(const transform *const restrict trans, const vec3 *const v, vec3 *const out);
vec3 transformDirectionC(const transform trans, const vec3 v);


extern transform g_transformIdentity;


#endif