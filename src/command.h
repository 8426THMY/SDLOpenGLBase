#ifndef command_h
#define command_h


#include <stddef.h>
#include <stdint.h>

#include "utilTypes.h"


// These settings mirror the Source engine.
#ifndef COMMAND_MAX_LENGTH
	#define COMMAND_MAX_LENGTH 256
#endif
#ifndef COMMAND_MAX_BUFFER_LENGTH
	#define COMMAND_MAX_BUFFER_LENGTH 8192
#endif
#ifndef COMMAND_MAX_ARGUMENTS
	#define COMMAND_MAX_ARGUMENTS 64
#endif

#define COMMAND_TYPE_FUNCTION 0x00
#define COMMAND_TYPE_VARIABLE 0x01
#define COMMAND_TYPE_FREE     0xFF

// Any character code less than this
// may be used by the command system.
#define COMMAND_SPECIAL_CHAR_LIMIT 60


/*
** Creating console variables:
**     - Console variables are represented by functions that set the variables' values.
**     - Variables should generally be defined in the ".c" file to prevent meddling from outside.
**
** Storing console functions/variables:
**     - Console functions and variables are stored in a trie for efficient lookup.
**     - Trie nodes that end on valid commands store a command pointer.
**
** Storing user variables and aliases:
**     - User variables are also stored in the same trie.
**     - Instead of storing a function pointer, they store a pointer to their string value.
**     - We use the last bit of the pointer to store whether the node represents a console function or user variable.
**
** Executing console functions:
**     - Given a semicolon-delimited command string, we tokenize each command and its arguments.
**     - The first argument for a command is always the command's name.
**     - We can then lookup the command name in the trie and execute the function, passing in the tokenized arguments.
*/


typedef size_t commandNodeIndex;

/*
** Command functions take in a tokenized
** array of strings as their arguments.
*/
typedef struct commandSystem commandSystem;
typedef void (*commandFunction)(
	commandSystem *const restrict cmdSys,
	const size_t argc, const char **const restrict argv
);
// User variables are stored as strings.
typedef char * commandVariable;
// The C standard does not enforce any sort of rules on the size of
// of data pointers versus function pointers, so the safest way to
// define a command is to use a union rather than any single type.
typedef union command {
	commandFunction func;
	commandVariable var;
} command;

/*
** Console commands/variables and aliases are all stored in a trie.
** We store commands as generic void pointers, as they can be either
** a function pointer for commands or a string pointer for aliases.
*/
typedef struct commandSystem {
	char value;
	commandSystem *children;
	commandNodeIndex numChildren;
	// This will be a NULL pointer if no command ends at this node.
	// We can't store the type of command in the least significant
	// bit of this pointer as we usually would, as it may already
	// be set for certain function pointers.
	command cmd;
	// Stores whether the command
	// is a function or a variable.
	flags8_t type;
} commandSystem, commandNode;


typedef uint_least8_t cmdTimestamp;

// Tokenized command.
typedef struct commandTokenized {
	// Stores pointers to first the command and then its
	// arguments inside a command buffer's argument buffer.
	const char *argv[COMMAND_MAX_ARGUMENTS];
	size_t argc;

	// Time at which the command was sent to the buffer.
	cmdTimestamp timestamp;
	// Number of ticks to wait before executing the command.
	cmdTimestamp delay;
} commandTokenized;

// Array of tokenized commands and their arguments.
typedef struct commandBuffer {
	// Stores a NULL terminated list of commands and their arguments.
	char argBuffer[COMMAND_MAX_BUFFER_LENGTH];
	size_t argBufferLength;

	// Single list of tokenized commands.
	commandTokenized *cmdList;
	// We'll usually be adding new commands to the end of
	// the list, so we may as well store a pointer to it.
	commandTokenized *cmdListEnd;
} commandBuffer;


void cmdSysInit(commandSystem *const restrict cmdSys);
return_t cmdSysAddFunction(
	commandSystem *const restrict cmdSys,
	const char *const restrict name, const commandFunction func
);
return_t cmdSysAddVariable(
	commandSystem *const restrict cmdSys,
	const char *const restrict name, const commandVariable var
);
commandNode *cmdSysFind(commandSystem *restrict cmdSys, const char *restrict name);
return_t cmdSysRemove(commandSystem *restrict cmdSys, const char *const restrict name);
void cmdSysDelete(commandSystem *const restrict cmdSys);

void cmdBufferInit(commandBuffer *const restrict cmdBuf);
void cmdBufferAddCommand(
	commandBuffer *const restrict cmdBuffer,
	const char *str, const size_t strLength,
	const cmdTimestamp timestamp, const cmdTimestamp delay
);
void cmdBufferExecute(commandBuffer *const restrict cmdBuf, commandSystem *const restrict cmdSys);
void cmdBufferDelete(commandBuffer *const restrict cmdBuf);


#endif