#include "modulePhysics.h"


memoryPool g_aabbNodeManager;
memoryPool g_physContactPairManager;
memoryPool g_physSeparationPairManager;
memorySingleList g_physColliderManager;
memorySingleList g_physRigidBodyDefManager;
memorySingleList g_physRigidBodyManager;


#warning "What if we aren't using the global memory manager?"


return_t modulePhysicsSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		// aabbNode
		memPoolInit(
			&g_aabbNodeManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_AABBNODES_MANAGER_SIZE)),
			MODULE_AABBNODES_MANAGER_SIZE, MODULE_AABBNODES_ELEMENT_SIZE
		) != NULL &&
		// physicsContactPair
		memPoolInit(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE)),
			MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE, MODULE_PHYSCOLLIDER_ELEMENT_SIZE
		) != NULL &&
		// physicsSeparationPair
		memPoolInit(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE)),
			MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE, MODULE_PHYSSEPARATIONPAIRS_ELEMENT_SIZE
		) != NULL &&
		// physicsCollider
		memSingleListInit(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE, MODULE_PHYSCOLLIDER_ELEMENT_SIZE
		) != NULL &&
		// physicsRigidBodyDef
		memSingleListInit(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE, MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE
		) != NULL &&
		// physicsRigidBody
		memSingleListInit(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE, MODULE_PHYSRIGIDBODY_ELEMENT_SIZE
		) != NULL
	);
}

/*
** Solve the constraints, such as contacts and joints,
** for every system that is currently being simulated.
**
** Currently, our physics tick looks something like this:
**
** for all scenes {
**     sceneTick(){
**         for all scene.objects {
**             objTick(){
**                 for all object.bones {
**                     if bone has rigid body {
**                         update collider configuration;
**                         update collider vertices;
**                         update collider AABB node;
**                     }else{
**                         animate bone;
**                     }
**                 }
**             }
**         }
**     }
** }
**
** for all rigid bodies {
**     integrate velocity;
**     reset forces;
**     update colliders;
**     update bounding boxes;
** }
**
** queryIslands(){
**     for all islands {
**         for all island.rigidBodies {
**             check collisions;
**             update separations;
**             update contacts {
**                 if contact persists {
**                     presolve contact;
**                 }
**             }
**         }
**     }
** }
**
** solveConstraints(){
**     for all joints {
**         presolve joint;
**     }
**
**     for all velocity solver iterations {
**         for all joints {
**             solve joint velocity constraints;
**         }
**
**         for all contacts {
**             solve contact velocity constraints;
**         }
**     }
**
**     for all rigid bodies {
**         integrate position;
**     }
**
**     for configuration solver iterations {
**         for all joints {
**             solve joint configuration constraints;
**         }
**
**         for all contacts {
**             solve contact configuration constraints;
**         }
**     }
** }
*/
void modulePhysicsSolveConstraints(const float dt){
	size_t i;


	// Presolve joints.
	/**MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, joint, physJoint *);
		physJointPresolveConstraints(joint, dt);
	MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, joint, break);**/


	// Iteratively solve joint and contact velocity constraints.
	for(i = PHYSICS_VELOCITY_SOLVER_NUM_ITERATIONS; i > 0; --i){
		// Solve joint velocity constraints.
		/**MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, joint, physJoint *);
			physJointSolveVelocityConstraints(joint);
		MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, joint, break);**/

		// Solve contact velocity constraints.
		MEMPOOL_LOOP_BEGIN(g_physContactPairManager, contact, physicsContactPair)
			physManifoldSolveVelocity(&contact->manifold, contact->cA->owner, contact->cB->owner);
		MEMPOOL_LOOP_END(g_physContactPairManager, contact, break)
	}


	// Integrate each physics object's position.
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyManager, body, physicsRigidBody)
		physRigidBodyIntegratePosition(body, dt);
	#ifndef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
	MEMSINGLELIST_LOOP_END(g_physRigidBodyManager, body, return)
	#else
	MEMSINGLELIST_LOOP_END(g_physRigidBodyManager, body, break)


	// Iteratively solve joint and contact configuration constraints.
	for(i = PHYSICS_POSITION_SOLVER_NUM_ITERATIONS; i > 0; --i){
		return_t solved = 1;
		float separation = PHYSCONTACT_PENETRATION_ERROR_THRESHOLD;

		#ifdef PHYSJOINT_USE_POSITIONAL_CORRECTION
		// Solve joint configuration constraints.
		/**MEMORY_QLINK_LOOP_BEGIN(__g_PhysicsJointResourceArray, joint, physJoint *);
			solved &= physJointSolveConfigurationConstraints(joint);
		MEMORY_QLINK_LOOP_END(__g_PhysicsJointResourceArray, joint, break);**/
		#endif

		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		// Solve contact configuration constraints.
		MEMPOOL_LOOP_BEGIN(g_physContactPairManager, contact, physicsContactPair)
			separation = physManifoldSolvePosition(&contact->manifold, contact->cA->owner, contact->cB->owner, separation);
		MEMPOOL_LOOP_END(g_physContactPairManager, contact, break)
		#endif

		// Exit if the errors are small.
		if(solved && separation >= PHYSCONTACT_PENETRATION_ERROR_THRESHOLD){
			return;
		}
	}
	#endif
}

