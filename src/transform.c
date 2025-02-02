#include "transform.h"


#include "utilMath.h"


#warning "Check if 'scale' is identity before doing the costly stuff with affine transforms."
#warning "Ideally, we should do this for most of the functions."
#warning "We should also probably check that we aren't trying to invert scales or shears that aren't invertible."


transform g_transformIdentity = {
	.pos.x   = 0.f, .pos.y   = 0.f, .pos.z   = 0.f,
	.rot.x   = 0.f, .rot.y   = 0.f, .rot.z   = 0.f, .rot.w = 1.f,
	#ifdef TRANSFORM_MATRIX_SHEAR
	.scale.m[0][0] = 1.f, .scale.m[0][1] = 0.f, .scale.m[0][2] = 0.f,
	.scale.m[1][0] = 0.f, .scale.m[1][1] = 1.f, .scale.m[1][2] = 0.f,
	.scale.m[2][0] = 0.f, .scale.m[2][1] = 0.f, .scale.m[2][2] = 1.f
	#else
	.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f,
	.shear.x = 0.f, .shear.y = 0.f, .shear.z = 0.f, .shear.w = 1.f
	#endif
};


void transformInit(transform *const restrict trans){
	*trans = g_transformIdentity;
}

transform transformInitC(){
	return(g_transformIdentity);
}


// Append the translation of "trans1" to "trans2", and store the result in "trans1".
void transformAppendPositionP1(transform *const restrict trans1, const transform *const restrict trans2){
	#ifdef TRANSFORM_MATRIX_SHEAR
	vec3 T2;

	// Q_1 S_1 Q_1^T T_2
	mat3MultiplyVec3Out(&trans1->scale, &trans2->pos, &T2);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3Fast(&trans1->rot, &T2);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3AddVec3(&trans1->pos, &T2);
	#else
	vec3 T2;
	quat RQ1;

	// Q_1^T T_2
	quatConjRotateVec3FastOut(&trans1->shear, &trans2->pos, &T2);
	// S_1 Q_1^T T_2
	vec3MultiplyVec3(&T2, &trans1->scale);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatMultiplyQuatOut(trans1->rot, trans1->shear, &RQ1);
	quatRotateVec3Fast(&RQ1, &T2);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3AddVec3(&trans1->pos, &T2);
	#endif
}

// Append the translation of "trans1" to "trans2", and store the result in "trans2".
void transformAppendPositionP2(const transform *const restrict trans1, transform *const restrict trans2){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// Q_1 S_1 Q_1^T T_2
	mat3MultiplyVec3(&trans1->scale, &trans2->pos);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3Fast(&trans1->rot, &trans2->pos);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3AddVec3(&trans2->pos, &trans1->pos);
	#else
	quat RQ1;

	// Q_1^T T_2
	quatConjRotateVec3Fast(&trans1->shear, &trans2->pos);
	// S_1 Q_1^T T_2
	vec3MultiplyVec3(&trans2->pos, &trans1->scale);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatMultiplyQuatOut(trans1->rot, trans1->shear, &RQ1);
	quatRotateVec3Fast(&RQ1, &trans2->pos);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3AddVec3(&trans2->pos, &trans1->pos);
	#endif
}

// Append the translation of "trans1" to "trans2", and store the result in "out".
void transformAppendPositionOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
){

	#ifdef TRANSFORM_MATRIX_SHEAR
	// Q_1 S_1 Q_1^T T_2
	mat3MultiplyVec3Out(&trans1->scale, &trans2->pos, &out->pos);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatRotateVec3Fast(&trans1->rot, &out->pos);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3AddVec3(&out->pos, &trans1->pos);
	#else
	quat RQ1;

	// Q_1^T T_2
	quatConjRotateVec3FastOut(&trans1->shear, &trans2->pos, &out->pos);
	// S_1 Q_1^T T_2
	vec3MultiplyVec3(&out->pos, &trans1->scale);
	// R_1 Q_1 S_1 Q_1^T T_2
	quatMultiplyQuatOut(trans1->rot, trans1->shear, &RQ1);
	quatRotateVec3Fast(&RQ1, &out->pos);
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	vec3AddVec3(&out->pos, &trans1->pos);
	#endif
}

// Append the scale of "trans1" to "trans2", and store the result in "trans1".
void transformAppendScaleP1(transform *const restrict trans1, const transform *const restrict trans2){
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 R2;

	mat3InitQuat(&R2, &trans2->rot);
	// Q_1 S_1 Q_1^T R_2
	mat3MultiplyMat3P1(&trans1->scale, R2);
	// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MultiplyMat3P1(&trans1->scale, trans2->scale);
	// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3TransMultiplyMat3P2(R2, &trans1->scale);
	#else
	quat Q1;
	mat3 QSQT1;
	mat3 QSQT2;

	// Q_1' = R_2^T Q_1
	quatConjMultiplyQuatOut(trans2->rot, trans1->shear, &Q1);
	// Q_1' S_1 Q_1'^T
	mat3InitShearQuat(&QSQT1, &trans1->scale, &Q1);
	// Q_2 S_2 Q_2^T
	mat3InitShearQuat(&QSQT2, &trans2->scale, &trans2->shear);
	// QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		QSQT1.m[0][0]*QSQT2.m[0][0] + QSQT1.m[1][0]*QSQT2.m[0][1] + QSQT1.m[2][0]*QSQT2.m[0][2],
		QSQT1.m[0][0]*QSQT2.m[1][0] + QSQT1.m[1][0]*QSQT2.m[1][1] + QSQT1.m[2][0]*QSQT2.m[1][2],
		QSQT1.m[0][0]*QSQT2.m[2][0] + QSQT1.m[1][0]*QSQT2.m[2][1] + QSQT1.m[2][0]*QSQT2.m[2][2],
		QSQT1.m[0][1]*QSQT2.m[1][0] + QSQT1.m[1][1]*QSQT2.m[1][1] + QSQT1.m[2][1]*QSQT2.m[1][2],
		QSQT1.m[0][1]*QSQT2.m[2][0] + QSQT1.m[1][1]*QSQT2.m[2][1] + QSQT1.m[2][1]*QSQT2.m[2][2],
		QSQT1.m[0][2]*QSQT2.m[2][0] + QSQT1.m[1][2]*QSQT2.m[2][1] + QSQT1.m[2][2]*QSQT2.m[2][2],
		&trans1->scale, &trans1->shear
	);
	#endif
}

