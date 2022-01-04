#include "command.h"


#include <ctype.h>

#include "utilMemory.h"
#include "memoryManager.h"
#include "moduleCommand.h"


#define cmdValidCharacter(c)      ((c) == '+' || (c) == '-' || (c) == '_' || isalpha(c))
#define cmdWhitespaceCharacter(c) ((c) == ' ' || (c) == '\t')


// Forward-declare any helper functions!
static return_t cmdSysAdd(
	commandSystem *restrict node,
	const char *restrict name, const command cmd, const flags_t type
);

static void cmdNodeInit(commandNode *const restrict cmdNode, const char value);
static return_t cmdNameValid(const char *const restrict name);
static commandNode *cmdNodeNext(const commandNode *const node, const char c);
static commandNode *cmdNodeAddChild(commandNode *const node, const char c);

static void cmdBufferAddArgument(
	commandBuffer *const restrict cmdBuffer,
	commandTokenized *const restrict cmdTok, const char *const str
);
static const char *cmdBufferTokenizeCommand(
	commandBuffer *const restrict cmdBuffer,
	commandTokenized *const restrict cmdTok,
	const char *str, const char *const strEnd
);


void cmdSysInit(commandSystem *const restrict cmdSys){
	cmdSys->value = '\0';
	cmdSys->children = NULL;
	cmdSys->numChildren = 0;
	cmdSys->type = COMMAND_TYPE_FREE;
}

/*
** Add a function to a command system and
** return whether or not we were successful!
*/
return_t cmdSysAddFunction(
	commandSystem *const restrict cmdSys,
	const char *const restrict name, const commandFunction func
){

	if(cmdSys != NULL && func != NULL){
		const command cmd = {.func = func};
		return(cmdSysAdd(cmdSys, name, cmd, COMMAND_TYPE_FUNCTION));
	}
	return(0);
}

/*
** Add a variable to a command system and
** return whether or not we were successful!
*/
return_t cmdSysAddVariable(
	commandSystem *const restrict cmdSys,
	const char *const restrict name, const commandVariable var
){

	if(cmdSys != NULL && var != NULL){
		const command cmd = {.var = var};
		return(cmdSysAdd(cmdSys, name, cmd, COMMAND_TYPE_VARIABLE));
	}
	return(0);
}

commandNode *cmdSysFind(commandSystem *restrict node, const char *restrict name){
	// Search down the trie for each character of the command's name.
	for(; node != NULL; ++name){
		const char c = tolower(*name);
		if(c == '\0'){
			return((commandNode *)node);
		}
		node = cmdNodeNext(node, c);
	}

	return(NULL);
}

/*
** Remove a command from a command system and
** return whether or not we were successful!
**
** Note that we keep the command's node in the
** trie, even if it's a leaf node, just in case
** we want to add it back later.
*/
return_t cmdSysRemove(commandSystem *restrict node, const char *const restrict name){
	node = cmdSysFind(node, name);
	// If a node with the name specified was found, free it.
	if(node != NULL){
		// If the node stores a string buffer, make sure we free it too.
		if(node->type == COMMAND_TYPE_VARIABLE){
			memoryManagerGlobalFree(node->cmd.var);
		}
		node->type = COMMAND_TYPE_FREE;

		return(1);
	}

	return(0);
}

void cmdSysDelete(commandSystem *const restrict node){
	commandNode *curChild = node->children;
	// Recursively delete the node's children if it has any.
	if(curChild != NULL){
		const commandNode *const lastChild = &curChild[node->numChildren];
		for(; curChild < lastChild; ++curChild){
			cmdSysDelete(curChild);
		}
		memoryManagerGlobalFree(node->children);
	}
	// If the node stores a string buffer, make sure we free it too.
	if(node->type == COMMAND_TYPE_VARIABLE){
		memoryManagerGlobalFree(node->cmd.var);
	}
}


void cmdBufferInit(commandBuffer *const restrict cmdBuffer){
	cmdBuffer->argBufferLength = 0;
	cmdBuffer->cmdList = NULL;
	cmdBuffer->cmdListEnd = NULL;
}

