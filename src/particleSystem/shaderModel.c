#include "shaderModel.h"


#include "shader.h"


return_t shaderModelInit(shaderModel *const restrict shader, const GLuint programID){
	// Make sure the shader program was loaded successfully.
	if(programID == SHADER_INVALID_ID){
		return(0);
	}


	shader->programID = programID;

	// Enable the shader we just loaded!
	glUseProgram(programID);

	// Find the positions of our shader's uniform variables!
	shader->vpMatrixID   = glGetUniformLocation(programID, "vpMatrix");
	shader->uvOffsetsID  = glGetUniformLocation(programID, "uvOffsets");
	shader->boneStatesID = glGetUniformLocation(programID, "boneStates");
	// Bind uniform variable "baseTex0" to the first texture mapping unit (GL_TEXTURE0)!
	glUniform1i(glGetUniformLocation(programID, "baseTex0"), 0);

	// Unbind the shader!
	glUseProgram(SHADER_INVALID_ID);


	return(1);
}