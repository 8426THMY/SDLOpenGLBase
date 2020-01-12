#include "texture.h"


#include <stdio.h>
#include <string.h>

#include <SDL2/SDL_image.h>

#include "utilFile.h"

#include "memoryManager.h"

#define TEXTURE_PATH_PREFIX        ".\\resource\\textures\\"
#define TEXTURE_PATH_PREFIX_LENGTH (sizeof(TEXTURE_PATH_PREFIX) - 1)
#define IMAGE_PATH_PREFIX          ".\\resource\\images\\"
#define IMAGE_PATH_PREFIX_LENGTH   (sizeof(IMAGE_PATH_PREFIX) - 1)


#define TEXTURE_FILTER_ANY       0
#define TEXTURE_FILTER_NEAREST   1
#define TEXTURE_FILTER_LINEAR    2
#define TEXTURE_FILTER_BILINEAR  3
#define TEXTURE_FILTER_TRILINEAR 4

#define TEXTURE_FILTER_DEFAULT GL_NEAREST
#define TEXTURE_FILTER_ERROR   GL_LINEAR

#define TEXTURE_ERROR_WIDTH  6
#define TEXTURE_ERROR_HEIGHT 1
#define TEXTURE_ERROR_FORMAT GL_RGBA


// This defines a texture to use when we
// cannot load the correct one. We can't
// set up its image data here, however.
texture texDefault = {
	.name = "error",

	.width = TEXTURE_ERROR_WIDTH,
	.height = TEXTURE_ERROR_HEIGHT,

	.format = TEXTURE_ERROR_FORMAT,
	.filtering = TEXTURE_FILTER_ERROR
};


#warning "What if we aren't using the global memory manager?"


void textureInit(texture *tex){
	tex->name = NULL;

	tex->id = 0;
	tex->width = 0;
	tex->height = 0;

	tex->format = invalidValue(tex->format);
	tex->filtering = invalidValue(tex->filtering);
}


// Load the texture specified by "texPath".
return_t textureLoad(texture *tex, const char *texPath){
	FILE *texFile;
	char texFullPath[FILE_MAX_LINE_LENGTH];
	size_t texPathLength = strlen(texPath);

	SDL_Surface *image = NULL;

	// Find the full path for the texture!
	fileGenerateFullPath(
		texPath, texPathLength,
		TEXTURE_PATH_PREFIX, TEXTURE_PATH_PREFIX_LENGTH,
		texFullPath
	);


	textureInit(tex);


	// Load the texture!
	texFile = fopen(texFullPath, "r");
	if(texFile != NULL){
		GLint format;
		GLint filtering;
		GLenum openGLError;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		while((line = fileReadLine(texFile, &lineBuffer[0], &lineLength)) != NULL){
			// Image path.
			if(memcmp(line, "i ", 2) == 0){
				if(image == NULL){
					char imgFullPath[FILE_MAX_LINE_LENGTH];
					// Find the full path for the image!
					fileGenerateFullPath(
						&lineBuffer[2], lineLength - 2,
						IMAGE_PATH_PREFIX, IMAGE_PATH_PREFIX_LENGTH,
						imgFullPath
					);

					// Now load the pixel data!
					image = IMG_Load(imgFullPath);
					if(image == NULL){
						printf(
							"Unable to create SDL2 surface for image!\n"
							"Path: %s\n"
							"Error: %s\n",
							imgFullPath, SDL_GetError()
						);
					}
				}

			// Filter type.
			}else if(memcmp(line, "f ", 2) == 0){
				filtering = strtoul(&lineBuffer[2], NULL, 10);
			}
		}

		fclose(texFile);


		if(image == NULL){
			printf(
				"Error loading texture!\n"
				"Path: %s\n"
				"Error: No image was specified!.\n",
				texFullPath
			);

			return(0);
		}

		// This determines the format that the image data is in
		// by checking how many bytes are used for each pixel.
		switch(image->format->BytesPerPixel){
			case 4:
				format = GL_RGBA;
				tex->format = GL_RGBA;
			break;

			default:
				format = GL_RGB;
				tex->format = GL_RGB;
			break;
		}

		tex->width = image->w;
		tex->height = image->h;


		// Create an OpenGL texture from our SDL2 surface.
		glGenTextures(1, &tex->id);
		glBindTexture(GL_TEXTURE_2D, tex->id);

		glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
		textureSetFiltering(tex, filtering, 0);

		// Unbind the texture now that we've set its parameters.
		glBindTexture(GL_TEXTURE_2D, 0);
		// Free the SDL2 surface, as we no longer need it.
		SDL_FreeSurface(image);


		openGLError = glGetError();
		// If we could set up the texture
		// successfully, set its properties!
		if(openGLError == GL_NO_ERROR){
			++texPathLength;
			tex->name = memoryManagerGlobalAlloc(texPathLength);
			if(tex->name == NULL){
				/** MALLOC FAILED **/
			}
			memcpy(tex->name, texPath, texPathLength);

			return(1);
		}

		// Otherwise, print out what went
		// wrong and delete the texture.
		printf(
			"Unable to create OpenGL texture!\n"
			"Path: %s\n"
			"Texture ID: %u\n"
			"Error: %i\n",
			texPath, tex->id, openGLError
		);

		textureDelete(tex);
	}else{
		printf(
			"Unable to open texture!\n"
			"Path: %s\n",
			texFullPath
		);
	}


	return(0);
}

// Set up the error texture!
return_t textureSetupDefault(){
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
	glGenTextures(1, &texDefault.id);
	glBindTexture(GL_TEXTURE_2D, texDefault.id);

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
			texDefault.id, openGLError
		);

		textureDelete(&texDefault);


		return(0);
	}


	// Otherwise, we can keep the texture!
	return(1);
}


// Update the filtering mode for a texture!
void textureSetFiltering(texture *tex, GLint filtering, const uint_least8_t mips){
	GLint filteringMin, filteringMag;

	if(filtering == TEXTURE_FILTER_ANY){
		filtering = TEXTURE_FILTER_DEFAULT;
	}

	switch(filtering){
		case TEXTURE_FILTER_NEAREST:
			filteringMin = (mips > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			filteringMag = GL_NEAREST;
		break;

		case TEXTURE_FILTER_LINEAR:
			filteringMin = (mips > 1) ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR;
			filteringMag = GL_LINEAR;
		break;

		case TEXTURE_FILTER_BILINEAR:
			filteringMin = (mips > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
			filteringMag = GL_LINEAR;
		break;

		case TEXTURE_FILTER_TRILINEAR:
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


// Loop through all the textures we've loaded until we find the one we need!
size_t textureFindNameIndex(const char *name){
	/*size_t i;
	for(i = 0; i < loadedTextures.size; ++i){
		if(strcmp(name, ((texture *)vectorGet(&loadedTextures, i))->name) == 0){
			break;
		}
	}

	return(i);*/
	return(0);
}


void textureDelete(texture *tex){
	// Only free the name if it's in use
	// and it's not the error texture.
	if(tex->name != NULL && tex != &texDefault){
		memoryManagerGlobalFree(tex->name);
	}

	if(tex->id != 0){
		glDeleteTextures(1, &tex->id);
	}
}