#include "shader.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include <stdio.h>

#include "bufferObject.h"

#include "memoryManager.h"


GLenum shaderCompTypeTable[SHADER_NUM_TYPES] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER
};

const char *shaderBlockName[SHADER_NUM_BLOCKS] = {
	"sharedData",
	"skeletonData"
};
const GLsizeiptr shaderBlockSize[SHADER_NUM_BLOCKS] = {
	sizeof(shaderBlockSharedData),
	sizeof(shaderBlockSkeletonData)
};
GLuint shaderBlockBufferID[SHADER_NUM_BLOCKS];

// This is the I.D. of the shader that's currently bound.
GLuint curShaderID;


// Forward-declare any helper functions!
static void printShaderError(const GLuint shaderID);
static void printProgramError(const GLuint shaderID);


// Load the shader component of the type specified!
void shaderCompInit(
	shaderComponent *const restrict shaderComp,
	const shaderComponentType type, const char *const restrict shaderPath
){

	FILE *shaderFile;

	shaderComp->id   = SHADER_COMPONENT_INVALID_ID;
	shaderComp->type = shaderCompTypeTable[type];

	// Load the shader file!
	if((shaderFile = fopen(shaderPath, "rb")) != NULL){
		const GLuint shaderID = glCreateShader(shaderComp->type);
		// Make sure we were able to create the new shader.
		if(glIsShader(shaderID)){
			// Check the length of the shader file!
			const size_t shaderFileSize = (
				fseek(shaderFile, 0, SEEK_END), ftell(shaderFile)
			);
			char *const shaderSource = memoryManagerGlobalAlloc(shaderFileSize + 1);
			GLint compiled;

			if(shaderSource == NULL){
				printf(
					"Failed to allocate memory for shader file!\n"
					"Path: %s\n", shaderPath
				);
				fclose(shaderFile);
				// Delete the shader if it couldn't be loaded.
				shaderDelete(shaderID);
			}

			// If we were able to allocate memory for
			// the shader's code, read it from the file!
			rewind(shaderFile);
			fread(shaderSource, sizeof(char), shaderFileSize, shaderFile);
			shaderSource[shaderFileSize] = '\0';
			fclose(shaderFile);

			// Compile the shader!
			glShaderSource(shaderID, 1, (const GLchar *const *)(&shaderSource), NULL);
			glCompileShader(shaderID);
			memoryManagerGlobalFree(shaderSource);

			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
			if(!compiled){
				printf(
					"Shader could not be compiled!\n"
					"Path: %s\n", shaderPath
				);
				printShaderError(shaderID);
				// Delete the shader if it couldn't be loaded.
				shaderDelete(shaderID);
			}

			// The shader was successfully loaded, so keep its ID!
			shaderComp->id = shaderID;
		}else{
			printf(
				"Failed to create shader!\n"
				"Path: %s\n", shaderPath
			);
			fclose(shaderFile);
		}
	}else{
		printf(
			"Unable to load shader file!\n"
			"Path: %s\n", shaderPath
		);
	}
}

void shaderCompDelete(shaderComponent *const restrict shaderComp){
	glDeleteShader(shaderComp->id);
}


void shaderUniformBindFloat(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const float *const restrict data
){

	glUniform1fv(uniform->id, count, (GLfloat *)data);
}

void shaderUniformBindVec2(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const vec2 *const restrict data
){

	glUniform2fv(uniform->id, count, (GLfloat *)data);
}

void shaderUniformBindVec3(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const vec3 *const restrict data
){

	glUniform3fv(uniform->id, count, (GLfloat *)data);
}

void shaderUniformBindVec4(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const vec4 *const restrict data
){

	glUniform4fv(uniform->id, count, (GLfloat *)data);
}

void shaderUniformBindMat3(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const mat3 *const restrict data
){

	glUniformMatrix3fv(uniform->id, count, GL_FALSE, (GLfloat *)data);
}

void shaderUniformBindMat3x4(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const mat3x4 *const restrict data
){

	glUniformMatrix4x3fv(uniform->id, count, GL_FALSE, (GLfloat *)data);
}

void shaderUniformBindMat4(
	const shaderUniform *const restrict uniform,
	const size_t count,
	const mat4 *const restrict data
){

	glUniformMatrix4fv(uniform->id, count, GL_FALSE, (GLfloat *)data);
}


