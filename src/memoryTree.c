#include "memoryTree.h"


#ifdef MEMTREE_DEBUG
#include <stdio.h>
#endif

#include "utilTypes.h"


#define MEMTREE_FLAG_INACTIVE 0x00
#define MEMTREE_FLAG_ACTIVE   0x01
#define MEMTREE_FLAG_FIRST    0x02
#define MEMTREE_FLAG_LAST     0x04
#define MEMTREE_FLAG_ONLY     0x06

#define MEMTREE_FLAG_ACTIVE_MASK (~MEMTREE_FLAG_ACTIVE)
#define MEMTREE_FLAG_FIRST_MASK  (~MEMTREE_FLAG_FIRST)
#define MEMTREE_FLAG_LAST_MASK   (~MEMTREE_FLAG_LAST)
#define MEMTREE_FLAG_ONLY_MASK   (~MEMTREE_FLAG_ONLY)

//Get the next block's list data from the current one's.
#define listNodeGetNextList(node, size) ((memTreeListNode *)memoryAddPointer(node, size))
//Get the previous block's list data from the current one's.
#define listNodeGetPrevList(node, size) ((memTreeListNode *)memorySubPointer(node, size))
//Return the tree component of an inactive block from its list component.
#define listNodeGetTree(node) ((memTreeNode *)memoryAddPointer(node, MEMTREE_BLOCK_HEADER_SIZE))

//We'll need to remove the active flag from
//the size if we want to get its real value.
#define listNodeGetSize(info) (((uintptr_t)(info)) & MEMORY_DATA_MASK)
//The active flag is stored in the
//least significant bit of the size.
#define listNodeGetFlags(info) (((uintptr_t)(info)) & MEMORY_FLAG_MASK)
//Write the flag specified by "flag" to the integer "size".
#define listNodeSetInfo(size, flag) ((size) | (flag))

//Remove the flag specified by "flag" from the block's information.
#define listNodeRemoveFlag(info, flag) (((uintptr_t)(info)) & (~(flag)))
//Remove the active flag from a block's information.
#define listNodeRemoveActive(info) listNodeRemoveFlag(info, MEMTREE_FLAG_ACTIVE)
//Remove the "first" flag from a block's information.
#define listNodeRemoveFirst(info) listNodeRemoveFlag(info, MEMTREE_FLAG_FIRST)
//Remove the "last" flag from a block's information.
#define listNodeRemoveLast(info) listNodeRemoveFlag(info, MEMTREE_FLAG_LAST)

//Get the flag specified by "flag" from the block's information.
#define listNodeGetFlag(info, flag) (((uintptr_t)(info)) & (flag))
//Get whether or not this block is active.
#define listNodeIsActive(info) listNodeGetFlag(info, MEMTREE_FLAG_ACTIVE)
//Get whether or not this block is the first.
#define listNodeIsFirst(info) listNodeGetFlag(info, MEMTREE_FLAG_FIRST)
//Get whether or not this block is the last.
#define listNodeIsLast(info) listNodeGetFlag(info, MEMTREE_FLAG_LAST)

//Set the size of a block without changing its flags.
#define listNodeSetSize(info, size) ((size) | listNodeGetFlags(info))
//Write the flag specified by "flag" to the block's information.
#define listNodeSetFlag(info, flag) ((info) | (flag))
//Set the value of a node's active flag.
#define listNodeMakeActive(info) listNodeSetFlag(info, MEMTREE_FLAG_ACTIVE)
//Set the value of a node's "first" flag.
#define listNodeMakeFirst(info) listNodeSetFlag(info, MEMTREE_FLAG_FIRST)
//Set the value of a node's "last" flag.
#define listNodeMakeLast(info) listNodeSetFlag(info, MEMTREE_FLAG_LAST)


//By default, nodes are black, as the
//flag bits will always begin unset.
//This means that when we set a node's
//children to NULL, they will be black.
#define TREE_NODE_COLOUR_BLACK 0x00
#define TREE_NODE_COLOUR_RED   0x01

