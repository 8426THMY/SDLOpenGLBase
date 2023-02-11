#include "mesh.h"


#define GLEW_STATIC
#include <GL/glew.h>


// We use a single state buffer plus
// orphaning for all instanced rendering.
GLuint meshInstanceBufferID;


// Initialize the global instance buffer.
void meshSetup(){
	// Generate a buffer object for our particle's states and bind it!
	glGenBuffers(1, &meshInstanceBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, meshInstanceBufferID);
	// We write to this buffer dynamically when drawing sprites.
	glBufferData(GL_ARRAY_BUFFER, MESH_INSTANCE_BUFFER_SIZE, NULL, GL_STREAM_DRAW);

	// Set up the vertex array object attributes that require this buffer!
	// Because the transformation state is a 4x4 matrix, it needs four vertex attributes.
	// Transformation state first row.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[0]));
	glVertexAttribDivisor(2, 1);
	// Transformation state second row.
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[1]));
	glVertexAttribDivisor(3, 1);
	// Transformation state third row.
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[2]));
	glVertexAttribDivisor(4, 1);
	// Transformation state fourth row.
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[3]));
	glVertexAttribDivisor(5, 1);
	// UV offsets.
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, uvOffsets));
	glVertexAttribDivisor(6, 1);
}


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

		// Set up the vertex array object attributes that require this buffer!
		// Vertex positions.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, pos));
		// Vertex UVs.
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, uv));
		// Vertex normals.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, normal));
		// Vertex bone IDs.
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, VERTEX_MAX_WEIGHTS, GL_UNSIGNED_INT, sizeof(vertex), (GLvoid *)offsetof(vertex, boneIDs));
		// Vertex bone weights.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, VERTEX_MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, boneWeights));


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

void meshDraw(
	const mesh *const restrict meshData,
	const textureGroupFrame *const restrict texFrame,
	const camera *const restrict cam,
	const meshShader *const restrict shader
){

	#warning "All of this stuff should be moved out eventually."
	#warning "This probably shouldn't be here."
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texFrame->tex->id);

	#warning "The uniforms should be moved out eventually."
	glUniform1fv(shader->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);

	glBindVertexArray(meshData->vertexArrayID);
	glDrawElements(GL_TRIANGLES, meshData->numIndices, GL_UNSIGNED_INT, NULL);
}

void meshDelete(mesh *const restrict meshData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our mesh structure to have no padding.
	glDeleteBuffers(2, &meshData->vertexBufferID);
	glDeleteVertexArrays(1, &meshData->vertexArrayID);
}


/*
** Initialize a sprite batch using pointers to
** the storage for the instance buffer object.
** This storage can be written to, but not read from.
**
** This function will also orphan the buffer if
** it's still in use. The assumption is that this
** function will only be called once per batch.
*/
void meshBatchInit(meshBatch *const restrict batch){
	glBindBuffer(GL_ARRAY_BUFFER, meshInstanceBufferID);
	// Orphan the instance buffer if it's still in use.
	glBufferData(GL_ARRAY_BUFFER, MESH_INSTANCE_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	// Retrieve a pointer to the instance buffer's storage.
	batch->instances = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	batch->numVertices = 0;
}

void meshBatchDraw(const meshBatch *const restrict batch){
	if(batch->numInstances > 0){
		#warning "All of this stuff should be moved out eventually."
		#if 0
		#warning "This probably shouldn't be here."
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texFrame->tex->id);

		#warning "The uniforms should be moved out eventually."
		glUniform1fv(shader->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);
		#endif

		// Remember to unmap the buffer storage before rendering.
		glBindBuffer(GL_ARRAY_BUFFER, meshInstanceBufferID);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// Bind the global instance buffer and draw it!
		glBindVertexArray(batch->mesh->vertexArrayID);
		glDrawElementsInstanced(
			GL_TRIANGLES, batch->mesh->numIndices,
			GL_UNSIGNED_INT, NULL, batch->numInstances
		);
	}
}


void meshCleanup(){
	glDeleteBuffers(1, &meshInstanceBufferID);
}