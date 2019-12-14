#ifndef vertex_h
#define vertex_h


#include <stddef.h>

#include "vec2.h"
#include "vec3.h"

#include "utilTypes.h"


#define VERTEX_MAX_WEIGHTS 4


typedef struct vertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;

	size_t boneIDs[VERTEX_MAX_WEIGHTS];
	float boneWeights[VERTEX_MAX_WEIGHTS];
} vertex;


return_t vertexDifferent(const vertex *v1, const vertex *v2);


#endif