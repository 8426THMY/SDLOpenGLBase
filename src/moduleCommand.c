#include "moduleCommand.h"


memoryDoubleList g_cmdTokManager;


#warning "What if we aren't using the global memory manager?"


return_t moduleCommandSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		memDoubleListInit(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			MODULE_COMMAND_MANAGER_SIZE, MODULE_COMMAND_ELEMENT_SIZE
		) != NULL
	);
}

void moduleCommandCleanup(){
	MEMDOUBLELIST_LOOP_BEGIN(g_cmdTokManager, i, commandTokenized)
		moduleCmdTokFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_cmdTokManager, i)
	memoryManagerGlobalDeleteRegions(g_cmdTokManager.region);
}


// Allocate a new command array.
commandTokenized *moduleCmdTokAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAlloc(&g_cmdTokManager));
	#else
	commandTokenized *newBlock = memDoubleListAlloc(&g_cmdTokManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			MODULE_COMMAND_MANAGER_SIZE
		)){
			newBlock = memDoubleListAlloc(&g_cmdTokManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a command at the beginning of an array.
commandTokenized *moduleCmdTokPrepend(commandTokenized **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListPrepend(&g_cmdTokManager, (void **)start));
	#else
	commandTokenized *newBlock = memDoubleListPrepend(&g_cmdTokManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			MODULE_COMMAND_MANAGER_SIZE
		)){
			newBlock = memDoubleListPrepend(&g_cmdTokManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a command at the end of an array.
commandTokenized *moduleCmdTokAppend(commandTokenized **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAppend(&g_cmdTokManager, (void **)start));
	#else
	commandTokenized *newBlock = memDoubleListAppend(&g_cmdTokManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			MODULE_COMMAND_MANAGER_SIZE
		)){
			newBlock = memDoubleListAppend(&g_cmdTokManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a command after the element "prevData".
commandTokenized *moduleCmdTokInsertBefore(commandTokenized **const restrict start, commandTokenized *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertBefore(&g_cmdTokManager, (void **)start, (void *)prevData));
	#else
	commandTokenized *newBlock = memDoubleListInsertBefore(&g_cmdTokManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			MODULE_COMMAND_MANAGER_SIZE
		)){
			newBlock = memDoubleListInsertBefore(&g_cmdTokManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert a command after the element "data".
commandTokenized *moduleCmdTokInsertAfter(commandTokenized **const restrict start, commandTokenized *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertAfter(&g_cmdTokManager, (void **)start, (void *)data));
	#else
	commandTokenized *newBlock = memDoubleListInsertAfter(&g_cmdTokManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			MODULE_COMMAND_MANAGER_SIZE
		)){
			newBlock = memDoubleListInsertAfter(&g_cmdTokManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

commandTokenized *moduleCmdTokNext(const commandTokenized *const restrict cmdTok){
	return(memDoubleListNext(cmdTok));
}

commandTokenized *moduleCmdTokPrev(const commandTokenized *const restrict cmdTok){
	return(memDoubleListPrev(cmdTok));
}

// Free a command that has been allocated.
void moduleCmdTokFree(commandTokenized **const restrict start, commandTokenized *const restrict cmdTok){
	memDoubleListFree(&g_cmdTokManager, (void **)start, (void *)cmdTok);
}

// Free an entire command array.
void moduleCmdTokFreeArray(commandTokenized **const restrict start){
	commandTokenized *cmdTok = *start;
	while(cmdTok != NULL){
		moduleCmdTokFree(start, cmdTok);
		cmdTok = *start;
	}
}

// Delete every command in the manager.
void moduleCmdTokClear(){
	MEMDOUBLELIST_LOOP_BEGIN(g_cmdTokManager, i, commandTokenized)
		moduleCmdTokFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_cmdTokManager, i)
	memDoubleListClear(&g_cmdTokManager);
}