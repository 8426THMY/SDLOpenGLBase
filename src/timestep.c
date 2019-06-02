#include "timestep.h"


#include "settingsProgram.h"


void timestepInit(timestep *step, const float updateRate, const float renderRate){
	//Updates/renders per second.
	step->updateRate = updateRate;
	step->renderRate = renderRate;

	//Updates/renders per millisecond.
	step->updateTickrate = updateRate / 1000.f;
	step->renderTickrate = renderRate / 1000.f;

	//Milliseconds per update/render.
	step->updateTime = 1000.f / updateRate;
	step->renderTime = 1000.f / renderRate;
}