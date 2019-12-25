#include "model.h"


#include <stdlib.h>
#include <string.h>

#include "vec2.h"
#include "vec3.h"
#include "quat.h"
#include "transform.h"
#include "vertex.h"

#include "memoryManager.h"
#include "moduleSkeleton.h"
#include "moduleTextureGroup.h"

#include "utilString.h"
#include "utilFile.h"


#define MODEL_PATH_PREFIX        ".\\resource\\models\\"
#define MODEL_PATH_PREFIX_LENGTH (sizeof(MODEL_PATH_PREFIX) - 1)

// These must be at least 1!
#define BASE_VERTEX_CAPACITY   1
#define BASE_INDEX_CAPACITY    BASE_VERTEX_CAPACITY
#define BASE_POSITION_CAPACITY BASE_VERTEX_CAPACITY
#define BASE_UV_CAPACITY       BASE_VERTEX_CAPACITY
#define BASE_NORMAL_CAPACITY   BASE_VERTEX_CAPACITY
#define BASE_BONE_CAPACITY     1


// By default, the error model only has a name.
// We need to set up the other data the hard way.
model mdlDefault = {
	.name     = "error",
	.skele    = &skeleDefault,
	.texGroup = &texGroupDefault
};


#warning "What if we aren't using the global memory manager?"


void modelInit(model *mdl){
	mdl->name = NULL;

	meshInit(&mdl->meshData);
	mdl->skele = &skeleDefault;
	mdl->texGroup = &texGroupDefault;
}


