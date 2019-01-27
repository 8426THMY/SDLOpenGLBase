#ifndef aabbTree_h
#define aabbTree_h


#include <stddef.h>

#include "colliderAABB.h"


typedef struct aabbNode aabbNode;
typedef struct aabbNodeChildren {
	aabbNode *left;
	aabbNode *right;
} aabbNodeChildren;

typedef struct aabbNodeLeaf {
	void *collider;
} aabbNodeLeaf;


typedef struct aabbNode {
	colliderAABB aabb;

	aabbNode *parent;
	union {
		aabbNodeChildren children;
		aabbNodeLeaf leaf;
	} data;

	//Specifies the node's distance from its deepest leaf.
	//If this node is a leaf itself, this will be zero.
	size_t height;
} aabbNode;

typedef struct aabbTree {
	aabbNode *root;
} aabbTree;


aabbNode *aabbTreeInsertNode(aabbTree *tree, colliderAABB *aabb, void *collider);
void aabbTreeUpdateNode(aabbTree *tree, aabbNode *node);
void aabbTreeRemoveNode(aabbTree *tree, aabbNode *node);

void aabbTreeQueryCollisions(aabbTree *tree, const aabbNode *node, void (*callback)(void *cA, void *cB));
void aabbTreeQueryCollisionsStack(aabbTree *tree, const aabbNode *node, void (*callback)(void *cA, void *cB));
aabbNode *aabbTreeFindNextNode(aabbTree *tree, const colliderAABB *aabb, const aabbNode *prevNode);


/**
//Update bodies.
for each object {
	Integrate velocity;

	//When we update the AABB, we store
	//the new one in the AABB tree node.
	Update vertices and AABB;
}

//Update broadphase.
//AABB tree nodes are added and removed
//when a new collider is added or removed.
for each object {
	for each collider {
		Update node in AABB tree;
	}
}

//Update collider pairs.
for each object {
	//
}


//Rigid bodies store an array of collision pairs involving
//themselves and any rigid bodies that have a larger pointer.
//Collision pairs thus need to store the pointers to the
//bodies involved in the collision/separation in order.
for each possible collision pair in AABB tree {
	curPair;
	if collision pair already exists in body A's array {
		curPair = pairInArray;
	}else{
		Add this pair to body A's array;
		curPair = newPair;
	}
	Narrowphase for curPair;
}
**/


#endif