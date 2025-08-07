#include "shaderSprite.h"


#include "shader.h"


return_t shaderSpriteInit(shaderSprite *const restrict shader, const GLuint programID){
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