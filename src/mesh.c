#include "mesh.h"


void meshInit(mesh *meshData){
	meshData->vertexArrayID = 0;
	meshData->vertexBufferID = 0;
	meshData->indexBufferID = 0;
	meshData->numIndices = 0;
}


#warning "The way we generate buffers really depends on the shader we want to use."
// Generate vertex and index buffers to hold our mesh data!
void meshGenerateBuffers(mesh *meshData, const vertex *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices){
	// Generate a vertex array object for our mesh and bind it!
	glGenVertexArrays(1, &meshData->vertexArrayID);
	glBindVertexArray(meshData->vertexArrayID);
		// Generate a vertex buffer object for our vertex data and bind it!
		glGenBuffers(1, &meshData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, meshData->vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * numVertices, vertices, GL_STATIC_DRAW);

		// Generate a vertex buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);


		// Meshes will need these vertex attributes!
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		// Tell OpenGL which data belongs to the vertex attributes!
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, normal));
		glVertexAttribIPointer(3, VERTEX_MAX_WEIGHTS, GL_UNSIGNED_INT, sizeof(vertex), (GLvoid *)offsetof(vertex, boneIDs));
		glVertexAttribPointer(4, VERTEX_MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, boneWeights));
	// Unbind the array object!
	glBindVertexArray(0);

	meshData->numIndices = numIndices;
}


void meshDelete(mesh *meshData){
	if(meshData->vertexBufferID != 0){
		glDeleteBuffers(1, &meshData->vertexBufferID);
	}
	if(meshData->indexBufferID != 0){
		glDeleteBuffers(1, &meshData->indexBufferID);
	}
	if(meshData->vertexArrayID != 0){
		glDeleteVertexArrays(1, &meshData->vertexArrayID);
	}
}