#include "object.h"


#define GLEW_STATIC
#include <GL/glew.h>

#include "memoryManager.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "modulePhysics.h"

#include "mat4.h"

/** TEMPORARY DEBUG DRAW STUFF **/
#include "debugDraw.h"


// Forward-declare any helper functions!
static void updateBones(object *obj, const float time);
static void prepareBoneMatrices(
	const skeletonState *const restrict skeleState,
	const transformState *const restrict rootState,
	mat4 *animStates
);


void objectDefInit(objectDef *objDef){
	objDef->name = NULL;

	objDef->skele = &g_skeleDefault;

	objDef->colliders = NULL;
	objDef->physBodies = NULL;
	objDef->physBoneIDs = NULL;
	objDef->numBodies = 0;

	objDef->mdlDefs = NULL;
	objDef->numModels = 0;
}

#warning "Can we allocate all of our memory in one go?"
void objectInit(object *const restrict obj, const objectDef *const restrict objDef){
	physicsRigidBody *curBody = NULL;
	const physicsRigidBodyDef *curBodyDef = objDef->physBodies;
	const boneIndex_t *curPhysBoneID = objDef->physBoneIDs;
	const modelDef **curMdlDef = objDef->mdlDefs;


	obj->objDef = objDef;

	skeleStateInit(&obj->skeleState, objDef->skele);
	transformStateInit(&obj->state);
	obj->boneTransforms = NULL;

	obj->colliders = NULL;
	obj->physBodies = NULL;
	// Instantiate the object's physics rigid bodies.
	while(curBodyDef != NULL){
		curBody = modulePhysicsBodyInsertAfter(&obj->physBodies, curBody);
		if(curBody == NULL){
			/** MALLOC FAILED **/
		}
		physRigidBodyInit(curBody, curBodyDef);
		objectPrepareRigidBody(obj, curBody, *curPhysBoneID);

		curBodyDef = modulePhysicsBodyDefNext(curBodyDef);
		++curPhysBoneID;
	}

	obj->mdls = NULL;
	if(curMdlDef != NULL){
		model *curMdl = NULL;
		const modelDef **const lastMdlDef = &curMdlDef[objDef->numModels];
		// Instantiate the object's default models.
		do {
			curMdl = moduleModelInsertAfter(&obj->mdls, curMdl);
			if(curMdl == NULL){
				/** MALLOC FAILED **/
			}
			modelInit(curMdl, *curMdlDef);

			++curMdlDef;
		} while(curMdlDef < lastMdlDef);
	}
}


return_t objectDefLoad(objectDef *const restrict objDef, const char *const restrict objFile){
	return(0);
}


/*
** When we add a rigid body to an object, we need to transform it by the bone its attached to.
** We only do this once, as afterwards it will be physically simulated so we shouldn't interfere.
*/
void objectPrepareRigidBody(object *const restrict obj, physicsRigidBody *const restrict body, const boneIndex_t boneID){
	if(physRigidBodyIsSimulated(body)){
		const bone *const skeleBone = &obj->skeleState.skele->bones[boneID];

		// Apply the current skeleton's local bind pose and any user transformations.
		if(boneID == 0){
			transformStateAppend(&skeleBone->localBind, &obj->state, &body->state);
		}else{
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///transformStateAppend(&skeleBone->localBind, curTransform, &body->state);
			body->state = skeleBone->localBind;
		}
		// Transform the bone using each animation.
		skeleStateGenerateBoneState(&obj->skeleState, boneID, skeleBone->name, &body->state);
		// Add the the bone's transformations to its parent's if it has one.
		if(!valueIsInvalid(skeleBone->parent, boneIndex_t)){
			transformStateAppend(&body->state, &obj->skeleState.bones[skeleBone->parent], &body->state);
		}
	}
	// Add the rigid body's colliders to the island.
	if(physRigidBodyIsCollidable(body)){
		body->flags |= PHYSRIGIDBODY_TRANSFORMED;
	}
}

