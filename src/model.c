#include "model.h"


#define MODEL_PATH_PREFIX        ".\\resource\\models\\"
#define MODEL_PATH_PREFIX_LENGTH (sizeof(MODEL_PATH_PREFIX) - 1)


//These must be at least 1!
#define BASE_VERTEX_CAPACITY   1
#define BASE_INDEX_CAPACITY    BASE_VERTEX_CAPACITY
#define BASE_POSITION_CAPACITY BASE_VERTEX_CAPACITY
#define BASE_UV_CAPACITY       BASE_VERTEX_CAPACITY * 2
#define BASE_NORMAL_CAPACITY   BASE_VERTEX_CAPACITY
#define BASE_BONE_CAPACITY     1


#include <stdlib.h>
#include <string.h>

#include "utilString.h"

#include "vec3.h"
#include "quat.h"

#include "memoryManager.h"


typedef struct vertex {
	vec3 pos;
	float uvs[2];
	vec3 normal;

	size_t boneIDs[MODEL_VERTEX_MAX_WEIGHTS];
	float boneWeights[MODEL_VERTEX_MAX_WEIGHTS];
} vertex;


//By default, the error model only has a name.
//We need to set up the other data the hard way.
#warning "This should use the error skeleton."
model errorMdl = {
	.name     = "error",
	.texGroup = &errorTexGroup
};


//Forward-declare any helper functions!
static return_t vertexUnique(const vertex *v1, const vertex *v2);


#warning "What if we aren't using the global memory manager?"


void modelInit(model *mdl){
	mdl->name = NULL;

	mdl->vertexBufferID = 0;
	mdl->indexBufferID = 0;
	mdl->vertexArrayID = 0;
	mdl->totalIndices = 0;

	skeleInit(&mdl->skele);
	mdl->texGroup = 0;
}


