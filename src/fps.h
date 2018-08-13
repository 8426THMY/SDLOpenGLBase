#ifndef fps_h
#define fps_h


#include <SDL2/SDL.h>


typedef struct fps {
	float updateRate;
	float renderRate;

	float updateDelta;
	float renderDelta;

	float updateTime;
	float renderTime;
} fps;


void fpsInit(fps *framerate, const float updateRate, const float renderRate);


#endif
