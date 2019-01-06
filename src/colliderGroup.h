#ifndef colliderGroup_h
#define colliderGroup_h


#define COLLIDER_TYPE_GROUP -1


#include <stdlib.h>


typedef struct collider collider;
//Stores multiple colliders. This type can
//be used to roughly represent concave shapes.
typedef struct colliderGroup {
	collider *colliders;
	size_t numColliders;
} colliderGroup;


#endif