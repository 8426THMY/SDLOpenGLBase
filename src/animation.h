#ifndef animation_h
#define animation_h


#include <stddef.h>

#include "utilTypes.h"


#define ANIMATION_LOOP_INDEFINITELY ((unsigned int)-1)


// Stores frame data for an instance of any sort of animation.
typedef struct animationFrameData {
	// This stores the timestamps for when each frame should end.
	float *time;
	size_t numFrames;
	// Number of times the animation should be looped.
	unsigned int playNum;
} animationFrameData;

// Stores information required for an instance of any sort of animation.
typedef struct animationData {
	float animTime;
	size_t currentFrame;
	// Number of times the animation has looped.
	unsigned int currentPlayNum;
} animationData;


void animFrameDataInit(animationFrameData *const restrict frameData);
void animationInit(animationData *const restrict animData, const size_t playNum);

void animationUpdate(animationData *const restrict animData, const animationFrameData *const restrict frameData, const float time);
float animationGetFrameProgress(const animationData *const restrict animData, const animationFrameData *const restrict frameData);
size_t animationGetNextFrame(const size_t currentFrame, const size_t numFrames);

void animFrameDataClear(animationFrameData *const restrict frameData);


extern float g_animTimeDefault;


#endif