#include "particle.h"


void particleDefInit(particleDef *partDef){
	meshInit(&partDef->meshData);
	partDef->texGroup = NULL;
}


// Update the particle and return whether or not it's still alive.
return_t particleUpdate(particle *part, const float time){
	part->lifetime -= time;
	// Fail if the particle has died.
	if(part->lifetime <= 0.f){
		return(0);
	}

	return(1);
}