#include "command.h"


#include <ctype.h>

#include "memoryManager.h"


static void cmdNodeInit(commandNode *const restrict cmdNode, const char value);
static return_t cmdNodeValid(const char *const restrict name, commandFunction func);
static commandNodeIndex_t cmdNodeNext(commandNode **const restrict node, const char c);
static void cmdNodeAddChild(commandNode **const restrict node, const char c);
static void cmdNodeRemoveChild(commandNode *const node, const commandNodeIndex_t index);


void cmdSysInit(commandSystem *const restrict cmdSys){
	cmdSys->children = NULL;
	cmdSys->numChildren = 0;
}

return_t cmdSysAdd(commandSystem *node, const char *restrict name, const commandFunction func){
	if(cmdNodeValid(name, func)){
		char curChar;
		// Go through each character in the command name,
		// adding any nodes that don't already exist.
		//
		// Command names are not case sensitive,
		// so we store them in lower case.
		for(curChar = tolower(*name); curChar != '\0'; ++name, curChar = tolower(*name)){
			cmdNodeAddChild(&node, curChar);
		}

		// Don't overwrite the node if it's already in use.
		if(node->func == NULL){
			node->func = func;
			return(1);
		}
	}

	return(0);
}

void cmdSysRemove(commandSystem *node, const char *restrict name){
	commandNodeIndex_t curIndex = 0;
	commandNodeIndex_t lastIndex = 0;
	commandNode *lastNode = node;
	// Search down the trie until we find the node we want to remove.
	while(!valueIsInvalid(curIndex = cmdNodeNext(&node, *name), commandNodeIndex_t)){
		// On the way, record the deepest node with more than one child.
		// This will tell us where we should begin when deleting now-unused nodes.
		if(node->numChildren > 1){
			lastIndex = curIndex;
			lastNode = node;
		}
		++name;
	}

	// If the node we're removing is the last in a branch, remove
	// it and all the ones before it that aren't otherwise in use.
	if(node->numChildren == 0){
		node = lastNode->children[lastIndex].children;
		// Every node below "node" will only have at most one child.
		while(node != NULL){
			commandNode *const nextNode = node->children;
			memoryManagerGlobalFree(node);
			node = nextNode;
		}
		// Note that if the trie had more than one command listed in it,
		// "lastNode" is guaranteed to have more than one child, so we
		// need to shift its children to the left to cover the hole.
		cmdNodeRemoveChild(lastNode, lastIndex);
	}
}

const commandFunction cmdSysFind(commandSystem *node, const char *restrict name){
	// Search the trie for each character of the command name. This will stop
	// when we can no longer continue or we start looking for the NUL terminator.
	while(!valueIsInvalid(cmdNodeNext(&node, *name), commandNodeIndex_t)){
		++name;
	}
	// If a valid ending node could not be found, this will be a NULL pointer.
	// As long as the root node is valid, this one should always be valid too.
	return(node->func);
}


static void cmdNodeInit(commandNode *const restrict cmdNode, const char value){
	cmdNode->value = value;
	cmdNode->children = NULL;
	cmdNode->numChildren = 0;
	cmdNode->func = NULL;
}

// Check if the name of a command node we want to add is valid.
static return_t cmdNodeValid(const char *const restrict name, commandFunction cmd){
	const char *check = name;

	if(check != NULL && cmd != NULL){
		char curChar;
		for(curChar = *check; curChar != '\0'; ++check, curChar = *check){
			// Check for any invalid characters.
			if(!isalnum(curChar) && curChar != '_'){
				return(0);
			}
		}
	}

	// This is only false if "name" is an empty string.
	return(name != check);
}

/*
** Find the child of "node" whose value is 'c', and return its index.
** If the child could be found, make "node" point to it too.
*/
static commandNodeIndex_t cmdNodeNext(commandNode **const restrict node, const char c){
	commandNodeIndex_t curIndex = 0;
	const commandNodeIndex_t numChildren = (*node)->numChildren;
	commandNode *curChild = (*node)->children;
	for(; curIndex < numChildren; ++curIndex, ++curChild){
		// If this is the child we seek, return it.
		if(c == curChild->value){
			*node = curChild;
			return(curIndex);

		// We store nodes in alphanumeric order, so if we
		// haven't found the right node yet, it must not exist.
		}else if(c > curChild->value){
			break;
		}
	}

	return(valueInvalid(commandNodeIndex_t));
}

static void cmdNodeAddChild(commandNode **const restrict node, const char c){
	commandNode *const curNode = *node;

	commandNodeIndex_t curIndex = 0;
	const commandNodeIndex_t numChildren = curNode->numChildren;
	commandNode *curChild = curNode->children;
	for(; curIndex < numChildren; ++curIndex, ++curChild){
		// If this is the child we seek, return it.
		if(c == curChild->value){
			*node = curChild;
			return;

		// If 'c' is larger than the current child's value,
		// we must insert a new child before this one.
		}else if(c > curChild->value){
			break;
		}
	}

	// If we've found the next child or we've reached
	// the end of the array, insert a new child node.
	{
		commandNode *const tempBuffer = memoryManagerGlobalRealloc(
			curNode->children, ++curNode->numChildren*sizeof(commandNode)
		);
		if(tempBuffer == NULL){
			/** REALLOC FAILED **/
		}
		curNode->children = tempBuffer;

		// The new node should be inserted before the current child.
		curChild = &tempBuffer[curIndex];
		// We want the nodes to be in alphanumeric order,
		// so we'll need to shift everything after and
		// including the current child to the right.
		if(curIndex < numChildren){
			memmove(curChild + 1, curChild, numChildren*sizeof(commandNode));
		}
		cmdNodeInit(curChild, c);
		*node = curChild;
	}
}

static void cmdNodeRemoveChild(commandNode *const restrict node, const commandNodeIndex_t index){
	--node->numChildren;

	// This should only be the case when the trie is now empty.
	if(node->numChildren == 0){
		memoryManagerGlobalFree(node->children);
		node->children = NULL;

	// Otherwise, we should shift its remaining children over to fill the hole.
	}else{
		if(index < node->numChildren){
			memmove(
				&node->children[index] + 1,
				&node->children[index],
				node->numChildren*sizeof(commandNode)
			);
		}
		{
			commandNode *const tempBuffer = memoryManagerGlobalRealloc(
				node->children, node->numChildren*sizeof(commandNode)
			);
			if(tempBuffer == NULL){
				/** REALLOC FAILED **/
			}
			node->children = tempBuffer;
		}
	}
}