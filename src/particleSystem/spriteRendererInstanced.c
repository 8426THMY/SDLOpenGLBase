#include "spriteRendererInstanced.h"


#define GLEW_STATIC
#include <GL/glew.h>


#warning "It might be a good idea to double check that we're using buffer orphaning correctly."
/// https://community.khronos.org/t/vbos-strangely-slow/60109/33
/// https://community.khronos.org/t/buffer-orphaning-question-streaming-vbos/61550
#warning "Our current method should be fine, but we can do better by handling synchronization ourselves."


// We use a single state buffer plus
// orphaning for all instanced rendering.
GLuint spriteInstancedBufferID;
spriteInstanced spriteInstancedDefault;


#error "This is totally wrong! We need to bind a VAO before we can use VBOs."
#error "We already have a VAO for our meshes though, so what can we do?"
// Initialize the global instance buffer.
void spriteRendererInstancedSetup(){
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

	const spriteVertexIndex_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	// Generate the global instance buffer object!
	glGenBuffers(1, &spriteInstancedBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, spriteInstancedBufferID);
	// We write to this buffer dynamically when drawing sprites.
	glBufferData(GL_ARRAY_BUFFER, SPRITE_RENDERER_INSTANCED_INSTANCE_BUFFER_SIZE, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	spriteInstancedGenerateBuffers(
		&spriteInstancedDefault,
		vertices, sizeof(vertices)/sizeof(*vertices),
		indices, sizeof(indices)/sizeof(*indices)
	);
}


/*
** Generate buffers to hold our sprite data! Note that
** the state and UV buffers are not filled at this stage.
** We only fill them when we actually draw our sprite.
*/
void spriteInstancedGenerateBuffers(
	spriteInstanced *const restrict spriteData,
	const spriteVertex *const restrict vertices, const spriteVertexIndex_t numVertices,
	const spriteVertexIndex_t *const restrict indices, const spriteVertexIndex_t numIndices
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
		// Vertex normal.
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2, 3, GL_FLOAT, GL_FALSE,
			sizeof(spriteVertex), (GLvoid *)offsetof(spriteVertex, normal)
		);


		/*#warning "Should this be a global buffer that's part of the program?"
		// Generate a buffer object for our particle's states and bind it!
		glGenBuffers(1, &spriteData->instanceBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteData->instanceBufferID);
		// We write to this buffer dynamically when drawing sprites.
		glBufferData(GL_ARRAY_BUFFER, SPRITE_RENDERER_INSTANCED_INSTANCE_BUFFER_SIZE, NULL, GL_STREAM_DRAW);

		// Set up the vertex attributes that require this buffer!
		// Because the transformation state is a 4x4 matrix, it needs four vertex attributes.
		// Transformation state first row.
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[0])
		);
		glVertexAttribDivisor(3, 1);
		// Transformation state second row.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[1])
		);
		glVertexAttribDivisor(4, 1);
		// Transformation state third row.
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(
			5, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[2])
		);
		glVertexAttribDivisor(5, 1);
		// Transformation state fourth row.
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(
			6, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[3])
		);
		glVertexAttribDivisor(6, 1);
		// UV offsets.
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(
			7, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, uvOffsets)
		);
		glVertexAttribDivisor(7, 1);*/


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
** Initialize a sprite renderer using pointers to
** the storage for the instance buffer object.
** This storage can be written to, but not read from.
**
** Rather than orphaning the instance buffer, we try
** to use whatever memory is left in it. We don't yet
** know how big the batch is, so we can always orphan
** it later if we really have to.
*/
void spriteRendererInstancedInit(spriteRendererInstanced *const restrict instancedRenderer){
	//
}

void spriteRendererInstancedOffset(spriteRendererInstanced *const restrict instancedRenderer){
	//
}

return_t spriteRendererInstancedHasRoom(
	const spriteRendererInstanced *const restrict instancedRenderer, const size_t numInstances
){

	return(0);
}

void spriteRendererInstancedDraw(const meshBatch *const restrict instancedRenderer){
	if(instancedRenderer->numInstances > 0){
		// Bind the sprite we're using!
		glBindVertexArray(instancedRenderer->base->vertexArrayID);

		#warning "All of this stuff should be moved out eventually."
		#if 0
		#warning "This probably shouldn't be here."
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texFrame->tex->id);

		#warning "The uniforms should be moved out eventually."
		glUniform1fv(shader->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);
		#endif

		// Bind the instance buffer, and make
		// sure we unmap it before rendering.
		//glBindBuffer(GL_ARRAY_BUFFER, instancedRenderer->base->instanceBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteInstanceBufferID);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// Draw each instance of our sprite!
		glDrawElementsInstanced(
			GL_TRIANGLES, instancedRenderer->base->numIndices,
			GL_UNSIGNED_INT, NULL, instancedRenderer->numInstances
		);
	}
}

// Orphan the old instance buffer and allocate a new one.
void spriteRendererInstancedOrphan(spriteRendererInstanced *const restrict instancedRenderer){
	glBindVertexArray(instancedRenderer->base->vertexArrayID);
		//glBindBuffer(GL_ARRAY_BUFFER, instancedRenderer->base->instanceBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, spriteInstancedBufferID);
		// Orphan the vertex buffer if it's still in use.
		glBufferData(GL_ARRAY_BUFFER, SPRITE_RENDERER_BATCHED_VERTEX_BUFFER_SIZE, NULL, GL_STREAM_DRAW);

		// Set up the vertex attributes that require this buffer!
		// Because the transformation state is a 4x4 matrix, it needs four vertex attributes.
		// Transformation state first row.
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[0])
		);
		glVertexAttribDivisor(3, 1);
		// Transformation state second row.
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(
			4, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[1])
		);
		glVertexAttribDivisor(4, 1);
		// Transformation state third row.
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(
			5, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[2])
		);
		glVertexAttribDivisor(5, 1);
		// Transformation state fourth row.
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(
			6, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, state.m[3])
		);
		glVertexAttribDivisor(6, 1);
		// UV offsets.
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(
			7, 4, GL_FLOAT, GL_FALSE,
			sizeof(spriteInstancedData), (GLvoid *)offsetof(spriteInstancedData, uvOffsets)
		);
		glVertexAttribDivisor(7, 1);
	glBindVertexArray(0);

	// Retrieve a pointer to the instance buffer's storage.
	instancedRenderer->instances = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	instancedRenderer->numInstances = 0;
}


void spriteRendererInstancedCleanup(){
	glDeleteBuffers(1, &spriteInstanceBufferID);
}