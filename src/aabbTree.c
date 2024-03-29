#include "aabbTree.h"


#include "utilMath.h"


#define AABBTREE_QUERY_STACK_SIZE 256


/*
** Based off Randy Gaul's dynamic AABB tree implementation:
**     https://github.com/RandyGaul/qu3e/blob/master/src/broadphase/q3DynamicAABBTree.cpp
*/


// Forward-declare any helper functions!
static aabbNode *balanceNode(aabbTree *const restrict tree, aabbNode *const restrict node);
static void balanceHierarchy(aabbTree *const restrict tree, aabbNode *const restrict node);
static void insertLeaf(aabbTree *const restrict tree, aabbNode *const restrict node, aabbNode *const restrict parent);
static void removeLeaf(aabbTree *const restrict tree, aabbNode *const restrict node);


void aabbTreeInit(aabbTree *const restrict tree){
	tree->root = NULL;
	tree->leaves = NULL;
}


// Add the user's data to the tree.
aabbNode *aabbTreeInsertNode(
	aabbTree *const restrict tree, const colliderAABB *const restrict aabb,
	void *const restrict value, aabbNode *(*const allocate)()
){

	aabbNode *const node = (*allocate)();
	if(node == NULL){
		/** MALLOC FAILED **/
	}

	node->aabb = *aabb;
	node->data.leaf.value = value;
	node->data.leaf.next = tree->leaves;
	node->height = AABBNODE_HEIGHT_LEAF;
	tree->leaves = node;

	if(tree->root != NULL){
		aabbNode *const parent = (*allocate)();
		if(parent == NULL){
			/** MALLOC FAILED **/
		}
		insertLeaf(tree, node, parent);

	// If the tree has no nodes,
	// add this one as the root node.
	}else{
		tree->root = node;
		node->parent = NULL;
	}

	return(node);
}

/*
** If the node's axis-aligned bounding box is no longer completely
** enveloped by its old, fattened bounding box, we must reinsert it.
*/
void aabbTreeUpdateNode(aabbTree *const restrict tree, aabbNode *const restrict node){
	// We only need to reinsert the node if
	// it is not the only node in the tree.
	if(node != tree->root){
		removeLeaf(tree, node);
		insertLeaf(tree, node, node->parent);
	}
}

// Remove the user's data from the tree.
void aabbTreeRemoveNode(
	aabbTree *const restrict tree, aabbNode *const restrict node,
	void (*const deallocate)(aabbNode *node, void *args), void *args
){

	// If we are not deleting the root node, we should
	// replace the node's parent with its sibling.
	if(node != tree->root){
		removeLeaf(tree, node);
		(*deallocate)(node->parent, args);

	// If we're deleting the root node, it cannot
	// have any children since it's a leaf, so we
	// can just update the tree's root pointer.
	}else{
		tree->root = NULL;
	}

	(*deallocate)(node, args);
}


/*
** Traverse the tree in post-order and run the function "callback"
** on every node. This is primarily used for memory deallocation.
*/
void aabbTreeTraverse(aabbTree *const restrict tree, void (*const callback)(aabbNode *node, void *args), void *args){
	aabbNode *node = tree->root;

	if(node != NULL){
		aabbNode *parent;

		// Start on the left-most node.
		while(!aabbNodeIsLeaf(node)){
			node = node->data.children.left;
		}

		do {
			// If we're at a branch, find the
			// left-most node of its right subtree.
			if(!aabbNodeIsLeaf(node)){
				node = node->data.children.right;
				while(!aabbNodeIsLeaf(node)){
					node = node->data.children.left;
				}
			}

			// Continue climbing the tree
			// while we're in a right subtree
			while(
				(parent = node->parent) != NULL &&
				node == parent->data.children.right
			){
				(*callback)(node, args);
				node = parent;
			}
			(*callback)(node, args);
			node = parent;
		} while(node != NULL);
	}
}

