#ifndef visCuller_h
#define visCuller_h


#include <stdint.h>

#include "visCullerPortal.h"


#define VISCULLER_NUM_TYPES 1

#define VISCULLER_TYPE_PORTAL 0


typedef uint_least8_t visCullerType;

/**
*** It might be good to support a few different methods of visibility
*** culling, such as portals for indoor areas and maybe something like
*** an octree or kd-tree for outdoor areas.
***     https://30fps.net/pages/pvs-portals-and-quake/
*** This engine also implements visibility culling on kd-trees:
***     https://www.visualizationlibrary.org/documentation/pag_guide_portals.html
*** The Source engine does all of its culling and rendering stuff in
*** /src/game/client/viewrender.cpp and /src/game/client/clientleafsystem.cpp.
**/
typedef struct visCuller {
	// This should be large enough
	// to store any type of culler.
	union {
		visCullerPortal portal;
	} data;
	// Stores which type of
	// culler this object is.
	visCullerType type;
} visCuller;


typedef renderView renderView;
void visCullerPopulateRenderView(
	visCuller *const restrict vc,
	renderView *const restrict view
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
);

void visCullerDelete(visCuller *const restrict vc);


extern void (*const visCullerPopulateRenderViewTable[VISCULLER_NUM_TYPES])(
	void *const restrict vc,
	renderView *const restrict view
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
);

extern void (*const visCullerDeleteTable[VISCULLER_NUM_TYPES])(
	void *const restrict vc
);


#endif