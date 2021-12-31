#include "transform.h"


#include "utilMath.h"


/**
#error "Check if 'stretchRot' is identity before doing the costly stuff with affine transforms."
#error "Ideally, we should do this for most of the functions."
**/


transformRigid g_transformRigidIdentity = {
	.pos.x   = 0.f, .pos.y   = 0.f, .pos.z   = 0.f,
	.rot.x   = 0.f, .rot.y   = 0.f, .rot.z   = 0.f, .rot.w = 1.f,
	.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f
};

transformAffine g_transformAffineIdentity = {
	.pos.x   = 0.f, .pos.y   = 0.f, .pos.z   = 0.f,
	.rot.x   = 0.f, .rot.y   = 0.f, .rot.z   = 0.f, .rot.w = 1.f,
	.stretchRot.x = 0.f, .stretchRot.y = 0.f, .stretchRot.z = 0.f, .stretchRot.w = 1.f,
	.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f
};


void transformRigidInit(transformRigid *const restrict trans){
	*trans = g_transformRigidIdentity;
}

void transformAffineInit(transformAffine *const restrict trans){
	*trans = g_transformAffineIdentity;
}


/*
** Append "trans2" to "trans1". This is equivalent to left-
** multiplying "trans1" by "trans2" as if they're matrices:
**     A = A_2 * A_1.
*/
void transformRigidAppend(const transformRigid *const trans1, const transformRigid *const trans2, transformRigid *const out){
	vec3 pos;

	vec3MultiplyVec3Out(&trans1->pos, &trans2->scale, &pos);
	quatRotateVec3Fast(&trans2->rot, &pos);
	// Generate the new position!
	vec3AddVec3Out(&trans2->pos, &pos, &out->pos);
	// Generate the new orientation!
	quatMultiplyQuatByOut(trans2->rot, trans1->rot, &out->rot);
	// A slight error will build up if we don't normalize the rotation.
	quatNormalizeQuat(&out->rot);
	// Generate the new scale!
	vec3MultiplyVec3Out(&trans2->scale, &trans1->scale, &out->scale);
}

/*
** Append "trans2" to "trans1". This is equivalent to left-
** multiplying "trans1" by "trans2" as if they're matrices:
**     A = A_2 * A_1.
*/
void transformAffineAppend(const transformAffine *const trans1, const transformAffine *const trans2, transformAffine *const out){
	/*
	** In terms of matrices, we may define our transformations by
	**     A_k = T_k R_k Q_k S_k Q_k^T,
	** for k in {1, 2}. Then if A = A_2 * A_1 is given similarly,
	**     T      = T_2 R_2 Q_2 S_2 Q_2^T T_1,
	**     RQSQ^T = R_2 Q_2 S_2 Q_2^T R_1 Q_1 S_1 Q^1^T.
	** Note that although Q_k S_k Q_k^T are symmetric for k in {1, 2}, we
	** have inserted R_1 between them, which in general breaks symmetry.
	** However, we also note that post-rotations such as R_2 do not affect
	** the scaling, and hence can be ignored. This leads to the definition
	** Q_2' = R_1^T Q_2, which is clearly a rotation, and gives
	**     RQSQ^T = R_2 R_1 Q_2' S_2 Q_2'^T Q_1 S_1 Q^1^T,
	**     {R     = R_2 R_1,
	**  => {QSQ^T = Q_2' S_2 Q_2'^T Q_1 S_1 Q^1^T.
	** We can therefore write it as a lovely symmetric matrix!
	*/

	// Temporarily store the original orientation.
	const quat rot1 = trans1->rot;

	// Compute the new position!
	{
		quat rot;
		vec3 pos;

		// Q_2^T T_1
		quatRotateVec3ConjFastOut(&trans2->stretchRot, &trans1->pos, &pos);
		// S Q_2^T T_1
		vec3MultiplyVec3Out(&pos, &trans2->scale, &pos);
		// R_2 Q_2 S_2 Q_2^T T_1
		quatMultiplyQuatByOut(trans2->rot, trans2->stretchRot, &rot);
		quatRotateVec3Fast(&rot, &pos);
		// T = T_2 R_2 Q_2 S_2 Q_2^T T_1
		vec3AddVec3Out(&trans2->pos, &pos, &out->pos);
	}

	// Compute the new orientation!
	{
		// R = R_2 R_1
		quatMultiplyQuatByOut(trans2->rot, trans1->rot, &out->rot);
		quatNormalizeQuat(&out->rot);
	}

	// Compute the new scale!
	{
		mat3 shear1;
		mat3 shear2;
		#if 0
		// Q_1 S_1 Q_1^T
		mat3InitShearQuat(&shear1, &trans1->stretchRot, &trans1->scale);
		// Q_2' S_2 Q_2'^T
		mat3InitShearQuat(&shear2, &trans2->stretchRot, &trans2->scale);
		#else
		quat stretchRot2;

		// Q_1 S_1 Q_1^T
		mat3InitShearQuat(&shear1, &trans1->stretchRot, &trans1->scale);
		// Q_2' = R_1^T Q_2
		quatMultiplyByQuatConjOut(trans2->stretchRot, rot1, &stretchRot2);
		// Q_2' S_2 Q_2'^T
		mat3InitShearQuat(&shear2, &stretchRot2, &trans2->scale);
		#endif
		// QSQ^T = Q_2' S_2 Q_2'^T Q_1 S_1 Q_1^T
		// The shear matrices are symmetric, so we
		// do the multiplication here to save time.
		diagonalizeMat3Symmetric(
			shear2.m[0][0]*shear1.m[0][0] + shear2.m[1][0]*shear1.m[0][1] + shear2.m[2][0]*shear1.m[0][2],
			shear2.m[0][0]*shear1.m[1][0] + shear2.m[1][0]*shear1.m[1][1] + shear2.m[2][0]*shear1.m[1][2],
			shear2.m[0][0]*shear1.m[2][0] + shear2.m[1][0]*shear1.m[2][1] + shear2.m[2][0]*shear1.m[2][2],
			shear2.m[0][1]*shear1.m[1][0] + shear2.m[1][1]*shear1.m[1][1] + shear2.m[2][1]*shear1.m[1][2],
			shear2.m[0][1]*shear1.m[2][0] + shear2.m[1][1]*shear1.m[2][1] + shear2.m[2][1]*shear1.m[2][2],
			shear2.m[0][2]*shear1.m[2][0] + shear2.m[1][2]*shear1.m[2][1] + shear2.m[2][2]*shear1.m[2][2],
			&out->scale, &out->stretchRot
		);
	}
}

