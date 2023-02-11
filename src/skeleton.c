#include "skeleton.h"


#include "utilString.h"
#include "utilFile.h"

#include "memoryManager.h"
#include "moduleSkeleton.h"


#define SKELETON_PATH_PREFIX        "."FILE_PATH_DELIMITER_STR"resource"FILE_PATH_DELIMITER_STR"models"FILE_PATH_DELIMITER_STR
#define SKELETON_PATH_PREFIX_LENGTH (sizeof(SKELETON_PATH_PREFIX) - 1)

// These must be at least 1!
#define BASE_BONE_CAPACITY  1
#define BASE_FRAME_CAPACITY 1

#define SKELE_ANIM_FRAME_RATE 24.f
#define SKELE_ANIM_FRAME_TIME (1000.f / SKELE_ANIM_FRAME_RATE)


// When a model doesn't have a skeleton, it needs to have a single root bone. Rather
// than create it every time, we can use these. The state is also useful when blending.
static bone defaultBone = {
	.name = "root",

	.localBind.pos.x       = 0.f, .localBind.pos.y       = 0.f, .localBind.pos.z       = 0.f,
	.localBind.rot.x       = 0.f, .localBind.rot.y       = 0.f, .localBind.rot.z       = 0.f, .localBind.rot.w     = 1.f,
	.localBind.scale.x     = 1.f, .localBind.scale.y     = 1.f, .localBind.scale.z     = 1.f,

	.invGlobalBind.pos.x   = 0.f, .invGlobalBind.pos.y   = 0.f, .invGlobalBind.pos.z   = 0.f,
	.invGlobalBind.rot.x   = 0.f, .invGlobalBind.rot.y   = 0.f, .invGlobalBind.rot.z   = 0.f, .invGlobalBind.rot.w = 1.f,
	.invGlobalBind.scale.x = 1.f, .invGlobalBind.scale.y = 1.f, .invGlobalBind.scale.z = 1.f,

	.parent = valueInvalid(boneIndex_t)
};

skeleton g_skeleDefault = {
	.name = "default",
	.bones = &defaultBone,
	.numBones = 1
};


void boneInit(
	bone *const restrict bone,
	char *const restrict name, const boneIndex_t parent,
	const boneState *const restrict state
){

	bone->name = name;
	bone->parent = parent;

	// If the bone's state has been specified, we can just set it.
	if(state != NULL){
		bone->invGlobalBind = *state;

	// Otherwise, use the default bone state!
	}else{
		bone->invGlobalBind = defaultBone.invGlobalBind;
	}
}

void skeleInit(skeleton *const restrict skele){
	skele->name = NULL;

	skele->bones = NULL;
	skele->numBones = 0;
}

void skeleInitSet(
	skeleton *const restrict skele, const char *const restrict name,
	const size_t nameLength, bone *const restrict bones, const boneIndex_t numBones
){

	skele->name = memoryManagerGlobalAlloc(nameLength);
	if(skele->name == NULL){
		/** MALLOC FAILED **/
	}
	strcpy(skele->name, name);

	// If the skeleton actually has some bones, we can just copy the pointers.
	if(bones != NULL){
		bone *curBone = bones;
		const bone *const lastBone = &curBone[numBones];

		// Make sure we invert each bone's state!
		for(; curBone < lastBone; ++curBone){
			transformInvert(&curBone->invGlobalBind);
		}

		skele->bones = bones;
		skele->numBones = numBones;

	// Otherwise, we need to store a single root bone.
	}else{
		skele->bones = &defaultBone;
		skele->numBones = 1;
	}
}

void skeleAnimDefInit(skeletonAnimDef *const restrict animDef){
	animDef->name = NULL;

	animFrameDataInit(&animDef->frameData);

	animDef->boneNames = NULL;
	animDef->frames = NULL;
	animDef->numBones = 0;
}