// Append the scale of "trans1" to "trans2", and store the result in "trans2".
void transformAppendScaleP2(const transform *const restrict trans1, transform *const restrict trans2){
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 R2;

	mat3InitQuat(&R2, &trans2->rot);
	// Q_1 S_1 Q_1^T R_2
	mat3MultiplyMat3P2(R2, &trans2->scale);
	// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MultiplyMat3P2(trans1->scale, &trans2->scale);
	// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3TransMultiplyMat3P2(R2, &trans2->scale);
	#else
	quat Q1;
	mat3 QSQT1;
	mat3 QSQT2;

	// Q_1' = R_2^T Q_1
	quatConjMultiplyQuatOut(trans2->rot, trans1->shear, &Q1);
	// Q_1' S_1 Q_1'^T
	mat3InitShearQuat(&QSQT1, &trans1->scale, &Q1);
	// Q_2 S_2 Q_2^T
	mat3InitShearQuat(&QSQT2, &trans2->scale, &trans2->shear);
	// QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		QSQT1.m[0][0]*QSQT2.m[0][0] + QSQT1.m[1][0]*QSQT2.m[0][1] + QSQT1.m[2][0]*QSQT2.m[0][2],
		QSQT1.m[0][0]*QSQT2.m[1][0] + QSQT1.m[1][0]*QSQT2.m[1][1] + QSQT1.m[2][0]*QSQT2.m[1][2],
		QSQT1.m[0][0]*QSQT2.m[2][0] + QSQT1.m[1][0]*QSQT2.m[2][1] + QSQT1.m[2][0]*QSQT2.m[2][2],
		QSQT1.m[0][1]*QSQT2.m[1][0] + QSQT1.m[1][1]*QSQT2.m[1][1] + QSQT1.m[2][1]*QSQT2.m[1][2],
		QSQT1.m[0][1]*QSQT2.m[2][0] + QSQT1.m[1][1]*QSQT2.m[2][1] + QSQT1.m[2][1]*QSQT2.m[2][2],
		QSQT1.m[0][2]*QSQT2.m[2][0] + QSQT1.m[1][2]*QSQT2.m[2][1] + QSQT1.m[2][2]*QSQT2.m[2][2],
		&trans2->scale, &trans2->shear
	);
	#endif
}

// Append the scale of "trans1" to "trans2", and store the result in "out".
void transformAppendScaleOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
){

	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 R2;

	mat3InitQuat(&R2, &trans2->rot);
	// Q_1 S_1 Q_1^T R_2
	mat3MultiplyMat3Out(R2, trans2->scale, &out->scale);
	// Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3MultiplyMat3P2(trans1->scale, &out->scale);
	// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
	mat3TransMultiplyMat3P2(R2, &out->scale);
	#else
	quat Q1;
	mat3 QSQT1;
	mat3 QSQT2;

	// Q_1' = R_2^T Q_1
	quatConjMultiplyQuatOut(trans2->rot, trans1->shear, &Q1);
	// Q_1' S_1 Q_1'^T
	mat3InitShearQuat(&QSQT1, &trans1->scale, &Q1);
	// Q_2 S_2 Q_2^T
	mat3InitShearQuat(&QSQT2, &trans2->scale, &trans2->shear);
	// QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		QSQT1.m[0][0]*QSQT2.m[0][0] + QSQT1.m[1][0]*QSQT2.m[0][1] + QSQT1.m[2][0]*QSQT2.m[0][2],
		QSQT1.m[0][0]*QSQT2.m[1][0] + QSQT1.m[1][0]*QSQT2.m[1][1] + QSQT1.m[2][0]*QSQT2.m[1][2],
		QSQT1.m[0][0]*QSQT2.m[2][0] + QSQT1.m[1][0]*QSQT2.m[2][1] + QSQT1.m[2][0]*QSQT2.m[2][2],
		QSQT1.m[0][1]*QSQT2.m[1][0] + QSQT1.m[1][1]*QSQT2.m[1][1] + QSQT1.m[2][1]*QSQT2.m[1][2],
		QSQT1.m[0][1]*QSQT2.m[2][0] + QSQT1.m[1][1]*QSQT2.m[2][1] + QSQT1.m[2][1]*QSQT2.m[2][2],
		QSQT1.m[0][2]*QSQT2.m[2][0] + QSQT1.m[1][2]*QSQT2.m[2][1] + QSQT1.m[2][2]*QSQT2.m[2][2],
		&out->scale, &out->shear
	);
	#endif
}

