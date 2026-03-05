#ifndef texture_h
#define texture_h


#include <stdint.h>

#include "utilTypes.h"


#define TEXTURE_INVALID_ID 0

#define TEXTURE_NUM_UNITS 1


typedef struct texture {
	char *name;

	unsigned int id;
	unsigned int width;
	unsigned int height;

	int format;
	int filtering;
} texture;


void textureInit(texture *const restrict tex);
texture *textureLoad(
	const char *const restrict texPath,
	const size_t texPathLength
);
void textureBind(
	const texture *const restrict tex,
	const unsigned int unit
);
void textureDelete(texture *const restrict tex);

return_t textureSetup();
void textureCleanup();


extern texture g_texDefault;


#endif