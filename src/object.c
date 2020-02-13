#include "object.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "moduleRenderable.h"
#include "moduleSkeleton.h"
#include "modulePhysics.h"


// Forward-declare any helper functions!
static void updateBones(object *obj, const float time);


void objectDefInit(objectDef *objDef){
	objDef->name = NULL;

	objDef->skele = &g_skeleDefault;

	objDef->colliders = NULL;
	objDef->physBodies = NULL;

	objDef->renderables = NULL;
}

void objectInit(object *const restrict obj, const objectDef *const restrict objDef){
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


return_t objectDefLoad(objectDef *const restrict objDef, const char *const restrict objFile){
	return(0);
}


void objectUpdate(object *const restrict obj, const float time){
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
#include "billboard.h"
void objectDraw(
	const object *const restrict obj, const camera *const restrict cam,
	const shaderObject *const restrict shader, const float time
){

	const renderable *curRenderable;

	#warning "Could we store these in the skeleton object and allocate them in the same call as the bone states?"
	#warning "We could possibly use a global bone states array."
	mat4 animStates[SKELETON_MAX_BONES];
	mat4 *curState = animStates;
	const boneState *curBone = obj->skeleData.bones;
	const boneState *const lastBone = &curBone[obj->skeleData.skele->numBones];

	// Convert every bone transformation to a matrix!
	do {
		transformStateToMat4(curBone, curState);
		++curState;
		++curBone;
	} while(curBone != lastBone);


	// Send the new model-view-projection matrix to the shader!
	#warning "Maybe do this outside since it applies to all objects?"
	/*const mat4 test = mat4RotateRadR(mat4InitTranslateR(2.f, 1.f, -1.f), 2.f, 1.f, -1.f);
	const vec3 axis = {.x = 0.f, .y = 1.f, .z = 0.f};
	const billboard bData = {
		.axis = &axis,
		.target = &cam->pos,
		.scale = 1.f,
		.flags = BILLBOARD_LOCK_XYZ
	};
	const mat4 rot = {
		cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0], 0.f,
		cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1], 0.f,
		cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2], 0.f,
		                    0.f,                     0.f,                     0.f, 1.f
	};
	mat4 test2 = mat4MultiplyByMat4R(rot, test);
	printf("Pre:\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n",
	test.m[0][0], test.m[0][1], test.m[0][2], test.m[0][3],
	test.m[1][0], test.m[1][1], test.m[1][2], test.m[1][3],
	test.m[2][0], test.m[2][1], test.m[2][2], test.m[2][3],
	test.m[3][0], test.m[3][1], test.m[3][2], test.m[3][3]);
	billboardState(&bData, cam, vec3InitSetR(2.f, 1.f, -1.f), test, &test2);
	printf("Post:\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n\n",
	test2.m[0][0], test2.m[0][1], test2.m[0][2], test2.m[0][3],
	test2.m[1][0], test2.m[1][1], test2.m[1][2], test2.m[1][3],
	test2.m[2][0], test2.m[2][1], test2.m[2][2], test2.m[2][3],
	test2.m[3][0], test2.m[3][1], test2.m[3][2], test2.m[3][3]);
	const mat4 mvp = mat4MultiplyByMat4R(cam->viewProjectionMatrix, test2);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&mvp);*/
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&cam->viewProjectionMatrix);


	curRenderable = obj->renderables;
	// Draw each of the renderables.
	while(curRenderable != NULL){
		renderableDraw(curRenderable, obj->skeleData.skele, animStates, shader);
		curRenderable = memSingleListNext(curRenderable);
	}
}


/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDelete(object *const restrict obj){
	skeleObjDelete(&obj->skeleData);

	// obj->colliders = NULL;
	modulePhysicsBodyFreeArray(&obj->physBodies);

	moduleRenderableFreeArray(&obj->renderables);
}

/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDefDelete(objectDef *const restrict objDef){
	if(objDef->name != NULL){
		memoryManagerGlobalFree(objDef->name);
	}

	// objDef->colliders = NULL;
	modulePhysicsBodyDefFreeArray(&objDef->physBodies);

	moduleRenderableDefFreeArray(&objDef->renderables);
}


// Update all of an object's bones!
static void updateBones(object *const restrict obj, const float time){
	boneState *curObjBone = obj->skeleData.bones;
	const boneState *const lastObjBone = &curObjBone[obj->skeleData.skele->numBones];
	const bone *curSkeleBone = obj->skeleData.skele->bones;
	size_t i = 0;
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