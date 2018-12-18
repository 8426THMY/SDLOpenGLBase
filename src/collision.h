#ifndef collision_h
#define collision_h


#define COLLISION_MAX_CONTACTS 4


#warning "Should points be stored dynamically?"
//Stores every contact point involved in the collision.
typedef struct contactManifold {
	vec3 points[COLLISION_MAX_CONTACTS];
	size_t numPoints;
} contactManifold;


#endif