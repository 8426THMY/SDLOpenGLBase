#ifndef moduleCommand_h
#define moduleCommand_h


#include "command.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_COMMAND
#define MODULE_COMMAND_SETUP_FAIL 1

#define MODULE_COMMAND_ELEMENT_SIZE sizeof(commandTokenized)

#ifndef MEMORY_MODULE_NUM_COMMANDS
	#define MEMORY_MODULE_NUM_COMMANDS 1
#endif

#define MODULE_COMMAND_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_COMMANDS, MODULE_COMMAND_ELEMENT_SIZE)


moduleDeclareDoubleList(CmdTok, commandTokenized, g_cmdTokManager)

return_t moduleCommandSetup();
void moduleCommandCleanup();


#endif