#ifndef mesh_h
#define mesh_h


#include <stdint.h>
#include <stddef.h>

#include "vec2.h"
#include "vec3.h"
#include "rectangle.h"
#include "mat4.h"

#include "utilTypes.h"


// By default, we allow 10,000 meshes to be instanced at once.
#define MESH_BUFFER_MAX_INSTANCES 10000
#define MESH_INSTANCE_BUFFER_SIZE (MESH_BUFFER_MAX_INSTANCES*sizeof(meshInstance))

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


// Stores the data required for instanced rendering.
typedef struct meshInstance {
	mat4 state;
	rectangle uvOffsets;
} meshInstance;

typedef struct meshBatch {
	const mesh *meshData;
	meshInstance *instances;
	size_t numInstances;
} meshBatch;


void meshSetup();

void meshInit(
	mesh *const restrict meshData,
	const meshVertex *const restrict vertices, const size_t numVertices,
	const meshVertexIndex *const restrict indices, const size_t numIndices
);
return_t meshVertexDifferent(
	const meshVertex *const restrict v1,
	const meshVertex *const restrict v2
);
void meshDraw(
	const mesh *const restrict meshData,
	const textureGroupFrame *const restrict texFrame,
	const camera *const restrict cam,
	const meshShader *const restrict shader
);
void meshDelete(mesh *const restrict meshData);

void meshBatchInit(meshBatch *const restrict batch);
void meshBatchDraw(const meshBatch *const restrict batch);

void meshCleanup();


#endif