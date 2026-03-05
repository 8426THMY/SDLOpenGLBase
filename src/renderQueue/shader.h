#ifndef shader_h
#define shader_h


#include <stdint.h>

#include "mat3x4.h"
#include "skeleton.h"

#include "utilTypes.h"


#define SHADER_COMPONENT_NUM_TYPES 3

#define SHADER_COMPONENT_TYPE_VERTEX   0
#define SHADER_COMPONENT_TYPE_FRAGMENT 1
#define SHADER_COMPONENT_TYPE_GEOMETRY 2


#define SHADER_COMPONENT_INVALID_ID 0
#define SHADER_INVALID_ID           0


#define SHADER_NUM_BLOCKS 2

#define SHADER_BLOCK_SHAREDDATA   0
#define SHADER_BLOCK_SKELETONDATA 1


#define SHADER_UNIFORM_TEXTURE0          0
#define SHADER_UNIFORM_TEXTURE0TRANSFORM 1


typedef uint_least8_t shaderComponentType;

typedef struct shaderComponent {
	unsigned int id;
	shaderComponentType type;
} shaderComponent;


// These structs represent the data stored
// by the shaders' shared uniform blocks!
typedef struct shaderBlockSharedData {
	mat4 vpMatrix;
} shaderBlockSharedData;

typedef struct shaderBlockSkeletonData {
	mat3x4 boneStates[SKELETON_MAX_BONES];
} shaderBlockSkeletonData;


typedef uint_least8_t shaderUniformName;

typedef struct shaderUniform {
	unsigned int id;
	shaderUniformName name;
} shaderUniform;

typedef struct shader {
	unsigned int id;
	shaderUniform *uniforms;
	size_t numUniforms;
} shader;


void shaderCompInit(
	shaderComponent *const restrict shaderComp,
	const shaderComponentType type, const char *const restrict shaderPath
);
void shaderCompDelete(shaderComponent *const restrict shaderComp);

void shaderInit(
	shader *const restrict prg, const shaderType type
	const shaderComponent *const restrict vertexShader,
	const shaderComponent *const restrict fragmentShader
);
void shaderBind(const shader *const restrict prg);
void shaderDelete(shader *const restrict prg);

void shaderSetup();
void shaderLoadBlockData(
	const unsigned int blockID,
	const size_t offset,
	const size_t bytes,
	const void *const restrict data
);
void shaderCleanup();


#endif
