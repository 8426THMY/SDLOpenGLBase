#ifndef sprite_h
#define sprite_h


#include <stdint.h>

#include "vec2.h"
#include "vec3.h"

#include "settingsSprites.h"


// By default, OpenGL uses a value of "-1" as
// its primitive restart index for triangle strips.
#define SPRITE_PRIMITIVE_RESTART_INDEX valueInvalid(unsigned int)


typedef struct spriteVertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;
} spriteVertex;

typedef uint_least32_t spriteVertexIndex;


#endif