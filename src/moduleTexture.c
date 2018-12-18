#include "moduleTexture.h"


memoryPool textureManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleTextureSetup(){
	//The module's setup will be successful if we
	//can allocate enough memory for our manager
	//and the error object can be setup correctly.
	return(
		memPoolInit(
			&textureManager,
			memoryManagerGlobalAlloc(memPoolMemoryForBlocks(MEMORY_MAX_TEXTURES, sizeof(texture))),
			MEMORY_MAX_TEXTURES, sizeof(texture)
		) != NULL &&
		textureSetupError()
	);
}


//Allocate memory for a texture
//and return a handle to it.
texture *moduleTextureAlloc(){
	return(memPoolAlloc(&textureManager));
}

//Free a texture that has been allocated.
void moduleTextureFree(texture *tex){
	textureDelete(tex);
	memPoolFree(&textureManager, tex);
}


#warning "Check for invalid flag to end the loop!"
void moduleTextureCleanup(){
	/*const size_t blockSize = textureManager.blockSize;
	memoryRegion *region = textureManager.region;
	//Loop through every region of the object allocator!
	while(region != NULL){
		//We store a pointer to the beginning of
		//the usable memory so we can free the
		//region once we've finished with it.
		void *memory = region->start;

		void *block = memory;
		//Free every block in the region!
		while(block < (void *)region){
			moduleTextureFree((texture *)memPoolBlockFlagGetData(block));
			block = memPoolBlockGetNextBlock(block, blockSize);
		}

		//Now move to the next region
		//and free the previous one!
		region = region->next;
		memoryManagerGlobalFree(memory);
	}*/

	memoryManagerGlobalFree(textureManager.region->start);
}


//Find a texture whose name matches "name".
texture *moduleTextureFind(const char *name){
	const size_t blockSize = textureManager.blockSize;
	memoryRegion *region = textureManager.region;
	//Loop through every region of the object allocator!
	while(region != NULL){
		void *block = region->start;
		//Loop through every block of the current region!
		while(block < (void *)region){
			//We're only interested in active blocks.
			if(memPoolBlockIsUsed(block)){
				//Get a reference to the actual data
				//and check if its name matches!
				texture *match = (texture *)memPoolBlockFlagGetData(block);
				if(strcmp(name, match->name) == 0){
					return(match);
				}
			}

			block = memPoolBlockGetNextBlock(block, blockSize);
		}

		region = region->next;
	}

	return(NULL);
}