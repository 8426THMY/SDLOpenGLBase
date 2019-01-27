#include "colliderAABB.h"


/*
** Increase the size of every face of an
** axis-aligned bounding box by "x".
** This is mainly used for the AABB tree.
*/
void colliderAABBFattenFloat(const colliderAABB *aabb, const float x, colliderAABB *out){
	vec3SubtractSFrom(&aabb->minPoint, x, &out->minPoint);
	vec3AddS(&aabb->maxPoint, x, &out->maxPoint);
}

/*
** Increase the size of an axis-aligned bounding box
** by a vec3, such as one representing linear velocity.
** This is mainly used for the AABB tree.
*/
void colliderAABBFattenVec3(const colliderAABB *aabb, const vec3 *v, colliderAABB *out){
	if(v->x >= 0.f){
		out->minPoint.x = aabb->minPoint.x;
		out->maxPoint.x = aabb->maxPoint.x + v->x;
	}else{
		out->minPoint.x = aabb->minPoint.x + v->x;
		out->maxPoint.x = aabb->maxPoint.x;
	}
	if(v->y >= 0.f){
		out->minPoint.y = aabb->minPoint.y;
		out->maxPoint.y = aabb->maxPoint.y + v->y;
	}else{
		out->minPoint.y = aabb->minPoint.y + v->y;
		out->maxPoint.y = aabb->maxPoint.y;
	}
	if(v->z >= 0.f){
		out->minPoint.z = aabb->minPoint.z;
		out->maxPoint.z = aabb->maxPoint.z + v->z;
	}else{
		out->minPoint.z = aabb->minPoint.z + v->z;
		out->maxPoint.z = aabb->maxPoint.z;
	}
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
** Return whether or not one axis-aligned
** bounding box completely envelops another.
*/
return_t colliderAABBEnvelopsAABB(const colliderAABB *aabbA, const colliderAABB *aabbB){
	return(
		aabbA->minPoint.x <= aabbB->minPoint.x &&
		aabbA->minPoint.y <= aabbB->minPoint.y &&
		aabbA->minPoint.z <= aabbB->minPoint.z &&

		aabbA->maxPoint.x >= aabbB->maxPoint.x &&
		aabbA->maxPoint.y >= aabbB->maxPoint.y &&
		aabbA->maxPoint.z >= aabbB->maxPoint.z
	);
}

/*
** Return whether or not two axis-aligned
** bounding boxes are intersecting each other.
*/
return_t colliderAABBCollidingAABB(const colliderAABB *aabbA, const colliderAABB *aabbB){
	return(
		aabbA->minPoint.x <= aabbB->maxPoint.x && aabbA->maxPoint.x >= aabbB->minPoint.x &&
		aabbA->minPoint.y <= aabbB->maxPoint.y && aabbA->maxPoint.y >= aabbB->minPoint.y &&
		aabbA->minPoint.z <= aabbB->maxPoint.z && aabbA->maxPoint.z >= aabbB->minPoint.z
	);
}