#include "physicsIsland.h"


#include "modulePhysics.h"

#include "physicsRigidBody.h"


void physIslandInit(physicsIsland *island){
	aabbTreeInit(&island->tree);
}


/*
** If a collider's new bounding box is no longer completely
** inside its old one, we'll need to update its tree node.
*/
void physIslandUpdateCollider(physicsIsland *island, physicsCollider *collider){
	if(!colliderAABBEnvelopsAABB(&collider->node->aabb, &collider->aabb)){
		#ifdef PHYSISLAND_AABB_EXPAND_BY_VELOCITY
		colliderAABBExpandVec3(&collider->aabb, &collider->owner->linearVelocity, &collider->node->aabb);
		colliderAABBExpandFloat(&collider->node->aabb, PHYSISLAND_AABB_EXTRA_SPACE, &collider->node->aabb);
		#else
		colliderAABBExpandFloat(&collider->aabb, PHYSISLAND_AABB_EXTRA_SPACE, &collider->node->aabb);
		#endif

		aabbTreeUpdateNode(&island->tree, collider->node);
	}
}

void physIslandRemoveCollider(physicsIsland *island, physicsCollider *collider){
	/** Should we clear the collider's pairs here? **/
	aabbTreeRemoveNode(&island->tree, collider->node, &modulePhysicsAABBNodeFree);
}


/*
** Check for collision between every collider in
** the island and update their collision pairs.
*/
void physIslandQueryCollisions(physicsIsland *island, const float dt){
	aabbNode *node = island->tree.leaves;
	physicsCollider *collider;

	while(node != NULL){
		//Update all of the current collider's separations and contacts.
		aabbTreeQueryCollisionsStack(&island->tree, node, &physColliderCollisionCallback);

		collider = (physicsCollider *)node->data.leaf.userData;
		node = node->data.leaf.next;

		//Remove any separations and contacts that are now inactive.
		physColliderUpdateSeparations(collider);
		//For valid contacts, we also need to compute the
		//new presolve information for each contact point.
		physColliderUpdateContacts(collider, dt);
	}
}


//Free every node in a physics island's tree.
void physIslandDelete(physicsIsland *island){
	aabbTreeTraverse(&island->tree, &modulePhysicsAABBNodeFree);
}