/*
** Prepare all of the physics objects at once. This is used when we first
** create an object to make the rigid bodies appear at their bones.
*/
void objectPreparePhysics(object *const restrict obj){
	const bone *curSkeleBone = obj->skeleState.skele->bones;
	physicsRigidBody *curBody = obj->physBodies;
	// We store the rigid bodies in order of increasing bone IDs.
	// We can simply move to the next ID when we find a bone with a rigid body.
	const boneIndex_t *curPhysBoneID = obj->objDef->physBoneIDs;
	const boneIndex_t *const lastPhysBoneID = &obj->objDef->physBoneIDs[obj->objDef->numBodies];

	boneIndex_t boneID;
	const boneIndex_t lastID = obj->skeleState.skele->numBones;

	#warning "This takes a significant chunk of the stack. Maybe allocate it on the heap?"
	transformState accumulators[SKELETON_MAX_BONES << 1];
	// Stores the total user transformation for each bone.
	transformState *stateAccumulator = &accumulators[0];
	// Stores the total animation state for each bone.
	transformState *animAccumulator = &accumulators[obj->skeleState.skele->numBones];


	*stateAccumulator = obj->state;
	*animAccumulator = curSkeleBone->localBind;
	// Transform the bone using each animation.
	skeleStateGenerateBoneState(&obj->skeleState, 0, curSkeleBone->name, animAccumulator);

	// We transform the first body outside of the main
	// loop, as it's stored differently to the others.
	if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == 0){
		if(physRigidBodyIsSimulated(curBody)){
			// Apply the accumulated animation state to the accumulated user transformations.
			transformStateAppend(animAccumulator, stateAccumulator, &curBody->state);
			// Make sure the rigid body actually stays where we've placed it.
			physRigidBodyCentroidFromPosition(curBody);

		}
		// Add the rigid body's colliders to the island.
		if(physRigidBodyIsCollidable(curBody)){
			curBody->flags |= PHYSRIGIDBODY_TRANSFORMED;
		}

		modulePhysicsBodyNext(curBody);
		++curPhysBoneID;
	}

	// Move the rest of the physics objects to their bones.
	for(boneID = 1; boneID < lastID; ++curPhysBoneID, ++boneID){
		++curSkeleBone;
		++stateAccumulator;
		++animAccumulator;

		// Add the parent's transformations if the bone has a parent.
		if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///transformStateAppend(curTransform, &accumulators[curSkeleBone->parent], stateAccumulator);
			transformStateAppend(&g_transformIdentity, &accumulators[curSkeleBone->parent], stateAccumulator);
			// Apply the parent's transformation to the bone's bind pose.
			transformStateAppend(&curSkeleBone->localBind, &accumulators[obj->skeleState.skele->numBones + curSkeleBone->parent], animAccumulator);
		}else{
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*stateAccumulator = curTransform;
			transformStateInit(stateAccumulator);
			*animAccumulator = curSkeleBone->localBind;
		}
		// Transform the bone using each animation.
		skeleStateGenerateBoneState(&obj->skeleState, boneID, curSkeleBone->name, animAccumulator);

		if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == boneID){
			if(physRigidBodyIsSimulated(curBody)){
				// Apply the accumulated animation state to the accumulated user transformations.
				transformStateAppend(animAccumulator, stateAccumulator, &curBody->state);
				// Make sure the rigid body actually stays where we've placed it.
				physRigidBodyCentroidFromPosition(curBody);

			}
			// Add the rigid body's colliders to the island.
			if(physRigidBodyIsCollidable(curBody)){
				curBody->flags |= PHYSRIGIDBODY_TRANSFORMED;
			}

			modulePhysicsBodyNext(curBody);
			++curPhysBoneID;
		}
	}
}


void objectUpdate(object *const restrict obj, const float time){
	skeletonAnim *curAnim = obj->skeleState.anims;
	// Update which frame each animation is currently on!
	while(curAnim != NULL){
		skeleAnimUpdate(curAnim, time);
		curAnim = moduleSkeletonAnimNext(curAnim);
	}

	updateBones(obj, time);

	model *curMdl = obj->mdls;
	// Animate each of the models.
	while(curMdl != NULL){
		modelUpdate(curMdl, time);
		curMdl = moduleModelNext(curMdl);
	}
}

