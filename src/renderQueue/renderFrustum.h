#ifndef renderFrustum_h
#define renderFrustum_h


#include "vec4.h"
#include "mat4.h"

#include "colliderSphere.h"
#include "colliderAABB.h"

#include "utilTypes.h"


#define RENDERFRUSTUM_NUM_PLANES 6

#define RENDERFRUSTUM_PLANE_LEFT   0
#define RENDERFRUSTUM_PLANE_RIGHT  1
#define RENDERFRUSTUM_PLANE_BOTTOM 2
#define RENDERFRUSTUM_PLANE_TOP    3
#define RENDERFRUSTUM_PLANE_NEAR   4
#define RENDERFRUSTUM_PLANE_FAR    5


/*
** A vec4 p = (n_x, n_y, n_z, d) defines a plane as follows:
**     n_x x + n_y y + n_z z + d = 0.
*/
typedef vec4 renderFrustumPlane;

// We assume that the frustum's planes have outward-pointing normals.
typedef struct renderFrustum {
	renderFrustumPlane planes[RENDERFRUSTUM_NUM_PLANES];
} renderFrustum;


void renderFrustumInit(
	renderFrustum *const restrict frustum,
	const mat4 *const restrict vpMatrix
);

return_t renderFrustumCollidingSphere(
	const renderFrustum *const restrict frustum,
	const colliderSphere *const restrict sphere
);
return_t renderFrustumCollidingAABB(
	const renderFrustum *const restrict frustum,
	const colliderAABB *const restrict aabb
);
return_t renderFrustumCollidingAABBAlt(
	const renderFrustum *const restrict frustum,
	const colliderAABB *const restrict aabb
);


#endif