//Load an OBJ using the model specified by "imgName".
return_t modelLoadOBJ(model *mdl, const char *mdlName){
	modelInit(mdl);


	//Find the full path for the model!
	const size_t mdlNameLength = strlen(mdlName);
	char *mdlPath = memoryManagerGlobalAlloc(MODEL_PATH_PREFIX_LENGTH + mdlNameLength + 1);
	if(mdlPath == NULL){
		/** MALLOC FAILED **/
	}
	memcpy(mdlPath, MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH);
	strcpy(mdlPath + MODEL_PATH_PREFIX_LENGTH, mdlName);

	//Load the model!
	FILE *mdlFile = fopen(mdlPath, "r");
	if(mdlFile != NULL){
		return_t success = 1;


		//Temporarily stores only unique vertices.
		size_t tempVerticesSize = 0;
		size_t tempVerticesCapacity = BASE_VERTEX_CAPACITY;
		vertex *tempVertices = memoryManagerGlobalAlloc(BASE_VERTEX_CAPACITY * sizeof(*tempVertices));
		if(tempVertices == NULL){
			/** MALLOC FAILED **/
		}

		//Temporarily stores vertex indices for faces.
		size_t tempIndicesSize = 0;
		size_t tempIndicesCapacity = BASE_INDEX_CAPACITY;
		size_t *tempIndices = memoryManagerGlobalAlloc(BASE_INDEX_CAPACITY * sizeof(*tempIndices));
		if(tempIndices == NULL){
			/** MALLOC FAILED **/
		}

		//Temporarily stores vertex positions, regardless of whether or not they are unique.
		size_t tempPositionsSize = 0;
		size_t tempPositionsCapacity = BASE_POSITION_CAPACITY;
		vec3 *tempPositions = memoryManagerGlobalAlloc(BASE_POSITION_CAPACITY * sizeof(*tempPositions));
		if(tempPositions == NULL){
			/** MALLOC FAILED **/
		}

		//Temporarily stores vertex UVs, regardless of whether or not they are unique.
		size_t tempUVsSize = 0;
		size_t tempUVsCapacity = BASE_UV_CAPACITY;
		float *tempUVs = memoryManagerGlobalAlloc(BASE_UV_CAPACITY * sizeof(*tempUVs));
		if(tempUVs == NULL){
			/** MALLOC FAILED **/
		}

		//Temporarily stores vertex normals, regardless of whether or not they are unique.
		size_t tempNormalsSize = 0;
		size_t tempNormalsCapacity = BASE_NORMAL_CAPACITY;
		vec3 *tempNormals = memoryManagerGlobalAlloc(BASE_NORMAL_CAPACITY * sizeof(*tempNormals));
		if(tempNormals == NULL){
			/** MALLOC FAILED **/
		}

		//Stores the name of the textureGroup that the model uses.
		char *tempTexGroupName = NULL;

		//Used when reading vertex data.
		char *startPos;
		char *endPos;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while(success && (line = readLineFile(mdlFile, &lineBuffer[0], &lineLength)) != NULL){
			//Vertex positions.
			if(memcmp(line, "v ", 2) == 0){
				//If we're out of space, allocate some more!
				if(tempPositionsSize >= tempPositionsCapacity){
					tempPositionsCapacity = tempPositionsSize * 2;
					tempPositions = memoryManagerGlobalRealloc(tempPositions, tempPositionsCapacity * sizeof(*tempPositions));
					if(tempPositions == NULL){
						/** REALLOC FAILED **/
					}
				}

				//Read the vertex positions from the line!
				startPos = &line[2];
				tempPositions[tempPositionsSize].x = strtod(startPos, &startPos);
				tempPositions[tempPositionsSize].y = strtod(startPos, &startPos);
				tempPositions[tempPositionsSize].z = strtod(startPos, &endPos);

				//If everything was successful, make sure we don't overwrite them next time!
				if(startPos != endPos || *endPos != '\0'){
					++tempPositionsSize;
				}

			//Vertex UVs.
			}else if(memcmp(line, "vt ", 3) == 0){
				//If we're out of space, allocate some more!
				if(tempUVsSize >= tempUVsCapacity){
					tempUVsCapacity = tempUVsSize * 2;
					tempUVs = memoryManagerGlobalRealloc(tempUVs, tempUVsCapacity * sizeof(*tempUVs));
					if(tempUVs == NULL){
						/** REALLOC FAILED **/
					}
				}

				//Read the vertex UVs from the line!
				startPos = &line[2];
				tempUVs[tempUVsSize]     = strtod(startPos, &startPos);
				tempUVs[tempUVsSize + 1] = -strtod(startPos, &endPos);

				//If everything was successful, make sure we don't overwrite them next time!
				if(startPos != endPos || *endPos != '\0'){
					tempUVsSize += 2;
				}

			//Vertex normals.
			}else if(memcmp(line, "vn ", 3) == 0){
				//If we're out of space, allocate some more!
				if(tempNormalsSize >= tempNormalsCapacity){
					tempNormalsCapacity = tempNormalsSize * 2;
					tempNormals = memoryManagerGlobalRealloc(tempNormals, tempNormalsCapacity * sizeof(*tempNormals));
					if(tempNormals == NULL){
						/** REALLOC FAILED **/
					}
				}

				//Read the vertex normals from the line!
				startPos = &line[2];
				tempNormals[tempNormalsSize].x = strtod(startPos, &startPos);
				tempNormals[tempNormalsSize].y = strtod(startPos, &startPos);
				tempNormals[tempNormalsSize].z = strtod(startPos, &endPos);

				//If everything was successful, make sure we don't overwrite them next time!
				if(startPos != endPos || *endPos != '\0'){
					++tempNormalsSize;
				}

			//TextureGroup path.
			}else if(memcmp(line, "usemtl ", 7) == 0){
				//We don't want to be keeping textureGroups that aren't used, so
				//we'll load this once we can be sure everything else was successful.
				//Note that we use realloc, so we only really store one texture.
				tempTexGroupName = memoryManagerGlobalRealloc(tempTexGroupName, (lineLength - 6) * sizeof(*tempTexGroupName));
				if(tempTexGroupName == NULL){
					/** REALLOC FAILED **/
				}
				//Our line reading function replaces the line break with a null terminator,
				//so we need to store it too.
				memcpy(tempTexGroupName, line + 7, (lineLength - 6) * sizeof(*tempTexGroupName));

			//Faces.
			}else if(memcmp(line, "f ", 2) == 0){
				char *curTok = strtok(&line[2], " ");
				size_t a;
				//If the vertex we want to add already exists, create an index to it!
				//Otherwise, add it to the vector!
				for(a = 0; a < 3 || curTok != NULL; ++a){
					vertex tempVertex;
					memset(&tempVertex, 0, sizeof(tempVertex));

					size_t posIndex, uvIndex, normalIndex;
					//Read the indices!
					posIndex = strtoul(curTok, &curTok, 10) - 1;
					++curTok;
					uvIndex = strtoul(curTok, &curTok, 10) - 1;
					++curTok;
					normalIndex = strtoul(curTok, &curTok, 10) - 1;

					//Fill up tempVertex with the vertex information we've stored! If the index is invalid, store a 0!
					if(posIndex < tempPositionsSize){
						tempVertex.pos = tempPositions[posIndex];
					}
					if(uvIndex < tempUVsSize){
						memcpy(tempVertex.uvs, &tempUVs[uvIndex * 2], sizeof(tempVertex.uvs));
					}
					if(normalIndex < tempNormalsSize){
						tempVertex.normal = tempNormals[normalIndex];
					}

					/** This is only temporary since we don't support bones here yet. **/
					tempVertex.boneIDs[0] = 0;
					memset(&tempVertex.boneIDs[1], -1, (MODEL_VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneIDs[0]));
					tempVertex.boneWeights[0] = 1.f;
					memset(&tempVertex.boneWeights[1], 0.f, (MODEL_VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneWeights[0]));


					const vertex *checkVertex = tempVertices;
					size_t b;
					//Check if this vertex already exists!
					for(b = 0; b < tempVerticesSize; ++b, ++checkVertex){
						//Looks like it does, so we don't need to store it again!
						if(vertexUnique(checkVertex, &tempVertex) == 0){
							break;
						}
					}
					//The vertex does not exist, so add it to the vector!
					if(b == tempVerticesSize){
						//If we're out of space, allocate some more!
						if(tempVerticesSize >= tempVerticesCapacity){
							tempVerticesCapacity = tempVerticesSize * 2;
							tempVertices = memoryManagerGlobalRealloc(tempVertices, tempVerticesCapacity * sizeof(*tempVertices));
							if(tempVertices == NULL){
								/** REALLOC FAILED **/
							}
						}
						tempVertices[tempVerticesSize] = tempVertex;
						++tempVerticesSize;
					}
					//If this face has more than three vertices, it needs to be split up!
					//We'll need to store two additional indices for every extra face.
					if(a >= 3){
						tempIndicesSize += 2;
						if(tempIndicesSize >= tempIndicesCapacity){
							tempIndicesCapacity = tempIndicesSize * 2;
							tempIndices = memoryManagerGlobalRealloc(tempIndices, tempIndicesCapacity * sizeof(*tempIndices));
							if(tempIndices == NULL){
								/** REALLOC FAILED **/
							}
						}
						//Get the index of the first vertex that this 'f' statement used.
						tempIndices[tempIndicesSize - 2] = tempIndices[tempIndicesSize - 2 - a];
						//Now get the index of the last vertex that this 'f' statement used.
						tempIndices[tempIndicesSize - 1] = tempIndices[tempIndicesSize - 3];

					//Otherwise, we only need to store one index!
					}else{
						if(tempIndicesSize >= tempIndicesCapacity){
							tempIndicesCapacity = tempIndicesSize * 2;
							tempIndices = memoryManagerGlobalRealloc(tempIndices, tempIndicesCapacity * sizeof(*tempIndices));
							if(tempIndices == NULL){
								/** REALLOC FAILED **/
							}
						}
					}
					tempIndices[tempIndicesSize] = b;
					++tempIndicesSize;


					//Get the beginning of the next vertex's data!
					curTok = strtok(NULL, " ");
				}

			//Syntax error.
			}else if(lineLength > 0){
				printf(
					"Syntax error loading model!\n"
					"Path: %s\n"
					"Line: %s\n",
					mdlPath, line
				);

				success = 0;
			}
		}

		fclose(mdlFile);


		//If there wasn't an error, add the model to the vector!
		if(success){
			//Generate a vertex array object for our model and bind it!
			glGenVertexArrays(1, &mdl->vertexArrayID);
			glBindVertexArray(mdl->vertexArrayID);
				//Generate a vertex buffer object for our vertex data and bind it!
				glGenBuffers(1, &mdl->vertexBufferID);
				glBindBuffer(GL_ARRAY_BUFFER, mdl->vertexBufferID);
				//Now add all our data to it!
				glBufferData(GL_ARRAY_BUFFER, tempVerticesSize * sizeof(*tempVertices), &tempVertices[0], GL_STATIC_DRAW);

				//Generate a vertex buffer object for our indices and bind it!
				glGenBuffers(1, &mdl->indexBufferID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->indexBufferID);
				//Now add all our data to it!
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, tempIndicesSize * sizeof(*tempIndices), &tempIndices[0], GL_STATIC_DRAW);


				//Models will need these vertex attributes!
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				glEnableVertexAttribArray(4);

				//Tell OpenGL which data belongs to the vertex attributes!
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, pos));
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, uvs));
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, normal));
				glVertexAttribIPointer(3, MODEL_VERTEX_MAX_WEIGHTS, GL_UNSIGNED_INT, sizeof(vertex), (GLvoid *)offsetof(vertex, boneIDs));
				glVertexAttribPointer(4, MODEL_VERTEX_MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, boneWeights));
			//Unbind the array object!
			glBindVertexArray(0);


			//Set the model's name!
			mdl->name = memoryManagerGlobalRealloc(mdlPath, mdlNameLength + 1);
			if(mdl->name == NULL){
				/** REALLOC FAILED **/
			}
			strcpy(mdl->name, mdlName);

			//Initialise the model's skeleton!
			skeleInitSet(&mdl->skele, mdl->name, NULL, 0);

			//Now that we can be sure everything was
			//successful, find the textureGroup.
			if(tempTexGroupName != NULL){
				mdl->texGroup = moduleTexGroupFind(tempTexGroupName);
				//If we couldn't find the textureGroup, load it!
				if(mdl->texGroup == NULL){
					//Make sure we can allocate
					//enough memory for the texture.
					if(!(mdl->texGroup = moduleTexGroupAlloc())){
						/** MALLOC FAILED **/
					}
					//If we can't load it, just
					//use the error texture.
					if(!texGroupLoad(mdl->texGroup, tempTexGroupName)){
						moduleTexGroupFree(mdl->texGroup);
						mdl->texGroup = &errorTexGroup;
					}
				}
			}

		//Otherwise, delete the model.
		}else{
			modelDelete(mdl);
			memoryManagerGlobalFree(mdlPath);
		}


		//We don't need to check if these are NULL,
		//as we do that when we're using them.
		memoryManagerGlobalFree(tempVertices);
		memoryManagerGlobalFree(tempIndices);
		memoryManagerGlobalFree(tempPositions);
		memoryManagerGlobalFree(tempUVs);
		memoryManagerGlobalFree(tempNormals);
		//This one, however, is allowed to be NULL.
		if(tempTexGroupName != NULL){
			memoryManagerGlobalFree(tempTexGroupName);
		}


		return(success);
	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			mdlPath
		);
	}

	memoryManagerGlobalFree(mdlPath);


	return(0);
}

