#include "transform.h"


 transformState g_transformIdentity = {
	.pos.x   = 0.f, .pos.y   = 0.f, .pos.z   = 0.f,
	.rot.x   = 0.f, .rot.y   = 0.f, .rot.z   = 0.f, .rot.w = 1.f,
	.scale.x = 1.f, .scale.y = 1.f, .scale.z = 1.f
};


void transformStateInit(transformState *trans){
	*trans = g_transformIdentity;
}


// Append "trans2" to "trans1". This effectively multiplies them as if they're matrices.
void transformStateAppend(const transformState *const trans1, const transformState *const trans2, transformState *const out){
	vec3 pos;

	vec3MultiplyVec3Out(&trans2->pos, &trans1->scale, &pos);
	quatRotateVec3Fast(&trans1->rot, &pos);
	// Generate the new position!
	vec3AddVec3Out(&trans1->pos, &pos, &out->pos);
	// Generate the new orientation!
	quatMultiplyQuatByOut(trans1->rot, trans2->rot, &out->rot);
	// A slight error will build up if we don't normalize the rotation.
	quatNormalizeQuat(&out->rot);
	// Generate the new scale!
	vec3MultiplyVec3Out(&trans1->scale, &trans2->scale, &out->scale);
}

// Remove the effect of prepending "trans1" to "trans2".
void transformStateUndoPrepend(const transformState *const trans1, const transformState *const trans2, transformState *const out){
	transformState inverse;
	vec3DivideSByFastOut(&trans1->scale, 1.f, &inverse.scale);
	quatConjugateOut(&trans1->rot, &inverse.rot);

	// Recover the original position!
	vec3SubtractVec3FromOut(&trans2->pos, &trans1->pos, &inverse.pos);
	quatRotateVec3Fast(&inverse.rot, &inverse.pos);
	vec3MultiplyVec3Out(&inverse.pos, &inverse.scale, &out->pos);

	// Recover the original orientation!
	quatMultiplyQuatByOut(inverse.rot, trans2->rot, &out->rot);
	quatNormalizeQuatFast(&out->rot);

	// Recover the original scale!
	vec3MultiplyVec3Out(&trans2->scale, &inverse.scale, &out->scale);
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
	// We translate, scale and then rotate:
	//     *out = mat4RotateQuatC(mat4ScaleVec3C(mat4InitTranslateVec3C(trans->pos), trans->scale), trans->rot);
	// It looks a bit weird because it's been optimized pretty heavily.
	mat4InitRotateQuat(out, &trans->rot);
	mat4ScaleVec3(out, &trans->scale);
	out->m[3][0] = trans->pos.x;
	out->m[3][1] = trans->pos.y;
	out->m[3][2] = trans->pos.z;
}


// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformStateTransformPosition(const transformState *const restrict trans, const vec3 *const v, vec3 *const out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
	vec3AddVec3(out, &trans->pos);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformStateTransformVelocity(const transformState *const restrict trans, const vec3 *const v, vec3 *const out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
}