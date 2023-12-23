#include "particleSystemNodeContainer.h"


#include "particleSystemNode.h"

#include "moduleParticle.h"


// Forward-declare any helper functions!
static size_t countParticles(
	const particleSystemNodeContainer *const restrict container
);


/*
** Recursively initialize a particle system node container
** and all of its children and grandchildren. We also return
** the address after the last child we added.
**
** This function assumes that "container" lives in an array
** that is big enough for it and all of its children. It also
** ensures that every child of a node is stored sequentially.
*/
particleSystemNodeContainer *particleSysNodeContainerInit(
	particleSystemNodeContainer *const restrict container,
	const particleSystemNodeDef *const restrict nodeDef,
	particleSystemNodeContainer *const restrict children
){

	container->nodeDef = nodeDef;
	container->instances = NULL;

	// Set the children pointer correctly if the node has no children.
	if(nodeDef->numChildren <= 0){
		container->children = NULL;

	// Otherwise, initialize all of the container's children.
	}else{
		particleSystemNodeContainer *curChild        = children;
		particleSystemNodeContainer *firstGrandchild = &curChild[nodeDef->numChildren];
		particleSystemNodeContainer *nextGrandchild  = firstGrandchild;
		const particleSystemNodeDef *curNodeDef  = nodeDef->children;
		const particleSystemNodeDef *lastNodeDef = NULL;

		container->children = children;

		// This loop recursively initializes all of the grandchildren.
		do {
			nextGrandchild = particleSysNodeContainerInit(curChild, curNodeDef, nextGrandchild);

			++curChild;
			curNodeDef = moduleParticleSysNodeDefNext(curNodeDef);
		} while(curChild != firstGrandchild);

		return(nextGrandchild);
	}

	return(children);
}

/*
** Allocate and spawn an instance of a particle system node.
** Here, "sibling" should be the owner's first sibling node.
*/
particleSystemNode *particleSysNodeContainerInstantiate(
	particleSystemNodeContainer *const restrict container
){

	particleSystemNode *const node = moduleParticleSysNodePrepend(&container->instances);

	particleSysNodeInit(node, container->nodeDef);
	node->container = container;
	node->next = moduleParticleSysNodeNext(container->instances);

	return(node);
}


// Update a container by updating all of the instances it contains.
void particleSysNodeContainerUpdate(
	particleSystemNodeContainer *const restrict container, const float dt
){

	particleSystemNode *curNode = container->instances;
	particleSystemNode *prevNode = NULL;
	// Update each instance of this node.
	while(curNode != NULL){
		particleSystemNode *const nextNode = moduleParticleSysNodeNext(curNode);

		// The particle system node is dead, so stop emitting particles.
		if(curNode->lifetime <= 0.f){
			// If no particles are left, we can delete the node.
			if(curNode->manager.numParticles <= 0){
				particleSysNodeDelete(curNode);
				moduleParticleSysNodeFree(&container->instances, curNode, prevNode);

			// Otherwise, update the remaining particles.
			}else{
				particleSysNodeUpdateParticles(curNode, dt);
			}

		// If the particle system is still alive, update as usual.
		}else{
			// We still need to check if there are particles to update.
			if(curNode->manager.numParticles > 0){
				particleSysNodeUpdateParticles(curNode, dt);
			}
			particleSysNodeUpdateEmitters(curNode, dt);
			curNode->lifetime -= dt;
		}

		prevNode = curNode;
		curNode  = nextNode;
	}

	#error "It might be a good idea to sort instances by depth here. This means we need the camera!"
}

/*
** Fill the specified buffer up with each instance's
** particles and return the next empty position in the buffer.
*/
void particleSysNodeContainerBatch(
	particleSystemNodeContainer *const restrict container,
	spriteRenderer *const restrict batch,
	const camera *const restrict cam, const float dt
){

	const particleRenderer *const partRenderer = container->nodeDef->renderer;
	const particleSystemNode *curNode = container->instances;

	// Draw the last batch if it's incompatible,
	// or continue filling the buffers if it is.
	particleRendererInitBatch(partRenderer, batch);

	// For each instance, fill the buffer with its particle data.
	// If the buffer is filled, we'll need to draw and orphan it.
	while(curNode != NULL){
		// If there's no room left in the current
		// batch, we'll have to draw and orphan it.
		#warning "Currently, we check if the batch is full after adding each particle."
		#warning "This makes the most out of the batch, but it may be slower due to all the checks."
		#warning "Instead, we can only add the new particles if there's room for all of them."
		#warning "This is less efficient in terms of memory usage when we have many large nodes."
		#warning "It also means we have a limit on the number of particles a system can spawn."
		#warning "This limit is especially small for instanced renderers."
		/*if(!spriteRendererHasRoom(batch, particleRendererBatchSize(partRenderer, &curNode->manager))){
			spriteRendererDrawFull(batch);
		}*/
		// Add the current instance's particles to the batch!
		particleRendererBatch(partRenderer, &curNode->manager, batch, cam, dt);
		curNode = moduleParticleSysNodeNext(curNode);
	}
}


void particleSysNodeContainerDelete(
	particleSystemNodeContainer *const restrict container
){

	particleSystemNode *curNode = container->instances;
	// Each container contains a number of
	// instances, so we need to free those.
	while(curNode != NULL){
		particleSysNodeDelete(curNode);
		curNode = moduleParticleSysNodeNext(curNode);
	}
	// Free the array of nodes.
	moduleParticleSysNodeFreeArray(container->instances);
}


/*
** Count the particles for each instance of a container.
** Containers are responsible for batch rendering all of
** their instances, so this lets us preallocate the buffer.
*/
static size_t particleSysNodeContainerBufferSize(
	const particleSystemNodeContainer *const restrict container
){

	size_t numParticles = 0;

	const particleSystemNode *curNode = container->instances;
	while(curNode != NULL){
		numParticles += curNode->numParticles;
		curNode = moduleParticleSysNodeNext(curNode);
	}

	return(particleRendererBufferSize(container->nodeDef->renderer, numParticles));
}