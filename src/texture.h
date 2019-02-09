#ifndef texture_h
#define texture_h


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


#define TEXTURE_FILTER_DEFAULT GL_LINEAR
#define TEXTURE_FILTER_ERROR   GL_LINEAR

#define TEXTURE_ERROR_WIDTH  6
#define TEXTURE_ERROR_HEIGHT 1
#define TEXTURE_ERROR_FORMAT GL_RGBA


typedef struct texture {
	char *name;

	GLuint id;
	GLuint width;
	GLuint height;
} texture;


void textureInit(texture *tex);

return_t textureLoad(texture *tex, const char *imgName);
return_t textureSetupError();

void textureDelete(texture *tex);

size_t textureFindNameIndex(const char *name);


extern texture errorTex;


#endif