/*
** Traverse the tree in order searching for leaves that collide
** with the input leaf. If there is a collision, run the function
** "callback" with the two potentially colliding leaf nodes.
*/
void aabbTreeQueryCollisions(
	aabbTree *const restrict tree, aabbNode *const node,
	void (*const callback)(void *const restrict d1, void *const restrict d2, void *args), void *args
){

	aabbNode *curNode = tree->root;

	// Make sure the tree isn't empty.
	if(curNode != NULL){
		// Our loop won't work if the tree's root is its only leaf.
		if(!aabbNodeIsLeaf(curNode)){
			for(;;){
				aabbNode *parent;

				// Keep searching down the tree until we find either
				// a branch that our bounding box doesn't collide with
				// or a branch that parents two leaf nodes.
				while(colliderAABBCollidingAABB(&node->aabb, &curNode->aabb)){
					// If we've found a node who parents two leaves, we
					// can run the callback function with its children.
					if(aabbNodeIsLastBranch(curNode)){
						(*callback)(node->data.leaf.value, curNode->data.children.left->data.leaf.value, args);
						(*callback)(node->data.leaf.value, curNode->data.children.right->data.leaf.value, args);

						break;
					}else{
						curNode = curNode->data.children.left;
					}
				}

				// Now that we've reached the end of the current branch,
				// we'll need to keep climbing the tree until we reach
				// a node whose right branch hasn't been explored.
				for(;;){
					parent = curNode->parent;
					// If we've reached the root node, we've
					// searched the entire tree and can exit.
					if(parent == NULL){
						return;
					}
					// If our node is the left child of its parent,
					// the right subtree has not yet been explored.
					if(curNode == parent->data.children.left){
						curNode = parent->data.children.right;
						break;
					}
					curNode = parent;
				}
			}

		// If our tree only has one node, we
		// can simply run our callback on it.
		}else{
			(*callback)(node->data.leaf.value, curNode->data.leaf.value, args);
		}
	}
}

/*
** Similar to the function above, but this implementation
** utilises a stack to prevent crossing any nodes twice.
*/
void aabbTreeQueryCollisionsStack(
	aabbTree *const restrict tree, const aabbNode *const node,
	void (*const callback)(void *const restrict d1, void *const restrict d2, void *args), void *args
){

	aabbNode *stack[AABBTREE_QUERY_STACK_SIZE];
	size_t i = 1;

	stack[0] = tree->root;

	do {
		aabbNode *curNode = stack[--i];

		if(aabbNodeIsLeaf(curNode)){
			(*callback)(node->data.leaf.value, curNode->data.leaf.value, args);
		}else if(colliderAABBCollidingAABB(&node->aabb, &curNode->aabb)){
			stack[i] = curNode->data.children.left;
			++i;
			stack[i] = curNode->data.children.right;
			++i;
		}
	} while(i);
}

/*
** Traverse the tree in order and return first node
** following "prevNode" that may be colliding with "aabb".
*/
aabbNode *aabbTreeFindNextNode(
	aabbTree *const restrict tree, const colliderAABB *const restrict aabb, const aabbNode *const restrict prevNode
){

	aabbNode *curNode = tree->root;

	if(curNode != NULL){
		// If the tree has more than one node, we'll
		// need to find the next node after "prevNode".
		if(!aabbNodeIsLeaf(curNode)){
			aabbNode *parent;

			// If a previous node was specified, we may have
			// some setup to do before we can find the next node.
			if(prevNode != NULL){
				curNode = prevNode->parent;
				// If our previous node was the left child of
				// its branch node, check the node's sibling.
				if(prevNode == curNode->data.children.left){
					aabbNode *sibling = curNode->data.children.right;
					// We can only return the node if
					// there's actually a collision.
					if(colliderAABBCollidingAABB(aabb, &sibling->aabb)){
						return(sibling);
					}
				}

				// Keep climbing the tree until we reach a
				// node that is the left child of its parent.
				for(;;){
					parent = curNode->parent;
					// If we've reached the root node, we've
					// searched the entire tree and can exit.
					if(parent == NULL){
						return(NULL);
					}
					// If our node is the left child of its parent,
					// the right subtree has not yet been explored.
					if(curNode == parent->data.children.left){
						curNode = parent->data.children.right;
						break;
					}
					curNode = parent;
				}
				// We can now start searching
				// this node's sibling branch.
				curNode = parent->data.children.right;
			}

			// Find the left-most node that our input
			// bounding box may be colliding with.
			for(;;){
				// Keep going left until we either find a branch that
				// our node doesn't collide with or we reach a leaf.
				if(colliderAABBCollidingAABB(aabb, &curNode->aabb)){
					if(aabbNodeIsLeaf(curNode)){
						return(curNode);
					}

					curNode = curNode->data.children.left;

				}else{
					// Keep climbing the tree until we reach a
					// node that is the left child of its parent.
					for(;;){
						parent = curNode->parent;
						// If we've reached the root node, we've
						// searched the entire tree and can exit.
						if(parent == NULL){
							return(NULL);
						}
						// If our node is the left child of its parent,
						// the right subtree has not yet been explored.
						if(curNode == parent->data.children.left){
							curNode = parent->data.children.right;
							break;
						}
						curNode = parent;
					}
				}
			}

		// Only return the root node if this is
		// the first time this function was called.
		}else if(prevNode == NULL && colliderAABBCollidingAABB(aabb, &curNode->aabb)){
			return(curNode);
		}
	}

	return(NULL);
}


