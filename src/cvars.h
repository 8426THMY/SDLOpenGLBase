#ifndef cvars_h
#define cvars_h


#include "command.h"
#include "utilTypes.h"


// Handle regular mouse motion events in an optimized way.
#define C_MOUSEMOVE_FAST


void c_exit(commandSystem *const restrict cmdSys, const size_t argc, const char **const restrict argv);

void c_mousemove(commandSystem *const restrict cmdSys, const size_t argc, const char **const restrict argv);


extern return_t cv_prg_running;

// Cumulative mouse movement for the current update.
extern int cv_mouse_dx;
extern int cv_mouse_dy;


#endif