//Get the next block's list data from the current one's tree data.
#define treeNodeGetNextList(node, size) ((memTreeListNode *)memoryAddPointer(node, (size) + MEMTREE_BLOCK_HEADER_SIZE))
//Get the previous block's list data from the current one's tree data.
#define treeNodeGetPrevList(node, size) ((memTreeListNode *)memorySubPointer(node, MEMTREE_BLOCK_HEADER_SIZE + (size)))
//Get the next block's tree data from the current one's tree data.
#define treeNodeGetNextTree(node, size) ((memTreeNode *)memoryAddPointer(node, size))
//Get the previous block's tree data from the current one's tree data.
#define treeNodeGetPrevTree(node, size) ((memTreeNode *)memorySubPointer(node, size))
//Return the list component of an inactive block from its tree component.
#define treeNodeGetList(node) ((memTreeListNode *)memorySubPointer(node, MEMTREE_BLOCK_HEADER_SIZE))

//Return the size of an inactive block from its tree component.
#define treeNodeGetSize(node) *((size_t *)memorySubPointer(node, sizeof(size_t)))
//Return the size of an inactive block from its tree component.
#define treeNodeGetPrevSize(node) *((size_t *)memorySubPointer(node, MEMTREE_BLOCK_HEADER_SIZE))
//Return the size of an inactive block from its tree component.
#define treeNodeGetFlags(node) listNodeGetFlags(treeNodeGetPrevSize(node))

//We'll need to remove the colour bit from the
//parent pointer if we want to get its real value.
#define treeNodeGetParent(node) ((memTreeNode *)(((uintptr_t)(((memTreeNode *)(node))->parent)) & MEMORY_DATA_MASK))
//Update the node's parent pointer without changing its colour.
#define treeNodeSetParent(node, nodeParent) ((memTreeNode *)(((uintptr_t)(nodeParent)) | treeNodeGetColour(node)))
//The colour of a node is stored in the least
//significant bit of the parent pointer.
#define treeNodeGetColour(node) (((uintptr_t)(((memTreeNode *)(node))->parent)) & MEMORY_FLAG_MASK)
//Write the specified colour flag
//to the specified parent pointer.
#define treeNodeSetColour(nodeParent, colour) ((memTreeNode *)(((uintptr_t)(nodeParent)) | (colour)))
//Set the node's parent pointer and its colour.
#define treeNodeCreateParentPointer(nodeParent, colour) ((memTreeNode *)(((uintptr_t)(nodeParent)) | (colour)))
//Get a node's parent from its parent pointer.
#define treeNodeInfoGetParent(info) ((memTreeNode *)(((uintptr_t)(info)) & MEMORY_DATA_MASK))
//Update the node's parent pointer without changing its colour.
#define treeNodeInfoSetParent(info, nodeParent) ((memTreeNode *)(((uintptr_t)(nodeParent)) | treeNodeInfoGetParent(info)))
//Get a node's colour from its parent pointer.
#define treeNodeInfoGetColour(info) (((uintptr_t)(info)) & MEMORY_FLAG_MASK)

//Return whether or not the node is black.
#define treeNodeIsBlack(node, nodeParent) (node == NULL || treeNodeIsBlackFast(node, nodeParent))
//Return whether or not the node is red.
#define treeNodeIsRed(node, nodeParent) (node != NULL && treeNodeIsRedFast(node, nodeParent))
//Since the black flag is defined as zero, we can check if
//its parent pointer is the same as its parent's actual address.
#define treeNodeIsBlackFast(node, nodeParent) ((((memTreeNode *)(node))->parent) == ((memTreeNode *)(nodeParent)))
//Same as above, but checks if the two are not equal.
#define treeNodeIsRedFast(node, nodeParent) ((((memTreeNode *)(node))->parent) != ((memTreeNode *)(nodeParent)))
//If we want to make a node black and we have the address
//of its parent, we can just set its parent pointer to that.
#define treeNodeMakeBlack(nodeParent) ((memTreeNode *)(nodeParent))
//We can't really do the same the other way around, so do it the slow way.
#define treeNodeMakeRed(nodeParent) (treeNodeSetColour(nodeParent, TREE_NODE_COLOUR_RED))


#warning "What if the beginning or end of a memory region is not aligned?"


static void splitBlock(memoryTree *tree, memTreeListNode *block, const size_t blockSize, const size_t newSize);

static void treeInsert(memoryTree *tree, memTreeNode *node, const size_t nodeSize);
static void treeDelete(memoryTree *tree, memTreeNode *node);
static void treeRotateLeft(memoryTree *tree, memTreeNode *node);
static void treeRotateRight(memoryTree *tree, memTreeNode *node);


