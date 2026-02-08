#ifndef visCullerPortal_h
#define visCullerPortal_h


#include <stddef.h>

#include "renderObject.h"
#include "light.h"


#warning "Renderables might need to store a flag that says whether they're visible."
#warning "We would set this flag when iterating over cells, and reset it when we're done."
#warning "Alternatively, we could do what Source does and store the current frame ID."
#warning "We'd have to update the current frame ID before populating any queue, though."


/*
** Portals act as one-way connectors between cells that automatically
** clip the viewing frustum on entry. This is particularly useful for
** narrow doorways into rooms with lots of objects out of view.
*/
typedef struct visCell visCell;
typedef struct visPortal {
	// Pointer to the cell on the other side of the portal.
	visCell *cell;
	#warning "We probably also need to store the portal's geometry somewhere."
	#warning "Do we want to support enabling and disabling portals?"
} visPortal;

/*
** Cells are convex volumes that divide the scene up. We only have
** to render the objects in a particular cell if it's visible from
** the cell that our camera is currently in.
*/
typedef struct visCell {
	// Cells visible from within this one. These cells need
	// not be adjacent, but they can't be owned by any portals.
	visCell *visibleCells;
	size_t numVisibleCells;
	// Portals owned by this cell.
	visPortal *portals;
	size_t numPortals;
	#warning "We probably also need to store the cell's convex hull."
	#warning "Otherwise, we have no way of knowing when an object is in it!"
	#warning "We should also store static geometry to render as part of the cell."

	#warning "We could probably use fixed-size arrays for these."
	// Render objects that are currently in this cell. Note that
	// a render object may be in multiple cells at the same time!
	renderObject *objs;
	size_t numObjs;
	// Light sources are stored in their own array. Again,
	// a light may exist in many different cells at once.
	light *lights;
	size_t numLights;
} visCell;

/*
** The portal culler divides the scene into convex volumes called
** cells, each of which stores an array of all of the objects within
** it. During rendering, objects in cells that are not visible from
** the one our camera is in are automatically culled.
*/
typedef struct visCullerPortal {
	visCell *cells;
	size_t numCells;
} visCullerPortal;


typedef struct renderView renderView;
void visCullerPortalPopulateRenderView(
	const visCullerPortal *const restrict vcPortal,
	renderView *const restrict view,
	const unsigned int viewID,
	const unsigned int frameID,
	const float dt
);

void visCullerPortalDelete(visCullerPortal *const restrict vcPortal);


#endif