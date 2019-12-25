#ifndef particle_h
#define particle_h


#include "vec3.h"
#include "transform.h"

#include "sprite.h"
#include "textureGroup.h"

#include "utilTypes.h"


// Collection of properties that describes a group of particles.
typedef struct particleDef {
	sprite spriteData;
	textureGroup *texGroup;
} particleDef;

typedef struct particle {
	transformState state;
	vec3 velocity;

	// The particle definition stores the actual
	// texture group that the particle uses.
	size_t currentAnim;
	#warning "We need a way of ensuring that the animation only uses one texture."
	#warning "It'd also be nice if we could store a bit less for animated textures."
	#warning "We could limit it so particles in a system can only play one animation. It's not like we'd have a good way of changing animations anyway."
	textureGroupAnim texAnim;

	// How much longer the particle can be alive for.
	float lifetime;
	// This is used for sorting particles with transparency.
	// We normally store the squared magnitude, not that it matters.
	float camDistance;
} particle;


void particleDefInit(particleDef *partDef);
void particleInit(particle *part);

void particleUpdate(particle *part, const float time);

return_t particleAlive(particle *part, const float time);

void particleDelete(particle *part);
void particleDefDelete(particleDef *partDef);



#endif