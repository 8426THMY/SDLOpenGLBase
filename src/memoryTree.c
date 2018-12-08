#include "memoryTree.h"


#include <stdio.h>

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
#define listNodeGetNextList(node, size) ((memTreeListNode *)(((uintptr_t)(node)) + (size) + MEMTREE_BLOCK_HEADER_SIZE))
//Get the previous block's list data from the current one's.
#define listNodeGetPrevList(node, size) ((memTreeListNode *)(((uintptr_t)(node)) - MEMTREE_BLOCK_HEADER_SIZE - (size)))
//Return the tree component of an inactive block from its list component.
#define listNodeGetTree(node) ((memTreeNode *)(((uintptr_t)(node)) + MEMTREE_BLOCK_HEADER_SIZE))

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
#define treeNodeGetNextList(node, size) ((memTreeListNode *)(((uintptr_t)node) + (size)))
//Get the previous block's list data from the current one's tree data.
#define treeNodeGetPrevList(node, size) ((memTreeListNode *)(((uintptr_t)node) - (size)))
//Get the next block's tree data from the current one's tree data.
#define treeNodeGetNextTree(node, size) ((memTreeNode *)(((uintptr_t)node) + (size) + MEMTREE_BLOCK_HEADER_SIZE))
//Get the previous block's tree data from the current one's tree data.
#define treeNodeGetPrevTree(node, size) ((memTreeNode *)(((uintptr_t)node) - MEMTREE_BLOCK_HEADER_SIZE - (size)))
//Return the list component of an inactive block from its tree component.
#define treeNodeGetList(node) ((memTreeListNode *)(((uintptr_t)(node)) - MEMTREE_BLOCK_HEADER_SIZE))

//Return the size of an inactive block from its tree component.
#define treeNodeGetSize(node) *((size_t *)(((uintptr_t)(node)) - sizeof(size_t)))
//Return the size of an inactive block from its tree component.
#define treeNodeGetPrevSize(node) *((size_t *)(((uintptr_t)(node)) - MEMTREE_BLOCK_HEADER_SIZE))
//Return the size of an inactive block from its tree component.
#define treeNodeGetFlags(node) listNodeGetFlags(treeNodeGetPrevSize(node))

//We'll need to remove the colour bit from the
//parent pointer if we want to get its real value.
#define treeNodeGetParent(node) ((memTreeNode *)(((uintptr_t)(((memTreeNode *)(node))->parent)) & MEMORY_DATA_MASK))
//Update the node's parent pointer without changing its colour.
#define treeNodeSetParent(nodeParent, colour) ((memTreeNode *)(((uintptr_t)(nodeParent)) | (colour)))
//The colour of a node is stored in the least
//significant bit of the parent pointer.
#define treeNodeGetColour(node) (((uintptr_t)(((memTreeNode *)(node))->parent)) & MEMORY_FLAG_MASK)
//Write the specified colour flag
//to the specified parent pointer.
#define treeNodeSetColour(nodeParent, colour) ((memTreeNode *)(((uintptr_t)(nodeParent)) | (colour)))
//Get a node's parent from its parent pointer.
#define treeNodeInfoGetParent(info) ((memTreeNode *)(((uintptr_t)(info)) & MEMORY_DATA_MASK))
//Update the node's parent pointer without changing its colour.
#define treeNodeInfoSetParent(info, nodeParent) ((memTreeNode *)(((uintptr_t)(nodeParent)) | treeNodeInfoGetParent(info)))
//Get a node's colour from its parent pointer.
#define treeNodeInfoGetColour(info) (((uintptr_t)(info)) & MEMORY_FLAG_MASK)

//Since the black flag is defined as zero, we can check if
//its parent pointer is the same as its parent's actual address.
#define treeNodeIsBlack(node, nodeParent) ((((memTreeNode *)(node))->parent) == ((memTreeNode *)(nodeParent)))
//Same as above, but checks if the two are not equal.
#define treeNodeIsRed(node, nodeParent) ((((memTreeNode *)(node))->parent) != ((memTreeNode *)(nodeParent)))
//If we want to make a node black and we have the address
//of its parent, we can just set its parent pointer to that.
#define treeNodeMakeBlack(parent) ((memTreeNode *)(parent))
//We can't really do the same the other way around, so do it the slow way.
#define treeNodeMakeRed(parent) (treeNodeSetColour((parent), TREE_NODE_COLOUR_RED))


