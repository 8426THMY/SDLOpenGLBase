#include "skeleton.h"


#define SKELETON_PATH_PREFIX        ".\\resource\\models\\"
#define SKELETON_PATH_PREFIX_LENGTH (sizeof(SKELETON_PATH_PREFIX) - 1)

//This must be at least 1!
#define BASE_BONE_CAPACITY 1

#define SKELE_ANIM_FRAME_RATE 24.f
#define SKELE_ANIM_FRAME_TIME (1000.f / SKELE_ANIM_FRAME_RATE)


#include "utilString.h"

#include "memoryManager.h"


#warning "Store the skeleObject normally. You need to find a way to indicate that two animations are blending that works when you copy to the next state!"
#warning "We still need to do animation blending and create bone lookups in case the animation's bones don't match the model's skeleton's (such as with the Scout)."


//Forward-declare any helper functions!
static void boneStateInvert(const boneState *b, boneState *out);

static void updateBoneStateSet(const boneState *s1, const boneState *s2, const float time, boneState *out);
static void updateBoneStateAdd(const boneState *s1, const boneState *s2, const float time, boneState *out);

static void getBlendStates(const skeleAnimState *animState, const skeleAnimState *blendState, const size_t boneNum,
                           const boneState *defState, const boneState **startState, const boneState **endState);


//When a model doesn't have a skeleton, it needs to have a single root bone. Rather
//than create it every time, we can use this. The state is also useful when blending.
static bone defaultBone = {
	.name = "root",
	.state.pos.x   = 0.f, .state.pos.y   = 0.f, .state.pos.z   = 0.f,
	.state.rot.x   = 0.f, .state.rot.y   = 0.f, .state.rot.z   = 0.f, .state.rot.w = 1.f,
	.state.scale.x = 1.f, .state.scale.y = 1.f, .state.scale.z = 1.f,
	.parent = -1
};


#warning "What if we aren't using the global memory manager?"


void boneInit(bone *bone, char *name, const size_t parent, const boneState *state){
	bone->name = name;
	bone->parent = parent;

	//If the bone's state has been specified, we can just set it.
	if(state != NULL){
		bone->state = *state;

	//Otherwise, use the default bone state!
	}else{
		bone->state = defaultBone.state;
	}
}

void skeleInit(skeleton *skele){
	skele->name = NULL;

	skele->bones = NULL;
	skele->numBones = 0;
}

void skeleInitSet(skeleton *skele, char *name, bone *bones, const size_t numBones){
	skele->name = name;

	//If the skeleton actually has some bones, we can just copy the pointers.
	if(numBones > 0 && bones != NULL){
		skele->bones = bones;
		skele->numBones = numBones;

	//Otherwise, we need to store a single root bone.
	}else{
		skele->bones = &defaultBone;
		skele->numBones = 1;
	}

	bone *curBone = skele->bones;
	const bone *lastBone = &skele->bones[skele->numBones];
	//Make sure we invert each bone's state!
	for(; curBone < lastBone; ++curBone){
		boneStateInvert(&curBone->state, &curBone->state);
	}
}

void skeleAnimInit(skeletonAnim *anim){
	anim->name = NULL;

	animFrameDataInit(&anim->frameData);

	anim->boneNames = NULL;
	anim->frames = NULL;
	anim->numBones = 0;
}

void skeleAnimStateInit(skeleAnimState *animState){
	animState->anim = NULL;

	animationInit(&animState->animData);
	animState->interpTime = 0.f;

	animState->blendAnim = NULL;
	animState->blendTime = 0.f;

	animState->lookup = NULL;

	animState->skeleState = NULL;
}

void skeleAnimInstInit(skeletonAnimInst *animInst){
	stateObjInit(animInst, sizeof(skeleAnimState));
	skeleAnimStateInit((skeleAnimState *)animInst->states[0]);
}

void skeleObjInit(skeletonObject *skeleObj, skeleton *skele){
	skeleObj->skele = skele;

	skeleObj->anims = NULL;
	skeleObj->numAnims = 0;

	//Allocate memory for the transformed bone states!
	if(skele != NULL && skele->numBones > 0){
		skeleObj->state = memoryManagerGlobalAlloc(skele->numBones * sizeof(*skeleObj->state));
		if(skeleObj->state == NULL){
			/** MALLOC FAILED **/
		}
	}else{
		skeleObj->state = NULL;
	}
}


