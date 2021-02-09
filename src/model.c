#include "model.h"


#include <stdio.h>
#include <string.h>

#include "vec2.h"
#include "vec3.h"
#include "quat.h"
#include "transform.h"

#include "memoryManager.h"
#include "moduleModel.h"
#include "moduleSkeleton.h"

#include "utilString.h"
#include "utilFile.h"


#define MODEL_PATH_PREFIX        "."FILE_PATH_DELIMITER_STR"resource"FILE_PATH_DELIMITER_STR"models"FILE_PATH_DELIMITER_STR
#define MODEL_PATH_PREFIX_LENGTH (sizeof(MODEL_PATH_PREFIX) - 1)

// These must be at least 1!
#define BASE_MESH_CAPACITY     1
#define BASE_VERTEX_CAPACITY   1
#define BASE_INDEX_CAPACITY    BASE_VERTEX_CAPACITY
#define BASE_POSITION_CAPACITY BASE_VERTEX_CAPACITY
#define BASE_UV_CAPACITY       BASE_VERTEX_CAPACITY
#define BASE_NORMAL_CAPACITY   BASE_VERTEX_CAPACITY
#define BASE_BONE_CAPACITY     1


// This mesh is only used by the default model.
// It cannot be accessed from outside this file.
mesh mdlMeshDefault;

// By default, the error model only has a name.
// We need to set up the other data the hard way.
model g_mdlDefault = {
	.name      = "error",
	.meshes    = &mdlMeshDefault,
	.texGroups = &g_texGroupArrayDefault,
	.numMeshes = 1,
	.skele     = &g_skeleDefault
};


// We need to store an array of vertices and indices for each mesh
// we're loading. We use a separate mesh for each texture group.
typedef struct meshData {
	char *texGroupPath;

	meshVertexIndex_t tempVerticesSize;
	meshVertexIndex_t tempVerticesCapacity;
	vertex *tempVertices;

	meshVertexIndex_t tempIndicesSize;
	meshVertexIndex_t tempIndicesCapacity;
	meshVertexIndex_t *tempIndices;
} meshData;


#warning "What if we aren't using the global memory manager?"


