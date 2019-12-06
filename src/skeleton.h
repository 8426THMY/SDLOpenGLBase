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

	// The bone's default local state.
	boneState localBind;
	// Inverse of the bone's default, accumulative state.
	boneState invGlobalBind;
	// Stores the index of this bone's parent.
	size_t parent;
} bone;

typedef struct skeleton {
	char *name;

	// Vector of bones that form the skeleton.
	bone *bones;
	size_t numBones;
} skeleton;


typedef struct skeletonAnimDef {
	char *name;

	// Stores the total number of frames and the timestamp for each frame.
	animationFrameData frameData;

	char **boneNames;
	// Vector of frames, where each frame has a vector of bone states.
	//
	// Note: Every bone should have the same number of keyframes. When we load
	// an animation where this doesn't hold, we just fill in the blanks.
	boneState **frames;
	size_t numBones;
} skeletonAnimDef;

// Stores data for an entity-specific instance of an animation.
typedef struct skeletonAnim {
	// Pointer to the animation being used.
	skeletonAnimDef *animDef;

	// Stores data relating to the animation.
	animationData animData;
	// This is a number between 0 and 1 that tells us how far through the current frame we are.
	float interpTime;
	// This is a number between 0 and 1 that tells us how much the animation should affect the bones.
	float intensity;

	// Stores the ID of the animation bone that each entity bone relates to.
	// If the lookup is a NULL pointer, the animation uses the same skeleton as the entity.
	size_t *lookup;

	// Stores the current state of each bone.
	/** Temporary. We can remove this by merging bone's     **/
	/** transformations with their parent's transformations **/
	/** when we're loading animations.                      **/
	boneState *skeleState;
} skeletonAnim;


void boneInit(bone *bone, char *name, const size_t parent, const boneState *state);
void skeleInit(skeleton *skele);
void skeleInitSet(skeleton *skele, const char *name, const size_t nameLength, bone *bones, const size_t numBones);
void skeleAnimDefInit(skeletonAnimDef *animDef);
void skeleAnimInit(skeletonAnim *anim, skeletonAnimDef *animDef);

return_t skeleAnimLoadSMD(skeletonAnimDef *skeleAnim, const char *skeleAnimName);

void skeleAnimUpdate(skeletonAnim *anim, const skeleton *skele, const float time, boneState *bones);

void boneDelete(bone *bone);
void skeleDelete(skeleton *skele);
void skeleAnimDefDelete(skeletonAnimDef *animDef);
void skeleAnimDelete(skeletonAnim *anim);


extern skeleton errorSkele;


/*
// This is the animation-related stuff that the entity will store.
typedef struct entity {
	// Stuff

	// Pointer to the entity's skeleton.
	skeleton *skele;
	// Vector of animation instances.
	skeletonAnim *anims;
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
		skeleAnimAnimate(&(ent->anims[i]));
	}

	// Update the entity's bones!
	skeletonAnimCreateGlobalAnimState(ent->anims, ent->numAnims);
}

void skeleAnimAnimate(skeletonAnim *anim){
	size_t i;
	// Update all of the animation's bones!
	for(i = 0; i < anim->anim->numBones; i++){
		animationUpdate(&(anim->boneAnims[i]), &(anim->anim->bones[i].frameData));
		animateBone(anim, i);
	}
}
*/


#endif