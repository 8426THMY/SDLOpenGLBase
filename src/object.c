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
static void updateBones(object *obj);
static void prepareBoneMatrices(const skeletonState *const restrict skeleState, mat3x4 *animStates);


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

void objectInit(object *const restrict obj, const objectDef *const restrict objDef){
	obj->objDef = objDef;

	obj->boneTransforms = NULL;
	objectSetSkeleton(obj, objDef->skele);
	transformInit(&obj->state);

	obj->colliders = NULL;

	if(objDef->numBodies == 0){
		obj->physBodies = NULL;
	}else{
		const physicsRigidBodyDef *curBodyDef = objDef->physBodies;
		const boneIndex *curPhysBoneID = objDef->physBoneIDs;

		// Instantiate the object's physics rigid bodies.
		do {
			objectAddRigidBody(obj, curBodyDef, *curPhysBoneID);
			curBodyDef = modulePhysicsRigidBodyDefNext(curBodyDef);
			++curPhysBoneID;
		} while(curBodyDef != NULL);

		// Copy the default physics bone IDs.
		obj->physBoneIDs = memoryManagerGlobalAlloc(objDef->numBodies * sizeof(*obj->physBoneIDs));
		memcpy(obj->physBoneIDs, objDef->physBoneIDs, objDef->numBodies * sizeof(*obj->physBoneIDs));
	}
	obj->numBodies = objDef->numBodies;

	if(objDef->numModels == 0){
		obj->mdls = NULL;
	}else{
		model *curMdl = NULL;
		const modelDef **curMdlDef = objDef->mdlDefs;
		const modelDef **const lastMdlDef = &curMdlDef[objDef->numModels];

		// Instantiate the object's default models.
		do {
			curMdl = moduleModelInsertAfter(&obj->mdls, curMdl);
			if(curMdl == NULL){
				/** MALLOC FAILED **/
			}
			modelInit(curMdl, *curMdlDef);

			++curMdlDef;
		} while(curMdlDef != lastMdlDef);
	}
}


return_t objectDefLoad(objectDef *const restrict objDef, const char *const restrict objFile){
	return(0);
}


// Set the object's skeleton to "skele".
void objectSetSkeleton(object *const restrict obj, const skeleton *const restrict skele){
	obj->boneTransforms = memoryManagerGlobalRealloc(obj->boneTransforms, skele->numBones * sizeof(*obj->boneTransforms));
	// Initialize all of the user transformations to the default state.
	{
		boneState *curTransform = obj->boneTransforms;
		const boneState *const lastTransform = &obj->boneTransforms[skele->numBones];
		while(curTransform != lastTransform){
			transformInit(curTransform);
			++curTransform;
		}
	}

	skeleStateInit(&obj->skeleState, skele);
}

/*
** When we add a rigid body to an object, we need to transform it by the bone its attached to.
** We only do this once, as afterwards it will be physically simulated so we shouldn't interfere.
*/
void objectAddRigidBody(
	object *const restrict obj,
	const physicsRigidBodyDef *const restrict bodyDef,
	const boneIndex boneID
){

	// Add the new rigid body to the beginning of the object's list.
	// Rigid bodies are stored in reverse order to the object definition!
	physicsRigidBody *const body = modulePhysicsRigidBodyPrepend(&obj->physBodies);
	if(body == NULL){
		/** MALLOC FAILED **/
	}
	physRigidBodyInit(body, bodyDef);

	/** This doesn't work, as we may not have animated the parent bone.  **/
	/** Should we animate the entire skeleton when initializing objects? **/
	#if 0
	if(physRigidBodyIsSimulated(body)){
		const bone *const skeleBone = &obj->skeleState.skele->bones[boneID];

		// Prepend any user transformations to the bone's local bind state.
		// S = B*U
		transformMultiplyOut(&skeleBone->localBind, &obj->boneTransforms[boneID], &body->state);
		// If the bone has a parent, append its transformation.
		// S = P*B*U
		if(!valueIsInvalid(skeleBone->parent, boneIndex)){
			transformMultiplyP2(&obj->skeleState.bones[skeleBone->parent], &body->state);
		}
		// Prepend the total animation transformations.
		// S = P*B*U*A
		skeleStatePrependAnimations(&body->state, &obj->skeleState, boneID, skeleBone->name);
	}
	#endif
	// Make sure the rigid body's colliders are added
	// to their island on the next physics update.
	if(physRigidBodyIsCollidable(body)){
		body->flags |= PHYSRIGIDBODY_TRANSFORMED;
	}
}