/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
//Load an SMD using the model specified by "imgName".
return_t modelLoadSMD(model *mdl, const char *mdlName){
	modelInit(mdl);


	//Find the full path for the model!
	const size_t mdlNameLength = strlen(mdlName);
	char *mdlPath = malloc(MODEL_PATH_PREFIX_LENGTH + mdlNameLength + 1);
	memcpy(mdlPath, MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH);
	strcpy(mdlPath + MODEL_PATH_PREFIX_LENGTH, mdlName);

	//Load the model!
	FILE *mdlFile = fopen(mdlPath, "r");
	if(mdlFile != NULL){
		return_t success = 1;


		//Temporarily stores only unique vertices.
		size_t tempVerticesSize = 0;
		size_t tempVerticesCapacity = BASE_VERTEX_CAPACITY;
		vertex *tempVertices = memoryManagerGlobalAlloc(BASE_VERTEX_CAPACITY * sizeof(*tempVertices));
		if(tempVertices == NULL){
			/** MALLOC FAILED **/
		}

		//Temporarily stores vertex indices for faces.
		size_t tempIndicesSize = 0;
		size_t tempIndicesCapacity = BASE_INDEX_CAPACITY;
		size_t *tempIndices = memoryManagerGlobalAlloc(BASE_INDEX_CAPACITY * sizeof(*tempIndices));
		if(tempIndices == NULL){
			/** MALLOC FAILED **/
		}

		//Temporarily stores bones.
		size_t tempBonesSize = 0;
		size_t tempBonesCapacity = BASE_BONE_CAPACITY;
		bone *tempBones = memoryManagerGlobalAlloc(BASE_BONE_CAPACITY * sizeof(*tempBones));
		if(tempBones == NULL){
			/** MALLOC FAILED **/
		}

		//This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		//This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[1024];
		char *line;
		size_t lineLength;


		while(success && (line = readLineFile(mdlFile, &lineBuffer[0], &lineLength)) != NULL){
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;
				}else if(strcmp(line, "triangles") == 0){
					dataType = 3;

				//If this isn't the version number and the line isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf(
						"Error loading model!\n"
						"Path: %s\n"
						"Line: %s\n"
						"Error: Unexpected identifier!\n",
						mdlPath, line
					);

					success = 0;
				}
			}else{
				if(strcmp(line, "end") == 0){
					//If we've finished identifying the skeleton's bones, shrink the vector!
					if(dataType == 1){
						tempBonesCapacity = tempBonesSize;
						tempBones = memoryManagerGlobalRealloc(tempBones, tempBonesCapacity * sizeof(*tempBones));
						if(tempBones == NULL){
							/** REALLOC FAILED **/
						}
					}

					dataType = 0;
					data = 0;
				}else{
					if(dataType == 1){
						char *tokPos = line;

						bone tempBone;

						//Get this bone's ID.
						size_t boneID = strtoul(tokPos, &tokPos, 10);
						if(boneID == tempBonesSize){
							//Get the bone's name.
							size_t boneNameLength;
							getDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &tokPos, &boneNameLength);
							tempBone.name = malloc(boneNameLength + 1);
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							//Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);


							//If we're out of space, allocate some more!
							if(tempBonesSize >= tempBonesCapacity){
								tempBonesCapacity = tempBonesSize * 2;
								tempBones = memoryManagerGlobalRealloc(tempBones, tempBonesCapacity * sizeof(*tempBones));
								if(tempBones == NULL){
									/** REALLOC FAILED **/
								}
							}
							//Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf(
								"Error loading model!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Found node %u when expecting node %u!\n",
								mdlPath, line, boneID, tempBonesSize
							);

							success = 0;
						}
					}else if(dataType == 2){
						//If the line begins with time, get the frame's timestamp!
						if(memcmp(line, "time ", 5) == 0){
							unsigned int newTime = strtoul(&line[5], NULL, 10);
							if(newTime >= data){
								data = newTime;
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Frame timestamps do not increment sequentially!\n",
									mdlPath, line
								);

								success = 0;
							}

						//Otherwise, we're setting the bone's state!
						}else{
							char *tokPos = line;

							//Get this bone's ID.
							size_t boneID = strtoul(tokPos, &tokPos, 10);
							//Make sure a bone with this ID actually exists.
							if(boneID < tempBonesSize){
								bone *currentBone = &tempBones[boneID];

								//If the current frame timestamp is 0, set the bone's initial state!
								if(data == 0){
									//Load the bone's position!
									float x = strtod(tokPos, &tokPos) * 0.05f;
									float y = strtod(tokPos, &tokPos) * 0.05f;
									float z = strtod(tokPos, &tokPos) * 0.05f;
									vec3InitSet(&currentBone->state.pos, x, y, z);

									//Load the bone's rotation!
									x = strtod(tokPos, &tokPos);
									y = strtod(tokPos, &tokPos);
									z = strtod(tokPos, NULL);
									quatInitEulerRad(&currentBone->state.rot, x, y, z);

									//Set the bone's scale!
									vec3InitSet(&currentBone->state.scale, 1.f, 1.f, 1.f);


									//If this bone has a parent, append its state to its parent's state!
									if(currentBone->parent != -1){
										boneStateAddTransform(&tempBones[currentBone->parent].state, &currentBone->state, &currentBone->state);
									}
								}
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Found skeletal data for bone %u, which doesn't exist!\n",
									mdlPath, line, boneID
								);

								success = 0;
							}
						}
					}else if(dataType == 3){
						if(data == 0){
							//This indicates the texture that the face uses.
						}else{
							char *tokPos = line;

							vertex tempVertex;
							memset(&tempVertex, 0, sizeof(tempVertex));

							//Read the vertex data from the line!
							size_t parentBoneID = strtoul(tokPos, &tokPos, 10);
							//Make sure a bone with this ID actually exists.
							if(parentBoneID < tempBonesSize){
								tempVertex.pos.x = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.pos.y = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.pos.z = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.normal.x = strtod(tokPos, &tokPos);
								tempVertex.normal.y = strtod(tokPos, &tokPos);
								tempVertex.normal.z = strtod(tokPos, &tokPos);
								tempVertex.uvs[0] = strtod(tokPos, &tokPos);
								tempVertex.uvs[1] = -strtod(tokPos, &tokPos);
								size_t numLinks = strtoul(tokPos, &tokPos, 10);
								//Make sure some links were specified.
								if(numLinks > 0){
									//If there are more than the maximum number of supported weights, we'll have to clamp it down!
									if(numLinks > MODEL_VERTEX_MAX_WEIGHTS){
										printf(
											"Error loading model!\n"
											"Path: %s\n"
											"Line: %s\n"
											"Error: Vertex has too many links! All extra links will be ignored.\n",
											mdlPath, line
										);

										numLinks = MODEL_VERTEX_MAX_WEIGHTS;
									}

									size_t parentPos = -1;
									float totalWeight = 0.f;

									size_t *curBoneID = tempVertex.boneIDs;
									float *curBoneWeight = tempVertex.boneWeights;
									size_t i;
									//Load all of the links!
									for(i = 0; i < numLinks; ++i, ++curBoneID, ++curBoneWeight){
										//Load the bone's ID!
										*curBoneID = strtoul(tokPos, &tokPos, 10);
										//Make sure it exists!
										if(*curBoneID > tempBonesSize){
											printf(
												"Error loading model!\n"
												"Path: %s\n"
												"Line: %s\n"
												"Error: Vertex link bone doesn't exist! The parent bone will be used instead.\n",
												mdlPath, line
											);

										//If we're loading the parent bone, remember its position!
										}else if(*curBoneID == parentBoneID){
											parentPos = i;
										}

										//Load the bone's weights!
										*curBoneWeight = strtod(tokPos, &tokPos);
										totalWeight += *curBoneWeight;
										//Make sure the total weight doesn't exceed 1!
										if(totalWeight > 1.f){
											*curBoneWeight -= totalWeight - 1.f;
											totalWeight = 1.f;

											++i;
											break;
										}
									}

									//Make sure the total weight isn't less than 1!
									if(totalWeight < 1.f){
										//If we never loaded the parent bone, see if we can add it!
										if(parentPos == -1){
											if(i < MODEL_VERTEX_MAX_WEIGHTS){
												*curBoneID = parentBoneID;
												*curBoneWeight = 0.f;
												parentPos = i;
												++i;

											//If there's no room, just use the first bone we loaded.
											}else{
												parentPos = 0;
											}
										}

										tempVertex.boneWeights[parentPos] += 1.f - totalWeight;
									}

									//Make sure we fill the rest with invalid values so we know they aren't used.
									memset(&curBoneID, -1, (MODEL_VERTEX_MAX_WEIGHTS - i) * sizeof(*tempVertex.boneIDs));
									memset(&curBoneWeight, 0.f, (MODEL_VERTEX_MAX_WEIGHTS - i) * sizeof(*tempVertex.boneWeights));

								//Otherwise, just bind it to the parent bone.
								}else{
									printf(
										"Error loading model!\n"
										"Path: %s\n"
										"Line: %s\n"
										"Error: Vertex has no links! The parent bone will be used.\n",
										mdlPath, line
									);

									tempVertex.boneIDs[0] = parentBoneID;
									memset(&tempVertex.boneIDs[1], -1, (MODEL_VERTEX_MAX_WEIGHTS - 1) * sizeof(*tempVertex.boneIDs));
									tempVertex.boneWeights[0] = 1.f;
									memset(&tempVertex.boneWeights[1], 0.f, (MODEL_VERTEX_MAX_WEIGHTS - 1) * sizeof(*tempVertex.boneWeights));
								}


								const vertex *checkVertex = tempVertices;
								size_t i;
								//Check if this vertex already exists!
								for(i = 0; i < tempVerticesSize; ++i, ++checkVertex){
									//Looks like it does, so we don't need to store it again!
									if(vertexUnique(checkVertex, &tempVertex) == 0){
										break;
									}
								}
								//The vertex does not exist, so add it to the vector!
								if(i == tempVerticesSize){
									//If we're out of space, allocate some more!
									if(tempVerticesSize >= tempVerticesCapacity){
										tempVerticesCapacity = tempVerticesSize * 2;
										tempVertices = memoryManagerGlobalRealloc(tempVertices, tempVerticesCapacity * sizeof(*tempVertices));
										if(tempVertices == NULL){
											/** REALLOC FAILED **/
										}
									}
									tempVertices[tempVerticesSize] = tempVertex;
									++tempVerticesSize;
								}
								//Add an index for the new vertex!
								if(tempIndicesSize >= tempIndicesCapacity){
									tempIndicesCapacity = tempIndicesSize * 2;
									tempIndices = memoryManagerGlobalRealloc(tempIndices, tempIndicesCapacity * sizeof(*tempIndices));
									if(tempIndices == NULL){
										/** REALLOC FAILED **/
									}
								}
								tempIndices[tempIndicesSize] = i;
								++tempIndicesSize;
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Vertex parent bone doesn't exist!\n",
									mdlPath, line
								);

								success = 0;
							}
						}

						if(data < 3){
							++data;
						}else{
							data = 0;
						}
					}
				}
			}
		}

		fclose(mdlFile);


		//If there wasn't an error, add the model to the vector!
		if(success){
			//Generate a vertex array object for our model and bind it!
			glGenVertexArrays(1, &mdl->vertexArrayID);
			glBindVertexArray(mdl->vertexArrayID);
				//Generate a vertex buffer object for our vertex data and bind it!
				glGenBuffers(1, &mdl->vertexBufferID);
				glBindBuffer(GL_ARRAY_BUFFER, mdl->vertexBufferID);
				//Now add all our data to it!
				glBufferData(GL_ARRAY_BUFFER, tempVerticesSize * sizeof(*tempVertices), &tempVertices[0], GL_STATIC_DRAW);

				//Generate a vertex buffer object for our indices and bind it!
				glGenBuffers(1, &mdl->indexBufferID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdl->indexBufferID);
				//Now add all our data to it!
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mdl->totalIndices * sizeof(*tempIndices), &tempIndices[0], GL_STATIC_DRAW);


				//Models will need these vertex attributes!
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(3);
				glEnableVertexAttribArray(4);

				//Tell OpenGL which data belongs to the vertex attributes!
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, pos));
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, uvs));
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, normal));
				glVertexAttribIPointer(3, MODEL_VERTEX_MAX_WEIGHTS, GL_UNSIGNED_INT, sizeof(vertex), (GLvoid *)offsetof(vertex, boneIDs));
				glVertexAttribPointer(4, MODEL_VERTEX_MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid *)offsetof(vertex, boneWeights));
			//Unbind the array object!
			glBindVertexArray(0);


			//Set the model's name!
			mdl->name = memoryManagerGlobalRealloc(mdlPath, mdlNameLength + 1);
			if(mdl->name == NULL){
				/** REALLOC FAILED **/
			}
			strcpy(mdl->name, mdlName);

			//Initialise the model's skeleton!
			skeleInitSet(&mdl->skele, mdl->name, NULL, 0);

			#warning "This is obviously incomplete."
			//Now that we can be sure everything was
			//successful, find the textureGroup.
			//if(tempTexGroupName != NULL){
			mdl->texGroup = moduleTexGroupFind("soldier.tdt");
			//If we couldn't find the textureGroup, load it!
			if(mdl->texGroup == NULL){
				//Make sure we can allocate
				//enough memory for the texture.
				if(!(mdl->texGroup = moduleTexGroupAlloc())){
					/** MALLOC FAILED **/
				}
				//If we can't load it, just
				//use the error texture.
				if(!texGroupLoad(mdl->texGroup, "soldier.tdt")){
					moduleTexGroupFree(mdl->texGroup);
					mdl->texGroup = &errorTexGroup;
				}
			}
			//}

		//Otherwise, delete the model.
		}else{
			modelDelete(mdl);
			memoryManagerGlobalFree(mdlPath);
		}


		//We don't need to check if these are NULL,
		//as we do that when we're using them.
		memoryManagerGlobalFree(tempVertices);
		memoryManagerGlobalFree(tempIndices);
		memoryManagerGlobalFree(tempBones);
		//This one, however, is allowed to be NULL.
		/*if(tempTexGroupName != NULL){
			memoryManagerGlobalFree(tempTexGroupName);
		}*/


		return(success);
	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			mdlPath
		);
	}

	memoryManagerGlobalFree(mdlPath);


	return(0);
}

