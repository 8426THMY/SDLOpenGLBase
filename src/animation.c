#include "animation.h"


#include <stdlib.h>

#include "memoryManager.h"


#warning "What if we aren't using the global memory manager?"


//This is used by "error" objects
//which require animation data.
float defaultAnimTime = 0.f;


void animFrameDataInit(animationFrameData *frameData){
	frameData->playNum = 0;

	frameData->time = NULL;
	frameData->numFrames = 0;
}

void animationInit(animationData *animData){
	animData->currentPlayNum = 0;

	animData->animTime = 0.f;

	animData->currentAnim = 0;
	animData->currentFrame = 0;
	animData->nextFrame = 0;
}


//Change the current animation!
void animationSetAnim(animationData *animData, const size_t playNum, const size_t animNum){
	animData->currentPlayNum = playNum;

	animData->animTime = 0.f;

	animData->currentAnim = animNum;
	animData->currentFrame = 0;
	animData->nextFrame = 0;
}

//Update the animation's current frame!
void animationUpdate(animationData *animData, const animationFrameData *frameData, const float time){
	//No point in animating if nothing has changed.
	/** Not sure if this would break things or not... **/
	if(time != 0.f){
		float lastFrameTime = frameData->time[frameData->numFrames - 1];
		//Make sure the animation goes for longer than 0 milliseconds to avoid infinite loops.
		if(lastFrameTime > 0.f){
			/** Note: If the animation has finished looping and you play it in the opposite direction   **/
			/**       (without completing a loop) then the original direction again, it will just stop. **/
			//We're animating forwards and we've haven't finished looping!
			if(time >= 0.f && (animData->currentPlayNum == -1 || animData->currentPlayNum != frameData->playNum)){
				animData->animTime += time;

				//If we've finished the animation, continue looping if we can!
				if(animData->animTime > lastFrameTime){
					size_t oldLoops = animData->currentPlayNum;
					do {
						animData->animTime -= lastFrameTime;
						++animData->currentPlayNum;
					} while(animData->animTime > lastFrameTime);

					//If the old loop counter was -1, we're looping indefinitely.
					if(oldLoops == -1){
						animData->currentPlayNum = -1;

					//If we've finished looping, end on the last frame!
					}else if(animData->currentPlayNum >= frameData->playNum){
						animData->currentPlayNum = frameData->playNum;

						animData->currentFrame = frameData->numFrames - 1;
						animData->nextFrame = animData->currentFrame;
						animData->animTime = lastFrameTime;

						return;
					}

					//Jump to the beginning of the animation so we don't skip any frames while finding the one we're up to!
					animData->currentFrame = 0;
				}

				//Find the frame we're up to!
				while(animData->animTime > frameData->time[animData->currentFrame]){
					++animData->currentFrame;
				}

			//We're animating backwards and we haven't finished looping!
			}else if(animData->currentPlayNum == -1 || animData->currentPlayNum != 0){
				animData->animTime += time;

				//If we've finished the animation, continue from the end!
				if(animData->animTime < 0.f){
					size_t oldLoops = animData->currentPlayNum;
					do {
						animData->animTime += lastFrameTime;
						--animData->currentPlayNum;
					} while(animData->animTime < 0.f);

					//If the old loop counter was -1, we're looping indefinitely.
					if(oldLoops == -1){
						animData->currentPlayNum = -1;

					//If we've finished looping, end on the first frame!
					}else if(animData->currentPlayNum == 0 || animData->currentPlayNum > oldLoops){
						animData->currentPlayNum = 0;
						animData->currentFrame = 0;
						animData->nextFrame = 0;
						animData->animTime = 0.f;

						return;
					}

					//Jump to the end of the animation so we don't skip any frames while finding the one we're up to!
					animData->currentFrame = frameData->numFrames - 1;
				}

				//Make sure we don't run this loop if we're on the first frame.
				if(animData->animTime > frameData->time[0]){
					//Find the frame we're up to!
					while(animData->animTime <= frameData->time[animData->currentFrame - 1]){
						--animData->currentFrame;
					}

				//If we are, just set it!
				}else{
					animData->currentFrame = 0;
				}
			}

			//Find the next frame!
			animData->nextFrame = animData->currentFrame + 1;
			if(animData->nextFrame == frameData->numFrames){
				animData->nextFrame = 0;
			}
		}
	}
}

float animationGetFrameProgress(const animationData *animData, const animationFrameData *frameData){
	//If the animation length is less than or equal to 0, we don't want to divide by 0.
	if(frameData->time[frameData->numFrames - 1] > 0.f){
		if(animData->currentFrame != 0){
			return((animData->animTime - frameData->time[animData->currentFrame - 1]) / (frameData->time[animData->currentFrame] - frameData->time[animData->currentFrame - 1]));
		}

		return(animData->animTime / frameData->time[animData->currentFrame]);
	}

	return(0.f);
}


void animFrameDataClear(animationFrameData *frameData){
	if(frameData->time != NULL){
		memoryManagerGlobalFree(frameData->time);
	}
}