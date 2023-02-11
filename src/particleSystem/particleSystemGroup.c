#include "particleSystemGroup.h"


#include "particleSystemNode.h"


void particleSysNodeGroupInit(
	particleSystemNodeGroup *const restrict group,
	const transform *const restrict state
){

	group->nodes = NULL;
	group->numNodes = 0;
	group->state = state;
}


// Insert a node at the beginning of the group's list.
void particleSysNodeGroupPrepend(
	particleSystemNodeGroup *const restrict group,
	particleSystemNode *const restrict node
){

	node->group = group;
	node->prevSibling = NULL;
	node->nextSibling = group->nodes;

	if(group->nodes != NULL){
		group->nodes->prevSibling = node;
	}
	group->nodes = node;
	++group->numNodes;
}


/*
** Orphan all of the particle system nodes in the group.
** This typically involves setting their lifetimes to zero
** and removing any references to the group.
*/
void particleSysNodeGroupOrphan(particleSystemNodeGroup *const restrict group){
	particleSystemNode *curNode = group->nodes;
	while(curNode != NULL){
		particleSystemNode *const nextNode = curNode->nextSibling;
		particleSysNodeOrphan(curNode);
		curNode = nextNode;
	}
}

// Remove a node from within the group's list.
void particleSysNodeGroupRemove(
	particleSystemNodeGroup *const restrict group,
	particleSystemNode *const restrict node
){

	if(node->prevSibling == NULL){
		group->nodes = node->nextSibling;
	}else{
		node->prevSibling->nextSibling = node->nextSibling;
	}
	if(node->nextSibling != NULL){
		node->nextSibling->prevSibling = node->prevSibling;
	}
}