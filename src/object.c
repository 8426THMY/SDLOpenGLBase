#include "object.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "moduleRenderable.h"
#include "moduleSkeleton.h"
#include "modulePhysics.h"


void objectDefInit(objectDef *objDef){
	objDef->name = NULL;

	objDef->skele = NULL;
	objDef->animDefs = NULL;
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
	skeletonAnim *curAnim = obj->anims;
	if(curAnim != NULL){
		const bone *curSkeleBone = obj->objDef->skele->bones;
		const boneState *curAnimBone = obj->anims->skeleState;

		// Set each of the object's bones to the default states!
		boneState *curObjBone = obj->bones;
		const boneState *lastObjBone = &curObjBone[obj->objDef->skele->numBones];
		for(; curObjBone < lastObjBone; ++curObjBone, ++curSkeleBone){
			//transformStateInit(curObjBone);
			*curObjBone = curSkeleBone->localBind;
		}

		/**
		How does blending work?

		For each animation {
			Interpolate between previous frame and current frame -> frameState;
			Interpolate between bind pose and animState based on intensity -> animState;
			Append animState to objState;
		}
		**/
		// Update each of the object's animations!
		while(curAnim != NULL){
			skeleAnimUpdate(curAnim, obj->objDef->skele, time, obj->bones);
			curAnim = memSingleListNext(curAnim);
		}

		#warning "Temporary skeletal animation stuff."
		curObjBone = obj->bones;
		curSkeleBone = obj->objDef->skele->bones;
		// Accumulate the transformations for each bone in the animation!
		for(; curObjBone < lastObjBone; ++curObjBone, ++curSkeleBone, ++curAnimBone){
			const size_t parentID = curSkeleBone->parent;
			// If this bone has a parent, add its animation transformations to those of its parent!
			if(!VALUE_IS_INVALID(parentID)){
				transformStateAppend(&obj->bones[parentID], curObjBone, curObjBone);
			}
		}

		curSkeleBone = obj->objDef->skele->bones;
		curObjBone = obj->bones;
		// Append each bone's inverse reference state!
		for(; curObjBone < lastObjBone; ++curObjBone, ++curSkeleBone){
			transformStateAppend(curObjBone, &curSkeleBone->invGlobalBind, curObjBone);
		}
	}


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
	moduleSkeleAnimFreeArray(&obj->anims);
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

	if(objDef->animDefs != NULL){
		memoryManagerGlobalFree(objDef->animDefs);
	}

	// objDef->colliders = NULL;
	modulePhysicsBodyDefFreeArray(&objDef->physBodies);

	moduleRenderableDefFreeArray(&objDef->renderables);
}