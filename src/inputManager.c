#include "inputManager.h"


#include <SDL2/SDL.h>

#include <stdio.h>

#include "cvars.h"
#include "memoryManager.h"


#ifdef C_MOUSEMOVE_FAST
	// Must be large enough for "mousemove x y", where
	// x and y are guaranteed to be exactly 5 bytes.
	#define MOTION_CMD_BUFFER_MAX_LENGTH 22
#else
	// Must be large enough for "mousemove x y", where x and y
	// can be no more than 5 digits (or 6 counting the sign).
	#define MOTION_CMD_BUFFER_MAX_LENGTH 24
#endif


// Forward-declare any helper functions!
#ifdef C_MOUSEMOVE_FAST
static void mouseMotionIntToStr(const int i, char *str);
#endif
static void mouseMotionAddCommand(
	commandBuffer *const restrict cmdBuffer,
	int *const restrict mouseDeltaX, int *const restrict mouseDeltaY,
	const cmdTimestamp mouseDeltaTime
);

static void inputBindingDelete(inputBinding *const restrict keybind);


// Initialize each keybind.
void inputMngrInit(inputManager *const restrict inputMngr){
	inputBinding *curBind = inputMngr->keyboardBinds;
	const inputBinding *lastBind = &curBind[INPUT_NUM_KEYBOARD_KEYS];
	int id = 0;
	// Delete all of the keyboard bindings.
	for(; curBind < lastBind; ++curBind, ++id){
		curBind->id = id;
		curBind->binding = NULL;
	}

	curBind = inputMngr->mouseBinds;
	lastBind = &curBind[INPUT_NUM_MOUSE_BUTTONS];
	id = 0;
	// Delete all of the mouse bindings.
	for(; curBind < lastBind; ++curBind, ++id){
		curBind->id = id;
		curBind->binding = NULL;
	}

	inputMngr->keyStates = SDL_GetKeyboardState(NULL);
}


