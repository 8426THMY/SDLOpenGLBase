#include "sprite.h"


#define SPRITE_INVALID_BUFFER_ID 0


sprite g_spriteDefault;


void spriteInit(sprite *spriteData){
	spriteData->vertexArrayID  = SPRITE_INVALID_BUFFER_ID;
	spriteData->vertexBufferID = SPRITE_INVALID_BUFFER_ID;

	spriteData->stateBufferID  = SPRITE_INVALID_BUFFER_ID;

	spriteData->indexBufferID  = SPRITE_INVALID_BUFFER_ID;
	spriteData->numIndices = 0;
}


/*
** Generate buffers to hold our sprite data! Note that
** the state and UV buffers are not filled at this stage.
** We only fill them when we actually draw our sprite.
*/
void spriteGenerateBuffers(sprite *spriteData, const spriteVertex *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices){
	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &spriteData->vertexArrayID);
	glBindVertexArray(spriteData->vertexArrayID);
		// Generate a buffer object for our vertex data and bind it!
		glGenBuffers(1, &spriteData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * numVertices, vertices, GL_STATIC_DRAW);

		// Set up the vertex array object attributes that require this buffer!
		// Vertex positions.
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, pos));
		// Vertex UVs.
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, uv));


		#warning "Should this be a global buffer that's part of the program?"
		// Generate a buffer object for our particle's states and bind it!
		glGenBuffers(1, &spriteData->stateBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->stateBufferID);
		// We write to this buffer dynamically when drawing sprites.
		glBufferData(GL_ARRAY_BUFFER, sizeof(spriteState) * SPRITE_MAX_INSTANCES, NULL, GL_STREAM_DRAW);

		// Set up the vertex array object attributes that require this buffer!
		// Because the transformation state is a 4x4 matrix, it needs four vertex attributes.
		// Transformation state first row.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[0]));
		glVertexAttribDivisor(2, 1);
		// Transformation state second row.
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[1]));
		glVertexAttribDivisor(3, 1);
		// Transformation state third row.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[2]));
		glVertexAttribDivisor(4, 1);
		// Transformation state fourth row.
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[3]));
		glVertexAttribDivisor(5, 1);
		// UV offsets.
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, uvOffsets));
		glVertexAttribDivisor(6, 1);


		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &spriteData->indexBufferID);
		// Bind the buffer to the vertex array object.
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteData->indexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices) * numIndices, indices, GL_STATIC_DRAW);
	// Unbind the array object!
	glBindVertexArray(0);

	spriteData->numIndices = numIndices;
}


/*
** Return whether or not two vertices are different.
** This works so long as our vertices have no padding.
*/
return_t spriteVertexDifferent(const spriteVertex *v1, const spriteVertex *v2){
	return(memcmp(v1, v2, sizeof(*v2)) != 0);
}

// Return whether or not two sprites are different.
return_t spriteDifferent(const sprite *s1, const sprite *s2){
	return(s1->vertexArrayID != s2->vertexArrayID);
}


return_t spriteSetupDefault(){
	const spriteVertex vertices[4] = {
		{
			.pos.x = -0.5f, .pos.y =  0.5f, .pos.z = 0.f,
			.uv.x = 0.f, 0.f
		},
		{
			.pos.x = -0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 0.f, 1.f
		},
		{
			.pos.x =  0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 1.f, 1.f
		},
		{
			.pos.x =  0.5f, .pos.y =  0.5f, .pos.z = 0.f,
			.uv.x = 1.f, 0.f
		}
	};

	const size_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	spriteGenerateBuffers(&g_spriteDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}


void spriteDelete(sprite *spriteData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our sprite structure to have no padding.
	glDeleteBuffers(3, &spriteData->vertexBufferID);
	glDeleteVertexArrays(1, &spriteData->vertexArrayID);
}