#include "visCuller.h"


#include "renderView.h"


void (*const visCullerPopulateRenderViewTable[VISCULLER_NUM_TYPES])(
	void *const restrict vc,
	renderView *const restrict view,
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
) = {

	visCullerPortalPopulateRenderView
};


void (*const visCullerDeleteTable[VISCULLER_NUM_TYPES])(
	void *const restrict vc
) = {

	visCullerPortalDelete
};


void visCullerPopulateRenderView(
	visCuller *const restrict vc,
	renderView *const restrict view,
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
){

	visCullerPopulateRenderViewTable[vc->type](
		(void *)(&vc->data), view, viewID, frameID, dt
	);
}


void visCullerDelete(visCuller *const restrict vc){
	visCullerDeleteTable[vc->type]((void *)(&vc->data));
}