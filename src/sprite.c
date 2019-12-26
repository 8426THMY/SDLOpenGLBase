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

		// Set up the vertex buffer's attribute!
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);


		#warning "Should this be a global buffer that's part of the program?"
		// Generate a buffer object for our particle's states and bind it!
		glGenBuffers(1, &spriteData->stateBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->stateBufferID);
		// We write to this buffer dynamically when drawing sprites.
		glBufferData(GL_ARRAY_BUFFER, sizeof(spriteState) * SPRITE_MAX_INSTANCES, NULL, GL_DYNAMIC_DRAW);

		// Set up the state buffer's attribute! Because we're using a 4x4 matrix
		// for the transformation state, we need to use four vertex attributes.
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[0]));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[1]));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[2]));
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[3]));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(spriteState), (GLvoid *)offsetof(spriteState, uvOffsets));
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);


		// Generate a buffer object for our indices and bind it!
		glGenBuffers(1, &spriteData->indexBufferID);
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