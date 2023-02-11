#ifndef particleRendererBeam_h
#define particleRendererBeam_h


#include "sprite.h"
#include "renderBatch.h"


typedef struct particleRendererBeam {
	float halfWidth;
	/// Number of cubic spline subdivisions?
} particleRendererBeam;


void particleRendererBeamInitBatch(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
);
typedef struct particleManager particleManager;
void particleRendererBeamBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
);


#endif