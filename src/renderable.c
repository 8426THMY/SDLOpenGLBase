#include "renderable.h"


#define GLEW_STATIC
#include <GL/glew.h>


void renderableDefInit(renderableDef *renderDef, model *mdl){
	renderDef->mdl = mdl;
	renderDef->texGroup = mdl->texGroup;
}

void renderableInit(renderable *render, const renderableDef *renderDef){
	render->mdl = renderDef->mdl;
	texGroupStateInit(&render->texState, renderDef->texGroup);
}


// Update a renderable's current state.
void renderableUpdate(renderable *render, const float time){
	texGroupStateUpdate(&render->texState, time);
}

#warning "We probably shouldn't have the OpenGL drawing stuff split up so much."
void renderableDraw(const renderable *render, const skeleton *skele, const mat4 *animStates, const shader *shaderProgram){
	const model *renderMdl = render->mdl;

	const size_t numBones = renderMdl->skele->numBones;
	const bone *curBone;
	const bone *lastBone;

	mat4 *curBoneState;
	// Before sending our bones to the shader, we
	// convert them all to a matrix representation.
	#warning "It'd be nice if we could avoid this. Maybe use a fixed sized array?"
	mat4 *boneStates = memoryManagerGlobalAlloc(sizeof(*boneStates) * numBones);
	if(boneStates == NULL){
		/** MALLOC FAILED **/
	}
	curBoneState = boneStates;

	curBone = renderMdl->skele->bones;
	lastBone = &curBone[numBones];
	for(; curBone < lastBone; ++curBone, ++curBoneState){
		const size_t boneID = skeleFindBone(skele, curBone->name);
		// If this bone appeared in an animation, convert the
		// bone state to a matrix so it can be sent to the shader!
		if(!VALUE_IS_INVALID(boneID)){
			*curBoneState = animStates[boneID];

		// Otherwise, use the root's transformation!
		}else{
			*curBoneState = *animStates;
		}
	}

	// Send them to the shader!
	glUniformMatrix4fv(shaderProgram->boneStatesID, numBones, GL_FALSE, (GLfloat *)boneStates);
	// Now we can free the bone states.
	memoryManagerGlobalFree(boneStates);


	// Bind the texture we're using!
	glBindTexture(GL_TEXTURE_2D, texGroupStateGetFrame(&render->texState, shaderProgram->uvOffsetsID));
	// Bind the vertex array object for the model!
	glBindVertexArray(renderMdl->vertexArrayID);


	// Draw the renderable!
	glDrawElements(GL_TRIANGLES, renderMdl->numIndices, GL_UNSIGNED_INT, 0);
}