#ifndef textureGroup_h
#define textureGroup_h


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"

#include "animation.h"
#include "texture.h"


//Stores the data for a frame!
typedef struct textureGroupFrame {
	//The OpenGL ID of the texture used by this frame.
	size_t texID;

	//Various frame data.
	float x;
	float y;
	float width;
	float height;
} textureGroupFrame;

//Stores the data for an animation!
typedef struct textureGroupAnimDef {
	char *name;

	//Stores all the frames used by the animation!
	textureGroupFrame *animFrames;
	//Stores the total number of frames and when each frame ends!
	animationFrameData frameData;
} textureGroupAnimDef;

typedef struct textureGroup {
	char *name;

	//Stores all the animations for the textureGroup!
	textureGroupAnimDef *texAnims;
	size_t numAnims;
} textureGroup;

//Stores the data for a single instance of an animated texture.
typedef struct textureGroupAnim {
	#warning "We don't need to store its position anymore."
	size_t texGroupPos;
	animationData animData;
} textureGroupAnim;


void texGroupFrameInit(textureGroupFrame *texGroupFrame);
void texGroupAnimDefInit(textureGroupAnimDef *texGroupAnimDef);
void texGroupInit(textureGroup *texGroup);
void texGroupAnimInit(textureGroupAnim *texGroupAnim, const size_t texGroupPos);

return_t texGroupLoad(textureGroup *texGroup, const char *texGroupName);

void texGroupAnimSetAnim(textureGroupAnim *texGroupAnim, const size_t currentAnim);
void texGroupAnimUpdateAnim(textureGroupAnim *texGroupAnim, const float time);
GLuint texGroupGetFrame(const textureGroup *texGroup, const size_t currentAnim, const size_t currentFrame, const GLuint uvOffsetsID);

void texGroupAnimDefDelete(textureGroupAnimDef *texGroupAnimDef);
void texGroupDelete(textureGroup *texGroup);

size_t texGroupFindAnimNameIndex(const textureGroup *texGroup, const char *name);
size_t texGroupFindNameIndex(const char *name);


extern textureGroup errorTexGroup;


#endif