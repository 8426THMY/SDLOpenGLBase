#include "renderable.h"


#define GLEW_STATIC
#include <GL/glew.h>


// Forward-declare any helper functions!
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
void renderableDraw(const renderable *render, const skeleton *objSkele, const mat4 *animStates, const shaderObject *shader){
	const textureGroupFrame *texFrame = texGroupStateGetFrame(&render->texState);


	updateShaderBones(render->mdl->skele, objSkele, animStates, shader->boneStatesID);

	// Bind the mesh we're using!
	glBindVertexArray(render->mdl->meshData.vertexArrayID);

	glActiveTexture(GL_TEXTURE0);
	// Bind the texture we're using!
	glBindTexture(GL_TEXTURE_2D, texFrame->diffuse->id);
	glUniform1fv(shader->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);

	// Draw the renderable!
	glDrawElements(GL_TRIANGLES, render->mdl->meshData.numIndices, GL_UNSIGNED_INT, NULL);
}


// Check which bones are used by the model and send their matrices to the shader.
static void updateShaderBones(const skeleton *mdlSkele, const skeleton *objSkele, const mat4 *animStates, const GLuint boneStatesID){
	const size_t numBones = mdlSkele->numBones;

	#warning "We could possibly use a global bone states array."
	mat4 boneStates[SKELETON_MAX_BONES];
	mat4 *curBoneState = boneStates;
	const bone *curBone = mdlSkele->bones;
	const bone *lastBone = &curBone[numBones];


	// Before sending our bones to the shader, we
	// convert them all to a matrix representation.
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
}