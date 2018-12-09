#include "moduleSkeleton.h"


memoryPool *skeleAnimManager;


#warning "How are we going to store animations? How about bones?"

#warning "What if we aren't using the global memory manager?"


return_t moduleSkeletonSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager.
	return(
		(skeleAnimManager = memoryManagerGlobalAlloc(memPoolMemoryForBlocks(MEMORY_MAX_SKELEANIMS, sizeof(skeletonAnim)) + sizeof(*skeleAnimManager))) != NULL &&
		memPoolInit(skeleAnimManager, skeleAnimManager + sizeof(*skeleAnimManager), MEMORY_MAX_SKELEANIMS, sizeof(skeletonAnim))
	);
}


//Allocate memory for a skeleton
//and return a handle to it.
skeletonAnim *moduleSkeletonAnimAlloc(){
	return(memPoolAlloc(skeleAnimManager));
}

//Free a skeleton that has been allocated.
void moduleSkeletonAnimFree(skeletonAnim *skeleAnim){
	skeleAnimDelete(skeleAnim);
	memPoolFree(skeleAnimManager, skeleAnim);
}


void moduleSkeletonCleanup(){
	/*size_t i = loadedTextures.size;
	//Loop through the skeleton manager.
	while(i > 0){
		--i;
		skeletonDelete((skeleton *)vectorGet(&loadedSkeletons, i));
	}*/
	memoryManagerGlobalFree(skeleAnimManager);
}