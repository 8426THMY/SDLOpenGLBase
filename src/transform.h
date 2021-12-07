#ifndef transform_h
#define transform_h


#include "vec3.h"
#include "quat.h"
#include "mat4.h"


/*
** A transform state is a representation of a rigid
** transformation that is more amenable to interpolation.
**
** The only caveat currently is that when combining two or
** more transformations (for instance, in a skeleton), we
** cannot correctly handle the propagation of non-uniform
** scaling. One possible solution to this is to introduce
** an additional "stretch quaternion" component; however,
** the problem then is that actually combining two scale
** and stretch quaternion pairs becomes very difficult
** without using something like the affine decomposition
** technique developed by Ken Shoemake in Graphics Gems IV.
**
** Non-uniform scaling causes issues elsewhere, however.
** For instance, collision detection for colliders such
** as spheres and capsules, which cannot be trivially
** replaced by general hulls, become much harder to handle.
** Interestingly, despite supporting non-uniform scaling to
** some degree, Unity specifically ignores it in such cases.
*/
typedef struct transformState {
	vec3 pos;
	quat rot;
	vec3 scale;
} transformState;


void transformStateInit(transformState *const restrict trans);

// All of these functions come from bones.
void transformStateAppend(const transformState *const trans1, const transformState *const trans2, transformState *const out);
void transformStatePrepend(const transformState *const trans1, const transformState *const trans2, transformState *const out);
void transformStateUndoPrepend(const transformState *const trans1, const transformState *const trans2, transformState *const out);
void transformStateInterpSet(const transformState *const trans1, const transformState *const trans2, const float time, transformState *const out);
void transformStateInterpAdd(const transformState *const trans1, const transformState *const trans2, const float time, transformState *const out);

void transformStateInvert(const transformState *const trans, transformState *const out);
void transformStateToMat4(const transformState *const restrict trans, mat4 *const restrict out);

void transformStateTransformPosition(const transformState *const restrict trans, const vec3 *const v, vec3 *const out);
void transformStateTransformVelocity(const transformState *const restrict trans, const vec3 *const v, vec3 *const out);


extern transformState g_transformIdentity;


#endif