#ifndef sprite_h
#define sprite_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <string.h>

#include "vec2.h"
#include "vec3.h"
#include "rectangle.h"
#include "mat4.h"

#include "utilTypes.h"

#include "settingsSprites.h"


#define SPRITE_IMAGE_TYPE_NORMAL 0
#define SPRITE_IMAGE_TYPE_SDF    1
#define SPRITE_IMAGE_TYPE_MSDF   2


typedef GLsizei spriteVertexIndex;

typedef struct spriteVertex {
	vec3 pos;
	vec2 uv;
} spriteVertex;

typedef struct sprite {
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint stateBufferID;

	GLuint indexBufferID;
	spriteVertexIndex numIndices;
} sprite;

// This represents the data we must send to
// the shader for each instance of a sprite.
typedef struct spriteState {
	mat4 state;
	rectangle uvOffsets;
} spriteState;


typedef struct spriteShader {
	GLuint programID;

	GLuint vpMatrixID;
	GLuint sdfTypeID;
	GLuint sdfColourID;
	GLuint sdfBackgroundID;
} spriteShader;


void spriteInit(sprite *const restrict spriteData);

void spriteGenerateBuffers(
	sprite *const restrict spriteData,
	const spriteVertex *const restrict vertices, const spriteVertexIndex numVertices,
	const spriteVertexIndex *const restrict indices, const spriteVertexIndex numIndices
);

return_t spriteVertexDifferent(const spriteVertex *const restrict v1, const spriteVertex *const restrict v2);
return_t spriteDifferent(const sprite *const restrict s1, const sprite *const restrict s2);

return_t spriteSetupDefault();

void spriteDelete(sprite *const restrict spriteData);

return_t spriteShaderInit(spriteShader *const restrict shader, const GLuint programID);


#endif