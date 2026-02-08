#include "shaderProgram.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include <stdio.h>

#include "shaderProgramModel.h"
#include "shaderProgramSprite.h"

#include "memoryManager.h"


// This struct is used to get offsets into the uniform block.
typedef struct shaderSharedData {
	mat4 vpMatrix;
} shaderSharedData;


GLenum shaderCompTypeLookup[SHADER_NUM_TYPES] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER
};

// This is the ID of the shader that's currently bound.
GLuint curShaderID;
// This is the ID of the uniform buffer that
// stores shared data across shader programs.
GLuint sharedDataBufferID;


// Forward-declare any helper functions!
static void printShaderError(const GLuint shaderID);
static void printProgramError(const GLuint shaderID);


// Load the shader component of the type specified!
void shaderCompInit(
	shaderComponent *const restrict shader,
	const shaderType type, const char *const restrict shaderPath
){

	FILE *shaderFile;

	shader->id   = SHADER_COMPONENT_INVALID_ID;
	shader->type = shaderTypeLookup[type];

	// Load the shader file!
	if((shaderFile = fopen(shaderPath, "rb")) != NULL){
		const GLuint shaderID = glCreateShader(shader->type);
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
			shader->id = shaderID;
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

void shaderCompDelete(shaderComponent *const restrict shader){
	glDeleteShader(shader->id);
}


/*
** Compile a shader program that uses the vertex and fragment
** shaders specified, and return whether or not we were successful!
*/
void shaderPrgInit(
	shaderProgram *const restrict shader, const shaderType type,
	const shaderComponent *const restrict vertexShader,
	const shaderComponent *const restrict fragmentShader
){

	shader->id           = SHADER_INVALID_ID;
	shader->sharedDataID = SHADER_INVALID_ID;
	shader->type         = type;

	if(
		vertexShader->id   != SHADER_COMPONENT_INVALID_ID &&
		fragmentShader->id != SHADER_COMPONENT_INVALID_ID
	){

		GLint compiled;

		// Create the new shader program and attach our component shaders!
		const GLuint programID = glCreateProgram();
		if(!glIsProgram(programID)){
			return;
		}
		glAttachShader(programID, vertexShader->id);
		glAttachShader(programID, fragmentShader->id);
	
		glLinkProgram(programID);
		glGetProgramiv(programID, GL_LINK_STATUS, &compiled);
		if(!compiled){
			printf("Shader program could not be compiled!\n");
			printProgramError(programID);
			glDeleteProgram(programID);
			return;
		}

		// We can detach the shaders now that
		// the program has been compiled.
		glDetachShader(programID, vertexShader->id);
		glDetachShader(programID, fragmentShader->id);


		glUseProgram(programID);

		shader->id = programID;
		// Bind the shared data uniform block.
		shader->sharedDataID = glGetUniformBlockIndex(programID, "sharedData");
		glBindBufferBase(GL_UNIFORM_BUFFER, shader->sharedDataID, sharedDataBufferID);

		// Initialize the specific shader program data!
		switch(type){
			case SHADER_PROGRAM_TYPE_MODEL:
				shaderPrgModelInit(&shader->data.modelShader, shader->id);
			break;
			case SHADER_PROGRAM_TYPE_SPRITE:
				shaderPrgSpriteInit(&shader->data.spriteShader, shader->id);
			break;
		}

		glUseProgram(SHADER_INVALID_ID);
	}
}

void shaderPrgBind(const shaderProgram *const restrict shader){
	if(shader->id != curShaderID){
		curShaderID = shader->id;
		glUseProgram(curShaderID);
	}
}

void shaderPrgDelete(shaderProgram *const restrict shader){
	// If the shader we're trying to delete is
	// currently bound, we should unbind it first.
	if(shader->id == curShaderID){
		curShaderID = SHADER_INVALID_ID;
		glUseProgram(SHADER_INVALID_ID);
	}
	glDeleteProgram(shader->id);
}


void shaderPrgSetup(){
	glGenBuffers(1, &sharedDataBufferID);
	glBufferData(
		GL_UNIFORM_BUFFER, sizeof(*vpMatrix),
		NULL, GL_DYNAMIC_DRAW
	);
}

void shaderPrgLoadSharedUniforms(const mat4 *const restrict vpMatrix){
	glBindBuffer(GL_UNIFORM_BUFFER, &sharedDataBufferID);
	glBufferData(
		GL_UNIFORM_BUFFER, offsetof(shaderSpriteData, vpMatrix),
		sizeof(*vpMatrix), vpMatrix
	);
}

void shaderPrgCleanup(){
	glDeleteBuffers(1, &sharedDataBufferID);
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