#ifndef colliderSphere_h
#define colliderSphere_h


#include "vec3.h"

#include "utilTypes.h"


typedef struct colliderSphere {
	vec3 pos;
	float radius;
} colliderSphere;


return_t colliderSphereRaycast();


#endif