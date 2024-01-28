#include "moduleCommand.h"


#include "memoryManager.h"


// commandTokenized
moduleDefineDoubleList(CmdTok, commandTokenized, g_cmdTokManager, MODULE_COMMAND_MANAGER_SIZE)
moduleDefineDoubleListFree(CmdTok, commandTokenized, g_cmdTokManager)


return_t moduleCommandSetup(){
	return(moduleCmdTokInit());
}

void moduleCommandCleanup(){
	moduleCmdTokDelete();
}