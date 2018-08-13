#include "textureGroup.h"


#include <stdlib.h>
#include <string.h>

#include "strUtil.h"
#include "texture.h"


vector loadedTextureGroups;


void texGroupFrameInit(textureGroupFrame *texGroupFrame){
	texGroupFrame->texID = 1;

	texGroupFrame->x = 0.f;
	texGroupFrame->y = 0.f;
	texGroupFrame->width = 1.f;
	texGroupFrame->height = 1.f;
}

unsigned char texGroupAnimInit(textureGroupAnim *texGroupAnim){
	texGroupAnim->name = NULL;

	texGroupAnim->frameData.playNum = 0;
	texGroupAnim->animFrames = NULL;
	frameDataInit(&texGroupAnim->frameData);

	return(1);
}

unsigned char texGroupInit(textureGroup *texGroup){
	texGroup->name = NULL;

	texGroup->texAnims = NULL;
	texGroup->numAnims = 0;

	return(1);
}

void texGroupAnimInstInit(textureGroupAnimInst *animInst, const size_t texGroupPos){
	//If the textureGroup has been loaded, use it!
	if(texGroupPos < loadedTextureGroups.size){
		animInst->texGroupPos = texGroupPos;

	//Otherwise, use the error textureGroup!
	}else{
		animInst->texGroupPos = 0;
	}

	animationInit(&animInst->animData);
	animInst->animData.currentPlayNum = ((textureGroup *)vectorGet(&loadedTextureGroups, animInst->texGroupPos))->texAnims[0].frameData.playNum;
}


