#ifndef physicsCollider_h
#define physicsCollider_h


#include "vec3.h"
#include "mat3.h"

#include "colliderMesh.h"


#define NUM_COLLIDER_TYPES 1
#define MAX_COLLIDER_SIZE sizeof(colliderMesh)


typedef struct physicsCollider {
	//Stores which type of collider this object is.
	unsigned char type;
	//This array should be large enough
	//tos store any type of collider.
	char data[MAX_COLLIDER_SIZE];

	//The collider's physical properties.
	float mass;
	float inverseMass;

	//Stores the rigid body's centre of mass.
	vec3 centroid;
} physicsCollider;


void colliderCalculateInertia(const physicsCollider *collider, const vec3 *centroid, float inertiaTensor[6]);


#endif