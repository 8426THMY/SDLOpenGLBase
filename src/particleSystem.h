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

#error "Alright, let's think about this properly."
#error "Here are some particle system postulates."
/**
1. There is (for now) only one particle structure that is shared by all renderers (for the following reason).
2. Particle subsystems can own multiple renderers (so we can render a beam with sprites at each vertex, for instance).
3. Particle subsystems store a sorted array of particles (for depth sorting).
4. The particle structure stores list pointers (so beam renderers know how to connect them).
5. Particles store texture group animation data.
6. Particle renderers store sprite and texture groups, and may override particle textures.
7. When a particle is spawned, child particle systems are instantiated at that particle.
**/

#if 0
/*
** // Pools
** //memoryPool g_partSubSysDefManager;
** memoryPool g_partSysDefManager;
** //memoryPool g_partContManager;
**
** // Single Lists
** memorySingleList g_partSubSysManager;
** memorySingleList g_partSysManager;
**
** // Non-Allocator List
** particleContainer *g_partContOrphans;
*/
/** We should store the particle system in the global allocator, not the containers.  **/
/** The idea is that containers stored by subsystems should not be in this allocator, **/
/** as this could lead to them being updated before we update their owner particles.  **/
/**                                                                                   **/
/** Alternatively, we could store a free list of orphaned subsystem containers, which **/
/** is only updated when one actually gets orphaned.                                  **/
/*
** Particle systems and subsystems should work as follows:
**     1. Objects store a list of particle systems that they own.
**     2. When objects are updated:
**         a. Owner states of the top-level containers are updated.
**         b. Top-level subsystem container is updated.
**     3. When subsystems are updated:
**         a. Particles are spawned.
**         b. Containers are allocated for new particles.
**         c. Particles are updated.
**         d. Containers of dead particles are orphaned.
**         e. Child subsystems are updated per particle.
**     4. When objects are deleted:
**         a. Set lifetimes of top-level subsystems to 0.
**         b. Add container to orphan list.
** Particle subsystems should be deleted when their lifetimes are
** 0 and they have no active particles. When an object that owns
** a subsystem container is deleted, the subsystems remain alive,
** and are added to an orphan list. All of the subsystems in this
** orphan list are then updated after all of the objects.
*/

/*
** A subsystem contains all of the information required
** to control and render part of a particle system.
*/
typedef struct particleSubSystemDef particleSubSystemDef;
typedef struct particleSubSystemDef {
	// The maximum number of particles that can be active at once.
	// This excludes children, and must be less than "SPRITE_MAX_INSTANCES".
	size_t maxParticles;
	// How long the system should be alive for.
	// If set to infinity, it will never die.
	float lifetime;

	// flags? (includes sorting mode)
	// inherit position
	// inherit velocity

	// These properties control the behaviour of the particles.
	/// Note: If we want to have customizable parameters, we should
	///       add the arguments to the unions for these structures.
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

	particleSubSystemDef *children;
	size_t numChildren;
} particleSubSystemDef;

/*
** A particle system is a tree of subsystems. An instance of each
** subsystem is spawned for each particle of the parent subsystem.
*/
typedef struct particleSystemDef {
	char *name;

	particleSubSystemDef *children;
	size_t numChildren;
} particleSystemDef;


/*
** A particle subsystem container is simply an array
** of particle subsystems that have been spawned by
** some object, including other particle subsystems.
*/
typedef struct particleSubsystemContainer {
	// This should typically be the transform
	// of whatever object owns this container.
	// Depending on the inheritance settings, we can either:
	//     1. Ignore this (never inherit).
	//     2. Add this to newly-created particles (inherit on create).
	//     3. Add this to particles when rendering (always inherit).
	// The owner is responsible for updating this.
	transform state;

	// Array of active top-level subsystems.
	particleSubSystem *children;
	size_t numChildren;
} particleSubsystemContainer;

/*
** Particle subsystem instance. These are spawned by
** parent subsystems, and store a particle container
** that in turn stores each of their child subsystems.
*/
typedef struct particleSubSystem {
	const particleSubSystemDef *partSubSysDef;

	// How much longer the system should live for.
	float lifetime;
	// Emitters are only active when the particle system is alive.
	// If the lifetime is less than or equal to 0, we should stop
	// spawning particles. The subsystem may then be deleted when
	// no more particles are alive.
	particleEmitter *emitters;

	// This array is always exactly big enough to store
	// the number of particles given by "maxParticles".
	//
	// Depending on the sorting mode specified in the
	// subsystem definition, this array may or may not
	// be sorted by depth.
	particle *particles;
	// Current number of spawned particles.
	size_t numParticles;
} particleSubSystem;

/*
** A particle system instance really just stores
** the top-level particle subsystem container.
*/
typedef struct particleSystem {
	/// This probably isn't necessary past initialization,
	/// but that can be handled by whatever owns this instance.
	const particleSystemDef *partSysDef;
	// Container storing the top-level particle subsystems.
	// To update the total system, we iterate through the
	// containers stored by the subsystems in this container.
	particleSubsystemContainer container;
} particleSystem;
#endif


#endif