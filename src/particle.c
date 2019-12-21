#include "particle.h"


#include <math.h>


#include "sprite.h"


void particleDefInit(particleDef *partDef){
	partDef->meshData = spriteMeshDefault;
	partDef->texGroup = &texGroupDefault;
}

#warning "We should need only one of these."
void particleInit(particle *part){
	part->lifetime = 0.f;
	part->camDistance = -INFINITY;
}


void particleUpdate(particle *part, const float time){
	//
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
	part->camDistance = -INFINITY;
}

void particleDefDelete(particleDef *partDef){
	// Only delete the system's mesh if
	// it's not the default sprite mesh.
	if(meshDifferent(&partDef->meshData, &spriteMeshDefault)){
		meshDelete(&partDef->meshData);
	}
}