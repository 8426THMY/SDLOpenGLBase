#ifndef mesh_h
#define mesh_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>

#include "vec2.h"
#include "vec3.h"

#include "utilTypes.h"


#define MESH_VERTEX_MAX_BONE_WEIGHTS 4


typedef GLsizei meshVertexIndex_t;
typedef GLuint meshBoneIndex_t;

#warning "Eventually, we should distinguish between mesh vertices and model vertices."
typedef struct meshVertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;

	meshBoneIndex_t boneIDs[MESH_VERTEX_MAX_BONE_WEIGHTS];
	float boneWeights[MESH_VERTEX_MAX_BONE_WEIGHTS];
} meshVertex;

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


void meshInit(
	mesh *const restrict meshData,
	const meshVertex *const restrict vertices, const meshVertexIndex_t numVertices,
	const meshVertexIndex_t *const restrict indices, const meshVertexIndex_t numIndices
);
return_t meshVertexDifferent(const meshVertex *const restrict v1, const meshVertex *const restrict v2);
return_t meshDifferent(const mesh *const restrict m1, const mesh *const restrict m2);
void meshDelete(mesh *const restrict meshData);

return_t meshShaderInit(meshShader *const restrict shader, const GLuint programID);


#endif