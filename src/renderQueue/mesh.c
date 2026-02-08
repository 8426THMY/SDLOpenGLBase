#include "mesh.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "sprite.h"
#include "model.h"


#define MESH_INVALID_VAO_ID 0


#warning "We'll probably need to move the particle rendering stuff here so this works."
#warning "We can make both instanced and batched drawing use the same buffer."
GLuint curVertexArrayID = MESH_INVALID_VAO_ID;


#warning "We should use a global vertex buffer with orphaning for instancing."
#warning "In fact, we can probably just reuse the code from spriteRendererInstanced!"


// Generate vertex and index buffers to hold our sprite data!
void meshSpriteInit(
	mesh *const restrict meshData,
	const spriteVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
){

	// Generate a vertex array object for our sprite and bind it!
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
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, pos)
			);
			// Vertex UVs.
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1, 2, GL_FLOAT, GL_FALSE,
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, uv)
			);
		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		// Bind the buffer to the vertex array object.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(MESH_INVALID_VAO_ID);

	meshData->numIndices = numIndices;
}

/*
** Generate vertex and index buffers to hold our sprite data!
** Sprites initialized using this function can be instanced.
** All meshes share a single vertex buffer object for their
** per-instance data, which we orphan as necessary.
*/
void meshSpriteInitInstanced(
	mesh *const restrict meshData,
	const spriteVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
){

	// Generate a vertex array object for our sprite and bind it!
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
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, pos)
			);
			// Vertex UVs.
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1, 2, GL_FLOAT, GL_FALSE,
				sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, uv)
			);
		#if 0
		// Bind the global mesh instance buffer.
		glBindBuffer(GL_ARRAY_BUFFER, instanceDataBufferID);
			// Set up the vertex attributes that require this buffer!
			// Because the transformation state is a 3x4 matrix, it needs four vertex attributes.
			// Transformation state first column.
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[0])
			);
			glVertexAttribDivisor(2, 1);
			// Transformation state second column.
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(
				3, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[1])
			);
			glVertexAttribDivisor(3, 1);
			// Transformation state third column.
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(
				4, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[2])
			);
			glVertexAttribDivisor(4, 1);
			// Transformation state fourth column.
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(
				5, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[3])
			);
			glVertexAttribDivisor(5, 1);
			// UV offsets.
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(
				6, 4, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, uvOffsets)
			);
			glVertexAttribDivisor(6, 1);
		#endif
		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		// Bind the buffer to the vertex array object.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(MESH_INVALID_VAO_ID);

	meshData->numIndices = numIndices;
}

// Generate vertex and index buffers to hold our model data!
void meshModelInit(
	mesh *const restrict meshData,
	const modelVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
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
				3, MODEL_VERTEX_MAX_BONE_WEIGHTS, GL_UNSIGNED_INT,
				sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, boneIDs)
			);
			// Vertex bone weights.
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(
				4, MODEL_VERTEX_MAX_BONE_WEIGHTS, GL_FLOAT, GL_FALSE,
				sizeof(meshVertex), (GLvoid *)offsetof(meshVertex, boneWeights)
			);
		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(MESH_INVALID_VAO_ID);

	meshData->numIndices = numIndices;
}

/*
** Generate vertex and index buffers to hold our model data!
** Models initialized using this function can be instanced.
** All meshes share a single vertex buffer object for their
** per-instance data, which we orphan as necessary.
*/
void meshModelInitInstanced(
	mesh *const restrict meshData,
	const modelVertex *const restrict vertices, const meshVertexIndex numVertices,
	const meshVertexIndex *const restrict indices, const meshVertexIndex numIndices
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
				sizeof(modelVertex), (GLvoid *)offsetof(modelVertex, pos)
			);
			// Vertex UVs.
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1, 2, GL_FLOAT, GL_FALSE,
				sizeof(modelVertex), (GLvoid *)offsetof(modelVertex, uv)
			);
			// Vertex normals.
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE,
				sizeof(modelVertex), (GLvoid *)offsetof(modelVertex, normal)
			);
			// Vertex bone IDs.
			glEnableVertexAttribArray(3);
			glVertexAttribIPointer(
				3, MODEL_VERTEX_MAX_BONE_WEIGHTS, GL_UNSIGNED_INT,
				sizeof(modelVertex), (GLvoid *)offsetof(modelVertex, boneIDs)
			);
			// Vertex bone weights.
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(
				4, MODEL_VERTEX_MAX_BONE_WEIGHTS, GL_FLOAT, GL_FALSE,
				sizeof(modelVertex), (GLvoid *)offsetof(modelVertex, boneWeights)
			);
		#if 0
		// Bind the global mesh instance buffer.
		glBindBuffer(GL_ARRAY_BUFFER, instanceDataBufferID);
			// Set up the vertex attributes that require this buffer!
			// Because the transformation state is a 3x4 matrix, it needs four vertex attributes.
			// Transformation state first column.
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[0])
			);
			glVertexAttribDivisor(2, 1);
			// Transformation state second column.
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(
				3, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[1])
			);
			glVertexAttribDivisor(3, 1);
			// Transformation state third column.
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(
				4, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[2])
			);
			glVertexAttribDivisor(4, 1);
			// Transformation state fourth column.
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(
				5, 3, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, state.m[3])
			);
			glVertexAttribDivisor(5, 1);
			// UV offsets.
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(
				6, 4, GL_FLOAT, GL_FALSE,
				sizeof(meshInstance), (GLvoid *)offsetof(meshInstance, uvOffsets)
			);
			glVertexAttribDivisor(6, 1);
		#endif
		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &meshData->indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(MESH_INVALID_VAO_ID);

	meshData->numIndices = numIndices;
}


/*
** Draw a mesh. We assume that all of the
** textures and uniforms have been bound.
*/
void meshDraw(const mesh *const restrict meshData){
	if(meshData->vertexArrayID != curVertexArrayID){
		curVertexArrayID = meshData->vertexArrayID;
		glBindVertexArray(curVertexArrayID);
	}
	glDrawElements(GL_TRIANGLES, meshData->numIndices, GL_UNSIGNED_INT, NULL);
}

/*
** Draw an instanced mesh. We assume that all
** of the textures and uniforms have been bound.
*/
void meshDrawInstanced(const mesh *const restrict meshData, const size_t numInstances){
	if(meshData->vertexArrayID != curVertexArrayID){
		curVertexArrayID = meshData->vertexArrayID;
		glBindVertexArray(curVertexArrayID);
	}
	#warning "We probably should either unmap buffers or upload instance data here."
	glDrawElementsInstanced(GL_TRIANGLES, meshData->numIndices, GL_UNSIGNED_INT, NULL, numInstances);
}


void meshDelete(mesh *const restrict meshData){
	glDeleteVertexArrays(1, &meshData->vertexArrayID);
	glDeleteBuffers(1, &meshData->vertexBufferID);
	glDeleteBuffers(1, &meshData->indexBufferID);
}


return_t meshSetup(){
	#warning "We should allocate the global instance data buffer here."
	return(1);
}

void meshCleanup(){
	#warning "We should delete the global instance data buffer here."
}