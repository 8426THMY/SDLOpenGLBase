#include "texture.h"


#define TEXTURE_PATH_PREFIX        ".\\resource\\textures\\"
#define TEXTURE_PATH_PREFIX_LENGTH sizeof(TEXTURE_PATH_PREFIX)

#define TEXTURE_FILTER_DEFAULT GL_LINEAR
#define TEXTURE_FILTER_ERROR   GL_LINEAR

#define TEXTURE_ERROR_WIDTH  6
#define TEXTURE_ERROR_HEIGHT 1
#define TEXTURE_ERROR_FORMAT GL_RGBA


#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_image.h>

#include "memoryManager.h"


//This defines a texture to use when we
//cannot load the correct one. We can't
//set up its image data here, however.
texture errorTex = {
	.name = "error",

	.width = TEXTURE_ERROR_WIDTH,
	.height = TEXTURE_ERROR_HEIGHT
};


#warning "What if we aren't using the global memory manager?"


void textureInit(texture *tex){
	tex->name = NULL;

	tex->id = 0;
	tex->width = 0;
	tex->height = 0;
}


//Load a texture using the image specified by "imgName".
return_t textureLoad(texture *tex, const char *imgName){
	textureInit(tex);

	//Find the full path for the texture!
	const size_t imgNameLength = strlen(imgName);
	char *imgPath = memoryManagerGlobalAlloc(TEXTURE_PATH_PREFIX_LENGTH + imgNameLength + 1);
	if(imgPath == NULL){
		/** MALLOC FAILED **/
	}
	memcpy(imgPath, TEXTURE_PATH_PREFIX, TEXTURE_PATH_PREFIX_LENGTH);
	strcpy(imgPath + TEXTURE_PATH_PREFIX_LENGTH, imgName);

	//Load the image and create an SDL2 surface!
	SDL_Surface *image = IMG_Load(imgPath);
	if(image != NULL){
		tex->width = image->w;
		tex->height = image->h;

		//This switch statement determines the format that the image
		//data is in by checking how many bytes are used for each pixel.
		GLint pixelFormat = -1;
		switch(image->format->BytesPerPixel){
			case 3:
				pixelFormat = GL_RGB;
			break;

			case 4:
				pixelFormat = GL_RGBA;
			break;
		}


		//Create an OpenGL texture from our SDL2 surface.
		glGenTextures(1, &tex->id);
		glBindTexture(GL_TEXTURE_2D, tex->id);

		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, tex->width, tex->height, 0, pixelFormat, GL_UNSIGNED_BYTE, image->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE_FILTER_DEFAULT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE_FILTER_DEFAULT);

		//Unbind the texture now that we've set its parameters.
		glBindTexture(GL_TEXTURE_2D, 0);

		//Free the SDL2 surface, as we no longer need it.
		SDL_FreeSurface(image);


		GLenum openGLError = glGetError();
		//If we could set up the texture
		//successfully, set its name!
		if(openGLError == GL_NO_ERROR){
			tex->name = memoryManagerGlobalRealloc(imgPath, imgNameLength + 1);
			if(tex->name == NULL){
				/** REALLOC FAILED **/
			}
			strcpy(tex->name, imgName);

			return(1);

		//Otherwise, print out what went
		//wrong and delete the texture.
		}else{
			printf(
				"Unable to create OpenGL texture!\n"
				"Path: %s\n"
				"Texture ID: %u\n"
				"Error: %i\n",
				imgPath, tex->id, openGLError
			);

			textureDelete(tex);
		}

	//If we could not load the image, print an error message.
	}else{
		printf(
			"Unable to create SDL2 surface!\n"
			"Path: %s\n"
			"Error: %s\n",
			imgPath, SDL_GetError()
		);
	}

	memoryManagerGlobalFree(imgPath);


	return(0);
}

//Set up the error texture!
return_t textureSetupError(){
	unsigned int pixels[TEXTURE_ERROR_WIDTH * TEXTURE_ERROR_HEIGHT];

	//Define the pixel data for the error texture!
	//Source Engine missing texutre. (Make sure to use GL_NEAREST!)
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
	//Rainbow missing texture. (Make sure to use GL_LINEAR!)
	pixels[0] = 0xFF0000FF;
	pixels[1] = 0xFF00FFFF;
	pixels[2] = 0xFF00FF00;
	pixels[3] = 0xFFFFFF00;
	pixels[4] = 0xFFFF0000;
	pixels[5] = 0xFFFF00FF;


	//Create an OpenGL texture using our pixel data.
	glGenTextures(1, &errorTex.id);
	glBindTexture(GL_TEXTURE_2D, errorTex.id);

	glTexImage2D(GL_TEXTURE_2D, 0, TEXTURE_ERROR_FORMAT, TEXTURE_ERROR_WIDTH, TEXTURE_ERROR_HEIGHT, 0, TEXTURE_ERROR_FORMAT, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TEXTURE_FILTER_ERROR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TEXTURE_FILTER_ERROR);

	//Unbind the texture now that we've set its parameters.
	glBindTexture(GL_TEXTURE_2D, 0);


	GLenum openGLError = glGetError();
	//If there was an error, print an error message.
	if(openGLError != GL_NO_ERROR){
		printf(
			"Unable to create OpenGL texture!\n"
			"Path: error\n"
			"Texture ID: %u\n"
			"Error: %i\n",
			errorTex.id, openGLError
		);

		textureDelete(&errorTex);


		return(0);
	}


	//Otherwise, we can keep the texture!
	return(1);
}


void textureDelete(texture *tex){
	//Only free the name if it's in use
	//and it's not the error texture.
	if(tex->name != NULL && tex != &errorTex){
		memoryManagerGlobalFree(tex->name);
	}

	if(tex->id != 0){
		glDeleteTextures(1, &tex->id);
	}
}


//Loop through all the textures we've loaded until we find the one we need!
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