void *memTreeInit(memoryTree *tree, void *memory, const size_t memorySize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		const size_t regionSize = memoryGetRegionSize(memorySize);

		//Create a free block to occupy all of the allocated memory.
		tree->root = memTreeClearRegion(memory, regionSize);
		memset(tree->root, (uintptr_t)NULL, sizeof(*tree->root));

		//Set up the tree's memory region footer.
		tree->region = memoryGetRegionFromSize(memory, regionSize);
		tree->region->start = memory;
		tree->region->next = NULL;
	}

	return(memory);
}


void *memTreeAlloc(memoryTree *tree, const size_t blockSize){
	//Make sure the new block's size is neither too small nor misaligned.
	const size_t fullSize = memTreeGetBlockSize(blockSize) + MEMTREE_BLOCK_HEADER_SIZE;

	memTreeNode *newNode = NULL;
	size_t newBlockSize;

	memTreeNode *currentNode = tree->root;
	//Search our free tree for the best fitting free block.
	for(;;){
		const size_t currentBlockSize = treeNodeGetSize(currentNode);

		memTreeNode *nextNode;
		//If the new size is smaller than the current node's, go left.
		if(fullSize < currentBlockSize){
			//This node is the smallest node we've found that
			//is large enough to store our data, so remember it.
			newNode = currentNode;
			newBlockSize = currentBlockSize;

			nextNode = currentNode->left;

		//If the new size is larger than the current node's, go right.
		}else if(fullSize > currentBlockSize){
			nextNode = currentNode->right;

		//Otherwise, we've found the ideal block to occupy!
		}else{
			newNode = currentNode;
			newBlockSize = currentBlockSize;

			break;
		}

		//If the current node has no children, exit
		//with whichever node is our current best.
		if(nextNode == NULL){
			break;
		}
		//Otherwise, check the next node!
		currentNode = nextNode;
	}

	//Make sure we could actually find a block large enough to store the data.
	if(newNode != NULL){
		memTreeListNode *currentBlock = treeNodeGetList(newNode);

		//Remove this block from the tree.
		treeDelete(tree, newNode);
		//We'll have to set the block to active, too.
		currentBlock->prevSize = listNodeMakeActive(currentBlock->prevSize);

		//Provided the leftover memory is large enough,
		//create a new empty block after the current one.
		splitBlock(tree, currentBlock, newBlockSize, fullSize);
	}

	//Provide the user a means of accessing their memory block!
	//Because "newNode" represents the beginning of the tree data,
	//which is now where our user data starts, we can just return it.
	//If we couldn't find a block large enough, this will return NULL.
	return(newNode);
}

void *memTreeRealloc(memoryTree *tree, void *block, const size_t blockSize){
	if(block != NULL){
		memTreeListNode *newBlock = treeNodeGetList(block);
		size_t newSize;
		memTreeNode *tempBlock;

		const size_t fullSize = blockSize + MEMTREE_BLOCK_HEADER_SIZE;
		const size_t oldSize = newBlock->size;
		//If the block is already big enough, we can exit early.
		if(fullSize <= oldSize){
			return(block);
		}

		//Otherwise, we will need to check the blocks to
		//the left and right in case we are able to coalesce.
		newSize = oldSize;

		//If this block is not the first, we might be able to merge left.
		if(!listNodeIsFirst(newBlock->prevSize)){
			memTreeListNode *leftBlock = listNodeGetPrevList(newBlock, listNodeGetSize(newBlock->prevSize));
			//We can only merge it if the block to its left is free.
			if(!listNodeIsActive(leftBlock->prevSize)){
				//Make sure we preserve the free block's "last" flag.
				leftBlock->prevSize |= listNodeIsLast(newBlock->prevSize);
				//We're merging the two blocks, so add their sizes.
				newSize += leftBlock->size;

				//The free block no longer exists as it has been merged,
				//so our pointer should now point to the left block.
				newBlock = leftBlock;

				//Remove the left block from the free tree,
				//as it's outdated. We'll add it back later.
				treeDelete(tree, listNodeGetTree(leftBlock));
			}
		}
		//If this block is not the last, we might be able to merge right.
		if(!listNodeIsLast(newBlock->prevSize)){
			memTreeListNode *rightBlock = listNodeGetNextList(newBlock, newSize);
			//We can only merge it if the block to its right is free.
			if(!listNodeIsActive(rightBlock->prevSize)){
				//We're merging the two blocks, so add their sizes.
				newSize += rightBlock->size;

				//Remove the right block from the free
				//tree, as it no longer exists. We'll
				//add the free block to the tree later.
				treeDelete(tree, listNodeGetTree(rightBlock));

				//If there is a block after this one,
				//update its previous size property.
				if(!listNodeIsLast(rightBlock->prevSize)){
					memTreeListNode *nextBlock = listNodeGetNextList(newBlock, newSize);
					nextBlock->prevSize = listNodeSetSize(nextBlock->prevSize, newSize);

				//Otherwise, make sure we preserve
				//the right block's "last" flag.
				}else{
					newBlock->prevSize = listNodeMakeLast(newBlock->prevSize);
				}

			//Otherwise, update the block's previous size property.
			}else{
				rightBlock->prevSize = listNodeSetSize(rightBlock->prevSize, newSize);
			}
		}

		newBlock->size = newSize;

		//If our new block is big enough, we can use it!
		if(newSize >= fullSize){
			//Make sure we set its active flag.
			newBlock->prevSize = listNodeMakeActive(newBlock->prevSize);

			//Provided the leftover memory is large enough,
			//create a new empty block after the current one.
			splitBlock(tree, newBlock, newSize, (size_t)memoryAlign(fullSize));

			tempBlock = listNodeGetTree(newBlock);
			//We only need to copy the user's data
			//if the block's pointer has moved.
			if(tempBlock != block){
				memmove(tempBlock, block, oldSize);
			}

			return(tempBlock);
		}

		//If the block is still too small, we've done all the merging we
		//normally do when freeing a block, so we can allocate a new one.
		newBlock->prevSize = listNodeRemoveActive(newBlock->prevSize);

		//We need to allocate a new block first so we don't overwrite
		//the user's data with any of the free block header information.
		tempBlock = memTreeAlloc(tree, blockSize);
		memcpy(tempBlock, block, oldSize);

		//Now we can add the old block back to the tree.
		treeInsert(tree, listNodeGetTree(newBlock), newSize);


		return(tempBlock);
	}

	//If we're performing a realloc on a NULL
	//pointer, just allocate a new block.
	return(memTreeAlloc(tree, blockSize));
}


