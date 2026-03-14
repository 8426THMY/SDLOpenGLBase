#ifndef material_h
#define material_h


#include <stddef.h>

#include "materialProxy.h"
#include "textureGroup.h"
#include "shader.h"

#include "renderView.h"


typedef struct materialDef {
	char *name;
	// Materials are sorted by shader before being assigned
	// an I.D. This means that we can sort by material I.D.
	// to minimize shader binds.
	#warning "We'll need to re-sort everything if we load a new material during runtime."
	#warning "Also, animations that use multiple textures aren't sorted properly during rendering."
	#warning "I guess it's possible to work around this by skipping I.D.s when materials use multiple textures."
	#warning "It might be good to go back to render queue keys having shader and texture parameters."
	unsigned int id;

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


materialDef *materialDefLoad(
	const char *const restrict materialPath,
	const size_t materialPathLength
);

void materialInit(
	material *const restrict mat,
	const materialDef *const restrict matDef
);
void materialUpdate(material *const restrict mat, const float dt);
return_t materialIsTranslucent(const material *const restrict mat);
renderQueueID materialGetRenderQueueKey(
	const material *const restrict mat,
	renderQueueKey *const restrict key
);
void materialBind(const material *const restrict mat);


extern materialDef g_materialDefDefault;
extern materialDef *g_materialDefArrayDefault;


#endif