void modulePhysicsCleanup(){
	// physicsRigidBody
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyManager, i, physicsRigidBody)
		modulePhysicsBodyFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physRigidBodyManager, i, break)
	memSingleListDelete(&g_physRigidBodyManager, memoryManagerGlobalFree);
	// physicsRigidBodyDef
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyDefManager, i, physicsRigidBodyDef)
		modulePhysicsBodyDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physRigidBodyDefManager, i, break)
	memSingleListDelete(&g_physRigidBodyDefManager, memoryManagerGlobalFree);
	// physicsCollider
	MEMSINGLELIST_LOOP_BEGIN(g_physColliderManager, i, physicsCollider)
		modulePhysicsColliderFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physColliderManager, i, break)
	memSingleListDelete(&g_physColliderManager, memoryManagerGlobalFree);
	// physicsSeparationPair
	MEMPOOL_LOOP_BEGIN(g_physSeparationPairManager, i, physicsSeparationPair)
		modulePhysicsSeparationPairFree(i);
	MEMPOOL_LOOP_END(g_physSeparationPairManager, i, break)
	memPoolDelete(&g_physSeparationPairManager, memoryManagerGlobalFree);
	// physicsContactPair
	MEMPOOL_LOOP_BEGIN(g_physContactPairManager, i, physicsContactPair)
		modulePhysicsContactPairFree(i);
	MEMPOOL_LOOP_END(g_physContactPairManager, i, break)
	memPoolDelete(&g_physContactPairManager, memoryManagerGlobalFree);
	// aabbNode
	MEMPOOL_LOOP_BEGIN(g_aabbNodeManager, i, aabbNode)
		modulePhysicsAABBNodeFree(i);
	MEMPOOL_LOOP_END(g_aabbNodeManager, i, break)
	memPoolDelete(&g_aabbNodeManager, memoryManagerGlobalFree);
}


// Allocate memory for an AABB tree node and return a handle to it.
aabbNode *modulePhysicsAABBNodeAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_aabbNodeManager));
	#else
	aabbNode *newBlock = memPoolAlloc(&g_aabbNodeManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_aabbNodeManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_AABBNODES_MANAGER_SIZE)),
			MODULE_AABBNODES_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_aabbNodeManager);
		}
	}
	return(newBlock);
	#endif
}

// Free an AABB tree node that has been allocated.
void modulePhysicsAABBNodeFree(aabbNode *const restrict node){
	memPoolFree(&g_aabbNodeManager, node);
}

// Delete every AABB tree node in the manager.
void modulePhysicsAABBNodeClear(){
	MEMPOOL_LOOP_BEGIN(g_aabbNodeManager, i, aabbNode)
		modulePhysicsAABBNodeFree(i);
	MEMPOOL_LOOP_END(g_aabbNodeManager, i, break)
	memPoolClear(&g_aabbNodeManager);
}


// Allocate memory for a contact
// pair and return a handle to it.
physicsContactPair *modulePhysicsContactPairAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_physContactPairManager));
	#else
	physicsContactPair *newBlock = memPoolAlloc(&g_physContactPairManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE)),
			MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_physContactPairManager);
		}
	}
	return(newBlock);
	#endif
}

// Free a contact pair that has been allocated.
void modulePhysicsContactPairFree(physicsContactPair *const restrict cPair){
	physContactPairDelete(cPair);
	memPoolFree(&g_physContactPairManager, cPair);
}

// Delete every contact pair in the manager.
void modulePhysicsContactPairClear(){
	MEMPOOL_LOOP_BEGIN(g_physContactPairManager, i, physicsContactPair)
		modulePhysicsContactPairFree(i);
	MEMPOOL_LOOP_END(g_physContactPairManager, i, break)
	memPoolClear(&g_physContactPairManager);
}


