#include "moduleCommand.h"


#include "memoryManager.h"


moduleDefineDoubleList(CmdTok, commandTokenized, g_cmdTokManager, {}, MODULE_COMMAND_MANAGER_SIZE)


return_t moduleCommandSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		memDoubleListInit(
			&g_cmdTokManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_COMMAND_MANAGER_SIZE), MODULE_COMMAND_ELEMENT_SIZE
		) != NULL
	);
}

void moduleCommandCleanup(){
	MEMDOUBLELIST_LOOP_BEGIN(g_cmdTokManager, i, commandTokenized)
		moduleCmdTokFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_cmdTokManager, i)
	memoryManagerGlobalDeleteRegions(g_cmdTokManager.region);
}