// Append the rotation of "trans1" to "trans2", and store the result in "trans1".
void transformAppendRotationP1(transform *const restrict trans1, const transform *const restrict trans2){
	// R = R_1 R_2
	quatMultiplyQuatP1(&trans1->rot, trans2->rot);
	quatNormalizeQuat(&trans1->rot);
}

// Append the rotation of "trans1" to "trans2", and store the result in "trans2".
void transformAppendRotationP2(const transform *const restrict trans1, transform *const restrict trans2){
	// R = R_1 R_2
	quatMultiplyQuatP2(trans1->rot, &trans2->rot);
	quatNormalizeQuat(&trans2->rot);
}

// Append the rotation of "trans1" to "trans2", and store the result in "out".
void transformAppendRotationOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
){

	// R = R_1 R_2
	quatMultiplyQuatOut(trans1->rot, trans2->rot, &out->rot);
	quatNormalizeQuat(&out->rot);
}


/*
** Append "trans1" to "trans2" and store the result in "trans1".
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
** Here, "trans2" can be anything (including "trans1").
*/
void transformMultiplyP1(transform *const restrict trans1, const transform *const restrict trans2){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_1 * A_2 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_2 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_1 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	// The position relies on the original scale and rotation, so we do it first.
	transformAppendPositionP1(trans1, trans2);
	// The scale relies on the original rotation, so it comes next.
	transformAppendScaleP1(trans1, trans2);
	// The rotation only requires the original rotation, so it can be done last.
	transformAppendRotationP1(trans1, trans2);
}

/*
** Append "trans1" to "trans2" and store the result in "trans2".
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
** This assumes that "trans1" is not "trans2".
*/
void transformMultiplyP2(const transform *const restrict trans1, transform *const restrict trans2){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_1 * A_2 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_2 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_1 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	// The position relies on the original scale and rotation, so we do it first.
	transformAppendPositionP2(trans1, trans2);
	// The scale relies on the original rotation, so it comes next.
	transformAppendScaleP2(trans1, trans2);
	// The rotation only requires the original rotation, so it can be done last.
	transformAppendRotationP2(trans1, trans2);
}

/*
** Append "trans1" to "trans2" and store the result in "out".
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
** This assumes that "out" is neither "trans1" nor "trans2".
*/
void transformMultiplyOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
){

	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_1 * A_2 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_2 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_1 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	// The position relies on the original scale and rotation, so we do it first.
	transformAppendPositionOut(trans1, trans2, out);
	// The scale relies on the original rotation, so it comes next.
	transformAppendScaleOut(trans1, trans2, out);
	// The rotation only requires the original rotation, so it can be done last.
	transformAppendRotationOut(trans1, trans2, out);
}

/*
** Append "trans1" to "trans2" and return the result.
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
*/
transform transformMultiplyC(const transform trans1, const transform trans2){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_1 * A_2 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_2 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_1 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	transform out;

	// Compute the new position!
	// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
	#ifdef TRANSFORM_MATRIX_SHEAR
	out.pos = vec3AddVec3C(
		trans1.pos,
		quatRotateVec3FastC(
			trans1.rot,
			mat3MultiplyVec3C(trans1.scale, trans2.pos)
		)
	);
	#else
	out.pos = vec3AddVec3C(
		trans1.pos,
		quatRotateVec3FastC(
			quatMultiplyQuatC(trans1.rot, trans1.shear),
			vec3MultiplyVec3C(
				trans1.scale,
				quatConjRotateVec3FastC(trans1.shear, trans2.pos)
			)
		)
	);
	#endif

	// Compute the new orientation!
	// R = R_1 R_2
	out.rot = quatNormalizeQuatC(quatMultiplyQuatC(trans1.rot, trans2.rot));

	// Compute the new scale!
	#ifdef TRANSFORM_MATRIX_SHEAR
	{
		const mat3 R2 = mat3InitQuatC(trans2.rot);
		// QSQ^T = R_2^T Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T
		out.scale = mat3MultiplyMat3C(
			mat3TransposeC(R2),
			mat3MultiplyMat3C(
				trans1.scale,
				mat3MultiplyMat3C(R2, trans2.scale)
			)
		);
	}
	#else
	{
		// Q_1' S_1 Q_1'^T
		const mat3 QSQT1 = mat3InitShearQuatC(trans1.scale, quatConjMultiplyQuatC(trans2.rot, trans1.shear));
		// Q_2 S_2 Q_2^T
		const mat3 QSQT2 = mat3InitShearQuatC(trans2.scale, trans2.shear);
		// QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T
		// The shear matrices are symmetric, so we
		// do the multiplication here to save time.
		mat3DiagonalizeSymmetric(
			QSQT1.m[0][0]*QSQT2.m[0][0] + QSQT1.m[1][0]*QSQT2.m[0][1] + QSQT1.m[2][0]*QSQT2.m[0][2],
			QSQT1.m[0][0]*QSQT2.m[1][0] + QSQT1.m[1][0]*QSQT2.m[1][1] + QSQT1.m[2][0]*QSQT2.m[1][2],
			QSQT1.m[0][0]*QSQT2.m[2][0] + QSQT1.m[1][0]*QSQT2.m[2][1] + QSQT1.m[2][0]*QSQT2.m[2][2],
			QSQT1.m[0][1]*QSQT2.m[1][0] + QSQT1.m[1][1]*QSQT2.m[1][1] + QSQT1.m[2][1]*QSQT2.m[1][2],
			QSQT1.m[0][1]*QSQT2.m[2][0] + QSQT1.m[1][1]*QSQT2.m[2][1] + QSQT1.m[2][1]*QSQT2.m[2][2],
			QSQT1.m[0][2]*QSQT2.m[2][0] + QSQT1.m[1][2]*QSQT2.m[2][1] + QSQT1.m[2][2]*QSQT2.m[2][2],
			&out.scale, &out.shear
		);
	}
	#endif

	return(out);
}