/*
** Compile a shader program that uses the vertex and fragment
** shaders specified, and return whether or not we were successful!
*/
unsigned int shaderInit(
	shader *const restrict prg, const shaderType type,
	const shaderComponent *const restrict vertexShader,
	const shaderComponent *const restrict fragmentShader
){

	prg->id = SHADER_INVALID_ID;
	if(
		vertexShader->id   != SHADER_COMPONENT_INVALID_ID &&
		fragmentShader->id != SHADER_COMPONENT_INVALID_ID
	){

		GLint compiled;
		unsigned int i;

		// Create the new shader program and attach our component shaders!
		const GLuint shaderID = glCreateProgram();
		if(!glIsProgram(shaderID)){
			return;
		}
		glAttachShader(shaderID, vertexShader->id);
		glAttachShader(shaderID, fragmentShader->id);
	
		glLinkProgram(shaderID);
		glGetProgramiv(shaderID, GL_LINK_STATUS, &compiled);
		if(!compiled){
			printf("Shader program could not be compiled!\n");
			printProgramError(shaderID);
			glDeleteProgram(shaderID);
			return;
		}

		// We can detach the shaders now that
		// the program has been compiled.
		glDetachShader(shaderID, vertexShader->id);
		glDetachShader(shaderID, fragmentShader->id);


		prg->id = shaderID;
		curShaderID = shaderID;
		glUseProgram(curShaderID);

		// Bind all of the shared uniform blocks that this shader uses.
		for(i = 0; i < SHADER_NUM_BLOCKS; ++i){
			const unsigned int curBlockID = glGetUniformBlockIndex(
				curShaderID, shaderBlockName[i]
			);
			if(curBlockID != GL_INVALID_INDEX){
				glBindBufferBase(
					GL_UNIFORM_BUFFER, curBlockID, shaderBlockBufferID[i]
				);
			}
		}
	}
}

void shaderBind(const shader *const restrict prg){
	if(prg->id != curShaderID){
		curShaderID = prg->id;
		glUseProgram(curShaderID);
	}
}

void shaderDelete(shader *const restrict prg){
	// If the shader we're trying to delete is
	// currently bound, we should unbind it first.
	if(curShaderID == prg->id){
		curShaderID = SHADER_INVALID_ID;
		glUseProgram(SHADER_INVALID_ID);
	}
	glDeleteProgram(prg->id);
}


void shaderSetup(){
	unsigned int i;
	glGenBuffers(SHADER_NUM_BLOCKS, shaderBlockBufferID);
	// Allocate memory for each shared uniform block.
	for(i = 0; i < SHADER_NUM_BLOCKS; ++i){
		glBufferData(
			GL_UNIFORM_BUFFER, shaderBlockSize[i], NULL, GL_DYNAMIC_DRAW
		);
	}
}

void shaderLoadBlockData(
	const unsigned int blockID,
	const size_t offset,
	const size_t bytes,
	const void *const restrict data
){

	bufferObjectBindUniformBuffer(shaderBlockBufferID[blockID]);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, bytes, data);
}

void shaderCleanup(){
	unsigned int i;
	for(i = 0; i < SHADER_NUM_BLOCKS; ++i){
		bufferObjectDeleteUniformBuffer(shaderBlockBufferID[i]);
	}
}


static void printShaderError(const GLuint shaderID){
	GLint infoLogLength;

	// If the compilation was unsuccessful, try to print the error message!
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	// According to the documentation, the length should be 0
	// if there is no error despite normally counting the NULL
	// terminator. When using Intel integrated graphics, however,
	// I've found that the size is 1 when there is no error.
	if(infoLogLength > 1){
		GLchar *shaderError = memoryManagerGlobalAlloc(infoLogLength);
		if(shaderError == NULL){
			/** MALLOC FAILED **/
		}
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderError);

		printf("%s", shaderError);
		memoryManagerGlobalFree(shaderError);
	}
}

static void printProgramError(const GLuint shaderID){
	GLint infoLogLength;

	// If the compilation was unsuccessful, try to print the error message!
	glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	// According to the documentation, the length should be 0
	// if there is no error despite normally counting the NULL
	// terminator. When using Intel integrated graphics, however,
	// I've found that the size is 1 when there is no error.
	if(infoLogLength > 1){
		GLchar *programError = memoryManagerGlobalAlloc(infoLogLength);
		if(programError == NULL){
			/** MALLOC FAILED **/
		}
		glGetProgramInfoLog(shaderID, infoLogLength, NULL, programError);

		printf("%s", programError);
		memoryManagerGlobalFree(programError);
	}
}