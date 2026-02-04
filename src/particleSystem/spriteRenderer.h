#ifndef spriteRenderer_h
#define spriteRenderer_h


#define GLEW_STATIC
#include <GL/glew.h>

#include <stdint.h>

#include "spriteRendererBatched.h"
#include "spriteRendererInstanced.h"
#include "spriteRendererCommon.h"

#include "utilTypes.h"


#define SPRITE_RENDERER_TYPE_UNUSED    0
#define SPRITE_RENDERER_TYPE_BATCHED   1
#define SPRITE_RENDERER_TYPE_INSTANCED 2


#warning "It might be good to make both types of sprite renderers use the same global buffer object."
#warning "That way, it might be possible to totally remove this distinction between sprite renderers."


// This data is shared by all sprite renderers.
typedef struct spriteRendererCommon {
	#warning "Should we maybe use a special type for sprite textures?"
	textureGroup *texGroup;
	// shader
	GLenum blendSrc;
	GLenum blendDest;
	GLenum drawMode;
	// colour inheritance
	// other stuff
} spriteRendererCommon;

typedef uint_least8_t spriteRendererType;

typedef struct spriteRenderer {
	union {
		spriteRendererBatched batchedRenderer;
		spriteRendererInstanced instancedRenderer;
	} data;
	spriteRendererCommon common;
	spriteRendererType type;
} spriteRenderer;


void spriteRendererInit(
	spriteRenderer *const restrict renderer,
	const spriteRendererCommon *const restrict common,
	const spriteRendererType type
);
return_t spriteRendererCompatible(
	const spriteRenderer *const restrict renderer,
	const spriteRendererCommon *const restrict common,
	const spriteRendererType type
);
void spriteRendererDraw(spriteRenderer *const restrict renderer);
void spriteRendererDrawFull(spriteRenderer *const restrict renderer);


#endif