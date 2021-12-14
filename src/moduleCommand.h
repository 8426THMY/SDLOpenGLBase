#ifndef moduleCommand_h
#define moduleCommand_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryDoubleList.h"

#include "command.h"


#define MODULE_COMMAND
#define MODULE_COMMAND_SETUP_FAIL 1

#define MODULE_COMMAND_ELEMENT_SIZE sizeof(commandTokenized)

#ifndef MEMORY_MODULE_NUM_COMMANDS
	#define MEMORY_MODULE_NUM_COMMANDS 1
#endif

#define MODULE_COMMAND_MANAGER_SIZE memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_COMMANDS, MODULE_COMMAND_ELEMENT_SIZE)


return_t moduleCommandSetup();
void moduleCommandCleanup();

commandTokenized *moduleCmdTokAlloc();
commandTokenized *moduleCmdTokPrepend(commandTokenized **const restrict start);
commandTokenized *moduleCmdTokAppend(commandTokenized **const restrict start);
commandTokenized *moduleCmdTokInsertBefore(commandTokenized **const restrict start, commandTokenized *const restrict next);
commandTokenized *moduleCmdTokInsertAfter(commandTokenized **const restrict start, commandTokenized *const restrict prev);
commandTokenized *moduleCmdTokNext(const commandTokenized *const restrict cmdTok);
commandTokenized *moduleCmdTokPrev(const commandTokenized *const restrict cmdTok);
void moduleCmdTokFree(commandTokenized **const restrict start, commandTokenized *const restrict cmdTok);
void moduleCmdTokFreeArray(commandTokenized **const restrict start);
void moduleCmdTokClear();


extern memoryDoubleList g_cmdTokManager;


#endif