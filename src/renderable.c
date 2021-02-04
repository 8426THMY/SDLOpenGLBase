#include "renderable.h"


#define GLEW_STATIC
#include <GL/glew.h>


#include "memoryManager.h"


// Forward-declare any helper functions!
static void prepareShaderBones(
	const skeleton *const restrict mdlSkele, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, GLuint boneStatesID
);


/*
** Initialize a renderable definition using a
** model and a custom array of texture groups.
**
** This assumes that the texture group array has already
** been allocated and is the same size as the model's.
**
** If the input array of texture groups is NULL,
** we should just use the model's default array.
*/
void renderableDefInit(
	renderableDef *const restrict renderDef,
	model *const restrict mdl, textureGroup **const restrict texGroups
){

	renderDef->mdl = mdl;
	renderDef->texGroups = texGroups;
}

void renderableInit(renderable *const restrict render, const renderableDef *const restrict renderDef){
	render->mdl = renderDef->mdl;

	// Allocate an array of texture group states for the model.
	// Note that "mdl->numMeshes" is always greater than zero.
	render->texStates = memoryManagerGlobalAlloc(renderDef->mdl->numMeshes * sizeof(*render->texStates));
	if(render->texStates == NULL){
		/** MALLOC FAILED **/
	}
	{
		// If the renderable definition's array is NULL,
		// we should use the model's default array.
		const textureGroup **curTexGroup = (renderDef->texGroups == NULL) ?
			(const textureGroup **)renderDef->mdl->texGroups :
			(const textureGroup **)renderDef->texGroups
		;
		textureGroupState *curTexState = render->texStates;
		const textureGroupState *const lastTexState = &curTexState[renderDef->mdl->numMeshes];
		do {
			texGroupStateInit(curTexState, *curTexGroup);

			++curTexGroup;
			++curTexState;
		} while(curTexState < lastTexState);
	}

	//billboardInit(&render->billboardData);
}


// Update a renderable's current state.
void renderableUpdate(renderable *const restrict render, const float time){
	textureGroupState *curTexState = render->texStates;
	const textureGroupState *const lastTexState = &curTexState[render->mdl->numMeshes];
	do {
		texGroupStateUpdate(curTexState, time);
		++curTexState;
	} while(curTexState < lastTexState);
}

#warning "We probably shouldn't have the OpenGL drawing stuff split up so much."
void renderableDraw(
	const renderable *const restrict render, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, const meshShader *const restrict shader
){

	const mesh *curMesh = render->mdl->meshes;
	const mesh *const lastMesh = &curMesh[render->mdl->numMeshes];
	const textureGroupState *curTexState = render->texStates;

	prepareShaderBones(render->mdl->skele, objSkele, animStates, shader->boneStatesID);

	glActiveTexture(GL_TEXTURE0);
	// Render each of the renderable's meshes!
	do {
		const textureGroupFrame *const texFrame = texGroupStateGetFrame(curTexState);

		// Bind the mesh we're using!
		glBindVertexArray(curMesh->vertexArrayID);
		// Bind the texture we're using!
		glBindTexture(GL_TEXTURE_2D, texFrame->tex->id);
		glUniform1fv(shader->uvOffsetsID, 4, (GLfloat *)&texFrame->bounds);
		// Draw the renderable!
		glDrawElements(GL_TRIANGLES, curMesh->numIndices, GL_UNSIGNED_INT, NULL);

		++curMesh;
		++curTexState;
	} while(curMesh < lastMesh);
}


void renderableDelete(renderable *const restrict render){
	if(render->texStates != NULL){
		memoryManagerGlobalFree(render->texStates);
	}
}

void renderableDefDelete(renderableDef *const restrict renderDef){
	if(renderDef->texGroups != NULL){
		memoryManagerGlobalFree(renderDef->texGroups);
	}
}


// Check which bones are used by the model and send their matrices to the shader.
static void prepareShaderBones(
	const skeleton *const restrict mdlSkele, const skeleton *const restrict objSkele,
	const mat4 *const restrict animStates, GLuint boneStatesID
){

	const bone *curBone = mdlSkele->bones;
	const bone *const lastBone = &curBone[mdlSkele->numBones];

	#warning "We send each bone to the shader one by one, which may result in a lot of graphics API calls."
	#warning "Depending on how fast these API calls are, it might be better to write to an array of matrices and send them all at once."
	#warning "It would be possible to make shaders store this array, although this wouldn't work if we later build a render queue."
	// Search the object's skeleton for bones shared by the
	// renderable's skeleton and copy them into a new array.
	//
	// We're allowed to increment the uniform location ID, as
 	// the IDs for array elements are guaranteed to be sequential.
	for(; curBone < lastBone; ++curBone, ++boneStatesID){
		const boneIndex_t boneID = skeleFindBone(objSkele, curBone->name);
		// If this bone appeared in an animation, convert the
		// bone state to a matrix so it can be sent to the shader!
		if(!valueIsInvalid(boneID, boneIndex_t)){
			glUniformMatrix4fv(boneStatesID, 1, GL_FALSE, (GLfloat *)&animStates[boneID]);

		// Otherwise, use the root's transformation!
		}else{
			glUniformMatrix4fv(boneStatesID, 1, GL_FALSE, (GLfloat *)animStates);
		}
	}
}