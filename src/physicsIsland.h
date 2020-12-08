#ifndef physicsIsland_h
#define physicsIsland_h


#include "aabbTree.h"

#include "physicsConstraintPair.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"


#ifndef PHYSISLAND_AABBTREE_NODE_PADDING
	#define PHYSISLAND_AABBTREE_NODE_PADDING 0.2f
#endif

#ifndef PHYSICS_VELOCITY_SOLVER_NUM_ITERATIONS
	#define PHYSICS_VELOCITY_SOLVER_NUM_ITERATIONS 4
#endif
#ifndef PHYSICS_POSITION_SOLVER_NUM_ITERATIONS
	#define PHYSICS_POSITION_SOLVER_NUM_ITERATIONS 4
#endif


#warning "We should investigate how Randy Gaul and Erin Catto handle physics islands."


typedef struct physicsIsland {
	aabbTree tree;

	// We store doubly-linked lists of the resources that the island "owns".
	// Colliders will store their own lists of contacts and separations, for
	// instance, but they get inserted into the island's lists.
	physicsRigidBody *bodies;
	physicsContactPair *contacts;
	physicsSeparationPair *separations;
	physicsJointPair *joints;
} physicsIsland;


void physIslandInit(physicsIsland *const restrict island);

void physIslandRemoveCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider);
void physIslandInsertRigidBody(physicsIsland *const restrict island, physicsRigidBody *const body);
void physIslandRemoveRigidBody(physicsIsland *const restrict island, physicsRigidBody *const body);
void physIslandInsertRigidBodyList(physicsIsland *const restrict island, physicsRigidBody *const bodies, size_t numBodies);
void physIslandRemoveRigidBodyList(physicsIsland *const restrict island, physicsRigidBody *const bodies, size_t numBodies);

#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physIslandUpdate(physicsIsland *const restrict island, const float dt, const float frequency);
#else
void physIslandUpdate(physicsIsland *const restrict island, const float dt);
#endif

void physIslandDelete(physicsIsland *const restrict island);


#endif