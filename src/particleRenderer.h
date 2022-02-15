#ifndef particleRenderer_h
#define particleRenderer_h


#include "particle.h"
#include "camera.h"
#include "sprite.h"


typedef struct particleRenderer {
	// This should be large enough
	// to store any type of renderer.
	union {
		//
	} data;

	// This function is executed on each particle.
	void (*func)(
		const void *const restrict renderer, const particle *const restrict particles,
		const size_t numParticles, const particleDef *const restrict properties,
		const camera *const restrict cam, const spriteShader *const restrict shader, const float dt
	);
} particleRenderer;


void particleRendererBeam(
	const void *const restrict renderer, const particle *const restrict particles,
	const size_t numParticles, const particleDef *const restrict properties,
	const camera *const restrict cam, const spriteShader *const restrict shader, const float dt
);
void particleRendererSprites(
	const void *const restrict renderer, const particle *const restrict particles,
	const size_t numParticles, const particleDef *const restrict properties,
	const camera *const restrict cam, const spriteShader *const restrict shader, const float dt
);


#endif