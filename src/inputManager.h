#ifndef inputManager_h
#define inputManager_h


#include <string.h>

#include "utilTypes.h"
#include "command.h"


#define INPUT_NUM_KEYBOARD_KEYS 256
#define INPUT_NUM_MOUSE_BUTTONS 7
#define INPUT_NUM_BUTTONS       (INPUT_NUM_MOUSE_BUTTONS + INPUT_NUM_KEYBOARD_KEYS)

#define INPUT_MBUTTON0   0
#define INPUT_MBUTTON1   1
#define INPUT_MBUTTON2   2
#define INPUT_MBUTTON3   3
#define INPUT_MBUTTON4   4
#define INPUT_MWHEELDOWN 5
#define INPUT_MWHEELUP   6


typedef struct inputBinding {
	// Identifier for the button
	// this binding applies to.
	int id;
	char *binding;
	size_t bindingLength;
} inputBinding;

// Stores and manages the user's key bindings.
typedef struct inputManager {
	// Array of keyboard bindings.
	inputBinding keyboardBinds[INPUT_NUM_KEYBOARD_KEYS];
	// Array of mouse bindings.
	inputBinding mouseBinds[INPUT_NUM_MOUSE_BUTTONS];
} inputManager;


void inputMngrInit(inputManager *const restrict inputMngr);

void inputMngrKeyboardBind(inputManager *const restrict inputMngr, const int id, const char *const binding, const size_t bindingLength);
void inputMngrMouseBind(inputManager *const restrict inputMngr, const int id, const char *const binding, const size_t bindingLength);
void inputMngrKeyboardUnbind(inputManager *const restrict inputMngr, const int id);
void inputMngrMouseUnbind(inputManager *const restrict inputMngr, const int id);

void inputMngrTakeInput(inputManager *const restrict inputMngr, commandBuffer *const restrict cmdBuffer);
void inputMngrResetMouseDeltas(inputManager *const restrict inputMngr, int *const mx, int *const my);

void inputMngrDelete(inputManager *const restrict inputMngr);


#endif