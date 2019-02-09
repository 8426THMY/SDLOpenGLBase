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


void physIslandInit(physicsIsland *island);

void physIslandUpdateCollider(physicsIsland *island, physicsCollider *collider);
void physIslandRemoveCollider(physicsIsland *island, physicsCollider *collider);

void physIslandQueryCollisions(physicsIsland *island, const float dt);

void physIslandDelete(physicsIsland *island);


#endif