/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
//Load an SMD animation! This is temporary and should
//be merged with the function in "model.c" or removed.
return_t skeleAnimLoadSMD(skeletonAnim *skeleAnim, const char *skeleAnimName){
	skeleAnimInit(skeleAnim);


	//Find the full path for the skeleton!
	const size_t skeleAnimNameLength = strlen(skeleAnimName);
	char *skeleAnimPath = memoryManagerGlobalAlloc(SKELETON_PATH_PREFIX_LENGTH + skeleAnimNameLength + 1);
	if(skeleAnimPath == NULL){
		/** MALLOC FAILED **/
	}
	memcpy(skeleAnimPath, SKELETON_PATH_PREFIX, SKELETON_PATH_PREFIX_LENGTH);
	strcpy(skeleAnimPath + SKELETON_PATH_PREFIX_LENGTH, skeleAnimName);

	//Load the skeleton!
	FILE *skeleAnimFile = fopen(skeleAnimPath, "r");
	if(skeleAnimFile != NULL){
		return_t success = 1;


		size_t tempCapacity = BASE_BONE_CAPACITY;
		//Temporarily stores bones.
		size_t tempBonesSize = 0;
		bone *tempBones = memoryManagerGlobalAlloc(BASE_BONE_CAPACITY * sizeof(*tempBones));
		if(tempBones == NULL){
			/** MALLOC FAILED **/
		}

		#warning "Ideally, we shouldn't have to do this."
		skeleAnim->frames = memoryManagerGlobalAlloc(sizeof(*skeleAnim->frames));
		if(skeleAnim->frames == NULL){
			/** MALLOC FAILED **/
		}
		skeleAnim->frameData.time = memoryManagerGlobalAlloc(sizeof(*skeleAnim->frameData.time));
		if(skeleAnim->frameData.time == NULL){
			/** MALLOC FAILED **/
		}

		//Store a reference to the current frame that we're loading.
		boneState *currentFrame = NULL;

		//This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		//This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while(success && (line = readLineFile(skeleAnimFile, &lineBuffer[0], &lineLength)) != NULL){
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;

				//If this isn't the version number and the line
				//isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf(
						"Error loading skeletal animtion!\n"
						"Path: %s\n"
						"Line: %s\n"
						"Error: Unexpected identifier!\n",
						skeleAnimPath, line
					);

					success = 0;
				}
			}else{
				if(strcmp(line, "end") == 0){
					//If we've finished identifying the skeleton's
					//bones, copy the data into our animation object!
					if(dataType == 1){
						tempBones = memoryManagerGlobalRealloc(tempBones, tempBonesSize * sizeof(*tempBones));
						if(tempBones == NULL){
							/** REALLOC FAILED **/
						}
						skeleAnim->boneNames = memoryManagerGlobalAlloc(tempBonesSize * sizeof(*skeleAnim->boneNames));
						if(skeleAnim->boneNames == NULL){
							/** MALLOC FAILED **/
						}
						skeleAnim->numBones = tempBonesSize;

						bone *curBone = tempBones;
						char **curName = skeleAnim->boneNames;
						const bone *lastBone = &tempBones[tempBonesSize];
						//Fill the array of bone names!
						for(; curBone < lastBone; ++curBone, ++curName){
							*curName = curBone->name;
						}

						tempCapacity = 1;

					//If we've finished loading the animation, shrink the vectors!
					}else if(dataType == 2){
						skeleAnim->frameData.time = memoryManagerGlobalRealloc(skeleAnim->frameData.time, skeleAnim->frameData.numFrames * sizeof(*skeleAnim->frameData.time));
						if(skeleAnim->frameData.time == NULL){
							/** REALLOC FAILED **/
						}
						skeleAnim->frames = memoryManagerGlobalRealloc(skeleAnim->frames, skeleAnim->frameData.numFrames * sizeof(*skeleAnim->frames));
						if(skeleAnim->frames == NULL){
							/** REALLOC FAILED **/
						}

						skeleAnim->frameData.playNum = -1;
					}

					dataType = 0;
					data = 0;
				}else{
					if(dataType == 1){
						char *tokPos = line;

						bone tempBone;

						//Get this bone's ID.
						size_t boneID = strtoul(tokPos, &tokPos, 10);
						//Make sure a bone with this ID actually exists.
						if(boneID == tempBonesSize){
							size_t boneNameLength;
							getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos, &boneNameLength);
							//Get the bone's name.
							tempBone.name = memoryManagerGlobalAlloc(boneNameLength + 1);
							if(tempBone.name == NULL){
								/** MALLOC FAILED **/
							}
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);


							//If we're out of space, allocate some more!
							if(tempBonesSize >= tempCapacity){
								tempCapacity = tempBonesSize * 2;
								tempBones = memoryManagerGlobalRealloc(tempBones, tempCapacity * sizeof(*tempBones));
								if(tempBones == NULL){
									/** REALLOC FAILED **/
								}
							}
							//Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf(
								"Error loading skeletal animtion!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Found node %u when expecting node %u!\n",
								skeleAnimPath, line, boneID, tempBonesSize
							);

							success = 0;
						}
					}else if(dataType == 2){
						//If the line begins with time, get the frame's timestamp!
						if(memcmp(line, "time ", 5) == 0){
							unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;

								//Allocate memory for the new frame if we have to!
								if(skeleAnim->frameData.numFrames >= tempCapacity){
									tempCapacity = skeleAnim->frameData.numFrames * 2;
									//Resize the time array!
									skeleAnim->frameData.time = memoryManagerGlobalRealloc(skeleAnim->frameData.time, tempCapacity * sizeof(*skeleAnim->frameData.time));
									if(skeleAnim->frameData.time == NULL){
										/** REALLOC FAILED **/
									}
									//Resize the frames array!
									skeleAnim->frames = memoryManagerGlobalRealloc(skeleAnim->frames, tempCapacity * sizeof(*skeleAnim->frames));
									if(skeleAnim->frames == NULL){
										/** REALLOC FAILED **/
									}
								}

								currentFrame = skeleAnim->frames[skeleAnim->frameData.numFrames];
								//Allocate memory for each bone state in this frame!
								currentFrame = memoryManagerGlobalAlloc(skeleAnim->numBones * sizeof(**skeleAnim->frames));
								if(currentFrame == NULL){
									/** MALLOC FAILED **/
								}

								skeleAnim->frameData.time[skeleAnim->frameData.numFrames] = (data + 1) * SKELE_ANIM_FRAME_TIME;
								++skeleAnim->frameData.numFrames;
							}else{
								printf(
									"Error loading skeletal animtion!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Frame timestamps do not increment sequentially!\n",
									skeleAnimPath, line
								);

								success = 0;
							}

						//Otherwise, we're setting the bone's state!
						}else{
							char *tokPos = line;

							//Get this bone's ID.
							size_t boneID = strtoul(tokPos, &tokPos, 10);
							if(boneID < tempBonesSize){
								boneState *currentState = &currentFrame[boneID];

								//Load the bone's position!
								float x = strtod(tokPos, &tokPos) * 0.05f;
								float y = strtod(tokPos, &tokPos) * 0.05f;
								float z = strtod(tokPos, &tokPos) * 0.05f;
								vec3InitSet(&currentState->pos, x, y, z);

								//Load the bone's rotation!
								x = strtod(tokPos, &tokPos);
								y = strtod(tokPos, &tokPos);
								z = strtod(tokPos, NULL);
								quatInitEulerRad(&currentState->rot, x, y, z);

								//Set the bone's scale!
								vec3InitSet(&currentState->scale, 1.f, 1.f, 1.f);
							}else{
								printf(
									"Error loading skeletal animtion!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Found skeletal data for bone %u, which doesn't exist!\n",
									skeleAnimPath, line, boneID
								);

								success = 0;
							}
						}
					}
				}
			}
		}

		fclose(skeleAnimFile);


		//We don't delete them properly because
		//we store the bones' names elsewhere.
		memoryManagerGlobalFree(tempBones);


		//If there wasn't an error, add it to the vector!
		if(success){
			skeleAnim->name = memoryManagerGlobalRealloc(skeleAnimPath, skeleAnimNameLength + 1);
			if(skeleAnim->name == NULL){
				/** REALLOC FAILED **/
			}
			strcpy(skeleAnim->name, skeleAnimName);

			return(1);
		}

		//Otherwise, delete the animation.
		skeleAnimDelete(skeleAnim);

	}else{
		printf(
			"Unable to open skeletal animation file!\n"
			"Path: %s\n",
			skeleAnimPath
		);
	}

	memoryManagerGlobalFree(skeleAnimPath);


	return(0);
}