void memTreeFree(memoryTree *tree, void *block){
	memTreeListNode *newBlock = treeNodeGetList(block);
	size_t newSize = newBlock->size;

	//If this block is not the first, we might be able to merge left.
	if(!listNodeIsFirst(newBlock->prevSize)){
		memTreeListNode *leftBlock = listNodeGetPrevList(newBlock, listNodeGetSize(newBlock->prevSize));
		//We can only merge it if the block to its left is free.
		if(!listNodeIsActive(leftBlock->prevSize)){
			//Make sure we preserve the free block's "last" flag.
			leftBlock->prevSize |= listNodeIsLast(newBlock->prevSize);
			//We're merging the two blocks, so add their sizes.
			newSize += leftBlock->size;

			//The free block no longer exists as it has been merged,
			//so our pointers should now point to the left block.
			block = listNodeGetTree(leftBlock);
			newBlock = leftBlock;

			//Remove the left block from the free tree,
			//as it's outdated. We'll add it back later.
			treeDelete(tree, block);
		}
	}
	//If this block is not the last, we might be able to merge right.
	if(!listNodeIsLast(newBlock->prevSize)){
		memTreeListNode *rightBlock = listNodeGetNextList(newBlock, newSize);
		//We can only merge it if the block to its right is free.
		if(!listNodeIsActive(rightBlock->prevSize)){
			//We're merging the two blocks, so add their sizes.
			newSize += rightBlock->size;

			//Remove the right block from the free
			//tree, as it no longer exists. We'll
			//add the free block to the tree later.
			treeDelete(tree, listNodeGetTree(rightBlock));

			//If there is a block after this one,
			//update its previous size property.
			if(!listNodeIsLast(rightBlock->prevSize)){
				memTreeListNode *nextBlock = listNodeGetNextList(newBlock, newSize);
				nextBlock->prevSize = listNodeSetSize(nextBlock->prevSize, newSize);

			//Otherwise, make sure we preserve
			//the right block's "last" flag.
			}else{
				newBlock->prevSize = listNodeMakeLast(newBlock->prevSize);
			}

		//Otherwise, update the block's previous size property.
		}else{
			rightBlock->prevSize = listNodeSetSize(rightBlock->prevSize, newSize);
		}
	}


	//Make sure we make the free block
	//inactive and update its size.
	newBlock->prevSize = listNodeRemoveActive(newBlock->prevSize);
	newBlock->size = newSize;
	//Now that we've finished any coalescence,
	//we can add our block to the free tree!
	treeInsert(tree, block, newSize);
}


