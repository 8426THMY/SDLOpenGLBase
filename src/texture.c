#include "texture.h"


#include <stdio.h>
#include <string.h>

#include <SDL2/SDL_image.h>

#include "utilFile.h"

#include "memoryManager.h"
#include "moduleTexture.h"

#define TEXTURE_PATH_PREFIX        "."FILE_PATH_DELIMITER_STR"resource"FILE_PATH_DELIMITER_STR"textures"FILE_PATH_DELIMITER_STR
#define TEXTURE_PATH_PREFIX_LENGTH (sizeof(TEXTURE_PATH_PREFIX) - 1)
#define IMAGE_PATH_PREFIX          "."FILE_PATH_DELIMITER_STR"resource"FILE_PATH_DELIMITER_STR"images"FILE_PATH_DELIMITER_STR
#define IMAGE_PATH_PREFIX_LENGTH   (sizeof(IMAGE_PATH_PREFIX) - 1)


#define TEXTURE_FILTER_DEFAULT_ID   0
#define TEXTURE_FILTER_NEAREST_ID   1
#define TEXTURE_FILTER_LINEAR_ID    2
#define TEXTURE_FILTER_BILINEAR_ID  3
#define TEXTURE_FILTER_TRILINEAR_ID 4

#define TEXTURE_FILTER_DEFAULT TEXTURE_FILTER_NEAREST_ID
#define TEXTURE_FILTER_ERROR   GL_LINEAR

#define TEXTURE_ERROR_WIDTH  6
#define TEXTURE_ERROR_HEIGHT 1
#define TEXTURE_ERROR_FORMAT GL_RGBA


// This defines a texture to use when we
// cannot load the correct one. We can't
// set up its image data here, however.
texture g_texDefault = {
	.name = "error",

	.width = TEXTURE_ERROR_WIDTH,
	.height = TEXTURE_ERROR_HEIGHT,

	.format = TEXTURE_ERROR_FORMAT,
	.filtering = TEXTURE_FILTER_ERROR
};


void textureInit(texture *const restrict tex){
	tex->name = NULL;

	tex->id = 0;
	tex->width = 0;
	tex->height = 0;

	tex->format = valueInvalid(GLint);
	tex->filtering = valueInvalid(GLint);
}


/*
** Load the texture specified by "texPath" and return a pointer to it.
** If the texture could not be loaded, return a pointer to the default texture.
*/
texture *textureLoad(const char *const restrict texPath, const size_t texPathLength){
	texture *tex;

	FILE *texFile;
	char texFullPath[FILE_MAX_PATH_LENGTH];


	// If the texture has already been loaded, return a pointer to it!
	tex = moduleTextureFind(texPath);
	if(tex != &g_texDefault){
		return(tex);
	}


	// Generate the full path for the texture!
	fileGenerateFullResourcePath(
		TEXTURE_PATH_PREFIX, TEXTURE_PATH_PREFIX_LENGTH,
		texPath, texPathLength,
		texFullPath
	);


	// Load the texture!
	texFile = fopen(texFullPath, "r");
	if(texFile != NULL){
		SDL_Surface *image = NULL;
		GLint format;
		GLint filtering;
		GLenum openGLError;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		while((line = fileReadLine(texFile, &lineBuffer[0], &lineLength)) != NULL){
			// Image path.
			if(lineLength >= 3 && memcmp(line, "i ", 2) == 0){
				if(image == NULL){
					char *const imgPath = memoryManagerGlobalAlloc((IMAGE_PATH_PREFIX_LENGTH + lineLength - 1) * sizeof(*imgPath));
					if(imgPath == NULL){
						/** MALLOC FAILED **/
					}
					// Generate the full path for the image!
					memcpy(imgPath, IMAGE_PATH_PREFIX, IMAGE_PATH_PREFIX_LENGTH);
					fileParseResourcePath(&imgPath[IMAGE_PATH_PREFIX_LENGTH], &line[2], lineLength - 2, NULL);

					// Now load the pixel data!
					image = IMG_Load(imgPath);
					if(image == NULL){
						printf(
							"Unable to create SDL2 surface for image!\n"
							"Path: %s\n"
							"Error: %s\n",
							imgPath, SDL_GetError()
						);
					}

					// Don't forget to free the path afterwards.
					memoryManagerGlobalFree(imgPath);
				}

			// Filter type.
			}else if(lineLength >= 3 && memcmp(line, "f ", 2) == 0){
				filtering = strtoul(&line[2], NULL, 10);
			}
		}

		fclose(texFile);


		if(image != NULL){
			GLuint texID;


			// This determines the format that the image data is in
			// by checking how many bytes are used for each pixel.
			switch(image->format->BytesPerPixel){
				case 1:
					format = GL_RED;
				break;

				case 2:
					format = GL_RG;
				break;

				case 3:
					format = GL_RGB;
				break;

				case 4:
					format = GL_RGBA;
				break;

				default:
					format = GL_RGB;
			}

			// Create an OpenGL texture from our SDL2 surface.
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);

			glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
			textureSetFiltering(texID, filtering, 0);

			// Unbind the texture now that we've set its parameters.
			glBindTexture(GL_TEXTURE_2D, 0);
			// Free the SDL2 surface, as we no longer need it.
			SDL_FreeSurface(image);


			openGLError = glGetError();
			// If we could set up the texture successfully, allocate memory for it and set it up!
			if(openGLError == GL_NO_ERROR){
				tex = moduleTextureAlloc();
				if(tex == NULL){
					/** MALLOC FAILED **/
				}

				// Set the texture's name!
				tex->name = memoryManagerGlobalAlloc(texPathLength + 1);
				if(tex->name == NULL){
					/** MALLOC FAILED **/
				}
				memcpy(tex->name, texPath, texPathLength + 1);

				tex->id = texID;
				tex->width = image->w;
				tex->height = image->h;

				tex->format = format;
			}else{
				printf(
					"Unable to create OpenGL texture!\n"
					"Path: %s\n"
					"Texture ID: %u\n"
					"Error: %i\n",
					texPath, texID, openGLError
				);
			}
		}else{
			printf(
				"Error loading texture!\n"
				"Path: %s\n"
				"Error: No image was specified!\n",
				texFullPath
			);
		}
	}else{
		printf(
			"Unable to open texture!\n"
			"Path: %s\n",
			texFullPath
		);
	}


	return(tex);
}


