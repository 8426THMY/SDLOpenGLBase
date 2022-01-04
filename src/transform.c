#include "transform.h"


#include "utilMath.h"


/**
#error "Check if 'scale' is identity before doing the costly stuff with affine transforms."
#error "Ideally, we should do this for most of the functions."
**/


transform g_transformIdentity = {
	.pos.x   = 0.f, .pos.y   = 0.f, .pos.z   = 0.f,
	.rot.x   = 0.f, .rot.y   = 0.f, .rot.z   = 0.f, .rot.w = 1.f,
	.shear.x = 0.f, .shear.y = 0.f, .shear.z = 0.f, .shear.w = 1.f,
	.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f
};


void transformInit(transform *const restrict trans){
	*trans = g_transformIdentity;
}

transform transformInitC(){
	return(g_transformIdentity);
}


/*
** Append "trans1" to "trans2" and store the result in "trans1".
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
** This assumes that "trans2" is not "trans1".
*/
void transformAppendP1(transform *const restrict trans1, const transform *const restrict trans2){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_2 * A_1 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_1 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_2 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	// Compute the new position!
	{
		quat RQ1;
		vec3 T2;

		// Q_1^T T_2
		quatConjRotateVec3FastOut(&trans1->shear, &trans2->pos, &T2);
		// S_1 Q_1^T T_2
		vec3MultiplyVec3(&T2, &trans1->scale);
		// R_1 Q_1 S_1 Q_1^T T_2
		quatMultiplyQuatOut(trans1->rot, trans1->shear, &RQ1);
		quatRotateVec3Fast(&RQ1, &T2);
		// T = T_1 R_1 Q_1 S_1 Q_1^T T_2
		vec3AddVec3(&trans1->pos, &T2);
	}

	// Compute the new orientation!
	{
		// R = R_1 R_2
		quatMultiplyQuatP1(&trans1->rot, trans2->rot);
		quatNormalizeQuat(&trans1->rot);
	}

	// Compute the new scale!
	{
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
	}
}

/*
** Append "trans1" to "trans2" and store the result in "trans2".
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
** This assumes that "trans1" is not "trans2".
*/
void transformAppendP2(const transform *const restrict trans1, transform *const restrict trans2){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_2 * A_1 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_1 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_2 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	// Compute the new position!
	{
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
	}

	// Compute the new scale!
	{
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
	}

	// Compute the new orientation!
	{
		// R = R_1 R_2
		quatMultiplyQuatP2(trans1->rot, &trans2->rot);
		quatNormalizeQuat(&trans2->rot);
	}
}

