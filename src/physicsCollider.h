#ifndef physicsCollider_h
#define physicsCollider_h


#include "utilTypes.h"

#include "vec3.h"
#include "mat3.h"

#include "colliderHull.h"


#define COLLIDER_NUM_TYPES 1
#define COLLIDER_MAX_SIZE sizeof(colliderHull)


typedef struct physicsCollider {
	//Stores which type of collider this object is.
	byte_t type;
	//This array should be large enough
	//to store any type of collider.
	void *data[COLLIDER_MAX_SIZE];

	//The collider's physical properties.
	float mass;
	float inverseMass;

	//Stores the rigid body's centre of mass.
	vec3 centroid;
} physicsCollider;


void colliderCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]);


#endif