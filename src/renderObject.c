#include "renderObject.h"


#include "programSettings.h"

#include "model.h"


vector allRenderObjects;


void renderObjStateInit(renderObjState *objState, const size_t mdlPos){
	/*transformStateInit(&objState->transform);

	//If the model we want to use has been loaded, use it!
	if(mdlPos < loadedModels.size){
		objState->mdlPos = mdlPos;

	//Otherwise, use the error model!
	}else{
		objState->mdlPos = 0;
	}
	model *objModel = (model *)vectorGet(&loadedModels, objState->mdlPos);

	texGroupAnimInstInit(&objState->texGroup, objModel->texGroupPos);
	texGroupAnimInstSetAnim(&objState->texGroup, 0);

	objState->skeleObj = malloc(sizeof(*objState->skeleObj));
	skeleObjInit(objState->skeleObj, &objModel->skele);*/
}

/** We'll be able to remove the vector eventually. **/
void renderObjCreate(const size_t mdlPos){
	renderObject renderObj;

	stateObjInit(&renderObj, sizeof(renderObjState));
	renderObjStateInit((renderObjState *)renderObj.states[0], mdlPos);

	vectorAdd(&allRenderObjects, &renderObj, 1);
}


void renderObjStateDraw(const renderObjState *objState, const vec3 *camPos, const mat4 *viewProjectionMatrix, const shader *shaderProgram, const float time){
	mat4 mvpMatrix = *viewProjectionMatrix;


	//These variables store the interpolated state of the object between the previous and current updates.
	vec3 newPos;
	quat newRot;
	vec3 newScale;
	transformStateGenRenderState(&objState->transform, time, &newPos, &newRot, &newScale);


	//Translate the model!
	mat4TranslateVec3(&mvpMatrix, &newPos);

	//Make the model face the screen!
	/** Make a function for billboards! To look an axis, set the  **/
	/** target's position for that axis to the camera's position! **/
	/*mat4 newViewMatrix;
	vec3 target, up;
	vec3InitSet(&target, 0.f, camPos->y, 0.f);
	vec3InitSet(&up, 0.f, 1.f, 0.f);
	//Generate a view matrix that looks from the camera to target!
	mat4LookAt(&newViewMatrix, camPos, &target, &up);
	//                        Right Vector           Up Vector              Forward Vector
	mat4 tempMatrix = {.m = {{newViewMatrix.m[0][0], newViewMatrix.m[1][0], newViewMatrix.m[2][0], 0.f},
	                         {newViewMatrix.m[0][1], newViewMatrix.m[1][1], newViewMatrix.m[2][1], 0.f},
	                         {newViewMatrix.m[0][2], newViewMatrix.m[1][2], newViewMatrix.m[2][2], 0.f},
	                         {0.f,                   0.f,                   0.f,                   1.f}}};
	mat4MultiplyToMat4(&tempMatrix, &mvpMatrix);*/

	//Apply the rotation transformation to our mvpMatrix!
	mat4RotateQuat(&mvpMatrix, &newRot);

	//Scale the model!
	mat4ScaleVec3(&mvpMatrix, &newScale);


	//Set our uniform variables!
	glUniformMatrix4fv(shaderProgram->modelViewMatrixID, 1, GL_FALSE, (GLfloat *)&mvpMatrix);


	/** This stuff should be moved outside of this function! **/
	glActiveTexture(GL_TEXTURE0);
	//Bind the texture we're using!
	/*textureGroup *tempTexGroup = (textureGroup *)vectorGet(&loadedTextureGroups, objState->texGroup.texGroupPos);
	glBindTexture(GL_TEXTURE_2D, texGroupGetFrame(tempTexGroup, objState->texGroup.animData.currentAnim, objState->texGroup.animData.currentFrame, shaderProgram->uvOffsetsID));*/

	//Bind the vertex array object for the model!
	/*model *tempModel = (model *)vectorGet(&loadedModels, objState->mdlPos);
	glBindVertexArray(tempModel->vertexArrayID);*/


	//We need to convert bone information to matrices so we can send them to the shader!
	mat4 boneStates[objState->skeleObj->skele->numBones];

	//Generate the skeleton's state!
	if(objState->skeleObj->numAnims > 0){
		size_t i;
		//Fill up the array!
		for(i = 0; i < objState->skeleObj->skele->numBones; ++i){
			boneStateConvertToMat4(&objState->skeleObj->state[i], &boneStates[i]);
		}

		//Draw the object's skeleton!
		for(i = 1; i < objState->skeleObj->skele->numBones; ++i){
			vec3 curBone, parBone;
			//Use bone states!
			curBone = objState->skeleObj->state[i].pos;
			if(objState->skeleObj->skele->bones[i].parent != -1){
				parBone = objState->skeleObj->state[objState->skeleObj->skele->bones[i].parent].pos;
			}else{
				parBone = curBone;
			}

			//Use matrices!
			/*vec3InitZero(&curBone);
			parBone = curBone;
			vec3Transform(&boneStates[i], &curBone, &curBone);
			if(objState->skeleObj->skele->bones[i].parent != -1){
				vec3Transform(&boneStates[objState->skeleObj->skele->bones[i].parent], &parBone, &parBone);
			}else{
				parBone = curBone;
			}*/

			glBegin(GL_LINES);
			glVertex3f(parBone.x, parBone.y, parBone.z);
			glVertex3f(curBone.x, curBone.y, curBone.z);
			glEnd();
		}

	//This is temporary.
	}else{
		size_t i;
		//Fill up the array!
		for(i = 0; i < objState->skeleObj->skele->numBones; ++i){
			boneStateConvertToMat4(&objState->skeleObj->skele->bones[i].state, &boneStates[i]);
		}
	}

	//Send them to the shader!
	glUniformMatrix4fv(shaderProgram->boneStatesID, objState->skeleObj->skele->numBones, GL_FALSE, (GLfloat *)&boneStates);


	//Draw the model!
	//glDrawElements(GL_TRIANGLES, tempModel->totalIndices, GL_UNSIGNED_INT, 0);


	/** This stuff should be moved outside of this function too! **/
	//Unbind the vertex array object!
	glBindVertexArray(0);

	//Unbind the texture!
	glBindTexture(GL_TEXTURE_2D, 0);
}

void renderObjStateShift(renderObject *renderObj){
	stateObjShift(renderObj, sizeof(renderObjState), NULL, NULL);

	renderObjState *currentState = renderObj->states[0];
	interpVec3MoveToNextState(&currentState->transform.pos);
	interpQuatMoveToNextState(&currentState->transform.rot);
	interpVec3MoveToNextState(&currentState->transform.scale);


	size_t i;
	//We should probably have a function that updates prepares animations for the next update.
	for(i = 0; i < currentState->skeleObj->numAnims; ++i){
		stateObjShift(&currentState->skeleObj->anims[i], sizeof(skeleAnimState), NULL, NULL);
	}
}


void renderObjDelete(renderObject *renderObj){
	renderObjState *currentState = (renderObjState *)renderObj->states[0];
	if(currentState != NULL && currentState->skeleObj != NULL){
		free(currentState->skeleObj);
	}
	stateObjDelete(renderObj, NULL);
}


return_t renderObjModuleSetup(){
	vectorInit(&allRenderObjects, sizeof(renderObject));

	//We don't need to create an error renderObject since it's
	//basically just a wrapper for models and textureGroups.

	return(1);
}

void renderObjModuleCleanup(){
	size_t i;
	for(i = 0; i < allRenderObjects.size; ++i){
		renderObjDelete((renderObject *)vectorGet(&allRenderObjects, i));
	}

	vectorClear(&allRenderObjects);
}