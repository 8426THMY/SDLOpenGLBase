#ifndef particleRendererSprite_h
#define particleRendererSprite_h


#include <stddef.h>

#include "sprite.h"
#include "spriteRenderer.h"

#include "camera.h"


typedef struct particleRendererSprite {
	sprite spriteData;
} particleRendererSprite;


void particleRendererSpriteInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
);
typedef struct particleManager particleManager;
size_t particleRendererSpriteBatchSize(
	const void *const restrict renderer,
	const particleManager *const restrict manager
);
void particleRendererSpriteBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
);


#endif