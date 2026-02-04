#include "light.h"


return_t (*const lightInFrustumTable[LIGHT_NUM_TYPES])(
	const light *const restrict l,
	const renderFrustum *const restrict frustum
) = {

	lightSpotInFrustum
	lightPointInFrustum
	lightSpotOrthoInFrustum
	lightOrthoInFrustum
}

void (*const lightUpdateGlobalTransformTable[LIGHT_NUM_TYPES])(
	light *const restrict l
) = {

	lightSpotUpdateGlobalTransform
	lightPointUpdateGlobalTransform
	lightSpotOrthoUpdateGlobalTransform
	lightOrthoUpdateGlobalTransform
}

void (*const lightPreDrawTable[LIGHT_NUM_TYPES])(
	const light *const restrict l,
	renderView *const restrict view
) = {

	lightSpotPreDraw
	lightPointPreDraw
	lightSpotOrthoPreDraw
	lightOrthoPreDraw
}

void (*const lightDrawTable[LIGHT_NUM_TYPES])(
	const light *const restrict l
) = {

	lightSpotDraw
	lightPointDraw
	lightSpotOrthoDraw
	lightOrthoDraw
}


return_t lightInFrustum(
	const light *const restrict l,
	const renderFrustum *const restrict frustum
){

	return(lightInFrustumTable[l->type]((const void *)(&l->data), frustum));
}

void lightUpdateGlobalTransform(light *const restrict l){
	lightUpdateGlobalTransformTable[l->type]((void *)(&l->data));
}

void lightPreDraw(
	const light *const restrict l,
	renderView *const restrict view
){

	lightPreDrawTable[l->type]((const void *)(&l->data), view);
}

void lightDraw(const light *const restrict l){
	lightDrawTable[l->type]((const void *)(&l->data));
}