#ifndef transform_h
#define transform_h


#include "vec3.h"
#include "quat.h"
#include "mat4.h"


typedef struct transformState {
	vec3 pos;
	quat rot;
	vec3 scale;
} transformState;


void transformStateInit(transformState *const restrict trans);

// All of these functions come from bones.
void transformStateAppend(const transformState *const restrict trans1, const transformState *const restrict trans2, transformState *const restrict out);
void transformStateUndoPrepend(const transformState *const restrict trans1, const transformState *const restrict trans2, transformState *const restrict out);
void transformStateInterpSet(const transformState *const restrict trans1, const transformState *const restrict trans2, const float time, transformState *const restrict out);
void transformStateInterpAdd(const transformState *const restrict trans1, const transformState *const restrict trans2, const float time, transformState *const restrict out);

void transformStateInvert(const transformState *const restrict trans, transformState *const restrict out);
void transformStateToMat4(const transformState *const restrict trans, mat4 *const restrict out);

void transformStateTransformPosition(const transformState *const restrict trans, const vec3 *const restrict v, vec3 *const restrict out);
void transformStateTransformVelocity(const transformState *const restrict trans, const vec3 *const restrict v, vec3 *const restrict out);


extern transformState g_transformIdentity;


#endif