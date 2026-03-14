#include "material.h"


#include "renderQueue.h"

#include "memoryManager.h"


// We use this material as a placeholder or
// when the desired one can't be found or loaded.
materialDef g_materialDefDefault = {
	.name         = "error",
	.id           = -1,

	.texGroups    = g_texGroupArrayDefault,
	.numTexGroups = 1,

	// We don't really know ahead of time what's going to
	// be using the error material, so a NULL pointer here
	// indicates that whatever is drawing this should use
	// its "default" shader.
	.prg          = NULL,

	#if 0
	.vars         = NULL,
	.numVars      = 0,
	.proxies      = NULL,
	.numProxies   = 0
	#endif
};
// A lot of structures store arrays of texture group pointers.
// By having a pointer on the stack, we can create a double pointer
// to the default texture group without any memory allocation.
materialDef *g_materialDefArrayDefault = &g_materialDefDefault;


materialDef *materialDefLoad(
	const char *const restrict materialPath,
	const size_t materialPathLength
){

	return(NULL);
}


void materialInit(
	material *const restrict mat,
	const materialDef *const restrict matDef
){

	mat->matDef = matDef;

	if(matDef->numTexGroups > 0){
		const textureGroup *curTexGroup = matDef->texGroups;
		const textureGroup *const lastTexGroup = &curTexGroup[matDef->numTexGroups];
		textureGroupState *curTexState;
		
		mat->texStates = memoryManagerGlobalAlloc(
			matDef->numTexGroups * sizeof(*mat->texStates)
		);
		if(mat->texStates == NULL){
			/** MALLOC FAILED **/
		}

		curTexState = mat->texStates;
		// Initialize the texture group state for each texture group.
		do {
			texGroupStateInit(curTexState, curTexGroup);
			++curTexState;
			++curTexGroup;
		} while(curTexGroup != lastTexGroup);
	}else{
		mat->texStates = NULL;
	}
}

void materialUpdate(material *const restrict mat, const float dt){
	if(mat->texStates != NULL){
		textureGroupState *curTexState = matDef->texStates;
		const textureGroupState *const lastTexState = &curTexState[matDef->numTexGroups];
		do {
			texGroupStateUpdate(curTexState, dt);
		} while(curTexState != lastTexState);
	}
}

#warning "We currently assume all materials are opaque!"
return_t materialIsTranslucent(const material *const restrict mat){
	return(0);
}

void materialBind(const material *const restrict mat){
	const shader *prg = mat->matDef->prg;
	shaderUniform *curUniform = prg->uniforms;
	const shaderUniform *lastUniform = &curUniform[prg->numUniforms];
	const size_t numTexGroups = mat->matDef->numTexGroups;

	#warning "We probably need to update the material's animations/proxies somewhere."
	//const textureGroupFrame *const texFrame = texGroupStateGetFrame(curTexState);

	shaderBind(prg);
	// Set the values of each of the shader's uniforms!
	for(; curUniform != lastUniform; ++curUniform){
		switch(curUniform->name){
			case SHADER_UNIFORM_TEXTURE0:
				if(numTexGroups > 0){
					textureBind(&mat->texStates[0], 0);
				}
			break;
			#warning "We should probably use a 2x3 matrix here instead."
			case SHADER_UNIFORM_TEXTURE0TRANSFORM:
				if(numTexGroups > 0){
					const textureGroupFrame *const texFrame =
						texGroupStateGetFrame(mat->texStates[0]);
					shaderUniformBindFloat(curUniform->id, 4, &texFrame->bounds);
				}
			break;
		}
	}
}