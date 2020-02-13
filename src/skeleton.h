#ifndef skeleton_h
#define skeleton_h


#include "utilTypes.h"
#include "vec3.h"
#include "quat.h"
#include "mat4.h"
#include "transform.h"

#include "animation.h"
#include "state.h"


#define SKELETON_MAX_BONES 128


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

	// Stores data relating to the animation's playback.
	animationData animData;
	// Progress through the current frame.
	float interpTime;
	// Animation weight factor.
	float intensity;
} skeletonAnim;


// Stores skeleton data for objects.
typedef struct skeletonObject {
	skeleton *skele;
	// Animations are stored in a singleList.
	skeletonAnim *anims;
	boneState *bones;
} skeletonObject;


void boneInit(bone *const restrict bone, char *const restrict name, const size_t parent, const boneState *const restrict state);
void skeleInit(skeleton *const restrict skele);
void skeleInitSet(
	skeleton *const restrict skele, const char *const restrict name,
	const size_t nameLength, bone *const restrict bones, const size_t numBones
);
void skeleAnimDefInit(skeletonAnimDef *animDef);
void skeleAnimInit(skeletonAnim *const restrict anim, skeletonAnimDef *const restrict animDef, const float intensity);
void skeleObjInit(skeletonObject *const restrict skeleObj, skeleton *const restrict skele);

skeletonAnimDef *skeleAnimSMDLoad(const char *const restrict skeleAnimPath, const size_t skeleAnimPathLength);

void skeleAnimUpdate(skeletonAnim *const restrict anim, const float time);
void skeleObjGenerateBoneState(const skeletonObject *const restrict skeleData, const size_t boneID, const char *const restrict boneName);

size_t skeleFindBone(const skeleton *const restrict skele, const char *const restrict name);
size_t skeleAnimFindBone(const skeletonAnim *const restrict skeleAnim, const char *name);

void boneDelete(bone *const restrict bone);
void skeleDelete(skeleton *const restrict skele);
void skeleAnimDefDelete(skeletonAnimDef *const restrict animDef);
void skeleObjDelete(skeletonObject *const restrict skeleObj);


extern skeleton g_skeleDefault;


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