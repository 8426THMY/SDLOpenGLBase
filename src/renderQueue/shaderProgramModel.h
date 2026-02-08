#ifndef shaderProgramModel_h
#define shaderProgramModel_h


#include "mat3x4.h"
#include "skeleton.h"

#include "utilTypes.h"


typedef struct shaderProgramModel {
	unsigned int modelDataID;
	unsigned int uvOffsetsID;
} shaderProgramModel;


void shaderPrgModelInit(
	shaderProgramModel *const restrict shader,
	const unsigned int shaderID
);

void shaderPrgModelSetup();
void shaderPrgModelLoadSharedUniforms(
	const mat3x4 *const restrict boneStates,
	const boneIndex numBones
);
void shaderPrgModelCleanup();


#endif