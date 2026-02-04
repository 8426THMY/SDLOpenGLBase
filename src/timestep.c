#include "timestep.h"


#include "settingsProgram.h"


void timestepInit(
	timestep *const restrict step,
	const float updateRate, const float renderRate,
	const float timeSpeed
){

	// Updates/renders per second.
	step->updateRate = updateRate;
	step->renderRate = renderRate;

	// Seconds per update/render.
	step->updateDelta = 1.f / updateRate;
	// This is the time to interpolate for when
	// we're rendering the scene. We set this
	// value during the program loop.
	step->renderDelta = 0.f;

	// Updates/renders per millisecond.
	step->updateTickrate = updateRate / 1000.f;
	step->renderTickrate = renderRate / 1000.f;

	// Milliseconds per update/render.
	step->updateTime = 1000.f / updateRate;
	step->renderTime = (renderRate <= 0.f) ? 0.f : 1000.f / renderRate;

	// Controls spacing between updates
	// independently to update logic.
	timestepSetTimeSpeed(step, timeSpeed);
}

/*
** Change the program's speed to the value given by "timeSpeed".
** Note that this just controls the spacing between updates,
** and will not affect the logic performed during each update.
*/
void timestepSetTimeSpeed(timestep *const restrict step, const float timeSpeed){
	step->timeSpeed = timeSpeed;
	step->updateTickrateScaled = step->updateTickrate * timeSpeed;
	step->updateTimeScaled     = step->updateTime     / timeSpeed;
}