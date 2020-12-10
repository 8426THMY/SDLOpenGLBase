#ifndef command_h
#define command_h


#include <stdio.h>


/*
** Creating console variables:
**     - Console variables are represented by functions that set the variables' values.
**     - Variables should generally be defined in the ".c" file to prevent meddling from outside.
**
** Storing console functions/variables:
**     - Console functions and variables are stored in a trie for efficient lookup.
**     - Trie nodes that end on valid commands store a function pointer.
**
** Storing user variables:
**     - User variables are also stored in the same trie.
**     - Instead of storing a function pointer, they store a pointer to their string value.
**     - We use the last bit of the pointer to store whether the node represents a console function or user variable.
**
** Executing console functions:
**     - When one or more semicolon-delimited console commands are entered, we parse them one command string at a time.
**     - For each command string, we need to tokenize the function and its arguments.
**     - We then lookup the function in the trie and execute the function, passing in the tokenized arguments.
**
** Creating aliases:
**     - First, a user variable must be defined and set to the required command string.
**     - We then use a function similar to Quake 3's "vstr" command to execute the command from the user variable.
*/


/*
** Command arguments are stored as strings
** and parsed every time they're executed.
*/
typedef struct commandArgument {
	size_t length;
	char *value;
} commandArgument;

typedef void (*commandFunction)(const size_t argc, const commandArgument *const restrict argv);


typedef size_t commandNodeIndex_t;

// Registered commands are stored in a trie.
typedef struct commandNode commandNode;
typedef struct commandNode {
	commandNode *children;
	commandNodeIndex_t numChildren;
	// This will be a NULL pointer if
	// no command ends at this node.
	commandFunction func;
	char value;
} commandNode, commandTrie;


/**
 ** We might have to store aliases similarly to Quake/Source.
 ** They use a linked list of name and command string pairs,
 ** which might be better than using another trie as we probably
 ** won't have that many aliases existing simultaneously.
 **/


/**
 ** The main problem with storing command arguments as strings is that we have
 ** to reparse them every time we want to execute the command - this is most
 ** notably an issue with keybinds and aliases. The reason we use strings is
 ** because we have no way of knowing ahead of time how to actually parse the
 ** inputs for a particular command.
 **
 ** One solution might be to store the type to parse inputs as in the trie node.
 ** This would significantly improve efficiency for commands that don't have
 ** mixed types. Of course, commands with mixed types (like keybinds, the main
 ** reason for doing this) don't benefit from this, as they'll usually have to
 ** use strings.
 **
 ** The only place I can think of where this would matter is keybinds, so we
 ** could probably have a special keybind type that says to treat the first
 ** input as a string and the second as a command vector.
 **
 ** Now that I think about it, this probably wouldn't help. I'm not sure how
 ** to approach aliases at the moment, and that seems to be the only other
 ** case where the difference is significant.
 **/


#endif