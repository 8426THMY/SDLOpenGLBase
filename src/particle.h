#ifndef particle_h
#define particle_h


#include "vec3.h"
#include "transform.h"

#include "sprite.h"
#include "textureGroup.h"

#include "utilTypes.h"


// Collection of properties that describes a group of particles.
#warning "This should be part of the renderer."
typedef struct particleDef {
	sprite spriteData;
	// All of the particles in a particle
	// system must use the same texture.
	#warning "This is a bit dumb."
	const texture *tex;
	// The texture group stores the animations that the
	// particles can use. Note that particles are each
	// animated independently by changing UV coordinates.
	const textureGroup *texGroup;
} particleDef;

typedef struct particle particle;
typedef struct particle {
	transform state;
	vec3 velocity;
	vec3 angularVelocity;

	// Animation progress for the particle.
	#warning "We need a way of ensuring that the animation only uses one texture."
	#warning "Storing the texture in the particleDef works, but it's kind of bad."
	#warning "It'd also be nice if we could store a bit less for animated textures."
	#warning "Storing a textureGroupState here also is not ideal, as the particle shouldn't need to know the textureGroup."
	textureGroupState texState;

	// Although this will usually represent how much longer
	// the particle may live for, it can also represent other
	// things, such as how long it has been alive for.
	#warning "We currently don't control this with operators."
	float lifetime;

	// Doubly linked list pointers for depth sorting.
	const particle *next;
	const particle *prev;
} particle;


void particleDefInit(particleDef *const restrict partDef);
void particleInit(particle *const restrict part, const textureGroup *const texGroup);

void particleUpdate(particle *const restrict part, const float dt);

return_t particleDead(const particle *const restrict part);
return_t particleCompare(const void *const restrict p1, const void *const restrict p2);

void particleDelete(particle *const restrict part);
void particleDefDelete(particleDef *const restrict partDef);

return_t particleSetup();
void particleCleanup();



#endif