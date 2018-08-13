#ifndef texture_h
#define texture_h


#define TEXTUREMODULE


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "vector.h"


typedef struct texture {
	char *name;

	GLuint id;
	GLuint width;
	GLuint height;
} texture;


void textureInit(texture *tex);

size_t textureLoad(const char *imgName);

void textureDelete(texture *tex);

size_t textureFindNameIndex(const char *name);


unsigned char textureModuleSetup();
void textureModuleCleanup();


extern vector loadedTextures;


#endif