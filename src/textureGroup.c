#include "textureGroup.h"


#include <stdio.h>
#include <string.h>

#include "utilString.h"
#include "utilFile.h"
#include "utilMath.h"

#include "memoryManager.h"
#include "moduleTextureGroup.h"


#define TEXTUREGROUP_PATH_PREFIX        "."FILE_PATH_DELIMITER_STR"resource"FILE_PATH_DELIMITER_STR"texgroups"FILE_PATH_DELIMITER_STR
#define TEXTUREGROUP_PATH_PREFIX_LENGTH (sizeof(TEXTUREGROUP_PATH_PREFIX) - 1)

// These must be at least 1!
#define BASE_TEXTURE_CAPACITY    1
#define BASE_ANIM_CAPACITY       1
#define BASE_ANIM_FRAME_CAPACITY 1


#warning "What if we aren't using the global memory manager?"


static textureGroupFrame texGroupFrameDefault = {
	.tex = &g_texDefault,
	.bounds.x = 0.f,
	.bounds.y = 0.f,
	.bounds.w = 1.f,
	.bounds.h = 1.f
};

static textureGroupAnimDef texGroupAnimDefault = {
	.name = "default",

	.animFrames = &texGroupFrameDefault,
	// This defines an animation with only
	// one frame, being the error texture.
	.frameData = {
		.playNum = 0,

		.time = &g_animTimeDefault,
		.numFrames = 1
	}
};

// This texture group is used when the
// real one cannot be found for some reason.
textureGroup g_texGroupDefault = {
	.name = "error",

	.texAnims = &texGroupAnimDefault,
	.numAnims = 1
};
// A lot of structures store arrays of texture group pointers,
// so it's useful to have a double pointer on the stack.
textureGroup *g_texGroupArrayDefault = &g_texGroupDefault;


// Forward-declare any helper functions!
static size_t texGroupAnimFindNameIndex(const textureGroupAnimDef *const restrict texAnims, const size_t numAnims, const char *const restrict name);
static return_t texGroupAnimLoad(
	textureGroupAnimDef *const restrict texGroupAnimDef, FILE *const restrict texGroupFile,
	const char *const restrict *const restrict texPaths, const size_t texPathsSize
);


void texGroupFrameInit(textureGroupFrame *const restrict texGroupFrame){
	texGroupFrame->tex = &g_texDefault;

	texGroupFrame->bounds.x = 0.f;
	texGroupFrame->bounds.y = 0.f;
	texGroupFrame->bounds.w = 1.f;
	texGroupFrame->bounds.h = 1.f;
}

void texGroupAnimDefInit(textureGroupAnimDef *const restrict texGroupAnimDef){
	texGroupAnimDef->name = texGroupAnimDefault.name;

	texGroupAnimDef->animFrames = NULL;
	animFrameDataInit(&texGroupAnimDef->frameData);
}

void texGroupStateInit(textureGroupState *const restrict texGroupState, const textureGroup *const restrict texGroup){
	texGroupState->texGroup = texGroup;

	texGroupState->currentAnim = 0;
	animationInit(&texGroupState->texGroupAnim, ANIMATION_LOOP_INDEFINITELY);
}


