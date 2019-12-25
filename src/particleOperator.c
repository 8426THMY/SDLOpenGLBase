#include "particleOperator.h"


void particleOperatorAddGravity(const void *operator, particle *part, const float time){
	vec3 force = {.x = 0.f, .y = -9.8f, .z = 0.f};
	vec3 acceleration;

	// Integrate the object's velocity using symplectic Euler.
	vec3MultiplySOut(&force, time, &acceleration);
	vec3AddVec3(&part->velocity, &acceleration);
}