#warning "A lot of this stuff should be moved outside, especially the OpenGL code and skeleton stuff."
#include "billboard.h"
void objectDraw(
	const object *const restrict obj, const camera *const restrict cam,
	const meshShader *const restrict shader, const float time
){

	const model *curMdl;

	#warning "Could we store these in the skeleton object and allocate them in the same call as the bone states?"
	#warning "We could possibly use a global bone states array."
	#warning "Probably not a good idea if we want to create a render queue sometime in the future."
	#warning "It's also a bad idea to be allocating large arrays on the stack."
	mat4 animStates[SKELETON_MAX_BONES];
	// Convert the object's bone states into
	// a matrix representation for the shader.
	prepareBoneMatrices(&obj->skeleState, &obj->state, animStates);


	/** TEMPORARY DEBUG DRAW TEST **/
	if(obj->skeleState.skele->numBones > 1){
		debugDrawSkeleton(&obj->skeleState, debugDrawInfoInit(GL_LINE, vec3InitSetC(0.f, 1.f, 0.f), 1.f), &cam->viewProjectionMatrix);
	}
	#warning "Rigid body instances aren't storing colliders?"
	#warning "I've forgotten what the above warning was about, although it seems like it's been fixed."
	if(obj->physBodies != NULL){
		if(obj->physBodies->colliders->node != NULL){
			debugDrawColliderAABB(&(obj->physBodies->colliders->node->aabb), debugDrawInfoInit(GL_LINE, vec3InitSetC(1.f, 0.4f, 0.f), 1.f), &cam->viewProjectionMatrix);
		}
		debugDrawColliderHull(&(obj->physBodies->colliders->global.data.hull), debugDrawInfoInit(GL_LINE, vec3InitSetC(1.f, 0.4f, 0.f), 1.f), &cam->viewProjectionMatrix);
	}


	// Send the new model view projection matrix to the shader!
	#warning "Maybe do this outside since it applies to all objects?"
	/*const mat4 test = mat4RotateC(mat4InitTranslateC(2.f, 1.f, -1.f), 2.f, 1.f, -1.f);
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
	mat4 test2 = mat4MultiplyMat4ByC(rot, test);
	printf("Pre:\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n",
	test.m[0][0], test.m[0][1], test.m[0][2], test.m[0][3],
	test.m[1][0], test.m[1][1], test.m[1][2], test.m[1][3],
	test.m[2][0], test.m[2][1], test.m[2][2], test.m[2][3],
	test.m[3][0], test.m[3][1], test.m[3][2], test.m[3][3]);
	billboardState(&bData, cam, vec3InitSetC(2.f, 1.f, -1.f), test, &test2);
	printf("Post:\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n[%f, %f, %f, %f]\n\n",
	test2.m[0][0], test2.m[0][1], test2.m[0][2], test2.m[0][3],
	test2.m[1][0], test2.m[1][1], test2.m[1][2], test2.m[1][3],
	test2.m[2][0], test2.m[2][1], test2.m[2][2], test2.m[2][3],
	test2.m[3][0], test2.m[3][1], test2.m[3][2], test2.m[3][3]);
	const mat4 mvp = mat4MultiplyMat4ByC(cam->viewProjectionMatrix, test2);
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&mvp);*/
	glUniformMatrix4fv(shader->vpMatrixID, 1, GL_FALSE, (GLfloat *)&cam->viewProjectionMatrix);


	curMdl = obj->mdls;
	// Draw each of the models.
	while(curMdl != NULL){
		modelDraw(curMdl, obj->skeleState.skele, animStates, shader);
		curMdl = moduleModelNext(curMdl);
	}
}