// Update the filtering mode for a texture!
void textureSetFiltering(const GLuint id, GLint filtering, const uint_least8_t mips){
	GLint filteringMin, filteringMag;

	if(filtering == TEXTURE_FILTER_DEFAULT_ID){
		filtering = TEXTURE_FILTER_DEFAULT;
	}

	switch(filtering){
		case TEXTURE_FILTER_NEAREST_ID:
			filteringMin = (mips > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			filteringMag = GL_NEAREST;
		break;

		case TEXTURE_FILTER_LINEAR_ID:
			filteringMin = (mips > 1) ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR;
			filteringMag = GL_LINEAR;
		break;

		case TEXTURE_FILTER_BILINEAR_ID:
			filteringMin = (mips > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
			filteringMag = GL_LINEAR;
		break;

		case TEXTURE_FILTER_TRILINEAR_ID:
			filteringMin = (mips > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
			filteringMag = GL_LINEAR;
		break;

		default:
			return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMag);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


// This should never be called on the default texture!
void textureDelete(texture *const restrict tex){
	// Only free the name if it's in use.
	if(tex->name != NULL){
		memoryManagerGlobalFree(tex->name);
	}

	if(tex->id != 0){
		glDeleteTextures(1, &tex->id);
	}
}


// Set up the error texture!
return_t textureSetup(){
	unsigned int pixels[TEXTURE_ERROR_WIDTH * TEXTURE_ERROR_HEIGHT];

	// Define the pixel data for the error texture!
	// Source Engine missing texutre. (Make sure to use GL_NEAREST!)
	/*size_t pixRow;
	for(pixRow = 0; pixRow < TEXTURE_ERROR_HEIGHT; ++pixRow){
		size_t pixCol;
		for(pixCol = 0; pixCol < TEXTURE_ERROR_WIDTH; ++pixCol){
			if((pixRow + pixCol) % 2){
				pixels[(pixRow * TEXTURE_ERROR_HEIGHT) + pixCol] = 0xFF000000;
			}else{
				pixels[(pixRow * TEXTURE_ERROR_HEIGHT) + pixCol] = 0xFFFF00DC;
			}
		}
	}*/
	// Rainbow missing texture. (Make sure to use GL_LINEAR!)
	pixels[0] = 0xFF0000FF;
	pixels[1] = 0xFF00FFFF;
	pixels[2] = 0xFF00FF00;
	pixels[3] = 0xFFFFFF00;
	pixels[4] = 0xFFFF0000;
	pixels[5] = 0xFFFF00FF;


	// Create an OpenGL texture using our pixel data.
	glGenTextures(1, &g_texDefault.id);
	glBindTexture(GL_TEXTURE_2D, g_texDefault.id);

	glTexImage2D(GL_TEXTURE_2D, 0, TEXTURE_ERROR_FORMAT, TEXTURE_ERROR_WIDTH, TEXTURE_ERROR_HEIGHT, 0, TEXTURE_ERROR_FORMAT, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE_FILTER_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE_FILTER_ERROR);

	// Unbind the texture now that we've set its parameters.
	glBindTexture(GL_TEXTURE_2D, 0);


	GLenum openGLError = glGetError();
	// If there was an error, print an error message.
	if(openGLError != GL_NO_ERROR){
		printf(
			"Unable to create OpenGL texture!\n"
			"Path: error\n"
			"Texture ID: %u\n"
			"Error: %i\n",
			g_texDefault.id, openGLError
		);

		textureCleanup();


		return(0);
	}


	// Otherwise, we can keep the texture!
	return(1);
}

void textureCleanup(){
	if(g_texDefault.id != 0){
		glDeleteTextures(1, &g_texDefault.id);
	}
}