#ifndef spriteRenderer_h
#define spriteRenderer_h


#include <stdint.h>

#include "spriteRendererBatched.h"
#include "spriteRendererInstanced.h"

#include "utilTypes.h"


#define SPRITE_RENDERER_TYPE_UNUSED    0
#define SPRITE_RENDERER_TYPE_BATCHED   1
#define SPRITE_RENDERER_TYPE_INSTANCED 2


#warning "It might be good to make both types of sprite renderers use the same global buffer object."


typedef uint_least8_t spriteRendererType;

typedef struct spriteRenderer {
	union {
		spriteRendererBatched batchedRenderer;
		spriteRendererInstanced instancedRenderer;
	} data;
	spriteRendererType type;
} spriteRenderer;


void spriteRendererInit(spriteRenderer *const restrict renderer, const spriteRendererType type);
void spriteRendererDraw(spriteRenderer *const restrict renderer);
void spriteRendererDrawFull(spriteRenderer *const restrict renderer);


#endif