void skeleAnimInit(
	skeletonAnim *const restrict anim, skeletonAnimDef *const restrict animDef,
	const float speed, const float intensity
){

	anim->animDef = animDef;

	animationInit(&anim->animData, speed, ANIMATION_LOOP_INDEFINITELY);
	anim->interpTime = 0.f;
	anim->intensity = intensity;
}

void skeleStateInit(skeletonState *const restrict skeleState, const skeleton *const restrict skele){
	skeleState->skele = skele;
	skeleState->anims = NULL;
	skeleState->bones = memoryManagerGlobalAlloc(sizeof(*skeleState->bones) * skele->numBones);
	if(skeleState->bones == NULL){
		/** MALLOC FAILED **/
	}
}


/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
/** At the moment, if any bones have some unspecified animation  **/
/** frames, their states will be undefined. This is very bad!    **/
/*
** Load the SMD animation specified by "skeleAnimPath" and return a pointer to it.
** If the animation could not be loaded, return a NULL pointer.
*/
/** This is temporary and should be merged with the function in "model.c" or removed. **/
skeletonAnimDef *skeleAnimSMDLoad(const char *const restrict skeleAnimPath, const size_t skeleAnimPathLength){
	skeletonAnimDef *animDef;

	FILE *skeleAnimFile;
	char skeleAnimFullPath[FILE_MAX_PATH_LENGTH];


	#ifdef TEMP_MODULE_FIND
	// If the animation has already been loaded, return a pointer to it!
	if((animDef = moduleSkeletonAnimDefFind(skeleAnimPath)) != NULL){
		return(animDef);
	}
	#else
	animDef = NULL;
	#endif


	// Generate the full path for the skeleton!
	fileGenerateFullResourcePath(
		SKELETON_PATH_PREFIX, SKELETON_PATH_PREFIX_LENGTH,
		skeleAnimPath, skeleAnimPathLength,
		skeleAnimFullPath
	);


	// Load the skeleton!
	skeleAnimFile = fopen(skeleAnimFullPath, "r");
	if(skeleAnimFile != NULL){
		return_t success = 1;

		// We use this capacity for all of our arrays, since we only use one at a time.
		size_t tempCapacity = BASE_BONE_CAPACITY;
		// This represents the current size of both the time and frame arrays.
		size_t numFrames = 0;

		size_t tempBonesSize = 0;
		bone *tempBones;
		float *tempTimes;
		boneState **tempFrames;

		// Store a reference to the current frame that we're loading.
		boneState *currentFrame = NULL;

		// This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		// This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char *tokPos;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		// Temporarily stores bones.
		tempBones = memoryManagerGlobalAlloc(BASE_BONE_CAPACITY * sizeof(*tempBones));
		if(tempBones == NULL){
			/** MALLOC FAILED **/
		}
		// Temporarily stores frame times.
		tempTimes = memoryManagerGlobalAlloc(BASE_FRAME_CAPACITY * sizeof(*tempTimes));
		if(tempTimes == NULL){
			/** MALLOC FAILED **/
		}
		// Temporarily stores frame states.
		tempFrames = memoryManagerGlobalAlloc(BASE_FRAME_CAPACITY * sizeof(*tempFrames));
		if(tempFrames == NULL){
			/** MALLOC FAILED **/
		}


		while(success && (line = fileReadLine(skeleAnimFile, &lineBuffer[0], &lineLength)) != NULL){
			// No command.
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;

				// If this isn't the version number and the line
				// isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf(
						"Error loading skeletal animtion!\n"
						"Path: %s\n"
						"Line: %s\n"
						"Error: Unexpected identifier!\n",
						skeleAnimFullPath, line
					);

					success = 0;
				}
			}else{
				// Finished command.
				if(strcmp(line, "end") == 0){
					// Finished loading bone names.
					if(dataType == 1){
						tempCapacity = BASE_FRAME_CAPACITY;

					// Finished loading keyframes.
					}else if(dataType == 2){
						// If we've finished loading the animation, shrink the arrays!
						tempTimes = memoryManagerGlobalResize(tempTimes, numFrames * sizeof(*tempTimes));
						if(tempTimes == NULL){
							/** REALLOC FAILED **/
						}
						tempFrames = memoryManagerGlobalResize(tempFrames, numFrames * sizeof(*tempFrames));
						if(tempFrames == NULL){
							/** REALLOC FAILED **/
						}
					}

					dataType = 0;
					data = 0;
				}else{
					// Loading bone names.
					if(dataType == 1){
						// Get this bone's ID.
						const boneIndex_t boneID = strtoul(line, &tokPos, 10);
						// Make sure a bone with this ID actually exists.
						if(boneID == tempBonesSize){
							bone tempBone;
							const size_t boneNameLength = stringMultiDelimited(tokPos, line + lineLength - tokPos, "\" ", (const char **)&tokPos);
							// Get the bone's name.
							tempBone.name = memoryManagerGlobalAlloc(boneNameLength + 1);
							if(tempBone.name == NULL){
								/** MALLOC FAILED **/
							}
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							// Get the ID of this bone's parent.
							tempBone.parent = strtol(tokPos + boneNameLength + 1, NULL, 10);


							// If we're out of space, allocate some more!
							if(tempBonesSize >= tempCapacity){
								tempCapacity = tempBonesSize * 2;
								tempBones = memoryManagerGlobalRealloc(tempBones, tempCapacity * sizeof(*tempBones));
								if(tempBones == NULL){
									/** REALLOC FAILED **/
								}
							}
							// Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf(
								"Error loading skeletal animtion!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Found node %u when expecting node "PRINTF_SIZE_T"!\n",
								skeleAnimFullPath, line, boneID, tempBonesSize
							);

							success = 0;
						}

					// Loading keyframes.
					}else if(dataType == 2){
						// If the line begins with time, get the frame's timestamp!
						if(lineLength >= 6 && memcmp(line, "time ", 5) == 0){
							const unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;

								// Allocate memory for the new frame if we have to!
								if(numFrames >= tempCapacity){
									tempCapacity = numFrames * 2;
									// Resize the time array!
									tempTimes = memoryManagerGlobalRealloc(tempTimes, tempCapacity * sizeof(*tempTimes));
									if(tempTimes == NULL){
										/** REALLOC FAILED **/
									}
									// Resize the frames array!
									tempFrames = memoryManagerGlobalRealloc(tempFrames, tempCapacity * sizeof(*tempFrames));
									if(tempFrames == NULL){
										/** REALLOC FAILED **/
									}
								}

								// Allocate memory for each bone state in this frame!
								currentFrame = memoryManagerGlobalAlloc(tempBonesSize * sizeof(**tempFrames));
								if(currentFrame == NULL){
									/** MALLOC FAILED **/
								}
								tempFrames[numFrames] = currentFrame;

								tempTimes[numFrames] = (data + 1) * SKELE_ANIM_FRAME_TIME;
								++numFrames;
							}else{
								printf(
									"Error loading skeletal animtion!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Frame timestamps do not increment sequentially!\n",
									skeleAnimFullPath, line
								);

								success = 0;
							}

						// Otherwise, we're setting the bone's state!
						}else{
							// Get this bone's ID.
							const boneIndex_t boneID = strtoul(line, &tokPos, 10);
							if(boneID < tempBonesSize){
								boneState *const currentState = &currentFrame[boneID];

								// Load the bone's position!
								float x = strtof(tokPos, &tokPos) * 0.05f;
								float y = strtof(tokPos, &tokPos) * 0.05f;
								float z = strtof(tokPos, &tokPos) * 0.05f;
								vec3InitSet(&currentState->pos, x, y, z);

								// Load the bone's rotation!
								x = strtof(tokPos, &tokPos);
								y = strtof(tokPos, &tokPos);
								z = strtof(tokPos, NULL);
								quatInitEulerXYZ(&currentState->rot, x, y, z);

								// Set the bone's scale!
								quatInitIdentity(&currentState->shear);
								vec3InitSet(&currentState->scale, 1.f, 1.f, 1.f);

								//The Source Engine uses Z as its up axis, so we need to fix that with the root bone.
								if(boneID == 0 && strcmp(skeleAnimPath, "soldier_animations_anims_new/a_flinch01.smd") != 0){
									quatRotateByEulerXYZ(&currentState->rot, -0.5f*3.14159265f, 0.f, 0.f);
									y = currentState->pos.y;
									currentState->pos.y = currentState->pos.z;
									currentState->pos.z = -y;
								}
							}else{
								printf(
									"Error loading skeletal animtion!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Found skeletal data for bone %u, which doesn't exist!\n",
									skeleAnimFullPath, line, boneID
								);

								success = 0;
							}
						}
					}
				}
			}
		}

		fclose(skeleAnimFile);


		// If there weren't any errors, allocate memory for the animation and set it up!
		if(success){
			const bone *curBone = tempBones;
			const bone *const lastBone = &tempBones[tempBonesSize];
			char **curName;


			animDef = moduleSkeletonAnimDefAlloc();
			if(animDef == NULL){
				/** MALLOC FAILED **/
			}


			// Set the animation's name!
			animDef->name = memoryManagerGlobalAlloc(skeleAnimPathLength + 1);
			if(animDef->name == NULL){
				/** MALLOC FAILED **/
			}
			memcpy(animDef->name, skeleAnimPath, skeleAnimPathLength + 1);


			animDef->frameData.time = tempTimes;
			animDef->frameData.numFrames = numFrames;
			animDef->frameData.playNum = valueInvalid(unsigned int);

			// Allocate memory for the array of bone names!
			animDef->boneNames = memoryManagerGlobalAlloc(tempBonesSize * sizeof(*animDef->boneNames));
			if(animDef->boneNames == NULL){
				/** MALLOC FAILED **/
			}

			curName = animDef->boneNames;
			// Fill the array of bone names!
			for(; curBone < lastBone; ++curBone, ++curName){
				*curName = curBone->name;
			}

			animDef->frames = tempFrames;
			animDef->numBones = tempBonesSize;
		}else{
			// We don't need to check if these are NULL,
			// as we do that when we're using them.
			memoryManagerGlobalFree(tempTimes);
			memoryManagerGlobalFree(tempFrames);
		}


		// We don't free the bones' names as the animation uses them.
		memoryManagerGlobalFree(tempBones);
	}else{
		printf(
			"Unable to open skeletal animation file!\n"
			"Path: %s\n",
			skeleAnimFullPath
		);
	}


	return(animDef);
}


