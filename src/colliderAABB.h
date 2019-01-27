#ifndef colliderAABB_h
#define colliderAABB_h


#include "utilTypes.h"
#include "vec3.h"


typedef struct colliderAABB {
	vec3 minPoint;
	vec3 maxPoint;
} colliderAABB;


void colliderAABBFattenFloat(const colliderAABB *aabb, const float x, colliderAABB *out);
void colliderAABBFattenVec3(const colliderAABB *aabb, const vec3 *v, colliderAABB *out);
void colliderAABBCombine(const colliderAABB *aabbA, const colliderAABB *aabbB, colliderAABB *out);

float colliderAABBVolume(const colliderAABB *aabb);
float colliderAABBSurfaceArea(const colliderAABB *aabb);
float colliderAABBSurfaceAreaHalf(const colliderAABB *aabb);

float colliderAABBCombinedVolume(const colliderAABB *aabbA, const colliderAABB *aabbB);
float colliderAABBCombinedSurfaceArea(const colliderAABB *aabbA, const colliderAABB *aabbB);
float colliderAABBCombinedSurfaceAreaHalf(const colliderAABB *aabbA, const colliderAABB *aabbB);

return_t colliderAABBEnvelopsAABB(const colliderAABB *aabbA, const colliderAABB *aabbB);
return_t colliderAABBCollidingAABB(const colliderAABB *aabbA, const colliderAABB *aabbB);


#endif