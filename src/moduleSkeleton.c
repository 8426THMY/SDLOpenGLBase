#include "moduleSkeleton.h"


memoryPool *skeletonManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleSkeletonSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager.
	return(
		(skeletonManager = memoryManagerGlobalAlloc(memPoolMemoryForBlocks(MEMORY_MAX_SKELETONS, sizeof(skeleton)) + sizeof(*skeletonManager))) != NULL &&
		memPoolInit(skeletonManager, skeletonManager + sizeof(*skeletonManager), MEMORY_MAX_SKELETONS, sizeof(skeleton))
	);
}


//Allocate memory for a skeleton
//and return a handle to it.
skeleton *moduleSkeletonAlloc(){
	return(memPoolAlloc(skeletonManager));
}

//Free a skeleton that has been allocated.
void moduleSkeletonFree(skeleton *skele){
	skeleDelete(skele);
	memPoolFree(skeletonManager, skele);
}


void moduleSkeletonCleanup(){
	/*size_t i = loadedTextures.size;
	//Loop through the skeleton manager.
	while(i > 0){
		--i;
		skeletonDelete((skeleton *)vectorGet(&loadedSkeletons, i));
	}*/
	memoryManagerGlobalFree(skeletonManager);
}