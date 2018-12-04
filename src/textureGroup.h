#ifndef textureGroup_h
#define textureGroup_h


#define TEXTUREGROUPMODULE


#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include "utilTypes.h"

#include "vector.h"
#include "animation.h"


//Stores the data for a frame!
typedef struct textureGroupFrame {
	//The OpenGL I.D. of the texture used by this frame.
	size_t texID;

	//Various frame data.
	float x;
	float y;
	float width;
	float height;
} textureGroupFrame;

//Stores the data for an animation!
typedef struct textureGroupAnim {
	char *name;

	//Stores all the frames used by the animation!
	textureGroupFrame *animFrames;
	//Stores the total number of frames and when each frame ends!
	animationFrameData frameData;
} textureGroupAnim;

typedef struct textureGroup {
	char *name;

	//Stores all the animations for the textureGroup!
	textureGroupAnim *texAnims;
	size_t numAnims;
} textureGroup;

//Stores the data for a single instance of an animated texture.
typedef struct textureGroupAnimInst {
	size_t texGroupPos;
	animationData animData;
} textureGroupAnimInst;


void texGroupFrameInit(textureGroupFrame *texGroupFrame);
return_t texGroupAnimInit(textureGroupAnim *texGroupAnim);
return_t texGroupInit(textureGroup *texGroup);
void texGroupAnimInstInit(textureGroupAnimInst *animInst, const size_t texGroupPos);

size_t texGroupLoad(const char *texGroupName);

void texGroupAnimInstSetAnim(textureGroupAnimInst *animInst, const size_t currentAnim);
void texGroupAnimInstUpdateAnim(textureGroupAnimInst *animInst, const float time);
GLuint texGroupGetFrame(const textureGroup *texGroup, const size_t currentAnim, const size_t currentFrame, const GLuint uvOffsetsID);

void texGroupAnimDelete(textureGroupAnim *texGroupAnim);
void texGroupDelete(textureGroup *texGroup);

size_t texGroupFindAnimNameIndex(const textureGroup *texGroup, const char *name);
size_t texGroupFindNameIndex(const char *name);


return_t textureGroupModuleSetup();
void textureGroupModuleCleanup();


extern vector loadedTextureGroups;


#endif