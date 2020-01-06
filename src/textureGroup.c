#include "textureGroup.h"


#include <stdlib.h>
#include <string.h>

#include "utilString.h"
#include "utilFile.h"

#include "memoryManager.h"
#include "moduleTexture.h"


#define TEXTUREGROUP_PATH_PREFIX        ".\\resource\\texgroups\\"
#define TEXTUREGROUP_PATH_PREFIX_LENGTH (sizeof(TEXTUREGROUP_PATH_PREFIX) - 1)

// These must be at least 1!
#define BASE_INDEX_CAPACITY 1
#define BASE_ANIM_CAPACITY  1


#warning "What if we aren't using the global memory manager?"


static textureGroupFrame texGroupFrameDefault = {
	.diffuse = &texDefault,
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

		.time = &animTimeDefault,
		.numFrames = 1
	}
};

// This texture group is used when the
// real one cannot be found for some reason.
textureGroup texGroupDefault = {
	.name = "error",

	.texAnims = &texGroupAnimDefault,
	.numAnims = 1
};


void texGroupFrameInit(textureGroupFrame *texGroupFrame){
	texGroupFrame->diffuse = &texDefault;

	texGroupFrame->bounds.x = 0.f;
	texGroupFrame->bounds.y = 0.f;
	texGroupFrame->bounds.w = 1.f;
	texGroupFrame->bounds.h = 1.f;
}

void texGroupAnimDefInit(textureGroupAnimDef *texGroupAnimDef){
	texGroupAnimDef->name = NULL;

	texGroupAnimDef->frameData.playNum = 0;
	texGroupAnimDef->animFrames = NULL;
	animFrameDataInit(&texGroupAnimDef->frameData);
}

void texGroupInit(textureGroup *texGroup){
	texGroup->name = NULL;

	texGroup->texAnims = NULL;
	texGroup->numAnims = 0;
}

void texGroupStateInit(textureGroupState *texGroupState, const textureGroup *texGroup){
	texGroupState->texGroup = texGroup;

	texGroupState->currentAnim = 0;
	animationInit(&texGroupState->texGroupAnim, ANIMATION_LOOP_INDEFINITELY);
}


