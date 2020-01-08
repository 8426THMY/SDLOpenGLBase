#ifndef sprite_h
#define sprite_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>

#include "rectangle.h"
#include "vec3.h"
#include "vec2.h"
#include "mat4.h"

#include "utilTypes.h"

#include "settingsSprites.h"


typedef struct spriteVertex {
	vec3 pos;
	vec2 uv;
} spriteVertex;

typedef struct sprite {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint stateBufferID;

	GLuint indexBufferID;
	size_t numIndices;
} sprite;

// This represents the data we must send to
// the shader for each instance of a sprite.
typedef struct spriteState {
	mat4 state;
	rectangle uvOffsets;
} spriteState;


void spriteInit(sprite *spriteData);

void spriteGenerateBuffers(sprite *spriteData, const spriteVertex *vertices, const size_t numVertices, const size_t *indices, const size_t numIndices);

return_t spriteVertexDifferent(const spriteVertex *v1, const spriteVertex *v2);
return_t spriteDifferent(const sprite *s1, const sprite *s2);

return_t spriteSetupDefault();

void spriteDelete(sprite *spriteData);


extern sprite spriteDefault;


#endif