// Interpolate from "trans1" to "trans2" and store the result in "out"!
void transformRigidInterpSet(const transformRigid *const trans1, const transformRigid *const trans2, const float time, transformRigid *const out){
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	vec3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
}

// Interpolate from "trans1" to "trans2" and store the result in "out"!
void transformAffineInterpSet(const transformAffine *const trans1, const transformAffine *const trans2, const float time, transformAffine *const out){
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	quatSlerpFasterOut(&trans1->stretchRot, &trans2->stretchRot, time, &out->stretchRot);
	vec3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
}

/*
** Interpolate between two states and add the result to "out"!
** This is useful for applying additive animations.
*/
void transformRigidInterpAdd(const transformRigid *const trans1, const transformRigid *const trans2, const float time, transformRigid *const out){
	transformRigid interp;
	// Interpolate from "trans1" to "trans2".
	transformRigidInterpSet(trans1, trans2, time, &interp);

	// Add the interpolated transformation to "out".
	vec3AddVec3(&out->pos, &interp.pos);
	quatMultiplyQuatBy(&out->rot, interp.rot);
	quatNormalizeQuat(&out->rot);
	vec3MultiplyVec3(&out->scale, &interp.scale);
}

// Interpolate between two states and add the offsets to "out"!
void transformAffineInterpAdd(const transformAffine *const trans1, const transformAffine *const trans2, const float time, transformAffine *const out){
	/*transformRigid interp;
	// Interpolate from "trans1" to "trans2".
	transformRigidInterpSet(trans1, trans2, time, &interp);

	// Add the interpolated transformation to "out".
	vec3AddVec3(&out->pos, &interp.pos);
	quatMultiplyQuatBy(&out->rot, interp.rot);
	quatNormalizeQuat(&out->rot);
	vec3MultiplyVec3(&out->scale, &interp.scale);*/
}


// Invert all three components of a transformation state.
void transformRigidInvert(const transformRigid *const trans, transformRigid *const out){
	// Invert the transform's rotation!
	quatConjugateOut(&trans->rot, &out->rot);
	// Invert its position with respect to the new rotation!
	quatRotateVec3FastOut(&out->rot, &trans->pos, &out->pos);
	vec3Negate(&out->pos);
	// Invert its scale by storing the reciprocal of each value!
	vec3DivideSByOut(&trans->scale, 1.f, &out->scale);
}

