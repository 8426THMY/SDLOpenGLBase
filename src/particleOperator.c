#include "particleOperator.h"


void particleOperatorAddGravity(const void *const restrict operator, particle *const restrict part, const float time){
	vec3 force = {.x = 0.f, .y = -9.8f, .z = 0.f};
	// Integrate the object's velocity using symplectic Euler.
	vec3Fmaf(time, &force, &part->velocity);
}