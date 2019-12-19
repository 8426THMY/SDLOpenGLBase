#ifndef mesh_h
#define mesh_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>

#include "vertex.h"


typedef struct mesh {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	size_t numIndices;
} mesh;


void meshInit(mesh *meshData);

void meshGenerateBuffers(mesh *meshData, const vertex *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices);

void meshDelete(mesh *meshData);


#endif