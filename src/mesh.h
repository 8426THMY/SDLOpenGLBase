#ifndef mesh_h
#define mesh_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>

#include "vec2.h"
#include "vec3.h"

#include "utilTypes.h"


#define VERTEX_MAX_WEIGHTS 4


typedef GLsizei meshVertexIndex_t;
typedef GLuint meshBoneIndex_t;

typedef struct vertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;

	meshBoneIndex_t boneIDs[VERTEX_MAX_WEIGHTS];
	float boneWeights[VERTEX_MAX_WEIGHTS];
} vertex;

typedef struct mesh {
	GLuint vertexArrayID;
	GLuint vertexBufferID;

	GLuint indexBufferID;
	meshVertexIndex_t numIndices;
} mesh;


// These shaders are used for drawing most things, such as objects.
typedef struct meshShader {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint uvOffsetsID;
	GLuint boneStatesID;
} meshShader;


void meshInit(mesh *const restrict meshData);

void meshGenerateBuffers(
	mesh *const restrict meshData, const vertex *const restrict vertices, const meshVertexIndex_t numVertices,
	const meshVertexIndex_t *const restrict indices, const meshVertexIndex_t numIndices
);

return_t vertexDifferent(const vertex *const restrict v1, const vertex *const restrict v2);
return_t meshDifferent(const mesh *const restrict m1, const mesh *const restrict m2);

return_t meshSetupDefault();

void meshDelete(mesh *const restrict meshData);

return_t meshShaderInit(meshShader *const restrict shader, const GLuint programID);


extern mesh g_meshDefault;


#endif