/*
** Load an OBJ using the model specified by "mdlPath" and return a pointer to it.
** If the model could not be loaded, return a pointer to the default model.
*/
model *modelOBJLoad(const char *const restrict mdlPath, const size_t mdlPathLength){
	model *mdl;

	FILE *mdlFile;
	char mdlFullPath[FILE_MAX_PATH_LENGTH];


	#ifdef TEMP_MODULE_FIND
	// If the model has already been loaded, return a pointer to it!
	if((mdl = moduleModelFind(mdlPath)) != &g_mdlDefault){
		return(mdl);
	}
	#else
	mdl = &g_mdlDefault;
	#endif


	// Generate the full path for the model!
	fileGenerateFullResourcePath(
		MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH,
		mdlPath, mdlPathLength,
		mdlFullPath
	);


	// Load the model!
	mdlFile = fopen(mdlFullPath, "r");
	if(mdlFile != NULL){
		meshVertexIndex_t tempPositionsSize = 0;
		meshVertexIndex_t tempPositionsCapacity = BASE_POSITION_CAPACITY;
		vec3 *tempPositions;

		meshVertexIndex_t tempUVsSize = 0;
		meshVertexIndex_t tempUVsCapacity = BASE_UV_CAPACITY;
		vec2 *tempUVs;

		meshVertexIndex_t tempNormalsSize = 0;
		meshVertexIndex_t tempNormalsCapacity = BASE_NORMAL_CAPACITY;
		vec3 *tempNormals;

		size_t tempMeshDataSize = 0;
		size_t tempMeshDataCapacity = BASE_MESH_CAPACITY;
		meshData *tempMeshData;
		meshData *curMeshData = NULL;

		// Used when reading vertex data.
		char *tokPos;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		// Temporarily stores vertex positions, regardless of whether or not they are unique.
		tempPositions = memoryManagerGlobalAlloc(BASE_POSITION_CAPACITY * sizeof(*tempPositions));
		if(tempPositions == NULL){
			/** MALLOC FAILED **/
		}
		// Temporarily stores vertex UVs, regardless of whether or not they are unique.
		tempUVs = memoryManagerGlobalAlloc(BASE_UV_CAPACITY * sizeof(*tempUVs));
		if(tempUVs == NULL){
			/** MALLOC FAILED **/
		}
		// Temporarily stores vertex normals, regardless of whether or not they are unique.
		tempNormals = memoryManagerGlobalAlloc(BASE_NORMAL_CAPACITY * sizeof(*tempNormals));
		if(tempNormals == NULL){
			/** MALLOC FAILED **/
		}
		// Temporarily stores the vertices, indices
		// and texture group path for each mesh.
		tempMeshData = memoryManagerGlobalAlloc(BASE_MESH_CAPACITY * sizeof(*tempMeshData));
		if(tempMeshData == NULL){
			/** MALLOC FAILED **/
		}


		while((line = fileReadLine(mdlFile, &lineBuffer[0], &lineLength)) != NULL){
			// Vertex position.
			if(lineLength >= 7 && memcmp(line, "v ", 2) == 0){
				vec3 newPosition;

				// If we're out of space, allocate some more!
				if(tempPositionsSize >= tempPositionsCapacity){
					tempPositionsCapacity = tempPositionsSize * 2;
					tempPositions = memoryManagerGlobalRealloc(
						tempPositions, tempPositionsCapacity * sizeof(*tempPositions)
					);
					if(tempPositions == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Read the vertex positions from the line!
				newPosition.x = strtof(&line[2], &tokPos);
				newPosition.y = strtof(tokPos, &tokPos);
				newPosition.z = strtof(tokPos, NULL);

				tempPositions[tempPositionsSize] = newPosition;
				++tempPositionsSize;

			// Vertex UV.
			}else if(lineLength >= 6 && memcmp(line, "vt ", 3) == 0){
				vec2 newUV;

				// If we're out of space, allocate some more!
				if(tempUVsSize >= tempUVsCapacity){
					tempUVsCapacity = tempUVsSize * 2;
					tempUVs = memoryManagerGlobalRealloc(
						tempUVs, tempUVsCapacity * sizeof(*tempUVs)
					);
					if(tempUVs == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Read the vertex UVs from the line!
				newUV.x = strtof(&line[3], &tokPos);
				newUV.y = 1.f - strtof(tokPos, NULL);

				tempUVs[tempUVsSize] = newUV;
				++tempUVsSize;

			// Vertex normal.
			}else if(lineLength >= 8 && memcmp(line, "vn ", 3) == 0){
				vec3 newNormal;

				// If we're out of space, allocate some more!
				if(tempNormalsSize >= tempNormalsCapacity){
					tempNormalsCapacity = tempNormalsSize * 2;
					tempNormals = memoryManagerGlobalRealloc(
						tempNormals, tempNormalsCapacity * sizeof(*tempNormals)
					);
					if(tempNormals == NULL){
						/** REALLOC FAILED **/
					}
				}

				// Read the vertex normals from the line!
				newNormal.x = strtof(&line[3], &tokPos);
				newNormal.y = strtof(tokPos, &tokPos);
				newNormal.z = strtof(tokPos, NULL);

				tempNormals[tempNormalsSize] = newNormal;
				++tempNormalsSize;

			// Texture group path.
			}else if(lineLength >= 8 && memcmp(line, "usemtl ", 7) == 0){
				char *const texGroupPath = memoryManagerGlobalAlloc((lineLength - 6) * sizeof(*texGroupPath));
				if(texGroupPath == NULL){
					/** MALLOC FAILED **/
				}
				fileParseResourcePath(texGroupPath, &line[7], lineLength - 7, NULL);

				// If we've seen this texture group before,
				// we should switch to the mesh that uses it.
				{
					const meshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
					for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
						// If we've seen this texture group before, we'll need
						// to free the memory we allocated for the new path.
						if(strcmp(texGroupPath, curMeshData->texGroupPath) == 0){
							memoryManagerGlobalFree(texGroupPath);
							break;
						}
					}

					// If this texture group is new, create a new mesh for it!
					if(curMeshData == lastMeshData){
						// If we're out of space, allocate some more!
						if(tempMeshDataSize >= tempMeshDataCapacity){
							tempMeshDataCapacity = tempMeshDataSize * 2;
							tempMeshData = memoryManagerGlobalRealloc(
								tempMeshData, tempMeshDataCapacity * sizeof(*tempMeshData)
							);
							if(tempMeshData == NULL){
								/** REALLOC FAILED **/
							}
						}

						// Initialize the new mesh data!
						curMeshData = &tempMeshData[tempMeshDataSize];
						curMeshData->texGroupPath = texGroupPath;
						curMeshData->tempVerticesSize = 0;
						curMeshData->tempVerticesCapacity = BASE_VERTEX_CAPACITY;
						curMeshData->tempVertices = memoryManagerGlobalAlloc(
							BASE_VERTEX_CAPACITY * sizeof(*curMeshData->tempVertices)
						);
						if(curMeshData->tempVertices == NULL){
							/** MALLOC FAILED **/
						}
						curMeshData->tempIndicesSize = 0;
						curMeshData->tempIndicesCapacity = BASE_INDEX_CAPACITY;
						curMeshData->tempIndices = memoryManagerGlobalAlloc(
							BASE_INDEX_CAPACITY * sizeof(*curMeshData->tempIndices)
						);
						if(curMeshData->tempIndices == NULL){
							/** MALLOC FAILED **/
						}

						++tempMeshDataSize;
					}
				}

			// Faces.
			}else if(lineLength >= 13 && memcmp(line, "f ", 2) == 0){
				const char *const lineEnd = &line[lineLength];
				size_t i;

				// If we haven't seen any texture groups yet,
				// allocate a new mesh using the default one.
				if(curMeshData == NULL){
					curMeshData = tempMeshData;
					curMeshData->texGroupPath = NULL;
					curMeshData->tempVerticesSize = 0;
					curMeshData->tempVerticesCapacity = BASE_VERTEX_CAPACITY;
					curMeshData->tempVertices = memoryManagerGlobalAlloc(
						BASE_VERTEX_CAPACITY * sizeof(*curMeshData->tempVertices)
					);
					if(curMeshData->tempVertices == NULL){
						/** MALLOC FAILED **/
					}
					curMeshData->tempIndicesSize = 0;
					curMeshData->tempIndicesCapacity = BASE_INDEX_CAPACITY;
					curMeshData->tempIndices = memoryManagerGlobalAlloc(
						BASE_INDEX_CAPACITY * sizeof(*curMeshData->tempIndices)
					);
					if(curMeshData->tempIndices == NULL){
						/** MALLOC FAILED **/
					}
				}

				tokPos = &line[2];
				// Iterate through the face's vertices. We keep going until we've
				// read at least 3 and we haven't reached the end of the line.
				for(i = 0; i < 3 || tokPos < lineEnd; ++i){
					vertex tempVertex;
					const vertex *checkVertex = curMeshData->tempVertices;
					meshVertexIndex_t j;


					// If we've reached the end of the line,
					// just initialize everything to zero.
					if(tokPos >= lineEnd){
						vec3InitZero(&tempVertex.pos);
						vec2InitZero(&tempVertex.uv);
						vec3InitZero(&tempVertex.normal);

					// Otherwise, try and read the
					// vertex data from the line.
					}else{
						// Load the vertex's position.
						meshVertexIndex_t curIndex = strtoul(tokPos, &tokPos, 10) - 1;
						if(curIndex < tempPositionsSize){
							tempVertex.pos = tempPositions[curIndex];
						}else{
							vec3InitZero(&tempVertex.pos);
						}
						// Load the vertex's UV coordinates.
						curIndex = strtoul(tokPos + 1, &tokPos, 10) - 1;
						if(curIndex < tempUVsSize){
							tempVertex.uv = tempUVs[curIndex];
						}else{
							vec2InitZero(&tempVertex.uv);
						}
						// Load the vertex's normal.
						curIndex = strtoul(tokPos + 1, &tokPos, 10) - 1;
						if(curIndex < tempNormalsSize){
							tempVertex.normal = tempNormals[curIndex];
						}else{
							vec3InitZero(&tempVertex.normal);
						}
					}

					// Object files don't support bones, so just
					// use the root bone of the default skeleton.
					tempVertex.boneIDs[0] = 0;
					memset(&tempVertex.boneIDs[1], valueInvalid(meshBoneIndex_t), (VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneIDs[0]));
					tempVertex.boneWeights[0] = 1.f;
					memset(&tempVertex.boneWeights[1], 0.f, (VERTEX_MAX_WEIGHTS - 1) * sizeof(tempVertex.boneWeights[0]));


					// If this vertex already exists,
					// we don't need to store it again.
					for(j = 0; j < curMeshData->tempVerticesSize; ++j){
						if(!vertexDifferent(checkVertex, &tempVertex)){
							break;
						}
						++checkVertex;
					}

					// If it doesn't already exist, add it to our mesh!
					if(j == curMeshData->tempVerticesSize){
						// If we're out of space, allocate some more!
						if(curMeshData->tempVerticesSize >= curMeshData->tempVerticesCapacity){
							curMeshData->tempVerticesCapacity = curMeshData->tempVerticesSize * 2;
							curMeshData->tempVertices = memoryManagerGlobalRealloc(
								curMeshData->tempVertices,
								curMeshData->tempVerticesCapacity * sizeof(*curMeshData->tempVertices)
							);
							if(curMeshData->tempVertices == NULL){
								/** REALLOC FAILED **/
							}
						}
						curMeshData->tempVertices[curMeshData->tempVerticesSize] = tempVertex;
						++curMeshData->tempVerticesSize;
					}


					// Faces with more than three vertices need to
					// be split into triangles. This requires two
					// additional indices for every extra vertex.
					if(i >= 3){
						meshVertexIndex_t *tempIndices;
						const meshVertexIndex_t oldIndicesSize = curMeshData->tempIndicesSize;

						curMeshData->tempIndicesSize += 2;
						if(curMeshData->tempIndicesSize >= curMeshData->tempIndicesCapacity){
							curMeshData->tempIndicesCapacity = curMeshData->tempIndicesSize * 2;
							curMeshData->tempIndices = memoryManagerGlobalRealloc(
								curMeshData->tempIndices,
								curMeshData->tempIndicesCapacity * sizeof(*curMeshData->tempIndices)
							);
							if(curMeshData->tempIndices == NULL){
								/** REALLOC FAILED **/
							}
						}

						tempIndices = curMeshData->tempIndices;
						// Get the index of the first vertex that this face used.
						tempIndices[oldIndicesSize] = tempIndices[oldIndicesSize - i];
						// Now get the index of the last vertex that this face used.
						tempIndices[oldIndicesSize + 1] = tempIndices[oldIndicesSize - 1];

					// Otherwise, we only need to store one index!
					}else{
						if(curMeshData->tempIndicesSize >= curMeshData->tempIndicesCapacity){
							curMeshData->tempIndicesCapacity = curMeshData->tempIndicesSize * 2;
							curMeshData->tempIndices = memoryManagerGlobalRealloc(
								curMeshData->tempIndices,
								curMeshData->tempIndicesCapacity * sizeof(*curMeshData->tempIndices)
							);
							if(curMeshData->tempIndices == NULL){
								/** REALLOC FAILED **/
							}
						}
					}
					// Store the index for this vertex.
					curMeshData->tempIndices[curMeshData->tempIndicesSize] = j;
					++curMeshData->tempIndicesSize;
				}
			}
		}

		fclose(mdlFile);


		// There's no point keeping the model if no meshes were loaded.
		if(tempMeshDataSize > 0){
			const meshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
			size_t numMeshes = 0;
			// Count the number of valid meshes (meshes with at least one face).
			for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
				if(curMeshData->tempIndicesSize >= 3){
					++numMeshes;

				// If the mesh is invalid, clean it up
				// now so we don't have to do it later.
				}else{
					memoryManagerGlobalFree(curMeshData->texGroupPath);
					memoryManagerGlobalFree(curMeshData->tempVertices);
					memoryManagerGlobalFree(curMeshData->tempIndices);
				}
			}

			// If there's at least one valid mesh, the model should be valid.
			if(numMeshes > 0){
				mesh *curMesh;
				textureGroup **curTexGroup;

				mdl = moduleModelAlloc();
				if(mdl == NULL){
					/** MALLOC FAILED **/
				}
				mdl->name = memoryManagerGlobalAlloc(mdlPathLength + 1);
				if(mdl->name == NULL){
					/** MALLOC FAILED **/
				}
				memcpy(mdl->name, mdlPath, mdlPathLength + 1);
				mdl->meshes = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdl->meshes));
				if(mdl->meshes == NULL){
					/** MALLOC FAILED **/
				}
				mdl->texGroups = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdl->texGroups));
				if(mdl->texGroups == NULL){
					/** MALLOC FAILED **/
				}
				mdl->numMeshes = numMeshes;
				mdl->skele = &g_skeleDefault;

				curMesh = mdl->meshes;
				curTexGroup = mdl->texGroups;
				// Generate buffer objects for each valid
				// mesh and load their texture groups.
				for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
					if(curMeshData->tempIndicesSize >= 3){
						meshGenerateBuffers(
							curMesh,
							curMeshData->tempVertices, curMeshData->tempVerticesSize,
							curMeshData->tempIndices, curMeshData->tempIndicesSize
						);
						if(curMeshData->texGroupPath == NULL){
							*curTexGroup = &g_texGroupDefault;
						}else{
							*curTexGroup = texGroupLoad(
								curMeshData->texGroupPath,
								strlen(curMeshData->texGroupPath)
							);
						}

						// The mesh and texture group should allocate
						// their own memory, so we don't need these anymore.
						memoryManagerGlobalFree(curMeshData->texGroupPath);
						memoryManagerGlobalFree(curMeshData->tempVertices);
						memoryManagerGlobalFree(curMeshData->tempIndices);

						++curMesh;
						++curTexGroup;
					}
				}
			}
		}


		// We don't need to check if these are NULL,
		// as we do that when we're using them.
		memoryManagerGlobalFree(tempPositions);
		memoryManagerGlobalFree(tempUVs);
		memoryManagerGlobalFree(tempNormals);
		// We've already deleted any arrays stored here.
		memoryManagerGlobalFree(tempMeshData);
	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			mdlFullPath
		);
	}


	return(mdl);
}