// Allocate memory for a separation pair and return a handle to it.
physicsSeparationPair *modulePhysicsSeparationPairAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memPoolAlloc(&g_physSeparationPairManager));
	#else
	physicsSeparationPair *newBlock = memPoolAlloc(&g_physSeparationPairManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memPoolExtend(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE)),
			MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE
		)){
			newBlock = memPoolAlloc(&g_physSeparationPairManager);
		}
	}
	return(newBlock);
	#endif
}

// Free a separation pair that has been allocated.
void modulePhysicsSeparationPairFree(physicsSeparationPair *const restrict sPair){
	physSeparationPairDelete(sPair);
	memPoolFree(&g_physSeparationPairManager, sPair);
}

// Delete every separation pair in the manager.
void modulePhysicsSeparationPairClear(){
	MEMPOOL_LOOP_BEGIN(g_physSeparationPairManager, i, physicsSeparationPair)
		modulePhysicsSeparationPairFree(i);
	MEMPOOL_LOOP_END(g_physSeparationPairManager, i, break)
	memPoolClear(&g_physSeparationPairManager);
}


// Allocate a new collider array.
physicsCollider *modulePhysicsColliderAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_physColliderManager));
	#else
	physicsCollider *newBlock = memSingleListAlloc(&g_physColliderManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_physColliderManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a collider at the beginning of an array.
physicsCollider *modulePhysicsColliderPrepend(physicsCollider **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_physColliderManager, (void **)start));
	#else
	physicsCollider *newBlock = memSingleListPrepend(&g_physColliderManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_physColliderManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a collider at the end of an array.
physicsCollider *modulePhysicsColliderAppend(physicsCollider **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_physColliderManager, (void **)start));
	#else
	physicsCollider *newBlock = memSingleListAppend(&g_physColliderManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_physColliderManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a collider after the element "prevData".
physicsCollider *modulePhysicsColliderInsertBefore(physicsCollider **const restrict start, physicsCollider *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_physColliderManager, (void **)start, (void *)prevData));
	#else
	physicsCollider *newBlock = memSingleListInsertBefore(&g_physColliderManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_physColliderManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert a collider after the element "data".
physicsCollider *modulePhysicsColliderInsertAfter(physicsCollider **const restrict start, physicsCollider *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_physColliderManager, (void **)start, (void *)data));
	#else
	physicsCollider *newBlock = memSingleListInsertAfter(&g_physColliderManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_physColliderManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

// Free a collider instance that has been allocated.
void modulePhysicsColliderFreeInstance(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prevData){
	physColliderDeleteInstance(collider);
	memSingleListFree(&g_physColliderManager, (void **)start, (void *)collider, (void *)prevData);
}

// Free a collider that has been allocated.
void modulePhysicsColliderFree(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prevData){
	physColliderDelete(collider);
	memSingleListFree(&g_physColliderManager, (void **)start, (void *)collider, (void *)prevData);
}

// Free an entire collider instance array.
void modulePhysicsColliderFreeInstanceArray(physicsCollider **const restrict start){
	physicsCollider *collider = *start;
	while(collider != NULL){
		modulePhysicsColliderFreeInstance(start, collider, NULL);
		collider = *start;
	}
}

// Free an entire collider array.
void modulePhysicsColliderFreeArray(physicsCollider **const restrict start){
	physicsCollider *collider = *start;
	while(collider != NULL){
		modulePhysicsColliderFree(start, collider, NULL);
		collider = *start;
	}
}

// Delete every collider in the manager.
void modulePhysicsColliderClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_physColliderManager, i, physicsCollider)
		modulePhysicsColliderFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physColliderManager, i, break)
	memSingleListClear(&g_physColliderManager);
}


