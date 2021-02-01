#include "renderable.h"


#define GLEW_STATIC
#include <GL/glew.h>


// Forward-declare any helper functions!
static void updateShaderBones(
	const skeleton *const restrict mdlSkele, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const GLuint boneStatesID
);


void renderableDefInit(renderableDef *const restrict renderDef, model *const restrict mdl){
	renderDef->mdl = mdl;
	renderDef->texGroup = &g_texGroupDefault;//mdl->texGroup;
}

void renderableInit(renderable *const restrict render, const renderableDef *const restrict renderDef){
	render->mdl = renderDef->mdl;
	texGroupStateInit(&render->texState, renderDef->texGroup);
	//billboardInit(&render->billboardData);
}


// Update a renderable's current state.
void renderableUpdate(renderable *const restrict render, const float time){
	//texGroupStateUpdate(&render->texState, time);
}

#warning "We probably shouldn't have the OpenGL drawing stuff split up so much."
void renderableDraw(
	const renderable *const restrict render, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const meshShader *const restrict shader
){

	const mesh *curMesh = render->mdl->meshes;
	const mesh *const lastMesh = &curMesh[render->mdl->numMeshes];
	const textureGroupFrame *const texFrame = texGroupStateGetFrame(&render->texState);

	updateShaderBones(render->mdl->skele, objSkele, animStates, shader->boneStatesID);

	glActiveTexture(GL_TEXTURE0);
	// Render each of the renderable's meshes!
	for(; curMesh < lastMesh; ++curMesh){
		// Bind the mesh we're using!
		glBindVertexArray(curMesh->vertexArrayID);
		// Bind the texture we're using!
		glBindTexture(GL_TEXTURE_2D, texFrame->tex->id);
		glUniform1fv(shader->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);

		// Draw the renderable!
		glDrawElements(GL_TRIANGLES, curMesh->numIndices, GL_UNSIGNED_INT, NULL);
	}
}


// Check which bones are used by the model and send their matrices to the shader.
static void updateShaderBones(
	const skeleton *const restrict mdlSkele, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const GLuint boneStatesID
){

	const boneIndex_t numBones = mdlSkele->numBones;

	#warning "We could possibly use a global bone states array."
	#warning "Probably not a good idea if we want to create a render queue sometime in the future."
	#warning "It's also a bad idea to be allocating large arrays on the stack."
	mat4 boneStates[SKELETON_MAX_BONES];
	mat4 *curBoneState = boneStates;
	const bone *curBone = mdlSkele->bones;
	const bone *const lastBone = &curBone[numBones];


	// Search the object's skeleton for bones shared by the
	// renderable's skeleton and copy them into a new array.
	for(; curBone < lastBone; ++curBone, ++curBoneState){
		const boneIndex_t boneID = skeleFindBone(objSkele, curBone->name);
		// If this bone appeared in an animation, convert the
		// bone state to a matrix so it can be sent to the shader!
		if(!valueIsInvalid(boneID, boneIndex_t)){
			*curBoneState = animStates[boneID];

		// Otherwise, use the root's transformation!
		}else{
			*curBoneState = *animStates;
		}
	}


	// Send the new bone array to the shader!
	glUniformMatrix4fv(boneStatesID, numBones, GL_FALSE, (GLfloat *)boneStates);
}