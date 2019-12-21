#ifndef particleEmitter_h
#define particleEmitter_h


#include <stddef.h>


typedef struct particleEmitterDef {
	//
} particleEmitterDef;

typedef struct particleEmitter {
	//
} particleEmitter;


size_t particleEmitterUpdate(const particleEmitter *emitter, const float time);


#endif