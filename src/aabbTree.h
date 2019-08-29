#ifndef aabbTree_h
#define aabbTree_h


#include <stddef.h>

#include "settingsPhysics.h"

#include "colliderAABB.h"


typedef struct aabbNode aabbNode;
typedef struct aabbNodeChildren {
	aabbNode *left;
	aabbNode *right;
} aabbNodeChildren;

typedef struct aabbNodeLeaf {
	void *userData;
	// Pointer to the next
	// leaf node in the list.
	aabbNode *next;
} aabbNodeLeaf;


typedef struct aabbNode {
	colliderAABB aabb;

	aabbNode *parent;
	union {
		aabbNodeChildren children;
		aabbNodeLeaf leaf;
	} data;

	// Specifies the node's distance from its deepest leaf.
	// If this node is a leaf itself, this will be zero.
	size_t height;
} aabbNode;

typedef struct aabbTree {
	aabbNode *root;
	// Linked list of leaf nodes.
	aabbNode *leaves;
} aabbTree;


void aabbTreeInit(aabbTree *tree);

aabbNode *aabbTreeInsertNode(aabbTree *tree, colliderAABB *aabb, void *userData, aabbNode *(*allocate)());
void aabbTreeUpdateNode(aabbTree *tree, aabbNode *node);
void aabbTreeRemoveNode(aabbTree *tree, aabbNode *node, void (*deallocate)(aabbNode *node));

void aabbTreeTraverse(aabbTree *tree, void (*callback)(aabbNode *node));
void aabbTreeQueryCollisions(aabbTree *tree, const aabbNode *node, void (*callback)(void *d1, void *d2));
void aabbTreeQueryCollisionsStack(aabbTree *tree, const aabbNode *node, void (*callback)(void *d1, void *d2));
aabbNode *aabbTreeFindNextNode(aabbTree *tree, const colliderAABB *aabb, const aabbNode *prevNode);


#endif