/*
** Prepare all of the physics objects at once. This is used when we first
** create an object to make the rigid bodies appear at their bones.
*/
void objectPreparePhysics(object *const restrict obj){
	boneState *curTransform = obj->boneTransforms;
	const bone *curSkeleBone = obj->skeleState.skele->bones;

	physicsRigidBody *curBody = obj->physBodies;
	boneIndex curBoneID = 0;
	const boneIndex numBones = obj->skeleState.skele->numBones;
	// We store the rigid bodies in order of increasing bone IDs.
	// We can simply move to the next ID when we find a bone with a rigid body.
	const boneIndex *curPhysBoneID = obj->physBoneIDs;
	const boneIndex *const lastPhysBoneID = &obj->physBoneIDs[obj->numBodies];

	// To prepare the physics objects, we'll need to loop through
	// each bone in the skeleton, constructing its initial state.
	boneState *const accumulators = memoryManagerGlobalAlloc(numBones * sizeof(*accumulators));
	boneState *curAccumulatorBone = accumulators;

	for(; curBoneID < numBones; ++curBoneID){
		// Prepend any user transformations to the bone's local bind state.
		// S = B*U
		transformMultiplyOut(&curSkeleBone->localBind, curTransform, curAccumulatorBone);
		// If the bone has a parent, append its transformation.
		// S = P*B*U
		if(!valueIsInvalid(curSkeleBone->parent, boneIndex)){
			transformMultiplyP2(&obj->skeleState.bones[curSkeleBone->parent], curAccumulatorBone);
		}
		// Prepend the total animation transformations.
		// S = P*B*U*A
		skeleStatePrependAnimations(curAccumulatorBone, &obj->skeleState, curBoneID, curSkeleBone->name);

		if(curPhysBoneID != lastPhysBoneID && *curPhysBoneID == curBoneID){
			// If the rigid body is simulated, we should
			// transform it using the current bone's state.
			if(physRigidBodyIsSimulated(curBody)){
				curBody->state = *curAccumulatorBone;
				// Make sure the rigid body actually stays where we've placed it.
				physRigidBodyCentroidFromPosition(curBody);

			}
			// Make sure the rigid body's colliders are added
			// to their island on the next physics update.
			if(physRigidBodyIsCollidable(curBody)){
				curBody->flags |= PHYSRIGIDBODY_TRANSFORMED;
			}

			++curPhysBoneID;
			// Exit if we've prepared all of the rigid bodies.
			if(curPhysBoneID == lastPhysBoneID){
				break;
			}
			modulePhysicsRigidBodyNext(curBody);
		}

		++curTransform;
		++curSkeleBone;
		++curAccumulatorBone;
	}

	// Remember to free the temporary bone states.
	memoryManagerGlobalFree(accumulators);
}


void objectUpdate(object *const restrict obj, const float dt){
	// Update each skeletal animation.
	skeleStateUpdate(&obj->skeleState, dt);
	// Generate the object's global bone states.
	updateBones(obj);

	model *curMdl = obj->mdls;
	// Animate each of the component models' textures.
	while(curMdl != NULL){
		modelUpdate(curMdl, dt);
		curMdl = moduleModelNext(curMdl);
	}
}

#warning "A lot of this stuff should be moved outside, especially the OpenGL code and skeleton stuff."
#include "billboard.h"
void objectDraw(
	const object *const restrict obj, const camera *const restrict cam,
	const meshShader *const restrict shader, const float dt
){


	/** TEMPORARY DEBUG DRAW TEST **/
	{
		if(obj->skeleState.skele->numBones > 1){
			debugDrawSkeleton(&obj->skeleState, debugDrawInfoInit(
				GL_LINES, GL_LINE, vec3InitSetC(0.f, 1.f, 0.f), 1.f
			), &cam->vpMatrix);
		}
		if(obj->physBodies != NULL){
			if(obj->physBodies->colliders->node != NULL){
				debugDrawColliderAABB(&(obj->physBodies->colliders->node->aabb), debugDrawInfoInit(
					GL_TRIANGLES, GL_LINE, vec3InitSetC(1.f, 0.4f, 0.f), 1.f
				), &cam->vpMatrix);
			}
			// We don't update the colliders when they aren't collidable.
			if(physRigidBodyIsCollidable(obj->physBodies)){
				debugDrawColliderHull(&(obj->physBodies->colliders->global.data.hull), debugDrawInfoInit(
					GL_TRIANGLES, GL_LINE, vec3InitSetC(1.f, 0.4f, 0.f), 1.f
				), &cam->vpMatrix);
			}
		}
	}


	{
		const model *curMdl = obj->mdls;
		mat3x4 *const animStates = memoryManagerGlobalAlloc(obj->skeleState.skele->numBones * sizeof(*animStates));
		// Convert the object's bone states into
		// a matrix representation for the shader.
		prepareBoneMatrices(&obj->skeleState, animStates);

		// Draw each of the models.
		while(curMdl != NULL){
			modelDraw(curMdl, obj->skeleState.skele, animStates, shader);
			curMdl = moduleModelNext(curMdl);
		}

		memoryManagerGlobalFree(animStates);
	}
}


