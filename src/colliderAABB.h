#ifndef colliderAABB_h
#define colliderAABB_h


#include "utilTypes.h"
#include "vec3.h"


typedef struct colliderAABB {
	vec3 min;
	vec3 max;
} colliderAABB;


void colliderAABBExpandFloat(const colliderAABB *const aabb, const float x, colliderAABB *const out);
void colliderAABBExpandVec3(const colliderAABB *const aabb, const vec3 *const restrict v, colliderAABB *const out);
void colliderAABBCombine(const colliderAABB *const aabbA, const colliderAABB *const aabbB, colliderAABB *const out);

float colliderAABBVolume(const colliderAABB *const restrict aabb);
float colliderAABBSurfaceArea(const colliderAABB *const restrict aabb);
float colliderAABBSurfaceAreaHalf(const colliderAABB *const restrict aabb);

float colliderAABBCombinedVolume(const colliderAABB *const restrict aabbA, const colliderAABB *const restrict aabbB);
float colliderAABBCombinedSurfaceArea(const colliderAABB *const restrict aabbA, const colliderAABB *const restrict aabbB);
float colliderAABBCombinedSurfaceAreaHalf(const colliderAABB *const restrict aabbA, const colliderAABB *const restrict aabbB);

return_t colliderAABBEnvelopsAABB(const colliderAABB *const restrict aabbA, const colliderAABB *const restrict aabbB);
return_t colliderAABBCollidingAABB(const colliderAABB *const restrict aabbA, const colliderAABB *const restrict aabbB);


#endif