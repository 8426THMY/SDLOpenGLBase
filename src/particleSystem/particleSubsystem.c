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

// Update each child nodes' parent pointers.
void particleSubsysUpdateParentPointers(particleSubsystem *const restrict subsys){
	particleSystemNode *curNode;
	for(; curNode != NULL; curNode = curNode->nextSibling){
		curChild->parent = &curParticle->subsys;
	}
}

// Update the parent state for each node in the subsystem.
void particleSubsysUpdateParentTransforms(
	particleSubsystem *const restrict subsys,
	const transform *const restrict parentState
){

	particleSystemNode *curNode;
	for(; curNode != NULL; curNode = curNode->nextSibling){
		particleSysNodeUpdateParentTransform(curNode, parentState);
	}
}

/*
** Orphan all of the particle system nodes in the subsystem.
** Any nodes that aren't allowed to survive without their
** parent will die the next time they're updated.
*/
void particleSubsysOrphan(particleSubsystem *const restrict subsys){
	particleSystemNode *curNode = subsys->nodes;
	while(curNode != NULL){
		particleSystemNode *const nextNode = curNode->nextSibling;
		particleSysNodeOrphan(curNode);
		curNode = nextNode;
	}
}

/*
** Remove a node from within the subsystem's list.
** This is only used when the node is deleted, so
** we don't need to set its parent pointer.
*/
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