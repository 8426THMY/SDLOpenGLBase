#include "sprite.h"


#include "shader.h"


#define SPRITE_INVALID_BUFFER_ID 0


void spriteInit(sprite *const restrict spriteData){
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
void spriteGenerateBuffers(
	sprite *const restrict spriteData,
	const spriteVertex *const restrict vertices, const spriteVertexIndex numVertices,
	const spriteVertexIndex *const restrict indices, const spriteVertexIndex numIndices
){

	// Generate a vertex array object for our sprite and bind it!
	glGenVertexArrays(1, &spriteData->vertexArrayID);
	glBindVertexArray(spriteData->vertexArrayID);
		// Generate a buffer object for our vertex data and bind it!
		glGenBuffers(1, &spriteData->vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->vertexBufferID);
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


		#warning "Should this be a global buffer that's part of the program?"
		// Generate a buffer object for our particle's states and bind it!
		glGenBuffers(1, &spriteData->stateBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->stateBufferID);
		// We write to this buffer dynamically when drawing sprites.
		glBufferData(GL_ARRAY_BUFFER, sizeof(spriteState) * SPRITE_MAX_INSTANCES, NULL, GL_DYNAMIC_DRAW);

		// Set up the vertex attributes that require this buffer!
		// Because the transformation state is a 3x4 matrix, it needs four vertex attributes.
		// Transformation state first column.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2, 3, GL_FLOAT, GL_FALSE,
			sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[0])
		);
		glVertexAttribDivisor(2, 1);
		// Transformation state second column.
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3, 3, GL_FLOAT, GL_FALSE,
			sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[1])
		);
		glVertexAttribDivisor(3, 1);
		// Transformation state third column.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4, 3, GL_FLOAT, GL_FALSE,
			sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[2])
		);
		glVertexAttribDivisor(4, 1);
		// Transformation state fourth column.
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(
			5, 3, GL_FLOAT, GL_FALSE,
			sizeof(spriteState), (GLvoid *)offsetof(spriteState, state.m[3])
		);
		glVertexAttribDivisor(5, 1);
		// UV offsets.
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(
			6, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteState), (GLvoid *)offsetof(spriteState, uvOffsets)
		);
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
return_t spriteVertexDifferent(const spriteVertex *const restrict v1, const spriteVertex *const restrict v2){
	return(memcmp(v1, v2, sizeof(*v2)) != 0);
}

// Return whether or not two sprites are different.
return_t spriteDifferent(const sprite *const restrict s1, const sprite *const restrict s2){
	return(s1->vertexArrayID != s2->vertexArrayID);
}


void spriteDelete(sprite *const restrict spriteData){
	// This works because the buffer IDs are in consecutive memory,
	// though it does require our sprite structure to have no padding.
	glDeleteBuffers(3, &spriteData->vertexBufferID);
	glDeleteVertexArrays(1, &spriteData->vertexArrayID);
}


return_t spriteShaderInit(spriteShader *const restrict shader, const GLuint programID){
	// Make sure the shader program was loaded successfully.
	if(programID == SHADER_INVALID_ID){
		return(0);
	}


	shader->programID = programID;

	// Enable the shader we just loaded!
	glUseProgram(programID);

	// Find the positions of our shader's uniform variables!
	shader->vpMatrixID      = glGetUniformLocation(programID, "vpMatrix");
	shader->sdfTypeID       = glGetUniformLocation(programID, "sdfType");
	shader->sdfColourID     = glGetUniformLocation(programID, "sdfColour");
	shader->sdfBackgroundID = glGetUniformLocation(programID, "sdfBackground");
	// Bind uniform variable "baseTex0" to the first texture mapping unit (GL_TEXTURE0)!
	glUniform1i(glGetUniformLocation(programID, "baseTex0"), 0);

	// Unbind the shader!
	glUseProgram(SHADER_INVALID_ID);


	return(1);
}