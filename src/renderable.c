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

	render->flags = BILLBOARD_DISABLED;
	render->axis = NULL;
	render->target = NULL;
}


// Update a renderable's current state.
void renderableUpdate(renderable *render, const float time){
	texGroupStateUpdate(&render->texState, time);
}

#warning "We probably shouldn't have the OpenGL drawing stuff split up so much."
void renderableDraw(const renderable *render, const skeleton *objSkele, const mat4 *animStates, const shader *shaderPrg){
	updateShaderBones(render->mdl->skele, objSkele, animStates, shaderPrg->boneStatesID);

	// Bind the texture we're using!
	glBindTexture(GL_TEXTURE_2D, texGroupStateGetFrame(&render->texState, shaderPrg->uvOffsetsID));
	// Bind the vertex array object for the model!
	glBindVertexArray(render->mdl->vertexArrayID);

	// Draw the renderable!
	glDrawElements(GL_TRIANGLES, render->mdl->numIndices, GL_UNSIGNED_INT, 0);
}


void renderableGenerateBillboardMatrix(
	const renderable *render, const camera *cam,
	const vec3 *centroid, const mat4 *root, mat4 *out
){

	// Use the camera's axes for billboarding.
	// We can just use the columns of its view matrix.
	if(flagsAreSet(render->flags, BILLBOARD_SPRITE)){
		vec3InitSet((vec3 *)&out->m[0][0], cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0]);

		if(render->axis != NULL){
			*((vec3 *)&out->m[1][0]) = *render->axis;
		}else{
			vec3InitSet((vec3 *)&out->m[1][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1]);
		}

		vec3InitSet((vec3 *)&out->m[2][0], cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]);

		out->m[0][3] =
		out->m[1][3] =
		out->m[2][3] = 0.f;
		out->m[3][3] = 1.f;


		// Scale the renderable based on its distance from the camera.
		if(flagsAreSet(render->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, centroid);// * render->scale;
			mat4Scale(out, distance, distance, distance);
		}

	// Lock some axes to prevent billboarding around them.
	}else if(flagsAreSet(render->flags, BILLBOARD_LOCK_XYZ)){
		vec3 eye, target, up;

		// The up vector is the axis to billboard on.
		if(render->axis != NULL){
			up = *render->axis;
		}else{
			up = cam->up;
		}

		// Set the eye and target vectors.
		if(flagsAreSet(render->flags, BILLBOARD_TARGET_SPRITE)){
			eye = *cam->target;
			target = cam->pos;
		}else if(render->target != NULL){
			eye = *centroid;
			target = *render->target;
		}else{
			eye = *centroid;
			target = cam->pos;
		}

		// Prevent the renderable from rotating on some axes.
		if(flagsAreUnset(render->flags, BILLBOARD_LOCK_X)){
			target.y = eye.y;
		}
		if(flagsAreUnset(render->flags, BILLBOARD_LOCK_Y)){
			target.x = eye.x;
		}
		if(flagsAreUnset(render->flags, BILLBOARD_LOCK_Z)){
			vec3InitSet(&up, 0.f, 1.f, 0.f);
		}


		// Translate the matrix back to the origin so we can transform it.
		//configuration = mat4TranslatePost(configuration, -centroid.x, -centroid.y, -centroid.z);

		// Scale the renderable based on its distance from the camera.
		if(flagsAreSet(render->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, centroid);// * render->scale;
			//configuration = mat4ScalePost(configuration, distance, distance, distance);
		}

		// Now translate it back and rotate it to face the target.
		/*configuration = mat4TranslatePost(
			mat4MMultM(
				mat4RotateToFace(eye, target, up),
				configuration
			),
			centroid.x, centroid.y, centroid.z
		);*/

	// If we're not using sprites or locking any axes, just use scale billboarding.
	}else{
		if(flagsAreSet(render->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, centroid);// * render->scale;
			mat4Scale(out, distance, distance, distance);
		}
	}
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