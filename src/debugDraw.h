#ifndef debugDraw_h
#define debugDraw_h


#include "vec3.h"
#include "mat4.h"

#include "utilTypes.h"


typedef struct debugDrawInfo {
	unsigned int fillMode;
	vec3 colour;
	float size;
} debugDrawInfo;


typedef struct skeletonState skeletonState;
typedef struct colliderAABB colliderAABB;
typedef struct colliderHull colliderHull;

debugDrawInfo debugDrawInfoInit(const unsigned int fillMode, const vec3 colour, const float size);

return_t debugDrawSetup();
void debugDrawSkeleton(const skeletonState *const restrict skeleState, const debugDrawInfo info, const mat4 *const restrict vpMatrix);
void debugDrawColliderAABB(const colliderAABB *aabb, const debugDrawInfo info, const mat4 *const restrict vpMatrix);
void debugDrawColliderHull(const colliderHull *const restrict hull, const debugDrawInfo info, const mat4 *const restrict vpMatrix);
void debugDrawCleanup();


#endif