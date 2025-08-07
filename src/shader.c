#include "shader.h"


#include <stdio.h>

#include "memoryManager.h"


// Forward-declare any helper functions!
static void printShaderError(const GLuint shaderID);
static void printProgramError(const GLuint shaderID);


// Load the shader of type "shaderType" specified by "shaderPath" and return its ID!
GLuint shaderLoad(const char *const restrict shaderPath, const GLenum shaderType){
	FILE *shaderFile;

	// Load the shader file!
	if((shaderFile = fopen(shaderPath, "rb")) != NULL){
		GLuint shaderID = glCreateShader(shaderType);
		// Make sure we were able to create the new shader.
		if(glIsShader(shaderID)){
			// Check the length of the shader file!
			const size_t shaderFileSize = (fseek(shaderFile, 0, SEEK_END), ftell(shaderFile));
			char *const shaderSource = memoryManagerGlobalAlloc(shaderFileSize + 1);

			// If we were able to allocate memory
			// for the shader's code, compile it!
			if(shaderSource != NULL){
				GLint compiled;

				// Read the shader's source from the file!
				rewind(shaderFile);
				fread(shaderSource, sizeof(char), shaderFileSize, shaderFile);
				shaderSource[shaderFileSize] = '\0';
				fclose(shaderFile);

				// Compile the shader!
				glShaderSource(shaderID, 1, (const GLchar *const *)(&shaderSource), NULL);
				glCompileShader(shaderID);

				memoryManagerGlobalFree(shaderSource);


				glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
				// If we were able to compile the shader, return its ID!
				if(compiled){
					return(shaderID);
				}


				printf(
					"Shader could not be compiled!\n"
					"Path: %s\n", shaderPath
				);
				// If the compilation was unsuccessful, try to print the error message!
				printShaderError(shaderID);
			}else{
				printf(
					"Failed to allocate memory for shader file!\n"
					"Path: %s\n", shaderPath
				);
				fclose(shaderFile);
			}

			// Delete the shader if it couldn't be loaded.
			shaderDelete(shaderID);
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


	return(SHADER_INVALID_ID);
}

// Compile a shader program that uses the vertex and fragment shaders specified!
GLuint shaderLoadProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID){
	if(vertexShaderID != SHADER_INVALID_ID && fragmentShaderID != SHADER_INVALID_ID){
		GLint compiled;

		// Create the new shader program and attach our component shaders!
		const GLuint programID = glCreateProgram();
		if(!glIsProgram(programID)){
			return(SHADER_INVALID_ID);
		}
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		glLinkProgram(programID);
		glGetProgramiv(programID, GL_LINK_STATUS, &compiled);
		if(compiled){
			// We can detach the shaders now that
			// the program has been compiled.
			glDetachShader(programID, vertexShaderID);
			glDetachShader(programID, fragmentShaderID);

			return(programID);
		}

		printf("Shader program could not be compiled!\n");
		// If the compilation was unsuccessful, try to print the error message!
		printProgramError(programID);
		shaderDeleteProgram(programID);
	}

	return(SHADER_INVALID_ID);
}


void shaderDelete(const GLuint shaderID){
	glDeleteShader(shaderID);
}

void shaderDeleteProgram(const GLuint programID){
	glUseProgram(SHADER_INVALID_ID);
	glDeleteProgram(programID);
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