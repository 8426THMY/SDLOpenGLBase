#include "shader.h"


#define SHADER_INVALID_ID 0


#include <stdio.h>
#include <stdlib.h>

#include "memoryManager.h"


void shaderInit(shader *shaderPrg){
	shaderPrg->programID = SHADER_INVALID_ID;

	shaderPrg->mvpMatrixID  = SHADER_INVALID_ID;
	shaderPrg->uvOffsetsID  = SHADER_INVALID_ID;
	shaderPrg->boneStatesID = SHADER_INVALID_ID;
}


// Load the shader of type "shaderType" specified by "shaderPath" and return its ID!
GLuint shaderLoad(const char *shaderPath, const GLenum shaderType){
	FILE *shaderFile;

	// Load the shader file!
	if((shaderFile = fopen(shaderPath, "rb")) != NULL){
		GLuint shaderID = glCreateShader(shaderType);
		// Make sure we were able to create the new shader.
		if(shaderID != SHADER_INVALID_ID){
			// Store the code!
			fseek(shaderFile, 0, SEEK_END);
			const long shaderFileSize = ftell(shaderFile);
			rewind(shaderFile);

			char *shaderSource = memoryManagerGlobalAlloc(shaderFileSize + 1);
			// If we were able to allocate memory
			// for the shader's code, compile it!
			if(shaderSource != NULL){
				GLint compiled;
				GLint infoLogLength;


				fread(shaderSource, 1, shaderFileSize, shaderFile);
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


				printf("Shader could not be compiled!\n");

				// If the compilation was unsuccessful, try to print the error message!
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
				// According to the documentation, the length should be 0
				// if there is no error despite normally counting the null
				// terminator. When using Intel integrated graphics, however,
				// I've found that the size is 1 when there is no error.
				if(infoLogLength > 1){
					GLchar shaderError[infoLogLength + 1];
					glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderError);

					printf("%s\n", shaderError);
				}
			}else{
				printf("Failed to allocate memory for shader file!\n"
					   "Path: %s\n", shaderPath);
				fclose(shaderFile);
			}

			// Delete the shader if it couldn't be loaded.
			glDeleteShader(shaderID);
		}else{
			printf("Failed to create shader!\n"
			       "Path: %s\n", shaderPath);
		}
	}else{
		printf("Unable to load shader file!\n"
		       "Path: %s\n", shaderPath);
		fclose(shaderFile);
	}


	return(SHADER_INVALID_ID);
}

return_t shaderLoadProgram(shader *shaderPrg, const char *vertexPath, const char *fragmentPath){
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
	GLint compiled;


	// Load the program's shaders!
	if((vertexShaderID = shaderLoad(vertexPath, GL_VERTEX_SHADER)) == SHADER_INVALID_ID){
		return(0);
	}
	if((fragmentShaderID = shaderLoad(fragmentPath, GL_FRAGMENT_SHADER)) == SHADER_INVALID_ID){
		return(0);
	}


	// Create the new shader program and attach our component shaders!
	shaderPrg->programID = glCreateProgram();
	glAttachShader(shaderPrg->programID, vertexShaderID);
	glAttachShader(shaderPrg->programID, fragmentShaderID);
	glLinkProgram(shaderPrg->programID);

	glGetProgramiv(shaderPrg->programID, GL_LINK_STATUS, &compiled);
	if(!compiled){
		GLint infoLogLength;


		printf("Shader program could not be compiled!\n");

		// If the compilation was unsuccessful, try to print the error message!
		glGetProgramiv(shaderPrg->programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		// According to the documentation, the length should be 0
		// if there is no error despite normally counting the null
		// terminator. When using Intel integrated graphics, however,
		// I've found that the size is 1 when there is no error.
		if(infoLogLength > 1){
			char programError[infoLogLength + 1];
			glGetShaderInfoLog(shaderPrg->programID, infoLogLength, NULL, programError);

			printf("%s\n", programError);
		}

		shaderPrg->programID = SHADER_INVALID_ID;


		return(0);
	}


	// We can delete the shaders now that
	// they've been linked to the program.
	glDetachShader(shaderPrg->programID, vertexShaderID);
	glDetachShader(shaderPrg->programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);


	// Enable the shader we just loaded!
	glUseProgram(shaderPrg->programID);

	// Find the positions of our shader's uniform variables (pretty sure they're positioned in alphabetical order)!
	shaderPrg->mvpMatrixID  = glGetUniformLocation(shaderPrg->programID, "mvpMatrix");
	shaderPrg->uvOffsetsID  = glGetUniformLocation(shaderPrg->programID, "uvOffsets");
	shaderPrg->boneStatesID = glGetUniformLocation(shaderPrg->programID, "boneStates");

	// Bind uniform variable "baseTex0" to the first texture mapping unit (GL_TEXTURE0)!
	glUniform1i(glGetUniformLocation(shaderPrg->programID, "baseTex0"), 0);


	return(1);
}


void shaderDelete(shader *shaderPrg){
	glUseProgram(SHADER_INVALID_ID);
	glDeleteProgram(shaderPrg->programID);
}