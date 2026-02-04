#ifndef particleSystemNode_h
#define particleSystemNode_h


#include <stddef.h>

#include "colliderAABB.h"

#include "transform.h"

#include "particleSubsystem.h"
#include "particleEmitter.h"
#include "particleInitializer.h"
#include "particleOperator.h"
#include "particleConstraint.h"
#include "particleRenderer.h"
#include "particleManager.h"
#include "particleSubsystem.h"

#include "sort.h"

#include "utilTypes.h"


#warning "Should particles be able to inherit velocities?"


// Never inherit parent properties.
#define PARTICLE_INHERIT_NOTHING 0x0000
// Specifies which properties a particle should inherit from its parent.
#define PARTICLE_INHERIT_POSITION  0x0001
#define PARTICLE_INHERIT_ROTATION  0x0002
#define PARTICLE_INHERIT_SCALE     0x0004
#define PARTICLE_INHERIT_TRANSFORM ( \
	PARTICLE_INHERIT_POSITION |      \
	PARTICLE_INHERIT_ROTATION |      \
	PARTICLE_INHERIT_SCALE           \
)
#define PARTICLE_INHERIT_LINEAR_VELOCITY  0x0040
#define PARTICLE_INHERIT_ANGULAR_VELOCITY 0x0080
#define PARTICLE_INHERIT_VELOCITY (    \
	PARTICLE_INHERIT_LINEAR_VELOCITY | \
	PARTICLE_INHERIT_ANGULAR_VELOCITY  \
)
#define PARTICLE_INHERIT_COLOUR 0x0100
	
// Specifies that the property should only be
// inherited when the particle is first created.
#define PARTICLE_INHERIT_POSITION_CREATE  PARTICLE_INHERIT_POSITION
#define PARTICLE_INHERIT_ROTATION_CREATE  PARTICLE_INHERIT_ROTATION
#define PARTICLE_INHERIT_SCALE_CREATE     PARTICLE_INHERIT_SCALE
#define PARTICLE_INHERIT_TRANSFORM_CREATE PARTICLE_INHERIT_TRANSFORM
#define PARTICLE_INHERIT_COLOUR_CREATE    PARTICLE_INHERIT_COLOUR
// Specifies that the property should be
// inherited as long as the parent is alive.
#define PARTICLE_INHERIT_POSITION_ALWAYS  (PARTICLE_INHERIT_POSITION | 0x0008)
#define PARTICLE_INHERIT_ROTATION_ALWAYS  (PARTICLE_INHERIT_ROTATION | 0x0010)
#define PARTICLE_INHERIT_SCALE_ALWAYS     (PARTICLE_INHERIT_SCALE    | 0x0020)
#define PARTICLE_INHERIT_TRANSFORM_ALWAYS ( \
	PARTICLE_INHERIT_POSITION_ALWAYS |      \
	PARTICLE_INHERIT_ROTATION_ALWAYS |      \
	PARTICLE_INHERIT_SCALE_ALWAYS           \
)
#define PARTICLE_INHERIT_COLOUR_ALWAYS    (PARTICLE_INHERIT_COLOUR | 0x0200)

#warning "Particles should be able to inherit colours."

// In total, we allow for five different sorting modes:
//     1. 0x0000: Don't do any sorting, just do whatever's fastest (default).
//     2. 0x0400: Sort in order of creation (oldest to youngest).
//     3. 0x0800: Sort in order of distance from the camera (back to front).
//     4. 0x1400: Sort in reversed order of creation (youngest to oldest).
//     5. 0x1800: Sort in reversed order of distance from the camera (front to back).
#define PARTICLE_SORT_NONE     0x0000
#define PARTICLE_SORT_CREATION 0x0400
#define PARTICLE_SORT_DISTANCE 0x0800
#define PARTICLE_SORT_REVERSED 0x1000
#define PARTICLE_SORT_CREATION_REVERSED (PARTICLE_SORT_CREATION | PARTICLE_SORT_REVERSED)
#define PARTICLE_SORT_DISTANCE_REVERSED (PARTICLE_SORT_DISTANCE | PARTICLE_SORT_REVERSED)

// These flags decide how a particle system can be destroyed:
//     1. 0x0000: Destroy the node once it expires (always active).
//     2. 0x2000: Destroy the node once its parent dies.
//     3. 0x4000: Destroy the node once its children are dead.
#define PARTICLE_DELETE_LIFETIME 0x0000
#define PARTICLE_DELETE_PARENT   0x2000
#define PARTICLE_DELETE_CHILDREN 0x4000


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
	#warning "It's probably fine to support multiple renderers."
	particleRenderer renderer;

	// How long the system should be alive for.
	// If set to infinity, it will never die.
	float lifetime;
	// The maximum number of particles
	// that can be active at once.
	size_t maxParticles;
	// The bounding box for the effect, used for
	// culling. This should be big enough to enclose
	// the total area that the effect can take up.
	colliderAABB aabb;
	flags16_t flags;

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
	// This bounding box tightly encloses the effect,
	// and is used as a narrowphase culling check.
	#warning "Should we create the AABB when updating the system or when rendering?"
	#warning "If we do it when rendering, it'll work with interpolation, but we're doing a lot of extra work."
	#warning "What if we generate an AABB for both updates, then interpolate them?"
	colliderAABB aabb;

	// Container that this subsystem lives in.
	particleSystemNodeContainer *container;
	// Subsystems that spawned this node.
	particleSubsystem *parent;
	// When the parent dies, we'll need to know where it was
	// last so that our particles know how to calculate their 
	// global states. This is a little inefficient, especially
	// when a particle spawns multiple nodes, but the alternative
	// is having particles store their states separately in memory.
	transform parentState;
	// Pointer to the previous and next nodes in the subsystem.
	particleSystemNode *prevSibling;
	particleSystemNode *nextSibling;
} particleSystemNode;


void particleSysNodeInit(
	particleSystemNode *const restrict node,
	const particleSystemNodeDef *const restrict nodeDef
);

void particleSysNodeUpdateParentTransform(
	particleSystemNode *const restrict node,
	const transform *const restrict parentState
);
void particleSysNodeUpdateParticles(particleSystemNode *const restrict node, const float dt);
void particleSysNodeUpdateEmitters(particleSystemNode *const restrict node, const float dt);
return_t particleSysNodeDead(const particleSystemNode *const restrict node);

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