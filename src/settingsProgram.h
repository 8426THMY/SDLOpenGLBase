#ifndef settingsProgram_h
#define settingsProgram_h


// Included for "size_t".
#include <stddef.h>
// Included for "M_PI".
#include <math.h>


#define GFX_DEFAULT_GL_VERSION_MAJOR 3
#define GFX_DEFAULT_GL_VERSION_MINOR 3
#define GFX_ENABLE_DOUBLEBUFFERING 1

#define PRG_WINDOW_DEFAULT_WIDTH  640
#define PRG_WINDOW_DEFAULT_HEIGHT 480
#define PRG_ASPECT_RATIO_X 4
#define PRG_ASPECT_RATIO_Y 3

#define PRG_FOV_DEFAULT M_PI/3

#define PRG_UPDATE_RATE 125.f
#define PRG_FRAME_RATE  125.f
#define PRG_TIME_SPEED  1.f

// We'll probably get undefined results if this is less than or equal to 0.
#define PRG_NUM_LOOKBACK_STATES ((size_t)PRG_UPDATE_RATE)



#endif