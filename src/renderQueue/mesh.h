#ifndef mesh_h
#define mesh_h


#include <stddef.h>

#include "mat3x4.h"
#include "rectangle.h"

#include "utilTypes.h"


typedef size_t meshVertexIndex;

#warning "We should store all of the meshes for a model in a single VAO, and use index offsets to access them."
typedef struct mesh {
	unsigned int vertexArrayID;
	unsigned int vertexBufferID;
	unsigned int indexBufferID;
	meshVertexIndex numIndices;
} mesh;


// This represents the data we must send to
// the shader for each instance of a mesh.
typedef struct meshInstance {
	mat3x4 state;
	rectangle uvOffsets;
} meshInstance;


typedef struct spriteVertex spriteVertex;
void meshSpriteInit(
	mesh *const restrict meshData,
	const spriteVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
);
void meshSpriteInitInstanced(
	mesh *const restrict meshData,
	const spriteVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
);
typedef struct modelVertex modelVertex;
void meshModelInit(
	mesh *const restrict meshData,
	const modelVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
);
void meshModelInitInstanced(
	mesh *const restrict meshData,
	const modelVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
);

void meshDraw(const mesh *const restrict meshData);
void meshDrawInstanced(const mesh *const restrict meshData, const size_t numInstances);

void meshDelete(mesh *const restrict meshData);

return_t meshSetup();
void meshCleanup();


#endif