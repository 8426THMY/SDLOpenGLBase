#include "aabbTree.h"


#define AABB_EXTRA_FAT 0.2f

//We can't check the left child as it shares its memory
//with the user data, but we can check the right child.
//Alternatively, just check the height! That'll do it.
#define AABBNODE_IS_LEAF(node) ((node)->height == 0)


#include "utilMath.h"


static aabbNode *balanceNode(aabbTree *tree, aabbNode *node);
static void balanceHierarchy(aabbTree *tree, aabbNode *node);
static void insertLeaf(aabbTree *tree, aabbNode *node);
static void removeLeaf(aabbTree *tree, aabbNode *node);


//Add a body to the AABB tree.
void aabbTreeInsert(aabbTree *tree, colliderAABB *aabb, void *body){
	/** This needs to be allocated... somewhere. **/
	aabbNode *node;
	/** Use the body's velocity to fatten this further. **/
	colliderAABBFatten(aabb, AABB_EXTRA_FAT, &node->aabb);
	node->data.body = body;
	node->data.children.right = NULL;
	node->height = 0;

	insertLeaf(tree, node);
}

//Remove a body from the AABB tree.
void aabbTreeRemove(aabbTree *tree, aabbNode *node){
	removeLeaf(tree, NULL);
	/** We need to deallocate "node" here. **/
}


