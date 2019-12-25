#ifndef sprite_h
#define sprite_h


#define GLEW_STATIC
#include <GL/glew.h>

#include "vec3.h"

#include "utilTypes.h"

#include "settingsSprites.h"


typedef struct sprite {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint stateBufferID;
	GLuint uvBufferID;

	GLuint indexBufferID;
	size_t numIndices;
} sprite;


void spriteInit(sprite *spriteData);

void spriteGenerateBuffers(sprite *spriteData, const vec3 *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices);
return_t spriteDifferent(const sprite *s1, const sprite *s2);

return_t spriteSetupDefault();

void spriteDelete(sprite *spriteData);


extern sprite spriteDefault;


#endif