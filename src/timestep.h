#ifndef timestep_h
#define timestep_h


typedef struct timestep {
	// Updates/renders per second.
	float updateRate;
	float renderRate;

	// Updates/renders per millisecond.
	float updateTickrate;
	float renderTickrate;

	// Milliseconds per update/render.
	float updateTime;
	float renderTime;

	// This is really just the inverse update rate.
	float updateDelta;
	// This is the time to interpolate
	// for when we're rendering the scene.
	float renderDelta;
} timestep;


void timestepInit(timestep *const restrict step, const float updateRate, const float renderRate);


#endif