// Progress an animation!
void skeleAnimUpdate(skeletonAnim *const restrict anim, const float dt){
	animationData *const animData = &anim->animData;
	const animationFrameData *const frameData = &anim->animDef->frameData;

	animationUpdate(animData, frameData, dt);
	anim->interpTime = animationGetFrameProgress(animData, frameData);
}

#warning "If interpolation is turned off, we don't need to call the transform functions."
/*
** For each animation in the skeleton state, compute the local
** transformations on the specified bone and apply them to "state".
** The input state will generally be the global bind pose, so these
** tranformations are prepended to it.
*/
void skeleStatePrependAnimations(
	boneState *const restrict state,
	const skeletonState *const restrict skeleState,
	const boneIndex_t boneID, const char *const restrict boneName
){

	const skeletonAnim *curAnim = skeleState->anims;

	// Update the bone using each animation!
	while(curAnim != NULL){
		const boneIndex_t animBoneID = skeleAnimFindBone(curAnim, boneName);
		// Make sure this bone exists in the animation!
		if(!valueIsInvalid(animBoneID, boneIndex_t)){
			const size_t currentFrame = curAnim->animData.currentFrame;
			const size_t nextFrame = animationGetNextFrame(currentFrame, curAnim->animDef->frameData.numFrames);
			boneState animState;

			// Interpolate between the current
			// and next frames of the animation.
			transformInterpSet(
				&curAnim->animDef->frames[currentFrame][animBoneID],
				&curAnim->animDef->frames[nextFrame][animBoneID],
				curAnim->interpTime,
				&animState
			);

			/** TEMPORARY **/
			// Remove the bind pose's "contribution" to the animation.
			if(strcmp(curAnim->animDef->name, "soldier_animations_anims_new/a_flinch01.smd") != 0){
				boneState invLocalBind;
				transformInvertOut(&skeleState->skele->bones[boneID].localBind, &invLocalBind);
				transformMultiplyP2(&invLocalBind, &animState);
			}
			// Set the animation's intensity by blending from the identity state.
			transformInterpSet(&g_transformIdentity, &animState, curAnim->intensity, &animState);
			transformMultiplyP1(state, &animState);
		}

		// Continue to the next animation in the list.
		curAnim = moduleSkeletonAnimNext(curAnim);
	}
}


