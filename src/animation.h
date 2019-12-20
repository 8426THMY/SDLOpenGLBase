#ifndef animation_h
#define animation_h


#include <stdio.h>

#include "utilTypes.h"


// Stores frame data for an instance of any sort of animation.
typedef struct animationFrameData {
	// Vector storing the timestamp for when each frame ends.
	float *time;
	size_t numFrames;

	// How many times the animation should be played!
	unsigned int playNum;
} animationFrameData;

// Stores information required for an instance of any sort of animation.
typedef struct animationData {
	unsigned int currentPlayNum;

	float animTime;

	size_t currentFrame;
	size_t nextFrame;
} animationData;


void animFrameDataInit(animationFrameData *frameData);
void animationInit(animationData *animData);

void animationSetAnim(animationData *animData, const size_t playNum);
void animationUpdate(animationData *animData, const animationFrameData *frameData, const float time);
float animationGetFrameProgress(const animationData *animData, const animationFrameData *frameData);

void animFrameDataClear(animationFrameData *frameData);


extern float animTimeDefault;


#endif