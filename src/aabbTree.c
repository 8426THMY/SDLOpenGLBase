#include "aabbTree.h"


#include "utilMath.h"


#define AABBTREE_QUERY_STACK_SIZE 256

#define AABBNODE_HEIGHT_LEAF        0
#define AABBNODE_HEIGHT_LAST_BRANCH 1

#define AABBNODE_IS_LEAF(node) ((node)->height == AABBNODE_HEIGHT_LEAF)
#define AABBNODE_IS_LAST_BRANCH(node) ((node)->height == AABBNODE_HEIGHT_LAST_BRANCH)


static aabbNode *balanceNode(aabbTree *tree, aabbNode *node);
static void balanceHierarchy(aabbTree *tree, aabbNode *node);
static void insertLeaf(aabbTree *tree, aabbNode *node, aabbNode *parent);
static void removeLeaf(aabbTree *tree, aabbNode *node);


void aabbTreeInit(aabbTree *tree){
	tree->root = NULL;
	tree->leaves = NULL;
}


// Add the user's data to the tree.
aabbNode *aabbTreeInsertNode(aabbTree *tree, colliderAABB *aabb, void *userData, aabbNode *(*allocate)()){
	aabbNode *node = (*allocate)();
	if(node == NULL){
		/** MALLOC FAILED **/
	}

	node->aabb = *aabb;
	node->data.leaf.userData = userData;
	node->data.leaf.next = tree->leaves;
	node->height = AABBNODE_HEIGHT_LEAF;
	tree->leaves = node;

	if(tree->root != NULL){
		aabbNode *parent = (*allocate)();
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
** We assume that the node's bounding box has already been updated.
*/
void aabbTreeUpdateNode(aabbTree *tree, aabbNode *node){
	// We only need to reinsert the node if
	// it is not the only node in the tree.
	if(node != tree->root){
		removeLeaf(tree, node);
		insertLeaf(tree, node, node->parent);
	}
}

// Remove the user's data from the tree.
void aabbTreeRemoveNode(aabbTree *tree, aabbNode *node, void (*deallocate)(aabbNode *node)){
	// If we are not deleting the root node, we should
	// replace the node's parent with its sibling.
	if(node != tree->root){
		removeLeaf(tree, node);
		(*deallocate)(node->parent);

	// If we're deleting the root node, it cannot
	// have any children since it's a leaf, so we
	// can just update the tree's root pointer.
	}else{
		tree->root = NULL;
	}

	(*deallocate)(node);
}


/*
** Traverse the tree in post-order and run the function "callback"
** on every node. This is primarily used for memory deallocation.
*/
void aabbTreeTraverse(aabbTree *tree, void (*callback)(aabbNode *node)){
	aabbNode *node = tree->root;

	if(node != NULL){
		// Make sure our tree has more than one node.
		if(!AABBNODE_IS_LEAF(node)){
			for(;;){
				aabbNode *parent;

				// Find the left-most branch node of the current subtree.
				while(!AABBNODE_IS_LAST_BRANCH(node)){
					node = node->data.children.left;
				}
				parent = node->parent;

				// Run the callback function on the
				// branch node and its two leaves.
				(*callback)(node->data.children.left);
				(*callback)(node->data.children.right);
				(*callback)(node);

				// Now that we've reached the end of the current branch,
				// we'll need to keep climbing the tree until we reach
				// a node whose right branch hasn't been explored.
				for(;;){
					// If we've reached the root node, we've
					// searched the entire tree and can exit.
					if(parent == NULL){
						return;
					}
					// If our node is the left child of its parent,
					// the right subtree has not yet been explored.
					if(node == parent->data.children.left){
						node = parent->data.children.right;
						break;
					}
					(*callback)(node);
					node = parent;
					parent = node->parent;
				}
			}

		// If the tree only has a single node,
		// we can run the callback on it and exit.
		}else{
			(*callback)(node);
		}
	}
}

/*
** Traverse the tree in order searching for leaves that collide
** with the input leaf. If there is a collision, run the function
** "callback" with the two potentially colliding leaf nodes.
*/
void aabbTreeQueryCollisions(aabbTree *tree, const aabbNode *node, void (*callback)(void *d1, void *d2)){
	aabbNode *curNode = tree->root;

	// Make sure the tree isn't empty.
	if(curNode != NULL){
		// Our loop won't work if the tree's root is its only leaf.
		if(!AABBNODE_IS_LEAF(curNode)){
			for(;;){
				aabbNode *parent;

				// Keep searching down the tree until we find either
				// a branch that our bounding box doesn't collide with
				// or a branch that parents two leaf nodes.
				while(colliderAABBCollidingAABB(&node->aabb, &curNode->aabb)){
					// If we've found a node who parents two leaves, we
					// can run the callback function with its children.
					if(AABBNODE_IS_LAST_BRANCH(curNode)){
						(*callback)(node->data.leaf.userData, curNode->data.children.left->data.leaf.userData);
						(*callback)(node->data.leaf.userData, curNode->data.children.right->data.leaf.userData);

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
			(*callback)(node->data.leaf.userData, curNode->data.leaf.userData);
		}
	}
}

/*
** Similar to the function above, but this implementation
** utilises a stack to prevent crossing any nodes twice.
*/
void aabbTreeQueryCollisionsStack(aabbTree *tree, const aabbNode *node, void (*callback)(void *d1, void *d2)){
	aabbNode *stack[AABBTREE_QUERY_STACK_SIZE];
	size_t i = 1;

	stack[0] = tree->root;

	do {
		aabbNode *curNode = stack[--i];

		if(AABBNODE_IS_LEAF(curNode)){
			(*callback)(node->data.leaf.userData, curNode->data.leaf.userData);
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
aabbNode *aabbTreeFindNextNode(aabbTree *tree, const colliderAABB *aabb, const aabbNode *prevNode){
	aabbNode *curNode = tree->root;

	if(curNode != NULL){
		// If the tree has more than one node, we'll
		// need to find the next node after "prevNode".
		if(!AABBNODE_IS_LEAF(curNode)){
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
					if(AABBNODE_IS_LEAF(curNode)){
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
static aabbNode *balanceNode(aabbTree *tree, aabbNode *node){
	// We only need to perform a rotation of this node
	// is not a leaf and it does not parent any leaves.
	if(node->height > 1){
		aabbNode *parent = node->parent;
		aabbNode *left   = node->data.children.left;
		aabbNode *right  = node->data.children.right;
		aabbNode *grandLeft;
		aabbNode *grandRight;

		const int balance = right->height - left->height;
		// If the node's right branch is significantly deeper
		// than its left branch, we must rotate the node left.
		if(balance > 1){
			grandLeft = right->data.children.left;
			grandRight = right->data.children.right;

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

				node->height  = maxNumFast(left->height, grandRight->height) + 1;
				right->height = maxNumFast(node->height, grandLeft->height) + 1;
			}else{
				right->data.children.right = grandRight;
				node->data.children.right  = grandLeft;
				grandLeft->parent = node;

				colliderAABBCombine(&left->aabb, &grandLeft->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandRight->aabb, &right->aabb);

				node->height  = maxNumFast(left->height, grandLeft->height) + 1;
				right->height = maxNumFast(node->height, grandRight->height) + 1;
			}


			return(right);

		// Otherwise, if the node's left branch is significantly
		// deeper than its right branch, we must rotate it right.
		}else if(balance < -1){
			grandLeft = left->data.children.left;
			grandRight = left->data.children.right;

			// Replace the node with its left child.
			if(parent != NULL){
				if(parent->data.children.left == node){
					parent->data.children.left = right;
				}else{
					parent->data.children.right = right;
				}
			}else{
				tree->root = right;
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

				colliderAABBCombine(&left->aabb, &grandRight->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandLeft->aabb, &left->aabb);

				node->height = maxNumFast(left->height, grandRight->height) + 1;
				left->height = maxNumFast(node->height, grandLeft->height) + 1;
			}else{
				left->data.children.left = grandRight;
				node->data.children.left = grandLeft;
				grandLeft->parent = node;

				colliderAABBCombine(&left->aabb, &grandLeft->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandRight->aabb, &left->aabb);

				node->height = maxNumFast(left->height, grandLeft->height) + 1;
				left->height = maxNumFast(node->height, grandRight->height) + 1;
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
static void balanceHierarchy(aabbTree *tree, aabbNode *node){
	do {
		aabbNode *left;
		aabbNode *right;

		node  = balanceNode(tree, node);
		left  = node->data.children.left;
		right = node->data.children.right;

		// Fix up the node's properties to represent its new children.
		colliderAABBCombine(&left->aabb, &right->aabb, &node->aabb);
		node->height = maxNumFast(left->height, right->height) + 1;

		// Continue to the node's parent.
		node = node->parent;
	} while(node != NULL);
}


/*
** Insert a leaf node into the tree. We assume that both
** "node" and "parent" have been allocated outside this function.
** We also assume that the tree is not completely empty.
*/
static void insertLeaf(aabbTree *tree, aabbNode *node, aabbNode *parent){
	aabbNode *sibling = tree->root;
	aabbNode *siblingParent;

	// Find a leaf node to serve as our new node's sibling.
	while(!AABBNODE_IS_LEAF(sibling)){
		/*aabbNode *left = sibling->data.children.left;
		aabbNode *right = sibling->data.children.right;
		const float leftCost = colliderAABBCombinedVolume(&node->aabb, &left->aabb) - colliderAABBVolume(&left->aabb);
		const float rightCost = colliderAABBCombinedVolume(&node->aabb, &right->aabb) - colliderAABBVolume(&right->aabb);

		// Continue on to the child whose bounding box gives
		// the least increase in volume when combined with ours.
		if(leftCost <= rightCost){
			sibling = left;
		}else{
			sibling = right;
		}*/

		aabbNode *left = sibling->data.children.left;
		aabbNode *right = sibling->data.children.right;

		// Calculate the cost of starting a new branch.
		const float combinedArea  = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &sibling->aabb);
		const float branchCost    = 2.f * combinedArea;
		const float inheritedCost = 2.f * (combinedArea - colliderAABBSurfaceAreaHalf(&sibling->aabb));

		float leftCost;
		float rightCost;

		// Calculate the total increase in surface area that we will get
		// after combining our node with the current one's left child.
		leftCost = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &left->aabb);
		// If the child is a leaf node, the cost must
		// include the creation of a new branch node.
		if(AABBNODE_IS_LEAF(left)){
			leftCost += -colliderAABBSurfaceAreaHalf(&left->aabb) + inheritedCost;
		}

		// Calculate the total increase in surface area that we will get
		// after combining our node with the current one's right child.
		rightCost = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &right->aabb);
		// If the child is a leaf node, the cost must
		// include the creation of a new branch node.
		if(AABBNODE_IS_LEAF(right)){
			rightCost += -colliderAABBSurfaceAreaHalf(&right->aabb) + inheritedCost;
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
	parent->data.children.left = node;
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
static void removeLeaf(aabbTree *tree, aabbNode *node){
	aabbNode *parent = node->parent;
	aabbNode *grandparent = parent->parent;
	aabbNode *sibling = (parent->data.children.left == node) ? parent->data.children.right : parent->data.children.left;

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