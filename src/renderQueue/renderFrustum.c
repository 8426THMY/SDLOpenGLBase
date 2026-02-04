#include "renderFrustum.h"


/*
** Compute the six planes of the viewing
** frustum using Gribb and Hartmann's method.
*/
void renderFrustumInit(
	renderFrustum *const restrict frustum,
	const mat4 *const restrict vpMatrix
){

	vec4InitSet(&frustum->left,
		vpMatrix->m[0][3] + vpMatrix->m[0][0],
		vpMatrix->m[1][3] + vpMatrix->m[1][0],
		vpMatrix->m[2][3] + vpMatrix->m[2][0],
		vpMatrix->m[3][3] + vpMatrix->m[3][0]
	);
	vec4InitSet(&frustum->right,
		vpMatrix->m[0][3] - vpMatrix->m[0][0],
		vpMatrix->m[1][3] - vpMatrix->m[1][0],
		vpMatrix->m[2][3] - vpMatrix->m[2][0],
		vpMatrix->m[3][3] - vpMatrix->m[3][0]
	);
	vec4InitSet(&frustum->bottom,
		vpMatrix->m[0][3] + vpMatrix->m[0][1],
		vpMatrix->m[1][3] + vpMatrix->m[1][1],
		vpMatrix->m[2][3] + vpMatrix->m[2][1],
		vpMatrix->m[3][3] + vpMatrix->m[3][1]
	);
	vec4InitSet(&frustum->top,
		vpMatrix->m[0][3] - vpMatrix->m[0][1],
		vpMatrix->m[1][3] - vpMatrix->m[1][1],
		vpMatrix->m[2][3] - vpMatrix->m[2][1],
		vpMatrix->m[3][3] - vpMatrix->m[3][1]
	);
	vec4InitSet(&frustum->near,
		vpMatrix->m[0][3] + vpMatrix->m[0][2],
		vpMatrix->m[1][3] + vpMatrix->m[1][2],
		vpMatrix->m[2][3] + vpMatrix->m[2][2],
		vpMatrix->m[3][3] + vpMatrix->m[3][2]
	);
	vec4InitSet(&frustum->far,
		vpMatrix->m[0][3] - vpMatrix->m[0][2],
		vpMatrix->m[1][3] - vpMatrix->m[1][2],
		vpMatrix->m[2][3] - vpMatrix->m[2][2],
		vpMatrix->m[3][3] - vpMatrix->m[3][2]
	);
}