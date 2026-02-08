#include "model.h"


#include <stdio.h>
#include <string.h>

#include "quat.h"
#include "mat3x4.h"
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
mesh mdlDefMeshDefault;

// By default, the error model only has a name.
// We need to set up the other data the hard way.
modelDef g_mdlDefDefault = {
	.name      = "error",

	.meshes    = &mdlDefMeshDefault,
	.texGroups = &g_texGroupArrayDefault,
	.numMeshes = 1,

	.skele     = &g_skeleDefault,

	.colliders = NULL,
	.colliderBoneIDs = NULL,
	.numColliders = 0,

	.physBodies = NULL,
	.physBoneIDs = NULL,
	.numBodies = 0,

	.aabb.min.x = 0.f,
	.aabb.min.y = 0.f,
	.aabb.min.z = 0.f,
	.aabb.max.x = 0.f,
	.aabb.max.y = 0.f,
	.aabb.max.z = 0.f
};


// We need to store an array of vertices and indices for each mesh
// we're loading. We use a separate mesh for each texture group.
typedef struct modelMeshData {
	char *texGroupPath;

	modelVertexIndex tempVerticesSize;
	modelVertexIndex tempVerticesCapacity;
	modelVertex *tempVertices;

	modelVertexIndex tempIndicesSize;
	modelVertexIndex tempIndicesCapacity;
	modelVertexIndex *tempIndices;
} modelMeshData;


// Forward-declare any helper functions!
static void updateBones(model *const restrict mdl);


// Initialize a model from a model definition.
void modelInit(model *const restrict mdl, const modelDef *const restrict mdlDef){
	mdl->mdlDef = mdlDef;

	// Allocate an array of texture group states for the model.
	// Note that "mdl->numMeshes" is always greater than zero.
	mdl->texStates = memoryManagerGlobalAlloc(mdlDef->numMeshes * sizeof(*mdl->texStates));
	if(mdl->texStates == NULL){
		/** MALLOC FAILED **/
	}else{
		const textureGroup **curTexGroup = (const textureGroup **)mdlDef->texGroups;
		textureGroupState *curTexState = mdl->texStates;
		const textureGroupState *const lastTexState = &curTexState[mdlDef->numMeshes];
		do {
			texGroupStateInit(curTexState, *curTexGroup);

			++curTexGroup;
			++curTexState;
		} while(curTexState < lastTexState);
	}

	transformInit(&mdl->state);
	mdl->boneTransforms = NULL;
	#warning "We should initialize the skeleton to its bind state."
	//objectSetSkeleton(mdl, mdlDef->skele);

	#warning "We need to copy the modelDef's colliders."
	mdl->colliders = NULL;

	#warning "We need to copy the modelDef's rigid bodies."
	mdl->physBodies = NULL;

	mdl->aabb = mdlDef->aabb;
}


