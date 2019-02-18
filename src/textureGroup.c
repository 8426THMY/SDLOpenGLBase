#include "textureGroup.h"


#include <stdlib.h>
#include <string.h>

#include "utilString.h"

#include "memoryManager.h"
#include "moduleTexture.h"


#define TEXTUREGROUP_PATH_PREFIX        ".\\resource\\textures\\"
#define TEXTUREGROUP_PATH_PREFIX_LENGTH (sizeof(TEXTUREGROUP_PATH_PREFIX) - 1)

//These must be at least 1!
#define BASE_INDEX_CAPACITY 1
#define BASE_ANIM_CAPACITY  1


#warning "What if we aren't using the global memory manager?"


static textureGroupFrame defaultTexGroupFrame = {
	//We will need to set this to
	//the ID of the error texture.
	.texID = 0,

	.x = 0.f,
	.y = 0.f,
	.width = 1.f,
	.height = 1.f
};

static textureGroupAnimDef defaultTexGroupAnim = {
	.name = "default",

	.animFrames = &defaultTexGroupFrame,
	//This defines an animation with only
	//one frame, being the error texture.
	.frameData = {
		.playNum = 0,

		.time = &defaultAnimTime,
		.numFrames = 1
	}
};

//This texture group is used when the
//real one cannot be found for some reason.
textureGroup errorTexGroup = {
	.name = "error",

	.texAnims = &defaultTexGroupAnim,
	.numAnims = 1
};


