#include "skeleton.h"


#include "programSettings.h"

#include "utilString.h"


#warning "Store the skeleObject normally. You need to find a way to indicate that two animations are blending that works when you copy to the next state!"
#warning "We still need to do animation blending and create bone lookups in case the animation's bones don't match the model's skeleton's (such as with the Scout)."
vector loadedSkeleAnims;


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

	size_t i;
	//Make sure we invert each bone's state!
	for(i = 0; i < skele->numBones; ++i){
		boneStateInvert(&skele->bones[i].state, &skele->bones[i].state);
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
		skeleObj->state = malloc(skele->numBones * sizeof(*skeleObj->state));
	}else{
		skeleObj->state = NULL;
	}
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

		size_t i;
		//Update each bone's state!
		for(i = 0; i < animState->anim->numBones; ++i){
			updateBoneStateSet(&animState->anim->frames[animState->animData.currentFrame][i],
			                   &animState->anim->frames[animState->animData.nextFrame][i],
			                   animState->interpTime, &animState->skeleState[i]);
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

void skeleAnimStateBlendSet(skeletonObject *skeleObj, const skeleAnimState *animState, const skeleAnimState *blendState){
	size_t i;
	//Loop through all the bones in the entity (or whatever is using the blended animations).
	for(i = 0; i < skeleObj->skele->numBones; ++i){
		const boneState *startState, *endState;
		//If this is the first blend we've applied, we'll
		//need to blend to and from the bone's current state.
		getBlendStates(animState, blendState, i, &skeleObj->state[i], &startState, &endState);

		//Blend between the two states and update the bone!
		updateBoneStateSet(startState, endState, animState->blendTime, &skeleObj->state[i]);
	}
}

/** This function is a duplicate of the one above it, but the last line and default state things are different. **/
void skeleAnimStateBlendAdd(skeletonObject *skeleObj, const skeleAnimState *animState, const skeleAnimState *blendState){
	size_t i;
	//Loop through all the bones in the entity (or whatever is using the blended animations).
	for(i = 0; i < skeleObj->skele->numBones; ++i){
		const boneState *startState, *endState;
		//If we're appending this to another blend, we'll need to blend to and
		//from the default state by default, as it applies no transformations.
		getBlendStates(animState, blendState, i, &defaultBone.state, &startState, &endState);

		//Blend between the two states and update the bone!
		updateBoneStateAdd(startState, endState, animState->blendTime, &skeleObj->state[i]);
	}
}


//Add an animation to the skeletonObject!
void skeleObjAddAnim(skeletonObject *skeleObj, skeletonAnim *anim){
	++skeleObj->numAnims;
	skeleObj->anims = realloc(skeleObj->anims, skeleObj->numAnims * sizeof(*skeleObj->anims));
	skeleAnimInstInit(&skeleObj->anims[skeleObj->numAnims - 1]);

	skeleAnimState *animState = skeleObj->anims[0].states[0];
	animState->anim = anim;
	animState->skeleState = malloc(anim->numBones * sizeof(*animState->skeleState));
}

//Merge a bone's state with that of its parent
//and multiply it by the inverse bind pose state!
/** Would it be better to use matrices? Merging bone states seems significantly faster   **/
/** without them, but applying transformations to a vec3 using them is slower. In other  **/
/** words, we're doing less work on the CPU and sending less stuff to the shader, but    **/
/** the shader is doing quite a bit more work per vertex than if we were using matrices. **/
/*
boneStateAddTransform
*40
+15
-12

boneStateAddTransform (with quatNormalizeQuat)
*52
+18
-14


Note: These can probably be optimised for transformation matrices.
mat4Translate (not required if we init as a translation matrix, which only uses sets)
*16
+12

mat4RotateQuat
*54
+30
-5

mat4Scale
*16

mat4Multiply
*64
+48

mat4CreateTransformMatrix
*70
+30
-5

mat4AddTransform
*134
+78
-5
*/
//Accumulate the transformations of every animation!
void skeleObjGenerateRenderState(skeletonObject *skeleObj){
	boneState *animState = ((skeleAnimState *)skeleObj->anims[0].states[0])->skeleState;

	size_t i;
	//Accumulate the transformations for each bone in the animation!
	for(i = 0; i < skeleObj->skele->numBones; ++i){
		//If this bone has a parent, add its animation transformations to those of its parent!
		if(skeleObj->skele->bones[i].parent != -1){
			boneStateAddTransform(&skeleObj->state[skeleObj->skele->bones[i].parent], &animState[i], &skeleObj->state[i]);

		//Otherwise, just use it by itself!
		}else{
			skeleObj->state[i] = animState[i];
		}
	}

	//Append each bone's inverse reference state!
	for(i = 0; i < skeleObj->skele->numBones; ++i){
		boneStateAddTransform(&skeleObj->state[i], &skeleObj->skele->bones[i].state, &skeleObj->state[i]);
	}
}


void boneDelete(bone *bone){
	if(bone->name != NULL){
		free(bone->name);
	}
}

void skeleDelete(skeleton *skele){
	if(skele->name != NULL){
		free(skele->name);
	}

	//Make sure we don't free the bone vector if its set to "defaultBone"!
	if(skele->bones != NULL && skele->bones != &defaultBone){
		free(skele->bones);
	}
}

void skeleAnimDelete(skeletonAnim *anim){
	if(anim->name != NULL){
		free(anim->name);
	}

	animFrameDataClear(&anim->frameData);

	if(anim->boneNames != NULL){
		size_t i;
		//Free each bone name!
		for(i = 0; i < anim->numBones; ++i){
			if(anim->boneNames[i] != NULL){
				free(anim->boneNames[i]);
			}
		}
		//Now free the array!
		free(anim->boneNames);
	}
	if(anim->frames != NULL){
		size_t i;
		//Free each bone state!
		for(i = 0; i < anim->frameData.numFrames; ++i){
			if(anim->frames[i] != NULL){
				free(anim->frames[i]);
			}
		}
		//Now free the array!
		free(anim->frames);
	}
}

void skeleAnimStateDelete(void *s){
	skeleAnimState *animState = (skeleAnimState *)s;
	if(animState->blendAnim != NULL){
		skeleAnimInstDelete(animState->blendAnim);
	}

	if(animState->lookup != NULL){
		free(animState->lookup);
	}

	if(animState->skeleState != NULL){
		free(animState->skeleState);
	}
}

void skeleAnimInstDelete(skeletonAnimInst *animInst){
	stateObjDelete(animInst, &skeleAnimStateDelete);
}

void skeleObjDelete(skeletonObject *skeleObj){
	if(skeleObj->anims != NULL){
		size_t i;
		//Free each animation instance!
		for(i = 0; i < skeleObj->numAnims; ++i){
			stateObjDelete(&skeleObj->anims[i], &skeleAnimStateDelete);
		}
		//Now free the array!
		free(skeleObj->anims);
	}

	if(skeleObj->state != NULL){
		free(skeleObj->state);
	}
}


//Invert a bone's state!
static void boneStateInvert(const boneState *b, boneState *out){
	//Invert the bone's rotation!
	quatConjugateFast(&b->rot, &out->rot);

	//Invert its position with respect to the new rotation!
	quatApplyRotationFast(&b->rot, &b->pos, &out->pos);
	vec3Negate(&out->pos, &out->pos);

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

	//If this entity bone appears in the first animation, start blending from the first animation's state!
	if(animState->lookup == NULL){
		*startState = &animState->skeleState[boneNum];
	}else if(animState->lookup[boneNum] < animState->anim->numBones){
		*startState = &animState->skeleState[animState->lookup[boneNum]];

	//Otherwise, blend from the default state that was specified.
	}else{
		*startState = defState;
	}

	//If this entity bone appears in the second animation, blend to the second animation's state!
	if(blendState->lookup == NULL){
		*endState = &blendState->skeleState[boneNum];
	}else if(blendState->lookup[boneNum] < blendState->anim->numBones){
		*endState = &blendState->skeleState[blendState->lookup[boneNum]];

	//Otherwise, blend to the state of the bone before this animation was applied.
	}else{
		*endState = defState;
	}
}


/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
//Load an SMD animation! This is temporary and should ideally be merged with the function in model.c or removed.
size_t skeleAnimLoadSMD(const char *skeleAnimName){
	size_t index = loadedSkeleAnims.size;


	//Create and initialize the animation!
	skeletonAnim skeleAnim;
	skeleAnimInit(&skeleAnim);


	//Find the full path for the model!
	const size_t skeleAnimNameLength = strlen(skeleAnimName);
	char *skeleAnimPath = malloc(18 + skeleAnimNameLength + 1);
	memcpy(skeleAnimPath, ".\\resource\\models\\", 18);
	strcpy(skeleAnimPath + 18, skeleAnimName);

	//Load the textureGroup!
	FILE *skeleAnimFile = fopen(skeleAnimPath, "r");
	if(skeleAnimFile != NULL){
		size_t tempCapacity = 1;
		//Temporarily stores bones.
		size_t tempBonesSize = 0;
		bone *tempBones = malloc(tempCapacity * sizeof(*tempBones));

		/** Ideally, we shouldn't have to do this. **/
		skeleAnim.frames = malloc(sizeof(*skeleAnim.frames));
		skeleAnim.frameData.time = malloc(sizeof(*skeleAnim.frameData.time));

		//This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		//This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while((line = readLineFile(skeleAnimFile, &lineBuffer[0], &lineLength)) != NULL && index != -1){
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;

				//If this isn't the version number and the line isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf("Error loading skeletal animtion!\n"
					       "Path: %s\n"
					       "Animation ID: %u\n"
					       "Line: %s\n"
					       "Error: Unexpected identifier!\n", skeleAnimPath, index, line);
					index = -1;
				}
			}else{
				if(strcmp(line, "end") == 0){
					//If we've finished identifying the skeleton's bones, copy the data into our animation object!
					if(dataType == 1){
						tempBones = realloc(tempBones, tempBonesSize * sizeof(*tempBones));
						skeleAnim.boneNames = malloc(tempBonesSize * sizeof(*skeleAnim.boneNames));
						skeleAnim.numBones = tempBonesSize;

						size_t i;
						//Fill the array of bone names!
						for(i = 0; i < tempBonesSize; ++i){
							skeleAnim.boneNames[i] = tempBones[i].name;
						}

						tempCapacity = 1;

					//If we've finished loading the animation, shrink the vectors!
					}else if(dataType == 2){
						skeleAnim.frameData.time = realloc(skeleAnim.frameData.time, skeleAnim.frameData.numFrames * sizeof(*skeleAnim.frameData.time));
						skeleAnim.frames = realloc(skeleAnim.frames, skeleAnim.frameData.numFrames * sizeof(*skeleAnim.frames));

						skeleAnim.frameData.playNum = -1;
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
							//Get the bone's name.
							size_t boneNameLength;
							getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos, &boneNameLength);
							tempBone.name = malloc(boneNameLength + 1);
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);


							//If we're out of space, allocate some more!
							if(tempBonesSize >= tempCapacity){
								tempCapacity = tempBonesSize * 2;
								tempBones = realloc(tempBones, tempCapacity * sizeof(*tempBones));
							}
							//Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf("Error loading skeletal animtion!\n"
							       "Path: %s\n"
							       "Animation ID: %u\n"
							       "Line: %s\n"
							       "Error: Found node %u when expecting node %u!\n",
							       skeleAnimPath, index, line, boneID, tempBonesSize);
							index = -1;
						}
					}else if(dataType == 2){
						//If the line begins with time, get the frame's timestamp!
						if(memcmp(line, "time ", 5) == 0){
							unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;

								//Allocate memory for the new frame if we have to!
								if(skeleAnim.frameData.numFrames >= tempCapacity){
									tempCapacity = skeleAnim.frameData.numFrames * 2;
									//Resize the time array!
									skeleAnim.frameData.time = realloc(skeleAnim.frameData.time, tempCapacity * sizeof(*skeleAnim.frameData.time));
									//Resize the frames array!
									skeleAnim.frames = realloc(skeleAnim.frames, tempCapacity * sizeof(*skeleAnim.frames));
								}
								//Allocate memory for each bone state in this frame!
								skeleAnim.frames[skeleAnim.frameData.numFrames] = malloc(skeleAnim.numBones * sizeof(**skeleAnim.frames));

								skeleAnim.frameData.time[skeleAnim.frameData.numFrames] = (data + 1) * (1000.f / 24.f);
								++skeleAnim.frameData.numFrames;
							}else{
								printf("Error loading skeletal animtion!\n"
								       "Path: %s\n"
								       "Animation ID: %u\n"
								       "Line: %s\n"
								       "Error: Frame timestamps do not increment sequentially!\n",
								       skeleAnimPath, index, line);
								index = -1;
							}

						//Otherwise, we're setting the bone's state!
						}else{
							char *tokPos = line;

							//Get this bone's ID.
							size_t boneID = strtoul(tokPos, &tokPos, 10);
							if(boneID < tempBonesSize){
								boneState *currentState = &skeleAnim.frames[skeleAnim.frameData.numFrames - 1][boneID];

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
								printf("Error loading skeletal animtion!\n"
									   "Path: %s\n"
									   "Animation ID: %u\n"
									   "Line: %s\n"
									   "Error: Found skeletal data for bone %u, which doesn't exist!\n",
									   skeleAnimPath, index, line, boneID);
								index = -1;
							}
						}
					}
				}
			}
		}


		//We don't delete them properly because we store the bone names elsewhere.
		if(tempBones != NULL){
			free(tempBones);
		}


		//If there wasn't an error, add it to the vector!
		if(index != -1){
			skeleAnim.name = malloc(skeleAnimNameLength + 1);
			strcpy(skeleAnim.name, skeleAnimName);

			vectorAdd(&loadedSkeleAnims, &skeleAnim, 1);

		//Otherwise, delete the animation.
		}else{
			skeleAnimDelete(&skeleAnim);
		}


		fclose(skeleAnimFile);
	}else{
		printf("Unable to open skeletal animation file!\n"
		       "Path: %s\n"
		       "Animation ID: %u\n", skeleAnimPath, index);
		index = -1;
	}

	if(skeleAnimPath != NULL){
		free(skeleAnimPath);
	}


	return(index);
}