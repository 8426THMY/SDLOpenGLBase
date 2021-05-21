#ifndef cvars_h
#define cvars_h


#include "command.h"
#include "utilTypes.h"


// Handle regular mouse motion events in an optimized way.
#define C_MOUSEMOVE_FAST


void c_exit(commandSystem *const restrict cmdSys, const size_t argc, const char **const restrict argv);

void c_mousemove(commandSystem *const restrict cmdSys, const size_t argc, const char **const restrict argv);


// The program will stop if this is set to 0.
extern return_t cv_prg_running;

// Cumulative mouse movement for the current update.
// These can be updated between input commands, so we should
// only update objects that depend on them (such as cameras)
// permanently after executing the entire command buffer.
extern int cv_mouse_dx;
extern int cv_mouse_dy;


#endif