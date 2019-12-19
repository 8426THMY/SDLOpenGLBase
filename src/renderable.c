#include "renderable.h"


#define GLEW_STATIC
#include <GL/glew.h>


static void updateShaderBones(const skeleton *mdlSkele, const skeleton *objSkele, const mat4 *animStates, const GLuint boneStatesID);


void renderableDefInit(renderableDef *renderDef, model *mdl){
	renderDef->mdl = mdl;
	renderDef->texGroup = mdl->texGroup;
}

void renderableInit(renderable *render, const renderableDef *renderDef){
	render->mdl = renderDef->mdl;
	texGroupStateInit(&render->texState, renderDef->texGroup);
	billboardInit(&render->billboardData);
}


// Update a renderable's current state.
void renderableUpdate(renderable *render, const float time){
	texGroupStateUpdate(&render->texState, time);
}

#warning "We probably shouldn't have the OpenGL drawing stuff split up so much."
void renderableDraw(const renderable *render, const skeleton *objSkele, const mat4 *animStates, const shader *shaderPrg){
	const textureGroupFrame *texFrame = texGroupStateGetFrame(&render->texState);


	updateShaderBones(render->mdl->skele, objSkele, animStates, shaderPrg->boneStatesID);

	glActiveTexture(GL_TEXTURE0);
	// Bind the texture we're using!
	glBindTexture(GL_TEXTURE_2D, texFrame->diffuse->id);
	glUniform1fv(shaderPrg->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);
	// Bind the vertex array object for the model!
	glBindVertexArray(render->mdl->meshData.vertexArrayID);

	// Draw the renderable!
	glDrawElements(GL_TRIANGLES, render->mdl->meshData.numIndices, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// Check which bones are used by the model and send their matrices to the shader.
static void updateShaderBones(const skeleton *mdlSkele, const skeleton *objSkele, const mat4 *animStates, const GLuint boneStatesID){
	const size_t numBones = mdlSkele->numBones;
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

	curBone = mdlSkele->bones;
	lastBone = &curBone[numBones];
	for(; curBone < lastBone; ++curBone, ++curBoneState){
		const size_t boneID = skeleFindBone(objSkele, curBone->name);
		// If this bone appeared in an animation, convert the
		// bone state to a matrix so it can be sent to the shader!
		if(!valueIsInvalid(boneID)){
			*curBoneState = animStates[boneID];

		// Otherwise, use the root's transformation!
		}else{
			*curBoneState = *animStates;
		}
	}

	// Send them to the shader!
	glUniformMatrix4fv(boneStatesID, numBones, GL_FALSE, (GLfloat *)boneStates);

	// Now we can free the bone states.
	memoryManagerGlobalFree(boneStates);
}