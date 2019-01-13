#include "colliderAABB.h"


/*
** Increase the size of every face of an
** axis-aligned bounding box by "num".
** This is mainly used for the AABB tree.
*/
void colliderAABBFatten(const colliderAABB *aabb, const float num, colliderAABB *out){
	vec3SubtractSFrom(&aabb->minPoint, num, &out->minPoint);
	vec3AddS(&aabb->maxPoint, num, &out->maxPoint);
}

/*
** Create an axis-aligned bounding box that is
** large enough to fit two others inside it exactly.
*/
void colliderAABBCombine(const colliderAABB *aabbA, const colliderAABB *aabbB, colliderAABB *out){
	vec3Min(&aabbA->minPoint, &aabbB->minPoint, &out->minPoint);
	vec3Max(&aabbA->maxPoint, &aabbB->maxPoint, &out->maxPoint);
}


//Return the volume of an axis-aligned bounding box.
float colliderAABBVolume(const colliderAABB *aabb){
	vec3 v;
	//The bounding box is axis-aligned, so this
	//will calculate the lengths of its sides.
	vec3SubtractVec3From(&aabb->minPoint, &aabb->maxPoint, &v);

	//Now we can just multiply the
	//lengths to find the volume.
	return(v.x * v.y * v.z);
}

//Return the surface area of an axis-aligned bounding box.
float colliderAABBSurfaceArea(const colliderAABB *aabb){
	vec3 v;
	//The bounding box is axis-aligned, so this
	//will calculate the lengths of its sides.
	vec3SubtractVec3From(&aabb->minPoint, &aabb->maxPoint, &v);

	//Optimised surface area calculation.
	//Instead of "2.f * (x * y + x * z + y * z)",
	//we can use "2.f * (x * (y + z) + y * z)".
    return(2.f * (v.x * (v.y + v.z) + v.y * v.z));
}

/*
** Return half of the surface area of an axis-aligned
** bounding box. In some cases, such as our AABB tree,
** we don't actually need to calculate the full area.
*/
float colliderAABBSurfaceAreaHalf(const colliderAABB *aabb){
	vec3 v;
	//The bounding box is axis-aligned, so this
	//will calculate the lengths of its sides.
	vec3SubtractVec3From(&aabb->minPoint, &aabb->maxPoint, &v);

	//Optimised surface area calculation.
	//Instead of "x * y + x * z + y * z",
	//we can use "x * (y + z) + y * z".
    return(v.x * (v.y + v.z) + v.y * v.z);
}


/*
** Return the volume of the axis-aligned bounding box
** that is large enough to contain two others completely.
*/
float colliderAABBCombinedVolume(const colliderAABB *aabbA, const colliderAABB *aabbB){
	vec3 v1;
	vec3 v2;

	vec3Min(&aabbA->minPoint, &aabbB->minPoint, &v1);
	vec3Max(&aabbA->maxPoint, &aabbB->maxPoint, &v2);
	vec3SubtractVec3From(&v1, &v2, &v1);

	return(v1.x * v1.y * v1.z);
}

/*
** Return the surface area of the axis-aligned bounding box
** that is large enough to contain two others completely.
*/
float colliderAABBCombinedSurfaceArea(const colliderAABB *aabbA, const colliderAABB *aabbB){
	vec3 v1;
	vec3 v2;

	vec3Min(&aabbA->minPoint, &aabbB->minPoint, &v1);
	vec3Max(&aabbA->maxPoint, &aabbB->maxPoint, &v2);
	vec3SubtractVec3From(&v1, &v2, &v1);

	return(2.f * (v1.x * (v1.y + v1.z) + v1.y * v1.z));
}

/*
** Return half the surface area of the axis-aligned bounding
** box that is large enough to contain two others completely.
*/
float colliderAABBCombinedSurfaceAreaHalf(const colliderAABB *aabbA, const colliderAABB *aabbB){
	vec3 v1;
	vec3 v2;

	vec3Min(&aabbA->minPoint, &aabbB->minPoint, &v1);
	vec3Max(&aabbA->maxPoint, &aabbB->maxPoint, &v2);
	vec3SubtractVec3From(&v1, &v2, &v1);

	return(v1.x * (v1.y + v1.z) + v1.y * v1.z);
}



/*
** Return whether or not two axis-aligned
** bounding boxes are intersecting each other.
*/
return_t colliderAABBColliding(const colliderAABB *aabbA, const colliderAABB *aabbB){
	return(
		(aabbA->minPoint.x <= aabbB->maxPoint.x && aabbA->maxPoint.x >= aabbB->minPoint.x) &&
		(aabbA->minPoint.y <= aabbB->maxPoint.y && aabbA->maxPoint.y >= aabbB->minPoint.y) &&
		(aabbA->minPoint.z <= aabbB->maxPoint.z && aabbA->maxPoint.z >= aabbB->minPoint.z)
	);
}