/*
** Load the texture group specified by "texGroupPath" and return a pointer to it.
** If the texture group could not be loaded, return a pointer to the default texture group.
*/
textureGroup *texGroupLoad(const char *const restrict texGroupPath, const size_t texGroupPathLength){
	textureGroup *texGroup;

	FILE *texGroupFile;
	char texGroupFullPath[FILE_MAX_PATH_LENGTH];


	// If the texture group has already been loaded, return a pointer to it!
	texGroup = moduleTexGroupFind(texGroupPath);
	if(texGroup != &g_texGroupDefault){
		return(texGroup);
	}


	// Generate the full path for the textureGroup!
	fileGenerateFullResourcePath(
		TEXTUREGROUP_PATH_PREFIX, TEXTUREGROUP_PATH_PREFIX_LENGTH,
		texGroupPath, texGroupPathLength,
		texGroupFullPath
	);


	// Load the textureGroup!
	texGroupFile = fopen(texGroupFullPath, "r");
	if(texGroupFile != NULL){
		size_t texPathsSize = 0;
		size_t texPathsCapacity = BASE_TEXTURE_CAPACITY;
		char **texPaths;

		size_t texAnimsSize = 0;
		size_t texAnimsCapacity = BASE_ANIM_CAPACITY;
		textureGroupAnimDef *texAnims;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		// Store the names of the textures we may need!
		// We only load the textures that are used in an animation.
		texPaths = memoryManagerGlobalAlloc(BASE_TEXTURE_CAPACITY * sizeof(*texPaths));
		if(texPaths == NULL){
			/** MALLOC FAILED **/
		}
		// Store the animations that we've loaded so far!
		texAnims = memoryManagerGlobalAlloc(BASE_ANIM_CAPACITY * sizeof(*texAnims));
		if(texAnims == NULL){
			/** MALLOC FAILED **/
		}


		while((line = fileReadLine(texGroupFile, &lineBuffer[0], &lineLength)) != NULL){
			// Texture or texture sequence.
			if(line[0] == 't'){
				// Texture.
				if(line[1] == ' '){
					char *const texPath = memoryManagerGlobalAlloc((lineLength - 1) * sizeof(*texPath));
					if(texPath == NULL){
						/** MALLOC FAILED **/
					}
					fileParseResourcePath(texPath, &line[2], lineLength - 2, NULL);

					// If we're out of space, allocate some more!
					if(texPathsSize >= texPathsCapacity){
						texPathsCapacity = texPathsSize * 2;
						texPaths = memoryManagerGlobalRealloc(texPaths, texPathsCapacity * sizeof(*texPaths));
						if(texPaths == NULL){
							/** REALLOC FAILED **/
						}
					}

					texPaths[texPathsSize] = texPath;
					++texPathsSize;

				// Texture sequence.
				}else if(memcmp(&line[1], "s ", 2) == 0){
					// Find the first and last indices for the sequence.
					unsigned int curIndex = strtoul(&line[2], &line, 10);
					unsigned int lastIndex = maxUint(strtoul(line, &line, 10), curIndex);

					char *texPath;
					size_t texPathLength;
					char *texExt;
					size_t texExtLength;


					texPath = memoryManagerGlobalAlloc((lineLength - 2) * sizeof(*texPath));
					if(texPath == NULL){
						/** MALLOC FAILED **/
					}
					// Load the base texture's name!
					texPathLength = fileParseResourcePath(
						texPath, line, lineLength - (size_t)(line - lineBuffer), NULL
					) + 1;

					// We insert the texture indices before the file
					// extension, so we need to find where it starts.
					texExt = strrchr(texPath, '.');
					// We only need to move the NULL terminator if there's no file extension.
					if(texExt == NULL){
						texExt = &texPath[texPathLength - 1];
						texExtLength = 1;

					// Otherwise, move everything after the period.
					}else{
						texExtLength = texPathLength - (size_t)(texExt - texPath);
					}


					// Generate and store the name of each texture in the sequence!
					for(; curIndex <= lastIndex; ++curIndex){
						char indexStr[ULONG_MAX_CHARS + 1];
						const size_t indexStrLength = ultostr(curIndex, indexStr);

						// Move the file extension over to create room for the frame number.
						memmove(&texExt[indexStrLength], texExt, texExtLength);
						// Copy the frame number between the texture path and the file extension.
						memcpy(texExt, indexStr, indexStrLength);

						// If we're out of space, allocate some more!
						if(texPathsSize >= texPathsCapacity){
							texPathsCapacity = texPathsSize * 2;
							texPaths = memoryManagerGlobalRealloc(texPaths, texPathsCapacity * sizeof(*texPaths));
							if(texPaths == NULL){
								/** REALLOC FAILED **/
							}
						}
						texPaths[texPathsSize] = memoryManagerGlobalAlloc(texPathLength + indexStrLength);
						memcpy(texPaths[texPathsSize], texPath, texPathLength + indexStrLength);
						++texPathsSize;

						// Move the file extension back to where it was originally.
						memmove(texExt, &texExt[indexStrLength], texExtLength);
					}


					// Don't forget to free the path afterwards.
					memoryManagerGlobalFree(texPath);
				}

			// Animation.
			}else if(line[0] == 'a' && line[lineLength - 1] == '{'){
				// Allocate room for the new animation if we've run out!
				if(texAnimsSize >= texAnimsCapacity){
					texAnimsCapacity = texAnimsSize * 2;
					texAnims = memoryManagerGlobalRealloc(texAnims, texAnimsCapacity * sizeof(*texAnims));
					if(texAnims == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Only keep the animation if it was loaded successfully!
				if(texGroupAnimLoad(&texAnims[texAnimsSize], texGroupFile, (const char *const *const)texPaths, texPathsSize)){
					++texAnimsSize;
				}
			}
		}

		fclose(texGroupFile);


		// We consider a texture group to have been loaded
		// successfully if it references at least one texture.
		if(texPathsSize > 0){
			texGroup = moduleTexGroupAlloc();
			if(texGroup == NULL){
				/** MALLOC FAILED **/
			}

			// Set the texture group's name!
			texGroup->name = memoryManagerGlobalAlloc(texGroupPathLength + 1);
			if(texGroup->name == NULL){
				/** MALLOC FAILED **/
			}
			memcpy(texGroup->name, texGroupPath, texGroupPathLength + 1);

			// If the texture group has no animations, create
			// one that uses the first texture we loaded.
			if(texAnimsSize == 0){
				// Allocate memory for a single default animation.
				texAnims = memoryManagerGlobalResize(texAnims, sizeof(*texAnims));
				if(texAnims == NULL){
					/** REALLOC FAILED **/
				}
				texAnims->animFrames = memoryManagerGlobalAlloc(sizeof(*texAnims->animFrames));
				if(texAnims->animFrames == NULL){
					/** MALLOC FAILED **/
				}

				// Use the default texture group's name.
				texAnims->name = texGroupAnimDefault.name;
				// Initialise its frame data!
				texAnims->frameData.playNum = 0;
				texAnims->frameData.time = &g_animTimeDefault;
				texAnims->frameData.numFrames = 1;
				texGroupFrameInit(texAnims->animFrames);
				// The animation should use the first texture we loaded.
				texAnims->animFrames->tex = textureLoad(*texPaths, strlen(*texPaths));

				texGroup->texAnims = texAnims;
				texGroup->numAnims = 1;

			// Otherwise, resize the animations
			// array so we aren't wasting memory!
			}else{
				texGroup->texAnims = memoryManagerGlobalResize(texAnims, texAnimsSize * sizeof(*texAnims));
				if(texGroup->texAnims == NULL){
					/** REALLOC FAILED **/
				}
				texGroup->numAnims = texAnimsSize;
			}
		}

		// Free each texture path that we read.
		{
			char **curTexPath = texPaths;
			char **lastTexPath = &texPaths[texPathsSize];
			for(; curTexPath < lastTexPath; ++curTexPath){
				memoryManagerGlobalFree(*curTexPath);
			}
			memoryManagerGlobalFree(texPaths);
		}
	}else{
		printf(
			"Unable to open textureGroup!\n"
			"Path: %s\n",
			texGroupFullPath
		);
	}


	return(texGroup);
}


// Continue animating a texture!
void texGroupStateUpdate(textureGroupState *const restrict texGroupState, const float time){
	animationUpdate(&texGroupState->texGroupAnim, &(texGroupState->texGroup->texAnims[texGroupState->currentAnim].frameData), time);
}

// Get the current frame of a texture group instance!
textureGroupFrame *texGroupStateGetFrame(const textureGroupState *const restrict texGroupState){
	const textureGroup *const texGroup = texGroupState->texGroup;
	const textureGroupAnim *const animData = &texGroupState->texGroupAnim;

	// If the animation and frame are valid, get the texture's ID and set the UV offsets!
	if(texGroupState->currentAnim < texGroup->numAnims){
		textureGroupAnimDef *tempAnim = &(texGroup->texAnims[texGroupState->currentAnim]);
		if(animData->currentFrame < tempAnim->frameData.numFrames){
			return(&(tempAnim->animFrames[animData->currentFrame]));
		}
	}

	return(&texGroupFrameDefault);
}


// This function assumes that the input isn't the default animation.
void texGroupAnimDefDelete(textureGroupAnimDef *const restrict texGroupAnimDef){
	if(texGroupAnimDef->name != NULL && texGroupAnimDef->name != texGroupAnimDefault.name){
		memoryManagerGlobalFree(texGroupAnimDef->name);
	}
	if(texGroupAnimDef->animFrames != NULL){
		memoryManagerGlobalFree(texGroupAnimDef->animFrames);
	}
	animFrameDataClear(&texGroupAnimDef->frameData);
}

void texGroupDelete(textureGroup *const restrict texGroup){
	// We can only free this stuff if we
	// are not freeing the error textureGroup.
	if(texGroup != &g_texGroupDefault){
		if(texGroup->name != NULL){
			memoryManagerGlobalFree(texGroup->name);
		}

		textureGroupAnimDef *curAnim = texGroup->texAnims;
		// Delete the textureGroup's animations.
		if(curAnim != NULL && curAnim != &texGroupAnimDefault){
			const textureGroupAnimDef *const lastAnim = &curAnim[texGroup->numAnims];
			do {
				texGroupAnimDefDelete(curAnim);
				++curAnim;
			} while(curAnim < lastAnim);
			memoryManagerGlobalFree(texGroup->texAnims);
		}
	}
}


// Check whether the animation with the name specified has already been loaded.
static size_t texGroupAnimFindNameIndex(const textureGroupAnimDef *const restrict texAnims, const size_t numAnims, const char *const restrict name){
	size_t i;
	for(i = 0; i < numAnims; ++i){
		if(strcmp(name, texAnims[i].name) == 0){
			break;
		}
	}

	return(i);
}

/*
** Load a texture group animation from a file and store the result
** in "animDef". This assumes that the file has already been opened
** and the file pointer has been moved to the correct location.
*/
static return_t texGroupAnimLoad(
	textureGroupAnimDef *const restrict texGroupAnimDef, FILE *const restrict texGroupFile,
	const char *const restrict *const restrict texPaths, const size_t texPathsSize
){

	char lineBuffer[FILE_MAX_LINE_LENGTH];
	char *line;
	size_t lineLength;

	// Most animations will only use one frame.
	size_t animFramesCapacity = BASE_ANIM_FRAME_CAPACITY;
	// Keep track of how long the animation has been going for.
	float playTime = 0.f;

	textureGroupAnimDef tempAnim;
	texGroupAnimDefInit(&tempAnim);
	// We'll need to allocate some space for the animation's frames.
	#warning "Should these be put into the initialization function?"
	tempAnim.animFrames = memoryManagerGlobalAlloc(BASE_ANIM_FRAME_CAPACITY * sizeof(*tempAnim.animFrames));
	if(tempAnim.animFrames == NULL){
		/** MALLOC FAILED **/
	}
	tempAnim.frameData.time = memoryManagerGlobalAlloc(BASE_ANIM_FRAME_CAPACITY * sizeof(*tempAnim.frameData.time));
	if(tempAnim.frameData.time == NULL){
		/** MALLOC FAILED **/
	}


	while((line = fileReadLine(texGroupFile, &lineBuffer[0], &lineLength)) != NULL){
		// Animation name.
		// Make sure we haven't set the name already.
		if(tempAnim.name == texGroupAnimDefault.name && memcmp(line, "n ", 2) == 0){
			const char *animName;
			#warning "We should check if we've already loaded an animation with this name."
			const size_t animNameLength = stringMultiDelimited(&line[2], lineLength - 2, "\" ", &animName);

			tempAnim.name = memoryManagerGlobalAlloc(animNameLength + 1);
			if(tempAnim.name == NULL){
				/** MALLOC FAILED **/
			}

			memcpy(tempAnim.name, animName, animNameLength);
			tempAnim.name[animNameLength] = '\0';

		// Animation default loop count.
		}else if(memcmp(line, "l ", 2) == 0){
			const int playNum = strtol(&line[2], NULL, 10);
			tempAnim.frameData.playNum = (playNum < 0) ? valueInvalid(unsigned int) : playNum;

		// Animation frames.
		}else if(line[0] == 'f'){
			char *tokPos;
			// Every frame format shares these inputs.
			const unsigned int startTex = strtoul(&line[2], &tokPos, 10);
			textureGroupFrame tempAnimFrame = {
				.tex = &g_texDefault,
				.bounds = {
					.x = strtof(tokPos, &tokPos),
					.y = strtof(tokPos, &tokPos),
					.w = strtof(tokPos, &tokPos),
					.h = strtof(tokPos, &tokPos)
				}
			};
			const float delay = strtof(tokPos, &tokPos);

			float texInvWidth  = 1.f / g_texDefault.width;
			float texInvHeight = 1.f / g_texDefault.height;


			// Frame.
			if(line[1] == ' '){
				// Allocate more memory for the new frame
				// and the timestamp if we've run out!
				if(tempAnim.frameData.numFrames >= animFramesCapacity){
					animFramesCapacity = tempAnim.frameData.numFrames * 2;
					tempAnim.animFrames = memoryManagerGlobalRealloc(
						tempAnim.animFrames, animFramesCapacity * sizeof(*tempAnim.animFrames)
					);
					if(tempAnim.animFrames == NULL){
						/** REALLOC FAILED **/
					}
					tempAnim.frameData.time = memoryManagerGlobalRealloc(
						tempAnim.frameData.time, animFramesCapacity * sizeof(*tempAnim.frameData.time)
					);
					if(tempAnim.frameData.time == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Get a pointer to the texture with the specified index.
				// This will load the texture if it hasn't already been loaded.
				if(startTex < texPathsSize){
					const char *const texPath = texPaths[startTex];
					tempAnimFrame.tex = textureLoad(texPath, strlen(texPath));
					texInvWidth  = 1.f / tempAnimFrame.tex->width;
					texInvHeight = 1.f / tempAnimFrame.tex->height;
				}
				// We also need to alter the frame bounds to appease OpenGL.
				tempAnimFrame.bounds.x *= texInvWidth;
				tempAnimFrame.bounds.y *= texInvHeight;
				tempAnimFrame.bounds.w *= texInvWidth;
				tempAnimFrame.bounds.h *= texInvHeight;
				// Increment how long the animation has been playing!
				playTime += delay;

				// Push our new frame into the animation frames vector!
				tempAnim.animFrames[tempAnim.frameData.numFrames] = tempAnimFrame;
				tempAnim.frameData.time[tempAnim.frameData.numFrames] = playTime;
				++tempAnim.frameData.numFrames;

			// Frame scroll.
			// Create "numFrames" many frames by repeatedly moving
			// the frame bounds by (scrollX, scrollY, scrollW, scrollH).
			}else if(memcmp(&line[1], "c ", 2) == 0){
				const unsigned int numFrames = strtoul(tokPos, &tokPos, 10);
				size_t curFrame = 0;

				// Make sure we're actually creating a frame.
				if(curFrame < numFrames){
					// Get a pointer to the texture with the specified index.
					// This will load the texture if it hasn't already been loaded.
					if(startTex < texPathsSize){
						const char *const texPath = texPaths[startTex];
						tempAnimFrame.tex = textureLoad(texPath, strlen(texPath));
						texInvWidth  = 1.f / tempAnimFrame.tex->width;
						texInvHeight = 1.f / tempAnimFrame.tex->height;
					}
					// We also need to alter the frame bounds to appease OpenGL.
					tempAnimFrame.bounds.x *= texInvWidth;
					tempAnimFrame.bounds.y *= texInvHeight;
					tempAnimFrame.bounds.w *= texInvWidth;
					tempAnimFrame.bounds.h *= texInvHeight;

					{
						const float scrollX = strtof(tokPos, &tokPos) * texInvWidth;
						const float scrollY = strtof(tokPos, &tokPos) * texInvHeight;
						const float scrollW = strtof(tokPos, &tokPos) * texInvWidth;
						const float scrollH = strtof(tokPos, NULL) * texInvHeight;

						do {
							// Allocate more memory for the new frame
							// and the timestamp if we've run out!
							if(tempAnim.frameData.numFrames >= animFramesCapacity){
								animFramesCapacity = tempAnim.frameData.numFrames * 2;
								tempAnim.animFrames = memoryManagerGlobalRealloc(
									tempAnim.animFrames, animFramesCapacity * sizeof(*tempAnim.animFrames)
								);
								if(tempAnim.animFrames == NULL){
									/** REALLOC FAILED **/
								}
								tempAnim.frameData.time = memoryManagerGlobalRealloc(
									tempAnim.frameData.time, animFramesCapacity * sizeof(*tempAnim.frameData.time)
								);
								if(tempAnim.frameData.time == NULL){
									/** REALLOC FAILED **/
								}
							}

							// Increment how long the animation has been playing!
							playTime += delay;

							// Push our new frame into the animation frames vector!
							tempAnim.animFrames[tempAnim.frameData.numFrames] = tempAnimFrame;
							tempAnim.frameData.time[tempAnim.frameData.numFrames] = playTime;
							++tempAnim.frameData.numFrames;

							// Scroll the frame's bounds!
							tempAnimFrame.bounds.x += scrollX;
							tempAnimFrame.bounds.y += scrollY;
							tempAnimFrame.bounds.w += scrollW;
							tempAnimFrame.bounds.h += scrollH;

							++curFrame;
						} while(curFrame < numFrames);
					}
				}

			// Frame sequence.
			// Create a frame for each texture with
			// indices from "startTex" up to "endTex".
			}else if(memcmp(&line[1], "s ", 2) == 0){
				const unsigned int endTex = strtoul(tokPos, NULL, 10);

				size_t curTex = startTex;
				for(; curTex <= endTex; ++curTex){
					// Allocate more memory for the new frame
					// and the timestamp if we've run out!
					if(tempAnim.frameData.numFrames >= animFramesCapacity){
						animFramesCapacity = tempAnim.frameData.numFrames * 2;
						tempAnim.animFrames = memoryManagerGlobalRealloc(
							tempAnim.animFrames, animFramesCapacity * sizeof(*tempAnim.animFrames)
						);
						if(tempAnim.animFrames == NULL){
							/** REALLOC FAILED **/
						}
						tempAnim.frameData.time = memoryManagerGlobalRealloc(
							tempAnim.frameData.time, animFramesCapacity * sizeof(*tempAnim.frameData.time)
						);
						if(tempAnim.frameData.time == NULL){
							/** REALLOC FAILED **/
						}
					}

					// Get a pointer to the texture with the specified index.
					// This will load the texture if it hasn't already been loaded.
					if(curTex < texPathsSize){
						const char *const texPath = texPaths[curTex];
						tempAnimFrame.tex = textureLoad(texPath, strlen(texPath));
						texInvWidth  = 1.f / tempAnimFrame.tex->width;
						texInvHeight = 1.f / tempAnimFrame.tex->height;
					}
					playTime += delay;

					// Push our new frame into the animation frames vector!
					{
						textureGroupFrame *const newAnimFrame = &tempAnim.animFrames[tempAnim.frameData.numFrames];
						newAnimFrame->tex = tempAnimFrame.tex;
						// We also need to alter the frame bounds to appease OpenGL.
						newAnimFrame->bounds.x = tempAnimFrame.bounds.x * texInvWidth;
						newAnimFrame->bounds.y = tempAnimFrame.bounds.y * texInvHeight;
						newAnimFrame->bounds.w = tempAnimFrame.bounds.w * texInvWidth;
						newAnimFrame->bounds.h = tempAnimFrame.bounds.h * texInvHeight;
					}
					tempAnim.frameData.time[tempAnim.frameData.numFrames] = playTime;
					++tempAnim.frameData.numFrames;
				}
			}

		// Animation end.
		}else if(line[0] == '}'){
			break;
		}
	}

	// If the animation has no frames, delete it!
	if(tempAnim.frameData.numFrames == 0){
		texGroupAnimDefDelete(&tempAnim);
		return(0);
	}

	// Otherwise, resize the frame vectors
	// so we aren't wasting any memory!
	tempAnim.animFrames = memoryManagerGlobalResize(
		tempAnim.animFrames, tempAnim.frameData.numFrames * sizeof(*tempAnim.animFrames)
	);
	if(tempAnim.animFrames == NULL){
		/** REALLOC FAILED **/
	}
	tempAnim.frameData.time = memoryManagerGlobalResize(
		tempAnim.frameData.time, tempAnim.frameData.numFrames * sizeof(*tempAnim.frameData.time)
	);
	if(tempAnim.frameData.time == NULL){
		/** REALLOC FAILED **/
	}

	#warning "If the animation's name wasn't set, we should create one ourselves."
	*texGroupAnimDef = tempAnim;
	return(1);
}