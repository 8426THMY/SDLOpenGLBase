#include "particleSubsystem.h"


#include "particle.h"
#include "particleSystemNode.h"


// Initialize the list of child nodes.
void particleSubsysInstantiate(
	particleSubsystem *const restrict subsys,
	const particleSystemNodeContainer *const restrict children,
	const size_t numChildren,
	particle *const restrict parent
){

	const particleSystemNodeContainer *curChild = children;
	const particleSystemNodeContainer *const lastChild = &children[numChildren];

	subsys->nodes = NULL;
	subsys->numNodes = numChildren;

	// Create instances of each of the child nodes.
	for(; curContainer != lastContainer; ++curContainer){
		particleSystemNode *const node = particleSysNodeContainerInstantiate(curContainer);

		node->parent = parent;
		node->prevSibling = NULL;
		node->nextSibling = subsys->nodes;

		if(subsys->nodes != NULL){
			subsys->nodes->prevSibling = node;
		}
		subsys->nodes = node;
	}
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