#ifndef particleSystemNode_h
#define particleSystemNode_h


#include <stddef.h>

#include "particleEmitter.h"
#include "particleInitializer.h"
#include "particleOperator.h"
#include "particleConstraint.h"
#include "particleRenderer.h"

#include "transform.h"

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

// In total, we allow for four different sorting modes:
//     1. 0x00: Sort in order of creation (default).
//     2. 0x40: Sort in order of distance from the camera.
//     3. 0x80: Sort in reversed order of creation.
//     4. 0xC0: Sort in reversed order of distance from the camera.
#define PARTICLE_SORT_DISTANCE 0x40
#define PARTICLE_SORT_REVERSED 0x80

// Masks for checking individual flag types.
#define PARTICLE_INHERIT_MASK 0x03
#define PARTICLE_SORT_MASK    0xC0


typedef struct particleSystemNodeDef particleSystemNodeDef;
typedef struct particleSystemNodeDef {
	// These properties control the behaviour of the particles.
	/// Note: If we want to have customizable parameters, we should
	///       add the arguments to the unions for these structures.
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
	flags_t flags;

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
typedef struct particle particle;
typedef struct particleSystemNode particleSystemNode;
typedef struct particleSystemNodeContainer particleSystemNodeContainer;
typedef struct particleSystemNodeContainer particleSystemNodeGroup;
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
	// Group that this subsystem lives in.
	particleSystemNodeGroup *group;
	// Pointers to the previous and next nodes with the same parent.
	// That is, the previous and next nodes in the group's list.
	particleSystemNode *prevSibling;
	particleSystemNode *nextSibling;
} particleSystemNode;


void particleSysNodeInit(
	particleSystemNode *const restrict node,
	const particleSystemNodeDef *const restrict nodeDef
);

void particleSysNodeUpdateEmitters(particleSystemNode *const restrict node, const float dt);
void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt);

void particleSysNodeOrphan(particleSystemNode *const restrict node);
void particleSysNodeDelete(particleSystemNode *const restrict node);
void particleSysNodeDefDelete(particleSystemNodeDef *const restrict nodeDef);


#endif