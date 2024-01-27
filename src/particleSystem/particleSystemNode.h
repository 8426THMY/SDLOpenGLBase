#ifndef particleSystemNode_h
#define particleSystemNode_h


#include <stddef.h>

#include "particleEmitter.h"
#include "particleInitializer.h"
#include "particleOperator.h"
#include "particleConstraint.h"
#include "particleRenderer.h"
#include "particleManager.h"

#include "sort.h"

#include "utilTypes.h"


// Never inherit parent properties.
#define PARTICLE_INHERIT_NOTHING 0x00
// Specifies which properties a particle should inherit from its parent.
#define PARTICLE_INHERIT_POSITION  0x01
#define PARTICLE_INHERIT_SCALE     0x02
#define PARTICLE_INHERIT_ROTATION  0x04
#define PARTICLE_INHERIT_TRANSFORM ( \
	PARTICLE_INHERIT_POSITION |      \
	PARTICLE_INHERIT_SCALE    |      \
	PARTICLE_INHERIT_ROTATION        \
)
// Specifies that the property should only be
// inherited when the particle is first created.
#define PARTICLE_INHERIT_POSITION_CREATE  PARTICLE_INHERIT_POSITION
#define PARTICLE_INHERIT_SCALE_CREATE     PARTICLE_INHERIT_SCALE
#define PARTICLE_INHERIT_ROTATION_CREATE  PARTICLE_INHERIT_ROTATION
#define PARTICLE_INHERIT_TRANSFORM_CREATE PARTICLE_INHERIT_TRANSFORM
// Specifies that the property should be
// inherited as long as the parent is alive.
#define PARTICLE_INHERIT_POSITION_ALWAYS  (PARTICLE_INHERIT_POSITION | 0x08)
#define PARTICLE_INHERIT_SCALE_ALWAYS     (PARTICLE_INHERIT_SCALE    | 0x10)
#define PARTICLE_INHERIT_ROTATION_ALWAYS  (PARTICLE_INHERIT_ROTATION | 0x20)
#define PARTICLE_INHERIT_TRANSFORM_ALWAYS ( \
	PARTICLE_INHERIT_POSITION_ALWAYS |      \
	PARTICLE_INHERIT_SCALE_ALWAYS    |      \
	PARTICLE_INHERIT_ROTATION_ALWAYS        \
)

#warning "Particles should be able to inherit colours."

// In total, we allow for five different sorting modes:
//     1. 0x00: Don't do any sorting, just do whatever's fastest (default).
//     2. 0x01: Sort in order of creation.
//     3. 0x02: Sort in order of distance from the camera (back to front).
//     4. 0x05: Sort in reversed order of creation.
//     5. 0x06: Sort in reversed order of distance from the camera (front to back).
#define PARTICLE_SORT_NONE     0x00
#define PARTICLE_SORT_CREATION 0x01
#define PARTICLE_SORT_DISTANCE 0x02
#define PARTICLE_SORT_REVERSED 0x04
#define PARTICLE_SORT_CREATION_REVERSED (PARTICLE_SORT_CREATION | PARTICLE_SORT_REVERSED)
#define PARTICLE_SORT_DISTANCE_REVERSED (PARTICLE_SORT_DISTANCE | PARTICLE_SORT_REVERSED)


typedef struct particleSystemNodeDef particleSystemNodeDef;
typedef struct particleSystemNodeDef {
	// These properties control the behaviour of the particles.
	/// Note: If we want to have customizable parameters, we should
	/// add the arguments to the unions for these structures.
	particleEmitterDef *emitters;
	size_t numEmitters;
	particleInitializer *initializers;
	particleInitializer *lastInitializer;
	particleOperator *operators;
	particleOperator *lastOperator;
	particleConstraint *constraints;
	particleConstraint *lastConstraint;
	// A particle subsystem can have only one renderer!
	// We can achieve the effect of multiple renderers
	// by simply making a child subsystem that inherits
	// the transformation of its parent subsystem.
	particleRenderer renderer;

	// How long the system should be alive for.
	// If set to infinity, it will never die.
	float lifetime;
	// The maximum number of particles
	// that can be active at once.
	size_t maxParticles;
	flags_t inheritFlags;
	flags_t sortFlags;

	// Because this node's children are stored sequentially
	// in the main single list, we can simply store the number
	// of children and a pointer to the first child.
	particleSystemNodeDef *children;
	size_t numChildren;
} particleSystemNodeDef;

/*
** Particle system nodes are responsible for spawning particles
** and controlling their behaviour. Child subsystems are spawned
** by the parent's particles, and kept in the corresponding
** particle subsystem container.
*/
typedef struct particleSystemNodeContainer particleSystemNodeContainer;
typedef struct particleSubsystem particleSubsystem;
typedef struct particleSystemNode particleSystemNode;
typedef struct particleSystemNode {
	// Emitters are only active when the particle system is alive.
	// If the lifetime is less than or equal to 0, we should stop
	// spawning particles. The subsystem may then be deleted when
	// no more particles are alive.
	particleEmitter *emitters;

	// Stores and manages the particles spawned by this node.
	particleManager manager;

	// How much longer the system should live for.
	float lifetime;

	// Container that this subsystem lives in.
	particleSystemNodeContainer *container;
	// Subsystem that this subsystem lives in.
	particleSubsystem *parent;
	// Pointer to the previous and
	// next nodes in the subsystem.
	particleSystemNode *prevSibling;
	particleSystemNode *nextSibling;
} particleSystemNode;


void particleSysNodeInit(
	particleSystemNode *const restrict node,
	const particleSystemNodeDef *const restrict nodeDef
);

void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt);
void particleSysNodeUpdateEmitters(particleSystemNode *const restrict node, const float dt);

void particleSysNodePresort(
	particleSystemNode *const restrict node,
	const camera *const restrict cam
);
keyValue *const void particleSysNodeSort(
	particleSystemNode *const restrict node,
	const camera *const restrict cam, const float dt
);

void particleSysNodeOrphan(particleSystemNode *const restrict node);
void particleSysNodeDelete(particleSystemNode *const restrict node);
void particleSysNodeDefDelete(particleSystemNodeDef *const restrict nodeDef);


#endif