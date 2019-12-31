#include "particle.h"


#include <math.h>


void particleDefInit(particleDef *partDef){
	partDef->spriteData = spriteDefault;
	partDef->texGroup = &texGroupDefault;
}

void particleInit(particle *part){
	transformStateInit(&part->state);
	vec3InitZero(&part->velocity);

	part->currentAnim = 0;
	animationInit(&part->texAnim, ANIMATION_LOOP_INDEFINITELY);

	part->lifetime = INFINITY;
	part->camDistance = -INFINITY;
}


void particleUpdate(particle *part, const float time){
	vec3 velocityDelta;

	// Integrate the particle's position using symplectic Euler.
	vec3MultiplySOut(&part->velocity, time, &velocityDelta);
	vec3AddVec3(&part->state.pos, &velocityDelta);

	// Update the texture group animation, among other things.
}


// Return whether or not a particle is still alive.
return_t particleAlive(particle *part, const float time){
	part->lifetime -= time;
	return(part->lifetime > 0.f);
}


/*
** By setting the distance from the camera to negative
** infinity, we guarantee that the particle will be
** moved to the end of the array during sorting.
*/
void particleDelete(particle *part){
	part->lifetime = 0.f;
	part->camDistance = -INFINITY;
}

void particleDefDelete(particleDef *partDef){
	// Only delete the system's mesh if
	// it's not the default sprite.
	if(spriteDifferent(&partDef->spriteData, &spriteDefault)){
		spriteDelete(&partDef->spriteData);
	}
}