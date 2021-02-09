#ifndef settingsProgram_h
#define settingsProgram_h


// Included for "size_t".
#include <stddef.h>
// Included for "M_PI".
#include <math.h>


#define WINDOW_DEFAULT_WIDTH  640
#define WINDOW_DEFAULT_HEIGHT 480
#define ASPECT_RATIO_X 4
#define ASPECT_RATIO_Y 3

#define FOV_DEFAULT M_PI/3

#define UPDATE_RATE 125.f
#define FRAME_RATE  125.f

// We'll probably get undefined results if this is less than or equal to 0.
#define NUM_LOOKBACK_STATES ((size_t)UPDATE_RATE)



#endif