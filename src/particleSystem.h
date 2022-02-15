#ifndef particleSystem_h
#define particleSystem_h


#include <stddef.h>

#include "vec3.h"
#include "quat.h"
#include "transform.h"

#include "camera.h"

#include "particle.h"
#include "particleInitializer.h"
#include "particleEmitter.h"
#include "particleOperator.h"
#include "particleConstraint.h"
#include "particleRenderer.h"

#include "sprite.h"
#include "settingsSprites.h"

#include "utilTypes.h"


/**
*** Maybe investigate soft particles? They don't seem too hard to implement.
*** http://alessandroribeiro.thegeneralsolution.com/en/2021/01/09/openglstarter-soft-particles/
*** https://www.youtube.com/watch?v=VT6OaSV5d38
**/


/*
** Children don't have to be rendered at exactly the same
** time as the rest of the particles/children in a system.
*/
typedef struct particleSystemDef particleSystemDef;
#warning "This isn't used at the moment. We also need a version for instantiated children."
typedef struct particleSystemChildDef {
	particleSystemDef *partSysDef;
	// How long to delay the rendering of
	// this system from the parent system.
	float delay;
} particleSystemChildDef;


/*
** Particle systems control the rendering and emission of a single particle
** particle effect. Although the particles may only use one texture, we can
** have any number of child systems that need not use the same texture.
*/
#warning "It'd also be nice if we had a way of deciding how many times a system should loop after all the particles have died."
#warning "Maybe add support for multiple renderers? Let billboarding be optional for the default one."
typedef struct particleSystemDef {
	char *name;

	// Base particle properties.
	particleDef properties;
	// Number of particles to begin with.
	size_t initialParticles;
	// The maximum number of particles that the system can spawn, excluding those
	// spawned by its children. This can be no greater than "SPRITE_MAX_INSTANCES".
	size_t maxParticles;

	// These properties control the behaviour of the particles.
	particleInitializer *initializers;
	particleInitializer *lastInitializer;
	particleEmitterDef *emitters;
	size_t numEmitters;
	particleOperator *operators;
	particleOperator *lastOperator;
	particleConstraint *constraints;
	particleConstraint *lastConstraint;
	particleRenderer *renderers;
	particleRenderer *lastRenderer;

	// How long the system should live for.
	// If set to infinity, it will never die.
	float lifetime;

	particleSystemDef *children;
	size_t numChildren;
} particleSystemDef;

// This is simply an instance of a particle system definition.
#warning "It'd be good if we had a way of choosing whether or not to sort particles. Of course, we'd still need to move dead particles to the end of the array."
typedef struct particleSystem particleSystem;
typedef struct particleSystem {
	const particleSystemDef *partSysDef;
	particleEmitter *emitters;

	#warning "We don't currently support scaling particle systems. Need to think about this some more."
	vec3 pos;
	quat rot;
	// How much longer the system should live for.
	#warning "Currently unused(ish)."
	float lifetime;

	// This array is always exactly big enough to store
	// the number of particles given by "maxParticles".
	particle *particles;
	// Current number of spawned particles.
	size_t numParticles;

	particleSystem *children;
} particleSystem;


void particleSysDefInit(particleSystemDef *const restrict partSysDef);
void particleSysInit(particleSystem *const restrict partSys, const particleSystemDef *const restrict partSysDef);

void particleSysUpdate(particleSystem *const restrict partSys, const float dt);
void particleSysDraw(
	const particleSystem *const restrict partSys, const camera *const restrict cam,
	const spriteShader *const restrict shader, const float dt
);

return_t particleSysAlive(particleSystem *const restrict partSys, const float dt);

void particleSysDelete(particleSystem *const restrict partSys);
void particleSysDefDelete(particleSystemDef *const restrict partSysDef);


/**
// Particles are rendered in order of depth, where
// those farther from the camera are rendered last.
typedef struct spriteRenderer {
	//
} spriteRenderer;


// Particles should be joined in the order they were created.
// We can probably use depth sorting to construct the triangles
// in a way that avoids translucency issues with self-overlap.
typedef struct beamRenderer {
	//
} beamRenderer;


Source engine allocates particles in a doubly linked list.
**/

#error "This works, but it would be nice if we didn't have to store so many list pointers..."
#error "Alternatively, we could use something like the Unreal Engine, where emitters keep track of their particles and render them."
#error "This would also allow us to save space, as certain renderers don't need certain particle information."
#error "To get a good feel for how it works, check Effekseer's 'Simple_Turbulence_Fireworks' effect."
/**
Init:
1. Store particles in a global doubly linked list allocator (size is the maximum allowable number of particles).
2. Particles store a pointer to the next particle created after them.
3. Particle systems store a pointer to the oldest particle.

Update:
1. Iterate through particles from oldest to youngest (so it's easier to fix the pointers).
2. When a particle is created or destroyed, call renderer update functions.
3. Renderers are able to track certain particle information exclusive to them.

Render:
1. Iterate through the particles, computing their depths.
2. Sort the particles using their linked list pointers.
3. For each renderer, iterate through the particles in depth order, computing their render states.
**/
/*
** We have two choices for depth sorting:
**     1. Sort the particles in an array by copying (slow sort, fast access, loses creation order).
**     2. Sort the particles as a doubly linked list (fast sort, slow access, can keep creation order).
**
** For creation order sorting:
** //
*/


#endif