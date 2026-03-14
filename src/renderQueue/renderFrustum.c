#include "renderFrustum.h"


#include "vec3.h"

#include "utilMath.h"


/*
** Compute the six planes of the viewing
** frustum using Gribb and Hartmann's method.
*/
void renderFrustumInit(
	renderFrustum *const restrict frustum,
	const mat4 *const restrict vpMatrix
){

	vec4InitSet(&frustum->planes[RENDERFRUSTUM_PLANE_LEFT],
		vpMatrix->m[0][3] + vpMatrix->m[0][0],
		vpMatrix->m[1][3] + vpMatrix->m[1][0],
		vpMatrix->m[2][3] + vpMatrix->m[2][0],
		vpMatrix->m[3][3] + vpMatrix->m[3][0]
	);
	vec4InitSet(&frustum->planes[RENDERFRUSTUM_PLANE_RIGHT],
		vpMatrix->m[0][3] - vpMatrix->m[0][0],
		vpMatrix->m[1][3] - vpMatrix->m[1][0],
		vpMatrix->m[2][3] - vpMatrix->m[2][0],
		vpMatrix->m[3][3] - vpMatrix->m[3][0]
	);
	vec4InitSet(&frustum->planes[RENDERFRUSTUM_PLANE_BOTTOM],
		vpMatrix->m[0][3] + vpMatrix->m[0][1],
		vpMatrix->m[1][3] + vpMatrix->m[1][1],
		vpMatrix->m[2][3] + vpMatrix->m[2][1],
		vpMatrix->m[3][3] + vpMatrix->m[3][1]
	);
	vec4InitSet(&frustum->planes[RENDERFRUSTUM_PLANE_TOP],
		vpMatrix->m[0][3] - vpMatrix->m[0][1],
		vpMatrix->m[1][3] - vpMatrix->m[1][1],
		vpMatrix->m[2][3] - vpMatrix->m[2][1],
		vpMatrix->m[3][3] - vpMatrix->m[3][1]
	);
	vec4InitSet(&frustum->planes[RENDERFRUSTUM_PLANE_NEAR],
		vpMatrix->m[0][3] + vpMatrix->m[0][2],
		vpMatrix->m[1][3] + vpMatrix->m[1][2],
		vpMatrix->m[2][3] + vpMatrix->m[2][2],
		vpMatrix->m[3][3] + vpMatrix->m[3][2]
	);
	vec4InitSet(&frustum->planes[RENDERFRUSTUM_PLANE_FAR],
		vpMatrix->m[0][3] - vpMatrix->m[0][2],
		vpMatrix->m[1][3] - vpMatrix->m[1][2],
		vpMatrix->m[2][3] - vpMatrix->m[2][2],
		vpMatrix->m[3][3] - vpMatrix->m[3][2]
	);

	planeNormalizeFast(&frustum->planes[RENDERFRUSTUM_PLANE_LEFT]);
	planeNormalizeFast(&frustum->planes[RENDERFRUSTUM_PLANE_RIGHT]);
	planeNormalizeFast(&frustum->planes[RENDERFRUSTUM_PLANE_BOTTOM]);
	planeNormalizeFast(&frustum->planes[RENDERFRUSTUM_PLANE_TOP]);
	planeNormalizeFast(&frustum->planes[RENDERFRUSTUM_PLANE_NEAR]);
	planeNormalizeFast(&frustum->planes[RENDERFRUSTUM_PLANE_FAR]);
}


/*
** The frustum intersection algorithms used here have
** been adapted from the following article of Dion Picco:
**     https://www.flipcode.com/archives/Frustum_Culling.shtml
** Note that they use inward-pointing normals for their
** viewing frustum, whereas ours are outward-pointing.
*/

/*
** A sphere intersects the frustum if and only if it
** isn't completely in front of any planes of the frustum.
*/
return_t renderFrustumCollidingSphere(
	const renderFrustum *const restrict frustum,
	const colliderSphere *const restrict sphere
){

	const renderFrustumPlane *curPlane = frustum->planes;
	const renderFrustumPlane *const lastPlane = &frustum->planes[RENDERFRUSTUM_NUM_PLANES];
	for(; curPlane < lastPlane; ++curPlane){
		if(planePointDistVec3(curPlane, &sphere->pos) > sphere->radius){
			return(0);
		}
	}

	return(1);
}

/*
** An AABB intersects the frustum if and only if it
** isn't completely in front of any planes of the frustum.
** We use a slightly more optimized approach for checking
** AABB-plane intersections, adapted from the following:
**     https://github.com/gdbooks/3DCollisions/blob/master/Chapter2/static_aabb_plane.md
*/
return_t renderFrustumCollidingAABB(
	const renderFrustum *const restrict frustum,
	const colliderAABB *const restrict aabb
){

	const renderFrustumPlane *curPlane = frustum->planes;
	const renderFrustumPlane *const lastPlane = &frustum->planes[RENDERFRUSTUM_NUM_PLANES];
	for(; curPlane < lastPlane; ++curPlane){
		const vec3 midpoint = {
			.x = 0.5f*(aabb->min.x + aabb->max.x),
			.y = 0.5f*(aabb->min.y + aabb->max.y),
			.z = 0.5f*(aabb->min.z + aabb->max.z)
		};
		const vec3 extents = {
			.x = aabb->max.x - midpoint.x,
			.y = aabb->max.y - midpoint.y,
			.z = aabb->max.z - midpoint.z
		};
		// Compute the "projection radius" of the AABB onto the plane's
		// normal. This is the largest projection of any point on the
		// AABB (relative to its center) onto the plane's normal.
		const float r = extents.x*fabsf(curPlane->x) +
		                extents.y*fabsf(curPlane->y) +
		                extents.z*fabsf(curPlane->z);
		// If the signed distance of the AABB's center from
		// the plane is larger than the projection radius,
		// the AABB is completely in front of the plane.
		if(planePointDistVec3(curPlane, &midpoint) > r){
			return(0);
		}
	}

	return(1);
}

/*
** An AABB intersects the frustum if and only if it
** isn't completely in front of any planes of the frustum.
** This uses a kind of naive approach to checking whether
** the AABB intersects the planes of the frustum.
*/
return_t renderFrustumCollidingAABBAlt(
	const renderFrustum *const restrict frustum,
	const colliderAABB *const restrict aabb
){

	const renderFrustumPlane *curPlane = frustum->planes;
	const renderFrustumPlane *const lastPlane = &frustum->planes[RENDERFRUSTUM_NUM_PLANES];
	for(; curPlane < lastPlane; ++curPlane){
		// If each vertex of the AABB is in front of this
		// plane, the AABB can't be colliding with the frustum.
		if(
			planePointDist(curPlane, aabb->min.x, aabb->min.y, aabb->min.z) > 0 &&
			planePointDist(curPlane, aabb->min.x, aabb->min.y, aabb->max.z) > 0 &&
			planePointDist(curPlane, aabb->min.x, aabb->max.y, aabb->min.z) > 0 &&
			planePointDist(curPlane, aabb->min.x, aabb->max.y, aabb->max.z) > 0 &&
			planePointDist(curPlane, aabb->max.x, aabb->min.y, aabb->min.z) > 0 &&
			planePointDist(curPlane, aabb->max.x, aabb->min.y, aabb->max.z) > 0 &&
			planePointDist(curPlane, aabb->max.x, aabb->max.y, aabb->min.z) > 0 &&
			planePointDist(curPlane, aabb->max.x, aabb->max.y, aabb->max.z) > 0
		){

			return(0);
		}
	}

	return(1);
}