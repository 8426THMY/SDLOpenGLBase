#ifndef textureGroup_h
#define textureGroup_h


#include <stddef.h>

#include "utilTypes.h"

#include "rectangle.h"
#include "animation.h"
#include "texture.h"


/** IMPORTANT! **/
#warning "Everything in here (and possibly even regular textures) needs a complete rewrite."
#warning "Texture group is a dumb name, and they shouldn't have multiple animations."
#warning "Maybe do something similar to (but simpler than) the Source engine?"

/**
1. Replace textureGroups/textureWrappers with materials.
2. Materials include one or more base textures.
3. Each base texture can have at most one normal map, bump map and specular map.
4. Materials must have one and only one animation.
5. Materials may also have support for scrolling that is interpolated during rendering.
**/


// Stores the data for a frame!
typedef struct textureGroupFrame {
	const texture *tex;
	rectangle bounds;
} textureGroupFrame;

#warning "Can we allocate frame times and animation frames in one memory manager call?"
// Stores the data for an animation!
typedef struct textureGroupAnimDef {
	char *name;

	// Stores all the frames used by the animation!
	textureGroupFrame *animFrames;
	// Stores the total number of frames and when each frame ends!
	animationFrameData frameData;
} textureGroupAnimDef;

// For now, textureGroupAnims don't need to be their own structure.
typedef struct animationData textureGroupAnim;

typedef struct textureGroup {
	char *name;

	#warning "This should probably have its own allocator."
	// Stores all the animations for the textureGroup!
	textureGroupAnimDef *texAnims;
	size_t numAnims;
} textureGroup;

typedef struct textureGroupState {
	const textureGroup *texGroup;

	size_t currentAnim;
	textureGroupAnim texGroupAnim;
} textureGroupState;


void texGroupFrameInit(textureGroupFrame *const restrict texGroupFrame);
void texGroupAnimDefInit(textureGroupAnimDef *const restrict texGroupAnimDef);
void texGroupInit(textureGroup *const restrict texGroup);
void texGroupStateInit(textureGroupState *const restrict texGroupState, const textureGroup *const restrict texGroup);

textureGroup *texGroupLoad(const char *const restrict texGroupPath, const size_t texGroupPathLength);

void texGroupStateUpdate(textureGroupState *const restrict texGroupState, const float time);
textureGroupFrame *texGroupStateGetFrame(const textureGroupState *const restrict texGroupState);

void texGroupAnimDefDelete(textureGroupAnimDef *const restrict texGroupAnimDef);
void texGroupDelete(textureGroup *const restrict texGroup);


extern textureGroup g_texGroupDefault;


#endif