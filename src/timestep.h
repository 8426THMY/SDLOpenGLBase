#ifndef timestep_h
#define timestep_h


typedef struct timestep {
	// Updates/renders per second.
	float updateRate;
	float renderRate;

	// Seconds per update.
	float updateDelta;
	// Progress through the current update.
	float renderDelta;

	// Updates/renders per millisecond.
	float updateTickrate;
	float renderTickrate;

	// Milliseconds per update/render.
	float updateTime;
	float renderTime;

	// Coefficient for the speed at which time flows.
	// This lets us control the program's speed while
	// keeping the update logic and deltas the same.
	float timeSpeed;
	float updateTickrateScaled;
	float updateTimeScaled;
} timestep;


void timestepInit(
	timestep *const restrict step,
	const float updateRate, const float renderRate,
	const float timeSpeed
);
void timestepSetTimeSpeed(timestep *const restrict step, const float timeSpeed);


#endif
