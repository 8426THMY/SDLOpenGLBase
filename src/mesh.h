#ifndef mesh_h
#define mesh_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>

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

typedef struct mesh {
	GLuint vertexArrayID;
	GLuint vertexBufferID;

	GLuint indexBufferID;
	size_t numIndices;
} mesh;


void meshInit(mesh *const restrict meshData);

void meshGenerateBuffers(
	mesh *const restrict meshData, const vertex *const restrict vertices, const size_t numVertices,
	const size_t *const restrict indices, const size_t numIndices
);

return_t vertexDifferent(const vertex *const restrict v1, const vertex *const restrict v2);
return_t meshDifferent(const mesh *const restrict m1, const mesh *const restrict m2);

return_t meshSetupDefault();

void meshDelete(mesh *const restrict meshData);


extern mesh g_meshDefault;


#endif