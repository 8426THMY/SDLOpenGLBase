#ifndef skeleton_h
#define skeleton_h


#include <stdlib.h>

#include "utilTypes.h"
#include "vec3.h"
#include "quat.h"
#include "mat4.h"
#include "transform.h"

#include "animation.h"
#include "state.h"


#define MAX_BONES_NUM 128


/** Ideally, models and animations should have their own skeletons. **/

// Stores the various positional data for a
// bone as offsets from its parent's data.
typedef transformState boneState;

typedef struct bone {
	char *name;

	// Inverse of the bone's default, accumulative state.
	boneState state;
	// Stores the index of this bone's parent.
	size_t parent;
} bone;

typedef struct skeleton {
	char *name;

	// Vector of bones that form the skeleton.
	bone *bones;
	size_t numBones;
} skeleton;


typedef struct skeletonAnim {
	char *name;

	// Stores the total number of frames and the timestamp for each frame.
	animationFrameData frameData;

	char **boneNames;
	// Vector of frames, where each frame has a vector of bone states.
	// Note: Every bone should have the same number of keyframes. When we load
	// an animation where this doesn't hold true, we just fill in the blanks.
	boneState **frames;
	size_t numBones;
} skeletonAnim;


typedef stateObject skeletonAnimInst;

// Stores data for an entity-specific instance of an animation.
typedef struct skeletonAnimState {
	// Pointer to the animation being used.
	skeletonAnim *anim;

	// Stores data relating to the animation.
	animationData animData;
	// This is a number between 0 and 1 that tells us how far through the current frame we are.
	float interpTime;

	// Stores a pointer to an animation instance that this one is blending to, or NULL if it isn't.
	skeletonAnimInst *blendAnim;
	// How far we are through the blend.
	float blendTime;

	// Stores which the ID of the animation bone that each entity bone relates to.
	// If the lookup is a NULL pointer, the animation uses the same skeleton as the entity.
	size_t *lookup;

	// Stores the current state of each bone.
	boneState *skeleState;
} skeletonAnimState;


// Every object that has a skeleton should have one of these.
typedef struct skeletonObject {
	// Pointer to the skeleton that this object uses.
	skeleton *skele;

	// Vector of animation instances that this object is playing.
	skeletonAnimInst *anims;
	size_t numAnims;

	// The current transformed state of the object.
	boneState *state;
} skeletonObject;


void boneInit(bone *bone, char *name, const size_t parent, const boneState *state);
void skeleInit(skeleton *skele);
void skeleInitSet(skeleton *skele, const char *name, const size_t nameLength, bone *bones, const size_t numBones);
void skeleAnimInit(skeletonAnim *anim);
void skeleAnimStateInit(skeletonAnimState *animState);
void skeleAnimInstInit(skeletonAnimInst *animInst);
void skeleObjInit(skeletonObject *skeleObj, skeleton *skele);

return_t skeleAnimLoadSMD(skeletonAnim *skeleAnim, const char *skeleAnimName);

void skeleAnimInstUpdate(skeletonAnimState *animState, const float time);
void skeleAnimStateBlendSet(skeletonObject *skeleObj, const skeletonAnimState *animState, const skeletonAnimState *blendState);
void skeleAnimStateBlendAdd(skeletonObject *skeleObj, const skeletonAnimState *animState, const skeletonAnimState *blendState);

void skeleObjAddAnim(skeletonObject *skeleObj, skeletonAnim *anim);
void skeleObjGenerateRenderState(skeletonObject *skeleObj);

void boneDelete(bone *bone);
void skeleDelete(skeleton *skele);
void skeleAnimDelete(skeletonAnim *anim);
void skeleAnimStateDelete(void *s);
void skeleAnimInstDelete(skeletonAnimInst *animInst);
void skeleObjDelete(skeletonObject *skeleObj);


extern skeleton errorSkele;


/*
// This is the animation-related stuff that the entity will store.
typedef struct entity {
	// Stuff

	// Pointer to the entity's skeleton.
	skeleton *skele;
	// Vector of animation instances.
	skeletonAnimInst *anims;
	size_t numAnims;
	// Combination of all the entity's animStates.
	boneState *globalState;
} entity;
*/


/*
void entityAnimate(entity *ent){
	size_t i;
	// Update all of the entity's animations!
	for(i = 0; i < ent->numAnims; i++){
		skeleAnimInstAnimate(&(ent->anims[i]));
	}

	// Update the entity's bones!
	skeletonAnimInstCreateGlobalAnimState(ent->anims, ent->numAnims);
}

void skeleAnimInstAnimate(skeletonAnimInst *animInst){
	size_t i;
	// Update all of the animation's bones!
	for(i = 0; i < animInst->anim->numBones; i++){
		animationUpdate(&(animInst->boneAnims[i]), &(animInst->anim->bones[i].frameData));
		animateBone(animInst, i);
	}
}
*/


#endif