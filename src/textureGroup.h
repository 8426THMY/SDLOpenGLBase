#ifndef textureGroup_h
#define textureGroup_h


#include <stddef.h>

#define GLEW_STATIC
#include <GL/glew.h>

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
3. Each base texture can have at most one normal map and at most one bump map.
4. Materials must have one and only one animation.
5. Materials may also have support for scrolling that is interpolated during rendering.
**/


// Stores the data for a frame!
typedef struct textureGroupFrame {
	const texture *diffuse;
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


void texGroupFrameInit(textureGroupFrame *texGroupFrame);
void texGroupAnimDefInit(textureGroupAnimDef *texGroupAnimDef);
void texGroupInit(textureGroup *texGroup);
void texGroupStateInit(textureGroupState *texGroupState, const textureGroup *texGroup);

return_t texGroupLoad(textureGroup *texGroup, const char *texGroupPath);

void texGroupStateUpdate(textureGroupState *texGroupState, const float time);
textureGroupFrame *texGroupStateGetFrame(const textureGroupState *texGroupState);

void texGroupAnimDefDelete(textureGroupAnimDef *texGroupAnimDef);
void texGroupDelete(textureGroup *texGroup);

size_t texGroupFindAnimNameIndex(const textureGroup *texGroup, const char *name);
size_t texGroupFindNameIndex(const char *name);


extern textureGroup texGroupDefault;


#endif