return_t modelSetupError(){
	vec3 tempPositions[8];
	tempPositions[0].x =  1.f; tempPositions[0].y = 2.f; tempPositions[0].z = -1.f;
	tempPositions[1].x = -1.f; tempPositions[1].y = 0.f; tempPositions[1].z = -1.f;
	tempPositions[2].x = -1.f; tempPositions[2].y = 2.f; tempPositions[2].z = -1.f;
	tempPositions[3].x =  1.f; tempPositions[3].y = 2.f; tempPositions[3].z =  1.f;
	tempPositions[4].x =  1.f; tempPositions[4].y = 0.f; tempPositions[4].z = -1.f;
	tempPositions[5].x = -1.f; tempPositions[5].y = 2.f; tempPositions[5].z =  1.f;
	tempPositions[6].x =  1.f; tempPositions[6].y = 0.f; tempPositions[6].z =  1.f;
	tempPositions[7].x = -1.f; tempPositions[7].y = 0.f; tempPositions[7].z =  1.f;

	float tempUVs[20][2];
	tempUVs[0][0]  = 0.f; tempUVs[0][1]  = 1.f;
	tempUVs[1][0]  = 1.f; tempUVs[1][1]  = 0.f;
	tempUVs[2][0]  = 1.f; tempUVs[2][1]  = 1.f;
	tempUVs[3][0]  = 0.f; tempUVs[3][1]  = 1.f;
	tempUVs[4][0]  = 1.f; tempUVs[4][1]  = 0.f;
	tempUVs[5][0]  = 1.f; tempUVs[5][1]  = 1.f;
	tempUVs[6][0]  = 0.f; tempUVs[6][1]  = 1.f;
	tempUVs[7][0]  = 1.f; tempUVs[7][1]  = 0.f;
	tempUVs[8][0]  = 1.f; tempUVs[8][1]  = 1.f;
	tempUVs[9][0]  = 0.f; tempUVs[9][1]  = 1.f;
	tempUVs[10][0] = 1.f; tempUVs[10][1] = 0.f;
	tempUVs[11][0] = 1.f; tempUVs[11][1] = 1.f;
	tempUVs[12][0] = 0.f; tempUVs[12][1] = 1.f;
	tempUVs[13][0] = 0.f; tempUVs[13][1] = 0.f;
	tempUVs[14][0] = 1.f; tempUVs[14][1] = 0.f;
	tempUVs[15][0] = 0.f; tempUVs[15][1] = 0.f;
	tempUVs[16][0] = 0.f; tempUVs[16][1] = 0.f;
	tempUVs[17][0] = 0.f; tempUVs[17][1] = 0.f;
	tempUVs[18][0] = 0.f; tempUVs[18][1] = 0.f;
	tempUVs[19][0] = 1.f; tempUVs[19][1] = 1.f;

	vec3 tempNormals[8];
	tempNormals[0].x =  0.5774f; tempNormals[0].y =  0.5774f; tempNormals[0].z = -0.5774f;
	tempNormals[1].x = -0.5774f; tempNormals[1].y = -0.5774f; tempNormals[1].z = -0.5774f;
	tempNormals[2].x = -0.5774f; tempNormals[2].y =  0.5774f; tempNormals[2].z = -0.5774f;
	tempNormals[3].x =  0.5774f; tempNormals[3].y =  0.5774f; tempNormals[3].z =  0.5774f;
	tempNormals[4].x =  0.5774f; tempNormals[4].y = -0.5774f; tempNormals[4].z = -0.5774f;
	tempNormals[5].x = -0.5774f; tempNormals[5].y =  0.5774f; tempNormals[5].z =  0.5774f;
	tempNormals[6].x =  0.5774f; tempNormals[6].y = -0.5774f; tempNormals[6].z =  0.5774f;
	tempNormals[7].x = -0.5774f; tempNormals[7].y = -0.5774f; tempNormals[7].z =  0.5774f;

	size_t tempIndices[108];
	tempIndices[0]   = 0; tempIndices[1]   =  0; tempIndices[2]   = 0;
	tempIndices[3]   = 1; tempIndices[4]   =  1; tempIndices[5]   = 1;
	tempIndices[6]   = 2; tempIndices[7]   =  2; tempIndices[8]   = 2;
	tempIndices[9]   = 3; tempIndices[10]  =  3; tempIndices[11]  = 3;
	tempIndices[12]  = 4; tempIndices[13]  =  4; tempIndices[14]  = 4;
	tempIndices[15]  = 0; tempIndices[16]  =  5; tempIndices[17]  = 0;
	tempIndices[18]  = 5; tempIndices[19]  =  6; tempIndices[20]  = 5;
	tempIndices[21]  = 6; tempIndices[22]  =  7; tempIndices[23]  = 6;
	tempIndices[24]  = 3; tempIndices[25]  =  8; tempIndices[26]  = 3;
	tempIndices[27]  = 2; tempIndices[28]  =  9; tempIndices[29]  = 2;
	tempIndices[30]  = 7; tempIndices[31]  = 10; tempIndices[32]  = 7;
	tempIndices[33]  = 5; tempIndices[34]  = 11; tempIndices[35]  = 5;
	tempIndices[36]  = 4; tempIndices[37]  =  4; tempIndices[38]  = 4;
	tempIndices[39]  = 7; tempIndices[40]  = 12; tempIndices[41]  = 7;
	tempIndices[42]  = 1; tempIndices[43]  = 13; tempIndices[44]  = 1;
	tempIndices[45]  = 3; tempIndices[46]  = 14; tempIndices[47]  = 3;
	tempIndices[48]  = 2; tempIndices[49]  =  9; tempIndices[50]  = 2;
	tempIndices[51]  = 5; tempIndices[52]  = 15; tempIndices[53]  = 5;
	tempIndices[54]  = 0; tempIndices[55]  =  0; tempIndices[56]  = 0;
	tempIndices[57]  = 4; tempIndices[58]  = 16; tempIndices[59]  = 4;
	tempIndices[60]  = 1; tempIndices[61]  =  1; tempIndices[62]  = 1;
	tempIndices[63]  = 3; tempIndices[64]  =  3; tempIndices[65]  = 3;
	tempIndices[66]  = 6; tempIndices[67]  = 17; tempIndices[68]  = 6;
	tempIndices[69]  = 4; tempIndices[70]  =  4; tempIndices[71]  = 4;
	tempIndices[72]  = 5; tempIndices[73]  =  6; tempIndices[74]  = 5;
	tempIndices[75]  = 7; tempIndices[76]  = 18; tempIndices[77]  = 7;
	tempIndices[78]  = 6; tempIndices[79]  =  7; tempIndices[80]  = 6;
	tempIndices[81]  = 2; tempIndices[82]  =  9; tempIndices[83]  = 2;
	tempIndices[84]  = 1; tempIndices[85]  = 13; tempIndices[86]  = 1;
	tempIndices[87]  = 7; tempIndices[88]  = 10; tempIndices[89]  = 7;
	tempIndices[90]  = 4; tempIndices[91]  =  4; tempIndices[92]  = 4;
	tempIndices[93]  = 6; tempIndices[94]  = 19; tempIndices[95]  = 6;
	tempIndices[96]  = 7; tempIndices[97]  = 12; tempIndices[98]  = 7;
	tempIndices[99]  = 3; tempIndices[100] = 14; tempIndices[101] = 3;
	tempIndices[102] = 0; tempIndices[103] =  5; tempIndices[104] = 0;
	tempIndices[105] = 2; tempIndices[106] =  9; tempIndices[107] = 2;


	//Temporarily stores only unique vertices.
	size_t verticesSize = 0;
	vertex vertices[20];
	//Temporarily stores vertex indices for faces.
	//We can use the model's totalIndices variable for the size so we don't have to set it later.
	errorMdl.totalIndices = 0;
	size_t indices[106];

	size_t a;
	for(a = 0; a < sizeof(tempIndices) / sizeof(*tempIndices); a += 3){
		vertex tempVertex;
		memset(&tempVertex, 0, sizeof(tempVertex));

		//Fill up tempVertex with the vertex information we've stored!
		tempVertex.pos = tempPositions[tempIndices[a]];
		memcpy(tempVertex.uvs, &tempUVs[tempIndices[a + 1]][0], sizeof(tempVertex.uvs));
		tempVertex.normal = tempNormals[tempIndices[a + 2]];

		tempVertex.boneIDs[0] = 0;
		memset(&tempVertex.boneIDs[1], -1, (MODEL_VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneIDs[0]));
		tempVertex.boneWeights[0] = 1.f;
		memset(&tempVertex.boneWeights[1], 0.f, (MODEL_VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneWeights[0]));


		const vertex *checkVertex = vertices;
		size_t b;
		//Check if this vertex already exists!
		for(b = 0; b < verticesSize; ++b, ++checkVertex){
			//Looks like it does, so we don't need to store it again!
			if(vertexUnique(checkVertex, &tempVertex) == 0){
				break;
			}
		}
		//The vertex does not exist, so add it to the vector!
		if(b == verticesSize){
			vertices[verticesSize] = tempVertex;
			++verticesSize;
		}
		//Add an index for the new vertex!
		indices[errorMdl.totalIndices] = b;
		++errorMdl.totalIndices;
	}


	//Generate a vertex array object for our model and bind it!
	glGenVertexArrays(1, &errorMdl.vertexArrayID);
	glBindVertexArray(errorMdl.vertexArrayID);
		//Generate a vertex buffer object for our vertex data and bind it!
		glGenBuffers(1, &errorMdl.vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, errorMdl.vertexBufferID);
		//Now add all our data to it!
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(*vertices), &vertices[0], GL_STATIC_DRAW);

		//Generate a vertex buffer object for our indices and bind it!
		glGenBuffers(1, &errorMdl.indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, errorMdl.indexBufferID);
		//Now add all our data to it!
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (errorMdl.totalIndices * sizeof(*indices)), &indices[0], GL_STATIC_DRAW);


		//Models will need these three vertex attributes!
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//Tell OpenGL which data belongs to the vertex attributes!
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, uvs));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)offsetof(vertex, normal));
	//Unbind the array object!
	glBindVertexArray(0);


	return(1);
}


void modelDelete(model *mdl){
	//Only free the name if it's in
	//use and it's not the error model.
	if(mdl->name != NULL && mdl != &errorMdl){
		memoryManagerGlobalFree(mdl->name);
	}

	if(mdl->vertexBufferID != 0){
		glDeleteBuffers(1, &mdl->vertexBufferID);
	}
	if(mdl->indexBufferID != 0){
		glDeleteBuffers(1, &mdl->indexBufferID);
	}
	if(mdl->vertexArrayID != 0){
		glDeleteVertexArrays(1, &mdl->vertexArrayID);
	}
}


//This works so long as our vertices have no padding.
static return_t vertexUnique(const vertex *v1, const vertex *v2){
	return(memcmp(v1, v2, sizeof(*v2)));
}