#warning "We should store bones in a search tree of some kind."
// Find a bone in a skeleton from its name and return its index.
boneIndex_t skeleFindBone(const skeleton *const restrict skele, const char *const restrict name){
	const bone *curBone = skele->bones;
	const bone *const lastBone = &curBone[skele->numBones];
	boneIndex_t i = 0;
	for(; curBone < lastBone; ++curBone, ++i){
		if(strcmp(curBone->name, name) == 0){
			return(i);
		}
	}

	return(-1);
}

#warning "We should store bones in a search tree of some kind."
// Find a bone in an animation from its name and return its index.
boneIndex_t skeleAnimFindBone(const skeletonAnim *const restrict skeleAnim, const char *const restrict name){
	char **curName = skeleAnim->animDef->boneNames;
	char **const lastName = &curName[skeleAnim->animDef->numBones];
	boneIndex_t i = 0;
	for(; curName < lastName; ++curName, ++i){
		if(strcmp(*curName, name) == 0){
			return(i);
		}
	}

	return(-1);
}


// This function assumes that the input isn't the default bone.
void boneDelete(bone *const restrict bone){
	if(bone->name != NULL){
		memoryManagerGlobalFree(bone->name);
	}
}

void skeleDelete(skeleton *const restrict skele){
	bone *curBone = skele->bones;

	if(skele->name != NULL){
		memoryManagerGlobalFree(skele->name);
	}

	// Make sure we don't free the bone array if it's set to "defaultBone"!
	if(curBone != NULL && curBone != &defaultBone){
		const bone *const lastBone = &curBone[skele->numBones];
		do {
			boneDelete(curBone);
			++curBone;
		} while(curBone < lastBone);
		memoryManagerGlobalFree(skele->bones);
	}
}

