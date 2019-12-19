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

	GLint format;
	GLint filtering;
} texture;


void textureInit(texture *tex);

return_t textureLoad(texture *tex, const char *texPath);
return_t textureSetupDefault();

void textureSetFiltering(texture *tex, GLint filtering, const uint_least8_t mips);

size_t textureFindNameIndex(const char *name);

void textureDelete(texture *tex);


extern texture texDefault;


#endif