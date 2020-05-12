#ifndef physicsIsland_h
#define physicsIsland_h


#include "aabbTree.h"

#include "physicsCollider.h"


#ifndef PHYSISLAND_AABB_EXTRA_SPACE
	#define PHYSISLAND_AABB_EXTRA_SPACE 0.2f
#endif


typedef struct physicsIsland {
	aabbTree tree;
} physicsIsland;


void physIslandInit(physicsIsland *const restrict island);

void physIslandUpdateCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider);
void physIslandRemoveCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider);

#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physIslandQueryCollisions(physicsIsland *const restrict island, const float dt);
#else
void physIslandQueryCollisions(physicsIsland *const restrict island);
#endif

void physIslandDelete(physicsIsland *const restrict island);


#endif