/* Add a command string to a command buffer!
**
** This function may be called while actually executing
** a command buffer, particularly it contains an alias.
*/
void cmdBufferAddCommand(
	commandBuffer *const restrict cmdBuffer,
	const char *str, const size_t strLength,
	const cmdTimestamp_t timestamp, const cmdTimestamp_t delay
){

	const char *const strEnd = &str[strLength];
	commandTokenized cmdTok;

	cmdTok.timestamp = timestamp;
	// When we're executing a command string containing an alias,
	// we need to add the alias' delay to every command in the string.
	cmdTok.delay = delay;

	// Tokenize each command in the string
	// and add them to the command buffer.
	while(str != strEnd){
		// Reset "argc" in preparation for the next command.
		// We keep the timestamp as it shouldn't be changing,
		// and any new delay needs to affect later commands.
		cmdTok.argc = 0;
		// Tokenize the arguments for the next command in the command string.
		str = cmdBufferTokenizeCommand(cmdBuffer, &cmdTok, str, strEnd);

		if(cmdTok.argc > 0){
			// If there was a "wait" command, we should handle it manually.
			if(strncmp(cmdTok.argv[0], "wait", 4) == 0){
				if(cmdTok.argc > 1){
					cmdTok.delay += strtoul(cmdTok.argv[1], NULL, 0);
				}else{
					++cmdTok.delay;
				}

			// Otherwise, insert the new tokenized
			// command into the command buffer's list.
			}else{
				commandTokenized *cmdInsert = cmdBuffer->cmdListEnd;
				commandTokenized *cmdNew;
				// Commands are sorted by their timestamps, so the new command
				// should be inserted after the first one whose timestamp is lesser.
				// Note that we start looping from the end of the command buffer.
				while(cmdInsert != NULL && timestamp < cmdInsert->timestamp){
					cmdInsert = moduleCmdTokPrev(cmdInsert);
				}

				// If "cmdInsert" is NULL and the list is not empty, then we've
				// looped all the way back to the beginning. Although this case
				// is now handled by "memoryDoubleListInsertAfter()", we handle
				// it here just to clearly acknowledge that this case can arise.
				if(cmdInsert == NULL && cmdBuffer->cmdList != NULL){
					cmdNew = moduleCmdTokPrepend(&cmdBuffer->cmdList);

				// Otherwise, just insert the new command into the list normally.
				}else{
					cmdNew = moduleCmdTokInsertAfter(&cmdBuffer->cmdList, cmdInsert);
				}
				if(cmdNew == NULL){
					/** MALLOC FAILED **/
				}
				*cmdNew = cmdTok;

				// If we inserted the new command at the end of
				// the list, we need to update the end pointer.
				if(cmdInsert == cmdBuffer->cmdListEnd){
					cmdBuffer->cmdListEnd = cmdNew;
				}
			}
		}
	}
}

// Execute the tokenized commands stored by a command buffer.
void cmdBufferExecute(commandBuffer *const restrict cmdBuffer, commandSystem *const restrict cmdSys){
	// We may need to update the length of the argument
	// buffer or the end of the tokenized command list.
	// This generally only happens if we have aliases.
	size_t argBufferLength = 0;
	commandTokenized *cmdListEnd = NULL;

	commandTokenized *cmdTok = cmdBuffer->cmdList;
	while(cmdTok != NULL){
		commandTokenized *const cmdTokNext = moduleCmdTokNext(cmdTok);

		// If the command is delayed, add it to the
		// front of the buffer for the next tick.
		if(cmdTok->delay > 0){
			const char **curArg = &cmdTok->argv[0];
			const char **lastArg = &curArg[cmdTok->argc - 1];
			const size_t cmdLength = *lastArg - *curArg + strlen(*lastArg);
			const size_t offset = *curArg - &cmdBuffer->argBuffer[argBufferLength];

			// Copy the command's arguments over.
			memmove(&cmdBuffer->argBuffer[argBufferLength], *curArg, cmdLength);
			// Fix up the pointers in the command's argument array.
			for(; curArg <= lastArg; ++curArg){
				*curArg -= offset;
			}
			--cmdTok->delay;

			argBufferLength += cmdLength;
			cmdListEnd = cmdTok;

		// Otherwise, we can try executing the command.
		}else{
			const commandNode *const node = cmdSysFind(cmdSys, cmdTok->argv[0]);
			// Note that "node" is only NULL if "cmdSys" is NULL.
			if(node != NULL){
				// Execute the command if it's a function.
				if(node->type == COMMAND_TYPE_FUNCTION){
					// The first argument stores the command's name, so we should skip it.
					node->cmd.func(cmdSys, cmdTok->argc - 1, &cmdTok->argv[1]);

				// If the command was a variable, we should tokenize
				// it and then insert it into the command buffer.
				}else if(node->type == COMMAND_TYPE_VARIABLE){
					// We temporarily set the end of the command list here
					// to make insertions faster. It will be fixed once we
					// finish executing the other commands in the buffer.
					cmdBuffer->cmdListEnd = cmdTok;
					cmdBufferAddCommand(
						cmdBuffer,
						node->cmd.var, strlen(node->cmd.var),
						cmdTok->timestamp, cmdTok->delay
					);
				}
			}

			// Remove the command from the list.
			moduleCmdTokFree(&cmdBuffer->cmdList, cmdTok);
		}

		cmdTok = cmdTokNext;
	}

	// The buffer should now only store commands that have
	// been delayed until the next tick, so update its length
	// and the pointer to the last command in the list.
	cmdBuffer->argBufferLength = argBufferLength;
	cmdBuffer->cmdListEnd = cmdListEnd;
}

