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
	const boneState *const restrict rootState,
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
	transformInit(&obj->state);
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
			transformMultiplyOut(&obj->state, &skeleBone->localBind, &body->state);
		}else{
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///transformMultiplyOut(&skeleBone->localBind, curTransform, &body->state);
			body->state = skeleBone->localBind;
		}
		// Transform the bone using each animation.
		skeleStatePrependAnimations(&body->state, &obj->skeleState, boneID, skeleBone->name);
		// Add the the bone's transformations to its parent's if it has one.
		if(!valueIsInvalid(skeleBone->parent, boneIndex_t)){
			transformMultiplyP2(&obj->skeleState.bones[skeleBone->parent], &body->state);
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
	boneState accumulators[SKELETON_MAX_BONES << 1];
	// Stores the total user transformation for each bone.
	boneState *userAccumulator = &accumulators[0];
	// Stores the total animation state for each bone.
	boneState *animAccumulator = &accumulators[obj->skeleState.skele->numBones];


	*userAccumulator = obj->state;
	*animAccumulator = curSkeleBone->localBind;
	// Transform the bone using each animation.
	skeleStatePrependAnimations(animAccumulator, &obj->skeleState, 0, curSkeleBone->name);

	// We transform the first body outside of the main
	// loop, as it's stored differently to the others.
	if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == 0){
		if(physRigidBodyIsSimulated(curBody)){
			// Apply the accumulated animation state to the accumulated user transformations.
			transformMultiplyOut(userAccumulator, animAccumulator, &curBody->state);
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
		++userAccumulator;
		++animAccumulator;

		// Add the parent's transformations if the bone has a parent.
		if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///transformMultiplyOut(&accumulators[curSkeleBone->parent], curTransform, userAccumulator);
			transformMultiplyOut(&accumulators[curSkeleBone->parent], &g_transformIdentity, userAccumulator);
			// Apply the parent's transformation to the bone's bind pose.
			transformMultiplyOut(&curSkeleBone->localBind, &accumulators[obj->skeleState.skele->numBones + curSkeleBone->parent], animAccumulator);
		}else{
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*userAccumulator = *curTransform;
			transformInit(userAccumulator);
			*animAccumulator = curSkeleBone->localBind;
		}
		// Transform the bone using each animation.
		skeleStatePrependAnimations(animAccumulator, &obj->skeleState, boneID, curSkeleBone->name);

		if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == boneID){
			if(physRigidBodyIsSimulated(curBody)){
				// Apply the accumulated animation state to the accumulated user transformations.
				transformMultiplyOut(userAccumulator, animAccumulator, &curBody->state);
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

	// Generate the object's global bone states.
	updateBones(obj, time);

	model *curMdl = obj->mdls;
	// Animate each of the component models' textures.
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
		debugDrawSkeleton(&obj->skeleState, debugDrawInfoInit(
			GL_LINES, GL_LINE, vec3InitSetC(0.f, 1.f, 0.f), 1.f
		), &cam->viewProjectionMatrix);
	}
	#warning "Rigid body instances aren't storing colliders?"
	#warning "I've forgotten what the above warning was about, although it seems like it's been fixed."
	if(obj->physBodies != NULL){
		if(obj->physBodies->colliders->node != NULL){
			debugDrawColliderAABB(&(obj->physBodies->colliders->node->aabb), debugDrawInfoInit(
				GL_TRIANGLES, GL_LINE, vec3InitSetC(1.f, 0.4f, 0.f), 1.f
			), &cam->viewProjectionMatrix);
		}
		debugDrawColliderHull(&(obj->physBodies->colliders->global.data.hull), debugDrawInfoInit(
			GL_TRIANGLES, GL_LINE, vec3InitSetC(1.f, 0.4f, 0.f), 1.f
		), &cam->viewProjectionMatrix);
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
	mat4 test2 = mat4MultiplyMat4C(rot, test);
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
	const mat4 mvp = mat4MultiplyMat4C(cam->viewProjectionMatrix, test2);
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


#warning "I don't think this procedure is correct!"
/**
#error "Start using affine transforms. It might be a good idea to use matrices for the scale, then convert everything when we're done."
#error "Correct order: P_U U P_A B A."
**/
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
	boneState accumulators[SKELETON_MAX_BONES << 1];
	// Stores the total user transformation for each bone.
	boneState *userAccumulator = &accumulators[0];
	// Stores the total animation state for each bone.
	boneState *animAccumulator = &accumulators[obj->skeleState.skele->numBones];

	boneIndex_t i;


	#if 0
	// We transform the first bone outside of the main
	// loop, as it's stored differently to the others.
	if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == 0 && physRigidBodyIsSimulated(curBody)){
		physRigidBodyUpdatePosition(curBody);
		// Copy the rigid body's state over to the bone.
		obj->state = curBody->state;
		*curObjBone = curBody->state;

		// Update the accumulators.
		*userAccumulator = obj->state;
		transformInit(animAccumulator);

		modulePhysicsBodyNext(curBody);
		++curPhysBoneID;

	// Apply the effects of each animation to the bone.
	}else{
		*userAccumulator = obj->state;
		*animAccumulator = curSkeleBone->localBind;
		// Transform the bone using each animation.
		skeleStatePrependAnimations(animAccumulator, &obj->skeleState, 0, curSkeleBone->name);

		// Apply the accumulated user transformations to the accumulated animation state.
		transformMultiplyOut(userAccumulator, animAccumulator, curObjBone);

		if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == 0){
			// Copy the bone's state over to the rigid body.
			curBody->state = *curObjBone;
			// Update the rigid body's centroid to reflect its new position.
			physRigidBodyCentroidFromPosition(curBody);

			modulePhysicsBodyNext(curBody);
			++curPhysBoneID;
		}
	}
	#else
	*curObjBone = curSkeleBone->localBind;
		/*transform testyThingy = g_transformIdentity;
		static float testyAngle = 0.f;
		testyThingy.scale.y = expf(-2.f*testyAngle)*cos(2.f*3.14159265f*testyAngle)*cos(2.f*3.14159265f*testyAngle) + 1.f/4.f;
		testyAngle += 0.005f;
		transformMultiplyP2(&testyThingy, curObjBone);*/
	// Prepend the total animation transformations to the bind pose.
	skeleStatePrependAnimations(curObjBone, &obj->skeleState, 0, curSkeleBone->name);
	#endif

	// Now apply the effects of each animation one bone at a time!
	for(++curObjBone, i = 1; curObjBone < lastObjBone; ++curObjBone, ++i){
		++curSkeleBone;
		#if 0
		++userAccumulator;
		++animAccumulator;

		if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == i && physRigidBodyIsSimulated(curBody)){
			physRigidBodyUpdatePosition(curBody);
			// Copy the rigid body's state over to the bone.
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*curTransform = curBody->state;
			*curObjBone = curBody->state;//*curTransform;

			// Update the accumulators.
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*userAccumulator = *curTransform;
			transformInit(userAccumulator);
			transformInit(animAccumulator);

			modulePhysicsBodyNext(curBody);
			++curPhysBoneID;
		}else{
			if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
				/** THIS SHOULD USE THE BONETRANSFORMS! **/
				///transformMultiplyOut(&accumulators[curSkeleBone->parent], curTransform, userAccumulator);
				transformMultiplyOut(&accumulators[curSkeleBone->parent], &g_transformIdentity, userAccumulator);
				//transformMultiplyOut(&accumulators[curSkeleBone->parent], &test, userAccumulator);
				// Apply the parent's transformation to the bone's bind pose.
				transformMultiplyOut(&accumulators[obj->skeleState.skele->numBones + curSkeleBone->parent], &curSkeleBone->localBind, animAccumulator);
			}else{
				/** THIS SHOULD USE THE BONETRANSFORMS! **/
				///*userAccumulator = *curTransform;
				transformInit(userAccumulator);
				*animAccumulator = curSkeleBone->localBind;
			}
			// Transform the bone using each animation.
			skeleStatePrependAnimations(animAccumulator, &obj->skeleState, i, curSkeleBone->name);

			// Apply the accumulated user transformations to the accumulated animation state.
			transformMultiplyOut(userAccumulator, animAccumulator, curObjBone);

			if(curPhysBoneID < lastPhysBoneID && *curPhysBoneID == i){
				// Copy the bone's state over to the rigid body.
				curBody->state = *curObjBone;
				// Update the rigid body's centroid to reflect its new position.
				physRigidBodyCentroidFromPosition(curBody);

				modulePhysicsBodyNext(curBody);
				++curPhysBoneID;
			}
		}
		#else
		if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
			// Append the parent's transformation to this bone's local bind pose.
			transformMultiplyOut(&obj->skeleState.bones[curSkeleBone->parent], &curSkeleBone->localBind, curObjBone);
			// Prepend the total animation transformations.
			skeleStatePrependAnimations(curObjBone, &obj->skeleState, i, curSkeleBone->name);
		}
		#endif
	}
}