// Invert all three components of a transformation state.
void transformAffineInvert(const transformAffine *const trans, transformAffine *const out){
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

	#if 0
	// Compute the new scale!
	{
		// S' = S^(-1)
		vec3DivideSByOut(&trans->scale, 1.f, &out->scale);
		// Q' = Q
		out->stretchRot = trans->stretchRot;
	}

	// Compute the new orientation!
	{
		// R' = R^T
		quatConjugateOut(&trans->rot, &out->rot);
	}

	// Compute the new position!
	{
		quat rot;

		// Q^T R^T T
		quatMultiplyByQuatConjOut(out->rot, out->stretchRot, &rot);
		quatRotateVec3FastOut(&rot, &trans->pos, &out->pos);
		// S^(-1)Q^T R^T T
		vec3MultiplyVec3(&out->pos, &out->scale);
		// QS^(-1)Q^T R^T T
		quatRotateVec3Fast(&out->stretchRot, &out->pos);
		// T' = -QS^(-1)Q^T R^T T
		vec3Negate(&out->pos);
	}
	#else
	// Temporarily store the original stretch rotation.
	const quat stretchRot = trans->stretchRot;

	// Compute the new scale!
	{
		// S' = S^(-1)
		vec3DivideSByOut(&trans->scale, 1.f, &out->scale);
		// Q' = RQ
		quatMultiplyQuatByOut(trans->rot, trans->stretchRot, &out->stretchRot);
	}

	// Compute the new orientation!
	{
		// R' = R^T
		quatConjugateOut(&trans->rot, &out->rot);
	}

	// Compute the new position!
	{
		// Q^T R^T T
		quatRotateVec3ConjFastOut(&out->stretchRot, &trans->pos, &out->pos);
		// S^(-1)Q^T R^T T
		vec3MultiplyVec3(&out->pos, &out->scale);
		// QS^(-1)Q^T R^T T
		quatRotateVec3Fast(&stretchRot, &out->pos);
		// T' = -QS^(-1)Q^T R^T T
		vec3Negate(&out->pos);
	}
	#endif
}

// Convert a transformation state to a matrix.
void transformRigidToMat4(const transformRigid *const restrict trans, mat4 *const restrict out){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	mat4InitQuat(out, &trans->rot);
	// This function right-multiplies by a scale matrix,
	// which is equivalent to performing the scale first.
	mat4ScalePreVec3(out, &trans->scale);
	mat4TranslateTransformVec3(out, &trans->pos);
}

// Convert a transformation state to a matrix.
void transformAffineToMat4(const transformAffine *const restrict trans, mat4 *const restrict out){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	mat4InitShearQuat(out, &trans->stretchRot, &trans->scale);
	mat4RotateByQuat(out, &trans->rot);
	mat4TranslateTransformVec3(out, &trans->pos);
}


// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformRigidTransformPoint(const transformRigid *const restrict trans, vec3 *const restrict v){
	vec3MultiplyVec3(v, &trans->scale);
	quatRotateVec3Fast(&trans->rot, v);
	vec3AddVec3(v, &trans->pos);
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformAffineTransformPoint(const transformAffine *const restrict trans, vec3 *const restrict v){
	quat rot;

	// (Q^T)v
	quatRotateVec3ConjFast(&trans->stretchRot, v);
	// (SQ^T)v
	vec3MultiplyVec3(v, &trans->scale);
	// (RQSQ^T)v
	quatMultiplyQuatByOut(trans->rot, trans->stretchRot, &rot);
	quatRotateVec3Fast(&rot, v);
	// v' = Av = (TRQSQ^T)v
	vec3AddVec3(v, &trans->pos);
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformRigidTransformPointOut(const transformRigid *const restrict trans, const vec3 *const v, vec3 *const out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
	vec3AddVec3(out, &trans->pos);
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformAffineTransformPointOut(const transformAffine *const restrict trans, const vec3 *const v, vec3 *const out){
	quat rot;

	// (Q^T)v
	quatRotateVec3ConjFastOut(&trans->stretchRot, v, out);
	// (SQ^T)v
	vec3MultiplyVec3(out, &trans->scale);
	// (RQSQ^T)v
	quatMultiplyQuatByOut(trans->rot, trans->stretchRot, &rot);
	quatRotateVec3Fast(&rot, out);
	// v' = Av = (TRQSQ^T)v
	vec3AddVec3(out, &trans->pos);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformRigidTransformDirection(const transformRigid *const restrict trans, vec3 *const restrict v){
	vec3MultiplyVec3(v, &trans->scale);
	quatRotateVec3Fast(&trans->rot, v);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformAffineTransformDirection(const transformAffine *const restrict trans, vec3 *const restrict v){
	quat rot;

	// (Q^T)v
	quatRotateVec3ConjFast(&trans->stretchRot, v);
	// (SQ^T)v
	vec3MultiplyVec3(v, &trans->scale);
	// v' = (RQSQ^T)v
	quatMultiplyQuatByOut(trans->rot, trans->stretchRot, &rot);
	quatRotateVec3Fast(&rot, v);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformRigidTransformDirectionOut(const transformRigid *const restrict trans, const vec3 *const v, vec3 *const out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformAffineTransformDirectionOut(const transformAffine *const restrict trans, const vec3 *const v, vec3 *const out){
	quat rot;

	// (Q^T)v
	quatRotateVec3ConjFastOut(&trans->stretchRot, v, out);
	// (SQ^T)v
	vec3MultiplyVec3(out, &trans->scale);
	// v' = (RQSQ^T)v
	quatMultiplyQuatByOut(trans->rot, trans->stretchRot, &rot);
	quatRotateVec3Fast(&rot, out);
}