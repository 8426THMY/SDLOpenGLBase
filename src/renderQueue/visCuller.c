#include "visCuller.h"


#include "renderView.h"


void visCullerPopulateRenderView(
	const visCuller *const restrict vc,
	renderView *const restrict view,
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
){

	switch(vc->type){
		case VISCULLER_TYPE_PORTAL:
			visCullerPortalPopulateRenderView(
				&vc->data.portal, view, viewID, frameID, dt
			);
		break;
	}
}


void visCullerDelete(visCuller *const restrict vc){
	switch(vc->type){
		case VISCULLER_TYPE_PORTAL:
			visCullerPortalDelete(&vc->data.portal);
		break;
	}
}