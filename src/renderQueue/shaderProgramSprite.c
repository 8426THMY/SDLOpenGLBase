#include "shaderProgramSprite.h"


#define GLEW_STATIC
#include <GL/glew.h>


// This struct is used to get offsets into the uniform block.
typedef struct shaderSpriteData {
	unsigned int sdfType;
	float padding0[3];
	vec4 sdfColour;
	vec4 sdfBackground;
} shaderSpriteData;


// This is the ID of the uniform buffer that
// stores sprite data across shader programs.
GLuint spriteDataBufferID;


/*
** Get the locations of the various uniforms used by the
** model shader specified! We assume the program has already
** been bound, and leave it to the caller to unbind it.
*/
void shaderPrgSpriteInit(
	shaderProgramSprite *const restrict shader,
	const unsigned int shaderID
){

	// Find the positions of our shader's uniform variables!
	shader->spriteDataID = glGetUniformBlockIndex(shaderID, "spriteData");

	// Bind the shared sprite data buffer to its block!
	glBindBufferBase(GL_UNIFORM_BUFFER, shader->spriteDataID, spriteDataBufferID);
	// Bind uniform variable "baseTex0" to the first texture mapping unit (GL_TEXTURE0)!
	glUniform1i(glGetUniformLocation(shaderID, "baseTex0"), 0);
}


return_t shaderPrgSpriteSetup(){
	glGenBuffers(1, &spriteDataBufferID);
	glBufferData(
		GL_UNIFORM_BUFFER, sizeof(shaderSpriteData),
		NULL, GL_DYNAMIC_DRAW
	);

	return(1);
}

void shaderPrgSpriteLoadSharedUniforms(
	const unsigned int sdfType,
	const vec4 *const restrict sdfColour,
	const vec4 *const restrict sdfBackground
){

	shaderSpriteData spriteData;
	spriteData.sdfType = sdfType;
	spriteData.sdfColour = sdfColour;
	spriteData.sdfBackground = sdfBackground;

	glBindBuffer(GL_UNIFORM_BUFFER, &spriteDataBufferID);
	glBufferSubData(
		GL_UNIFORM_BUFFER, 0,
		sizeof(shaderSpriteData), &spriteData
	);
}

void shaderPrgSpriteCleanup(){
	glDeleteBuffers(1, &spriteDataBufferID);
}