/*
** Perform a left or right rotate and fix up any invalid
** bounding boxes to restore balance to a single node.
*/
static aabbNode *balanceNode(aabbTree *tree, aabbNode *node){
	//We only need to perform a rotation of this node
	//is not a leaf and it does not parent any leaves.
	if(node->height > 1){
		aabbNode *parent = node->parent;
		aabbNode *left   = node->data.children.left;
		aabbNode *right  = node->data.children.right;
		aabbNode *grandLeft;
		aabbNode *grandRight;

		const int balance = right->height - left->height;
		//If the node's right branch is significantly deeper
		//than its left branch, we must rotate the node left.
		if(balance > 1){
			grandLeft = right->data.children.left;
			grandRight = right->data.children.right;

			//Replace the node with its right child.
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

			//Now that the node is right's left child, we should make
			//right's right child the grandchild whose branch ran deeper.
			if(grandLeft->height >= grandRight->height){
				right->data.children.right = grandLeft;
				node->data.children.right  = grandRight;
				grandRight->parent = node;

				colliderAABBCombine(&left->aabb, &grandRight->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandLeft->aabb, &right->aabb);

				node->height  = maxNum(left->height, grandRight->height) + 1;
				right->height = maxNum(node->height, grandLeft->height) + 1;
			}else{
				right->data.children.right = grandRight;
				node->data.children.right  = grandLeft;
				grandLeft->parent = node;

				colliderAABBCombine(&left->aabb, &grandLeft->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandRight->aabb, &right->aabb);

				node->height  = maxNum(left->height, grandLeft->height) + 1;
				right->height = maxNum(node->height, grandRight->height) + 1;
			}


			return(right);

		//Otherwise, if the node's left branch is significantly
		//deeper than its right branch, we must rotate it right.
		}else if(balance < -1){
			grandLeft = left->data.children.left;
			grandRight = left->data.children.right;

			//Replace the node with its left child.
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

			//Now that the node is left's right child, we should make
			//left's left child the grandchild whose branch ran deeper.
			if(grandLeft->height >= grandRight->height){
				left->data.children.left = grandLeft;
				node->data.children.left = grandRight;
				grandRight->parent = node;

				colliderAABBCombine(&left->aabb, &grandRight->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandLeft->aabb, &left->aabb);

				node->height = maxNum(left->height, grandRight->height) + 1;
				left->height = maxNum(node->height, grandLeft->height) + 1;
			}else{
				left->data.children.left = grandRight;
				node->data.children.left = grandLeft;
				grandLeft->parent = node;

				colliderAABBCombine(&left->aabb, &grandLeft->aabb, &node->aabb);
				colliderAABBCombine(&node->aabb, &grandRight->aabb, &left->aabb);

				node->height = maxNum(left->height, grandLeft->height) + 1;
				left->height = maxNum(node->height, grandRight->height) + 1;
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

		//Fix up the node's properties to represent its new children.
		colliderAABBCombine(&left->aabb, &right->aabb, &node->aabb);
		node->height = maxNum(left->height, right->height) + 1;

		//Continue to the node's parent.
		node = node->parent;
	} while(node != NULL);
}


//Insert a leaf node into the AABB tree.
static void insertLeaf(aabbTree *tree, aabbNode *node){
	if(tree->root != NULL){
		aabbNode *sibling = tree->root;
		//Find a leaf node to serve as our new node's sibling.
		while(!AABBNODE_IS_LEAF(sibling)){
			/*aabbNode *left = sibling->data.children.left;
			aabbNode *right = sibling->data.children.right;
			const float leftCost = colliderAABBCombinedVolume(&node->aabb, &left->aabb) - colliderAABBVolume(&left->aabb);
			const float rightCost = colliderAABBCombinedVolume(&node->aabb, &right->aabb) - colliderAABBVolume(&right->aabb);

			//Continue on to the child whose bounding box gives
			//the least increase in volume when combined with ours.
			if(leftCost <= rightCost){
				sibling = left;
			}else{
				sibling = right;
			}*/

			aabbNode *left = sibling->data.children.left;
			aabbNode *right = sibling->data.children.right;

			//Calculate the cost of starting a new branch.
			const float combinedArea  = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &sibling->aabb);
			const float branchCost    = 2.f * combinedArea;
			const float inheritedCost = 2.f * (combinedArea - colliderAABBSurfaceAreaHalf(&sibling->aabb));

			float leftCost;
			float rightCost;

			//Calculate the total increase in surface area that we will get
			//after combining our node with the current one's left child.
			leftCost = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &left->aabb);
			//If the child is a leaf node, the cost must
			//include the creation of a new branch node.
			if(AABBNODE_IS_LEAF(left)){
				leftCost += -colliderAABBSurfaceAreaHalf(&left->aabb) + inheritedCost;
			}

			//Calculate the total increase in surface area that we will get
			//after combining our node with the current one's right child.
			rightCost = colliderAABBCombinedSurfaceAreaHalf(&node->aabb, &right->aabb);
			//If the child is a leaf node, the cost must
			//include the creation of a new branch node.
			if(AABBNODE_IS_LEAF(right)){
				rightCost += -colliderAABBSurfaceAreaHalf(&right->aabb) + inheritedCost;
			}

			//If it's cheapest to create a new
			//branch, we can exit the loop here.
			if(branchCost < leftCost && branchCost < rightCost){
				break;
			}
			//Otherwise, continue on
			//to the cheaper child.
			if(leftCost <= rightCost){
				sibling = left;
			}else{
				sibling = right;
			}
		}


		//Create a branch node to parent
		//our new node and its sibling.
		/** This needs to be allocated... somewhere. **/
		aabbNode *parent;
		aabbNode *siblingParent = sibling->parent;

		colliderAABBCombine(&node->aabb, &sibling->aabb, &parent->aabb);
		parent->parent = siblingParent;
		parent->data.children.left = sibling;
		parent->data.children.left = node;
		parent->height = sibling->height + 1;

		node->parent = parent;
		sibling->parent = parent;

		//If the node's sibling was not the tree's root, we'll
		//need to make its parent point to the new parent node.
		if(siblingParent != NULL){
			if(siblingParent->data.children.left == sibling){
			   siblingParent->data.children.left = parent;
			}else{
				siblingParent->data.children.right = parent;
			}

		//If the sibling node was the tree's root,
		//we'll need to update the root pointer.
		}else{
			tree->root = parent;
		}


		//Ensure that the tree is correctly balanced.
		balanceHierarchy(tree, parent);


	//If the tree has no nodes,
	//add this one as the root node.
	}else{
		tree->root = node;
		node->parent = NULL;
	}
}

//Remove a leaf node from the AABB tree.
static void removeLeaf(aabbTree *tree, aabbNode *node){
	//If we are not deleting the root node, we should
	//replace the node's parent with its sibling.
	if(node != tree->root){
		aabbNode *parent = node->parent;
		aabbNode *grandparent = parent->parent;
		aabbNode *sibling = (parent->data.children.left == node) ? parent->data.children.right : parent->data.children.left;

		//If the node is not a child of the root node,
		//we should replace its parent with its sibling.
		if(grandparent != NULL){
			if(grandparent->data.children.left == parent){
				grandparent->data.children.left = sibling;
			}else{
				grandparent->data.children.right = sibling;
			}
			sibling->parent = grandparent;

			//Ensure that the tree is correctly balanced.
			//If the node did not have a grandparent,
			//there is no need to rebalance the tree.
			balanceHierarchy(tree, grandparent);

		//If the node's parent is the tree's root,
		//the sibling should become the new root.
		}else{
			tree->root = sibling;
			sibling->parent = NULL;
		}

		/** We need to deallocate "parent" here. **/

	//If we're deleting the root node, it cannot
	//have any children since it's a leaf, so we
	//can just update the tree's root pointer.
	}else{
		tree->root = NULL;
	}
}