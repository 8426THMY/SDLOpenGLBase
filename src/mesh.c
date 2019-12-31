#include "mesh.h"


#define MESH_INVALID_BUFFER_ID 0


mesh meshDefault;


void meshInit(mesh *meshData){
	meshData->vertexArrayID  = MESH_INVALID_BUFFER_ID;
	meshData->vertexBufferID = MESH_INVALID_BUFFER_ID;

	meshData->indexBufferID  = MESH_INVALID_BUFFER_ID;
	meshData->numIndices = 0;
}


// Generate vertex and index buffers to hold our mesh data!
void meshGenerateBuffers(mesh *meshData, const vertex *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices){
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
return_t vertexDifferent(const vertex *v1, const vertex *v2){
	return(memcmp(v1, v2, sizeof(*v2)) != 0);
}

// Return whether or not two meshes are different.
return_t meshDifferent(const mesh *m1, const mesh *m2){
	return(m1->vertexArrayID != m2->vertexArrayID);
}


return_t meshSetupDefault(){
	const vertex vertices[20] = {
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		}
	};

	const size_t indices[36] = {
		 0,  1,  2,
		 3,  4,  5,
		 6,  7,  8,
		 9, 10, 11,
		 4, 12, 13,
		14,  9, 15,
		 0, 16,  1,
		 3, 17,  4,
		 6, 18,  7,
		 9, 13, 10,
		 4, 19, 12,
		14,  5,  9
	};

	meshGenerateBuffers(&meshDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}


void meshDelete(mesh *meshData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our mesh structure to have no padding.
	glDeleteBuffers(2, &meshData->vertexBufferID);
	glDeleteVertexArrays(1, &meshData->vertexArrayID);
}