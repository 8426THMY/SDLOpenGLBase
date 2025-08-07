#ifndef sprite_h
#define sprite_h


#include "vec2.h"
#include "vec3.h"

#include "mesh.h"

#include "utilTypes.h"


#warning "Should we rename sprite to meshStatic?"


typedef meshVertexIndex spriteVertexIndex;

typedef struct spriteVertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;
} spriteVertex;

typedef mesh sprite;


return_t spriteSetup();
void spriteCleanup();


#endif