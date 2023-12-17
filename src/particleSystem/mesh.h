#ifndef mesh_h
#define mesh_h


#include <stdint.h>
#include <stddef.h>

#include "vec2.h"
#include "vec3.h"

#include "utilTypes.h"


#define MESH_VERTEX_MAX_BONE_WEIGHTS 4


typedef struct meshVertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;

	size_t boneIDs[MESH_VERTEX_MAX_BONE_WEIGHTS];
	float boneWeights[MESH_VERTEX_MAX_BONE_WEIGHTS];
} meshVertex;

typedef uint_least32_t meshVertexIndex;

typedef struct mesh {
	unsigned int vertexArrayID;
	unsigned int vertexBufferID;
	unsigned int indexBufferID;
	size_t numIndices;
} mesh;


void meshInit(
	mesh *const restrict meshData,
	const meshVertex *const restrict vertices, const size_t numVertices,
	const meshVertexIndex *const restrict indices, const size_t numIndices
);
return_t meshVertexDifferent(
	const meshVertex *const restrict v1,
	const meshVertex *const restrict v2
);
void meshDelete(mesh *const restrict meshData);


#endif