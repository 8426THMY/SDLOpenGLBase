#ifndef skeleton_h
#define skeleton_h


#include <stdint.h>

#include "mat3x4.h"
#include "transform.h"

#include "animation.h"

#include "utilTypes.h"


#define SKELETON_INVALID_BONE_INDEX valueInvalid(boneIndex)

#define SKELETON_MAX_BONES 128


/** Ideally, models and animations should have their own skeletons. **/

// Stores the various positional data for a
// bone as offsets from its parent's data.
typedef transform boneState;
typedef uint_least8_t boneIndex;

typedef struct bone {
	char *name;

	// The bone's default local state (relative to its parent).
	boneState localBind;
	// Transformation from global space to the bone's local space.
	mat3x4 invGlobalBind;
	// Stores the index of this bone's parent. In the case of the root
	// bone, we store -1. We never have to worry about a bone whose parent
	// has this index, since bones are always stored after their parents.
	boneIndex parent;
} bone;

typedef struct skeleton {
	char *name;

	// Vector of bones that form the skeleton.
	bone *bones;
	boneIndex numBones;
} skeleton;


typedef struct skeletonAnimDef {
	char *name;

	// Stores the total number of frames and the timestamp for each frame.
	animationFrameData frameData;

	char **boneNames;
	// Array of frames, each stored as an array of bone states in
	// local space, relative to their parents' states. We assume
	// every bone in the animation has the same number of keyframes.
	boneState **frames;
	boneIndex numBones;
} skeletonAnimDef;

// Stores data for an entity-specific instance of an animation.
typedef struct skeletonAnim {
	// Pointer to the animation being used.
	skeletonAnimDef *animDef;

	// For each bone in the owner's skeleton, store the
	// index of the corresponding bone in the animation.
	boneIndex lookup[SKELETON_MAX_BONES];

	// Stores data relating to the animation's playback.
	animationData animData;
	// Progress through the current frame.
	float interpTime;
	// Animation weight factor.
	float intensity;
} skeletonAnim;


// Stores skeleton data for objects.
typedef struct skeletonState {
	// Animations are stored in a singleList.
	skeletonAnim *anims;
	boneState *states;
	boneState *prevStates;
	// Interpolated transform matrices stored in
	// local space. These are used during rendering.
	mat3x4 *interpStates;
} skeletonState;


void boneInit(
	bone *const restrict bone,
	char *const restrict name, const boneIndex parent,
	const mat3x4 *const restrict invGlobalBind
);
void skeleInit(skeleton *const restrict skele);
void skeleInitSet(
	skeleton *const restrict skele, const char *const restrict name,
	const size_t nameLength, bone *const restrict bones, const boneIndex numBones
);
void skeleAnimDefInit(skeletonAnimDef *animDef);
void skeleAnimInit(
	skeletonAnim *const restrict anim, skeletonAnimDef *const restrict animDef,
	const skeleton *const restrict skele, const float speed, const float intensity
);
void skeleStateInit(skeletonState *const restrict skeleState, const skeleton *const restrict skele);

skeletonAnimDef *skeleAnimSMDLoad(const char *const restrict skeleAnimPath, const size_t skeleAnimPathLength);

void skeleAnimUpdate(skeletonAnim *const restrict anim, const float dt);
void skeleStateUpdate(skeletonState *const restrict skeleState, const float dt);
void skeleStatePrependAnimations(
	const skeletonState *const restrict skeleState,
	const boneIndex boneID, const boneState *const restrict localBind,
	boneState *const restrict state
);

boneIndex skeleFindBone(const skeleton *const restrict skele, const char *const restrict name);
boneIndex skeleAnimDefFindBone(const skeletonAnimDef *const restrict animDef, const char *const restrict name);

void boneDelete(bone *const restrict bone);
void skeleDelete(skeleton *const restrict skele);
void skeleAnimDefDelete(skeletonAnimDef *const restrict animDef);
void skeleStateDelete(skeletonState *const restrict skeleState);


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