/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
/*
** Load an SMD using the model specified by "mdlPath" and return a pointer to it.
** If the model could not be loaded, return a pointer to the default model.
*/
model *modelSMDLoad(const char *const restrict mdlPath, const size_t mdlPathLength){
	model *mdl;

	FILE *mdlFile;
	char mdlFullPath[FILE_MAX_PATH_LENGTH];


	#ifdef TEMP_MODULE_FIND
	// If the model has already been loaded, return a pointer to it!
	if((mdl = moduleModelFind(mdlPath)) != &g_mdlDefault){
		return(mdl);
	}
	#else
	mdl = &g_mdlDefault;
	#endif


	// Generate the full path for the model!
	fileGenerateFullResourcePath(
		MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH,
		mdlPath, mdlPathLength,
		mdlFullPath
	);


	// Load the model!
	mdlFile = fopen(mdlFullPath, "r");
	if(mdlFile != NULL){
		return_t success = 1;

		size_t tempBonesSize = 0;
		size_t tempBonesCapacity = BASE_BONE_CAPACITY;
		bone *tempBones;

		size_t tempMeshDataSize = 0;
		size_t tempMeshDataCapacity = BASE_MESH_CAPACITY;
		meshData *tempMeshData;
		meshData *curMeshData = NULL;

		char *tokPos;
		// This indicates what sort of data we're currently supposed to be reading.
		byte_t dataType = 0;
		// This variable stores data specific to the type we're currently loading.
		unsigned int data = 0;

		char lineBuffer[FILE_MAX_LINE_LENGTH];
		char *line;
		size_t lineLength;


		// Temporarily stores bones.
		tempBones = memoryManagerGlobalAlloc(BASE_BONE_CAPACITY * sizeof(*tempBones));
		if(tempBones == NULL){
			/** MALLOC FAILED **/
		}
		// Temporarily stores the vertices, indices
		// and texture group path for each mesh.
		tempMeshData = memoryManagerGlobalAlloc(BASE_MESH_CAPACITY * sizeof(*tempMeshData));
		if(tempMeshData == NULL){
			/** MALLOC FAILED **/
		}


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
						// Get this bone's ID.
						const boneIndex_t boneID = strtoul(line, &tokPos, 10);
						if(boneID == tempBonesSize){
							bone tempBone;
							// Get the bone's name.
							const size_t boneNameLength = stringMultiDelimited(tokPos, line + lineLength - tokPos, "\" ", (const char **)&tokPos);
							tempBone.name = memoryManagerGlobalAlloc(boneNameLength + 1);
							if(tempBone.name == NULL){
								/** MALLOC FAILED **/
							}
							memcpy(tempBone.name, tokPos, boneNameLength);
							tempBone.name[boneNameLength] = '\0';

							// Get the ID of this bone's parent.
							tempBone.parent = strtol(tokPos + boneNameLength + 1, NULL, 10);

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
								"Error: Found node %u when expecting node "PRINTF_SIZE_T"!\n",
								mdlFullPath, line, boneID, tempBonesSize
							);

							success = 0;
						}
					}else if(dataType == 2){
						// If the line begins with time, get the frame's timestamp!
						if(lineLength >= 6 && memcmp(line, "time ", 5) == 0){
							const unsigned int newTime = strtoul(&line[5], NULL, 10);
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
							const boneIndex_t boneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(boneID < tempBonesSize){
								bone *currentBone = &tempBones[boneID];

								// If the current frame timestamp is 0, set the bone's initial state!
								if(data == 0){
									// Load the bone's position!
									float x = strtof(tokPos, &tokPos) * 0.05f;
									float y = strtof(tokPos, &tokPos) * 0.05f;
									float z = strtof(tokPos, &tokPos) * 0.05f;
									vec3InitSet(&currentBone->localBind.pos, x, y, z);

									// Load the bone's rotation!
									x = strtof(tokPos, &tokPos);
									y = strtof(tokPos, &tokPos);
									z = strtof(tokPos, NULL);
									quatInitEulerXYZ(&currentBone->localBind.rot, x, y, z);

									// Set the bone's scale!
									vec3InitSet(&currentBone->localBind.scale, 1.f, 1.f, 1.f);


									// Set the inverse bind state!
									currentBone->invGlobalBind = currentBone->localBind;
									// If this bone has a parent, append its state to its parent's state!
									if(!valueIsInvalid(currentBone->parent, boneIndex_t)){
										transformStateAppend(
											&currentBone->invGlobalBind,
											&tempBones[currentBone->parent].invGlobalBind,
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
						// Texture group for the following face.
						if(data == 0){
							char *const texGroupPath = memoryManagerGlobalAlloc((lineLength + 5) * sizeof(*texGroupPath));
							size_t texGroupPathLength;
							if(texGroupPath == NULL){
								/** MALLOC FAILED **/
							}
							texGroupPathLength = fileParseResourcePath(texGroupPath, line, lineLength, NULL);
							// Add the file extension!
							memcpy(&texGroupPath[texGroupPathLength], ".tdg", sizeof(".tdg"));

							// If we've seen this texture group before,
							// we should switch to the mesh that uses it.
							{
								const meshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
								for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
									// If we've seen this texture group before, we'll need
									// to free the memory we allocated for the new path.
									if(strcmp(texGroupPath, curMeshData->texGroupPath) == 0){
										memoryManagerGlobalFree(texGroupPath);
										break;
									}
								}

								// If this texture group is new, create a new mesh for it!
								if(curMeshData == lastMeshData){
									// If we're out of space, allocate some more!
									if(tempMeshDataSize >= tempMeshDataCapacity){
										tempMeshDataCapacity = tempMeshDataSize * 2;
										tempMeshData = memoryManagerGlobalRealloc(
											tempMeshData, tempMeshDataCapacity * sizeof(*tempMeshData)
										);
										if(tempMeshData == NULL){
											/** REALLOC FAILED **/
										}
									}

									// Initialize the new mesh data!
									curMeshData = &tempMeshData[tempMeshDataSize];
									curMeshData->texGroupPath = texGroupPath;
									curMeshData->tempVerticesSize = 0;
									curMeshData->tempVerticesCapacity = BASE_VERTEX_CAPACITY;
									curMeshData->tempVertices = memoryManagerGlobalAlloc(
										BASE_VERTEX_CAPACITY * sizeof(*curMeshData->tempVertices)
									);
									if(curMeshData->tempVertices == NULL){
										/** MALLOC FAILED **/
									}
									curMeshData->tempIndicesSize = 0;
									curMeshData->tempIndicesCapacity = BASE_INDEX_CAPACITY;
									curMeshData->tempIndices = memoryManagerGlobalAlloc(
										BASE_INDEX_CAPACITY * sizeof(*curMeshData->tempIndices)
									);
									if(curMeshData->tempIndices == NULL){
										/** MALLOC FAILED **/
									}

									++tempMeshDataSize;
								}
							}

						// Vertex.
						}else{
							// Read the vertex data from the line!
							const meshBoneIndex_t parentBoneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(parentBoneID < tempBonesSize){
								vertex tempVertex;
								meshBoneIndex_t numLinks;
								const vertex *checkVertex;
								meshVertexIndex_t i;

								tempVertex.pos.x = strtof(tokPos, &tokPos) * 0.05f;
								tempVertex.pos.y = strtof(tokPos, &tokPos) * 0.05f;
								tempVertex.pos.z = strtof(tokPos, &tokPos) * 0.05f;
								tempVertex.normal.x = strtof(tokPos, &tokPos);
								tempVertex.normal.y = strtof(tokPos, &tokPos);
								tempVertex.normal.z = strtof(tokPos, &tokPos);
								tempVertex.uv.x = strtof(tokPos, &tokPos);
								tempVertex.uv.y = 1.f - strtof(tokPos, &tokPos);

								numLinks = strtoul(tokPos, &tokPos, 10);
								// Make sure some links were specified.
								if(numLinks > 0){
									meshBoneIndex_t parentPos = valueInvalid(meshBoneIndex_t);
									meshBoneIndex_t *curBoneID = tempVertex.boneIDs;
									meshBoneIndex_t curLink;
									float totalWeight = 0.f;
									float *curBoneWeight = tempVertex.boneWeights;


									// If there are more than the maximum number of
									// supported weights, we'll have to clamp it down!
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

									// Load all of the links!
									for(curLink = 0; curLink < numLinks; ++curLink){
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
											parentPos = curLink;
										}

										// Load the bone's weights!
										*curBoneWeight = strtof(tokPos, &tokPos);
										totalWeight += *curBoneWeight;
										// Make sure the total weight doesn't exceed 1!
										if(totalWeight > 1.f){
											*curBoneWeight -= totalWeight - 1.f;
											totalWeight = 1.f;

											++curLink;
											++curBoneID;
											++curBoneWeight;

											break;
										}

										++curBoneID;
										++curBoneWeight;
									}

									// Make sure the total weight isn't less than 1!
									if(totalWeight < 1.f){
										// If we never loaded the parent bone, see if we can add it!
										if(valueIsInvalid(parentPos, meshBoneIndex_t)){
											if(curLink < VERTEX_MAX_WEIGHTS){
												*curBoneID = parentBoneID;
												*curBoneWeight = 0.f;
												parentPos = curLink;

												++curLink;
												++curBoneID;
												++curBoneWeight;

											// If there's no room, just use the first bone we loaded.
											}else{
												parentPos = 0;
											}
										}

										tempVertex.boneWeights[parentPos] += 1.f - totalWeight;
									}

									// Make sure we fill the rest with invalid values so we know they aren't used.
									memset(curBoneID, valueInvalid(meshBoneIndex_t), (VERTEX_MAX_WEIGHTS - curLink) * sizeof(*tempVertex.boneIDs));
									memset(curBoneWeight, 0.f, (VERTEX_MAX_WEIGHTS - curLink) * sizeof(*tempVertex.boneWeights));

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
									memset(&tempVertex.boneIDs[1], valueInvalid(meshBoneIndex_t), (VERTEX_MAX_WEIGHTS - 1) * sizeof(*tempVertex.boneIDs));
									tempVertex.boneWeights[0] = 1.f;
									memset(&tempVertex.boneWeights[1], 0.f, (VERTEX_MAX_WEIGHTS - 1) * sizeof(*tempVertex.boneWeights));
								}


								// If we haven't seen any texture groups yet,
								// allocate a new mesh using the default one.
								if(curMeshData == NULL){
									curMeshData = tempMeshData;
									curMeshData->texGroupPath = NULL;
									curMeshData->tempVerticesSize = 0;
									curMeshData->tempVerticesCapacity = BASE_VERTEX_CAPACITY;
									curMeshData->tempVertices = memoryManagerGlobalAlloc(
										BASE_VERTEX_CAPACITY * sizeof(*curMeshData->tempVertices)
									);
									if(curMeshData->tempVertices == NULL){
										/** MALLOC FAILED **/
									}
									curMeshData->tempIndicesSize = 0;
									curMeshData->tempIndicesCapacity = BASE_INDEX_CAPACITY;
									curMeshData->tempIndices = memoryManagerGlobalAlloc(
										BASE_INDEX_CAPACITY * sizeof(*curMeshData->tempIndices)
									);
									if(curMeshData->tempIndices == NULL){
										/** MALLOC FAILED **/
									}
								}


								checkVertex = curMeshData->tempVertices;
								// Check if this vertex already exists!
								for(i = 0; i < curMeshData->tempVerticesSize; ++i){
									// Looks like it does, so we don't need to store it again!
									if(!vertexDifferent(checkVertex, &tempVertex)){
										break;
									}
									++checkVertex;
								}

								// The vertex does not exist, so add it to the vector!
								if(i == curMeshData->tempVerticesSize){
									// If we're out of space, allocate some more!
									if(curMeshData->tempVerticesSize >= curMeshData->tempVerticesCapacity){
										curMeshData->tempVerticesCapacity = curMeshData->tempVerticesSize * 2;
										curMeshData->tempVertices = memoryManagerGlobalRealloc(
											curMeshData->tempVertices,
											curMeshData->tempVerticesCapacity * sizeof(*curMeshData->tempVertices)
										);
										if(curMeshData->tempVertices == NULL){
											/** REALLOC FAILED **/
										}
									}
									curMeshData->tempVertices[curMeshData->tempVerticesSize] = tempVertex;
									++curMeshData->tempVerticesSize;
								}

								// Add an index for the new vertex!
								if(curMeshData->tempIndicesSize >= curMeshData->tempIndicesCapacity){
									curMeshData->tempIndicesCapacity = curMeshData->tempIndicesSize * 2;
									curMeshData->tempIndices = memoryManagerGlobalRealloc(
										curMeshData->tempIndices,
										curMeshData->tempIndicesCapacity * sizeof(*curMeshData->tempIndices)
									);
									if(curMeshData->tempIndices == NULL){
										/** REALLOC FAILED **/
									}
								}
								curMeshData->tempIndices[curMeshData->tempIndicesSize] = i;
								++curMeshData->tempIndicesSize;
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


		// There's no point keeping the model if no meshes were loaded.
		if(tempMeshDataSize > 0){
			const meshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
			size_t numMeshes = 0;
			// Count the number of valid meshes (meshes with at least one face).
			for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
				if(curMeshData->tempIndicesSize >= 3){
					++numMeshes;

				// If the mesh is invalid, clean it up
				// now so we don't have to do it later.
				}else{
					memoryManagerGlobalFree(curMeshData->texGroupPath);
					memoryManagerGlobalFree(curMeshData->tempVertices);
					memoryManagerGlobalFree(curMeshData->tempIndices);
				}
			}

			// If there's at least one valid mesh, the model should be valid.
			if(numMeshes > 0){
				mesh *curMesh;
				textureGroup **curTexGroup;

				mdl = moduleModelAlloc();
				if(mdl == NULL){
					/** MALLOC FAILED **/
				}
				mdl->name = memoryManagerGlobalAlloc(mdlPathLength + 1);
				if(mdl->name == NULL){
					/** MALLOC FAILED **/
				}
				memcpy(mdl->name, mdlPath, mdlPathLength + 1);
				mdl->meshes = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdl->meshes));
				if(mdl->meshes == NULL){
					/** MALLOC FAILED **/
				}
				mdl->texGroups = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdl->texGroups));
				if(mdl->texGroups == NULL){
					/** MALLOC FAILED **/
				}
				mdl->numMeshes = numMeshes;
				// Initialise the model's skeleton!
				if(tempBonesSize > 0){
					tempBones = memoryManagerGlobalResize(tempBones, sizeof(*tempBones) * tempBonesSize);
					if(tempBones != NULL){
						/** MALLOC FAILED **/
					}
					mdl->skele = moduleSkeletonAlloc();
					if(mdl->skele != NULL){
						/** MALLOC FAILED **/
					}
					skeleInitSet(mdl->skele, mdl->name, mdlPathLength, tempBones, tempBonesSize);
				}else{
					mdl->skele = &g_skeleDefault;
				}

				curMesh = mdl->meshes;
				curTexGroup = mdl->texGroups;
				// Generate buffer objects for each valid
				// mesh and load their texture groups.
				for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
					if(curMeshData->tempIndicesSize >= 3){
						meshGenerateBuffers(
							curMesh,
							curMeshData->tempVertices, curMeshData->tempVerticesSize,
							curMeshData->tempIndices, curMeshData->tempIndicesSize
						);
						if(curMeshData->texGroupPath == NULL){
							*curTexGroup = &g_texGroupDefault;
						}else{
							*curTexGroup = texGroupLoad(
								curMeshData->texGroupPath,
								strlen(curMeshData->texGroupPath)
							);
						}

						// The mesh and texture group should allocate
						// their own memory, so we don't need these anymore.
						memoryManagerGlobalFree(curMeshData->texGroupPath);
						memoryManagerGlobalFree(curMeshData->tempVertices);
						memoryManagerGlobalFree(curMeshData->tempIndices);

						++curMesh;
						++curTexGroup;
					}
				}
			}else{
				memoryManagerGlobalFree(tempBones);
			}
		}else{
			memoryManagerGlobalFree(tempBones);
		}

		// We've already deleted any arrays stored here.
		memoryManagerGlobalFree(tempMeshData);
	}else{
		printf(
			"Unable to open model file!\n"
			"Path: %s\n",
			mdlFullPath
		);
	}


	return(mdl);
}


