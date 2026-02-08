#include "shaderProgramModel.h"


#define GLEW_STATIC
#include <GL/glew.h>


// This struct is used to get offsets into the uniform block.
typedef struct shaderModelData {
	mat3x4 boneStates[SKELETON_MAX_BONES];
} shaderModelData;


// This is the ID of the uniform buffer that
// stores model data across shader programs.
GLuint modelDataBufferID;


/*
** Get the locations of the various uniforms used by the
** model shader specified! We assume the program has already
** been bound, and leave it to the caller to unbind it.
*/
void shaderPrgModelInit(
	shaderProgramModel *const restrict shader,
	const unsigned int shaderID
){

	shader->modelDataID = glGetUniformBlockIndex(shaderID, "modelData");
	shader->uvOffsetsID = glGetUniformLocation(shaderID, "uvOffsets");

	// Bind the shared model data buffer to its block!
	glBindBufferBase(GL_UNIFORM_BUFFER, shader->modelDataID, modelDataBufferID);
	// Bind uniform variable "baseTex0" to the first texture mapping unit (GL_TEXTURE0)!
	glUniform1i(glGetUniformLocation(shaderID, "baseTex0"), 0);
}


return_t shaderPrgModelSetup(){
	glGenBuffers(1, &modelDataBufferID);
	glBufferData(
		GL_UNIFORM_BUFFER, sizeof(shaderModelData),
		NULL, GL_DYNAMIC_DRAW
	);

	return(1);
}

void shaderPrgModelLoadSharedUniforms(
	const mat3x4 *const restrict boneStates,
	const boneIndex numBones
){

	glBindBuffer(GL_UNIFORM_BUFFER, &modelDataBufferID);
	glBufferSubData(
		GL_UNIFORM_BUFFER, offsetof(shaderModelData, boneStates),
		numBones * sizeof(*boneStates), boneStates
	);
}

void shaderPrgModelCleanup(){
	glDeleteBuffers(1, &modelDataBufferID);
}