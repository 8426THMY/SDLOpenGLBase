#include "timestep.h"


#include "settingsProgram.h"


void timestepInit(timestep *const restrict step, const float updateRate, const float renderRate){
	// Updates/renders per second.
	step->updateRate = updateRate;
	step->renderRate = renderRate;

	// Updates/renders per millisecond.
	step->updateTickrate = updateRate / 1000.f;
	step->renderTickrate = renderRate / 1000.f;

	// Milliseconds per update/render.
	step->updateTime = 1000.f / updateRate;
	step->renderTime = (renderRate <= 0.f) ? 0.f : 1000.f / renderRate;

	// Seconds per update/render.
	step->updateDelta = 1.f / updateRate;
	// This is set during the program loop.
	step->renderDelta = 0.f;
}