/*
** Load an OBJ using the model specified by "mdlDefPath" and return a pointer to it.
** If the model could not be loaded, return a pointer to the default model.
*/
modelDef *modelDefOBJLoad(const char *const restrict mdlDefPath, const size_t mdlDefPathLength){
	modelDef *mdlDef;

	FILE *mdlDefFile;
	char mdlDefFullPath[FILE_MAX_PATH_LENGTH];


	#ifdef TEMP_MODULE_FIND
	// If the model has already been loaded, return a pointer to it!
	if((mdlDef = moduleModelDefFind(mdlDefPath)) != &g_mdlDefDefault){
		return(mdlDef);
	}
	#else
	mdlDef = &g_mdlDefDefault;
	#endif


	// Generate the full path for the model!
	fileGenerateFullResourcePath(
		MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH,
		mdlDefPath, mdlDefPathLength,
		mdlDefFullPath
	);


	// Load the model!
	mdlDefFile = fopen(mdlDefFullPath, "r");
	if(mdlDefFile != NULL){
		modelVertexIndex tempPositionsSize = 0;
		modelVertexIndex tempPositionsCapacity = BASE_POSITION_CAPACITY;
		vec3 *tempPositions;

		modelVertexIndex tempUVsSize = 0;
		modelVertexIndex tempUVsCapacity = BASE_UV_CAPACITY;
		vec2 *tempUVs;

		modelVertexIndex tempNormalsSize = 0;
		modelVertexIndex tempNormalsCapacity = BASE_NORMAL_CAPACITY;
		vec3 *tempNormals;

		size_t tempMeshDataSize = 0;
		size_t tempMeshDataCapacity = BASE_MESH_CAPACITY;
		modelMeshData *tempMeshData;
		modelMeshData *curMeshData = NULL;

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


		while((line = fileReadLine(mdlDefFile, &lineBuffer[0], &lineLength)) != NULL){
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
					const modelMeshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
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
					modelVertex tempVertex;
					const modelVertex *checkVertex = curMeshData->tempVertices;
					modelVertexIndex j;


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
						modelVertexIndex curIndex = strtoul(tokPos, &tokPos, 10) - 1;
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
					memset(
						&tempVertex.boneIDs[1], valueInvalid(boneIndex),
						(MODEL_VERTEX_MAX_BONE_WEIGHTS - 1) * sizeof(tempVertex.boneIDs[0])
					);
					tempVertex.boneWeights[0] = 1.f;
					memset(
						&tempVertex.boneWeights[1], 0.f,
						(MODEL_VERTEX_MAX_BONE_WEIGHTS - 1) * sizeof(tempVertex.boneWeights[0])
					);


					// If this vertex already exists,
					// we don't need to store it again.
					for(j = 0; j < curMeshData->tempVerticesSize; ++j){
						if(!modelVertexDifferent(checkVertex, &tempVertex)){
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
						modelVertexIndex *tempIndices;
						const modelVertexIndex oldIndicesSize = curMeshData->tempIndicesSize;

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

		fclose(mdlDefFile);


		// There's no point keeping the model if no meshes were loaded.
		if(tempMeshDataSize > 0){
			const modelMeshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
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

				mdlDef = moduleModelDefAlloc();
				if(mdlDef == NULL){
					/** MALLOC FAILED **/
				}
				mdlDef->name = memoryManagerGlobalAlloc(mdlDefPathLength + 1);
				if(mdlDef->name == NULL){
					/** MALLOC FAILED **/
				}
				memcpy(mdlDef->name, mdlDefPath, mdlDefPathLength + 1);
				mdlDef->meshes = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdlDef->meshes));
				if(mdlDef->meshes == NULL){
					/** MALLOC FAILED **/
				}
				mdlDef->texGroups = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdlDef->texGroups));
				if(mdlDef->texGroups == NULL){
					/** MALLOC FAILED **/
				}
				mdlDef->numMeshes = numMeshes;
				mdlDef->skele = &g_skeleDefault;

				curMesh = mdlDef->meshes;
				curTexGroup = mdlDef->texGroups;
				// Generate buffer objects for each valid
				// mesh and load their texture groups.
				for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
					if(curMeshData->tempIndicesSize >= 3){
						meshInit(
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
			mdlDefFullPath
		);
	}


	return(mdlDef);
}

/** When loading bone states, they need to be done in order.     **/
/** Additionally, we should ensure bone states are specified     **/
/** after "time". If we skip some frames, we should interpolate. **/
/*
** Load an SMD using the model specified by "mdlDefPath" and return a pointer to it.
** If the model could not be loaded, return a pointer to the default model.
*/
modelDef *modelDefSMDLoad(const char *const restrict mdlDefPath, const size_t mdlDefPathLength){
	modelDef *mdlDef;

	FILE *mdlDefFile;
	char mdlDefFullPath[FILE_MAX_PATH_LENGTH];


	#ifdef TEMP_MODULE_FIND
	// If the model has already been loaded, return a pointer to it!
	if((mdlDef = moduleModelDefFind(mdlDefPath)) != &g_mdlDefDefault){
		return(mdlDef);
	}
	#else
	mdlDef = &g_mdlDefDefault;
	#endif


	// Generate the full path for the model!
	fileGenerateFullResourcePath(
		MODEL_PATH_PREFIX, MODEL_PATH_PREFIX_LENGTH,
		mdlDefPath, mdlDefPathLength,
		mdlDefFullPath
	);


	// Load the model!
	mdlDefFile = fopen(mdlDefFullPath, "r");
	if(mdlDefFile != NULL){
		return_t success = 1;

		size_t tempBonesSize = 0;
		size_t tempBonesCapacity = BASE_BONE_CAPACITY;
		bone *tempBones;

		size_t tempMeshDataSize = 0;
		size_t tempMeshDataCapacity = BASE_MESH_CAPACITY;
		modelMeshData *tempMeshData;
		modelMeshData *curMeshData = NULL;

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


		while(success && (line = fileReadLine(mdlDefFile, &lineBuffer[0], &lineLength)) != NULL){
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
						mdlDefFullPath, line
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
						const boneIndex boneID = strtoul(line, &tokPos, 10);
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
								mdlDefFullPath, line, boneID, tempBonesSize
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
									mdlDefFullPath, line
								);

								success = 0;
							}

						// Otherwise, we're setting the bone's state!
						}else{
							// Get this bone's ID.
							const boneIndex boneID = strtoul(line, &tokPos, 10);
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
									#ifdef TRANSFORM_MATRIX_SHEAR
									mat3InitIdentity(&currentBone->localBind.scale);
									#else
									quatInitIdentity(&currentBone->localBind.shear);
									vec3InitSet(&currentBone->localBind.scale, 1.f, 1.f, 1.f);
									#endif


									// If this bone has a parent, its global bind pose is
									// given by appending its parent's global bind pose.
									// We invert these after computing them for each bone.
									if(currentBone->parent != SKELETON_INVALID_BONE_INDEX){
										transformToMat3x4(
											&currentBone->localBind,
											&currentBone->invGlobalBind
										);
										mat3x4MultiplyMat3x4P2(
											&tempBones[currentBone->parent].invGlobalBind,
											&currentBone->invGlobalBind
										);

									// If the bone has no parent, just use the local bind pose.
									}else{
										currentBone->invGlobalBind = currentBone->localBind;
									}
								}
							}else{
								printf(
									"Error loading model!\n"
									"Path: %s\n"
									"Line: %s\n"
									"Error: Found skeletal data for bone %u, which doesn't exist!\n",
									mdlDefFullPath, line, boneID
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
								const modelMeshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
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
							const boneIndex parentBoneID = strtoul(line, &tokPos, 10);
							// Make sure a bone with this ID actually exists.
							if(parentBoneID < tempBonesSize){
								modelVertex tempVertex;
								boneIndex numLinks;
								const modelVertex *checkVertex;
								modelVertexIndex i;

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
									boneIndex parentPos = valueInvalid(boneIndex);
									boneIndex *curBoneID = tempVertex.boneIDs;
									boneIndex curLink;
									float totalWeight = 0.f;
									float *curBoneWeight = tempVertex.boneWeights;


									// If there are more than the maximum number of
									// supported weights, we'll have to clamp it down!
									if(numLinks > MODEL_VERTEX_MAX_BONE_WEIGHTS){
										printf(
											"Error loading model!\n"
											"Path: %s\n"
											"Line: %s\n"
											"Error: Vertex has too many links! All extra links will be ignored.\n",
											mdlDefFullPath, line
										);

										numLinks = MODEL_VERTEX_MAX_BONE_WEIGHTS;
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
												mdlDefFullPath, line
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
										if(valueIsInvalid(parentPos, boneIndex)){
											if(curLink < MODEL_VERTEX_MAX_BONE_WEIGHTS){
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
									memset(
										curBoneID, valueInvalid(boneIndex),
										(MODEL_VERTEX_MAX_BONE_WEIGHTS - curLink) * sizeof(*tempVertex.boneIDs)
									);
									memset(
										curBoneWeight, 0.f,
										(MODEL_VERTEX_MAX_BONE_WEIGHTS - curLink) * sizeof(*tempVertex.boneWeights)
									);

								// Otherwise, just bind it to the parent bone.
								}else{
									printf(
										"Error loading model!\n"
										"Path: %s\n"
										"Line: %s\n"
										"Error: Vertex has no links! The parent bone will be used.\n",
										mdlDefFullPath, line
									);

									tempVertex.boneIDs[0] = parentBoneID;
									memset(
										&tempVertex.boneIDs[1], valueInvalid(boneIndex),
										(MODEL_VERTEX_MAX_BONE_WEIGHTS - 1) * sizeof(*tempVertex.boneIDs)
									);
									tempVertex.boneWeights[0] = 1.f;
									memset(
										&tempVertex.boneWeights[1], 0.f,
										(MODEL_VERTEX_MAX_BONE_WEIGHTS - 1) * sizeof(*tempVertex.boneWeights)
									);
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
									if(!modelVertexDifferent(checkVertex, &tempVertex)){
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
									mdlDefFullPath, line
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

		fclose(mdlDefFile);


		// There's no point keeping the model if no meshes were loaded.
		if(tempMeshDataSize > 0){
			const modelMeshData *const lastMeshData = &tempMeshData[tempMeshDataSize];
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

				mdlDef = moduleModelDefAlloc();
				if(mdlDef == NULL){
					/** MALLOC FAILED **/
				}
				mdlDef->name = memoryManagerGlobalAlloc(mdlDefPathLength + 1);
				if(mdlDef->name == NULL){
					/** MALLOC FAILED **/
				}
				memcpy(mdlDef->name, mdlDefPath, mdlDefPathLength + 1);
				mdlDef->meshes = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdlDef->meshes));
				if(mdlDef->meshes == NULL){
					/** MALLOC FAILED **/
				}
				mdlDef->texGroups = memoryManagerGlobalAlloc(numMeshes * sizeof(*mdlDef->texGroups));
				if(mdlDef->texGroups == NULL){
					/** MALLOC FAILED **/
				}
				mdlDef->numMeshes = numMeshes;
				// Initialise the model's skeleton!
				if(tempBonesSize > 0){
					tempBones = memoryManagerGlobalResize(tempBones, sizeof(*tempBones) * tempBonesSize);
					if(tempBones != NULL){
						/** MALLOC FAILED **/
					}
					mdlDef->skele = moduleSkeletonAlloc();
					if(mdlDef->skele != NULL){
						/** MALLOC FAILED **/
					}
					// This function will invert the global bind pose.
					skeleInitSet(mdlDef->skele, mdlDef->name, mdlDefPathLength, tempBones, tempBonesSize);
				}else{
					mdlDef->skele = &g_skeleDefault;
				}

				curMesh = mdlDef->meshes;
				curTexGroup = mdlDef->texGroups;
				// Generate buffer objects for each valid
				// mesh and load their texture groups.
				for(curMeshData = tempMeshData; curMeshData < lastMeshData; ++curMeshData){
					if(curMeshData->tempIndicesSize >= 3){
						meshInit(
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
			mdlDefFullPath
		);
	}


	return(mdlDef);
}


// Update a model's current state.
void modelUpdate(model *const restrict mdl, const float dt){
	{
		textureGroupState *curTexState = mdl->texStates;
		const textureGroupState *const lastTexState = &curTexState[mdl->mdlDef->numMeshes];
		// Update each texture group.
		do {
			texGroupStateUpdate(curTexState, dt);
			++curTexState;
		} while(curTexState < lastTexState);
	}

	// Update each skeletal animation.
	skeleStateUpdate(&mdl->skeleState, dt);
	// Generate the object's global bone states.
	updateBones(mdl);
}


// Return whether the model's bounding box is within the frustum.
return_t modelInFrustum(
	const model *const restrict mdl,
	const renderFrustum *const restrict frustum
){

	#warning "Finish this!!"
	return 1;
}

/*
** Create and return a render queue key for the model.
** If the model has multiple meshes and textures, this
** will just return the key for the first one.
*/
renderQueueID modelGetRenderQueueKey(
	const model *const restrict mdl,
	renderQueueKey *const restrict key
){

	#warning "Finish this!!"
	*key = 0;
	return 0;
}

/*
** To transform a mesh vertex by a bone, we need to move it into
** the bone's local space by multiplying by the global inverse bind
** pose, and then transform it using the global animation state.
**
** During our update step, we generate global bone states for each
** bone in the animation. Before we send them to the shader, we need
** to bring them back into local space and convert them to matrices.
*/
void modelUpdateGlobalTransform(
	model *const restrict mdl, const float dt
){

	const boneState *curStateBone = mdl->skeleState.states;
	const boneState *const lastStateBone = &curStateBone[mdl->mdlDef->skele->numBones];
	const boneState *prevStateBone = mdl->skeleState.prevStates;
	const mat3x4 *interpState = mdl->skeleState.interpStates;

	for(; curStateBone < lastStateBone; ++curStateBone){
		boneState tempBone;

		transformInterpSet(prevStateBone, curStateBone, dt, &tempBone);
		transformToMat3x4(&tempBone, interpState);
		mat3x4MultiplyMat3x4P1(interpState, &curSkeleBone->invGlobalBind);

		++curSkeleBone;
		++prevStateBone;
		++interpState;
	}
}

// Models don't have any pre-draw logic.
void modelPreDraw(
	const model *const restrict mdl,
	renderView *const restrict view
){

	return;
}

void modelDraw(const model *const restrict mdl){
	const mesh *curMesh = mdl->mdlDef->meshes;
	const mesh *const lastMesh = &curMesh[mdl->mdlDef->numMeshes];
	const textureGroupState *curTexState = mdl->texStates;

	/** TEMPORARY DEBUG DRAW TEST **/
	{
		if(obj->skeleState.skele->numBones > 1){
			debugDrawSkeleton(
				&obj->skeleState,
				debugDrawInfoInit(
					DEBUG_DRAW_MODE_LINES, DEBUG_DRAW_FILL_LINE,
					vec3InitSetC(0.f, 1.f, 0.f), 1.f
				)
			);
		}
		if(obj->physBodies != NULL){
			if(obj->physBodies->colliders->node != NULL){
				debugDrawColliderAABB(
					&(obj->physBodies->colliders->node->aabb),
					debugDrawInfoInit(
						DEBUG_DRAW_MODE_TRIANGLES, DEBUG_DRAW_FILL_LINE,
						vec3InitSetC(1.f, 0.4f, 0.f), 1.f
					)
				);
			}
			// We don't update the rigid bodies when they aren't collidable.
			if(physRigidBodyIsCollidable(obj->physBodies)){
				debugDrawColliderHull(
					&(obj->physBodies->colliders->global.data.hull),
					debugDrawInfoInit(
						GL_TRIANGLES, GL_LINE,
						vec3InitSetC(1.f, 0.4f, 0.f), 1.f
					)
				);
			}
		}
	}

	shaderPrgModelLoadSharedUniforms(mdl->skeleState.interpStates);
	do {
		const textureGroupFrame *const texFrame = texGroupStateGetFrame(curTexState);
		/*
		materialBind(const material *const restrict mat){
			size_t i;
			const texture *curTex = mat->textures;
			const rectangle *curUVs = mat->uvOffsets;

			shaderPrgBind(mat->shader);

			// Bind each texture to its texture unit.
			for(i = 0; i < mat->numTextures; ++i){
				textureBind(curTex, curUVs, i);

				++curTex;
				++curUVs;
			}
		}
		// somehow bind UV offsets for animated textures
		meshDraw(curMesh);
		*/

		++curMesh;
		++curTexState;
	} while(curMesh < lastMesh);
}


void modelDelete(model *const restrict mdl){
	if(mdl->texStates != NULL){
		memoryManagerGlobalFree(mdl->texStates);
	}
}

// This should never be called on the default model!
void modelDefDelete(modelDef *const restrict mdlDef){
	// Only free the name if it's in use.
	if(mdlDef->name != NULL){
		memoryManagerGlobalFree(mdlDef->name);
	}

	// Free the model's meshes and texture groups.
	if(mdlDef->numMeshes > 0){
		mesh *curMesh = mdlDef->meshes;
		const mesh *const lastMesh = &curMesh[mdlDef->numMeshes];
		for(; curMesh < lastMesh; ++curMesh){
			meshDelete(curMesh);
		}
		memoryManagerGlobalFree(mdlDef->meshes);
		memoryManagerGlobalFree(mdlDef->texGroups);
	}
}


return_t modelSetup(){
	const modelVertex vertices[20] = {
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

	const modelVertexIndex indices[36] = {
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

	meshModelInit(
		&mdlDefMeshDefault,
		vertices, sizeof(vertices)/sizeof(*vertices),
		indices, sizeof(indices)/sizeof(*indices)
	);


	return(1);
}

void modelCleanup(){
	meshDelete(&mdlDefMeshDefault);
}


/*
** Update an object's skeleton using the following procedure for each bone:
**     1. Begin with the bone's local bind pose, B.
**     2. Append the bone's user transformation, U.
**     3. Append the bone's parent's transformation, P.
**     4. Prepend the bone's animation transformation, A.
** If S is the final state and these transformations are treated as matrices, then
**     S = P*U*B*A.
** The resulting transformation S takes us from the bone's local space to global space.
** Note that this process implicitly assumes that parent bones are animated before children.
*/
static void updateBones(model *const restrict mdl){
	boneIndex curBoneID;
	const boneIndex numBones = mdl->mdlDef->skele->numBones;

	const boneState *curTransform = mdl->boneTransforms;
	const bone *curSkeleBone = mdl->mdlDef->skele->bones;
	boneState *curStateBone = mdl->skeleState.states;
	boneState *prevStateBone = mdl->skeleState.prevStates;

	physicsRigidBody *curBody = mdl->physBodies;
	// We store the rigid bodies in order of increasing bone IDs.
	// We can simply move to the next ID when we find a bone with a rigid body.
	const boneIndex *curPhysBoneID = mdl->mdlDef->physBoneIDs;
	const boneIndex *const lastPhysBoneID = &mdl->mdlDef->physBoneIDs[mdl->mdlDef->numBodies];

	for(curBoneID = 0; curBoneID < numBones; ++curBoneID){
		// Keep the previous state so we can
		// interpolate from it during rendering.
		*prevStateBone = *curStateBone;

		// If a simulated rigid body is attached to the current bone,
		// we should transform the bone based off the rigid body's state.
		if(
			curPhysBoneID != lastPhysBoneID &&
			*curPhysBoneID == curBoneID &&
			physRigidBodyIsSimulated(curBody)
		){

			physRigidBodyUpdatePosition(curBody);
			// Copy the rigid body's state over to the bone.
			*curStateBone = curBody->state;

			modulePhysicsRigidBodyNext(curBody);
			++curPhysBoneID;

		// Otherwise, if this bone does not have a rigid
		// body attached, we should animate it normally.
		}else{
			// Prepend any user transformations to the bone's local bind state.
			// S = U*B
			transformMultiplyOut(curTransform, &curSkeleBone->localBind, curStateBone);
			// If the bone has a parent, append its transformation.
			// S = P*U*B
			if(curSkeleBone->parent != SKELETON_INVALID_BONE_INDEX){
				transformMultiplyP2(&obj->skeleState.bones[curSkeleBone->parent], curStateBone);
			}
			// Prepend the total animation transformations.
			// S = P*U*B*A
			skeleStatePrependAnimations(
				&obj->skeleState,
				curBoneID, &curSkeleBone->localBind,
				curStateBone
			);

			// If the current bone has a rigid body that is not simulated,
			// we should transform the rigid body based off the bone's state.
			if(curPhysBoneID != lastPhysBoneID && *curPhysBoneID == curBoneID){
				// Copy the bone's state over to the rigid body.
				curBody->state = *curStateBone;
				// Update the rigid body's centroid to reflect its new position.
				physRigidBodyCentroidFromPosition(curBody);

				modulePhysicsRigidBodyNext(curBody);
				++curPhysBoneID;
			}
		}

		++curTransform;
		++curSkeleBone;
		++curStateBone;
		++prevStateBone;
		++curBoneID;
	}
}