/** We don't currently have a way of freeing the stuff that's commented out. **/
void objectDelete(object *const restrict obj){
	physicsRigidBody *const body = obj->physBodies;

	if(obj->boneTransforms != NULL){
		memoryManagerGlobalFree(obj->boneTransforms);
	}
	skeleStateDelete(&obj->skeleState);

	//obj->colliders = NULL;

	// Recall that an object's rigid bodies may be stored
	// in an island's double list. Therefore, we need to
	// be careful not to use the array free function, as
	// this could remove the rigid bodies of other objects!
	if(body != NULL){
		size_t numBodies = obj->objDef->numBodies;
		do {
			modulePhysicsRigidBodyFree(&obj->physBodies, body);
			--numBodies;
		} while(numBodies > 0);
	}
	if(obj->physBoneIDs != NULL){
		memoryManagerGlobalFree(obj->physBoneIDs);
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

	modulePhysicsRigidBodyDefFreeArray(&objDef->physBodies);
	if(objDef->physBoneIDs != NULL){
		memoryManagerGlobalFree(objDef->physBoneIDs);
	}

	if(objDef->mdlDefs != NULL){
		memoryManagerGlobalFree(objDef->mdlDefs);
	}
}


/*
** Update an object's skeleton using the following procedure for each bone:
**     1. Begin with the bone's local bind pose, B.
**     2. Append the bone's user transformation, U.
**     3. Append the bone's parent's transformation, P.
**     4. Prepend the bone's animation transformation, A.
** If S is the final state and these transformations are treated as matrices, then
**     S = P*U*B*A.
** Note that this process implicitly assumes that parent bones are animated before children.
*/
static void updateBones(object *const restrict obj){
	boneState *curTransform = obj->boneTransforms;
	boneState *curObjBone = obj->skeleState.bones;
	const boneState *const lastObjBone = &curObjBone[obj->skeleState.skele->numBones];
	const bone *curSkeleBone = obj->skeleState.skele->bones;

	physicsRigidBody *curBody = obj->physBodies;
	boneIndex curBoneID = 0;
	// We store the rigid bodies in order of increasing bone IDs.
	// We can simply move to the next ID when we find a bone with a rigid body.
	const boneIndex *curPhysBoneID = obj->physBoneIDs;
	const boneIndex *const lastPhysBoneID = &obj->physBoneIDs[obj->numBodies];

	for(; curObjBone < lastObjBone; ++curObjBone){
		// If a simulated rigid body is attached to the current bone,
		// we should transform the bone based off the rigid body's state.
		if(
			curPhysBoneID != lastPhysBoneID &&
			*curPhysBoneID == curBoneID &&
			physRigidBodyIsSimulated(curBody)
		){

			physRigidBodyUpdatePosition(curBody);
			// Copy the rigid body's state over to the bone.
			*curTransform = curBody->state;
			*curObjBone = curBody->state;

			modulePhysicsRigidBodyNext(curBody);
			++curPhysBoneID;

		// Otherwise, if this bone does not have a rigid
		// body attached, we should animate it normally.
		}else{
			// Prepend any user transformations to the bone's local bind state.
			// S = U*B
			transformMultiplyOut(curTransform, &curSkeleBone->localBind, curObjBone);
			// If the bone has a parent, append its transformation.
			// S = P*U*B
			if(!valueIsInvalid(curSkeleBone->parent, boneIndex)){
				transformMultiplyP2(&obj->skeleState.bones[curSkeleBone->parent], curObjBone);
			}
			// Prepend the total animation transformations.
			// S = P*U*B*A
			skeleStatePrependAnimations(curObjBone, &obj->skeleState, curBoneID, curSkeleBone->name);

			// If the current bone has a rigid body that is not simulated,
			// we should transform the rigid body based off the bone's state.
			if(curPhysBoneID != lastPhysBoneID && *curPhysBoneID == curBoneID){
				// Copy the bone's state over to the rigid body.
				curBody->state = *curObjBone;
				// Update the rigid body's centroid to reflect its new position.
				physRigidBodyCentroidFromPosition(curBody);

				modulePhysicsRigidBodyNext(curBody);
				++curPhysBoneID;
			}
		}

		++curTransform;
		++curSkeleBone;
		++curBoneID;
	}
}

/*
** During our update step, we generate global bone states for each
** bone in the animation. Before we send them to the shader, we need
** to bring them back into local space and convert them to matrices.
*/
static void prepareBoneMatrices(const skeletonState *const restrict skeleState, mat3x4 *animStates){
	const boneState *curObjBone = skeleState->bones;
	const boneState *const lastObjBone = &curObjBone[skeleState->skele->numBones];
	const bone *curSkeleBone = skeleState->skele->bones;

	// Convert each bone from global space to local space,
	// then convert it to a matrix and store it in "animStates".
	for(; curObjBone < lastObjBone; ++curObjBone){
		boneState tempBone;

		transformMultiplyOut(curObjBone, &curSkeleBone->invGlobalBind, &tempBone);
		transformToMat3x4(&tempBone, animStates);

		++curSkeleBone;
		++animStates;
	}
}