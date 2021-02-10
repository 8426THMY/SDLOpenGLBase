#ifndef command_h
#define command_h


#include <stddef.h>
#include <stdint.h>

#include "utilTypes.h"


// These settings mirror the Source engine.
#define COMMAND_MAX_LENGTH 256
#define COMMAND_MAX_BUFFER_LENGTH 8192
#define COMMAND_MAX_ARGUMENTS 64


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


typedef size_t commandNodeIndex_t;
typedef uintptr_t command;
typedef uint_least8_t cmdTimestamp_t;

/*
** Console commands/variables and aliases are all stored in a trie.
** We store commands as generic void pointers, as they can be either
** a function pointer for commands/variables or a string pointer for
** aliases. If the node stores an alias, the least significant bit
** of the command pointer will be set to '1'.
*/
typedef struct commandNode commandNode;
typedef struct commandNode {
	char value;
	commandNode *children;
	commandNodeIndex_t numChildren;
	// This will be a NULL pointer if
	// no command ends at this node.
	command cmd;
} commandNode, commandSystem;

/*
** Command functions take in a tokenized
** array of strings as their arguments.
*/
typedef void (*commandFunction)(
	commandSystem *const restrict cmdSys,
	const size_t argc, const char **const restrict argv
);


//
typedef struct commandTokenized {
	// Stores pointers to first the command and then its
	// arguments inside a command buffer's argument buffer.
	const char *argv[COMMAND_MAX_ARGUMENTS];
	size_t argc;

	// Time at which the command was sent to the buffer.
	cmdTimestamp_t timestamp;
	// Number of ticks to wait before executing the command.
	cmdTimestamp_t delay;
} commandTokenized;

//
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
return_t cmdSysAdd(commandSystem *node, const char *restrict name, const command cmd);
const command cmdSysFind(const commandSystem *node, const char *restrict name);
void cmdSysDelete(commandSystem *const restrict cmdSys);

void cmdBufferInit(commandBuffer *const restrict cmdBuf);
void cmdBufferAddCommand(
	commandBuffer *const restrict cmdBuffer,
	const char *str, const size_t strLength,
	const cmdTimestamp_t timestamp, const cmdTimestamp_t delay
);
void cmdBufferExecute(commandBuffer *const restrict cmdBuf, commandSystem *const restrict cmdSys);
void cmdBufferDelete(commandBuffer *const restrict cmdBuf);


#endif