void transformInvert(transform *const restrict trans){
	/*
	** In terms of matrices, we may define our transformation by
	**     A = TRQSQ^T,
	** Then the inverse of A, A^{-1}, is given by
	**     A^{-1} = (TRQSQ^T)^{-1} = -QS^{-1}Q^T R^T T.
	** Therefore, if A^{-1} = T'R'Q'S'(Q')^T, then
	**     T'           = -QS^{-1}Q^T R^T T,
	**     R'Q'S'(Q')^T = QS^{-1}Q^T R^T.
	** Playing a similar trick to before by letting Q'' = RQ,
	**     R'Q'S'(Q')^T = R^T Q''S^{-1}(Q'')^T,
	**     {R' = R^T,
	**  => {Q' = RQ,
	**     {S' = S^{-1}.
	*/


	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 R;
	mat3 QSQTInverse;

	// Compute the new scale!
	mat3InitQuat(&R, &trans->rot);
	// QS^{-1}Q^T
	mat3InvertOut(trans->scale, &QSQTInverse);
	// RQS^{-1}Q^T
	mat3MultiplyMat3Out(R, QSQTInverse, &trans->scale);
	// Q' = RQS^{-1}Q^T R^T
	mat3MultiplyMat3TransP1(&trans->scale, R);
	#else
	// Temporarily store the original stretch rotation.
	const quat Q = trans->shear;

	// Compute the new scale!
	// S' = S^{-1}
	vec3DivideSByFast(&trans->scale, 1.f);
	// Q' = RQ
	quatMultiplyQuatP2(trans->rot, &trans->shear);
	#endif

	// Compute the new orientation!
	// R' = R^T
	quatConjugate(&trans->rot);

	// Compute the new position!
	#ifdef TRANSFORM_MATRIX_SHEAR
	// R^T T
	quatRotateVec3Fast(&trans->rot, &trans->pos);
	// QS^{-1}Q^T R^T T
	mat3MultiplyVec3(&QSQTInverse, &trans->pos);
	// T' = -QS^{-1}Q^T R^T T
	vec3Negate(&trans->pos);
	#else
	// Q^T R^T T
	quatConjRotateVec3Fast(&trans->shear, &trans->pos);
	// S^{-1}Q^T R^T T
	vec3MultiplyVec3(&trans->pos, &trans->scale);
	// QS^{-1}Q^T R^T T
	quatRotateVec3Fast(&Q, &trans->pos);
	// T' = -QS^{-1}Q^T R^T T
	vec3Negate(&trans->pos);
	#endif
}

// Invert all three components of a transformation state.
void transformInvertOut(const transform *const restrict trans, transform *const restrict out){
	/*
	** In terms of matrices, we may define our transformation by
	**     A = TRQSQ^T,
	** Then the inverse of A, A^{-1}, is given by
	**     A^{-1} = (TRQSQ^T)^{-1} = -QS^{-1}Q^T R^T T.
	** Therefore, if A^{-1} = T'R'Q'S'(Q')^T, then
	**     T'           = -QS^{-1}Q^T R^T T,
	**     R'Q'S'(Q')^T = QS^{-1}Q^T R^T.
	** Playing a similar trick to before by letting Q'' = RQ,
	**     R'Q'S'(Q')^T = R^T Q''S^{-1}(Q'')^T,
	**     {R' = R^T,
	**  => {Q' = RQ,
	**     {S' = S^{-1}.
	*/

	// Compute the new scale!
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3 R;
	mat3 QSQTInverse;

	mat3InitQuat(&R, &trans->rot);
	// QS^{-1}Q^T
	mat3InvertOut(trans->scale, &QSQTInverse);
	// RQS^{-1}Q^T
	mat3MultiplyMat3Out(R, QSQTInverse, &out->scale);
	// Q' = RQS^{-1}Q^T R^T
	mat3MultiplyMat3TransP1(&out->scale, R);
	#else
	// S' = S^{-1}
	vec3DivideSByFastOut(&trans->scale, 1.f, &out->scale);
	// Q' = RQ
	quatMultiplyQuatOut(trans->rot, trans->shear, &out->shear);
	#endif

	// Compute the new orientation!
	// R' = R^T
	quatConjugateOut(&trans->rot, &out->rot);

	// Compute the new position!
	#ifdef TRANSFORM_MATRIX_SHEAR
	// R^T T
	quatRotateVec3FastOut(&out->rot, &trans->pos, &out->pos);
	// QS^{-1}Q^T R^T T
	mat3MultiplyVec3(&QSQTInverse, &out->pos);
	// T' = -QS^{-1}Q^T R^T T
	vec3Negate(&out->pos);
	#else
	// Q^T R^T T
	quatConjRotateVec3FastOut(&out->shear, &trans->pos, &out->pos);
	// S^{-1}Q^T R^T T
	vec3MultiplyVec3(&out->pos, &out->scale);
	// QS^{-1}Q^T R^T T
	quatRotateVec3Fast(&trans->shear, &out->pos);
	// T' = -QS^{-1}Q^T R^T T
	vec3Negate(&out->pos);
	#endif
}