void skeleAnimDefDelete(skeletonAnimDef *const restrict animDef){
	if(animDef->name != NULL){
		memoryManagerGlobalFree(animDef->name);
	}

	animFrameDataClear(&animDef->frameData);

	char **curName = animDef->boneNames;
	if(curName != NULL){
		char **const lastName = &curName[animDef->numBones];
		// Free each bone name!
		do {
			char *const curNameValue = *curName;
			if(curNameValue != NULL){
				memoryManagerGlobalFree(curNameValue);
			}
			++curName;
		} while(curName < lastName);
		// Now free the array!
		memoryManagerGlobalFree(animDef->boneNames);
	}
	boneState **curFrame = animDef->frames;
	if(curFrame != NULL){
		boneState **const lastFrame = &curFrame[animDef->frameData.numFrames];
		// Free each bone state!
		do {
			boneState *const curFrameState = *curFrame;
			if(curFrameState != NULL){
				memoryManagerGlobalFree(curFrameState);
			}
			++curFrame;
		} while(curFrame < lastFrame);
		// Now free the array!
		memoryManagerGlobalFree(animDef->frames);
	}
}

void skeleStateDelete(skeletonState *const restrict skeleState){
	moduleSkeletonAnimFreeArray(&skeleState->anims);
	if(skeleState->bones != NULL){
		memoryManagerGlobalFree(skeleState->bones);
	}
}