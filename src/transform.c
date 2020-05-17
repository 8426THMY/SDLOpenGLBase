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
void transformStateAppend(const transformState *const restrict trans1, const transformState *const restrict trans2, transformState *const restrict out){
	vec3 pos;

	vec3MultiplyVec3Out(&trans1->scale, &trans2->pos, &pos);
	quatRotateVec3Fast(&trans1->rot, &pos);
	// Generate the new position!
	vec3AddVec3Out(&trans1->pos, &pos, &out->pos);
	// Generate the new orientation!
	quatMultiplyByQuatOut(trans1->rot, trans2->rot, &out->rot);
	// A slight error will build up if we don't normalize the rotation.
	quatNormalizeQuat(&out->rot);
	// Generate the new scale!
	vec3MultiplyVec3Out(&trans1->scale, &trans2->scale, &out->scale);
}

// Interpolate between two states and store the result in "out"!
void transformStateInterpSet(const transformState *const restrict trans1, const transformState *const restrict trans2, const float time, transformState *const restrict out){
	vec3Lerp(&trans1->pos, &trans2->pos, time, &out->pos);
	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &out->rot);
	vec3Lerp(&trans1->scale, &trans2->scale, time, &out->scale);
}

// Interpolate between two states and add the offsets to "out"!
void transformStateInterpAdd(const transformState *const restrict trans1, const transformState *const restrict trans2, const float time, transformState *const restrict out){
	union {
		vec3 pos;
		quat rot;
		vec3 scale;
	} interp;

	// Interpolate between "trans1" and "trans2", then
	// add the resulting offsets to the "out" state!
	vec3Lerp(&trans1->pos, &trans2->pos, time, &interp.pos);
	vec3AddVec3(&out->pos, &interp.pos);

	quatSlerpFasterOut(&trans1->rot, &trans2->rot, time, &interp.rot);
	quatMultiplyByQuat(&out->rot, &interp.rot);
	quatNormalizeQuat(&out->rot);

	vec3Lerp(&trans1->scale, &trans2->scale, time, &interp.scale);
	vec3MultiplyVec3(&out->scale, &interp.scale);
}


// Invert all three components of a transformation state.
void transformStateInvert(const transformState *const restrict trans, transformState *const restrict out){
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
	mat4InitTranslateVec3(out, &trans->pos);
	mat4RotateQuat(out, &trans->rot);
	mat4ScaleVec3(out, &trans->scale);
}


// Transform a vec3 by scaling it, rotating it and finally translating it.
void transformStateTransformPosition(const transformState *const restrict trans, const vec3 *const restrict v, vec3 *const restrict out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
	vec3AddVec3(out, &trans->pos);
}

// Transform a vec3 by scaling it and rotating it, but not translating it.
void transformStateTransformVelocity(const transformState *const restrict trans, const vec3 *const restrict v, vec3 *const restrict out){
	vec3MultiplyVec3Out(&trans->scale, v, out);
	quatRotateVec3Fast(&trans->rot, out);
}