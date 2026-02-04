#ifndef renderFrustum_h
#define renderFrustum_h


#include "vec4.h"
#include "mat4.h"


/*
** A vec4 p = (n_x, n_y, n_z, d) defines a plane as follows:
**     n_x x + n_y y + n_z z + d = 0.
*/
typedef vec4 renderFrustumPlane;

typedef struct renderFrustum {
	renderFrustumPlane left;
	renderFrustumPlane right;
	renderFrustumPlane bottom;
	renderFrustumPlane top;
	renderFrustumPlane near;
	renderFrustumPlane far;
} renderFrustum;


void renderFrustumInit(
	renderFrustum *const restrict frustum,
	const mat4 *const restrict vpMatrix
);


#endif