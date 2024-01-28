#include "moduleCommand.h"


#include "memoryManager.h"


// commandTokenized
moduleDefineDoubleList(CmdTok, commandTokenized, g_cmdTokManager, MODULE_COMMAND_MANAGER_SIZE)
moduleDefineDoubleListFree(CmdTok, commandTokenized, g_cmdTokManager)


return_t moduleCommandSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(moduleCmdTokInit());
}

void moduleCommandCleanup(){
	moduleCmdTokDelete();
}