// Load the textureGroup specified by "texGroupPath".
return_t texGroupLoad(textureGroup *texGroup, const char *texGroupPath){
	FILE *texGroupFile;
	char texGroupFullPath[FILE_MAX_LINE_LENGTH];
	size_t texGroupPathLength = strlen(texGroupPath);

	// Find the full path for the textureGroup!
	fileGenerateFullPath(
		texGroupPath, texGroupPathLength,
		TEXTUREGROUP_PATH_PREFIX, TEXTUREGROUP_PATH_PREFIX_LENGTH,
		texGroupFullPath
	);


	texGroupInit(texGroup);


	// Load the textureGroup!
	texGroupFile = fopen(texGroupFullPath, "r");
	if(texGroupFile != NULL){
		// Stores the positions of the textures being used!
		size_t texturesSize = 0;
		size_t texturesCapacity = BASE_INDEX_CAPACITY;
		texture **textures = memoryManagerGlobalAlloc(texturesCapacity * sizeof(*textures));
		if(textures == NULL){
			/** MALLOC FAILED **/
		}

		// Store the animations that we've loaded so far!
		size_t texAnimsSize = 0;
		size_t texAnimsCapacity = BASE_ANIM_CAPACITY;
		textureGroupAnimDef *texAnims = memoryManagerGlobalAlloc(texAnimsCapacity * sizeof(*texAnims));
		if(texAnims == NULL){
			/** MALLOC FAILED **/
		}

		// Store pointers to the animation we're currently working on.
		textureGroupAnimDef *tempAnim = NULL;
		size_t animFramesSize = 0;
		size_t animFramesCapacity = 0;
		animationFrameData *tempAnimFrameData = NULL;

		// This is the index of the animation we're currently working on.
		size_t currentAnim = invalidValue(currentAnim);

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		while((line = fileReadLine(texGroupFile, &lineBuffer[0], &lineLength)) != NULL){
			// If we aren't loading an animation, check
			// for any texture or animation definitions.
			if(valueIsInvalid(currentAnim)){
				// Texture path.
				if(memcmp(line, "t ", 2) == 0){
					char *tempName;
					size_t nameLength;
					// Get the file name!
					tempName = getMultiDelimitedString(&line[2], lineLength - 2, "\" ", &nameLength);

					size_t frameStart = 0;
					size_t frameEnd = 0;
					char *tempEnd = tempName + nameLength;
					// Skip past any quotation marks or spaces!
					while(*tempEnd != '\0' && (*tempEnd == '\"' || *tempEnd == ' ')){
						++tempEnd;
					}

					// If we haven't reached the end of the
					// line, multiple frames may be specified.
					if(tempEnd != &line[lineLength]){
						// Get the number of the first frame!
						frameStart = strtoul(tempEnd, &tempEnd, 10);

						// If it was successful and we haven't reached the end
						// of the line, try and get the number of the last frame!
						if(tempEnd != &line[lineLength]){
							frameEnd = strtoul(tempEnd, NULL, 10);
						}
						if(frameStart < 0){
							frameStart = 0;
						}
						if(frameEnd < frameStart){
							frameEnd = frameStart;
						}
					}


					// We add the frame number before the
					// file extension, so find the period!
					int nameDotPos = strchr(tempName, '.') - tempName;
					if(nameDotPos < 0){
						nameDotPos = nameLength;
					}

					// This array stores the current frame number as a string!
					char frameNum[ULONG_MAX_CHARS + 1];
					size_t frameNumLength;


					// This variable stores the name of
					// the image used by the current frame!
					char *frameName = NULL;
					// Store a pointer to the frame name!
					char *namePointer;

					size_t i;
					// Loop through all the texture frames we're loading!
					for(i = frameStart; i <= frameEnd; ++i){
						// Add the value in 'a' to the end of the texture's
						// file name if there are multiple frames to add!
						if(frameEnd > 0){
							// Convert the frame number to a string.
							frameNumLength = ultostr(i, frameNum);

							frameName = memoryManagerGlobalRealloc(frameName, nameLength + frameNumLength + 1);
							if(frameName == NULL){
								/** REALLOC FAILED **/
							}
							// Now copy tempName into frameName!
							memcpy(frameName, tempName, nameDotPos * sizeof(*frameName));
							// Copy the frame number into frameName!
							strcpy(frameName + nameDotPos, frameNum);
							// Don't forget to add the file extension and the null terminator!
							memcpy(frameName + nameDotPos + frameNumLength, tempName + nameDotPos, nameLength - nameDotPos);
							frameName[nameLength + frameNumLength] = '\0';

							namePointer = frameName;
						}else{
							namePointer = tempName;
						}


						// Check if we've already loaded a texture with this name!
						texture *tex = moduleTextureFind(namePointer);
						// If we couldn't find the texture, load it!
						if(tex == NULL){
							// Make sure we can allocate
							// enough memory for the texture.
							if(!(tex = moduleTextureAlloc())){
								/** MALLOC FAILED **/
							}
							// If we can't load it, just
							// use the error texture.
							if(!textureLoad(tex, namePointer)){
								moduleTextureFree(tex);
								tex = &texDefault;
							}
						}

						// If we're out of space, allocate some more!
						if(texturesSize >= texturesCapacity){
							texturesCapacity = texturesSize * 2;
							textures = memoryManagerGlobalRealloc(textures, texturesCapacity * sizeof(*textures));
							if(textures == NULL){
								/** REALLOC FAILED **/
							}
						}
						textures[texturesSize] = tex;
						++texturesSize;
					}

					if(frameName != NULL){
						memoryManagerGlobalFree(frameName);
					}

				// Animation start.
				}else if(memcmp(line, "a", 1) == 0 && line[lineLength - 1] == '{'){
					// If we're out of space for the new
					// animation, allocate some more!
					if(texAnimsSize >= texAnimsCapacity){
						texAnimsCapacity = texAnimsSize * 2;
						texAnims = memoryManagerGlobalRealloc(texAnims, texAnimsCapacity * sizeof(*texAnims));
						if(texAnims == NULL){
							/** REALLOC FAILED **/
						}
					}
					tempAnim = &texAnims[texAnimsSize];
					tempAnimFrameData = &tempAnim->frameData;

					texGroupAnimDefInit(tempAnim);
					// We'll need to allocate some space for the animation's frames.
					#warning "Ideally, we shouldn't have to do this."
					tempAnim->animFrames = memoryManagerGlobalAlloc(sizeof(*tempAnim->animFrames));
					if(tempAnim->animFrames == NULL){
						/** MALLOC FAILED **/
					}
					tempAnimFrameData->time = memoryManagerGlobalAlloc(sizeof(*tempAnimFrameData->time));
					if(tempAnimFrameData->time == NULL){
						/** MALLOC FAILED **/
					}

					currentAnim = texAnimsSize;
					++texAnimsSize;
				}

			// If we are loading an animation,
			// check for any of its details.
			}else{
				// Animation details.
				if(memcmp(line, "ad ", 3) == 0){
					char *tempName;
					size_t nameLength;
					tempName = getMultiDelimitedString(&line[3], lineLength - 3, "\" ", &nameLength);
					// Store the animation's name!
					tempAnim->name = memoryManagerGlobalAlloc(nameLength + 1);
					if(tempAnim->name == NULL){
						/** MALLOC FAILED **/
					}
					memcpy(tempAnim->name, tempName, nameLength);
					tempAnim->name[nameLength] = '\0';

					char *tempEnd = tempName + nameLength + 1;
					// Find and store the number of times to loop if we can!
					if(tempEnd != &line[lineLength]){
						char *playNumEnd;
						tempAnimFrameData->playNum = strtol(tempEnd, &playNumEnd, 10);
						// If no digits were read, set playNum to -1 (so it loops indefinitely).
						if(playNumEnd == tempEnd){
							tempAnimFrameData->playNum = invalidValue(tempAnimFrameData->playNum);
						}
					}


					// If an animation with this name already exists, delete the new one!
					if(texGroupFindAnimNameIndex(texGroup, tempAnim->name) != currentAnim){
						texGroupAnimDefDelete(tempAnim);
						--texAnimsSize;

						currentAnim = invalidValue(currentAnim);
					}

				// Animation frame.
				}else if(memcmp(line, "af ", 3) == 0){
					char *tokPos;

					// Read the compressed frame data!
					size_t tempStartTex = strtoul(&line[3], &tokPos, 10);
					size_t tempEndTex = strtoul(tokPos, &tokPos, 10);
					size_t tempFramesPerTex = strtoul(tokPos, &tokPos, 10);

					float tempTime = strtod(tokPos, &tokPos);
					float tempX = strtod(tokPos, &tokPos);
					float tempY = strtod(tokPos, &tokPos);
					float tempWidth = strtod(tokPos, &tokPos);
					float tempHeight = strtod(tokPos, &tokPos);

					size_t tempFramesPerLine = strtoul(tokPos, &tokPos, 10);
					int tempAxis = strtol(tokPos, NULL, 10);

					// Animations need at least one frame!
					if(tempFramesPerTex < 1){
						tempFramesPerTex = 1;
					}
					// We don't want endTex to be less than startTex!
					if(tempEndTex < tempStartTex){
						tempEndTex = tempStartTex;
					}
					// If framesPerRow hasn't been set...
					if(tempFramesPerLine <= 0){
						// If we're using only one texture, set it to the number of frames!
						if(tempStartTex == tempEndTex){
							tempFramesPerLine = tempFramesPerTex;

						// Otherwise, we're probably only going to have a single frame per texture, so set it to 1!
						}else{
							tempFramesPerLine = 1;
						}
					}

					float frameTime = 0.f;
					// Let the new animFrame(s) continue on from the previous one(s)!
					if(animFramesSize > 0){
						frameTime = tempAnimFrameData->time[animFramesSize - 1];
					}


					textureGroupFrame tempFrame;
					texGroupFrameInit(&tempFrame);

					unsigned int currentLineX = 0;
					unsigned int currentLineY = 0;

					GLuint tempTexWidth;
					GLuint tempTexHeight;

					texture **startTex = &textures[tempStartTex];
					texture **lastTex = &textures[texturesSize];
					// Loop through all the textures that we need to use!
					for(; tempStartTex <= tempEndTex; ++tempStartTex, ++startTex){
						// If this texture is valid, get its ID, width and height!
						if(startTex < lastTex){
							texture *currentTex = *startTex;
							tempFrame.diffuse = currentTex;
							tempTexWidth      = currentTex->width;
							tempTexHeight     = currentTex->height;

						// Otherwise, get the error texture's!
						}else{
							tempFrame.diffuse = &texDefault;
							tempTexWidth      = texDefault.width;
							tempTexHeight     = texDefault.height;
						}


						size_t i;
						// Loops through all the frames that we need to make!
						for(i = 0; i < tempFramesPerTex; ++i){
							frameTime += tempTime;

							tempFrame.bounds.x = tempX / tempTexWidth + tempWidth / tempTexWidth * currentLineX;
							tempFrame.bounds.y = tempY / tempTexHeight + tempHeight / tempTexHeight * currentLineY;
							if(tempWidth > 0.f){
								tempFrame.bounds.w = tempWidth / tempTexWidth;
							}else{
								tempFrame.bounds.w = 1.f;
							}
							if(tempHeight > 0.f){
								tempFrame.bounds.h = tempHeight / tempTexHeight;
							}else{
								tempFrame.bounds.h = 1.f;
							}


							// If we've reached the last frame on
							// the line, go onto the next line!
							// If axis is less than or equal to 0,
							// we're animating in rows.
							if(tempAxis <= 0){
								++currentLineX;
								if(currentLineX >= tempFramesPerLine){
									currentLineX = 0;
									++currentLineY;
								}

							// Otherwise, we're animating in columns.
							}else{
								++currentLineY;
								if(currentLineY >= tempFramesPerLine){
									currentLineY = 0;
									++currentLineX;
								}
							}


							// Push our new frame into the animation frames vector!
							if(animFramesSize >= animFramesCapacity){
								animFramesCapacity = animFramesSize * 2;
								tempAnim->animFrames = memoryManagerGlobalRealloc(tempAnim->animFrames, animFramesCapacity * sizeof(*tempAnim->animFrames));
								if(tempAnim->animFrames == NULL){
									/** REALLOC FAILED **/
								}
								tempAnimFrameData->time = memoryManagerGlobalRealloc(tempAnimFrameData->time, animFramesCapacity * sizeof(*tempAnimFrameData->time));
								if(tempAnimFrameData->time == NULL){
									/** REALLOC FAILED **/
								}
							}
							tempAnim->animFrames[animFramesSize] = tempFrame;
							tempAnimFrameData->time[animFramesSize] = frameTime;

							++animFramesSize;
						}


						currentLineX = 0;
						currentLineY = 0;
					}

				// Animation end.
				}else if(line[0] == '}'){
					// If the animation has no frames, delete it!
					if(animFramesSize == 0){
						texGroupAnimDefDelete(tempAnim);
						--texAnimsSize;

					// Otherwise, resize the frame vector
					// so we aren't wasting memory!
					}else{
						tempAnim->animFrames = memoryManagerGlobalResize(tempAnim->animFrames, animFramesSize * sizeof(*tempAnim->animFrames));
						if(tempAnim->animFrames == NULL){
							/** REALLOC FAILED **/
						}
						tempAnimFrameData->time = memoryManagerGlobalResize(tempAnimFrameData->time, animFramesSize * sizeof(*tempAnimFrameData->time));
						if(tempAnimFrameData->time == NULL){
							/** REALLOC FAILED **/
						}

						// Set the animation's frame count!
						tempAnimFrameData->numFrames = animFramesSize;
						// Set the textureGroup's animation count!
						// We need to do this for when we
						// check for duplicate animations.
						texGroup->numAnims = texAnimsSize;
					}

					animFramesSize = 0;
					animFramesCapacity = 0;

					currentAnim = invalidValue(currentAnim);
				}
			}
		}

		fclose(texGroupFile);


		if(texturesSize > 0){
			// If the textureGroup has no
			// animations, use the default one.
			if(texAnimsSize == 0){
				// Allocate memory for a single default animation.
				texAnims = memoryManagerGlobalRealloc(texAnims, sizeof(*texAnims));
				if(texAnims == NULL){
					/** REALLOC FAILED **/
				}
				texAnims->name = memoryManagerGlobalAlloc(sizeof("default"));
				if(texAnims->name == NULL){
					/** MALLOC FAILED **/
				}
				texAnims->animFrames = memoryManagerGlobalAlloc(sizeof(*texAnims->animFrames));
				if(texAnims->animFrames == NULL){
					/** MALLOC FAILED **/
				}

				memcpy(texAnims->name, "default", sizeof("default"));
				// Initialise its frame data!
				texAnims->frameData.playNum = 0;
				texAnims->frameData.time = &animTimeDefault;
				texAnims->frameData.numFrames = 1;
				texGroupFrameInit(texAnims->animFrames);
				// The animation should use the first texture we loaded.
				texAnims->animFrames->diffuse = *textures;

				texGroup->texAnims = texAnims;
				texGroup->numAnims = 1;

			// If there are animations, resize the
			// array so we aren't wasting memory!
			}else{
				texGroup->texAnims = memoryManagerGlobalResize(texAnims, texAnimsSize * sizeof(*texAnims));
				if(texGroup->texAnims == NULL){
					/** REALLOC FAILED **/
				}
			}

			memoryManagerGlobalFree(textures);

			// If we could set up the textureGroup successfully, set its name!
			++texGroupPathLength;
			texGroup->name = memoryManagerGlobalAlloc(texGroupPathLength);
			if(texGroup->name == NULL){
				/** MALLOC FAILED **/
			}
			memcpy(texGroup->name, texGroupPath, texGroupPathLength);


			return(1);
		}


		// We don't need to check if this is NULL,
		// as we do that when we're using it.
		memoryManagerGlobalFree(textures);
		// If the textureGroup referenced
		// no textures, just delete it.
		texGroupDelete(texGroup);

	}else{
		printf(
			"Unable to open textureGroup!\n"
			"Path: %s\n",
			texGroupFullPath
		);
	}


	return(0);
}


