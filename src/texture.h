#ifndef texture_h
#define texture_h


#include <stdint.h>

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


void textureInit(texture *const restrict tex);

texture *textureLoad(const char *const restrict texPath, const size_t texPathLength);
return_t textureSetupDefault();

void textureSetFiltering(const GLuint id, GLint filtering, const uint_least8_t mips);

void textureDelete(texture *const restrict tex);


extern texture g_texDefault;


#endif