/*
** Append "trans1" to "trans2" and store the result in "out".
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
** This assumes that "out" is neither "trans1" nor "trans2".
*/
void transformAppendOut(
	const transform *const restrict trans1, const transform *const restrict trans2,
	transform *const restrict out
){

	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_2 * A_1 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_1 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_2 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_1' = R_2^T Q_1, which is clearly a rotation, and gives
	**     RQSQ^T = R_1 R_2 Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T,
	**     {R     = R_1 R_2,
	**  => {QSQ^T = Q_1' S_1 Q_1'^T Q_2 S_2 Q_2^T.
	** That is, we can write QSQ^T as a lovely symmetric matrix, which
	** can then be diagonalized to solve individually for Q and S!
	*/

	// Compute the new position!
	{
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
	}

	// Compute the new orientation!
	{
		// R = R_1 R_2
		quatMultiplyQuatOut(trans1->rot, trans2->rot, &out->rot);
		quatNormalizeQuat(&out->rot);
	}

	// Compute the new scale!
	{
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
	}
}

/*
** Append "trans1" to "trans2" and return the result.
** This is equivalent to left-multiplying "trans2" by "trans1" as if they're matrices:
**     A = A_1 * A_2.
*/
transform transformAppendC(const transform trans1, const transform trans2){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_2 * A_1 is given similarly,
	**     T      = T_1 R_1 Q_1 S_1 Q_1^T T_2,
	**     RQSQ^T = R_1 Q_1 S_1 Q_1^T R_2 Q_2 S_2 Q_2^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_1 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_2 do not affect
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

	// Compute the new orientation!
	// R = R_1 R_2
	out.rot = quatNormalizeQuatC(quatMultiplyQuatC(trans1.rot, trans2.rot));

	// Compute the new scale!
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

	return(out);
}


// Interpolate from "trans1" to "trans2" and store the result in "out"!
void transformInterpSet(const transform *const trans1, const transform *const trans2, const float time, transform *const out){
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	vec3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
	quatSlerpFasterOut(&trans1->shear, &trans2->shear, time, &out->shear);
}

// Interpolate from "trans1" to "trans2" and return the result!
transform transformInterpSetC(const transform trans1, const transform trans2, const float time){
	const transform out = {
		.pos = vec3LerpC(trans1.pos, trans2.pos, time),
		.rot = quatSlerpFasterC(trans1.rot, trans2.rot, time),
		.scale = vec3LerpC(trans1.scale, trans2.scale, time),
		.shear = quatSlerpFasterC(trans1.shear, trans2.shear, time)
	};
	return(out);
}

// Interpolate between two states and add the offsets to "out"!
void transformInterpAdd(const transform *const trans1, const transform *const trans2, const float time, transform *const out){
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


void transformInvert(transform *const restrict trans){
	/*
	** In terms of matrices, we may define our transformation by
	**     A = TRQSQ^T,
	** Then the inverse of A, A^(-1), is given by
	**     A^(-1) = (TRQSQ^T)^(-1) = -QS^(-1)Q^T R^T T.
	** Therefore, if A^(-1) = T'R'Q'S'(Q')^T, then
	**     T'           = -QS^(-1)Q^T R^T T,
	**     R'Q'S'(Q')^T = QS^(-1)Q^T R^T.
	** Playing a similar trick to before by letting Q'' = RQ,
	**     R'Q'S'(Q')^T = R^T Q''S^(-1)(Q'')^T,
	**     {R' = R^T,
	**  => {Q' = RQ,
	**     {S' = S^(-1).
	*/

	// Temporarily store the original stretch rotation.
	const quat Q = trans->shear;

	// Compute the new scale!
	{
		// S' = S^(-1)
		vec3DivideSByFast(&trans->scale, 1.f);
		// Q' = RQ
		quatMultiplyQuatP2(trans->rot, &trans->shear);
	}

	// Compute the new orientation!
	{
		// R' = R^T
		quatConjugate(&trans->rot);
	}

	// Compute the new position!
	{
		// Q^T R^T T
		quatConjRotateVec3Fast(&trans->shear, &trans->pos);
		// S^(-1)Q^T R^T T
		vec3MultiplyVec3(&trans->pos, &trans->scale);
		// QS^(-1)Q^T R^T T
		quatRotateVec3Fast(&Q, &trans->pos);
		// T' = -QS^(-1)Q^T R^T T
		vec3Negate(&trans->pos);
	}
}

// Invert all three components of a transformation state.
void transformInvertOut(const transform *const restrict trans, transform *const restrict out){
	/*
	** In terms of matrices, we may define our transformation by
	**     A = TRQSQ^T,
	** Then the inverse of A, A^(-1), is given by
	**     A^(-1) = (TRQSQ^T)^(-1) = -QS^(-1)Q^T R^T T.
	** Therefore, if A^(-1) = T'R'Q'S'(Q')^T, then
	**     T'           = -QS^(-1)Q^T R^T T,
	**     R'Q'S'(Q')^T = QS^(-1)Q^T R^T.
	** Playing a similar trick to before by letting Q'' = RQ,
	**     R'Q'S'(Q')^T = R^T Q''S^(-1)(Q'')^T,
	**     {R' = R^T,
	**  => {Q' = RQ,
	**     {S' = S^(-1).
	*/

	// Compute the new scale!
	{
		// S' = S^(-1)
		vec3DivideSByFastOut(&trans->scale, 1.f, &out->scale);
		// Q' = RQ
		quatMultiplyQuatOut(trans->rot, trans->shear, &out->shear);
	}

	// Compute the new orientation!
	{
		// R' = R^T
		quatConjugateOut(&trans->rot, &out->rot);
	}

	// Compute the new position!
	{
		// Q^T R^T T
		quatConjRotateVec3FastOut(&out->shear, &trans->pos, &out->pos);
		// S^(-1)Q^T R^T T
		vec3MultiplyVec3(&out->pos, &out->scale);
		// QS^(-1)Q^T R^T T
		quatRotateVec3Fast(&trans->shear, &out->pos);
		// T' = -QS^(-1)Q^T R^T T
		vec3Negate(&out->pos);
	}
}

transform transformInvertC(const transform trans){
	/*
	** In terms of matrices, we may define our transformation by
	**     A = TRQSQ^T,
	** Then the inverse of A, A^(-1), is given by
	**     A^(-1) = (TRQSQ^T)^(-1) = -QS^(-1)Q^T R^T T.
	** Therefore, if A^(-1) = T'R'Q'S'(Q')^T, then
	**     T'           = -QS^(-1)Q^T R^T T,
	**     R'Q'S'(Q')^T = QS^(-1)Q^T R^T.
	** Playing a similar trick to before by letting Q'' = RQ,
	**     R'Q'S'(Q')^T = R^T Q''S^(-1)(Q'')^T,
	**     {R' = R^T,
	**  => {Q' = RQ,
	**     {S' = S^(-1).
	*/

	transform out;

	// Compute the new scale!
	// S' = S^(-1)
	out.scale = vec3DivideSByFastC(trans.scale, 1.f);
	// Q' = RQ
	out.shear = quatMultiplyQuatC(trans.rot, trans.shear);

	// Compute the new orientation!
	// R' = R^T
	out.rot = quatConjugateC(trans.rot);

	// Compute the new position!
	// T' = -QS^(-1)Q^T R^T T
	out.pos = vec3NegateC(
		quatRotateVec3FastC(
			trans.shear,
			vec3MultiplyVec3C(
				out.scale,
				quatConjRotateVec3FastC(out.shear, trans.pos)
			)
		)
	);

	return(out);
}


// Convert a transformation state to a 4x4 transformation matrix.
void transformToMat4(const transform *const restrict trans, mat4 *const restrict out){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	mat4InitShearQuat(out, &trans->scale, &trans->shear);
	mat4RotateByQuat(out, &trans->rot);
	mat4TranslateTransformVec3(out, &trans->pos);
}

// Convert a transformation state to a 4x4 transformation matrix.
mat4 transformToMat4C(const transform trans){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	return(mat4TranslateTransformVec3C(
		mat4RotateByQuatC(
			mat4InitShearQuatC(trans.scale, trans.shear),
			trans.rot
		),
		trans.pos
	));
}

/*
** Convert a transformation state to a 3x3 matrix.
** This will ignore the translation component!
*/
void transformToMat3(const transform *const restrict trans, mat3 *const restrict out){
	mat3InitShearQuat(out, &trans->scale, &trans->shear);
	mat3RotateByQuat(out, &trans->rot);
}

/*
** Convert a transformation state to a 3x3 matrix.
** This will ignore the translation component!
*/
mat3 transformToMat3C(const transform trans){
	return(mat3RotateByQuatC(mat3InitShearQuatC(trans.scale, trans.shear), trans.rot));
}


// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformTransformPoint(const transform *const restrict trans, vec3 *const restrict v){
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
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformTransformPointOut(const transform *const restrict trans, const vec3 *const v, vec3 *const out){
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
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
vec3 transformTransformPointC(const transform trans, const vec3 v){
	// v' = Av = (TRQSQ^T)v
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
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformTransformDirection(const transform *const restrict trans, vec3 *const restrict v){
	quat rot;

	// (Q^T)v
	quatConjRotateVec3Fast(&trans->shear, v);
	// (SQ^T)v
	vec3MultiplyVec3(v, &trans->scale);
	// v' = (RQSQ^T)v
	quatMultiplyQuatOut(trans->rot, trans->shear, &rot);
	quatRotateVec3Fast(&rot, v);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformTransformDirectionOut(const transform *const restrict trans, const vec3 *const v, vec3 *const out){
	quat rot;

	// (Q^T)v
	quatConjRotateVec3FastOut(&trans->shear, v, out);
	// (SQ^T)v
	vec3MultiplyVec3(out, &trans->scale);
	// v' = (RQSQ^T)v
	quatMultiplyQuatOut(trans->rot, trans->shear, &rot);
	quatRotateVec3Fast(&rot, out);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
vec3 transformTransformDirectionC(const transform trans, const vec3 v){
	// v' = (RQSQ^T)v
	return(quatRotateVec3FastC(
		quatMultiplyQuatC(trans.rot, trans.shear),
		vec3MultiplyVec3C(
			trans.scale,
			quatConjRotateVec3FastC(trans.shear, v)
		)
	));
}