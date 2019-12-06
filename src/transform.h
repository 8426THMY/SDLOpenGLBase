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


void transformStateInit(transformState *trans);

// All of these functions come from bones.
void transformStateAppend(const transformState *trans1, const transformState *trans2, transformState *out);
void transformStateInterpSet(const transformState *trans1, const transformState *trans2, const float time, transformState *out);
void transformStateInterpAdd(const transformState *trans1, const transformState *trans2, const float time, transformState *out);

void transformStateInvert(const transformState *trans, transformState *out);
void transformStateToMat4(const transformState *trans, mat4 *out);

void transformStateTransformVec3(const transformState *trans, const vec3 *v, vec3 *out);


extern transformState identityTransform;


#endif