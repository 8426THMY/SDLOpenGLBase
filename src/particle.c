#include "particle.h"


#include <math.h>

#include "sort.h"


void particleDefInit(particleDef *partDef){
	partDef->spriteData = g_spriteDefault;
	partDef->texGroup = &g_texGroupDefault;
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
** Compare two particles for use with sorting.
** Only three values can be returned:
**
** SORT_COMPARE_LESSER  - p1's distance from the camera is greater than p2's.
** SORT_COMPARE_EQUAL   - p1's distance from the camera is equal to p2's.
** SORT_COMPARE_GREATER - p1's distance from the camera is less than p2's.
*/
return_t particleCompare(const void *p1, const void *p2){
	// Since we want to draw farther particles first, we swap their order in this function.
	return(compareFloat(((particle *)p2)->camDistance, ((particle *)p1)->camDistance));
}


/*
** By setting the distance from the camera to negative
** infinity, we guarantee that the particle will be
** moved to the end of the array during sorting.
*/
#warning "Is this necessary?"
void particleDelete(particle *part){
	part->lifetime = 0.f;
	part->camDistance = -INFINITY;
}

void particleDefDelete(particleDef *partDef){
	// Only delete the system's mesh if
	// it's not the default sprite.
	if(spriteDifferent(&partDef->spriteData, &g_spriteDefault)){
		spriteDelete(&partDef->spriteData);
	}
}