// Load an OBJ using the model specified by "mdlPath".
return_t modelLoadOBJ(model *mdl, const char *mdlPath){
	FILE *mdlFile;
	char mdlFullPath[FILE_MAX_LINE_LENGTH];
	size_t mdlPathLength = strlen(mdlPath);

	// Find the full path for the model!
	fileGenerateFullPath(
		mdlPath, mdlPathLength,
		MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH,
		mdlFullPath
	);


	modelInit(mdl);


	// Load the model!
	mdlFile = fopen(mdlFullPath, "r");
	if(mdlFile != NULL){
		return_t success = 1;


		// Temporarily stores only unique vertices.
		size_t tempVerticesSize = 0;
		size_t tempVerticesCapacity = BASE_VERTEX_CAPACITY;
		vertex *tempVertices = memoryManagerGlobalAlloc(BASE_VERTEX_CAPACITY * sizeof(*tempVertices));
		if(tempVertices == NULL){
			/** MALLOC FAILED **/
		}

		// Temporarily stores vertex indices for faces.
		size_t tempIndicesSize = 0;
		size_t tempIndicesCapacity = BASE_INDEX_CAPACITY;
		size_t *tempIndices = memoryManagerGlobalAlloc(BASE_INDEX_CAPACITY * sizeof(*tempIndices));
		if(tempIndices == NULL){
			/** MALLOC FAILED **/
		}

		// Temporarily stores vertex positions, regardless of whether or not they are unique.
		size_t tempPositionsSize = 0;
		size_t tempPositionsCapacity = BASE_POSITION_CAPACITY;
		vec3 *tempPositions = memoryManagerGlobalAlloc(BASE_POSITION_CAPACITY * sizeof(*tempPositions));
		if(tempPositions == NULL){
			/** MALLOC FAILED **/
		}

		// Temporarily stores vertex UVs, regardless of whether or not they are unique.
		size_t tempUVsSize = 0;
		size_t tempUVsCapacity = BASE_UV_CAPACITY;
		vec2 *tempUVs = memoryManagerGlobalAlloc(BASE_UV_CAPACITY * sizeof(*tempUVs));
		if(tempUVs == NULL){
			/** MALLOC FAILED **/
		}

		// Temporarily stores vertex normals, regardless of whether or not they are unique.
		size_t tempNormalsSize = 0;
		size_t tempNormalsCapacity = BASE_NORMAL_CAPACITY;
		vec3 *tempNormals = memoryManagerGlobalAlloc(BASE_NORMAL_CAPACITY * sizeof(*tempNormals));
		if(tempNormals == NULL){
			/** MALLOC FAILED **/
		}

		// Stores the name of the textureGroup that the model uses.
		char *tempTexGroupName = NULL;

		// Used when reading vertex data.
		char *tokPos;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		while(success && (line = fileReadLine(mdlFile, &lineBuffer[0], &lineLength)) != NULL){
			// Vertex positions.
			if(memcmp(line, "v ", 2) == 0){
				vec3 newPosition;

				// If we're out of space, allocate some more!
				if(tempPositionsSize >= tempPositionsCapacity){
					tempPositionsCapacity = tempPositionsSize * 2;
					tempPositions = memoryManagerGlobalRealloc(tempPositions, tempPositionsCapacity * sizeof(*tempPositions));
					if(tempPositions == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Read the vertex positions from the line!
				newPosition.x = strtod(&line[2], &tokPos);
				newPosition.y = strtod(tokPos, &tokPos);
				newPosition.z = strtod(tokPos, NULL);

				tempPositions[tempPositionsSize] = newPosition;
				++tempPositionsSize;

			// Vertex UVs.
			}else if(memcmp(line, "vt ", 3) == 0){
				vec2 newUV;

				// If we're out of space, allocate some more!
				if(tempUVsSize >= tempUVsCapacity){
					tempUVsCapacity = tempUVsSize * 2;
					tempUVs = memoryManagerGlobalRealloc(tempUVs, tempUVsCapacity * sizeof(*tempUVs));
					if(tempUVs == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Read the vertex UVs from the line!
				newUV.x = strtod(&line[3], &tokPos);
				newUV.y = -strtod(tokPos, NULL);

				tempUVs[tempUVsSize] = newUV;
				++tempUVsSize;

			// Vertex normals.
			}else if(memcmp(line, "vn ", 3) == 0){
				vec3 newNormal;

				// If we're out of space, allocate some more!
				if(tempNormalsSize >= tempNormalsCapacity){
					tempNormalsCapacity = tempNormalsSize * 2;
					tempNormals = memoryManagerGlobalRealloc(tempNormals, tempNormalsCapacity * sizeof(*tempNormals));
					if(tempNormals == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Read the vertex normals from the line!
				newNormal.x = strtod(&line[3], &tokPos);
				newNormal.y = strtod(tokPos, &tokPos);
				newNormal.z = strtod(tokPos, NULL);

				tempNormals[tempNormalsSize] = newNormal;
				++tempNormalsSize;

			// TextureGroup path.
			}else if(memcmp(line, "usemtl ", 7) == 0){
				// We don't want to be keeping textureGroups that aren't used, so
				// we'll load this once we can be sure everything else was successful.
				// Note that we use realloc, so we only really store one texture.
				tempTexGroupName = memoryManagerGlobalRealloc(tempTexGroupName, (lineLength - 6) * sizeof(*tempTexGroupName));
				if(tempTexGroupName == NULL){
					/** REALLOC FAILED **/
				}
				// Our line reading function replaces the line break
				// with a null terminator, so we need to store it too.
				memcpy(tempTexGroupName, line + 7, (lineLength - 6) * sizeof(*tempTexGroupName));

			// Faces.
			}else if(memcmp(line, "f ", 2) == 0){
				size_t a;
				char *tokEnd;

				tokPos = &line[2];
				tokEnd = getToken(tokPos, ' ');
				if(tokEnd != NULL){
					*tokEnd = '\0';
					++tokEnd;
				}

				// If the vertex we want to add already exists, create an index to it!
				// Otherwise, add it to the vector!
				for(a = 0; a < 3 || tokPos != NULL; ++a){
					size_t b;

					vertex tempVertex;
					size_t posIndex, uvIndex, normalIndex;
					const vertex *checkVertex = tempVertices;


					memset(&tempVertex, 0.f, sizeof(tempVertex));

					// Read the indices!
					posIndex = strtoul(tokPos, &tokPos, 10) - 1;
					++tokPos;
					uvIndex = strtoul(tokPos, &tokPos, 10) - 1;
					++tokPos;
					normalIndex = strtoul(tokPos, &tokPos, 10) - 1;

					// Fill up tempVertex with the vertex information
					// we've stored! If the index is invalid, store a 0!
					if(posIndex < tempPositionsSize){
						tempVertex.pos = tempPositions[posIndex];
					}
					if(uvIndex < tempUVsSize){
						tempVertex.uv = tempUVs[uvIndex];
					}
					if(normalIndex < tempNormalsSize){
						tempVertex.normal = tempNormals[normalIndex];
					}

					#warning "This is only temporary since we don't support bones here yet."
					tempVertex.boneIDs[0] = 0;
					memset(&tempVertex.boneIDs[1], -1, (VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneIDs[0]));
					tempVertex.boneWeights[0] = 1.f;
					memset(&tempVertex.boneWeights[1], 0.f, (VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneWeights[0]));


					// Check if this vertex already exists!
					for(b = 0; b < tempVerticesSize; ++b){
						// Looks like it does, so we don't need to store it again!
						if(!vertexDifferent(checkVertex, &tempVertex)){
							break;
						}

						++checkVertex;
					}
					// The vertex does not exist, so add it to the vector!
					if(b == tempVerticesSize){
						// If we're out of space, allocate some more!
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
					// If this face has more than three vertices, it needs to be split up!
					// We'll need to store two additional indices for every extra face.
					if(a >= 3){
						tempIndicesSize += 2;
						if(tempIndicesSize >= tempIndicesCapacity){
							tempIndicesCapacity = tempIndicesSize * 2;
							tempIndices = memoryManagerGlobalRealloc(tempIndices, tempIndicesCapacity * sizeof(*tempIndices));
							if(tempIndices == NULL){
								/** REALLOC FAILED **/
							}
						}
						// Get the index of the first vertex that this face used.
						tempIndices[tempIndicesSize - 2] = tempIndices[tempIndicesSize - 2 - a];
						// Now get the index of the last vertex that this face used.
						tempIndices[tempIndicesSize - 1] = tempIndices[tempIndicesSize - 3];

					// Otherwise, we only need to store one index!
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


					tokPos = tokEnd;
					// Get the beginning of the next vertex's data!
					if(tokEnd != NULL){
						tokEnd = getToken(tokEnd, ' ');
						if(tokEnd != NULL){
							*tokEnd = '\0';
							++tokEnd;
						}
					}
				}

			// Syntax error.
			}else if(lineLength > 0){
				printf(
					"Syntax error loading model!\n"
					"Path: %s\n"
					"Line: %s\n",
					mdlFullPath, line
				);

				success = 0;
			}
		}

		fclose(mdlFile);


		// If there wasn't an error, add the model to the vector!
		if(success){
			meshGenerateBuffers(&mdl->meshData, tempVertices, tempVerticesSize, tempIndices, tempIndicesSize);


			// Set the model's name!
			++mdlPathLength;
			mdl->name = memoryManagerGlobalAlloc(mdlPathLength);
			if(mdl->name == NULL){
				/** MALLOC FAILED **/
			}
			memcpy(mdl->name, mdlPath, mdlPathLength);

			// Now that we can be sure everything was
			// successful, find the textureGroup.
			if(tempTexGroupName != NULL){
				mdl->texGroup = moduleTexGroupFind(tempTexGroupName);
				// If we couldn't find the textureGroup, load it!
				if(mdl->texGroup == NULL){
					// Make sure we can allocate
					// enough memory for the texture.
					if(!(mdl->texGroup = moduleTexGroupAlloc())){
						/** MALLOC FAILED **/
					}
					// If we can't load it, just
					// use the error texture.
					if(!texGroupLoad(mdl->texGroup, tempTexGroupName)){
						moduleTexGroupFree(mdl->texGroup);
						mdl->texGroup = &texGroupDefault;
					}
				}
			}

		// Otherwise, delete the model.
		}else{
			modelDelete(mdl);
		}


		// We don't need to check if these are NULL,
		// as we do that when we're using them.
		memoryManagerGlobalFree(tempVertices);
		memoryManagerGlobalFree(tempIndices);
		memoryManagerGlobalFree(tempPositions);
		memoryManagerGlobalFree(tempUVs);
		memoryManagerGlobalFree(tempNormals);
		// This one, however, is allowed to be NULL.
		if(tempTexGroupName != NULL){
			memoryManagerGlobalFree(tempTexGroupName);
		}


		return(success);
	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			mdlFullPath
		);
	}


	return(0);
}

/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
// Load an SMD using the model specified by "imgName".
return_t modelLoadSMD(model *mdl, const char *mdlPath){
	FILE *mdlFile;
	char mdlFullPath[FILE_MAX_LINE_LENGTH];
	size_t mdlPathLength = strlen(mdlPath);

	// Find the full path for the model!
	fileGenerateFullPath(
		mdlPath, mdlPathLength,
		MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH,
		mdlFullPath
	);


	modelInit(mdl);


	// Load the model!
	mdlFile = fopen(mdlFullPath, "r");
	if(mdlFile != NULL){
		return_t success = 1;


		// Temporarily stores only unique vertices.
		size_t tempVerticesSize = 0;
		size_t tempVerticesCapacity = BASE_VERTEX_CAPACITY;
		vertex *tempVertices = memoryManagerGlobalAlloc(BASE_VERTEX_CAPACITY * sizeof(*tempVertices));
		if(tempVertices == NULL){
			/** MALLOC FAILED **/
		}

		// Temporarily stores vertex indices for faces.
		size_t tempIndicesSize = 0;
		size_t tempIndicesCapacity = BASE_INDEX_CAPACITY;
		size_t *tempIndices = memoryManagerGlobalAlloc(BASE_INDEX_CAPACITY * sizeof(*tempIndices));
		if(tempIndices == NULL){
			/** MALLOC FAILED **/
		}

		// Temporarily stores bones.
		size_t tempBonesSize = 0;
		size_t tempBonesCapacity = BASE_BONE_CAPACITY;
		bone *tempBones = memoryManagerGlobalAlloc(BASE_BONE_CAPACITY * sizeof(*tempBones));
		if(tempBones == NULL){
			/** MALLOC FAILED **/
		}

		char *tokPos;
		// This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		// This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		while(success && (line = fileReadLine(mdlFile, &lineBuffer[0], &lineLength)) != NULL){
			if(dataType == 0){
				if(strcmp(line, "nodes") == 0){
					dataType = 1;
				}else if(strcmp(line, "skeleton") == 0){
					dataType = 2;
				}else if(strcmp(line, "triangles") == 0){
					dataType = 3;

				// If this isn't the version number and the line isn't empty, it's something we can't handle!
				}else if(lineLength > 0 && strcmp(line, "version 1") != 0){
					printf(
						"Error loading model!\n"
						"Path: %s\n"
						"Line: %s\n"
						"Error: Unexpected identifier!\n",
						mdlFullPath, line
					);

					success = 0;
				}
			}else{
				if(strcmp(line, "end") == 0){
					// If we've finished identifying the skeleton's bones, shrink the vector!
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
						tokPos = line;

						bone tempBone;

						// Get this bone's ID.
						size_t boneID = strtoul(tokPos, &tokPos, 10);
						if(boneID == tempBonesSize){
							// Get the bone's name.
							size_t boneNameLength;
							tokPos = getMultiDelimitedString(tokPos, line + lineLength - tokPos, "\" ", &boneNameLength);
							tempBone.name = memoryManagerGlobalAlloc(boneNameLength + 1);
							if(tempBone.name == NULL){
								/** MALLOC FAILED **/
							}
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							// Get the ID of this bone's parent.
							tempBone.parent = strtoul(tokPos + boneNameLength + 1, NULL, 10);


							// If we're out of space, allocate some more!
							if(tempBonesSize >= tempBonesCapacity){
								tempBonesCapacity = tempBonesSize * 2;
								tempBones = memoryManagerGlobalRealloc(tempBones, tempBonesCapacity * sizeof(*tempBones));
								if(tempBones == NULL){
									/** REALLOC FAILED **/
								}
							}
							// Add the bone to our vector!
							tempBones[tempBonesSize] = tempBone;
							++tempBonesSize;
						}else{
							printf(
								"Error loading model!\n"
								"Path: %s\n"
								"Line: %s\n"
								"Error: Found node %u when expecting node %u!\n",
								mdlFullPath, line, boneID, tempBonesSize
							);

							success = 0;
						}
					}else if(dataType == 2){
						// If the line begins with time, get the frame's timestamp!
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
									mdlFullPath, line
								);

								success = 0;
							}

						// Otherwise, we're setting the bone's state!
						}else{
							// Get this bone's ID.
							size_t boneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(boneID < tempBonesSize){
								bone *currentBone = &tempBones[boneID];

								// If the current frame timestamp is 0, set the bone's initial state!
								if(data == 0){
									// Load the bone's position!
									float x = strtod(tokPos, &tokPos) * 0.05f;
									float y = strtod(tokPos, &tokPos) * 0.05f;
									float z = strtod(tokPos, &tokPos) * 0.05f;
									vec3InitSet(&currentBone->localBind.pos, x, y, z);

									// Load the bone's rotation!
									x = strtod(tokPos, &tokPos);
									y = strtod(tokPos, &tokPos);
									z = strtod(tokPos, NULL);
									quatInitEulerRad(&currentBone->localBind.rot, x, y, z);

									// Set the bone's scale!
									vec3InitSet(&currentBone->localBind.scale, 1.f, 1.f, 1.f);


									// Set the inverse bind state!
									currentBone->invGlobalBind = currentBone->localBind;
									// If this bone has a parent, append its state to its parent's state!
									if(!valueIsInvalid(currentBone->parent)){
										transformStateAppend(
											&tempBones[currentBone->parent].invGlobalBind,
											&currentBone->invGlobalBind,
											&currentBone->invGlobalBind
										);
									}
								}
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Found skeletal data for bone %u, which doesn't exist!\n",
									mdlFullPath, line, boneID
								);

								success = 0;
							}
						}
					}else if(dataType == 3){
						if(data == 0){
							// This indicates the texture that the face uses.
						}else{
							vertex tempVertex;
							memset(&tempVertex, 0.f, sizeof(tempVertex));

							// Read the vertex data from the line!
							size_t parentBoneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(parentBoneID < tempBonesSize){
								tempVertex.pos.x = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.pos.y = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.pos.z = strtod(tokPos, &tokPos) * 0.05f;
								tempVertex.normal.x = strtod(tokPos, &tokPos);
								tempVertex.normal.y = strtod(tokPos, &tokPos);
								tempVertex.normal.z = strtod(tokPos, &tokPos);
								tempVertex.uv.x = strtod(tokPos, &tokPos);
								tempVertex.uv.y = -strtod(tokPos, &tokPos);
								size_t numLinks = strtoul(tokPos, &tokPos, 10);
								// Make sure some links were specified.
								if(numLinks > 0){
									// If there are more than the maximum number of supported weights, we'll have to clamp it down!
									if(numLinks > VERTEX_MAX_WEIGHTS){
										printf(
											"Error loading model!\n"
											"Path: %s\n"
											"Line: %s\n"
											"Error: Vertex has too many links! All extra links will be ignored.\n",
											mdlFullPath, line
										);

										numLinks = VERTEX_MAX_WEIGHTS;
									}

									size_t parentPos = invalidValue(parentPos);
									float totalWeight = 0.f;

									size_t *curBoneID = tempVertex.boneIDs;
									float *curBoneWeight = tempVertex.boneWeights;
									size_t i;
									// Load all of the links!
									for(i = 0; i < numLinks; ++i){
										// Load the bone's ID!
										*curBoneID = strtoul(tokPos, &tokPos, 10);
										// Make sure it exists!
										if(*curBoneID > tempBonesSize){
											printf(
												"Error loading model!\n"
												"Path: %s\n"
												"Line: %s\n"
												"Error: Vertex link bone doesn't exist! The parent bone will be used instead.\n",
												mdlFullPath, line
											);

										// If we're loading the parent bone, remember its position!
										}else if(*curBoneID == parentBoneID){
											parentPos = i;
										}

										// Load the bone's weights!
										*curBoneWeight = strtod(tokPos, &tokPos);
										totalWeight += *curBoneWeight;
										// Make sure the total weight doesn't exceed 1!
										if(totalWeight > 1.f){
											*curBoneWeight -= totalWeight - 1.f;
											totalWeight = 1.f;

											++i;
											break;
										}

										++curBoneID;
										++curBoneWeight;
									}

									// Make sure the total weight isn't less than 1!
									if(totalWeight < 1.f){
										// If we never loaded the parent bone, see if we can add it!
										if(valueIsInvalid(parentPos)){
											if(i < VERTEX_MAX_WEIGHTS){
												*curBoneID = parentBoneID;
												*curBoneWeight = 0.f;
												parentPos = i;
												++i;

											// If there's no room, just use the first bone we loaded.
											}else{
												parentPos = 0;
											}
										}

										tempVertex.boneWeights[parentPos] += 1.f - totalWeight;
									}

									// Make sure we fill the rest with invalid values so we know they aren't used.
									memset(&curBoneID, -1, (VERTEX_MAX_WEIGHTS - i) * sizeof(*tempVertex.boneIDs));
									memset(&curBoneWeight, 0.f, (VERTEX_MAX_WEIGHTS - i) * sizeof(*tempVertex.boneWeights));

								// Otherwise, just bind it to the parent bone.
								}else{
									printf(
										"Error loading model!\n"
										"Path: %s\n"
										"Line: %s\n"
										"Error: Vertex has no links! The parent bone will be used.\n",
										mdlFullPath, line
									);

									tempVertex.boneIDs[0] = parentBoneID;
									memset(&tempVertex.boneIDs[1], -1, (VERTEX_MAX_WEIGHTS - 1) * sizeof(*tempVertex.boneIDs));
									tempVertex.boneWeights[0] = 1.f;
									memset(&tempVertex.boneWeights[1], 0.f, (VERTEX_MAX_WEIGHTS - 1) * sizeof(*tempVertex.boneWeights));
								}


								const vertex *checkVertex = tempVertices;
								size_t i;
								// Check if this vertex already exists!
								for(i = 0; i < tempVerticesSize; ++i){
									// Looks like it does, so we don't need to store it again!
									if(!vertexDifferent(checkVertex, &tempVertex)){
										break;
									}

									++checkVertex;
								}
								// The vertex does not exist, so add it to the vector!
								if(i == tempVerticesSize){
									// If we're out of space, allocate some more!
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
								// Add an index for the new vertex!
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
									mdlFullPath, line
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


		// If there wasn't an error, add the model to the vector!
		if(success){
			meshGenerateBuffers(&mdl->meshData, tempVertices, tempVerticesSize, tempIndices, tempIndicesSize);


			// Set the model's name!
			++mdlPathLength;
			mdl->name = memoryManagerGlobalAlloc(mdlPathLength);
			if(mdl->name == NULL){
				/** MALLOC FAILED **/
			}
			memcpy(mdl->name, mdlPath, mdlPathLength);

			tempBones = memoryManagerGlobalResize(tempBones, sizeof(*tempBones) * tempBonesSize);
			// Initialise the model's skeleton!
			if(tempBones != NULL && tempBonesSize > 0){
				mdl->skele = moduleSkeletonAlloc();
				skeleInitSet(mdl->skele, mdl->name, mdlPathLength, tempBones, tempBonesSize);
			}

			#warning "This is obviously incomplete."
			// Now that we can be sure everything was
			// successful, find the textureGroup.
			// if(tempTexGroupName != NULL){
			mdl->texGroup = moduleTexGroupFind("misc\\soldier.tdg");
			// If we couldn't find the textureGroup, load it!
			if(mdl->texGroup == NULL){
				// Make sure we can allocate
				// enough memory for the texture.
				if(!(mdl->texGroup = moduleTexGroupAlloc())){
					/** MALLOC FAILED **/
				}
				// If we can't load it, just
				// use the error texture.
				if(!texGroupLoad(mdl->texGroup, "misc\\soldier.tdg")){
					moduleTexGroupFree(mdl->texGroup);
					mdl->texGroup = &texGroupDefault;
				}
			}
			// }

		// Otherwise, delete the model.
		}else{
			modelDelete(mdl);
		}


		// We don't need to check if these are NULL,
		// as we do that when we're using them.
		memoryManagerGlobalFree(tempVertices);
		memoryManagerGlobalFree(tempIndices);
		// This one, however, is allowed to be NULL.
		#warning "Remember to do something with this."
		/*if(tempTexGroupName != NULL){
			memoryManagerGlobalFree(tempTexGroupName);
		}*/


		return(success);
	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			mdlFullPath
		);
	}


	return(0);
}

return_t modelSetupDefault(){
	mdlDefault.meshData = meshDefault;
	return(1);
}


void modelDelete(model *mdl){
	// Only free the name if it's in
	// use and it's not the error model.
	if(mdl->name != NULL && mdl != &mdlDefault){
		memoryManagerGlobalFree(mdl->name);
	}
	meshDelete(&mdl->meshData);
}