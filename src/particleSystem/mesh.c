#include "mesh.h"


#define GLEW_STATIC
#include <GL/glew.h>


// Generate vertex and index buffers to hold our mesh data!
void meshInit(
	mesh *const restrict meshData,
	const meshVertex *const restrict vertices, const size_t numVertices,
	const meshVertexIndex *const restrict indices, const size_t numIndices
){

	// Generate a vertex array object for our mesh and bind it!
	glGenVertexArrays(1, &meshData->vertexArrayID);
	glBindVertexArray(meshData->vertexArrayID);
		// Generate a buffer object for our vertex data and bind it!
		glGenBuffers(1, &meshData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, meshData->vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * numVertices, vertices, GL_STATIC_DRAW);

		// Set up the vertex attributes that require this buffer!
		// Vertex positions.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, 3, GL_FLOAT, GL_FALSE,
			sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, pos)
		);
		// Vertex UVs.
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1, 2, GL_FLOAT, GL_FALSE,
			sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, uv)
		);
		// Vertex normals.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2, 3, GL_FLOAT, GL_FALSE,
			sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, normal)
		);
		// Vertex bone IDs.
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(
			3, MESH_VERTEX_MAX_BONE_WEIGHTS, GL_UNSIGNED_INT,
			sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, boneIDs)
		);
		// Vertex bone weights.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4, MESH_VERTEX_MAX_BONE_WEIGHTS, GL_FLOAT, GL_FALSE,
			sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, boneWeights)
		);


		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(0);

	meshData->numIndices = numIndices;
}

/*
** Return whether or not two vertices are different.
** This works so long as our vertices have no padding.
*/
return_t meshVertexDifferent(
	const meshVertex *const restrict v1,
	const meshVertex *const restrict v2
){

	return(memcmp(v1, v2, sizeof(*v2)) != 0);
}

void meshDelete(mesh *const restrict meshData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our mesh structure to have no padding.
	glDeleteBuffers(2, &meshData->vertexBufferID);
	glDeleteVertexArrays(1, &meshData->vertexArrayID);
}