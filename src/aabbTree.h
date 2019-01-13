#ifndef aabbTree_h
#define aabbTree_h


#include <stddef.h>

#include "colliderAABB.h"


typedef struct aabbNode aabbNode;
typedef struct aabbNode {
	colliderAABB aabb;

	aabbNode *parent;
	union {
		struct {
			aabbNode *left;
			aabbNode *right;
		} children;
		//struct {
			void *body;
		//	aabbNode *next;
		//} leaf;
	} data;

	//Specifies the node's distance from its deepest leaf.
	//If this node is a leaf itself, this will be zero.
	size_t height;
} aabbNode;

typedef struct aabbTree {
	aabbNode *root;
} aabbTree;


void aabbTreeInsert(aabbTree *tree, colliderAABB *aabb, void *body);
void aabbTreeRemove(aabbTree *tree, aabbNode *node);


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