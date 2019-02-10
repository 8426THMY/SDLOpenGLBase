#include "colliderAABB.h"


/*
** Increase the size of every face of an
** axis-aligned bounding box by "x".
** This is mainly used for the AABB tree.
*/
void colliderAABBExpandFloat(const colliderAABB *aabb, const float x, colliderAABB *out){
	vec3SubtractSFromOut(&aabb->min, x, &out->min);
	vec3AddSOut(&aabb->max, x, &out->max);
}

/*
** Increase the size of an axis-aligned bounding box
** by a vec3, such as one representing linear velocity.
** This is mainly used for the AABB tree.
*/
void colliderAABBExpandVec3(const colliderAABB *aabb, const vec3 *v, colliderAABB *out){
	if(v->x >= 0.f){
		out->min.x = aabb->min.x;
		out->max.x = aabb->max.x + v->x;
	}else{
		out->min.x = aabb->min.x + v->x;
		out->max.x = aabb->max.x;
	}
	if(v->y >= 0.f){
		out->min.y = aabb->min.y;
		out->max.y = aabb->max.y + v->y;
	}else{
		out->min.y = aabb->min.y + v->y;
		out->max.y = aabb->max.y;
	}
	if(v->z >= 0.f){
		out->min.z = aabb->min.z;
		out->max.z = aabb->max.z + v->z;
	}else{
		out->min.z = aabb->min.z + v->z;
		out->max.z = aabb->max.z;
	}
}

/*
** Create an axis-aligned bounding box that is
** large enough to fit two others inside it exactly.
*/
void colliderAABBCombine(const colliderAABB *aabbA, const colliderAABB *aabbB, colliderAABB *out){
	vec3Min(&aabbA->min, &aabbB->min, &out->min);
	vec3Max(&aabbA->max, &aabbB->max, &out->max);
}


//Return the volume of an axis-aligned bounding box.
float colliderAABBVolume(const colliderAABB *aabb){
	vec3 v;
	//The bounding box is axis-aligned, so this
	//will calculate the lengths of its sides.
	vec3SubtractVec3FromOut(&aabb->min, &aabb->max, &v);

	//Now we can just multiply the
	//lengths to find the volume.
	return(v.x * v.y * v.z);
}

//Return the surface area of an axis-aligned bounding box.
float colliderAABBSurfaceArea(const colliderAABB *aabb){
	vec3 v;
	//The bounding box is axis-aligned, so this
	//will calculate the lengths of its sides.
	vec3SubtractVec3FromOut(&aabb->min, &aabb->max, &v);

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
	vec3SubtractVec3FromOut(&aabb->min, &aabb->max, &v);

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

	vec3Min(&aabbA->min, &aabbB->min, &v1);
	vec3Max(&aabbA->max, &aabbB->max, &v2);
	vec3SubtractVec3From(&v1, &v2);

	return(v1.x * v1.y * v1.z);
}

/*
** Return the surface area of the axis-aligned bounding box
** that is large enough to contain two others completely.
*/
float colliderAABBCombinedSurfaceArea(const colliderAABB *aabbA, const colliderAABB *aabbB){
	vec3 v1;
	vec3 v2;

	vec3Min(&aabbA->min, &aabbB->min, &v1);
	vec3Max(&aabbA->max, &aabbB->max, &v2);
	vec3SubtractVec3From(&v1, &v2);

	return(2.f * (v1.x * (v1.y + v1.z) + v1.y * v1.z));
}

/*
** Return half the surface area of the axis-aligned bounding
** box that is large enough to contain two others completely.
*/
float colliderAABBCombinedSurfaceAreaHalf(const colliderAABB *aabbA, const colliderAABB *aabbB){
	vec3 v1;
	vec3 v2;

	vec3Min(&aabbA->min, &aabbB->min, &v1);
	vec3Max(&aabbA->max, &aabbB->max, &v2);
	vec3SubtractVec3From(&v1, &v2);

	return(v1.x * (v1.y + v1.z) + v1.y * v1.z);
}


/*
** Return whether or not one axis-aligned
** bounding box completely envelops another.
*/
return_t colliderAABBEnvelopsAABB(const colliderAABB *aabbA, const colliderAABB *aabbB){
	return(
		aabbA->min.x <= aabbB->min.x &&
		aabbA->min.y <= aabbB->min.y &&
		aabbA->min.z <= aabbB->min.z &&

		aabbA->max.x >= aabbB->max.x &&
		aabbA->max.y >= aabbB->max.y &&
		aabbA->max.z >= aabbB->max.z
	);
}

/*
** Return whether or not two axis-aligned
** bounding boxes are intersecting each other.
*/
return_t colliderAABBCollidingAABB(const colliderAABB *aabbA, const colliderAABB *aabbB){
	return(
		aabbA->min.x <= aabbB->max.x && aabbA->max.x >= aabbB->min.x &&
		aabbA->min.y <= aabbB->max.y && aabbA->max.y >= aabbB->min.y &&
		aabbA->min.z <= aabbB->max.z && aabbA->max.z >= aabbB->min.z
	);
}