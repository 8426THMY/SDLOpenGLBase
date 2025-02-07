#include "particleSystemNodeContainer.h"


#include "particleSystemNode.h"
#include "particleRenderer.h"

#include "moduleParticle.h"


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

// Allocate and spawn an instance of a particle system node.
particleSystemNode *particleSysNodeContainerInstantiate(
	particleSystemNodeContainer *const restrict container
){

	particleSystemNode *const node = moduleParticleSysNodePrepend(&container->instances);
	particleSysNodeInit(node, container->nodeDef);
	node->container = container;

	return(node);
}


// Update a container by updating all of the instances it contains.
void particleSysNodeContainerUpdate(
	particleSystemNodeContainer *const restrict container,
	const camera *const restrict cam, const float dt
){

	particleSystemNode *curNode = container->instances;
	particleSystemNode *prevNode = NULL;
	// Update each instance of this node.
	while(curNode != NULL){
		particleSystemNode *const nextNode = moduleParticleSysNodeNext(curNode);

		#warning "Should we update new particles on the same tick we initialize them?"
		#warning "The way we're currently doing things, they stay stationary for a frame."

		// Delete the node if it's dead.
		if(particleSysNodeDead(curNode)){
			particleSysNodeDelete(curNode);
			moduleParticleSysNodeFree(&container->instances, curNode, prevNode);
		}else{
			// We update our particles before emitting them so that
			// new particles stay where they spawned for one tick.
			particleSysNodeUpdateParticles(curNode, dt);
			particleSysNodeUpdateEmitters(curNode, dt);
			curNode->lifetime -= dt;

			// Sort the node's particles! This should hopefully
			// mean that sorting them during rendering is faster.
			particleSysNodePresort(curNode, cam);
		}

		prevNode = curNode;
		curNode  = nextNode;
	}
}

/*
** Fill the specified buffer up with each instance's
** particles and return the next empty position in the buffer.
*/
void particleSysNodeContainerBatch(
	const particleSystemNodeContainer *const restrict container,
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
		#error "Is there a better way of doing all of this?"
		/** Currently, we check if the batch is full after adding  **/
		/** each particle. This results in minimal draw calls, but **/
		/** we have to keep checking if the batch is full. One way **/
		/** to improve this is to check how many particles we have **/
		/** room for, add them to the batch, draw it and begin on  **/
		/** on a new one. We have to do this inside the rendering  **/
		/** function, as beams could look disconnected otherwise.  **/
		/*
		size_t particlesBatched = 0;
		const keyValue *curKeyValue = keyValues;

		do {
			const size_t currentBatchSize = uintMin(remainingRoom, numParticles - particlesBatched);
			// Get the last key value that will fit in the current batch.
			const keyValue *const lastKeyValue = &curKeyValue[currentBatchSize];

			#warning "This doesn't work at all for beams. Not only would this recompute the spline every time, but we'd also create disconnects in the beam for every batch we build."
			// Add as many particles to the batch as we can!
			particleRendererBatch(partRenderer, &curNode->manager, curKeyValue, lastKeyValue, batch, cam, dt);
			// Draw the batch if it's now full!
			if(remainingRoom <= numParticles - particlesBatched){
				spriteRendererDrawFull(batch);
			}

			particlesBatched += currentBatchSize;
			curKeyValue = lastKeyValue;
		} while(particlesBatched < numParticles);
		*/
		/** 1. Allocate and sort an array of key-values.              **/
		/** 2. Iterate over the key-values in the rendering function. **/
		/**                                                           **/
		/** If we never need to iterate over them in order,           **/
		/** maybe we should presort them using list pointers?         **/
		const keyValue *const keyValues = particleSysNodeSort(curNode, cam, dt);
		// Add the current instance's particles to the batch!
		particleRendererBatch(partRenderer, &curNode->manager, keyValues, batch, cam, dt);
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