//Append the transformations applied by "b2" to "b1" and store the result in "out"!
void boneStateAddTransform(const boneState *b1, const boneState *b2, boneState *out){
	vec3 pos;
	vec3MultiplyVec3(&b1->scale, &b2->pos, &pos);
	quatApplyRotationFast(&b1->rot, &pos, &pos);
	//Generate the new position!
	vec3AddVec3(&b1->pos, &pos, &out->pos);
	//Generate the new orientation!
	quatMultiplyQuat(&b1->rot, &b2->rot, &out->rot);
	//A slight error will build up if we don't normalize the rotation.
	quatNormalizeQuat(&out->rot, &out->rot);
	//Generate the new scale!
	vec3MultiplyVec3(&b1->scale, &b2->scale, &out->scale);
}

//Convert a bone state to a mat4!
void boneStateConvertToMat4(const boneState *b, mat4 *out){
	mat4InitTranslateVec3(out, &b->pos);
	mat4RotateQuat(out, &b->rot);
	mat4ScaleVec3(out, &b->scale);
}


//Animate all of an animation instance's bones!
void skeleAnimInstUpdate(skeleAnimState *animState, const float time){
	//No point in animating if nothing has changed.
	/** Not sure if this would break things or not... **/
	if(time != 0.f){
		animationData *animData = &animState->animData;
		const animationFrameData *frameData = &animState->anim->frameData;

		//Progress the animation!
		animationUpdate(animData, frameData, time);
		animState->interpTime = animationGetFrameProgress(animData, frameData);

		boneState **currentFrame = &animState->anim->frames[animState->animData.currentFrame];
		boneState **nextFrame = &animState->anim->frames[animState->animData.nextFrame];
		boneState *curState = animState->skeleState;
		const boneState *lastState = &animState->skeleState[animState->anim->numBones];
		//Update each bone's state!
		for(; curState < lastState; ++currentFrame, ++nextFrame, ++curState){
			updateBoneStateSet(
				*currentFrame,
				*nextFrame,
				animState->interpTime,
				curState
			);
		}
	}
}

