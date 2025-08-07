#ifndef mesh_h
#define mesh_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


typedef GLsizei meshVertexIndex;

typedef struct mesh {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
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
void meshDrawInstanced(const mesh *const restrict meshData, const GLsizei numInstances);

void meshDelete(mesh *const restrict meshData);

return_t meshSetup();
void meshCleanup();


#endif