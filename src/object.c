#include "object.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "moduleRenderable.h"
#include "moduleSkeleton.h"
#include "modulePhysics.h"


static void updateBones(object *obj, const float time);


void objectDefInit(objectDef *objDef){
	objDef->name = NULL;

	objDef->skele = &skeleDefault;

	objDef->colliders = NULL;
	objDef->physBodies = NULL;

	objDef->renderables = NULL;
}

void objectInit(object *obj, const objectDef *objDef){
	const physicsRigidBodyDef *curBody = objDef->physBodies;
	const renderableDef *curRenderable = objDef->renderables;

	obj->objDef = objDef;

	skeleObjInit(&obj->skeleData, objDef->skele);

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


void objectUpdate(object *obj, const float time){
	skeletonAnim *curAnim = obj->skeleData.anims;
	// Update which frame each animation is currently on!
	while(curAnim != NULL){
		skeleAnimUpdate(curAnim, time);
		curAnim = memSingleListNext(curAnim);
	}

	updateBones(obj, time);

	renderable *curRenderable = obj->renderables;
	// Animate each of the renderables.
	while(curRenderable != NULL){
		renderableUpdate(curRenderable, time);
		curRenderable = memSingleListNext(curRenderable);
	}
}

#warning "A lot of this stuff should be moved outside, especially the OpenGL code and skeleton stuff."
void objectDraw(const object *obj, const vec3 *camPos, mat4 mvpMatrix, const shader *shaderPrg, const float time){
	const renderable *curRenderable;
	mat4 *animStates;
	mat4 *curState;
	const boneState *curBone;
	const boneState *lastBone;


	// Send the new model view projection matrix to the shader!
	glUniformMatrix4fv(shaderPrg->mvpMatrixID, 1, GL_FALSE, (GLfloat *)&mvpMatrix);

	#warning "Could we store these in the skeleton object and allocate them in the same call as the bone states?"
	animStates = memoryManagerGlobalAlloc(sizeof(*animStates) * obj->skeleData.skele->numBones);
	if(animStates == NULL){
		/** MALLOC FAILED **/
	}

	curState = animStates;
	curBone = obj->skeleData.bones;
	lastBone = &curBone[obj->skeleData.skele->numBones];
	// Convert every bone transformation to a matrix!
	do {
		transformStateToMat4(curBone, curState);
		++curState;
		++curBone;
	} while(curBone != lastBone);


	glActiveTexture(GL_TEXTURE0);

	curRenderable = obj->renderables;
	// Draw each of the renderables.
	while(curRenderable != NULL){
		renderableDraw(curRenderable, obj->skeleData.skele, animStates, shaderPrg);
		curRenderable = memSingleListNext(curRenderable);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);


	// Now we can free the bone states.
	memoryManagerGlobalFree(animStates);
}


/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDelete(object *obj){
	skeleObjDelete(&obj->skeleData);

	// obj->colliders = NULL;
	modulePhysicsBodyFreeArray(&obj->physBodies);

	moduleRenderableFreeArray(&obj->renderables);
}

/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDefDelete(objectDef *objDef){
	if(objDef->name != NULL){
		memoryManagerGlobalFree(objDef->name);
	}

	// objDef->colliders = NULL;
	modulePhysicsBodyDefFreeArray(&objDef->physBodies);

	moduleRenderableDefFreeArray(&objDef->renderables);
}


// Update all of an object's bones!
static void updateBones(object *obj, const float time){
	boneState *curObjBone;
	const boneState *lastObjBone;
	const bone *curSkeleBone;
	size_t i;

	curObjBone = obj->skeleData.bones;
	lastObjBone = &curObjBone[obj->skeleData.skele->numBones];
	curSkeleBone = obj->skeleData.skele->bones;
	i = 0;
	// Apply the effects of each animation one bone at a time!
	for(; curObjBone < lastObjBone; ++curObjBone, ++curSkeleBone, ++i){
		const size_t parentID = curSkeleBone->parent;

		// Apply the current skeleton's local bind pose.
		*curObjBone = curSkeleBone->localBind;

		// Transform the bone using each animation.
		skeleObjGenerateBoneState(&obj->skeleData, i, curSkeleBone->name);

		// If this bone has a parent, add its animation
		// transformations to those of its parent.
		if(!valueIsInvalid(parentID)){
			transformStateAppend(&obj->skeleData.bones[parentID], curObjBone, curObjBone);
		}
	}

	curObjBone = obj->skeleData.bones;
	curSkeleBone = obj->skeleData.skele->bones;
	#warning "We may need to move this out eventually."
	// Append each bone's inverse bind pose!
	for(; curObjBone < lastObjBone; ++curObjBone, ++curSkeleBone){
		transformStateAppend(curObjBone, &curSkeleBone->invGlobalBind, curObjBone);
	}
}