//Load a textureGroup and return its index in the loadedTextureGroups vector!
size_t texGroupLoad(const char *texGroupName){
	size_t index = loadedTextureGroups.size;


	//Create and initialize the textureGroup!
	textureGroup texGroup;
	texGroupInit(&texGroup);


	//Find the full path for the textureGroup!
	const size_t texGroupNameLength = strlen(texGroupName);
	char *texGroupPath = malloc(20 + texGroupNameLength + 1);
	memcpy(texGroupPath, ".\\resource\\textures\\", 20);
	strcpy(texGroupPath + 20, texGroupName);

	//Load the textureGroup!
	FILE *texGroupFile = fopen(texGroupPath, "r");
	if(texGroupFile != NULL){
		//Temporarily store the capacity of the texGroup's animations
		//vector and the current animation's frame vector.
		size_t texAnimsCapacity = 1;
		//We don't need to set this immediately.
		size_t animFramesCapacity = 1;

		//Stores the positions of the textures being used!
		size_t texIndicesSize = 0;
		size_t texIndicesCapacity = 1;
		size_t *texIndices = malloc(texIndicesCapacity * sizeof(*texIndices));
		//This is the index of the animation we're currently working on.
		size_t currentAnim = 0;

		/** Ideally, we shouldn't have to do this. **/
		texGroup.texAnims = malloc(sizeof(*texGroup.texAnims));

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while((line = readLineFile(texGroupFile, &lineBuffer[0], &lineLength)) != NULL && index != -1){
			//Texture path.
			if(memcmp(line, "t ", 2) == 0){
				//Get the file name!
				char *tempName;
				size_t nameLength;
				getDelimitedString(&line[2], lineLength - 2, "\" ", &tempName, &nameLength);

				size_t frameStart = 0;
				size_t frameEnd = 0;
				char *tempEnd = tempName + nameLength;
				//Skip past any quotation marks or spaces!
				while(*tempEnd != '\0' && (*tempEnd == '\"' || *tempEnd == ' ')){
					++tempEnd;
				}
				if(tempEnd != &line[lineLength]){
					//Get the number of the first frame!
					frameStart = strtoul(tempEnd, &tempEnd, 10);
					//If it was successful and we haven't reached the end of the line, try and get the number of the last frame!
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


				//We add the frame number before the file extension, so find the period!
				int nameDotPos = strchr(tempName, '.') - tempName;
				if(nameDotPos < 0){
					nameDotPos = nameLength;
				}

				//This array stores the current frame number as a string!
				char frameNum[ULONG_MAX_CHARS + 1];
				size_t frameNumLength;


				//This variable stores the name of the current frame if it has a number added to it!
				char *frameName = NULL;
				//Store a pointer to the frame name!
				char *namePointer;

				size_t i;
				//Loop through all the texture frames we're loading!
				for(i = frameStart; i <= frameEnd; ++i){
					//Add the value in 'a' to the end of the texture's file name if there are multiple frames to add!
					if(frameEnd > 0){
						//Convert the frame number to a string.
						frameNumLength = ultostr(i, frameNum);

						frameName = realloc(frameName, nameLength + frameNumLength + 1);
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
					size_t texIndex = textureFindNameIndex(namePointer);
					//If we couldn't find the texture, load it!
					if(texIndex == loadedTextures.size){
						texIndex = textureLoad(namePointer);
					}
					//If we're out of space, allocate some more!
					if(texIndicesSize >= texIndicesCapacity){
						texIndicesCapacity = texIndicesSize * 2;
						texIndices = realloc(texIndices, texIndicesCapacity * sizeof(*texIndices));
					}
					texIndices[texIndicesSize] = texIndex;
					++texIndicesSize;
				}

				if(frameName != NULL){
					free(frameName);
				}

			//Animation start.
			}else if(memcmp(line, "a", 1) == 0 && line[lineLength - 1] == '{' && currentAnim == 0){
				if(texGroup.numAnims >= texAnimsCapacity){
					texAnimsCapacity = texGroup.numAnims * 2;
					texGroup.texAnims = realloc(texGroup.texAnims, texAnimsCapacity * sizeof(*texGroup.texAnims));
				}
				texGroupAnimInit(&texGroup.texAnims[texGroup.numAnims]);
				/** Ideally, we shouldn't have to do this. **/
				texGroup.texAnims[texGroup.numAnims].animFrames = malloc(sizeof(*texGroup.texAnims[texGroup.numAnims].animFrames));
				texGroup.texAnims[texGroup.numAnims].frameData.time = malloc(sizeof(*texGroup.texAnims[texGroup.numAnims].frameData.time));
				++texGroup.numAnims;

				currentAnim = texGroup.numAnims;

			//Animation details.
			}else if(memcmp(line, "ad ", 3) == 0 && currentAnim > 0){
				textureGroupAnim *tempAnim = &texGroup.texAnims[currentAnim - 1];

				char *tempName;
				size_t nameLength;
				getDelimitedString(&line[3], lineLength - 3, "\" ", &tempName, &nameLength);
				//Store the animation's name!
				tempAnim->name = malloc(nameLength + 1);
				memcpy(tempAnim->name, tempName, nameLength);
				tempAnim->name[nameLength] = '\0';

				char *tempEnd = tempName + nameLength + 1;
				//Find and store the number of times to loop if we can!
				if(tempEnd != &line[lineLength]){
					char *playNumEnd;
					tempAnim->frameData.playNum = strtol(tempEnd, &playNumEnd, 10);
					//If no digits were read, set playNum to -1 (so it loops indefinitely).
					if(playNumEnd == tempEnd){
						tempAnim->frameData.playNum = -1;
					}
				}


				//If an animation with this name already exists, delete the new one!
				if(texGroupFindAnimNameIndex(&texGroup, tempAnim->name) != currentAnim - 1){
					texGroupAnimDelete(tempAnim);
					if(currentAnim != texGroup.numAnims){
						memmove(tempAnim, &texGroup.texAnims[currentAnim], (texGroup.numAnims - currentAnim) * sizeof(*texGroup.texAnims));
					}
					--texGroup.numAnims;

					currentAnim = 0;
				}

			//Animation frame.
			}else if(memcmp(line, "af ", 3) == 0 && currentAnim > 0){
				textureGroupAnim *tempAnim = &texGroup.texAnims[currentAnim - 1];

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

				//You need at least one frame!
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
				if(tempAnim->frameData.numFrames > 0){
					frameTime = tempAnim->frameData.time[tempAnim->frameData.numFrames - 1];
				}


				textureGroupFrame tempFrame;
				texGroupFrameInit(&tempFrame);

				unsigned int currentLineX = 0;
				unsigned int currentLineY = 0;

				GLuint tempTexWidth;
				GLuint tempTexHeight;

				size_t a;
				//Loop through all the textures that we need to use!
				for(a = tempStartTex; a <= tempEndTex; ++a){
					//If this texture is valid, get its I.D., width and height!
					if(a < texIndicesSize){
						tempFrame.texID = ((texture *)vectorGet(&loadedTextures, texIndices[a]))->id;

						tempTexWidth = ((texture *)vectorGet(&loadedTextures, texIndices[a]))->width;
						tempTexHeight = ((texture *)vectorGet(&loadedTextures, texIndices[a]))->height;

					//Otherwise, get the error texture's!
					}else{
						tempFrame.texID = ((texture *)vectorGet(&loadedTextures, 0))->id;

						tempTexWidth = ((texture *)vectorGet(&loadedTextures, 0))->width;
						tempTexHeight = ((texture *)vectorGet(&loadedTextures, 0))->height;
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


						//If we've reached the last frame on the line, going onto the next line!
						//If axis is less than or equal to 0, we're animating in rows. Otherwise, we're doing columns.
						if(tempAxis <= 0){
							++currentLineX;
							if(currentLineX >= tempFramesPerLine){
								currentLineX = 0;
								++currentLineY;
							}
						}else{
							++currentLineY;
							if(currentLineY >= tempFramesPerLine){
								currentLineY = 0;
								++currentLineX;
							}
						}


						//Push our new frame into the animation frames vector!
						if(tempAnim->frameData.numFrames >= animFramesCapacity){
							animFramesCapacity = tempAnim->frameData.numFrames * 2;
							tempAnim->animFrames = realloc(tempAnim->animFrames, animFramesCapacity * sizeof(*tempAnim->animFrames));
							tempAnim->frameData.time = realloc(tempAnim->frameData.time, animFramesCapacity * sizeof(*tempAnim->frameData.time));
						}
						tempAnim->animFrames[tempAnim->frameData.numFrames] = tempFrame;
						tempAnim->frameData.time[tempAnim->frameData.numFrames] = frameTime;

						++tempAnim->frameData.numFrames;
					}


					currentLineX = 0;
					currentLineY = 0;
				}

			//Animation end.
			}else if(line[0] == '}'){
				if(currentAnim > 0){
					textureGroupAnim *tempAnim = &texGroup.texAnims[currentAnim - 1];

					//If the animation has no frames, delete it!
					if(tempAnim->frameData.numFrames == 0){
						texGroupAnimDelete(tempAnim);
						if(currentAnim != texGroup.numAnims){
							memmove(tempAnim, &texGroup.texAnims[currentAnim], (texGroup.numAnims - currentAnim) * sizeof(*texGroup.texAnims));
						}
						--texGroup.numAnims;

					//Otherwise, resize the frame vector so we aren't wasting memory!
					}else{
						tempAnim->animFrames = realloc(tempAnim->animFrames, tempAnim->frameData.numFrames * sizeof(*tempAnim->animFrames));
						tempAnim->frameData.time = realloc(tempAnim->frameData.time, tempAnim->frameData.numFrames * sizeof(*tempAnim->frameData.time));
					}

					currentAnim = 0;
					animFramesCapacity = 0;
				}
			}
		}


		//If no textures have been referenced, don't keep this!
		if(texIndicesSize == 0){
			index = -1;

		//Otherwise, if there are no animations, make one!
		}else if(texGroup.numAnims == 0){
			++texGroup.numAnims;
			texGroupAnimInit(texGroup.texAnims);
			texGroup.texAnims->name = malloc(8);
			strcpy(texGroup.texAnims->name, "default");

			++texGroup.texAnims->frameData.numFrames;
			texGroup.texAnims->frameData.time = malloc(sizeof(*texGroup.texAnims->frameData.time));
			texGroup.texAnims->frameData.time[0] = 0.f;
			texGroup.texAnims->animFrames = malloc(sizeof(*texGroup.texAnims->animFrames));
			texGroupFrameInit(&texGroup.texAnims->animFrames[0]);
			//Use the first texture that we loaded!
			texGroup.texAnims->animFrames[0].texID = ((texture *)vectorGet(&loadedTextures, *texIndices))->id;

		//If there are animations, resize the animation vector so we aren't wasting memory!
		}else{
			texGroup.texAnims = realloc(texGroup.texAnims, texGroup.numAnims * sizeof(*texGroup.texAnims));
		}

		if(texIndices != NULL){
			free(texIndices);
		}


		//If there wasn't an error, add the textureGroup to the vector!
		if(index != -1){
			texGroup.name = malloc(texGroupNameLength + 1);
			strcpy(texGroup.name, texGroupName);

			vectorAdd(&loadedTextureGroups, &texGroup, 1);
		}else{
			texGroupDelete(&texGroup);
		}


		fclose(texGroupFile);
	}else{
		printf("Unable to open textureGroup!\n"
		       "Path: %s\n"
		       "TextureGroup ID: %u\n", texGroupPath, index);
		index = -1;
	}

	if(texGroupPath != NULL){
		free(texGroupPath);
	}


	return(index);
}


void texGroupAnimInstSetAnim(textureGroupAnimInst *animInst, const size_t currentAnim){
	const textureGroup *tempTexGroup = (textureGroup *)vectorGet(&loadedTextureGroups, animInst->texGroupPos);
	if(currentAnim < tempTexGroup->numAnims){
		animationSetAnim(&animInst->animData, tempTexGroup->texAnims[currentAnim].frameData.playNum, currentAnim);
	}
}

void texGroupAnimInstUpdateAnim(textureGroupAnimInst *animInst, const float time){
	const textureGroup *tempTexGroup = (textureGroup *)vectorGet(&loadedTextureGroups, animInst->texGroupPos);
	if(animInst->animData.currentAnim < tempTexGroup->numAnims){
		animationUpdate(&animInst->animData, &(tempTexGroup->texAnims[animInst->animData.currentAnim].frameData), time);
	}
}

GLuint texGroupGetFrame(const textureGroup *texGroup, const size_t currentAnim, const size_t currentFrame, const GLuint uvOffsetsID){
	GLuint tempTexID;
	float uvOffsets[4];

	//If the animation and frame are valid, get the texture's I.D. and set the UV offsets!
	if(currentAnim < texGroup->numAnims && currentFrame < texGroup->texAnims[currentAnim].frameData.numFrames){
		textureGroupFrame *tempFrame = &(texGroup->texAnims[currentAnim].animFrames[currentFrame]);

		tempTexID    = tempFrame->texID;
		uvOffsets[0] = tempFrame->x;
		uvOffsets[1] = tempFrame->y;
		uvOffsets[2] = tempFrame->width;
		uvOffsets[3] = tempFrame->height;

	//If the animation or frame is invalid, use the error texture!
	}else{
		const texture *tempTex = (texture *)vectorGet(&loadedTextures, 0);
		tempTexID    = tempTex->id;
		uvOffsets[0] = 0.f;
		uvOffsets[1] = 0.f;
		uvOffsets[2] = tempTex->width;
		uvOffsets[3] = tempTex->height;
	}

	//Give the current UV offsets to the shader!
	glUniform1fv(uvOffsetsID, 4, uvOffsets);


	return(tempTexID);
}


void texGroupAnimDelete(textureGroupAnim *texGroupAnim){
	if(texGroupAnim->name != NULL){
		free(texGroupAnim->name);
	}
	if(texGroupAnim->animFrames != NULL){
		free(texGroupAnim->animFrames);
	}
	frameDataClear(&texGroupAnim->frameData);
}

void texGroupDelete(textureGroup *texGroup){
	if(texGroup->name != NULL){
		free(texGroup->name);
	}

	if(texGroup->texAnims != NULL){
		size_t i = texGroup->numAnims;
		while(i > 0){
			--i;
			texGroupAnimDelete(&texGroup->texAnims[i]);
		}
		free(texGroup->texAnims);
	}
}


//Loop through all of a textureGroup's animations until we find the one we need!
size_t texGroupFindAnimNameIndex(const textureGroup *texGroup, const char *name){
	size_t i;
	for(i = 0; i < texGroup->numAnims; ++i){
		if(strcmp(name, texGroup->texAnims[i].name) == 0){
			break;
		}
	}

	return(i);
}

//Loop through all the textureGroups we've loaded until we find the one we need!
size_t texGroupFindNameIndex(const char *name){
	size_t i;
	for(i = 0; i < loadedTextureGroups.size; ++i){
		if(strcmp(name, ((textureGroup *)vectorGet(&loadedTextureGroups, i))->name) == 0){
			break;
		}
	}

	return(i);
}


unsigned char textureGroupModuleSetup(){
	unsigned char success = 0;


	if(loadedTextures.size > 0){
		//Initialize our loadedTextureGroups vector.
		vectorInit(&loadedTextureGroups, sizeof(textureGroup));


		//Create an error textureGroup!
		textureGroup texGroup;

		//Initialize the error textureGroup!
		texGroup.name = malloc(6);
		strcpy(texGroup.name, "error");

		//Create an animation that uses the error texture!
		texGroup.numAnims = 1;
		texGroup.texAnims = malloc(sizeof(*texGroup.texAnims));
		texGroupAnimInit(&texGroup.texAnims[0]);
		texGroup.texAnims->name = malloc(8);
		strcpy(texGroup.texAnims->name, "default");

		++texGroup.texAnims->frameData.numFrames;
		texGroup.texAnims->frameData.time = malloc(sizeof(*texGroup.texAnims->frameData.time));
		texGroup.texAnims->frameData.time[0] = 0.f;
		texGroup.texAnims->animFrames = malloc(sizeof(*texGroup.texAnims->animFrames));
		texGroupFrameInit(&texGroup.texAnims->animFrames[0]);
		texGroup.texAnims->animFrames[0].texID = 1;

		//Add it to our vector!
		vectorAdd(&loadedTextureGroups, &texGroup, 1);


		success = 1;
	}


	return(success);
}

void textureGroupModuleCleanup(){
	size_t i = loadedTextureGroups.size;
	while(i > 0){
		--i;
		texGroupDelete((textureGroup *)vectorGet(&loadedTextureGroups, i));
		vectorRemove(&loadedTextureGroups, i);
	}
	vectorClear(&loadedTextureGroups);
}