#warning "What if the beginning or end of a memory region is not aligned?"


static void treeInsert(memoryTree *tree, memTreeNode *node, const size_t nodeSize);
static void treeDelete(memoryTree *tree, memTreeNode *node);
static void treeRotateLeft(memoryTree *tree, memTreeNode *node);
static void treeRotateRight(memoryTree *tree, memTreeNode *node);


void *memTreeInit(memoryTree *tree, void *memory, const size_t memorySize){
	//Make sure the user isn't being difficult.
	if(memory != NULL){
		const size_t regionSize = memoryGetRegionSize(memorySize);

		//Create a free block to occupy all of the allocated memory.
		memTreeNode *root = memTreeClearRegion(memory, regionSize);
		root->left = NULL;
		root->right = NULL;
		root->parent = NULL;

		tree->root = root;

		//Set up the tree's memory region footer.
		tree->region = memoryGetRegionFromSize(memory, regionSize);
		tree->region->start = memory;
		tree->region->next = NULL;
	}

	return(memory);
}


void *memTreeAlloc(memoryTree *tree, const size_t blockSize){
	//Make sure the new block's size is neither too small nor misaligned.
	const size_t newSize = memTreeGetBlockSize(blockSize);

	memTreeNode *newNode = NULL;
	size_t newBlockSize;

	memTreeNode *currentNode = tree->root;
	//Search our free tree for the best fitting free block.
	while(1){
		const size_t currentBlockSize = treeNodeGetSize(currentNode);

		memTreeNode *nextNode;
		//If the new size is smaller than the current node's, go left.
		if(newSize < currentBlockSize){
			//This node is the smallest node we've found that
			//is large enough to store our data, so remember it.
			newNode = currentNode;
			newBlockSize = currentBlockSize;

			nextNode = currentNode->left;

		//If the new size is larger than the current node's, go right.
		}else if(newSize > currentBlockSize){
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
		size_t sizeDiff = newBlockSize - newSize;

		//Remove this block from the tree.
		treeDelete(tree, newNode);

		//We'll have to set the block to active, too.
		currentBlock->prevSize = listNodeMakeActive(currentBlock->prevSize);
		//Provided the leftover memory is large enough,
		//create a new empty block after the current one.
		if(sizeDiff >= MEMTREE_BLOCK_MIN_SIZE){
			memTreeListNode *freeBlock = treeNodeGetNextList(newNode, newSize);

			currentBlock->size = newSize;
			//We don't need to explicitly make the block inactive,
			//since we would just be performing an OR with '0'.
			freeBlock->prevSize = newSize;

			//If this block isn't the last, we'll
			//have to modify the block after it.
			if(!listNodeIsLast(currentBlock->prevSize)){
				memTreeListNode *nextBlock = listNodeGetNextList(freeBlock, sizeDiff);

				//If the next block is inactive, we
				//can merge it with our new free block.
				if(!listNodeIsActive(nextBlock->prevSize)){
					const size_t nextNodeSize = nextBlock->size + MEMTREE_BLOCK_HEADER_SIZE;
					//Add the next block's size to the difference.
					//We'll use this for the free block's size.
					sizeDiff += nextNodeSize;

					treeDelete(tree, listNodeGetTree(nextBlock));

					//If this block still isn't the last, update the
					//previous size element of the one following it.
					if(!listNodeIsLast(nextBlock->prevSize)){
						nextBlock = listNodeGetNextList(nextBlock, nextNodeSize);
						nextBlock->prevSize = listNodeSetSize(nextBlock->prevSize, sizeDiff);

					//Otherwise, give the block's
					//"last" flag to our free block.
					}else{
						freeBlock->prevSize = listNodeMakeLast(freeBlock->prevSize);
					}
				}

			//If the free block is the last, give
			//it the used block's "last" flag.
			}else{
				currentBlock->prevSize = listNodeRemoveLast(currentBlock->prevSize);
				freeBlock->prevSize = listNodeMakeLast(freeBlock->prevSize);
			}

			//Now we can set the free block's
			//size and add it to the tree.
			freeBlock->size = sizeDiff - MEMTREE_BLOCK_HEADER_SIZE;
			treeInsert(tree, listNodeGetTree(freeBlock), sizeDiff);
		}
	}

	//Provide the user a means of accessing their memory block!
	//Because "newNode" represents the beginning of the tree data,
	//which is now where our user data starts, we can just return it.
	//If we couldn't find a block large enough, this will return NULL.
	return(newNode);
}

void *memTreeRealloc(memoryTree *tree, void *block, const size_t blockSize){
	if(block != NULL){
		const size_t oldSize = treeNodeGetSize(block);
		//If the block is already big enough, we can exit early.
		if(blockSize <= oldSize){
			return(block);
		}

		void *tempBlock = block;
		//Otherwise, we will need to check the blocks to
		//the left and right in case we are able to coalesce.
		memTreeListNode *newBlock = treeNodeGetList(block);
		size_t newSize = newBlock->size;

		//If this block is not the last, we might be able to merge right.
		if(!listNodeIsLast(newBlock->prevSize)){
			memTreeListNode *rightBlock = listNodeGetNextList(newBlock, newSize);
			//We can only merge it if the block to its right is free.
			if(!listNodeIsActive(rightBlock->prevSize)){
				//Make sure we preserve the right block's "last" flag.
				newBlock->prevSize |= listNodeIsLast(rightBlock->prevSize);
				//We're merging the two blocks, so add their sizes.
				newSize += rightBlock->size;

				//Remove the right block from the free
				//tree, as it no longer exists. We'll
				//add the free block to the tree later.
				treeDelete(tree, listNodeGetTree(rightBlock));
			}
		}

		//If the new block still isn't big
		//enough, check the block to its left.
		if(newSize < blockSize){
			//If this block is not the first, we might be able to merge left.
			if(!listNodeIsFirst(newBlock->prevSize)){
				memTreeListNode *leftBlock = listNodeGetPrevList(newBlock, listNodeGetSize(newBlock->prevSize));
				//We can only merge it if the block to its left is free.
				if(!listNodeIsActive(leftBlock->prevSize)){
					memTreeNode *leftTree = listNodeGetTree(leftBlock);

					//Make sure we preserve the free block's "last" flag.
					leftBlock->prevSize |= listNodeIsLast(newBlock->prevSize);
					//We're merging the two blocks, so add their sizes.
					newSize += leftBlock->size;

					//The free block no longer exists as it has been merged,
					//so our pointers should now point to the left block.
					block = leftTree;
					newBlock = leftBlock;

					//Remove the left block from the free tree,
					//as it's outdated. We'll add it back later.
					treeDelete(tree, leftTree);
				}
			}
		}

		//We'll need to update the previous size element of
		//the block to the right of the one we're removing.
		if(!listNodeIsLast(newBlock->prevSize)){
			memTreeListNode *nextBlock = listNodeGetNextList(newBlock, newSize);
			nextBlock->prevSize = listNodeSetSize(nextBlock->prevSize, newSize);
		}

		//Update the new block's size.
		newBlock->size = newSize;
		//If our new block is big enough, return a pointer to it!
		if(newSize >= blockSize){
			//We only need to copy the user's data
			//if the block's pointer has moved.
			if(block != tempBlock){
				memmove(block, tempBlock, oldSize);
			}

			return(block);
		}

		//Otherwise, we've done all of the merging we normally do when
		//freeing a block, so allocate a new block and free the old one.
		newBlock->prevSize = listNodeRemoveActive(newBlock->prevSize);

		//We need to allocate a new block first so we don't overwrite
		//the user's data with any of the free block header information.
		tempBlock = memTreeAlloc(tree, blockSize);
		memcpy(tempBlock, block, oldSize);

		//Now we can add the old block back to the tree.
		treeInsert(tree, block, newSize);


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
			memTreeNode *leftTree = listNodeGetTree(leftBlock);

			//Make sure we preserve the free block's "last" flag.
			leftBlock->prevSize |= listNodeIsLast(newBlock->prevSize);
			//We're merging the two blocks, so add their sizes.
			newSize += leftBlock->size;

			//The free block no longer exists as it has been merged,
			//so our pointers should now point to the left block.
			block = leftTree;
			newBlock = leftBlock;

			//Remove the left block from the free tree,
			//as it's outdated. We'll add it back later.
			treeDelete(tree, leftTree);
		}
	}
	//If this block is not the last, we might be able to merge right.
	if(!listNodeIsLast(newBlock->prevSize)){
		memTreeListNode *rightBlock = listNodeGetNextList(newBlock, newSize);
		//We can only merge it if the block to its right is free.
		if(!listNodeIsActive(rightBlock->prevSize)){
			//Make sure we preserve the right block's "last" flag.
			newBlock->prevSize |= listNodeIsLast(rightBlock->prevSize);
			//We're merging the two blocks, so add their sizes.
			newSize += rightBlock->size;

			//Remove the right block from the free
			//tree, as it no longer exists. We'll
			//add the free block to the tree later.
			treeDelete(tree, listNodeGetTree(rightBlock));

			//If there is a block after this one,
			//update its previous size property.
			if(!listNodeIsLast(newBlock->prevSize)){
				memTreeListNode *nextBlock = listNodeGetNextList(newBlock, newSize);
				nextBlock->prevSize = listNodeSetSize(nextBlock->prevSize, newSize);
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
	((memTreeListNode *)memory)->size = memorySize - MEMTREE_BLOCK_HEADER_SIZE;

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
	memTreeNode *root = memTreeClearRegion(memory, (size_t)((byte_t *)region->start - (byte_t *)region));
	root->left = NULL;
	root->right = NULL;
	root->parent = NULL;
	tree->root = root;

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
		while(1){
			//If the node is active, we just print its size and flags.
			if(listNodeIsActive(node->prevSize)){
				printf(
					"Address: %u, Size: %u, Flags: %u\n\n",
					(uintptr_t)node, node->size, listNodeGetFlags(node->prevSize)
				);

			//Otherwise, we can print its tree data too!
			}else{
				memTreeNode *nodeTree = listNodeGetTree(node);
				printf(
					"Address: %u, Size: %u, Flags: %u,\n"
					"Left: %u, Right: %u, Parent: %u, Colour: %u\n\n",
					(uintptr_t)node, node->size, listNodeGetFlags(node->prevSize),
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
	memTreeNode *nodeLeft;
	//Start by finding the tree's leftmost node.
	while((nodeLeft = node->left) != NULL){
		node = nodeLeft;
	}

	while(node != NULL){
		//Sizes include the block's header, so
		//we need to remove that when we print it.
		printf("Address: %u, Size: %u, Flags: %u,\n"
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
#endif


static void treeInsert(memoryTree *tree, memTreeNode *node, const size_t nodeSize){
	memTreeNode *root = tree->root;
	//Make sure our tree isn't empty.
	if(root != NULL){
		memTreeNode *tempNode = root;
		//Search our tree to find where the new node should be inserted.
		while(1){
			size_t currentBlockSize = treeNodeGetSize(tempNode);

			//If the new size is smaller than or equal to the
			//current node's, go left. Otherwise, go right.
			memTreeNode **nextNode = (nodeSize <= currentBlockSize) ? &tempNode->left : &tempNode->right;

			//If we've reached the end of the tree, add our new node!
			if(*nextNode == NULL){
				*nextNode = node;
				//Update the new leaf node's properties!
				node->left = NULL;
				node->right = NULL;
				node->parent = treeNodeMakeRed(tempNode);

				break;
			}
			//Otherwise, check the next node!
			tempNode = *nextNode;
		}

		//Now that our node has been inserted, we
		//must make sure our tree is still compliant.

		//Keep recolouring while the current node's parent is red.
		while(node != root && (tempNode = treeNodeGetParent(node), treeNodeGetColour(tempNode))){
			memTreeNode *grandparent = treeNodeGetParent(tempNode);

			if(tempNode == grandparent->left){
				memTreeNode *uncle = grandparent->right;

				//If we have a red uncle, we can simply
				//do a colour swap and move up the tree.
				if(uncle != NULL && treeNodeIsRed(uncle, grandparent)){
					tempNode->parent = treeNodeMakeBlack(grandparent);
					uncle->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(treeNodeGetParent(grandparent));

					node = grandparent;
				}else{
					//Left right case.
					if(node == tempNode->right){
						treeRotateLeft(tree, tempNode);
						tempNode = node;
					}

					//Left left case.
					tempNode->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(treeNodeGetParent(grandparent));
					treeRotateRight(tree, grandparent);

					//No need to continue, we know the tree is valid.
					break;
				}
			}else{
				memTreeNode *uncle = grandparent->left;

				//If we have a red uncle, we can simply
				//do a colour swap and move up the tree.
				if(uncle != NULL && treeNodeIsRed(uncle, grandparent)){
					tempNode->parent = treeNodeMakeBlack(grandparent);
					uncle->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(treeNodeGetParent(grandparent));

					node = grandparent;
				}else{
					//Right left case.
					if(node == tempNode->left){
						treeRotateRight(tree, tempNode);
						tempNode = node;
					}

					//Right right case.
					tempNode->parent = treeNodeMakeBlack(grandparent);
					grandparent->parent = treeNodeMakeRed(treeNodeGetParent(grandparent));
					treeRotateLeft(tree, grandparent);

					//No need to continue, we know the tree is valid.
					break;
				}
			}
		}

		//Make sure the root node's parent is NULL.
		tree->root->parent = NULL;

	//If the tree is empty, inserting is quite simple.
	}else{
		tree->root = node;
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;
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
			memTreeNode *successorLeft;

			successor = nodeRight;
			//Find the smallest node in the right subtree.
			while(successorLeft = successor->left, successorLeft != NULL){
				successor = successorLeft;
			}
			child = successor->right;

		//The node has a left child but no right child.
		}else{
			successor = node;
			child = node->left;
		}

	//The node has a right child but no left child.
	}else if(nodeRight != NULL){
		successor = node;
		child = node->right;

	//The node has no children.
	}else{
		successor = node;
		child = NULL;
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
		child->parent = treeNodeSetParent(successorParent, treeNodeIsRed(child, successor));
	}

	//If the successor and its child are both
	//black, we still have some work to do.
	if(treeNodeIsBlack(successor, successorParent)){
		memTreeNode *root = tree->root;
		while(child != root && (child == NULL || treeNodeIsBlack(child, successorParent))){
			memTreeNode *sibling;
			memTreeNode *siblingLeft;
			memTreeNode *siblingRight;

			if(child == successorParent->left){
				sibling = successorParent->right;

				//If the sibling is red, swap its colour with its parent and rotate left.
				if(treeNodeIsRed(sibling, successorParent)){
					sibling->parent = treeNodeMakeBlack(successorParent);
					successorParent->parent = treeNodeMakeRed(treeNodeGetParent(successorParent));
					treeRotateLeft(tree, successorParent);
					sibling = successorParent->right;
				}

				//Keep pointers to our sibling's children.
				siblingLeft = sibling->left;
				siblingRight = sibling->right;

				//If both of the sibling's children are black,
				//make the sibling red and continue upwards.
				if((siblingLeft == NULL || treeNodeIsBlack(siblingLeft, sibling)) &&
				   (siblingRight == NULL || treeNodeIsBlack(siblingRight, sibling))){

					sibling->parent = treeNodeMakeRed(successorParent);
					child = successorParent;
					successorParent = treeNodeGetParent(successorParent);
				}else{
					//Right right case.
					if(siblingRight == NULL || treeNodeIsBlack(siblingRight, sibling)){
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

					break;
				}
			}else{
				sibling = successorParent->left;

				//If the sibling is red, swap its colour with its parent and rotate left.
				if(treeNodeIsRed(sibling, successorParent)){
					sibling->parent = treeNodeMakeBlack(successorParent);
					successorParent->parent = treeNodeMakeRed(treeNodeGetParent(successorParent));
					treeRotateRight(tree, successorParent);
					sibling = successorParent->left;
				}

				//Keep pointers to our sibling's children.
				siblingLeft = sibling->left;
				siblingRight = sibling->right;

				//If both of the sibling's children are black,
				//make the sibling red and continue upwards.
				if((siblingRight == NULL || treeNodeIsBlack(siblingRight, sibling)) &&
				   (siblingLeft == NULL || treeNodeIsBlack(siblingLeft, sibling))){

					sibling->parent = treeNodeMakeRed(successorParent);
					child = successorParent;
					successorParent = treeNodeGetParent(successorParent);
				}else{
					//Left left case.
					if(siblingLeft == NULL || treeNodeIsBlack(siblingLeft, sibling)){
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

					break;
				}
			}
		}

		//Make sure the child is black.
		if(child != NULL){
			child->parent = treeNodeMakeBlack(successorParent);
		}
	}

	//If the successor wasn't the node we wanted to delete initially,
	//we'll have to replace the original node with the successor.
	if(node != successor){
		//Update the left child's parent pointer.
		if(nodeLeft != NULL){
			nodeLeft->parent = treeNodeSetParent(successor, treeNodeIsRed(nodeLeft, node));
		}
		//Set the successor's left child.
		successor->left = nodeLeft;

		//The node's right child may have been the successor, in
		//which case we need to update the pointer we've stored.
		if(nodeRight == successor){
			nodeRight = node->right;
		}
		//Update the right child's parent pointer.
		if(nodeRight != NULL){
			nodeRight->parent = treeNodeSetParent(successor, treeNodeIsRed(nodeRight, node));
		}
		//Set the successor's right child.
		successor->right = nodeRight;

		successorParent = treeNodeGetParent(node);
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
	memTreeNode *nodeRight = node->right;
	//We can't rotate the tree if the node has no right child.
	if(nodeRight != NULL){
		memTreeNode *tempNode = nodeRight->left;

		node->right = tempNode;
		//If the child node has a left child, we'll need to move it.
		if(tempNode != NULL){
			tempNode->parent = treeNodeSetParent(node, treeNodeIsRed(tempNode, nodeRight));
		}
		nodeRight->left = node;

		tempNode = treeNodeGetParent(node);
		nodeRight->parent = treeNodeSetParent(tempNode, treeNodeIsRed(nodeRight, node));
		//If the node had a parent, make it point to the new node.
		if(tempNode != NULL){
			//Make sure we set the correct child pointer.
			if(tempNode->left == node){
				tempNode->left = nodeRight;
			}else{
				tempNode->right = nodeRight;
			}

		//Otherwise, it must have been the root node.
		}else{
			tree->root = nodeRight;
		}
		node->parent = treeNodeSetParent(nodeRight, treeNodeGetColour(node));
	}
}

static void treeRotateRight(memoryTree *tree, memTreeNode *node){
	memTreeNode *nodeLeft = node->left;
	//We can't rotate the tree if the node has no left child.
	if(nodeLeft != NULL){
		memTreeNode *tempNode = nodeLeft->right;

		node->left = tempNode;
		//If the child node has a right child, we'll need to move it.
		if(tempNode != NULL){
			tempNode->parent = treeNodeSetParent(node, treeNodeIsRed(tempNode, nodeLeft));
		}
		nodeLeft->right = node;

		tempNode = treeNodeGetParent(node);
		nodeLeft->parent = treeNodeSetParent(tempNode, treeNodeIsRed(nodeLeft, node));
		//If the node had a parent, make it point to the new node.
		if(tempNode != NULL){
			//Make sure we set the correct child pointer.
			if(tempNode->left == node){
				tempNode->left = nodeLeft;
			}else{
				tempNode->right = nodeLeft;
			}

		//Otherwise, it must have been the root node.
		}else{
			tree->root = nodeLeft;
		}
		node->parent = treeNodeSetParent(nodeLeft, treeNodeGetColour(node));
	}
}
