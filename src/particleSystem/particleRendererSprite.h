#ifndef particleRendererSprite_h
#define particleRendererSprite_h


#include "sprite.h"
#include "renderBatch.h"


typedef struct particleRendererSprite {
	sprite spriteData;
} particleRendererSprite;


void particleRendererSpriteInitBatch(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
);
typedef struct particleManager particleManager;
void particleRendererSpriteBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
);


#endif