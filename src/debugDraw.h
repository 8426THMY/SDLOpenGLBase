#ifndef debugDraw_h
#define debugDraw_h


#include "vec3.h"
#include "mat4.h"

#include "utilTypes.h"


typedef struct debugDrawInfo {
	unsigned int fillMode;
	vec3 colour;
} debugDrawInfo;


typedef struct colliderHull colliderHull;

debugDrawInfo debugDrawInfoInit(const unsigned int fillMode, const vec3 colour);

return_t debugDrawSetup();
void debugDrawColliderHull(const colliderHull *const restrict hull, const debugDrawInfo info, const mat4 *const restrict vpMatrix);
void debugDrawCleanup();


#endif