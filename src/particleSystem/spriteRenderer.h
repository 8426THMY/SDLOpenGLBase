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


typedef uint_least8_t spriteRendererType_t;

typedef struct spriteRenderer {
	union {
		spriteRendererBatched batchedRenderer;
		spriteRendererInstanced instancedRenderer;
	} data;
	spriteRendererType_t type;
} spriteRenderer;


void spriteRendererInit(spriteRenderer *const restrict renderer, const spriteRendererType_t type);
return_t spriteRendererHasRoom(const spriteRenderer *const restrict renderer, const size_t batchSize);
void spriteRendererDraw(spriteRenderer *const restrict renderer);
void spriteRendererDrawFull(spriteRenderer *const restrict renderer);


#endif