/**
Here's what needs to happen. For starters, you have two choices for approaching the main animation system:
1. Entities can only use animations that share their skeletons. In that case, for bones that aren't included
   in an animation, you'll need to store a NULL pointer for their boneSequences and animStates.
2. Entities can use any animation, but each animation would need to store a lookup to the entity's skeleton.

You also need to be able to blend animations together.
1. If an animation contains bones that don't move, the animStates will either be NULL (idea 1) or nonexistant
   (idea 2).
2. Some animations should set the bones' states (walking, standing, hand motions, core actions), while
   others should simply add the offsets (flinching, sub actions). This isn't necessary,
   but it'd make things easier for people making projects as they wouldn't need separate animations for the
   upper and lower bodies, for instance.
**/
/**
Make it so these variables of skeletonAnimInsts are vectors, containing an element for each game state.
If an animation's blendTime is greater than or equal to 1.f, we should skip it. If it's less than 0.f
on the last game state, which we're getting rid of, then we can delete the skeleAnimState and shift
the others in the blend over.

animationData animData;
float interpTime;
float blendTime;
size_t *lookup;
boneState *skeleState;
**/

//blendBone(animState, blendState, state, numStateBones, unchangedState, updateFunc);

#warning "This can be optimised much, much better than this by finding the states and lookup IDs here instead of in getBlendStates."
void skeleAnimStateBlendSet(skeletonObject *skeleObj, const skeleAnimState *animState, const skeleAnimState *blendState){
	boneState *curState = skeleObj->state;
	size_t i;
	//Loop through all the bones in the entity (or whatever is using the blended animations).
	for(i = 0; i < skeleObj->skele->numBones; ++i){
		const boneState *startState, *endState;
		//If this is the first blend we've applied, we'll
		//need to blend to and from the bone's current state.
		getBlendStates(animState, blendState, i, curState, &startState, &endState);

		//Blend between the two states and update the bone!
		updateBoneStateSet(startState, endState, animState->blendTime, curState);

		++curState;
	}
}

