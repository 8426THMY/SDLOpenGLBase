#ifndef particleRendererSprite_h
#define particleRendererSprite_h


#include <stddef.h>

#include "sprite.h"
#include "spriteRenderer.h"

#include "sort.h"

#include "camera.h"


typedef struct particleRendererSprite {
	sprite spriteData;
} particleRendererSprite;


size_t particleRendererSpriteBatchSize(
	const particleRendererSprite *const restrict renderer,
	const size_t numParticles
);
void particleRendererSpriteBatch(
	const particleRendererSprite *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
);


#endif