// Continue animating a texture!
void texGroupStateUpdate(textureGroupState *texGroupState, const float time){
	animationUpdate(&texGroupState->texGroupAnim, &(texGroupState->texGroup->texAnims[texGroupState->currentAnim].frameData), time);
}

// Get the current frame of a texture group instance!
textureGroupFrame *texGroupStateGetFrame(const textureGroupState *texGroupState){
	const textureGroup *texGroup = texGroupState->texGroup;
	const textureGroupAnim *animData = &texGroupState->texGroupAnim;

	// If the animation and frame are valid, get the texture's ID and set the UV offsets!
	if(texGroupState->currentAnim < texGroup->numAnims){
		textureGroupAnimDef *tempAnim = &(texGroup->texAnims[texGroupState->currentAnim]);
		if(animData->currentFrame < tempAnim->frameData.numFrames){
			return(&(tempAnim->animFrames[animData->currentFrame]));
		}
	}

	return(&texGroupFrameDefault);
}


void texGroupAnimDefDelete(textureGroupAnimDef *texGroupAnimDef){
	if(texGroupAnimDef->name != NULL){
		memoryManagerGlobalFree(texGroupAnimDef->name);
	}
	if(texGroupAnimDef->animFrames != NULL){
		memoryManagerGlobalFree(texGroupAnimDef->animFrames);
	}
	animFrameDataClear(&texGroupAnimDef->frameData);
}