// This should never be called on the default model!
void modelDelete(model *const restrict mdl){
	// Only free the name if it's in use.
	if(mdl->name != NULL){
		memoryManagerGlobalFree(mdl->name);
	}

	// Free the model's meshes and texture groups.
	if(mdl->numMeshes > 0){
		mesh *curMesh = mdl->meshes;
		const mesh *const lastMesh = &curMesh[mdl->numMeshes];
		for(; curMesh < lastMesh; ++curMesh){
			meshDelete(curMesh);
		}
		memoryManagerGlobalFree(mdl->meshes);
		memoryManagerGlobalFree(mdl->texGroups);
	}
}


return_t modelSetup(){
	const vertex vertices[20] = {
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 1.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = 0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = -0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = -0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = -0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.f, .pos.z = 0.5f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.5774f, .normal.y = -0.5774f, .normal.z = 0.5774f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		}
	};

	const meshVertexIndex_t indices[36] = {
		 0,  1,  2,
		 3,  4,  5,
		 6,  7,  8,
		 9, 10, 11,
		 4, 12, 13,
		14,  9, 15,
		 0, 16,  1,
		 3, 17,  4,
		 6, 18,  7,
		 9, 13, 10,
		 4, 19, 12,
		14,  5,  9
	};

	meshGenerateBuffers(&mdlMeshDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}

void modelCleanup(){
	meshDelete(&mdlMeshDefault);
}