//Create a tree node that takes up all of the memory in the region.
memTreeNode *memTreeClearRegion(void *memory, const size_t memorySize){
	//Set up the starting block's list components.
	((memTreeListNode *)memory)->prevSize = listNodeSetInfo(0, MEMTREE_FLAG_ONLY);
	((memTreeListNode *)memory)->size = memorySize;

	//Set up the starting block's tree components.
	memTreeNode *root = listNodeGetTree((memTreeListNode *)memory);


	return(root);
}

//Clear every memory region in the allocator.
//This assumes that there is at least one region.
void memTreeClear(memoryTree *tree, void *memory, const size_t memorySize){
	memoryRegion *region = tree->region;
	//Clear the first region here so we
	//can set the first node in the tree.
	tree->root = memTreeClearRegion(memory, (size_t)((byte_t *)region->start - (byte_t *)region));
	memset(tree->root, (uintptr_t)NULL, sizeof(*tree->root));

	region = region->next;
	//Clear every region in the allocator.
	while(region != NULL){
		const size_t regionSize = (size_t)((byte_t *)region->start - (byte_t *)region);
		//Create a free block to occupy all of the region's memory.
		treeInsert(tree, memTreeClearRegion(region->start, regionSize), regionSize);

		region = region->next;
	}
}


void *memTreeExtend(memoryTree *tree, void *memory, const size_t memorySize){
	if(memory != NULL){
		const size_t regionSize = memoryGetRegionSize(memorySize);
		memoryRegion *newRegion = memoryGetRegionFromSize(memory, regionSize);

		//Add the new region to the end of the list!
		memoryRegionAppend(&tree->region, newRegion, memory);
		//Create a free block to occupy all of the allocated memory.
		treeInsert(tree, memTreeClearRegion(memory, regionSize), regionSize);
	}

	return(memory);
}


void memTreeDelete(memoryTree *tree){
	memoryAllocatorDelete(tree->region);
}


#ifdef MEMTREE_DEBUG
void memTreePrintAllSizes(memoryTree *tree){
	puts("MEMTREE_DEBUG: All Blocks\n"
	     "~~~~~~~~~~~~~~~~~~~~~~~~~");

	memoryRegion *region = tree->region;
	size_t regionNum = 0;
	//Loop through all of the memory regions that this tree uses.
	do {
		//Print some details about the region.
		printf(
			"Region Number: %u, Address: %u, Start: %u, Next: %u\n"
			"-------------------------\n",
			regionNum, (uintptr_t)region, (uintptr_t)(region->start), (uintptr_t)(region->next)
		);

		memTreeListNode *node = (memTreeListNode *)(region->start);
		//Loop through all of the nodes in this region.
		for(;;){
			//If the node is active, we just print its size and flags.
			if(listNodeIsActive(node->prevSize)){
				printf(
					"Used Address: %u, Size: %u, Flags: %u\n\n",
					(uintptr_t)node, node->size, listNodeGetFlags(node->prevSize)
				);

			//Otherwise, we can print its tree data too!
			}else{
				memTreeNode *nodeTree = listNodeGetTree(node);
				printf(
					"Free Address: %u, Size: %u, Flags: %u,\n"
					"Left: %u, Right: %u, Parent: %u, Colour: %u\n\n",
					(uintptr_t)node, node->size - MEMTREE_BLOCK_HEADER_SIZE, listNodeGetFlags(node->prevSize),
					(uintptr_t)(nodeTree->left), (uintptr_t)(nodeTree->right), (uintptr_t)(treeNodeGetParent(nodeTree)), treeNodeGetColour(nodeTree)
				);
			}

			//If this node is the last, we've finished!
			if(listNodeIsLast(node->prevSize)){
				break;
			}
			//Otherwise, move to the next node!
			node = listNodeGetNextList(node, node->size);
		}

		//Now that we're done, move to the next region!
		region = region->next;
		++regionNum;
	} while(region != NULL);
}

