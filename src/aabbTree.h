#ifndef aabbTree_h
#define aabbTree_h


#include <stddef.h>

#include "settingsPhysics.h"

#include "colliderAABB.h"


#define AABBNODE_HEIGHT_LEAF        0
#define AABBNODE_HEIGHT_LAST_BRANCH 1

#define aabbNodeIsLeaf(node)       ((node)->height == AABBNODE_HEIGHT_LEAF)
#define aabbNodeIsLastBranch(node) ((node)->height == AABBNODE_HEIGHT_LAST_BRANCH)


typedef struct aabbNode aabbNode;
typedef struct aabbNodeChildren {
	aabbNode *left;
	aabbNode *right;
} aabbNodeChildren;

typedef struct aabbNodeLeaf {
	void *value;
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


void aabbTreeInit(aabbTree *const restrict tree);

aabbNode *aabbTreeInsertNode(
	aabbTree *const restrict tree, const colliderAABB *const restrict aabb,
	void *const restrict value, aabbNode *(*const allocate)()
);
void aabbTreeUpdateNode(aabbTree *const restrict tree, aabbNode *const restrict node);
void aabbTreeRemoveNode(
	aabbTree *const restrict tree, aabbNode *const restrict node, void (*const deallocate)(aabbNode *node, void *args), void *args
);

void aabbTreeTraverse(aabbTree *const restrict tree, void (*const callback)(aabbNode *node, void *args), void *args);
void aabbTreeQueryCollisions(
	aabbTree *const restrict tree, aabbNode *const node,
	void (*const callback)(void *const restrict d1, void *const restrict d2, void *args), void *args
);
void aabbTreeQueryCollisionsStack(
	aabbTree *const restrict tree, const aabbNode *const node,
	void (*const callback)(void *const restrict d1, void *const restrict d2, void *args), void *args
);
aabbNode *aabbTreeFindNextNode(
	aabbTree *const restrict tree, const colliderAABB *const restrict aabb, const aabbNode *const restrict prevNode
);


#endif