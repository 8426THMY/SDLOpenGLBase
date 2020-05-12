#include "physicsIsland.h"


#include "physicsRigidBody.h"

#include "modulePhysics.h"


void physIslandInit(physicsIsland *const restrict island){
	aabbTreeInit(&island->tree);
}


/*
** If a collider's new bounding box is no longer completely
** inside its old one, we'll need to update its tree node.
*/
void physIslandUpdateCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider){
	// If the collider isn't already part of an island, add it to this one!
	if(collider->node == NULL){
		collider->node = aabbTreeInsertNode(&island->tree, &collider->aabb, (void *)collider, &modulePhysicsAABBNodeAlloc);

	// Otherwise, update its node!
	}else if(!colliderAABBEnvelopsAABB(&collider->node->aabb, &collider->aabb)){
		#ifdef PHYSISLAND_AABB_EXPAND_BY_VELOCITY
		colliderAABBExpandVec3(&collider->aabb, &collider->owner->linearVelocity, &collider->node->aabb);
		colliderAABBExpandFloat(&collider->node->aabb, PHYSISLAND_AABB_EXTRA_SPACE, &collider->node->aabb);
		#else
		colliderAABBExpandFloat(&collider->aabb, PHYSISLAND_AABB_EXTRA_SPACE, &collider->node->aabb);
		#endif

		aabbTreeUpdateNode(&island->tree, collider->node);
	}
}

void physIslandRemoveCollider(physicsIsland *const restrict island, physicsCollider *const restrict collider){
	/** Should we clear the collider's pairs here? **/
	aabbTreeRemoveNode(&island->tree, collider->node, &modulePhysicsAABBNodeFree);
}


/*
** Check for collision between every collider in
** the island and update their collision pairs.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physIslandQueryCollisions(physicsIsland *const restrict island, const float dt){
#else
void physIslandQueryCollisions(physicsIsland *const restrict island){
#endif
	aabbNode *node = island->tree.leaves;
	physicsCollider *collider;

	while(node != NULL){
		// Update all of the current collider's separations and contacts.
		aabbTreeQueryCollisionsStack(&island->tree, node, &physColliderCollisionCallback);

		collider = (physicsCollider *)node->data.leaf.value;
		node = node->data.leaf.next;

		// Remove any separations and contacts that are now inactive.
		physColliderUpdateSeparations(collider);
		// For valid contacts, we also need to compute the
		// new presolve information for each contact point.
		#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		physColliderUpdateContacts(collider, dt);
		#else
		physColliderUpdateContacts(collider);
		#endif
	}
}


// Free every node in a physics island's tree.
void physIslandDelete(physicsIsland *const restrict island){
	aabbTreeTraverse(&island->tree, &modulePhysicsAABBNodeFree);
}