/** This function is a duplicate of the one above it, but the last line and default state things are different. **/
void skeleAnimStateBlendAdd(skeletonObject *skeleObj, const skeleAnimState *animState, const skeleAnimState *blendState){
	boneState *curState = skeleObj->state;
	size_t i;
	//Loop through all the bones in the entity (or whatever is using the blended animations).
	for(i = 0; i < skeleObj->skele->numBones; ++i){
		const boneState *startState, *endState;
		//If we're appending this to another blend, we'll need to blend to and
		//from the default state by default, as it applies no transformations.
		getBlendStates(animState, blendState, i, &defaultBone.state, &startState, &endState);

		//Blend between the two states and update the bone!
		updateBoneStateAdd(startState, endState, animState->blendTime, curState);

		++curState;
	}
}


//Add an animation to the skeletonObject!
void skeleObjAddAnim(skeletonObject *skeleObj, skeletonAnim *anim){
	++skeleObj->numAnims;
	skeleObj->anims = memoryManagerGlobalRealloc(skeleObj->anims, skeleObj->numAnims * sizeof(*skeleObj->anims));
	if(skeleObj->anims == NULL){
		/** REALLOC FAILED **/
	}
	skeleAnimInstInit(&skeleObj->anims[skeleObj->numAnims - 1]);

	skeleAnimState *animState = skeleObj->anims[0].states[0];
	animState->anim = anim;
	animState->skeleState = memoryManagerGlobalAlloc(anim->numBones * sizeof(*animState->skeleState));
	if(animState->skeleState == NULL){
		/** MALLOC FAILED **/
	}
}

//Accumulate the transformations of every animation!
void skeleObjGenerateRenderState(skeletonObject *skeleObj){
	boneState *animState = ((skeleAnimState *)skeleObj->anims[0].states[0])->skeleState;

	const bone *curSkeleBone = skeleObj->skele->bones;
	const boneState *curAnimBone = animState;
	boneState *curObjBone = skeleObj->state;
	const bone *lastSkeleBone = &skeleObj->skele->bones[skeleObj->skele->numBones];
	//Accumulate the transformations for each bone in the animation!
	for(; curSkeleBone < lastSkeleBone; ++curSkeleBone, ++curAnimBone, ++curObjBone){
		const size_t parentID = curSkeleBone->parent;
		//If this bone has a parent, add its animation transformations to those of its parent!
		if(parentID != -1){
			boneStateAddTransform(&skeleObj->state[parentID], curAnimBone, curObjBone);

		//Otherwise, just use it by itself!
		}else{
			*curObjBone = *curAnimBone;
		}
	}

	curSkeleBone = skeleObj->skele->bones;
	curObjBone = skeleObj->state;
	lastSkeleBone = &skeleObj->skele->bones[skeleObj->skele->numBones];
	//Append each bone's inverse reference state!
	for(; curSkeleBone < lastSkeleBone; ++curSkeleBone, ++curObjBone){
		boneStateAddTransform(curObjBone, &curSkeleBone->state, curObjBone);
	}
}


void boneDelete(bone *bone){
	//We can't free the default bone's name.
	if(bone->name != NULL && bone != &defaultBone){
		memoryManagerGlobalFree(bone->name);
	}
}

void skeleDelete(skeleton *skele){
	if(skele->name != NULL){
		memoryManagerGlobalFree(skele->name);
	}

	//Make sure we don't free the bone array if its set to "defaultBone"!
	if(skele->bones != NULL && skele->bones != &defaultBone){
		memoryManagerGlobalFree(skele->bones);
	}
}

void skeleAnimDelete(skeletonAnim *anim){
	if(anim->name != NULL){
		memoryManagerGlobalFree(anim->name);
	}

	animFrameDataClear(&anim->frameData);

	char **curName = anim->boneNames;
	if(curName != NULL){
		char **lastName = &anim->boneNames[anim->numBones];
		//Free each bone name!
		for(; curName < lastName; ++curName){
			char *curNameValue = *curName;
			if(curNameValue != NULL){
				memoryManagerGlobalFree(curNameValue);
			}
		}
		//Now free the array!
		memoryManagerGlobalFree(anim->boneNames);
	}
	boneState **curFrame = anim->frames;
	if(curFrame != NULL){
		boneState **lastFrame = &anim->frames[anim->frameData.numFrames];
		//Free each bone state!
		for(; curFrame < lastFrame; ++curFrame){
			boneState *curFrameValue = *curFrame;
			if(curFrameValue != NULL){
				memoryManagerGlobalFree(curFrameValue);
			}
		}
		//Now free the array!
		memoryManagerGlobalFree(anim->frames);
	}
}

