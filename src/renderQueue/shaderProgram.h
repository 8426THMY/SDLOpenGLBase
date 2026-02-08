#ifndef shaderProgram_h
#define shaderProgram_h


#include <stdint.h>

#include "utilTypes.h"


#define SHADER_COMPONENT_NUM_TYPES 3

#define SHADER_COMPONENT_TYPE_VERTEX   0
#define SHADER_COMPONENT_TYPE_FRAGMENT 1
#define SHADER_COMPONENT_TYPE_GEOMETRY 2

#define SHADER_COMPONENT_INVALID_ID 0


#define SHADER_PROGRAM_NUM_TYPES 2

#define SHADER_PROGRAM_TYPE_MODEL  0
#define SHADER_PROGRAM_TYPE_SPRITE 1

#define SHADER_PROGRAM_INVALID_ID 0


typedef uint_least8_t shaderComponentType;
typedef uint_least8_t shaderProgramType;

typedef struct shaderComponent {
	unsigned int id;
	shaderType type;
} shaderComponent;

typedef struct shaderProgram {
	unsigned int id;
	unsigned int sharedDataID;

	union {
		shaderProgramModel modelShader;
		shaderProgramSprite spriteShader;
	} data;
	shaderProgramType type;
} shaderProgram;


unsigned int shaderCompLoad(
	shaderComponent *const restrict shaderComp,
	const shaderComponentType type, const char *const restrict shaderPath
);
void shaderCompDelete(shaderComponent *const restrict shaderComp);

void shaderPrgInit(
	shaderProgram *const restrict shader, const shaderProgramType type
	const shaderComponent *const restrict vertexShader,
	const shaderComponent *const restrict fragmentShader
);
void shaderPrgBind(const shader *const restrict prg);
void shaderPrgDelete(shader *const restrict prg);

void shaderPrgSetup();
void shaderPrgLoadSharedUniforms(const mat4 *const restrict vpMatrix);
void shaderPrgCleanup();


#endif
