#include "transform.h"


 transformState g_transformIdentity = {
	.pos.x   = 0.f, .pos.y   = 0.f, .pos.z   = 0.f,
	.rot.x   = 0.f, .rot.y   = 0.f, .rot.z   = 0.f, .rot.w = 1.f,
	.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f
};


void transformStateInit(transformState *trans){
	*trans = g_transformIdentity;
}


/*
** Append "trans2" to "trans1". This is equivalent to left-
** multiplying "trans1" by "trans2" as if they're matrices:
**     T = T_2 * T_1.
*/
void transformStateAppend(const transformState *const trans1, const transformState *const trans2, transformState *const out){
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
** Prepend "trans2" to "trans1". This is equivalent to right-
** multiplying "trans1" by "trans2" as if they're matrices:
**     T = T_1 * T_2.
** It can be used as an inverse to our append function:
**     T = prepend(append(T_0, T), invert(T_0)).
*/
void transformStatePrepend(const transformState *const trans1, const transformState *const trans2, transformState *const out){
	// Generate the new position!
	vec3AddVec3Out(&trans1->pos, &trans2->pos, &out->pos);
	quatRotateVec3Fast(&trans2->rot, &out->pos);
	vec3MultiplyVec3(&out->pos, &trans2->scale);
	// Generate the new orientation!
	quatMultiplyQuatByOut(trans2->rot, trans1->rot, &out->rot);
	// A slight error will build up if we don't normalize the rotation.
	quatNormalizeQuat(&out->rot);
	// Generate the new scale!
	vec3MultiplyVec3Out(&trans1->scale, &trans2->scale, &out->scale);
}

/*
** Remove the effect of prepending "trans2" to "trans1".
** This is equivalent to right-multiplying "trans1" by
** the inverse of "trans2" as if they're matrices:
**     T = T_1 * T_2^(-1).
*/
void transformStateUndoPrepend(const transformState *const trans1, const transformState *const trans2, transformState *const out){
	transformState inverse;
	quatConjugateOut(&trans2->rot, &inverse.rot);
	vec3DivideSByFastOut(&trans2->scale, 1.f, &inverse.scale);

	// Recover the original position!
	vec3SubtractVec3FromOut(&trans1->pos, &trans2->pos, &out->pos);
	quatRotateVec3Fast(&inverse.rot, &out->pos);
	vec3MultiplyVec3(&out->pos, &inverse.scale);

	// Recover the original orientation!
	quatMultiplyQuatByOut(inverse.rot, trans1->rot, &out->rot);
	quatNormalizeQuat(&out->rot);

	// Recover the original scale!
	vec3MultiplyVec3Out(&trans1->scale, &inverse.scale, &out->scale);
}

// Interpolate between two states and store the result in "out"!
void transformStateInterpSet(const transformState *const trans1, const transformState *const trans2, const float time, transformState *const out){
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	vec3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
}

// Interpolate between two states and add the offsets to "out"!
void transformStateInterpAdd(const transformState *const trans1, const transformState *const trans2, const float time, transformState *const restrict out){
	transformState interp;

	// Interpolate between "trans1" and "trans2", then
	// add the resulting offsets to the "out" state!
	vec3Lerp(&trans1->pos, &trans2->pos, time, &interp.pos);
	vec3AddVec3(&out->pos, &interp.pos);

	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &interp.rot);
	quatMultiplyQuatBy(&out->rot, interp.rot);
	quatNormalizeQuat(&out->rot);

	vec3Lerp(&trans1->scale, &trans2->scale, time, &interp.scale);
	vec3MultiplyVec3(&out->scale, &interp.scale);
}


// Invert all three components of a transformation state.
void transformStateInvert(const transformState *const trans, transformState *const out){
	/*// Invert the transform's position!
	vec3NegateOut(&trans->pos, &out->pos);
	// Invert the transform's rotation!
	quatConjugateOut(&trans->rot, &out->rot);
	// Invert its scale by storing the reciprocal of each value!
	vec3DivideSByOut(&trans->scale, 1.f, &out->scale);*/

	// Invert the transform's rotation!
	quatConjugateOut(&trans->rot, &out->rot);

	// Invert its position with respect to the new rotation!
	quatRotateVec3FastOut(&out->rot, &trans->pos, &out->pos);
	vec3Negate(&out->pos);

	// Invert its scale by storing the reciprocal of each value!
	vec3DivideSByOut(&trans->scale, 1.f, &out->scale);
}

// Convert a transformation state to a matrix.
void transformStateToMat4(const transformState *const restrict trans, mat4 *const restrict out){
	// We use the regular order, that is, we first
	// scale, then rotate and finally translate.
	mat4InitRotateQuat(out, &trans->rot);
	// This function right-multiplies by a scale matrix,
	// which is equivalent to performing the scale first.
	mat4ScalePreVec3(out, &trans->scale);
	mat4TranslateTransformVec3(out, &trans->pos);
}


// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformStateTransformPoint(const transformState *const restrict trans, vec3 *const restrict v){
	vec3MultiplyVec3(v, &trans->scale);
	quatRotateVec3Fast(&trans->rot, v);
	vec3AddVec3(v, &trans->pos);
}

// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformStateTransformPointOut(const transformState *const restrict trans, const vec3 *const v, vec3 *const out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
	vec3AddVec3(out, &trans->pos);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformStateTransformDirection(const transformState *const restrict trans, vec3 *const restrict v){
	vec3MultiplyVec3(v, &trans->scale);
	quatRotateVec3Fast(&trans->rot, v);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformStateTransformDirectionOut(const transformState *const restrict trans, const vec3 *const v, vec3 *const out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
}