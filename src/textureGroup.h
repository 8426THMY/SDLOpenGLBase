#ifndef textureGroup_h
#define textureGroup_h


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"

#include "rectangle.h"
#include "animation.h"
#include "texture.h"


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
	textureGroupAnim texGroupAnim;
} textureGroupState;


void texGroupFrameInit(textureGroupFrame *texGroupFrame);
void texGroupAnimDefInit(textureGroupAnimDef *texGroupAnimDef);
void texGroupInit(textureGroup *texGroup);
void texGroupStateInit(textureGroupState *texGroupState, const textureGroup *texGroup);

return_t texGroupLoad(textureGroup *texGroup, const char *texGroupName);

void texGroupStateUpdate(textureGroupState *texGroupState, const float time);
GLuint texGroupStateGetFrame(const textureGroupState *texGroupState, const GLuint uvOffsetsID);

void texGroupAnimDefDelete(textureGroupAnimDef *texGroupAnimDef);
void texGroupDelete(textureGroup *texGroup);

size_t texGroupFindAnimNameIndex(const textureGroup *texGroup, const char *name);
size_t texGroupFindNameIndex(const char *name);


extern textureGroup texGroupDefault;


#endif