void texGroupFrameInit(textureGroupFrame *texGroupFrame){
	texGroupFrame->texID = 1;

	texGroupFrame->x      = 0.f;
	texGroupFrame->y      = 0.f;
	texGroupFrame->width  = 1.f;
	texGroupFrame->height = 1.f;
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

void texGroupAnimInit(textureGroupAnim *texGroupAnim, const size_t texGroupPos){
	//If the textureGroup has been loaded, use it!
	/*if(texGroupPos < loadedTextureGroups.size){
		texGroupAnim->texGroupPos = texGroupPos;

	//Otherwise, use the error textureGroup!
	}else{
		texGroupAnim->texGroupPos = 0;
	}

	animationInit(&texGroupAnim->animData);
	texGroupAnim->animData.currentPlayNum = ((textureGroup *)vectorGet(&loadedTextureGroups, texGroupAnim->texGroupPos))->texAnims[0].frameData.playNum;*/
}


//Load a textureGroup using the file specified by "texGroupName".
return_t texGroupLoad(textureGroup *texGroup, const char *texGroupName){
	texGroupInit(texGroup);

	//Find the full path for the textureGroup!
	const size_t texGroupNameLength = strlen(texGroupName);
	char *texGroupPath = memoryManagerGlobalAlloc(TEXTUREGROUP_PATH_PREFIX_LENGTH + texGroupNameLength + 1);
	if(texGroupPath == NULL){
		/** MALLOC FAILED **/
	}
	memcpy(texGroupPath, TEXTUREGROUP_PATH_PREFIX, TEXTUREGROUP_PATH_PREFIX_LENGTH);
	strcpy(texGroupPath + TEXTUREGROUP_PATH_PREFIX_LENGTH, texGroupName);

	//Load the textureGroup!
	FILE *texGroupFile = fopen(texGroupPath, "r");
	if(texGroupFile != NULL){
		//Stores the positions of the textures being used!
		size_t texturesSize = 0;
		size_t texturesCapacity = BASE_INDEX_CAPACITY;
		texture **textures = memoryManagerGlobalAlloc(texturesCapacity * sizeof(*textures));
		if(textures == NULL){
			/** MALLOC FAILED **/
		}

		//Store the animations that we've loaded so far!
		size_t texAnimsSize = 0;
		size_t texAnimsCapacity = BASE_ANIM_CAPACITY;
		textureGroupAnimDef *texAnims = memoryManagerGlobalAlloc(texAnimsCapacity * sizeof(*texAnims));
		if(texAnims == NULL){
			/** MALLOC FAILED **/
		}

		//Store pointers to the animation we're currently working on.
		textureGroupAnimDef *tempAnim = NULL;
		size_t animFramesSize = 0;
		size_t animFramesCapacity = 0;
		animationFrameData *tempAnimFrameData = NULL;

		//This is the index of the animation we're currently working on.
		size_t currentAnim = -1;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while((line = readLineFile(texGroupFile, &lineBuffer[0], &lineLength)) != NULL){
			//If we aren't loading an animation, check
			//for any texture or animation definitions.
			if(currentAnim == -1){
				//Texture path.
				if(memcmp(line, "t ", 2) == 0){
					char *tempName;
					size_t nameLength;
					//Get the file name!
					getDelimitedString(&line[2], lineLength - 2, "\" ", &tempName, &nameLength);

					size_t frameStart = 0;
					size_t frameEnd = 0;
					char *tempEnd = tempName + nameLength;
					//Skip past any quotation marks or spaces!
					while(*tempEnd != '\0' && (*tempEnd == '\"' || *tempEnd == ' ')){
						++tempEnd;
					}

					//If we haven't reached the end of the
					//line, multiple frames may be specified.
					if(tempEnd != &line[lineLength]){
						//Get the number of the first frame!
						frameStart = strtoul(tempEnd, &tempEnd, 10);

						//If it was successful and we haven't reached the end
						//of the line, try and get the number of the last frame!
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


					//We add the frame number before the
					//file extension, so find the period!
					int nameDotPos = strchr(tempName, '.') - tempName;
					if(nameDotPos < 0){
						nameDotPos = nameLength;
					}

					//This array stores the current frame number as a string!
					char frameNum[ULONG_MAX_CHARS + 1];
					size_t frameNumLength;


					//This variable stores the name of
					//the image used by the current frame!
					char *frameName = NULL;
					//Store a pointer to the frame name!
					char *namePointer;

					size_t i;
					//Loop through all the texture frames we're loading!
					for(i = frameStart; i <= frameEnd; ++i){
						//Add the value in 'a' to the end of the texture's
						//file name if there are multiple frames to add!
						if(frameEnd > 0){
							//Convert the frame number to a string.
							frameNumLength = ultostr(i, frameNum);

							frameName = memoryManagerGlobalRealloc(frameName, nameLength + frameNumLength + 1);
							if(frameName == NULL){
								/** REALLOC FAILED **/
							}
							//Now copy tempName into frameName!
							memcpy(frameName, tempName, nameDotPos * sizeof(*frameName));
							//Copy the frame number into frameName!
							strcpy(frameName + nameDotPos, frameNum);
							//Don't forget to add the file extension and the null terminator!
							memcpy(frameName + nameDotPos + frameNumLength, tempName + nameDotPos, nameLength - nameDotPos);
							frameName[nameLength + frameNumLength] = '\0';

							namePointer = frameName;
						}else{
							namePointer = tempName;
						}


						//Check if we've already loaded a texture with this name!
						texture *tex = moduleTextureFind(namePointer);
						//If we couldn't find the texture, load it!
						if(tex == NULL){
							//Make sure we can allocate
							//enough memory for the texture.
							if(!(tex = moduleTextureAlloc())){
								/** MALLOC FAILED **/
							}
							//If we can't load it, just
							//use the error texture.
							if(!textureLoad(tex, namePointer)){
								moduleTextureFree(tex);
								tex = &errorTex;
							}
						}

						//If we're out of space, allocate some more!
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

				//Animation start.
				}else if(memcmp(line, "a", 1) == 0 && line[lineLength - 1] == '{'){
					//If we're out of space for the new
					//animation, allocate some more!
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
					//We'll need to allocate some space for the animation's frames.
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

			//If we are loading an animation,
			//check for any of its details.
			}else{
				//Animation details.
				if(memcmp(line, "ad ", 3) == 0){
					char *tempName;
					size_t nameLength;
					getDelimitedString(&line[3], lineLength - 3, "\" ", &tempName, &nameLength);
					//Store the animation's name!
					tempAnim->name = memoryManagerGlobalAlloc(nameLength + 1);
					if(tempAnim->name == NULL){
						/** MALLOC FAILED **/
					}
					memcpy(tempAnim->name, tempName, nameLength);
					tempAnim->name[nameLength] = '\0';

					char *tempEnd = tempName + nameLength + 1;
					//Find and store the number of times to loop if we can!
					if(tempEnd != &line[lineLength]){
						char *playNumEnd;
						tempAnimFrameData->playNum = strtol(tempEnd, &playNumEnd, 10);
						//If no digits were read, set playNum to -1 (so it loops indefinitely).
						if(playNumEnd == tempEnd){
							tempAnimFrameData->playNum = -1;
						}
					}


					//If an animation with this name already exists, delete the new one!
					if(texGroupFindAnimNameIndex(texGroup, tempAnim->name) != currentAnim){
						texGroupAnimDefDelete(tempAnim);
						--texAnimsSize;

						currentAnim = -1;
					}

				//Animation frame.
				}else if(memcmp(line, "af ", 3) == 0){
					char *tokPos = &line[3];

					//Read the compressed frame data!
					size_t tempStartTex = strtoul(tokPos, &tokPos, 10);
					size_t tempEndTex = strtoul(tokPos, &tokPos, 10);
					size_t tempFramesPerTex = strtoul(tokPos, &tokPos, 10);

					float tempTime = strtod(tokPos, &tokPos);
					float tempX = strtod(tokPos, &tokPos);
					float tempY = strtod(tokPos, &tokPos);
					float tempWidth = strtod(tokPos, &tokPos);
					float tempHeight = strtod(tokPos, &tokPos);

					size_t tempFramesPerLine = strtoul(tokPos, &tokPos, 10);
					int tempAxis = strtol(tokPos, &tokPos, 10);

					//Animations need at least one frame!
					if(tempFramesPerTex < 1){
						tempFramesPerTex = 1;
					}
					//We don't want endTex to be less than startTex!
					if(tempEndTex < tempStartTex){
						tempEndTex = tempStartTex;
					}
					//If framesPerRow hasn't been set...
					if(tempFramesPerLine <= 0){
						//If we're using only one texture, set it to the number of frames!
						if(tempStartTex == tempEndTex){
							tempFramesPerLine = tempFramesPerTex;

						//Otherwise, we're probably only going to have a single frame per texture, so set it to 1!
						}else{
							tempFramesPerLine = 1;
						}
					}

					float frameTime = 0.f;
					//Let the new animFrame(s) continue on from the previous one(s)!
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
					texture **endTex = &textures[tempStartTex];
					texture **lastTex = &textures[texturesSize];
					//Loop through all the textures that we need to use!
					for(; startTex <= endTex; ++startTex){
						//If this texture is valid, get its ID, width and height!
						if(startTex < lastTex){
							texture *currentTex = *startTex;
							tempFrame.texID = currentTex->id;
							tempTexWidth    = currentTex->width;
							tempTexHeight   = currentTex->height;

						//Otherwise, get the error texture's!
						}else{
							tempFrame.texID = errorTex.id;
							tempTexWidth    = errorTex.width;
							tempTexHeight   = errorTex.height;
						}


						size_t b;
						//Loops through all the frames that we need to make!
						for(b = 0; b < tempFramesPerTex; ++b){
							frameTime += tempTime;

							tempFrame.x = tempX / tempTexWidth + tempWidth / tempTexWidth * currentLineX;
							tempFrame.y = tempY / tempTexHeight + tempHeight / tempTexHeight * currentLineY;
							if(tempWidth > 0.f){
								tempFrame.width = tempWidth / tempTexWidth;
							}else{
								tempFrame.width = 1.f;
							}
							if(tempHeight > 0.f){
								tempFrame.height = tempHeight / tempTexHeight;
							}else{
								tempFrame.height = 1.f;
							}


							//If we've reached the last frame on
							//the line, go onto the next line!
							//If axis is less than or equal to 0,
							//we're animating in rows.
							if(tempAxis <= 0){
								++currentLineX;
								if(currentLineX >= tempFramesPerLine){
									currentLineX = 0;
									++currentLineY;
								}

							//Otherwise, we're animating in columns.
							}else{
								++currentLineY;
								if(currentLineY >= tempFramesPerLine){
									currentLineY = 0;
									++currentLineX;
								}
							}


							//Push our new frame into the animation frames vector!
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

				//Animation end.
				}else if(line[0] == '}'){
					//If the animation has no frames, delete it!
					if(animFramesSize == 0){
						texGroupAnimDefDelete(tempAnim);
						--texAnimsSize;

					//Otherwise, resize the frame vector
					//so we aren't wasting memory!
					}else{
						tempAnim->animFrames = memoryManagerGlobalRealloc(tempAnim->animFrames, animFramesSize * sizeof(*tempAnim->animFrames));
						if(tempAnim->animFrames == NULL){
							/** REALLOC FAILED **/
						}
						tempAnimFrameData->time = memoryManagerGlobalRealloc(tempAnimFrameData->time, animFramesSize * sizeof(*tempAnimFrameData->time));
						if(tempAnimFrameData->time == NULL){
							/** REALLOC FAILED **/
						}

						//Set the animation's frame count!
						tempAnimFrameData->numFrames = animFramesSize;
						//Set the textureGroup's animation count!
						//We need to do this for when we
						//check for duplicate animations.
						texGroup->numAnims = texAnimsSize;
					}

					animFramesSize = 0;
					animFramesCapacity = 0;

					currentAnim = -1;
				}
			}
		}

		fclose(texGroupFile);


		//We don't need to check if this is NULL,
		//as we do that when we're using it.
		memoryManagerGlobalFree(textures);


		if(texturesSize > 0){
			//If the textureGroup has no
			//animations, use the default one.
			if(texAnimsSize == 0){
				//We delete the textureGroup here, as that
				//will free any memory that it is using.
				texGroupDelete(texGroup);

				texGroup->texAnims = &defaultTexGroupAnim;
				texGroup->numAnims = 1;

			//If there are animations, resize the
			//array so we aren't wasting memory!
			}else{
				texGroup->texAnims = memoryManagerGlobalRealloc(texAnims, texAnimsSize * sizeof(*texAnims));
				if(texGroup->texAnims == NULL){
					/** REALLOC FAILED **/
				}
			}

			//If we could set up the textureGroup
			//successfully, set its name!
			texGroup->name = memoryManagerGlobalRealloc(texGroupPath, texGroupNameLength + 1);
			if(texGroup->name == NULL){
				/** REALLOC FAILED **/
			}
			strcpy(texGroup->name, texGroupName);


			return(1);
		}

		//If the textureGroup referenced
		//no textures, just delete it.
		texGroupDelete(texGroup);

	}else{
		printf(
			"Unable to open textureGroup!\n"
			"Path: %s\n",
			texGroupPath
		);
	}

	memoryManagerGlobalFree(texGroupPath);


	return(0);
}


//Change the textureGroup's current animation!
void texGroupAnimSetAnim(textureGroupAnim *texGroupAnim, const size_t currentAnim){
	/*const textureGroup *tempTexGroup = (textureGroup *)vectorGet(&loadedTextureGroups, texGroupAnim->texGroupPos);
	if(currentAnim < tempTexGroup->numAnims){
		animationSetAnim(&texGroupAnim->animData, tempTexGroup->texAnims[currentAnim].frameData.playNum, currentAnim);
	}*/
}

//Play the textureGroup's current animation!
void texGroupAnimUpdateAnim(textureGroupAnim *texGroupAnim, const float time){
	/*const textureGroup *tempTexGroup = (textureGroup *)vectorGet(&loadedTextureGroups, texGroupAnim->texGroupPos);
	if(texGroupAnim->animData.currentAnim < tempTexGroup->numAnims){
		animationUpdate(&texGroupAnim->animData, &(tempTexGroup->texAnims[texGroupAnim->animData.currentAnim].frameData), time);
	}*/
}

//Get the sub-image coordinates for the current frame!
GLuint texGroupGetFrame(const textureGroup *texGroup, const size_t currentAnim, const size_t currentFrame, const GLuint uvOffsetsID){
	GLuint tempTexID;
	float uvOffsets[4];

	//If the animation and frame are valid, get the texture's ID and set the UV offsets!
	if(currentAnim < texGroup->numAnims && currentFrame < texGroup->texAnims[currentAnim].frameData.numFrames){
		textureGroupFrame *tempFrame = &(texGroup->texAnims[currentAnim].animFrames[currentFrame]);

		tempTexID    = tempFrame->texID;
		uvOffsets[0] = tempFrame->x;
		uvOffsets[1] = tempFrame->y;
		uvOffsets[2] = tempFrame->width;
		uvOffsets[3] = tempFrame->height;

	//If the animation or frame is invalid, use the error texture!
	}else{
		tempTexID    = errorTex.id;
		uvOffsets[0] = 0.f;
		uvOffsets[1] = 0.f;
		uvOffsets[2] = TEXTURE_ERROR_WIDTH;
		uvOffsets[3] = TEXTURE_ERROR_HEIGHT;
	}

	//Give the current UV offsets to the shader!
	glUniform1fv(uvOffsetsID, 4, uvOffsets);


	return(tempTexID);
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
	//We can only free this stuff if we
	//are not freeing the error textureGroup.
	if(texGroup != &errorTexGroup){
		if(texGroup->name != NULL){
			memoryManagerGlobalFree(texGroup->name);
		}

		textureGroupAnimDef *curAnim = texGroup->texAnims;
		//Delete the textureGroup's animations.
		if(curAnim != NULL && curAnim != &defaultTexGroupAnim){
			const textureGroupAnimDef *lastAnim = &curAnim[texGroup->numAnims];
			for(; curAnim < lastAnim; ++curAnim){
				texGroupAnimDefDelete(curAnim);
			}
			memoryManagerGlobalFree(texGroup->texAnims);
		}
	}
}


//Loop through all of a textureGroup's animations until we find the one we need!
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

//Loop through all the textureGroups we've loaded until we find the one we need!
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


/*void textureGroupModuleCleanup(){

}*/