void cmdBufferDelete(commandBuffer *const restrict cmdBuffer){
	commandTokenized *cmdTok = cmdBuffer->cmdList;
	while(cmdTok != NULL){
		commandTokenized *const cmdTokNext = moduleCmdTokNext(cmdTok);
		moduleCmdTokFree(&cmdBuffer->cmdList, cmdTok);
		cmdTok = cmdTokNext;
	}
}


// Add a new command to a command system!
return_t cmdSysAdd(
	commandSystem *restrict node,
	const char *restrict name, const command cmd, const flags_t type
){

	if(cmdNameValid(name)){
		char c = *name;
		// Go through each character in the command name,
		// adding any nodes that don't already exist.
		//
		// Command names are not case sensitive,
		// so we store them in lower case.
		do {
			if((node = cmdNodeAddChild(node, tolower(c))) == NULL){
				/** REALLOC FAILED **/
			}
			++name;
			c = *name;
		} while(c != '\0');

		// Only overwrite the node if it's not already in use.
		if(node->type == COMMAND_TYPE_FREE){
			node->cmd = cmd;
			node->type = type;
			return(1);
		}
	}

	return(0);
}


static void cmdNodeInit(commandNode *const restrict cmdNode, const char value){
	cmdNode->value = value;
	cmdNode->children = NULL;
	cmdNode->numChildren = 0;
	cmdNode->type = COMMAND_TYPE_FREE;
}

// Check if the name of a command node we want to add is valid.
static return_t cmdNameValid(const char *name){
	if(name == NULL || *name == '\0'){
		return(0);
	}else{
		char c = *name;
		// Check the command name for any invalid characters.
		do {
			if(cmdValidCharacter(c)){
				++name;
			}else{
				return(0);
			}
			c = *name;
		} while(c != '\0');

		return(1);
	}
}

// Return the child of "node" whose value is 'c'.
static commandNode *cmdNodeNext(const commandNode *const node, const char c){
	commandNode *curChild = node->children;
	const commandNode *const lastChild = &curChild[node->numChildren];
	// Iterate through the node's children
	// until we find one whose value is 'c'.
	for(; curChild < lastChild; ++curChild){
		if(c == curChild->value){
			return(curChild);
		}else if(c < curChild->value){
			break;
		}
	}

	return(NULL);
}

