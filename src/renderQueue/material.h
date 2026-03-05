#ifndef material_h
#define material_h


#include <stddef.h>

#include "materialProxy.h"
#include "textureGroup.h"
#include "shader.h"


typedef struct materialDef {
	char *name;

	// Array of texture groups, one
	// for each used texture unit.
	const textureGroup *texGroups;
	size_t numTexGroups;

	const shader *prg;

	#if 0
	// Initial values of user variables.
	materialProxyVar *vars;
	size_t numVars;
	// Array of proxies to modify
	// textures during runtime.
	materialProxy *proxies;
	size_t numProxies;
	#endif
} materialDef;

typedef struct material {
	const materialDef *matDef;

	textureGroupState *texStates;

	#if 0
	// Current values of user variables.
	materialProxyVar *vars;
	#endif
} material;


void materialInit(
	material *const restrict mat,
	const materialDef *const restrict matDef
);
void materialUpdate(material *const restrict mat, const float dt);

materialDef *materialDefLoad(
	const char *const restrict materialPath,
	const size_t materialPathLength
);


extern materialDef g_materialDefDefault;
extern materialDef *g_materialDefArrayDefault;


#endif