#ifndef memoryTree_h
#define memoryTree_h


#include <string.h>

#include "settingsMemory.h"
#include "memoryShared.h"


#define MEMTREE_BLOCK_HEADER_SIZE sizeof(memTreeListNode)
#define MEMTREE_BLOCK_MIN_BODY_SIZE ((uintptr_t)memoryAlign(sizeof(memTreeNode)))
#define MEMTREE_BLOCK_MIN_SIZE (MEMTREE_BLOCK_HEADER_SIZE + MEMTREE_BLOCK_MIN_BODY_SIZE)

#define memTreeRegionStart(region) (((memoryRegion *)(region))->start)

//Return the minimum block size for an element of "size" bytes.
#define memTreeGetBlockSize(size) ((size_t)memoryAlign( \
	((size) > MEMTREE_BLOCK_MIN_BODY_SIZE) ? (size) : MEMTREE_BLOCK_MIN_BODY_SIZE \
))
//Return the amount of memory required for a
//memory tree with "size" many usable bytes.
#define memTreeMemoryForSize(size) memoryGetRequiredSize(size)


//Block data usage diagrams:
//Used:      [list][           data           ]
//Free:      [list][tree][        fill        ]
//List Node: [prevSize][size]
//Tree Node: [left][right][parent]

/* Our blocks are aligned to addresses that are evenly   */
/* divisible by 8. Because of this, the size of blocks   */
/* will also be evenly divisible by 8, allowing us to    */
/* store additional data in the three least significant  */
/* bits of "size". We use this for the active flag as    */
/* well as flags that indicate whether the block is the  */
/* first or last block in the linked list. The parent    */
/* pointers stored in tree nodes are also aligned like   */
/* this, allowing us to store their colour.              */
/*                                                       */
/* Note that the size includes the entire block.         */
/* This includes any headers, data and filling.          */

/** It may be worthwhile to investigate solutions such as TLSF. **/


//Our generic pool uses a doubly linked list that joins every block...
typedef struct memTreeListNode {
	//Note that these sizes include the block's header.
	//The "prevSize" element also includes flags in
	//the last three bits specifying if it's active,
	//if it's the first block in the allocator and
	//if it's the last block in the allocator.
	size_t prevSize;
	size_t size;
} memTreeListNode;

//... and a red-black tree that only connects free blocks.
typedef struct memTreeNode memTreeNode;
typedef struct memTreeNode {
	//The last bit of the parent pointer
	//stores whether or not the node is red.
	memTreeNode *parent;
	memTreeNode *left;
	memTreeNode *right;
} memTreeNode;


typedef struct memoryTree {
	memTreeNode *root;

	//This is stored at the very end of the allocated memory,
	//meaning it can be used as a pointer to the end. The
	//structure contains a pointer to the start of the region
	//as well as a pointer to the extension that follows it.
	//The start of the region is used as the
	//start of the tree's linked list component.
	memoryRegion *region;
} memoryTree;


void *memTreeInit(memoryTree *tree, void *memory, const size_t memorySize);

void *memTreeAlloc(memoryTree *tree, const size_t blockSize);
void *memTreeRealloc(memoryTree *tree, void *block, const size_t blockSize);

void memTreeFree(memoryTree *tree, void *block);

memTreeNode *memTreeClearRegion(void *memory, const size_t memorySize);
void memTreeClear(memoryTree *tree, void *memory, const size_t memorySize);

void *memTreeExtend(memoryTree *tree, void *memory, const size_t memorySize);

void memTreeDelete(memoryTree *tree);

#ifdef MEMTREE_DEBUG
void memTreePrintAllSizes(memoryTree *tree);
void memTreePrintFreeSizes(memoryTree *tree);
#endif


#endif