/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDelete(object *const restrict obj){
	skeleStateDelete(&obj->skeleState);
	if(obj->boneTransforms != NULL){
		memoryManagerGlobalFree(&obj->boneTransforms);
	}

	//obj->colliders = NULL;
	// We have to free each rigid body ourselves rather than
	// use the array free function, as otherwise it will
	// cause problems for rigid bodies owned by islands.
	if(obj->physBodies != NULL){
		physicsRigidBody *body = obj->physBodies;
		size_t numBodies = obj->objDef->numBodies;
		do {
			modulePhysicsBodyFree(&obj->physBodies, body);
			--numBodies;
		} while(numBodies > 0);
	}

	if(obj->mdls != NULL){
		moduleModelFreeArray(&obj->mdls);
	}
}

/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDefDelete(objectDef *const restrict objDef){
	if(objDef->name != NULL){
		memoryManagerGlobalFree(objDef->name);
	}

	//objDef->colliders = NULL;
	modulePhysicsBodyDefFreeArray(&objDef->physBodies);
	if(objDef->physBoneIDs != NULL){
		memoryManagerGlobalFree(objDef->physBoneIDs);
	}

	if(objDef->mdlDefs != NULL){
		memoryManagerGlobalFree(objDef->mdlDefs);
	}
}


/*
** Update an object's skeleton using the following procedure for each bone:
**     1. Begin with the bone's parent's user transformation.
**     2. Append the bone's user transformation.
**     3. Append the bone's parent's animation transformation.
**     4. Append the bone's bind pose.
**     5. Append the bone's animation transformation.
** Note that this process implicitly assumes that parent bones are animated before children.
** Also, we handle user and animation transformations separately, then merge them at the end.
*/
static void updateBones(object *const restrict obj, const float time){
	boneState *curObjBone = obj->skeleState.bones;
	const boneState *const lastObjBone = &curObjBone[obj->skeleState.skele->numBones];
	const bone *curSkeleBone = obj->skeleState.skele->bones;
	physicsRigidBody *curBody = obj->physBodies;
	// We store the rigid bodies in order of increasing bone IDs.
	// We can simply move to the next ID when we find a bone with a rigid body.
	const boneIndex_t *curPhysBoneID = obj->objDef->physBoneIDs;
	const boneIndex_t *const lastPhysBoneID = &obj->objDef->physBoneIDs[obj->objDef->numBodies];

	#warning "This takes a significant chunk of the stack. Maybe allocate it on the heap?"
	transformState accumulators[SKELETON_MAX_BONES << 1];
	// Stores the total user transformation for each bone.
	transformState *stateAccumulator = &accumulators[0];
	// Stores the total animation state for each bone.
	transformState *animAccumulator = &accumulators[obj->skeleState.skele->numBones];

	boneIndex_t i;


	// We transform the first bone outside of the main
	// loop, as it's stored differently to the others.
	if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == 0 && physRigidBodyIsSimulated(curBody)){
		physRigidBodyUpdatePosition(curBody);
		// Copy the rigid body's state over to the bone.
		obj->state = curBody->state;
		*curObjBone = curBody->state;

		// Update the accumulators.
		*stateAccumulator = obj->state;
		transformStateInit(animAccumulator);

		modulePhysicsBodyNext(curBody);
		++curPhysBoneID;

	// Apply the effects of each animation to the bone.
	}else{
		*stateAccumulator = obj->state;
		*animAccumulator = curSkeleBone->localBind;
		// Transform the bone using each animation.
		skeleStateGenerateBoneState(&obj->skeleState, 0, curSkeleBone->name, animAccumulator);

		// Apply the accumulated animation state to the accumulated user transformations.
		transformStateAppend(animAccumulator, stateAccumulator, curObjBone);

		if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == 0){
			// Copy the bone's state over to the rigid body.
			curBody->state = *curObjBone;
			// Update the rigid body's centroid to reflect its new position.
			physRigidBodyCentroidFromPosition(curBody);

			modulePhysicsBodyNext(curBody);
			++curPhysBoneID;
		}
	}

	// Now apply the effects of each animation one bone at a time!
	for(++curObjBone, i = 1; curObjBone < lastObjBone; ++curObjBone, ++i){
		++curSkeleBone;
		++stateAccumulator;
		++animAccumulator;

		if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == i && physRigidBodyIsSimulated(curBody)){
			physRigidBodyUpdatePosition(curBody);
			// Copy the rigid body's state over to the bone.
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*curTransform = curBody->state;
			*curObjBone = curBody->state;

			// Update the accumulators.
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*stateAccumulator = curTransform;
			transformStateInit(stateAccumulator);
			transformStateInit(animAccumulator);

			modulePhysicsBodyNext(curBody);
			++curPhysBoneID;
		}else{
			if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
				/** THIS SHOULD USE THE BONETRANSFORMS! **/
				///transformStateAppend(curTransform, &accumulators[curSkeleBone->parent], stateAccumulator);
				transformStateAppend(&g_transformIdentity, &accumulators[curSkeleBone->parent], stateAccumulator);
				// Apply the parent's transformation to the bone's bind pose.
				transformStateAppend(&curSkeleBone->localBind, &accumulators[obj->skeleState.skele->numBones + curSkeleBone->parent], animAccumulator);
			}else{
				/** THIS SHOULD USE THE BONETRANSFORMS! **/
				///*stateAccumulator = curTransform;
				transformStateInit(stateAccumulator);
				*animAccumulator = curSkeleBone->localBind;
			}
			// Transform the bone using each animation.
			skeleStateGenerateBoneState(&obj->skeleState, i, curSkeleBone->name, animAccumulator);

			// Apply the accumulated animation state to the accumulated user transformations.
			transformStateAppend(animAccumulator, stateAccumulator, curObjBone);

			if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == i){
				// Copy the bone's state over to the rigid body.
				curBody->state = *curObjBone;
				// Update the rigid body's centroid to reflect its new position.
				physRigidBodyCentroidFromPosition(curBody);

				modulePhysicsBodyNext(curBody);
				++curPhysBoneID;
			}
		}
	}
}

