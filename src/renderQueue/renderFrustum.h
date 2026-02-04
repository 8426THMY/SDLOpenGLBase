#ifndef renderFrustum_h
#define renderFrustum_h


#include "vec4.h"
#include "mat4.h"


// A vec4 p = (p_x, p_y, p_z, p_w) defines a plane as follows:
//     p_x x + p_y y + p_z z + p_w = 0.
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