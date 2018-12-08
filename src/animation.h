#ifndef animation_h
#define animation_h


#include <stdio.h>

#include "utilTypes.h"


//Stores frame data for an instance of any sort of animation.
typedef struct animationFrameData {
	//Vector storing the timestamp for when each frame ends.
	float *time;
	size_t numFrames;

	//How many times the animation should be played!
	unsigned int playNum;
} animationFrameData;

//Stores information required for an instance of any sort of animation.
typedef struct animationData {
	unsigned int currentPlayNum;

	float animTime;

	size_t currentAnim;
	size_t currentFrame;
	size_t nextFrame;
} animationData;


void animFrameDataInit(animationFrameData *frameData);
void animationInit(animationData *anim);

void animationSetAnim(animationData *animData, const size_t playNum, const size_t animNum);
void animationUpdate(animationData *anim, const animationFrameData *animData, const float time);
float animationGetFrameProgress(const animationData *anim, const animationFrameData *animData);

void animFrameDataClear(animationFrameData *frameData);


extern float defaultAnimTime;


#endif