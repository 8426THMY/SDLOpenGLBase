#include "animation.h"


#include "memoryManager.h"


#warning "What if we aren't using the global memory manager?"


// This is used by "error" objects
// which require animation data.
float g_animTimeDefault = 0.f;


// Forward-declare any helper functions!
static void getFrameForward(animationData *const restrict animData, const animationFrameData *const restrict frameData);
static void getFrameBackward(animationData *const restrict animData, const animationFrameData *const restrict frameData);


void animFrameDataInit(animationFrameData *const restrict frameData){
	frameData->time = NULL;
	frameData->numFrames = 0;
	frameData->playNum = 0;
}

void animationInit(animationData *const restrict animData, const size_t playNum){
	animData->animTime = 0.f;
	animData->currentFrame = 0;
	animData->currentPlayNum = playNum;
}


/** Note: If the animation has finished looping and you play it in the opposite direction   **/
/**       (without completing a loop) then the original direction again, it will just stop. **/
// Update the animation's current frame!
#warning "This needs a rewrite to make it simpler. We shouldn't handle rewinding (or maybe even play counts) by default."
void animationUpdate(animationData *const restrict animData, const animationFrameData *const restrict frameData, const float time){
	animData->animTime += time;

	// If we're animating forwards and haven't finished looping, continue playback!
	if(time > 0.f && (animData->currentPlayNum == ANIMATION_LOOP_INDEFINITELY || animData->currentPlayNum != frameData->playNum)){
		getFrameForward(animData, frameData);

	// If we're animating backwards and haven't finished looping, continue rewinding!
	}else if(time < 0.f && (animData->currentPlayNum == ANIMATION_LOOP_INDEFINITELY || animData->currentPlayNum != 0)){
		getFrameBackward(animData, frameData);
	}
}

/*
** Return a number between 0 and 1 that tells
** us how far through the current frame we are.
*/
float animationGetFrameProgress(const animationData *const restrict animData, const animationFrameData *const restrict frameData){
	// If the animation length is less than or equal to 0, we don't want to divide by 0.
	if(frameData->time[frameData->numFrames - 1] > 0.f){
		if(animData->currentFrame != 0){
			return((animData->animTime - frameData->time[animData->currentFrame - 1]) / (frameData->time[animData->currentFrame] - frameData->time[animData->currentFrame - 1]));
		}

		return(animData->animTime / frameData->time[animData->currentFrame]);
	}

	return(0.f);
}

// Return the next frame of the animation.
size_t animationGetNextFrame(const size_t currentFrame, const size_t numFrames){
	return((currentFrame < numFrames - 1) ? currentFrame + 1 : 0);
}


void animFrameDataClear(animationFrameData *const restrict frameData){
	if(frameData->time != NULL && frameData->time != &g_animTimeDefault){
		memoryManagerGlobalFree(frameData->time);
	}
}


// Check which frame we should be on!
static void getFrameForward(animationData *const restrict animData, const animationFrameData *const restrict frameData){
	const float lastFrameTime = frameData->time[frameData->numFrames - 1];
	// Make sure the animation takes time to avoid infinite loops.
	if(lastFrameTime > 0.f){
		const float *curTime;

		// If the animation has finished, loop to the beginning!
		if(animData->animTime > lastFrameTime){
			size_t numLoops = 0;
			do {
				animData->animTime -= lastFrameTime;
				++numLoops;
			} while(animData->animTime > lastFrameTime);

			// Only increment the play count if we're not looping indefinitely.
			if(!valueIsInvalid(animData->currentPlayNum, unsigned int)){
				animData->currentPlayNum += numLoops;
				// If we've finished looping, end on the last frame!
				if(animData->currentPlayNum >= frameData->playNum){
					animData->currentPlayNum = frameData->playNum;
					animData->currentFrame = frameData->numFrames - 1;
					animData->animTime = lastFrameTime;

					return;
				}
			}

			// Jump to the beginning of the animation so we don't
			// skip any frames while finding the one we're up to!
			animData->currentFrame = 0;
		}

		curTime = &frameData->time[animData->currentFrame];
		// Find the frame we're up to!
		while(animData->animTime > *curTime){
			++animData->currentFrame;
			++curTime;
		}
	}
}

// Check which frame we should be on if the animation is playing in reverse!
static void getFrameBackward(animationData *const restrict animData, const animationFrameData *const restrict frameData){
	const float lastFrameTime = frameData->time[frameData->numFrames - 1];
	// Make sure the animation takes time to avoid infinite loops.
	if(lastFrameTime > 0.f){
		// If the animation has finished, continue from the end!
		if(animData->animTime < 0.f){
			size_t numLoops = 0;
			do {
				animData->animTime += lastFrameTime;
				--numLoops;
			} while(animData->animTime < 0.f);

			// Only increment the play count if we're not looping indefinitely.
			if(!valueIsInvalid(animData->currentPlayNum, unsigned int)){
				animData->currentPlayNum -= numLoops;
				// If we've finished looping, end on the last frame!
				if(animData->currentPlayNum <= numLoops){
					animData->currentPlayNum = 0;
					animData->currentFrame = 0;
					animData->animTime = 0.f;

					return;
				}
			}

			// Jump to the end of the animation so we don't
			// skip any frames while finding the one we're up to!
			animData->currentFrame = frameData->numFrames - 1;
		}

		// Make sure we don't run this loop if we're on the first frame.
		if(animData->animTime > frameData->time[0]){
			const float *curTime = &frameData->time[animData->currentFrame - 1];
			// Find the frame we're up to!
			while(animData->animTime <= *curTime){
				--animData->currentFrame;
				--curTime;
			}
		}else{
			animData->currentFrame = 0;
		}
	}
}