#include "inputManager.h"


#include <SDL2/SDL.h>

#include "memoryManager.h"


// Forward-declare any helper functions!
static void inputBindingDelete(inputBinding *const restrict keybind);


void inputMngrInit(inputManager *const restrict inputMngr){
	memset(inputMngr, 0, sizeof(*inputMngr));
}


void inputMngrBind(inputManager *const restrict inputMngr, const int id, const char *const binding, const size_t bindingLength){
	inputBinding *const keybind = &inputMngr->keybinds[id];

	keybind->id = id;
	// If the key has already been bound, we
	// should replace it with the new bind.
	if(keybind == NULL){
		keybind->binding = memoryManagerGlobalRealloc(keybind->binding, bindingLength * sizeof(*binding));
	}else{
		keybind->binding = memoryManagerGlobalAlloc(bindingLength * sizeof(*binding));
	}
	if(keybind->binding == NULL){
		/** REALLOC FAILED **/
		/** MALLOC FAILED **/
	}
	memcpy(keybind->binding, binding, bindingLength * sizeof(*binding));
	keybind->bindingLength = bindingLength;
}

void inputMngrUnbind(inputManager *const restrict inputMngr, const int id){
	inputBindingDelete(&inputMngr->keybinds[id]);
}


void inputMngrTakeInput(inputManager *const restrict inputMngr, commandBuffer *const restrict cmdBuffer){
	inputBinding *keybind;

	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			// A keyboard key has been pressed.
			case SDL_KEYDOWN:
				keybind = &inputMngr->keybinds[INPUT_NUM_MOUSE_BUTTONS + event.key.keysym.scancode];
				// Make sure the key was only just pressed
				// this tick and it has a valid binding.
				if(!event.key.repeat && keybind->binding != NULL){
					cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.key.timestamp, 0);
				}
			break;

			// A keyboard key has been released.
			case SDL_KEYUP:
				keybind = &inputMngr->keybinds[INPUT_NUM_MOUSE_BUTTONS + event.key.keysym.scancode];
				// Make sure the key was only just released
				// this tick and it has a valid binding.
				if(!event.key.repeat && keybind->binding != NULL){
					// If the command begins with a '+', we should execute its '-' pair.
					if(keybind->binding[0] == '+'){
						keybind->binding[0] = '-';
						cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.key.timestamp, 0);
						keybind->binding[0] = '+';
					}
				}
			break;

			case SDL_MOUSEMOTION:
				// The cursor was moved.
				inputMngr->mx += event.motion.xrel;
				inputMngr->my += event.motion.yrel;
			break;

			// A mouse button has been pressed.
			case SDL_MOUSEBUTTONDOWN:
				keybind = &inputMngr->keybinds[event.button.button - 1];
				// Make sure the button has a valid binding.
				if(keybind->binding != NULL){
					cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.button.timestamp, 0);
				}
			break;

			// A mouse button has been released.
			case SDL_MOUSEBUTTONUP:
				keybind = &inputMngr->keybinds[event.button.button - 1];
				// Make sure the button has a valid binding.
				if(keybind->binding != NULL){
					// If the command begins with a '+', we should execute its '-' pair.
					if(keybind->binding[0] == '+'){
						keybind->binding[0] = '-';
						cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.button.timestamp, 0);
						keybind->binding[0] = '+';
					}
				}
			break;

			// The mouse wheel was scrolled.
			case SDL_MOUSEWHEEL:
				if(event.wheel.y < 0){
					keybind = &inputMngr->keybinds[INPUT_MWHEELDOWN];
				}else if(event.wheel.y > 0){
					keybind = &inputMngr->keybinds[INPUT_MWHEELUP];
				}else{
					break;
				}
				// Make sure the button has a valid binding.
				if(keybind->binding != NULL){
					// When the mouse wheel is scrolled, we should
					// perform both the press and release events.
					cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.wheel.timestamp, 0);
					// If the command begins with a '+', execute its '-' pair.
					if(keybind->binding[0] == '+'){
						keybind->binding[0] = '-';
						cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.wheel.timestamp, 0);
						keybind->binding[0] = '+';
					}
				}
			break;

		}
	}
}

void inputMngrResetMouseDeltas(inputManager *const restrict inputMngr, int *const mx, int *const my){
	*mx = inputMngr->mx;
	*my = inputMngr->my;
	inputMngr->mx = 0;
	inputMngr->my = 0;
}


void inputMngrDelete(inputManager *const restrict inputMngr){
	inputBinding *curBind = inputMngr->keybinds;
	const inputBinding *lastBind = &curBind[INPUT_NUM_BUTTONS];
	// Delete all of the keyboard and mouse bindings.
	for(; curBind < lastBind; ++curBind){
		inputBindingDelete(curBind);
	}
}


static void inputBindingDelete(inputBinding *const restrict keybind){
	if(keybind->binding != NULL){
		memoryManagerGlobalFree(keybind->binding);
	}
	memset(keybind, 0, sizeof(*keybind));
}