#ifndef transform_h
#define transform_h


#include "vec3.h"
#include "quat.h"
#include "mat4.h"


/*
** Stores all the information necessary to represent
** a rigid transformation plus a uniform scaling.
*/
typedef struct transformRigid {
	vec3 pos;
	quat rot;
	vec3 scale;
} transformRigid;

/*
** To correctly handle non-uniform scaling, we introduce
** a "stretch quaternion" that rotates us so that we can
** easily perform our scaling along the x, y and z axes.
**
** This is usually only necessary for things like skeletons,
** where we may want to add extra flare to their animations.
** However, it cannot be used with most colliders, such as
** spheres and capsules. The one exception to this is hulls.
*/
typedef struct transformAffine {
	vec3 pos;
	quat rot;
	quat stretchRot;
	vec3 scale;
} transformAffine;


void transformRigidInit(transformRigid *const restrict trans);
void transformAffineInit(transformAffine *const restrict trans);

// All of these functions come from bones.
void transformRigidAppend(const transformRigid *const trans1, const transformRigid *const trans2, transformRigid *const out);
void transformAffineAppend(const transformAffine *const trans1, const transformAffine *const trans2, transformAffine *const out);
void transformRigidInterpSet(const transformRigid *const trans1, const transformRigid *const trans2, const float time, transformRigid *const out);
void transformAffineInterpSet(const transformAffine *const trans1, const transformAffine *const trans2, const float time, transformAffine *const out);
void transformRigidInterpAdd(const transformRigid *const trans1, const transformRigid *const trans2, const float time, transformRigid *const out);
void transformAffineInterpAdd(const transformAffine *const trans1, const transformAffine *const trans2, const float time, transformAffine *const out);

void transformRigidInvert(const transformRigid *const trans, transformRigid *const out);
void transformAffineInvert(const transformAffine *const trans, transformAffine *const out);
void transformRigidToMat4(const transformRigid *const restrict trans, mat4 *const restrict out);
void transformAffineToMat4(const transformAffine *const restrict trans, mat4 *const restrict out);

void transformRigidTransformPoint(const transformRigid *const restrict trans, vec3 *const restrict v);
void transformAffineTransformPoint(const transformAffine *const restrict trans, vec3 *const restrict v);
void transformRigidTransformPointOut(const transformRigid *const restrict trans, const vec3 *const v, vec3 *const out);
void transformAffineTransformPointOut(const transformAffine *const restrict trans, const vec3 *const v, vec3 *const out);
void transformRigidTransformDirection(const transformRigid *const restrict trans, vec3 *const restrict v);
void transformAffineTransformDirection(const transformAffine *const restrict trans, vec3 *const restrict v);
void transformRigidTransformDirectionOut(const transformRigid *const restrict trans, const vec3 *const v, vec3 *const out);
void transformAffineTransformDirectionOut(const transformAffine *const restrict trans, const vec3 *const v, vec3 *const out);


extern transformRigid g_transformRigidIdentity;
extern transformAffine g_transformAffineIdentity;


#endif