void texGroupDelete(textureGroup *texGroup){
	// We can only free this stuff if we
	// are not freeing the error textureGroup.
	if(texGroup != &texGroupDefault){
		if(texGroup->name != NULL){
			memoryManagerGlobalFree(texGroup->name);
		}

		textureGroupAnimDef *curAnim = texGroup->texAnims;
		// Delete the textureGroup's animations.
		if(curAnim != NULL && curAnim != &texGroupAnimDefault){
			const textureGroupAnimDef *lastAnim = &curAnim[texGroup->numAnims];
			for(; curAnim < lastAnim; ++curAnim){
				texGroupAnimDefDelete(curAnim);
			}
			memoryManagerGlobalFree(texGroup->texAnims);
		}
	}
}


// Loop through all of a textureGroup's animations until we find the one we need!
size_t texGroupFindAnimNameIndex(const textureGroup *texGroup, const char *name){
	/*size_t i;
	for(i = 0; i < texGroup->numAnims; ++i){
		if(strcmp(name, texGroup->texAnims[i].name) == 0){
			break;
		}
	}

	return(i);*/
	return(0);
}

// Loop through all the textureGroups we've loaded until we find the one we need!
size_t texGroupFindNameIndex(const char *name){
	/*size_t i;
	for(i = 0; i < loadedTextureGroups.size; ++i){
		if(strcmp(name, ((textureGroup *)vectorGet(&loadedTextureGroups, i))->name) == 0){
			break;
		}
	}

	return(i);*/
	return(0);
}