void skeleAnimStateDelete(void *s){
	skeleAnimState *animState = (skeleAnimState *)s;
	if(animState->blendAnim != NULL){
		skeleAnimInstDelete(animState->blendAnim);
	}

	if(animState->lookup != NULL){
		memoryManagerGlobalFree(animState->lookup);
	}

	if(animState->skeleState != NULL){
		memoryManagerGlobalFree(animState->skeleState);
	}
}

void skeleAnimInstDelete(skeletonAnimInst *animInst){
	stateObjDelete(animInst, &skeleAnimStateDelete);
}

void skeleObjDelete(skeletonObject *skeleObj){
	if(skeleObj->anims != NULL){
		skeletonAnimInst *curAnim = skeleObj->anims;
		skeletonAnimInst *lastAnim = &skeleObj->anims[skeleObj->numAnims];
		//Free each animation instance!
		for(; curAnim < lastAnim; ++curAnim){
			stateObjDelete(curAnim, &skeleAnimStateDelete);
		}
		//Now free the array!
		memoryManagerGlobalFree(skeleObj->anims);
	}

	if(skeleObj->state != NULL){
		memoryManagerGlobalFree(skeleObj->state);
	}
}


//Invert a bone's state!
static void boneStateInvert(const boneState *b, boneState *out){
	//Invert the bone's rotation!
	quatConjugateFast(&b->rot, &out->rot);

	//Invert its position with respect to the new rotation!
	quatApplyRotationFast(&b->rot, &b->pos, &out->pos);
	vec3Negate(&out->pos);

	//Invert its scale by storing the reciprocal of each value!
	vec3DivideSBy(&b->scale, 1.f, &out->scale);
}


//Set a bone's state to a position between two states!
static void updateBoneStateSet(const boneState *s1, const boneState *s2, const float time, boneState *out){
	//Interpolate between the current frame and the next one!
	vec3Lerp(&s1->pos, &s2->pos, time, &out->pos);
	quatSlerpFast(&s1->rot, &s2->rot, time, &out->rot);
	vec3Lerp(&s1->scale, &s2->scale, time, &out->scale);
}

//Set a bone's state to a position between two states!
static void updateBoneStateAdd(const boneState *s1, const boneState *s2, const float time, boneState *out){
	vec3 newPos;
	quat newRot;
	vec3 newScale;

	//Interpolate between the current frame and the next one!
	vec3Lerp(&s1->pos, &s2->pos, time, &newPos);
	quatSlerpFast(&s1->rot, &s2->rot, time, &newRot);
	vec3Lerp(&s1->scale, &s2->scale, time, &newScale);

	//Add the new offsets to our "out" state!
	vec3AddVec3(&out->pos, &newPos, &out->pos);
	quatMultiplyQuat(&out->rot, &newRot, &out->rot);
	vec3AddVec3(&out->scale, &newPos, &out->scale);
}


//Find the bone states to blend between and store pointers to them in "startState" and "endState"!
static void getBlendStates(const skeleAnimState *animState, const skeleAnimState *blendState, const size_t boneNum,
                           const boneState *defState, const boneState **startState, const boneState **endState){

	//If this entity bone appears in the first animation,
	//start blending from the first animation's state!
	if(animState->lookup == NULL){
		*startState = &animState->skeleState[boneNum];
	}else{
		const size_t lookupID = animState->lookup[boneNum];

		if(lookupID < animState->anim->numBones){
			*startState = &animState->skeleState[lookupID];

		//Otherwise, blend from the default state that was specified.
		}else{
			*startState = defState;
		}
	}

	//If this entity bone appears in the second
	//animation, blend to the second animation's state!
	if(blendState->lookup == NULL){
		*endState = &blendState->skeleState[boneNum];
	}else{
		const size_t lookupID = blendState->lookup[boneNum];

		if(lookupID < blendState->anim->numBones){
			*endState = &blendState->skeleState[lookupID];

		//Otherwise, blend to the state of the
		//bone before this animation was applied.
		}else{
			*endState = defState;
		}
	}
}