transform transformInvertC(const transform trans){
	/*
	** In terms of matrices, we may define our transformation by
	**     A = TRQSQ^T,
	** Then the inverse of A, A^{-1}, is given by
	**     A^{-1} = (TRQSQ^T)^{-1} = -QS^{-1}Q^T R^T T.
	** Therefore, if A^{-1} = T'R'Q'S'(Q')^T, then
	**     T'           = -QS^{-1}Q^T R^T T,
	**     R'Q'S'(Q')^T = QS^{-1}Q^T R^T.
	** Playing a similar trick to before by letting Q'' = RQ,
	**     R'Q'S'(Q')^T = R^T Q''S^{-1}(Q'')^T,
	**     {R' = R^T,
	**  => {Q' = RQ,
	**     {S' = S^{-1}.
	*/

	transform out;

	// Compute the new scale!
	#ifdef TRANSFORM_MATRIX_SHEAR
	const mat3 R = mat3InitQuatC(trans.rot);
	const mat3 QSQTInverse = mat3InvertC(trans.scale);

	// Q' = RQS^{-1}Q^T R^T
	out.scale = mat3MultiplyMat3TransC(
		mat3MultiplyMat3C(QSQTInverse, R),
		R
	);
	#else
	// S' = S^{-1}
	out.scale = vec3DivideSByFastC(trans.scale, 1.f);
	// Q' = RQ
	out.shear = quatMultiplyQuatC(trans.rot, trans.shear);
	#endif

	// Compute the new orientation!
	// R' = R^T
	out.rot = quatConjugateC(trans.rot);

	// Compute the new position!
	#ifdef TRANSFORM_MATRIX_SHEAR
	// T' = -QS^{-1}Q^T R^T T
	out.pos = vec3NegateC(
		mat3MultiplyVec3C(
			QSQTInverse,
			quatRotateVec3FastC(out.rot, trans.pos)
		)
	);
	#else
	// T' = -QS^{-1}Q^T R^T T
	out.pos = vec3NegateC(
		quatRotateVec3FastC(
			trans.shear,
			vec3MultiplyVec3C(
				out.scale,
				quatConjRotateVec3FastC(out.shear, trans.pos)
			)
		)
	);
	#endif

	return(out);
}


// Interpolate from "trans1" to "trans2" and store the result in "out"!
void transformInterpSet(
	const transform *const trans1, const transform *const trans2,
	const float time, transform *const out
){

	#ifdef TRANSFORM_MATRIX_SHEAR
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	mat3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
	#else
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	vec3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
	quatSlerpFasterOut(&trans1->shear, &trans2->shear, time, &out->shear);
	#endif
}

// Interpolate from "trans1" to "trans2" and return the result!
transform transformInterpSetC(const transform trans1, const transform trans2, const float time){
	#ifdef TRANSFORM_MATRIX_SHEAR
	transform out = {
		.pos   = vec3LerpC(trans1.pos, trans2.pos, time),
		.rot   = quatSlerpFasterC(trans1.rot, trans2.rot, time),
		.scale = mat3LerpC(trans1.scale, trans2.scale, time)
	};
	#else
	const transform out = {
		.pos   = vec3LerpC(trans1.pos, trans2.pos, time),
		.rot   = quatSlerpFasterC(trans1.rot, trans2.rot, time),
		.scale = vec3LerpC(trans1.scale, trans2.scale, time),
		.shear = quatSlerpFasterC(trans1.shear, trans2.shear, time)
	};
	#endif

	return(out);
}

// Interpolate between two states and add the offsets to "out"!
void transformInterpAdd(
	const transform *const trans1, const transform *const trans2,
	const float time, transform *const out
){

	/*transformRigid interp;
	// Interpolate from "trans1" to "trans2".
	transformRigidInterpSet(trans1, trans2, time, &interp);

	// Add the interpolated transformation to "out".
	vec3AddVec3(&out->pos, &interp.pos);
	quatMultiplyQuatP1(&out->rot, interp.rot);
	quatNormalizeQuat(&out->rot);
	vec3MultiplyVec3(&out->scale, &interp.scale);*/
}

// Interpolate between two states and add the offsets to "out"!
transform transformInterpAddC(const transform trans1, const transform trans2, const float time){
	return(g_transformIdentity);
}


/*
** Convert a transformation state to a 3x3 matrix.
** This will ignore the translation component!
*/
void transformToMat3(const transform *const restrict trans, mat3 *const restrict out){
	#ifdef TRANSFORM_MATRIX_SHEAR
	*out = trans->scale;
	mat3RotateByQuat(out, &trans->rot);
	#else
	mat3InitShearQuat(out, &trans->scale, &trans->shear);
	mat3RotateByQuat(out, &trans->rot);
	#endif
}