static commandNode *cmdNodeAddChild(commandNode *const node, const char c){
	commandNode *tempBuffer;

	commandNode *curChild = node->children;
	const commandNode *lastChild = &curChild[node->numChildren];
	// Iterate through the node's children
	// until we find one whose value is 'c'.
	for(; curChild < lastChild; ++curChild){
		if(c == curChild->value){
			return(curChild);

		// If 'c' is less than the current child's value,
		// our new node should be inserted before it.
		}else if(c < curChild->value){
			tempBuffer = memoryManagerGlobalRealloc(
				node->children, ++node->numChildren*sizeof(*tempBuffer)
			);
			if(tempBuffer == NULL){
				return(NULL);
			}

			curChild = memoryAddPointer(tempBuffer, memorySubPointer(curChild, node->children));
			lastChild = memoryAddPointer(tempBuffer, memorySubPointer(lastChild, node->children));
			memmove(curChild + 1, curChild, (uintptr_t)memorySubPointer(lastChild, curChild));
			node->children = tempBuffer;
			cmdNodeInit(curChild, c);

			return(curChild);
		}
	}

	// Insert the new child node at the end of the node's array!
	tempBuffer = memoryManagerGlobalRealloc(
		node->children, ++node->numChildren*sizeof(*tempBuffer)
	);
	if(tempBuffer == NULL){
		return(NULL);
	}
	node->children = tempBuffer;
	curChild = &tempBuffer[node->numChildren - 1];
	cmdNodeInit(curChild, c);

	return(curChild);
}


static void cmdBufferAddArgument(
	commandBuffer *const restrict cmdBuffer,
	commandTokenized *const restrict cmdTok, const char *const str
){

	size_t argLength = str - cmdTok->argv[cmdTok->argc];
	// Clamp the argument if it's too long to fit in the buffer.
	if(cmdBuffer->argBufferLength + argLength + 1 > COMMAND_MAX_BUFFER_LENGTH){
		argLength = COMMAND_MAX_BUFFER_LENGTH - cmdBuffer->argBufferLength - 1;
	}

	// Add the argument to the buffer.
	memcpy(&cmdBuffer->argBuffer[cmdBuffer->argBufferLength], cmdTok->argv[cmdTok->argc], argLength);
	cmdTok->argv[cmdTok->argc] = &cmdBuffer->argBuffer[cmdBuffer->argBufferLength];
	cmdBuffer->argBufferLength += argLength;
	++cmdTok->argc;

	// We also need to append a NULL terminator to the end of every argument.
	cmdBuffer->argBuffer[cmdBuffer->argBufferLength] = '\0';
	++(cmdBuffer->argBufferLength);
}

// Tokenize a single command and all of its arguments.
static const char *cmdBufferTokenizeCommand(
	commandBuffer *const restrict cmdBuffer,
	commandTokenized *const restrict cmdTok,
	const char *str, const char *const strEnd
){

	const size_t argBufferLengthOld = cmdBuffer->argBufferLength;
	unsigned int strQuoted = 0;
	unsigned int strCommented = 0;
	unsigned int strReading = 0;

	for(; str != strEnd; ++str){
		const char c = *str;

		// The next character has been escaped,
		// so we shouldn't treat it specially.
		if(c == '\\'){
			++str;
		}else if(!strCommented){
			const unsigned int strWhitespace = cmdWhitespaceCharacter(c);

			// If we've just finished reading an argument, add it to the buffer!
			if((!strQuoted && strWhitespace) || (strQuoted && c == '\"')){
				cmdBufferAddArgument(cmdBuffer, cmdTok, str);
				strQuoted = 0;
				strReading = 0;
			}else if(!strQuoted){
				// Check for C-style comments.
				if(c == '/'){
					++str;
					if(str == strEnd){
						break;
					}else if(*str == '/'){
						strCommented = 1;
					}else{
						continue;
					}

				// Commands are delimited by semicolons.
				}else if(c == ';'){
					break;

				// If we aren't currently reading an argument, start
				// reading one at the next character that isn't whitespace.
				}else if(!strReading && !strWhitespace){
					if(c == '\"'){
						cmdTok->argv[cmdTok->argc] = str+1;
						strQuoted = 1;
					}else{
						cmdTok->argv[cmdTok->argc] = str;
					}
					strReading = 1;
				}
			}

		// Read up until the end of the comment so we
		// know where the next command should start.
		}else if(c == '\n'){
			break;
		}
	}

	// Add the last argument to the buffer if we were reading one.
	// The command's argument vector has already been set up.
	if(strReading){
		cmdBufferAddArgument(cmdBuffer, cmdTok, str);
	}

	// Ignore the command if it was too long.
	if(cmdBuffer->argBufferLength - argBufferLengthOld > COMMAND_MAX_LENGTH){
		cmdBuffer->argBufferLength = argBufferLengthOld;
		cmdTok->argc = 0;
	}


	return(str);
}