/*
** Move each bone's state from global to local space and
** convert it to a matrix representation for the shader.
*/
static void prepareBoneMatrices(
	const skeletonState *const restrict skeleState,
	const transformState *const restrict rootState,
	mat4 *animStates
){

	mat4 *curState = animStates;
	const boneState *curObjBone = skeleState->bones;
	const bone *curSkeleBone = skeleState->skele->bones;
	const boneState *const lastBone = &curObjBone[skeleState->skele->numBones];

	#warning "This takes a significant chunk of the stack. Maybe allocate it on the heap?"
	transformState accumulators[SKELETON_MAX_BONES];
	// Stores the total user transformation for each bone.
	transformState *stateAccumulator = &accumulators[0];

	boneState localBone;
	mat4 curTransformMatrix;


	// Transform the root bone separately as we store it differently.
	*stateAccumulator = *rootState;
	transformStateUndoPrepend(stateAccumulator, curObjBone, &localBone);
	// Move the current bone into local space by appending its inverse bind pose.
	transformStateAppend(&curSkeleBone->invGlobalBind, &localBone, &localBone);

	// Convert the local bone to a matrix representation.
	transformStateToMat4(&localBone, curState);
	transformStateToMat4(stateAccumulator, &curTransformMatrix);
	mat4MultiplyMat4ByOut(curTransformMatrix, *curState, curState);


	++curObjBone;
	for(; curObjBone < lastBone; ++curObjBone){
		++curState;
		++curSkeleBone;
		++stateAccumulator;

		if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///transformStateAppend(curTransform, &accumulators[curSkeleBone->parent], stateAccumulator);
			transformStateAppend(&g_transformIdentity, &accumulators[curSkeleBone->parent], stateAccumulator);
		}else{
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*stateAccumulator = curTransform;
			transformStateInit(stateAccumulator);
		}
		transformStateUndoPrepend(stateAccumulator, curObjBone, &localBone);
		// Move the current bone into local space by appending its inverse bind pose.
		transformStateAppend(&curSkeleBone->invGlobalBind, &localBone, &localBone);

		// Convert the local bone to a matrix representation.
		transformStateToMat4(&localBone, curState);
		transformStateToMat4(stateAccumulator, &curTransformMatrix);
		mat4MultiplyMat4ByOut(curTransformMatrix, *curState, curState);
	}
}