void inputMngrKeyboardBind(inputManager *const restrict inputMngr, const int id, const char *const binding, const size_t bindingLength){
	inputBinding *const keybind = &inputMngr->keyboardBinds[id];

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

void inputMngrMouseBind(inputManager *const restrict inputMngr, const int id, const char *const binding, const size_t bindingLength){
	inputBinding *const keybind = &inputMngr->mouseBinds[id];

	// If the button has already been bound, we
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

void inputMngrKeyboardUnbind(inputManager *const restrict inputMngr, const int id){
	inputBindingDelete(&inputMngr->keyboardBinds[id]);
}

void inputMngrMouseUnbind(inputManager *const restrict inputMngr, const int id){
	inputBindingDelete(&inputMngr->mouseBinds[id]);
}


void inputMngrTakeInput(inputManager *const restrict inputMngr, commandBuffer *const restrict cmdBuffer){
	inputBinding *keybind;

	// Used for tracking mouse movement between commands.
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	cmdTimestamp mouseDeltaTime = 0;

	SDL_Event event;

	// Make sure the real mouse deltas are
	// reset in case the mouse was not moved.
	cv_mouse_dx = 0;
	cv_mouse_dy = 0;

	#warning "I think this can cause microstutters. If it gets bad, consider using 'SDL_PeepEvents' so we aren't constantly calling 'SDL_PumpEvents'."
	while(SDL_PollEvent(&event)){
		switch(event.type){
			// Ideally, we shouldn't have to handle this here.
			case SDL_QUIT:
				cv_prg_running = 0;
			break;

			// A keyboard key has been pressed.
			case SDL_KEYDOWN:
				keybind = &inputMngr->keyboardBinds[event.key.keysym.scancode];
				// Make sure the key was only just pressed
				// this tick and it has a valid binding.
				if(!event.key.repeat && keybind->binding != NULL){
					mouseMotionAddCommand(cmdBuffer, &mouseDeltaX, &mouseDeltaY, mouseDeltaTime);
					cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.key.timestamp, 0);
				}
			break;

			// A keyboard key has been released.
			case SDL_KEYUP:
				keybind = &inputMngr->keyboardBinds[event.key.keysym.scancode];
				// Make sure the key was only just released
				// this tick and it has a valid binding.
				if(!event.key.repeat && keybind->binding != NULL){
					// If the command begins with a '+', we should execute its '-' pair.
					if(keybind->binding[0] == '+'){
						mouseMotionAddCommand(cmdBuffer, &mouseDeltaX, &mouseDeltaY, mouseDeltaTime);
						keybind->binding[0] = '-';
						cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.key.timestamp, 0);
						keybind->binding[0] = '+';
					}
				}
			break;

			// Every time the cursor is moved, we need to update the deltas.
			// The total movement gets added to the buffer when a different
			// command is issued, and these deltas get reset. This lets us
			// handle mouse movement between other keypresses.
			///#error "Can we do mouse input correctly (that is, use ordered mouse input and commands)? I've forgotten."
			case SDL_MOUSEMOTION:
				mouseDeltaX += event.motion.xrel;
				mouseDeltaY += event.motion.yrel;
				mouseDeltaTime = event.motion.timestamp;
			break;

			// A mouse button has been pressed.
			case SDL_MOUSEBUTTONDOWN:
				keybind = &inputMngr->mouseBinds[event.button.button - 1];
				// Make sure the button has a valid binding.
				if(keybind->binding != NULL){
					mouseMotionAddCommand(cmdBuffer, &mouseDeltaX, &mouseDeltaY, mouseDeltaTime);
					cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.button.timestamp, 0);
				}
			break;

			// A mouse button has been released.
			case SDL_MOUSEBUTTONUP:
				keybind = &inputMngr->mouseBinds[event.button.button - 1];
				// Make sure the button has a valid binding.
				if(keybind->binding != NULL){
					// If the command begins with a '+', we should execute its '-' pair.
					if(keybind->binding[0] == '+'){
						mouseMotionAddCommand(cmdBuffer, &mouseDeltaX, &mouseDeltaY, mouseDeltaTime);
						keybind->binding[0] = '-';
						cmdBufferAddCommand(cmdBuffer, keybind->binding, keybind->bindingLength, event.button.timestamp, 0);
						keybind->binding[0] = '+';
					}
				}
			break;

			// The mouse wheel was scrolled.
			case SDL_MOUSEWHEEL:
				if(event.wheel.y < 0){
					keybind = &inputMngr->mouseBinds[INPUT_MWHEELDOWN];
				}else if(event.wheel.y > 0){
					keybind = &inputMngr->mouseBinds[INPUT_MWHEELUP];
				}else{
					break;
				}
				// Make sure the button has a valid binding.
				if(keybind->binding != NULL){
					mouseMotionAddCommand(cmdBuffer, &mouseDeltaX, &mouseDeltaY, mouseDeltaTime);
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

	// Handle any trailing mouse movement.
	mouseMotionAddCommand(cmdBuffer, &mouseDeltaX, &mouseDeltaY, mouseDeltaTime);
}

/*
** We take mouse input every time we render. However, rather
** than adding the mouse movement commands to the buffer to
** execute later, we can just execute the command here.
*/
void inputMngrUpdateMouseDeltas(inputManager *const restrict inputMngr, commandBuffer *const restrict cmdBuffer){
	cv_mouse_dx = 0;
	cv_mouse_dy = 0;

	SDL_Event event;
	#warning "I think this can cause microstutters."
	while(SDL_PumpEvents(), SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION)){
		cv_mouse_dx += event.motion.xrel;
		cv_mouse_dy += event.motion.yrel;
	}
}


void inputMngrDelete(inputManager *const restrict inputMngr){
	inputBinding *curBind = inputMngr->keyboardBinds;
	const inputBinding *lastBind = &curBind[INPUT_NUM_KEYBOARD_KEYS];
	// Delete all of the keyboard bindings.
	for(; curBind < lastBind; ++curBind){
		inputBindingDelete(curBind);
	}

	curBind = inputMngr->mouseBinds;
	lastBind = &curBind[INPUT_NUM_MOUSE_BUTTONS];
	// Delete all of the mouse bindings.
	for(; curBind < lastBind; ++curBind){
		inputBindingDelete(curBind);
	}
}


#ifdef C_MOUSEMOVE_FAST
/*
** Convert a signed integer to a special string
** format that the command system can safely use.
**
** We use a flag byte followed by 4 bytes for the actual integer value.
** If any of these 4 bytes have an individual value less than 60, we
** need to add 60 to them and make a note of the change in the flag byte.
**
** We choose 60 as any values larger are guaranteed
** not to be special command system characters.
*/
static void mouseMotionIntToStr(const int i, char *str){
	unsigned int curIndex = 1;
	char *flagChar = str;

	++str;
	// Set the flag byte's initial value to 60.
	*flagChar = COMMAND_SPECIAL_CHAR_LIMIT;

	memcpy(str, &i, sizeof(i));
	// Process each byte of the original integer.
	for(; curIndex < (1 << sizeof(i)); curIndex <<= 1, ++str){
		// If the byte's individual value is less than 60,
		// it may contain a special character, so add 60 to it.
		if((unsigned char)*str < COMMAND_SPECIAL_CHAR_LIMIT){
			*str += COMMAND_SPECIAL_CHAR_LIMIT;
			// We need to keep track of this change in the flag byte.
			// Just add 2^{n-1} to it, where n is the index of the modified byte.
			*flagChar += curIndex;
		}
	}
}
#endif

/*
** If the mouse has been moved, add an extra command to the buffer.
** We assume that "mouseDeltaX" and "mouseDeltaY" are no more than
** 6 digits (including sign). Luckily, this should never happen.
*/
static void mouseMotionAddCommand(
	commandBuffer *const restrict cmdBuffer,
	int *const restrict mouseDeltaX, int *const restrict mouseDeltaY,
	const cmdTimestamp mouseDeltaTime
){

	// Only send a command if the mouse has moved.
	if(*mouseDeltaX | *mouseDeltaY){
		#ifdef C_MOUSEMOVE_FAST
		char motionCommand[MOTION_CMD_BUFFER_MAX_LENGTH];

		// Construct the command string.
		memcpy(&motionCommand[0], "mousemove ", sizeof("mousemove "));
		mouseMotionIntToStr(*mouseDeltaX, &motionCommand[10]);
		motionCommand[15] = ' ';
		mouseMotionIntToStr(*mouseDeltaY, &motionCommand[16]);
		motionCommand[21] = '\0';
		// Add the command to the command buffer.
		cmdBufferAddCommand(cmdBuffer, motionCommand, MOTION_CMD_BUFFER_MAX_LENGTH, mouseDeltaTime, 0);
		#else
		char motionCommand[MOTION_CMD_BUFFER_MAX_LENGTH];
		size_t motionCommandLength = sizeof("mousemove ") - 1;

		// Construct the command string.
		memcpy(&motionCommand[0], "mousemove ", motionCommandLength);
		motionCommandLength += sprintf(&motionCommand[motionCommandLength], "%i %i", *mouseDeltaX, *mouseDeltaY);
		// Add the command to the command buffer.
		cmdBufferAddCommand(cmdBuffer, motionCommand, motionCommandLength, mouseDeltaTime, 0);
		#endif

		// Reset the mouse deltas.
		*mouseDeltaX = 0;
		*mouseDeltaY = 0;
	}
}


static void inputBindingDelete(inputBinding *const restrict keybind){
	if(keybind->binding != NULL){
		memoryManagerGlobalFree(keybind->binding);
		keybind->binding = NULL;
	}
}