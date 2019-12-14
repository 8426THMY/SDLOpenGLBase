#include "sprite.h"


#include "vertex.h"

#include "textureGroup.h"
#include "skeleton.h"


model spriteMdlDefault = {
	.name     = "sprite",
	.skele    = &skeleDefault,
	.texGroup = &texGroupDefault
};

sprite spriteDefault = {
	.mdl = &spriteMdlDefault
};


return_t spriteSetupDefault(){
	const vertex vertices[4] = {
		{
			.pos.x = -0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		}
	};

	const size_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};


	spriteMdlDefault.numIndices = sizeof(indices)/sizeof(*indices);

	// Generate a vertex array object for our model and bind it!
	glGenVertexArrays(1, &spriteMdlDefault.vertexArrayID);
	glBindVertexArray(spriteMdlDefault.vertexArrayID);
		// Generate a vertex buffer object for our vertex data and bind it!
		glGenBuffers(1, &spriteMdlDefault.vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteMdlDefault.vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

		// Generate a vertex buffer object for our indices and bind it!
		glGenBuffers(1, &spriteMdlDefault.indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteMdlDefault.indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);


		// Models will need these three vertex attributes!
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		// Tell OpenGL which data belongs to the vertex attributes!
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal));
		glVertexAttribIPointer(3, VERTEX_MAX_WEIGHTS, GL_UNSIGNED_INT, sizeof(vertex), (GLvoid *)offsetof(vertex, boneIDs));
		glVertexAttribPointer(4, VERTEX_MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, boneWeights));
	// Unbind the array object!
	glBindVertexArray(0);


	return(1);
}