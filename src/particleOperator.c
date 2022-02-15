#include "particleOperator.h"


#warning "The force should be stored as part of the operator."
void particleOperatorAddGravity(const void *const restrict operator, particle *const restrict part, const float dt){
	const vec3 force = {.x = 0.f, .y = -9.8f, .z = 0.f};
	// Integrate the object's velocity using symplectic Euler.
	vec3Fmaf(dt, &force, &part->velocity);
}

#warning "We should allow decreasing by a multiple of 'dt'."
void particleOperatorDecayLifetime(const void *const restrict operator, particle *const restrict part, const float dt){
	part->lifetime -= dt;
}