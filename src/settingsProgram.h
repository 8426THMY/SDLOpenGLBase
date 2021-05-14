#ifndef settingsProgram_h
#define settingsProgram_h


// Included for "size_t".
#include <stddef.h>
// Included for "M_PI".
#include <math.h>


#define GFX_DEFAULT_GL_VERSION_MAJOR 3
#define GFX_DEFAULT_GL_VERSION_MINOR 3
#define GFX_ENABLE_DOUBLEBUFFERING 1

/*
** Rather than wait until "nextRender" to begin rendering logic,
** we can start rendering sooner as long as no updates come first.
** Afterwards, we sleep until "nextRender" and then draw everything.
**
** This may possibly help maintain the framerate limit when rendering
** by anticipating lag spikes and using otherwise wasted time to draw.
**
** In reality, this doesn't seem to make a difference.
** This is likely a result of how OpenGL buffers draw calls.
*/
#define PRG_ENABLE_EFFICIENT_RENDERING

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