/*
** Convert a transformation state to a 3x3 matrix.
** This will ignore the translation component!
*/
mat3 transformToMat3C(const transform trans){
	#ifdef TRANSFORM_MATRIX_SHEAR
	return(mat3RotateByQuatC(trans.scale, trans.rot));
	#else
	return(mat3RotateByQuatC(mat3InitShearQuatC(trans.scale, trans.shear), trans.rot));
	#endif
}

// Convert a transformation state to a 3x4 transformation matrix.
void transformToMat3x4(const transform *const restrict trans, mat3x4 *const restrict out){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat3x4InitQuat(out, &trans->rot);
	mat3x4MultiplyMat3(out, trans->scale);
	mat3x4TranslateVec3(out, &trans->pos);
	#else
	mat3x4InitShearQuat(out, &trans->scale, &trans->shear);
	mat3x4RotateByQuat(out, &trans->rot);
	mat3x4TranslateVec3(out, &trans->pos);
	#endif
}

// Convert a transformation state to a 3x4 transformation matrix.
mat3x4 transformToMat3x4C(const transform trans){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	#ifdef TRANSFORM_MATRIX_SHEAR
	return(mat3x4TranslateVec3C(
		mat3x4MultiplyMat3C(
			mat3x4InitQuatC(trans.rot),
			trans.scale
		),
		trans.pos
	));
	#else
	return(mat3x4TranslateVec3C(
		mat3x4RotateByQuatC(
			mat3x4InitShearQuatC(trans.scale, trans.shear),
			trans.rot
		),
		trans.pos
	));
	#endif
}

// Convert a transformation state to a 4x4 transformation matrix.
void transformToMat4(const transform *const restrict trans, mat4 *const restrict out){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	#ifdef TRANSFORM_MATRIX_SHEAR
	mat4InitQuat(out, &trans->rot);
	mat4MultiplyMat3(out, trans->scale);
	mat4TranslateVec3(out, &trans->pos);
	#else
	mat4InitShearQuat(out, &trans->scale, &trans->shear);
	mat4RotateByQuat(out, &trans->rot);
	mat4TranslateTransformVec3(out, &trans->pos);
	#endif
}

// Convert a transformation state to a 4x4 transformation matrix.
mat4 transformToMat4C(const transform trans){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	#ifdef TRANSFORM_MATRIX_SHEAR
	return(mat4TranslateVec3C(
		mat4MultiplyMat3C(
			mat4InitQuatC(trans.rot),
			trans.scale
		),
		trans.pos
	));
	#else
	return(mat4TranslateTransformVec3C(
		mat4RotateByQuatC(
			mat4InitShearQuatC(trans.scale, trans.shear),
			trans.rot
		),
		trans.pos
	));
	#endif
}

// Convert a 3x3 transformation matrix to a transformation state.
void mat3ToTransform(const mat3 *const restrict A, transform *const restrict out){
	mat3 rot;

	#ifdef TRANSFORM_MATRIX_SHEAR
	// A 3x3 matrix has no translation component.
	vec3InitZero(&out->pos);
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(*A, &rot);
	mat3ToQuat(&rot, &out->rot);
	// The shear is then given by R^T A.
	mat3TransMultiplyMat3Out(rot, *A, &out->scale);
	#else
	// A 3x3 matrix has no translation component.
	vec3InitZero(&out->pos);
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(*A, &rot);
	mat3ToQuat(&rot, &out->rot);
	// The shear is then given by R^T A.
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		rot.m[0][0]*A->m[0][0] + rot.m[0][1]*A->m[0][1] + rot.m[0][2]*A->m[0][2],
		rot.m[0][0]*A->m[0][0] + rot.m[0][1]*A->m[1][1] + rot.m[0][2]*A->m[1][2],
		rot.m[0][0]*A->m[0][0] + rot.m[0][1]*A->m[2][1] + rot.m[0][2]*A->m[2][2],
		rot.m[1][0]*A->m[0][0] + rot.m[1][1]*A->m[1][1] + rot.m[1][2]*A->m[1][2],
		rot.m[1][0]*A->m[0][0] + rot.m[1][1]*A->m[2][1] + rot.m[1][2]*A->m[2][2],
		rot.m[2][0]*A->m[0][0] + rot.m[2][1]*A->m[2][1] + rot.m[2][2]*A->m[2][2],
		&out->scale, &out->shear
	);
	#endif
}

// Convert a 3x3 transformation matrix to a transformation state.
transform mat3ToTransformC(const mat3 A){
	mat3 rot;
	transform out;

	#ifdef TRANSFORM_MATRIX_SHEAR
	// A 3x3 matrix has no translation component.
	out.pos = vec3InitZeroC();
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(A, &rot);
	out.rot = mat3ToQuatC(rot);
	// The shear is then given by R^T A.
	out.scale = mat3TransMultiplyMat3C(rot, A);
	#else
	// A 3x3 matrix has no translation component.
	out.pos = vec3InitZeroC();
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(A, &rot);
	out.rot = mat3ToQuatC(rot);
	// The shear is then given by R^T A.
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		rot.m[0][0]*A.m[0][0] + rot.m[0][1]*A.m[0][1] + rot.m[0][2]*A.m[0][2],
		rot.m[0][0]*A.m[0][0] + rot.m[0][1]*A.m[1][1] + rot.m[0][2]*A.m[1][2],
		rot.m[0][0]*A.m[0][0] + rot.m[0][1]*A.m[2][1] + rot.m[0][2]*A.m[2][2],
		rot.m[1][0]*A.m[0][0] + rot.m[1][1]*A.m[1][1] + rot.m[1][2]*A.m[1][2],
		rot.m[1][0]*A.m[0][0] + rot.m[1][1]*A.m[2][1] + rot.m[1][2]*A.m[2][2],
		rot.m[2][0]*A.m[0][0] + rot.m[2][1]*A.m[2][1] + rot.m[2][2]*A.m[2][2],
		&out.scale, &out.shear
	);
	#endif

	return(out);
}

