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


void meshInit(mesh *meshData);

void meshGenerateBuffers(mesh *meshData, const vertex *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices);

return_t vertexDifferent(const vertex *v1, const vertex *v2);
return_t meshDifferent(const mesh *m1, const mesh *m2);

return_t meshSetupDefault();

void meshDelete(mesh *meshData);


extern mesh g_meshDefault;


#endif