void memTreePrintFreeSizes(memoryTree *tree){
	puts("MEMTREE_DEBUG: Free Blocks\n"
	     "~~~~~~~~~~~~~~~~~~~~~~~~~~");

	memTreeNode *node = tree->root;
	if(node != NULL){
		memTreeNode *nodeLeft;
		//Start by finding the tree's leftmost node.
		while((nodeLeft = node->left) != NULL){
			node = nodeLeft;
		}

		while(node != NULL){
			//Sizes include the block's header, so
			//we need to remove that when we print it.
			printf("Free Address: %u, Size: %u, Flags: %u,\n"
				   "Left: %u, Right: %u, Parent: %u, Colour: %u\n\n",
				   (uintptr_t)treeNodeGetList(node), treeNodeGetSize(node) - MEMTREE_BLOCK_HEADER_SIZE, treeNodeGetFlags(node),
				   (uintptr_t)(node->left), (uintptr_t)(node->right), (uintptr_t)(treeNodeGetParent(node)), treeNodeGetColour(node));

			//If there is a node to the right of
			//the one the iterator is currently
			//on, find that node's leftmost one.
			memTreeNode *nodeRight = node->right;
			if(nodeRight != NULL){
				node = nodeRight;

				while((nodeLeft = node->left) != NULL){
					node = nodeLeft;
				}

			//If there are no more nodes to the right, keep
			//travelling up the tree until the node that
			//the iterator is the left child of is found.
			}else{
				memTreeNode *nodeParent;
				while((nodeParent = treeNodeGetParent(node)) != NULL &&
					  node == nodeParent->right){

					node = nodeParent;
				}

				node = nodeParent;
			}
		}
	}
}
#endif


/*
** If "block", whose current size is given by "blockSize"
** but only needs a size of "newSize", is large enough to
** be split into a separate free block, split it up!
*/
static void splitBlock(memoryTree *tree, memTreeListNode *block, const size_t blockSize, const size_t newSize){
	size_t sizeDiff = blockSize - newSize;

	//Make sure the difference in the current size and
	//the ideal size is large enough for a new free block.
	if(sizeDiff >= MEMTREE_BLOCK_MIN_SIZE){
		memTreeListNode *freeBlock = listNodeGetNextList(block, newSize);

		//If the free block is the last, give
		//it the used block's "last" flag.
		if(listNodeIsLast(block->prevSize)){
			block->prevSize = listNodeRemoveLast(block->prevSize);
			freeBlock->prevSize = listNodeMakeLast(newSize);

		//If this block isn't the last, we will have to
		//set the previous size element of the next block.
		}else{
			memTreeListNode *nextBlock = listNodeGetNextList(freeBlock, sizeDiff);
			nextBlock->prevSize = listNodeSetSize(nextBlock->prevSize, sizeDiff);

			//We don't need to explicitly make the block inactive,
			//since we would just be performing an OR with '0'.
			freeBlock->prevSize = newSize;
		}

		//Set the size elements of the two blocks.
		block->size = newSize;
		freeBlock->size = sizeDiff;

		//Add the new free block to the tree!
		treeInsert(tree, listNodeGetTree(freeBlock), sizeDiff);
	}
}


static void treeInsert(memoryTree *tree, memTreeNode *node, const size_t nodeSize){
	memTreeNode *parent = tree->root;

	node->left = NULL;
	node->right = NULL;
	//Make sure the tree isn't empty before inserting the node.
	if(parent != NULL){
		//Search our tree to find where the new node should be inserted.
		for(;;){
			//If the new size is smaller than or equal to the
			//current node's, go left. Otherwise, go right.
			memTreeNode **nextNode = (nodeSize <= treeNodeGetSize(parent)) ? &parent->left : &parent->right;

			//If we've reached the end of the tree, add our new node!
			if(*nextNode == NULL){
				*nextNode = node;

				break;
			}
			//Otherwise, check the next node!
			parent = *nextNode;
		}

		//Update the new leaf node's properties!
		node->parent = treeNodeMakeRed(parent);


		//Now that our node has been inserted, we
		//must make sure our tree is still compliant.
		memTreeNode *grandparent = treeNodeGetParent(parent);
		//Keep recolouring while the current node's parent is red.
		while(treeNodeIsRed(parent, grandparent)){
			memTreeNode *uncle = grandparent->left;

			if(parent == uncle){
				uncle = grandparent->right;

				//If we have a red uncle, we can simply
				//do a colour swap and move up the tree.
				if(treeNodeIsRed(uncle, grandparent)){
					parent->parent = treeNodeMakeBlack(grandparent);
					uncle->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(grandparent->parent);

					node = grandparent;
				}else{
					//Left right case.
					if(node == parent->right){
						treeRotateLeft(tree, parent);
						parent = node;
						node = parent->left;
					}

					//Left left case.
					parent->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(grandparent->parent);
					treeRotateRight(tree, grandparent);
				}
			}else{
				//If we have a red uncle, we can simply
				//do a colour swap and move up the tree.
				if(treeNodeIsRed(uncle, grandparent)){
					parent->parent = treeNodeMakeBlack(grandparent);
					uncle->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(grandparent->parent);

					node = grandparent;
				}else{
					//Right left case.
					if(node == parent->left){
						treeRotateRight(tree, parent);
						parent = node;
						node = parent->right;
					}

					//Right right case.
					parent->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(grandparent->parent);
					treeRotateLeft(tree, grandparent);
				}
			}

			//We should end the loop if our
			//node is now the root node.
			if(node == tree->root){
				break;
			}
			//Otherwise, we'll need to update
			//the parent and grandparent nodes.
			parent = treeNodeGetParent(node);
			grandparent = treeNodeGetParent(parent);
		}

		//Make sure the root node's parent is NULL.
		tree->root->parent = NULL;

	//If the tree is empty, insert as the root node.
	}else{
		node->parent = NULL;
		tree->root = node;
	}
}

