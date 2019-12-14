#include "shader.h"


#include <stdio.h>
#include <stdlib.h>


void shaderInit(shader *shaderPrg){
	shaderPrg->shaderID = 0;

	shaderPrg->mvpMatrixID = 0;
	shaderPrg->uvOffsetsID = 0;
	shaderPrg->boneStatesID = 0;
}

#warning "We still use some mallocs here because this is called before we initialise the memory managers."
#warning "This is bad, because we may need to call this after we've set them up."
return_t shaderLoad(shader *shaderPrg, const char *vertexPath, const char *fragmentPath){
	FILE *shaderFile;

	GLint compiled = GL_FALSE;
	GLint infoLogLength = 0;


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Load the vertex shader file!
	if((shaderFile = fopen(vertexPath, "rb")) != NULL){
		// Store the code!
		fseek(shaderFile, 0, SEEK_END);
		const long vertexFileSize = ftell(shaderFile);
		rewind(shaderFile);

		char *vertexSource = malloc(vertexFileSize + 1);
		if(vertexSource == NULL){
			printf("Failed to allocate memory for vertex shader file!\n"
			       "Path: %s\n", vertexPath);
			fclose(shaderFile);

			return(0);
		}
		fread(vertexSource, 1, vertexFileSize, shaderFile);
		vertexSource[vertexFileSize] = '\0';
		fclose(shaderFile);


		// Compile the vertex shader!
		const char *vertexSourcePointer = vertexSource;
		glShaderSource(vertexShader, 1, &vertexSourcePointer, NULL);
		glCompileShader(vertexShader);

		// We don't want any memory leaking shenanigans!
		free(vertexSource);


		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
		if(!compiled){
			printf("Vertex shader could not be compiled!\n");

			// If the compilation was unsuccessful, try to print the error message!
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
			// According to the documentation, the length should be 0 if there is no error despite normally counting the null terminator.
			// When using Intel integrated graphics, however, I've found that the size is 1 when there is no error.
			if(infoLogLength > 1){
				GLchar vertexError[infoLogLength + 1];
				glGetShaderInfoLog(vertexShader, infoLogLength, NULL, vertexError);

				printf("%s\n", vertexError);
			}


			return(0);
		}
	}else{
		printf("Unable to load vertex shader file!\n"
		       "Path: %s\n", vertexPath);
		fclose(shaderFile);


		return(0);
	}


	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load the fragment shader file!
	if((shaderFile = fopen(fragmentPath, "rb")) != NULL){
		// Store the code!
		fseek(shaderFile, 0, SEEK_END);
		const long fragmentFileSize = ftell(shaderFile);
		rewind(shaderFile);

		char *fragmentSource = malloc(fragmentFileSize + 1);
		if(fragmentSource == NULL){
			printf("Failed to allocate memory for fragment shader file!\n"
			       "Path: %s\n", fragmentPath);
			fclose(shaderFile);

			return(0);
		}
		fread(fragmentSource, 1, fragmentFileSize, shaderFile);
		fragmentSource[fragmentFileSize] = '\0';
		fclose(shaderFile);


		// Compile the fragment shader!
		const char *fragmentSourcePointer = fragmentSource;
		glShaderSource(fragmentShader, 1, &fragmentSourcePointer, NULL);
		glCompileShader(fragmentShader);

		// We don't want any memory leaking shenanigans!
		free(fragmentSource);


		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
		if(!compiled){
			printf("Fragment shader could not be compiled!\n");

			// If the compilation was unsuccessful, try to print the error message!
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
			// According to the documentation, the length should be 0 if there is no error despite normally counting the null terminator.
			// When using Intel integrated graphics, however, I've found that the size is 1 when there is no error.
			if(infoLogLength > 1){
				GLchar fragmentError[infoLogLength + 1];
				glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, fragmentError);

				printf("%s\n", fragmentError);
			}


			return(0);
		}
	}else{
		printf("Unable to load fragment shader file!\n"
		       "Path: %s\n", fragmentPath);
		fclose(shaderFile);


		return(0);
	}


	shaderPrg->shaderID = glCreateProgram();
	glAttachShader(shaderPrg->shaderID, vertexShader);
	glAttachShader(shaderPrg->shaderID, fragmentShader);
	glLinkProgram(shaderPrg->shaderID);

	glGetProgramiv(shaderPrg->shaderID, GL_LINK_STATUS, &compiled);
	if(!compiled){
		printf("Shader program could not be compiled!\n");

		// If the compilation was unsuccessful, try to print the error message!
		glGetProgramiv(shaderPrg->shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		// According to the documentation, the length should be 0 if there is no error despite normally counting the null terminator.
		// When using Intel integrated graphics, however, I've found that the size is 1 when there is no error.
		if(infoLogLength > 1){
			char programError[infoLogLength + 1];
			glGetShaderInfoLog(shaderPrg->shaderID, infoLogLength, NULL, programError);

			printf("%s\n", programError);
		}

		shaderPrg->shaderID = 0;


		return(0);
	}


	glDetachShader(shaderPrg->shaderID, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(shaderPrg->shaderID, fragmentShader);
	glDeleteShader(fragmentShader);


	// Enable the shader we just loaded!
	glUseProgram(shaderPrg->shaderID);

	// Find the positions of our shader's uniform variables (pretty sure they're positioned in alphabetical order)!
	shaderPrg->mvpMatrixID = glGetUniformLocation(shaderPrg->shaderID, "mvpMatrix");
	shaderPrg->uvOffsetsID = glGetUniformLocation(shaderPrg->shaderID, "uvOffsets");
	shaderPrg->boneStatesID = glGetUniformLocation(shaderPrg->shaderID, "boneStates");

	// Bind uniform variable "baseTex0" to the first texture mapping unit (GL_TEXTURE0)!
	glUniform1i(glGetUniformLocation(shaderPrg->shaderID, "baseTex0"), 0);


	return(1);
}

void shaderDelete(shader *shaderPrg){
	glUseProgram(0);
	glDeleteProgram(shaderPrg->shaderID);
}