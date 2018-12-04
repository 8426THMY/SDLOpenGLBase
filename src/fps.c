#include "fps.h"


#include "programSettings.h"


void fpsInit(fps *framerate, const float updateRate, const float renderRate){
	framerate->updateRate = updateRate;
	framerate->renderRate = renderRate;

	framerate->updateDelta = BASE_UPDATE_RATE / updateRate;
	framerate->renderDelta = 0.f;

	framerate->updateTime = 1000.f / updateRate;
	framerate->renderTime = 1000.f / renderRate;
}