static void treeDelete(memoryTree *tree, memTreeNode *node){
	memTreeNode *nodeLeft = node->left;
	memTreeNode *nodeRight = node->right;

	memTreeNode *successor;
	memTreeNode *successorParent;
	memTreeNode *child;

	//The node to be deleted will be stored in "node".
	//If we're deleting a node with two children, we
	//swap it with its in-order successor and delete
	//the node using its new, single-child location.
	if(nodeLeft != NULL){
		if(nodeRight != NULL){
			successor = nodeRight;
			//Find the smallest node in the right subtree.
			while(successor->left != NULL){
				successor = successor->left;
			}
			child = successor->right;

		//The node has a left child but no right child.
		}else{
			successor = node;
			child = nodeLeft;
		}

	//The node either has a right child and no left
	//child or it has no children. In the latter case,
	//the child pointer will be NULL as it should be.
	}else{
		successor = node;
		child = nodeRight;
	}

	successorParent = treeNodeGetParent(successor);
	//Make the successor's parent point to the child node.
	if(successorParent != NULL){
		if(successorParent->left == successor){
			successorParent->left = child;
		}else{
			successorParent->right = child;
		}
	}else{
		tree->root = child;
	}
	//Set the child's parent to its successor's.
	if(child != NULL){
		child->parent = treeNodeSetParent(child, successorParent);
	}

	//If the successor and its child are both
	//black, we still have some work to do.
	if(treeNodeIsBlackFast(successor, successorParent)){
		while(child != tree->root && treeNodeIsBlack(child, successorParent)){
			memTreeNode *sibling = successorParent->left;
			memTreeNode *siblingLeft;
			memTreeNode *siblingRight;

			if(child == sibling){
				sibling = successorParent->right;

				//If the sibling is red, swap its colour with its parent and rotate left.
				if(treeNodeIsRedFast(sibling, successorParent)){
					sibling->parent = treeNodeMakeBlack(successorParent);
					successorParent->parent = treeNodeMakeRed(successorParent->parent);
					treeRotateLeft(tree, successorParent);
					sibling = successorParent->right;
				}

				//Keep pointers to our sibling's children.
				siblingLeft = sibling->left;
				siblingRight = sibling->right;

				//If both of the sibling's children are black,
				//make the sibling red and continue upwards.
				if(treeNodeIsBlack(siblingLeft, sibling) && treeNodeIsBlack(siblingRight, sibling)){
					sibling->parent = treeNodeMakeRed(successorParent);
					child = successorParent;
					successorParent = treeNodeGetParent(successorParent);
				}else{
					//Right right case.
					if(treeNodeIsBlack(siblingRight, sibling)){
						siblingLeft->parent = treeNodeMakeBlack(sibling);
						sibling->parent = treeNodeMakeRed(successorParent);
						treeRotateRight(tree, sibling);

						siblingRight = sibling;
						sibling = successorParent->right;
					}
					//Right left case.
					sibling->parent = treeNodeSetColour(successorParent, treeNodeGetColour(successorParent));
					successorParent->parent = treeNodeMakeBlack(treeNodeGetParent(successorParent));
					siblingRight->parent = treeNodeMakeBlack(sibling);
					treeRotateLeft(tree, successorParent);

					//Break the loop.
					child = tree->root;
					successorParent = NULL;
					break;
				}
			}else{
				//If the sibling is red, swap its colour with its parent and rotate right.
				if(treeNodeIsRedFast(sibling, successorParent)){
					sibling->parent = treeNodeMakeBlack(successorParent);
					successorParent->parent = treeNodeMakeRed(successorParent->parent);
					treeRotateRight(tree, successorParent);
					sibling = successorParent->left;
				}

				//Keep pointers to our sibling's children.
				siblingLeft = sibling->left;
				siblingRight = sibling->right;

				//If both of the sibling's children are black,
				//make the sibling red and continue upwards.
				if(treeNodeIsBlack(siblingRight, sibling) && treeNodeIsBlack(siblingLeft, sibling)){
					sibling->parent = treeNodeMakeRed(successorParent);
					child = successorParent;
					successorParent = treeNodeGetParent(successorParent);
				}else{
					//Left left case.
					if(treeNodeIsBlack(siblingLeft, sibling)){
						siblingRight->parent = treeNodeMakeBlack(sibling);
						sibling->parent = treeNodeMakeRed(successorParent);
						treeRotateLeft(tree, sibling);

						siblingLeft = sibling;
						sibling = successorParent->left;
					}
					//Left right case.
					sibling->parent = treeNodeSetColour(successorParent, treeNodeGetColour(successorParent));
					successorParent->parent = treeNodeMakeBlack(treeNodeGetParent(successorParent));
					siblingLeft->parent = treeNodeMakeBlack(sibling);
					treeRotateRight(tree, successorParent);

					//Break the loop.
					child = tree->root;
					successorParent = NULL;
					break;
				}
			}
		}

		//Make sure the child is black.
		if(child != NULL){
			child->parent = treeNodeMakeBlack(successorParent);
		}
	}

	//If the node is not its own successor,
	//we will need to swap the two around.
	if(node != successor){
		nodeLeft = node->left;
		nodeRight = node->right;
		successorParent = treeNodeGetParent(node);

		//Update the left child's parent pointer.
		if(nodeLeft != NULL){
			nodeLeft->parent = treeNodeSetParent(nodeLeft, successor);
		}
		//Set the successor's left child.
		successor->left = nodeLeft;

		//Update the right child's parent pointer.
		if(nodeRight != NULL){
			nodeRight->parent = treeNodeSetParent(nodeRight, successor);
		}
		//Set the successor's right child.
		successor->right = nodeRight;

		//Make the node's parent point to the successor.
		if(successorParent != NULL){
			if(successorParent->left == node){
				successorParent->left = successor;
			}else{
				successorParent->right = successor;
			}
		}else{
			tree->root = successor;
		}
		//Set the successor's parent.
		successor->parent = node->parent;
	}
}

