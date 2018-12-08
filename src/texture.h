#ifndef texture_h
#define texture_h


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"


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