// Allocate a new rigid body base array.
physicsRigidBodyDef *modulePhysicsBodyDefAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_physRigidBodyDefManager));
	#else
	physicsRigidBodyDef *newBlock = memSingleListAlloc(&g_physRigidBodyDefManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_physRigidBodyDefManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body base at the beginning of an array.
physicsRigidBodyDef *modulePhysicsBodyDefPrepend(physicsRigidBodyDef **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_physRigidBodyDefManager, (void **)start));
	#else
	physicsRigidBodyDef *newBlock = memSingleListPrepend(&g_physRigidBodyDefManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_physRigidBodyDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body base at the end of an array.
physicsRigidBodyDef *modulePhysicsBodyDefAppend(physicsRigidBodyDef **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_physRigidBodyDefManager, (void **)start));
	#else
	physicsRigidBodyDef *newBlock = memSingleListAppend(&g_physRigidBodyDefManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_physRigidBodyDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body base after the element "prevData".
physicsRigidBodyDef *modulePhysicsBodyDefInsertBefore(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_physRigidBodyDefManager, (void **)start, (void *)prevData));
	#else
	physicsRigidBodyDef *newBlock = memSingleListInsertBefore(&g_physRigidBodyDefManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_physRigidBodyDefManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body base after the element "data".
physicsRigidBodyDef *modulePhysicsBodyDefInsertAfter(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_physRigidBodyDefManager, (void **)start, (void *)data));
	#else
	physicsRigidBodyDef *newBlock = memSingleListInsertAfter(&g_physRigidBodyDefManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_physRigidBodyDefManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

// Free a rigid body base that has been allocated.
void modulePhysicsBodyDefFree(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict bodyDef, physicsRigidBodyDef *const restrict prevData){
	physRigidBodyDefDelete(bodyDef);
	memSingleListFree(&g_physRigidBodyDefManager, (void **)start, (void *)bodyDef, (void *)prevData);
}

// Free an entire rigid body base array.
void modulePhysicsBodyDefFreeArray(physicsRigidBodyDef **const restrict start){
	physicsRigidBodyDef *bodyDef = *start;
	while(bodyDef != NULL){
		modulePhysicsBodyDefFree(start, bodyDef, NULL);
		bodyDef = *start;
	}
}

// Delete every rigid body base in the manager.
void modulePhysicsBodyDefClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyDefManager, i, physicsRigidBodyDef)
		modulePhysicsBodyDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physRigidBodyDefManager, i, break)
	memSingleListClear(&g_physRigidBodyDefManager);
}


// Allocate a new rigid body array.
physicsRigidBody *modulePhysicsBodyAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAlloc(&g_physRigidBodyManager));
	#else
	physicsRigidBody *newBlock = memSingleListAlloc(&g_physRigidBodyManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE
		)){
			newBlock = memSingleListAlloc(&g_physRigidBodyManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body at the beginning of an array.
physicsRigidBody *modulePhysicsBodyPrepend(physicsRigidBody **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListPrepend(&g_physRigidBodyManager, (void **)start));
	#else
	physicsRigidBody *newBlock = memSingleListPrepend(&g_physRigidBodyManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE
		)){
			newBlock = memSingleListPrepend(&g_physRigidBodyManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body at the end of an array.
physicsRigidBody *modulePhysicsBodyAppend(physicsRigidBody **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListAppend(&g_physRigidBodyManager, (void **)start));
	#else
	physicsRigidBody *newBlock = memSingleListAppend(&g_physRigidBodyManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE
		)){
			newBlock = memSingleListAppend(&g_physRigidBodyManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body after the element "prevData".
physicsRigidBody *modulePhysicsBodyInsertBefore(physicsRigidBody **const restrict start, physicsRigidBody *const restrict prevData){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertBefore(&g_physRigidBodyManager, (void **)start, (void *)prevData));
	#else
	physicsRigidBody *newBlock = memSingleListInsertBefore(&g_physRigidBodyManager, (void **)start, (void *)prevData);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertBefore(&g_physRigidBodyManager, (void **)start, (void *)prevData);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body after the element "data".
physicsRigidBody *modulePhysicsBodyInsertAfter(physicsRigidBody **const restrict start, physicsRigidBody *const restrict data){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_physRigidBodyManager, (void **)start, (void *)data));
	#else
	physicsRigidBody *newBlock = memSingleListInsertAfter(&g_physRigidBodyManager, (void **)start, (void *)data);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE
		)){
			newBlock = memSingleListInsertAfter(&g_physRigidBodyManager, (void **)start, (void *)data);
		}
	}
	return(newBlock);
	#endif
}

// Free a rigid body that has been allocated.
void modulePhysicsBodyFree(physicsRigidBody **const restrict start, physicsRigidBody *const restrict body, physicsRigidBody *const restrict prevData){
	physRigidBodyDelete(body);
	memSingleListFree(&g_physRigidBodyManager, (void **)start, (void *)body, (void *)prevData);
}

// Free an entire rigid body array.
void modulePhysicsBodyFreeArray(physicsRigidBody **const restrict start){
	physicsRigidBody *body = *start;
	while(body != NULL){
		modulePhysicsBodyFree(start, body, NULL);
		body = *start;
	}
}

// Delete every rigid body in the manager.
void modulePhysicsBodyClear(){
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyManager, i, physicsRigidBody)
		modulePhysicsBodyFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physRigidBodyManager, i, break)
	memSingleListClear(&g_physRigidBodyManager);
}