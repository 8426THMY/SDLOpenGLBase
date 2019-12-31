#include "sprite.h"


#define SPRITE_INVALID_BUFFER_ID 0


sprite spriteDefault;


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
void spriteGenerateBuffers(sprite *spriteData, const vec3 *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices){
	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &spriteData->vertexArrayID);
	glBindVertexArray(spriteData->vertexArrayID);
		// Generate a buffer object for our vertex data and bind it!
		glGenBuffers(1, &spriteData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->vertexBufferID);
		// Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * numVertices, vertices, GL_STATIC_DRAW);

		// Set up the vertex array object attributes that require this buffer!
		#warning "We should have vertex UVs for sprites."
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);


		#warning "Should this be a global buffer that's part of the program?"
		// Generate a buffer object for our particle's states and bind it!
		glGenBuffers(1, &spriteData->stateBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->stateBufferID);
		// We write to this buffer dynamically when drawing sprites.
		glBufferData(GL_ARRAY_BUFFER, sizeof(spriteState) * SPRITE_MAX_INSTANCES, NULL, GL_STREAM_DRAW);

		// Set up the vertex array object attributes that require this buffer!
		// Because the transformation state is a 4x4 matrix, it needs four vertex attributes.
		// Transformation state first row.
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[0]));
		glVertexAttribDivisor(1, 1);
		// Transformation state second row.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[1]));
		glVertexAttribDivisor(2, 1);
		// Transformation state third row.
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[2]));
		glVertexAttribDivisor(3, 1);
		// Transformation state fourth row.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[3]));
		glVertexAttribDivisor(4, 1);
		// UV offsets.
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, uvOffsets));
		glVertexAttribDivisor(5, 1);


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

// Return whether or not two sprites are different.
return_t spriteDifferent(const sprite *s1, const sprite *s2){
	return(s1->vertexArrayID != s2->vertexArrayID);
}


return_t spriteSetupDefault(){
	const vec3 vertices[4] = {
		{.x = -0.5f, .y =  0.5f, .z = 0.f},
		{.x = -0.5f, .y = -0.5f, .z = 0.f},
		{.x =  0.5f, .y = -0.5f, .z = 0.f},
		{.x =  0.5f, .y =  0.5f, .z = 0.f}
	};

	const size_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	spriteGenerateBuffers(&spriteDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}


void spriteDelete(sprite *spriteData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our sprite structure to have no padding.
	glDeleteBuffers(3, &spriteData->vertexBufferID);
	glDeleteVertexArrays(1, &spriteData->vertexArrayID);
}