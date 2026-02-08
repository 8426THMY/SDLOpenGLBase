#include "light.h"


return_t lightInFrustum(
	const light *const restrict l,
	const renderFrustum *const restrict frustum
){

	switch(l->type){
		case LIGHT_TYPE_SPOT:
			return(lightSpotInFrustum(&l->data.spot, frustum));
		break;
		case LIGHT_TYPE_POINT:
			return(lightPointInFrustum(&l->data.point, frustum));
		break;
		case LIGHT_TYPE_SPOTORTHO:
			return(lightSpotOrthoInFrustum(&l->data.spotOrtho, frustum));
		break;
		case LIGHT_TYPE_ORTHO:
			return(lightOrthoInFrustum(&l->data.ortho, frustum));
		break;
	}
	return(0);
}

void lightUpdateGlobalTransform(light *const restrict l){
	switch(l->type){
		case LIGHT_TYPE_SPOT:
			lightSpotUpdateGlobalTransform(&l->data.spot);
		break;
		case LIGHT_TYPE_POINT:
			lightPointUpdateGlobalTransform(&l->data.point);
		break;
		case LIGHT_TYPE_SPOTORTHO:
			lightSpotOrthoUpdateGlobalTransform(&l->data.spotOrtho);
		break;
		case LIGHT_TYPE_ORTHO:
			lightOrthoUpdateGlobalTransform(&l->data.ortho);
		break;
	}
}

void lightPreDraw(
	const light *const restrict l,
	renderView *const restrict view
){

	switch(l->type){
		case LIGHT_TYPE_SPOT:
			lightSpotPreDraw(&l->data.spot, view);
		break;
		case LIGHT_TYPE_POINT:
			lightPointPreDraw(&l->data.point, view);
		break;
		case LIGHT_TYPE_SPOTORTHO:
			lightSpotOrthoPreDraw(&l->data.spotOrtho, view);
		break;
		case LIGHT_TYPE_ORTHO:
			lightOrthoPreDraw(&l->data.ortho, view);
		break;
	}
}

void lightDraw(const light *const restrict l){
	switch(l->type){
		case LIGHT_TYPE_SPOT:
			lightSpotDraw(&l->data.spot);
		break;
		case LIGHT_TYPE_POINT:
			lightPointDraw(&l->data.point);
		break;
		case LIGHT_TYPE_SPOTORTHO:
			lightSpotOrthoDraw(&l->data.spotOrtho);
		break;
		case LIGHT_TYPE_ORTHO:
			lightOrthoDraw(&l->data.ortho);
		break;
	}
}