/*
** Perform a left or right rotate and fix up any invalid
** bounding boxes to restore balance to a single node.
*/
static aabbNode *balanceNode(aabbTree *const restrict tree, aabbNode *const restrict node){
	// We only need to perform a rotation of this node
	// is not a leaf and it does not parent any leaves.
	if(node->height > 1){
		aabbNode *const parent = node->parent;
		aabbNode *const left   = node->data.children.left;
		aabbNode *const right  = node->data.children.right;

		const int balance = right->height - left->height;
		// If the node's right branch is significantly deeper
		// than its left branch, we must rotate the node left.
		if(balance > 1){
			aabbNode *const grandLeft = right->data.children.left;
			aabbNode *const grandRight = right->data.children.right;

			// Replace the node with its right child.
			if(parent != NULL){
				if(parent->data.children.left == node){
					parent->data.children.left = right;
				}else{
					parent->data.children.right = right;
				}
			}else{
				tree->root = right;
			}
			right->parent = parent;
			right->data.children.left = node;
			node->parent = right;

			// Now that the node is right's left child, we should make
			// right's right child the grandchild whose branch ran deeper.
			if(grandLeft->height >= grandRight->height){
				right->data.children.right = grandLeft;
				node->data.children.right  = grandRight;
				grandRight->parent = node;

				colliderAABBCombine(&left->aabb, &grandRight->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandLeft->aabb, &right->aabb);

				node->height  = uintMax(left->height, grandRight->height) + 1;
				right->height = uintMax(node->height, grandLeft->height) + 1;
			}else{
				right->data.children.right = grandRight;
				node->data.children.right  = grandLeft;
				grandLeft->parent = node;

				colliderAABBCombine(&left->aabb, &grandLeft->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandRight->aabb, &right->aabb);

				node->height  = uintMax(left->height, grandLeft->height) + 1;
				right->height = uintMax(node->height, grandRight->height) + 1;
			}


			return(right);

		// Otherwise, if the node's left branch is significantly
		// deeper than its right branch, we must rotate it right.
		}else if(balance < -1){
			aabbNode *const grandLeft = left->data.children.left;
			aabbNode *const grandRight = left->data.children.right;

			// Replace the node with its left child.
			if(parent != NULL){
				if(parent->data.children.left == node){
					parent->data.children.left = left;
				}else{
					parent->data.children.right = left;
				}
			}else{
				tree->root = left;
			}
			left->parent = parent;
			left->data.children.right = node;
			node->parent = left;

			// Now that the node is left's right child, we should make
			// left's left child the grandchild whose branch ran deeper.
			if(grandLeft->height >= grandRight->height){
				left->data.children.left = grandLeft;
				node->data.children.left = grandRight;
				grandRight->parent = node;

				colliderAABBCombine(&right->aabb, &grandRight->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandLeft->aabb, &left->aabb);

				node->height = uintMax(right->height, grandRight->height) + 1;
				left->height = uintMax(node->height, grandLeft->height) + 1;
			}else{
				left->data.children.left = grandRight;
				node->data.children.left = grandLeft;
				grandLeft->parent = node;

				colliderAABBCombine(&right->aabb, &grandLeft->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandRight->aabb, &left->aabb);

				node->height = uintMax(right->height, grandLeft->height) + 1;
				left->height = uintMax(node->height, grandRight->height) + 1;
			}


			return(left);
		}
	}

	return(node);
}

