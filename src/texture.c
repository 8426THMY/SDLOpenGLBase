#include "texture.h"


#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_image.h>


vector loadedTextures;


void textureInit(texture *tex){
	tex->name = NULL;

	tex->id = 0;
	tex->width = 0;
	tex->height = 0;
}


//Load a texture and return its index in the loadedTextures vector!
size_t textureLoad(const char *imgName){
	size_t index = loadedTextures.size;


	//Create and initialize the texture!
	texture tex;
	textureInit(&tex);


	//Find the full path for the texture!
	const size_t imgNameLength = strlen(imgName);
	char *imgPath = malloc(20 + imgNameLength + 1);
	memcpy(imgPath, ".\\resource\\textures\\", 20);
	strcpy(imgPath + 20, imgName);

	//Load the image and create an SDL2 surface!
	SDL_Surface *image = IMG_Load(imgPath);
	if(image != NULL && index != -1){
		tex.width = image->w;
		tex.height = image->h;

		//This switch statement determines the format that the image data is in by checking how many bytes are used for each pixel.
		GLint pixelFormat = -1;
		switch(image->format->BytesPerPixel){
			case 3:
				pixelFormat = GL_RGB;
			break;

			case 4:
				pixelFormat = GL_RGBA;
			break;
		}


		//Convert our SDL2 surface to an OpenGL texture!
		glGenTextures(1, &tex.id);
		glBindTexture(GL_TEXTURE_2D, tex.id);

		glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, tex.width, tex.height, 0, pixelFormat, GL_UNSIGNED_BYTE, image->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		SDL_FreeSurface(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		GLenum openGLError = glGetError();
		if(openGLError != GL_NO_ERROR){
			printf("Unable to create OpenGL texture!\n"
			       "Path: %s\n"
			       "Texture ID: %u\n"
			       "Error: %i\n", imgPath, index, openGLError);
			index = -1;
		}


		//If there wasn't an error, add the texture to the vector!
		if(index != -1){
			tex.name = malloc(imgNameLength + 1);
			strcpy(tex.name, imgName);

			vectorAdd(&loadedTextures, &tex, 1);

		//Otherwise, delete the texture.
		}else{
			textureDelete(&tex);
		}
	}else{
		printf("Unable to create SDL2 surface!\n"
		       "Path: %s\n"
			   "Texture ID: %u\n"
		       "Error: %s\n", imgPath, index, SDL_GetError());
		index = -1;
	}

	if(imgPath != NULL){
		free(imgPath);
	}


	return(index);
}


void textureDelete(texture *tex){
	if(tex->name != NULL){
		free(tex->name);
	}

	if(tex->id != 0){
		glDeleteTextures(1, &tex->id);
	}
}


//Loop through all the textures we've loaded until we find the one we need!
size_t textureFindNameIndex(const char *name){
	size_t i;
	for(i = 0; i < loadedTextures.size; ++i){
		if(strcmp(name, ((texture *)vectorGet(&loadedTextures, i))->name) == 0){
			break;
		}
	}

	return(i);
}


unsigned char textureModuleSetup(){
	unsigned char success = 0;


	//Initialize our loadedTextures vector.
	vectorInit(&loadedTextures, sizeof(texture));


	//Create an error texture!
	texture tex;

	//Initialize the error texture!
	tex.name = malloc(6);
	strcpy(tex.name, "error");

	//Define the pixel data for the error texture!
	//Source Engine missing texutre. (Make sure to use GL_NEAREST!)
	/*unsigned int pixels[8 * 8];
	size_t a;
	for(a = 0; a < 8; ++a){
		size_t b;
		for(b = 0; b < 8; ++b){
			if((a + b) % 2){
				pixels[(a * 8) + b] = 0xFF000000;
			}else{
				pixels[(a * 8) + b] = 0xFFFF00DC;
			}
		}
	}
	tex.width = 8;
	tex.height = 8;*/
	//Rainbow missing texture. (Make sure to use GL_LINEAR!)
	unsigned int pixels[6];
	pixels[0] = 0xFF0000FF;
	pixels[1] = 0xFF00FFFF;
	pixels[2] = 0xFF00FF00;
	pixels[3] = 0xFFFFFF00;
	pixels[4] = 0xFFFF0000;
	pixels[5] = 0xFFFF00FF;
	tex.width = 6;
	tex.height = 1;

	//Generate an OpenGL texture!
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum openGLError = glGetError();
	if(openGLError != GL_NO_ERROR){
		printf("Unable to create OpenGL texture!\n"
			   "Path: %s\n"
			   "Texture ID: 0\n"
			   "Error: %i\n", tex.name, openGLError);

		textureDelete(&tex);
	}else{
		//Add it to our vector!
		vectorAdd(&loadedTextures, &tex, 1);

		success = 1;
	}


	return(success);
}

void textureModuleCleanup(){
	size_t i = loadedTextures.size;
	while(i > 0){
		--i;
		textureDelete((texture *)vectorGet(&loadedTextures, i));
		vectorRemove(&loadedTextures, i);
	}
	vectorClear(&loadedTextures);
}