static void treeRotateLeft(memoryTree *tree, memTreeNode *node){
	memTreeNode *right = node->right;
	memTreeNode *tempNode = right->left;

	node->right = tempNode;
	//The left child of the node's right child
	//should now become the node's right child.
	if(tempNode != NULL){
		tempNode->parent = treeNodeSetParent(tempNode, node);
	}

	tempNode = treeNodeGetParent(node);
	//The node's parent should point
	//to the node's old right child.
	if(tempNode != NULL){
		if(tempNode->left == node){
			tempNode->left = right;
		}else{
			tempNode->right = right;
		}
	}else{
		tree->root = right;
	}
	//Update the right child's parent.
	right->parent = treeNodeSetParent(right, tempNode);

	//The node's old right child
	//should point to the node.
	right->left = node;
	//Update the node's parent.
	node->parent = treeNodeSetParent(node, right);
}

static void treeRotateRight(memoryTree *tree, memTreeNode *node){
	memTreeNode *left = node->left;
	memTreeNode *tempNode = left->right;

	node->left = tempNode;
	//The left child of the node's left child
	//should now become the node's left child.
	if(tempNode != NULL){
		tempNode->parent = treeNodeSetParent(tempNode, node);
	}

	tempNode = treeNodeGetParent(node);
	//The node's parent should point
	//to the node's old left child.
	if(tempNode != NULL){
		if(tempNode->left == node){
			tempNode->left = left;
		}else{
			tempNode->right = left;
		}
	}else{
		tree->root = left;
	}
	//Update the left child's parent.
	left->parent = treeNodeSetParent(left, tempNode);

	//The node's old left child
	//should point to the node.
	left->right = node;
	//Update the node's parent.
	node->parent = treeNodeSetParent(node, left);
}