/*
** Iteratively restore balance to every
** node above the one specified by "node".
*/
static void balanceHierarchy(aabbTree *const restrict tree, aabbNode *node){
	do {
		aabbNode *left;
		aabbNode *right;

		node = balanceNode(tree, node);
		left = node->data.children.left;
		right = node->data.children.right;

		// Fix up the node's properties to represent its new children.
		colliderAABBCombine(&left->aabb, &right->aabb, &node->aabb);
		node->height = uintMax(left->height, right->height) + 1;

		// Continue to the node's parent.
		node = node->parent;
	} while(node != NULL);
}


/*
** Insert a leaf node into the tree. We assume that both
** "node" and "parent" have been allocated outside this function.
** We also assume that the tree is not completely empty.
*/
static void insertLeaf(aabbTree *const restrict tree, aabbNode *const restrict node, aabbNode *const restrict parent){
	aabbNode *sibling = tree->root;
	aabbNode *siblingParent;

	// Find a leaf node to serve as our new node's sibling.
	while(!aabbNodeIsLeaf(sibling)){
		aabbNode *const left = sibling->data.children.left;
		aabbNode *const right = sibling->data.children.right;

		// Calculate the cost of starting a new branch.
		const float combinedArea  = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &sibling->aabb);
		const float branchCost    = 2.f * combinedArea;
		const float inheritedCost = 2.f * (combinedArea - colliderAABBSurfaceAreaHalf(&sibling->aabb));

		float leftCost;
		float rightCost;

		// Calculate the total increase in surface area that we will get
		// after combining our node with the current one's left child.
		leftCost = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &left->aabb) + inheritedCost;
		// If the child is a leaf node, the cost must
		// include the creation of a new branch node.
		if(!aabbNodeIsLeaf(left)){
			leftCost -= colliderAABBSurfaceAreaHalf(&left->aabb);
		}

		// Calculate the total increase in surface area that we will get
		// after combining our node with the current one's right child.
		rightCost = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &right->aabb) + inheritedCost;
		// If the child is a leaf node, the cost must
		// include the creation of a new branch node.
		if(!aabbNodeIsLeaf(right)){
			rightCost -= colliderAABBSurfaceAreaHalf(&right->aabb);
		}

		// If it's cheapest to create a new
		// branch, we can exit the loop here.
		if(branchCost < leftCost && branchCost < rightCost){
			break;
		}
		// Otherwise, continue on
		// to the cheaper child.
		if(leftCost <= rightCost){
			sibling = left;
		}else{
			sibling = right;
		}
	}


	// We assume that the node "parent" has already been allocated.
	// It will become a new branch node to parent our two leaves.
	siblingParent = sibling->parent;

	colliderAABBCombine(&node->aabb, &sibling->aabb, &parent->aabb);
	parent->parent = siblingParent;
	parent->data.children.left = sibling;
	parent->data.children.right = node;
	parent->height = sibling->height + 1;

	node->parent = parent;
	sibling->parent = parent;

	// If the node's sibling was not the tree's root, we'll
	// need to make its parent point to the new parent node.
	if(siblingParent != NULL){
		if(siblingParent->data.children.left == sibling){
			siblingParent->data.children.left = parent;
		}else{
			siblingParent->data.children.right = parent;
		}

	// If the sibling node was the tree's root,
	// we'll need to update the root pointer.
	}else{
		tree->root = parent;
	}


	// Ensure that the tree is correctly balanced.
	balanceHierarchy(tree, parent);
}

/*
** Remove a leaf node from the AABB tree. We assume that both
** "node" and "parent" are deallocated outside of this function.
** We also assume that "node" is not the tree's root node.
*/
static void removeLeaf(aabbTree *const restrict tree, aabbNode *const restrict node){
	const aabbNode *const parent = node->parent;
	aabbNode *const grandparent = parent->parent;
	aabbNode *const sibling = (parent->data.children.left == node) ? parent->data.children.right : parent->data.children.left;

	// If the node is not a child of the root node,
	// we should replace its parent with its sibling.
	if(grandparent != NULL){
		if(grandparent->data.children.left == parent){
			grandparent->data.children.left = sibling;
		}else{
			grandparent->data.children.right = sibling;
		}
		sibling->parent = grandparent;

		// Ensure that the tree is correctly balanced.
		// If the node did not have a grandparent,
		// there is no need to rebalance the tree.
		balanceHierarchy(tree, grandparent);

	// If the node's parent is the tree's root,
	// the sibling should become the new root.
	}else{
		tree->root = sibling;
		sibling->parent = NULL;
	}
}