/*
** During our update step, we generate global bone states for each
** bone in the animation. To handle the propagation of scale, we
** convert the bone states to matrices using the following process:
**     1. Begin with the global bone state.
**     2. Undo the accumulated user transformations.
**     3. Convert the user transformations and new bone state to matrices.
**     4. Multiply them as matrices to preserve effects such as skewing.
**     5. Multiply by the global inverse bind pose to return to local space.
*/
/**#error "Skeletal animation: Try not to use matrices, as they probably take up too much space and are too slow to send to the shader."
#error "That being said, we really only need to send 4x3 matrices, which should save a lot of time."
#error "We propagate scaling correctly for user transformations, but we do it in a strange way with animations."
#error "It would be nice if we could use the precalculated global inverse bind pose here."
///https://www.gamedev.net/forums/topic.asp?topic_id=339800
///Ken Shoemake's Polar Decomposition (Graphics Gems IV)
///https://github.com/erich666/GraphicsGems/tree/master/gemsiv/polar_decomp
///Non-Uniform Bone Scaling From Art Pipeline to Real-Time Rendering
#error "To fix this, we need to use stretch quaternions: see Ken Shoemake's affine decomposition and 'non-uniform.pdf'."
#error "Ideally, we would encode uniform scaling in quaternions, and only do extra work if we have to."
#error "However, I'm not sure if it's easy to propagate this correctly."**/
static void prepareBoneMatrices(
	const skeletonState *const restrict skeleState,
	const boneState *const restrict rootState,
	mat4 *animStates
){

	mat4 *curState = animStates;
	const boneState *curObjBone = skeleState->bones;
	const bone *curSkeleBone = skeleState->skele->bones;
	const boneState *const lastBone = &curObjBone[skeleState->skele->numBones];

	#warning "This takes a significant chunk of the stack. Maybe allocate it on the heap?"
	boneState accumulators[SKELETON_MAX_BONES];
	// Stores the total user transformation for each bone.
	boneState *userAccumulator = &accumulators[0];

	boneState tempBone;
	mat4 tempMatrix;
	#if 0


	*userAccumulator = *rootState;
	// Undo the accumulated user transformations to restore
	// the unaltered global animation state for this bone.
	transformInvertOut(userAccumulator, &tempBone);
	transformMultiplyP1(&tempBone, curObjBone);

	// In order to properly support propagation of scale in animations
	// and user transformations, we need to apply them as matrices.
	transformToMat4(userAccumulator, &tempMatrix);
	transformToMat4(&tempBone, curState);
	mat4MultiplyMat4Out(tempMatrix, *curState, curState);
	transformToMat4(&curSkeleBone->invGlobalBind, &tempMatrix);
	mat4MultiplyMat4Out(*curState, tempMatrix, curState);
	#else
	transformMultiplyOut(curObjBone, &curSkeleBone->invGlobalBind, &tempBone);
	transformToMat4(&tempBone, curState);
	#endif


	#warning "Maybe store the bone's ID as the parent ID if the bone has no parent."
	#warning "This gives us an extra bone, but it means we need to know the bone's ID to check the parent."
	++curObjBone;
	for(; curObjBone < lastBone; ++curObjBone){
		++curState;
		++curSkeleBone;
		++userAccumulator;
		#if 0

		if(!valueIsInvalid(curSkeleBone->parent, boneIndex_t)){
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///transformMultiplyOut(&accumulators[curSkeleBone->parent], curTransform, userAccumulator);
			transformMultiplyOut(&accumulators[curSkeleBone->parent], &g_transformIdentity, userAccumulator);
		}else{
			/** THIS SHOULD USE THE BONETRANSFORMS! **/
			///*userAccumulator = *curTransform;
			transformInit(userAccumulator);
		}
		transformInvertOut(userAccumulator, &tempBone);
		transformMultiplyP1(&tempBone, curObjBone);

		transformToMat4(userAccumulator, &tempMatrix);
		transformToMat4(&tempBone, curState);
		mat4MultiplyMat4Out(tempMatrix, *curState, curState);
		transformToMat4(&curSkeleBone->invGlobalBind, &tempMatrix);
		mat4MultiplyMat4Out(*curState, tempMatrix, curState);
		#else
		transformMultiplyOut(curObjBone, &curSkeleBone->invGlobalBind, &tempBone);
		transformToMat4(&tempBone, curState);
		#endif
	}
}