#include "particleSubsystem.h"


#include "particleSystemNode.h"


/*
** Initialize the array of children. We expect the
** owner to set the current and previous transforms
*/
void particleSubsysInit(particleSubsystem *const restrict subsys){
	transformInit(&subsys->state[0]);
	transformInit(&subsys->state[1]);
	subsys->nodes = NULL;
	subsys->numNodes = 0;
}


// Insert a node at the beginning of the subsystem's list.
void particleSubsysPrepend(
	particleSubsystem *const restrict subsys,
	particleSystemNode *const restrict node
){

	node->parent = subsys;
	node->prevSibling = NULL;
	node->nextSibling = subsys->nodes;

	if(subsys->nodes != NULL){
		subsys->nodes->prevSibling = node;
	}
	subsys->nodes = node;
	++subsys->numNodes;
}


// Orphan all of the particle system nodes in the subsystem.
void particleSubsysOrphan(particleSubsystem *const restrict subsys){
	particleSystemNode *curNode = subsys->nodes;
	while(curNode != NULL){
		particleSystemNode *const nextNode = curNode->nextSibling;
		particleSysNodeOrphan(curNode);
		curNode = nextNode;
	}
}

// Remove a node from within the subsystem's list.
void particleSubsysRemove(
	particleSubsystem *const restrict subsys,
	particleSystemNode *const restrict node
){

	if(node->prevSibling == NULL){
		subsys->nodes = node->nextSibling;
	}else{
		node->prevSibling->nextSibling = node->nextSibling;
	}
	if(node->nextSibling != NULL){
		node->nextSibling->prevSibling = node->prevSibling;
	}
}