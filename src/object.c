#include "object.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "moduleRenderable.h"
#include "moduleSkeleton.h"
#include "modulePhysics.h"


void objectDefInit(objectDef *objDef){
	objDef->name = NULL;

	objDef->skele = NULL;
	objDef->anims = NULL;
	objDef->numAnims = 0;

	objDef->colliders = NULL;
	objDef->physBodies = NULL;

	objDef->renderables = NULL;
}

void objectInit(object *obj, const objectDef *objDef){
	const physicsRigidBodyDef *curBody = objDef->physBodies;
	const renderableDef *curRenderable = objDef->renderables;

	obj->objDef = objDef;

	obj->anims = NULL;
	obj->bones = NULL;

	obj->colliders = NULL;
	obj->physBodies = NULL;
	// Instantiate the object's physics rigid bodies.
	while(curBody != NULL){
		obj->physBodies = modulePhysicsBodyPrepend(&obj->physBodies);
		physRigidBodyInit(obj->physBodies, curBody);
		curBody = memSingleListNext(curBody);
	}

	obj->renderables = NULL;
	// Instantiate the object's default renderables.
	while(curRenderable != NULL){
		obj->renderables = moduleRenderablePrepend(&obj->renderables);
		renderableInit(obj->renderables, curRenderable);
		curRenderable = memSingleListNext(curRenderable);
	}
}


return_t renderObjDefLoad(objectDef *objDef, const char *objFile){
	return(0);
}


void objectUpdate(object *obj, const float time){
	renderable *curRenderable = obj->renderables;
	// Animate each of the renderables.
	while(curRenderable != NULL){
		renderableUpdate(curRenderable, time);
		curRenderable = memSingleListNext(curRenderable);
	}
}

#warning "A lot of this stuff should be moved outside, especially the OpenGL code and skeleton stuff."
void objectDraw(const object *obj, const vec3 *camPos, mat4 mvpMatrix, const shader *shaderProgram, const float time){
	const renderable *curRenderable;

	const size_t numBones = (obj->objDef->skele == NULL) ? 0 : obj->objDef->skele->numBones;
	const boneState *curBone = obj->bones;
	const boneState *lastBone = &curBone[numBones];
	mat4 *curState;
	// Before sending our bones to the shader, we
	// convert them all to a matrix representation.
	mat4 *boneStates = memoryManagerGlobalAlloc(sizeof(*boneStates) * numBones);
	if(boneStates == NULL){
		/** MALLOC FAILED **/
	}
	curState = boneStates;
	// Convert our bone states to matrices!
	for(; curBone < lastBone; ++curBone, ++curState){
		transformStateToMat4(curBone, curState);
	}
	// Send them to the shader!
	glUniformMatrix4fv(shaderProgram->boneStatesID, numBones, GL_FALSE, (GLfloat *)boneStates);
	// Now we can free the bone states.
	memoryManagerGlobalFree(boneStates);

	// Send the new model view projection matrix to the shader!
	glUniformMatrix4fv(shaderProgram->modelViewMatrixID, 1, GL_FALSE, (GLfloat *)&mvpMatrix);


	glActiveTexture(GL_TEXTURE0);

	curRenderable = obj->renderables;
	// Draw each of the renderables.
	while(curRenderable != NULL){
		renderableDraw(curRenderable, shaderProgram);
		curRenderable = memSingleListNext(curRenderable);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDelete(object *obj){
	moduleSkeleAnimStateFreeArray(&obj->anims);
	if(obj->bones != NULL){
		memoryManagerGlobalFree(obj->bones);
	}

	// obj->colliders = NULL;
	modulePhysicsBodyFreeArray(&obj->physBodies);

	moduleRenderableFreeArray(&obj->renderables);
}

/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDefDelete(objectDef *objDef){
	if(objDef->name != NULL){
		memoryManagerGlobalFree(objDef->name);
	}

	if(objDef->anims != NULL){
		memoryManagerGlobalFree(objDef->anims);
	}

	// objDef->colliders = NULL;
	modulePhysicsBodyDefFreeArray(&objDef->physBodies);

	moduleRenderableDefFreeArray(&objDef->renderables);
}