// Convert a 3x4 transformation matrix to a transformation state.
void mat3x4ToTransform(const mat3x4 *const restrict A, transform *const restrict out){
	mat3 A3;
	mat3 rot;

	#ifdef TRANSFORM_MATRIX_SHEAR
	// Extract the translation from the total transformation.
	out->pos = *((const vec3 *)A->m[3]);
	// Extract the rotation from the total transformation.
	mat3InitMat3x4(&A3, A);
	mat3PolarDecompose(A3, &rot);
	mat3ToQuat(&rot, &out->rot);
	// The shear is then given by R^T A.
	mat3TransMultiplyMat3Out(rot, A3, &out->scale);
	#else
	// Extract the translation from the total transformation.
	out->pos = *((const vec3 *)A->m[3]);
	// Extract the rotation from the total transformation.
	mat3InitMat3x4(&A3, A);
	mat3PolarDecompose(A3, &rot);
	mat3ToQuat(&rot, &out->rot);
	// The shear is then given by R^T A.
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[0][1] + rot.m[0][2]*A3.m[0][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[1][1] + rot.m[0][2]*A3.m[1][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[2][1] + rot.m[0][2]*A3.m[2][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[1][1] + rot.m[1][2]*A3.m[1][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[2][1] + rot.m[1][2]*A3.m[2][2],
		rot.m[2][0]*A3.m[0][0] + rot.m[2][1]*A3.m[2][1] + rot.m[2][2]*A3.m[2][2],
		&out->scale, &out->shear
	);
	#endif
}

// Convert a 3x4 transformation matrix to a transformation state.
transform mat3x4ToTransformC(const mat3x4 A){
	const mat3 A3 = mat3InitMat3x4C(A);
	mat3 rot;
	transform out;

	#ifdef TRANSFORM_MATRIX_SHEAR
	// Extract the translation from the total transformation.
	out.pos = *((const vec3 *)A.m[3]);
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(A3, &rot);
	out.rot = mat3ToQuatC(rot);
	// The shear is then given by R^T A.
	out.scale = mat3TransMultiplyMat3C(rot, A3);
	#else
	// Extract the translation from the total transformation.
	out.pos = *((const vec3 *)A.m[3]);
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(A3, &rot);
	out.rot = mat3ToQuatC(rot);
	// The shear is then given by R^T A.
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[0][1] + rot.m[0][2]*A3.m[0][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[1][1] + rot.m[0][2]*A3.m[1][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[2][1] + rot.m[0][2]*A3.m[2][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[1][1] + rot.m[1][2]*A3.m[1][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[2][1] + rot.m[1][2]*A3.m[2][2],
		rot.m[2][0]*A3.m[0][0] + rot.m[2][1]*A3.m[2][1] + rot.m[2][2]*A3.m[2][2],
		&out.scale, &out.shear
	);
	#endif

	return(out);
}

// Convert a 4x4 transformation matrix to a transformation state.
void mat4ToTransform(const mat4 *const restrict A, transform *const restrict out){
	mat3 A3;
	mat3 rot;

	#ifdef TRANSFORM_MATRIX_SHEAR
	// Extract the translation from the total transformation.
	out->pos = *((const vec3 *)A->m[3]);
	// Extract the rotation from the total transformation.
	mat3InitMat4(&A3, A);
	mat3PolarDecompose(A3, &rot);
	mat3ToQuat(&rot, &out->rot);
	// The shear is then given by R^T A.
	mat3TransMultiplyMat3Out(rot, A3, &out->scale);
	#else
	// Extract the translation from the total transformation.
	out->pos = *((const vec3 *)A->m[3]);
	// Extract the rotation from the total transformation.
	mat3InitMat4(&A3, A);
	mat3PolarDecompose(A3, &rot);
	mat3ToQuat(&rot, &out->rot);
	// The shear is then given by R^T A.
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[0][1] + rot.m[0][2]*A3.m[0][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[1][1] + rot.m[0][2]*A3.m[1][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[2][1] + rot.m[0][2]*A3.m[2][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[1][1] + rot.m[1][2]*A3.m[1][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[2][1] + rot.m[1][2]*A3.m[2][2],
		rot.m[2][0]*A3.m[0][0] + rot.m[2][1]*A3.m[2][1] + rot.m[2][2]*A3.m[2][2],
		&out->scale, &out->shear
	);
	#endif
}

// Convert a 4x4 transformation matrix to a transformation state.
transform mat4ToTransformC(const mat4 A){
	const mat3 A3 = mat3InitMat4C(A);
	mat3 rot;
	transform out;

	#ifdef TRANSFORM_MATRIX_SHEAR
	// Extract the translation from the total transformation.
	out.pos = *((const vec3 *)A.m[3]);
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(A3, &rot);
	out.rot = mat3ToQuatC(rot);
	// The shear is then given by R^T A.
	out.scale = mat3TransMultiplyMat3C(rot, A3);
	#else
	// Extract the translation from the total transformation.
	out.pos = *((const vec3 *)A.m[3]);
	// Extract the rotation from the total transformation.
	mat3PolarDecompose(A3, &rot);
	out.rot = mat3ToQuatC(rot);
	// The shear is then given by R^T A.
	// The shear matrices are symmetric, so we
	// do the multiplication here to save time.
	mat3DiagonalizeSymmetric(
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[0][1] + rot.m[0][2]*A3.m[0][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[1][1] + rot.m[0][2]*A3.m[1][2],
		rot.m[0][0]*A3.m[0][0] + rot.m[0][1]*A3.m[2][1] + rot.m[0][2]*A3.m[2][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[1][1] + rot.m[1][2]*A3.m[1][2],
		rot.m[1][0]*A3.m[0][0] + rot.m[1][1]*A3.m[2][1] + rot.m[1][2]*A3.m[2][2],
		rot.m[2][0]*A3.m[0][0] + rot.m[2][1]*A3.m[2][1] + rot.m[2][2]*A3.m[2][2],
		&out.scale, &out.shear
	);
	#endif

	return(out);
}


// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformPoint(const transform *const restrict trans, vec3 *const restrict v){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MultiplyVec3(&trans->scale, v);
	// (RQSQ^T)v
	quatRotateVec3Fast(&trans->rot, v);
	// v' = Av = (TRQSQ^T)v
	vec3AddVec3(v, &trans->pos);
	#else
	quat rot;

	// (Q^T)v
	quatConjRotateVec3Fast(&trans->shear, v);
	// (SQ^T)v
	vec3MultiplyVec3(v, &trans->scale);
	// (RQSQ^T)v
	quatMultiplyQuatOut(trans->rot, trans->shear, &rot);
	quatRotateVec3Fast(&rot, v);
	// v' = Av = (TRQSQ^T)v
	vec3AddVec3(v, &trans->pos);
	#endif
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformPointOut(const transform *const restrict trans, const vec3 *const v, vec3 *const out){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MultiplyVec3Out(&trans->scale, v, out);
	// (RQSQ^T)v
	quatRotateVec3Fast(&trans->rot, out);
	// v' = Av = (TRQSQ^T)v
	vec3AddVec3(out, &trans->pos);
	#else
	quat rot;

	// (Q^T)v
	quatConjRotateVec3FastOut(&trans->shear, v, out);
	// (SQ^T)v
	vec3MultiplyVec3(out, &trans->scale);
	// (RQSQ^T)v
	quatMultiplyQuatOut(trans->rot, trans->shear, &rot);
	quatRotateVec3Fast(&rot, out);
	// v' = Av = (TRQSQ^T)v
	vec3AddVec3(out, &trans->pos);
	#endif
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
vec3 transformPointC(const transform trans, const vec3 v){
	// v' = Av = (TRQSQ^T)v
	#ifdef TRANSFORM_MATRIX_SHEAR
	return(vec3AddVec3C(
		trans.pos,
		quatRotateVec3FastC(
			trans.rot,
			mat3MultiplyVec3C(trans.scale, v)
		)
	));
	#else
	return(vec3AddVec3C(
		trans.pos,
		quatRotateVec3FastC(
			quatMultiplyQuatC(trans.rot, trans.shear),
			vec3MultiplyVec3C(
				trans.scale,
				quatConjRotateVec3FastC(trans.shear, v)
			)
		)
	));
	#endif
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformDirection(const transform *const restrict trans, vec3 *const restrict v){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MultiplyVec3(&trans->scale, v);
	// v' = (RQSQ^T)v
	quatRotateVec3Fast(&trans->rot, v);
	#else
	quat rot;

	// (Q^T)v
	quatConjRotateVec3Fast(&trans->shear, v);
	// (SQ^T)v
	vec3MultiplyVec3(v, &trans->scale);
	// v' = (RQSQ^T)v
	quatMultiplyQuatOut(trans->rot, trans->shear, &rot);
	quatRotateVec3Fast(&rot, v);
	#endif
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformDirectionOut(const transform *const restrict trans, const vec3 *const v, vec3 *const out){
	#ifdef TRANSFORM_MATRIX_SHEAR
	// (QSQ^T)v
	mat3MultiplyVec3Out(&trans->scale, v, out);
	// v' = (RQSQ^T)v
	quatRotateVec3Fast(&trans->rot, out);
	#else
	quat rot;

	// (Q^T)v
	quatConjRotateVec3FastOut(&trans->shear, v, out);
	// (SQ^T)v
	vec3MultiplyVec3(out, &trans->scale);
	// v' = (RQSQ^T)v
	quatMultiplyQuatOut(trans->rot, trans->shear, &rot);
	quatRotateVec3Fast(&rot, out);
	#endif
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
vec3 transformDirectionC(const transform trans, const vec3 v){
	// v' = (RQSQ^T)v
	#ifdef TRANSFORM_MATRIX_SHEAR
	return(quatRotateVec3FastC(
		trans.rot,
		mat3MultiplyVec3C(trans.scale, v)
	));
	#else
	return(quatRotateVec3FastC(
		quatMultiplyQuatC(trans.rot, trans.shear),
		vec3MultiplyVec3C(
			trans.scale,
			quatConjRotateVec3FastC(trans.shear, v)
		)
	));
	#endif
}