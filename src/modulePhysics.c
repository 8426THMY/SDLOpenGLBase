#include "modulePhysics.h"


memoryPool g_aabbNodeManager;
memoryDoubleList g_physContactPairManager;
memoryDoubleList g_physSeparationPairManager;
memoryDoubleList g_physJointManager;
memorySingleList g_physColliderManager;
memorySingleList g_physRigidBodyDefManager;
memoryDoubleList g_physRigidBodyManager;


return_t modulePhysicsSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our managers.
	return(
		// aabbNode
		memPoolInit(
			&g_aabbNodeManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_AABBNODE_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_AABBNODE_MANAGER_SIZE), MODULE_AABBNODE_ELEMENT_SIZE
		) != NULL &&
		// physicsContactPair
		memDoubleListInit(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE), MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE
		) != NULL &&
		// physicsSeparationPair
		memDoubleListInit(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE), MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE
		) != NULL &&
		// physicsJoint
		memDoubleListInit(
			&g_physJointManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE), MODULE_PHYSJOINT_ELEMENT_SIZE
		) != NULL &&
		// physicsCollider
		memSingleListInit(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE), MODULE_PHYSCOLLIDER_ELEMENT_SIZE
		) != NULL &&
		// physicsRigidBodyDef
		memSingleListInit(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE), MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE
		) != NULL &&
		// physicsRigidBody
		memDoubleListInit(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE), MODULE_PHYSRIGIDBODY_ELEMENT_SIZE
		) != NULL
	);
}

void modulePhysicsCleanup(){
	// physicsRigidBody
	MEMDOUBLELIST_LOOP_BEGIN(g_physRigidBodyManager, i, physicsRigidBody)
		modulePhysicsBodyFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physRigidBodyManager, i)
	memoryManagerGlobalDeleteRegions(g_physRigidBodyManager.region);
	// physicsRigidBodyDef
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyDefManager, i, physicsRigidBodyDef)
		modulePhysicsBodyDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physRigidBodyDefManager, i)
	memoryManagerGlobalDeleteRegions(g_physRigidBodyDefManager.region);
	// physicsCollider
	MEMSINGLELIST_LOOP_BEGIN(g_physColliderManager, i, physicsCollider)
		modulePhysicsColliderFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(g_physColliderManager, i)
	memoryManagerGlobalDeleteRegions(g_physColliderManager.region);
	// physicsJoint
	MEMDOUBLELIST_LOOP_BEGIN(g_physJointManager, i, physicsJoint)
		modulePhysicsJointFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physJointManager, i)
	memoryManagerGlobalDeleteRegions(g_physJointManager.region);
	// physicsSeparationPair
	MEMDOUBLELIST_LOOP_BEGIN(g_physSeparationPairManager, i, physicsSeparationPair)
		modulePhysicsSeparationPairFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physSeparationPairManager, i)
	memoryManagerGlobalDeleteRegions(g_physSeparationPairManager.region);
	// physicsContactPair
	MEMDOUBLELIST_LOOP_BEGIN(g_physContactPairManager, i, physicsContactPair)
		modulePhysicsContactPairFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physContactPairManager, i)
	memoryManagerGlobalDeleteRegions(g_physContactPairManager.region);
	// aabbNode
	MEMPOOL_LOOP_BEGIN(g_aabbNodeManager, i, aabbNode)
		modulePhysicsAABBNodeFree(i);
	MEMPOOL_LOOP_END(g_aabbNodeManager, i)
	memoryManagerGlobalDeleteRegions(g_aabbNodeManager.region);
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
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_AABBNODE_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_AABBNODE_MANAGER_SIZE)
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
	MEMPOOL_LOOP_END(g_aabbNodeManager, i)
	memPoolClear(&g_aabbNodeManager);
}


// Allocate a new contact pair array.
physicsContactPair *modulePhysicsContactPairAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAlloc(&g_physContactPairManager));
	#else
	physicsContactPair *newBlock = memDoubleListAlloc(&g_physContactPairManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAlloc(&g_physContactPairManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a contact pair at the beginning of an array.
physicsContactPair *modulePhysicsContactPairPrepend(physicsContactPair **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListPrepend(&g_physContactPairManager, (void **)start));
	#else
	physicsContactPair *newBlock = memDoubleListPrepend(&g_physContactPairManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListPrepend(&g_physContactPairManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a contact pair at the end of an array.
physicsContactPair *modulePhysicsContactPairAppend(physicsContactPair **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAppend(&g_physContactPairManager, (void **)start));
	#else
	physicsContactPair *newBlock = memDoubleListAppend(&g_physContactPairManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAppend(&g_physContactPairManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a contact pair before the element "next".
physicsContactPair *modulePhysicsContactPairInsertBefore(physicsContactPair **const restrict start, physicsContactPair *const restrict next){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertBefore(&g_physContactPairManager, (void **)start, (void *)next));
	#else
	physicsContactPair *newBlock = memDoubleListInsertBefore(&g_physContactPairManager, (void **)start, (void *)next);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertBefore(&g_physContactPairManager, (void **)start, (void *)next);
		}
	}
	return(newBlock);
	#endif
}

// Insert a contact pair after the element "prev".
physicsContactPair *modulePhysicsContactPairInsertAfter(physicsContactPair **const restrict start, physicsContactPair *const restrict prev){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertAfter(&g_physContactPairManager, (void **)start, (void *)prev));
	#else
	physicsContactPair *newBlock = memDoubleListInsertAfter(&g_physContactPairManager, (void **)start, (void *)prev);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertAfter(&g_physContactPairManager, (void **)start, (void *)prev);
		}
	}
	return(newBlock);
	#endif
}

physicsContactPair *modulePhysicsContactPairNext(const physicsContactPair *const restrict cPair){
	return(memDoubleListNext(cPair));
}

physicsContactPair *modulePhysicsContactPairPrev(const physicsContactPair *const restrict cPair){
	return(memDoubleListPrev(cPair));
}

// Free a contact pair that has been allocated.
void modulePhysicsContactPairFree(physicsContactPair **const restrict start, physicsContactPair *const restrict cPair){
	physContactPairDelete(cPair);
	memDoubleListFree(&g_physContactPairManager, (void **)start, (void *)cPair);
}

// Free an entire contact pair array.
void modulePhysicsContactPairFreeArray(physicsContactPair **const restrict start){
	physicsContactPair *cPair = *start;
	while(cPair != NULL){
		modulePhysicsContactPairFree(start, cPair);
		cPair = *start;
	}
}

// Delete every contact pair in the manager.
void modulePhysicsContactPairClear(){
	MEMDOUBLELIST_LOOP_BEGIN(g_physContactPairManager, i, physicsContactPair)
		modulePhysicsContactPairFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physContactPairManager, i)
	memDoubleListClear(&g_physContactPairManager);
}


// Allocate a new separation pair array.
physicsSeparationPair *modulePhysicsSeparationPairAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAlloc(&g_physSeparationPairManager));
	#else
	physicsSeparationPair *newBlock = memDoubleListAlloc(&g_physSeparationPairManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAlloc(&g_physSeparationPairManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a separation pair at the beginning of an array.
physicsSeparationPair *modulePhysicsSeparationPairPrepend(physicsSeparationPair **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListPrepend(&g_physSeparationPairManager, (void **)start));
	#else
	physicsSeparationPair *newBlock = memDoubleListPrepend(&g_physSeparationPairManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListPrepend(&g_physSeparationPairManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a separation pair at the end of an array.
physicsSeparationPair *modulePhysicsSeparationPairAppend(physicsSeparationPair **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAppend(&g_physSeparationPairManager, (void **)start));
	#else
	physicsSeparationPair *newBlock = memDoubleListAppend(&g_physSeparationPairManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAppend(&g_physSeparationPairManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a separation pair before the element "next".
physicsSeparationPair *modulePhysicsSeparationPairInsertBefore(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict next){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertBefore(&g_physSeparationPairManager, (void **)start, (void *)next));
	#else
	physicsSeparationPair *newBlock = memDoubleListInsertBefore(&g_physSeparationPairManager, (void **)start, (void *)next);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertBefore(&g_physSeparationPairManager, (void **)start, (void *)next);
		}
	}
	return(newBlock);
	#endif
}

// Insert a separation pair after the element "prev".
physicsSeparationPair *modulePhysicsSeparationPairInsertAfter(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict prev){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertAfter(&g_physSeparationPairManager, (void **)start, (void *)prev));
	#else
	physicsSeparationPair *newBlock = memDoubleListInsertAfter(&g_physSeparationPairManager, (void **)start, (void *)prev);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCONTACTPAIR_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertAfter(&g_physSeparationPairManager, (void **)start, (void *)prev);
		}
	}
	return(newBlock);
	#endif
}

physicsSeparationPair *modulePhysicsSeparationPairNext(const physicsSeparationPair *const restrict sPair){
	return(memDoubleListNext(sPair));
}

physicsSeparationPair *modulePhysicsSeparationPairPrev(const physicsSeparationPair *const restrict sPair){
	return(memDoubleListPrev(sPair));
}

// Free a separation pair that has been allocated.
void modulePhysicsSeparationPairFree(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict sPair){
	physSeparationPairDelete(sPair);
	memDoubleListFree(&g_physSeparationPairManager, (void **)start, (void *)sPair);
}

// Free an entire separation pair array.
void modulePhysicsSeparationPairFreeArray(physicsSeparationPair **const restrict start){
	physicsSeparationPair *sPair = *start;
	while(sPair != NULL){
		modulePhysicsSeparationPairFree(start, sPair);
		sPair = *start;
	}
}

// Delete every separation pair in the manager.
void modulePhysicsSeparationPairClear(){
	MEMDOUBLELIST_LOOP_BEGIN(g_physSeparationPairManager, i, physicsSeparationPair)
		modulePhysicsSeparationPairFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physSeparationPairManager, i)
	memDoubleListClear(&g_physSeparationPairManager);
}


// Allocate a new joint array.
physicsJoint *modulePhysicsJointAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAlloc(&g_physJointManager));
	#else
	physicsJoint *newBlock = memDoubleListAlloc(&g_physJointManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physJointManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAlloc(&g_physJointManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a joint at the beginning of an array.
physicsJoint *modulePhysicsJointPrepend(physicsJoint **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListPrepend(&g_physJointManager, (void **)start));
	#else
	physicsJoint *newBlock = memDoubleListPrepend(&g_physJointManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physJointManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)
		)){
			newBlock = memDoubleListPrepend(&g_physJointManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a joint at the end of an array.
physicsJoint *modulePhysicsJointAppend(physicsJoint **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAppend(&g_physJointManager, (void **)start));
	#else
	physicsJoint *newBlock = memDoubleListAppend(&g_physJointManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physJointManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAppend(&g_physJointManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a joint before the element "next".
physicsJoint *modulePhysicsJointInsertBefore(physicsJoint **const restrict start, physicsJoint *const restrict next){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertBefore(&g_physJointManager, (void **)start, (void *)next));
	#else
	physicsJoint *newBlock = memDoubleListInsertBefore(&g_physJointManager, (void **)start, (void *)next);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physJointManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertBefore(&g_physJointManager, (void **)start, (void *)next);
		}
	}
	return(newBlock);
	#endif
}

// Insert a joint after the element "prev".
physicsJoint *modulePhysicsJointInsertAfter(physicsJoint **const restrict start, physicsJoint *const restrict prev){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertAfter(&g_physJointManager, (void **)start, (void *)prev));
	#else
	physicsJoint *newBlock = memDoubleListInsertAfter(&g_physJointManager, (void **)start, (void *)prev);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physJointManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSJOINT_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertAfter(&g_physJointManager, (void **)start, (void *)prev);
		}
	}
	return(newBlock);
	#endif
}

physicsJoint *modulePhysicsJointNext(const physicsJoint *const restrict joint){
	return(memDoubleListNext(joint));
}

physicsJoint *modulePhysicsJointPrev(const physicsJoint *const restrict joint){
	return(memDoubleListPrev(joint));
}

// Free a joint that has been allocated.
void modulePhysicsJointFree(physicsJoint **const restrict start, physicsJoint *const restrict joint){
	physJointDelete(joint);
	memDoubleListFree(&g_physJointManager, (void **)start, (void *)joint);
}

// Free an entire joint array.
void modulePhysicsJointFreeArray(physicsJoint **const restrict start){
	physicsJoint *joint = *start;
	while(joint != NULL){
		modulePhysicsJointFree(start, joint);
		joint = *start;
	}
}

// Delete every joint in the manager.
void modulePhysicsJointClear(){
	MEMDOUBLELIST_LOOP_BEGIN(g_physJointManager, i, physicsJoint)
		modulePhysicsJointFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physJointManager, i)
	memDoubleListClear(&g_physJointManager);
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
			memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)
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
			memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)
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
			memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)
		)){
			newBlock = memSingleListAppend(&g_physColliderManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a collider after the element "prev".
physicsCollider *modulePhysicsColliderInsertAfter(physicsCollider **const restrict start, physicsCollider *const restrict prev){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_physColliderManager, (void **)start, (void *)prev));
	#else
	physicsCollider *newBlock = memSingleListInsertAfter(&g_physColliderManager, (void **)start, (void *)prev);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)
		)){
			newBlock = memSingleListInsertAfter(&g_physColliderManager, (void **)start, (void *)prev);
		}
	}
	return(newBlock);
	#endif
}

physicsCollider *modulePhysicsColliderNext(const physicsCollider *const restrict collider){
	return(memSingleListNext(collider));
}

// Free a collider instance that has been allocated.
void modulePhysicsColliderFreeInstance(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prev){
	physColliderDeleteInstance(collider);
	memSingleListFree(&g_physColliderManager, (void **)start, (void *)collider, (void *)prev);
}

// Free a collider that has been allocated.
void modulePhysicsColliderFree(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prev){
	physColliderDelete(collider);
	memSingleListFree(&g_physColliderManager, (void **)start, (void *)collider, (void *)prev);
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
	MEMSINGLELIST_LOOP_END(g_physColliderManager, i)
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
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)
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
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)
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
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)
		)){
			newBlock = memSingleListAppend(&g_physRigidBodyDefManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body base after the element "prev".
physicsRigidBodyDef *modulePhysicsBodyDefInsertAfter(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict prev){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memSingleListInsertAfter(&g_physRigidBodyDefManager, (void **)start, (void *)prev));
	#else
	physicsRigidBodyDef *newBlock = memSingleListInsertAfter(&g_physRigidBodyDefManager, (void **)start, (void *)prev);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memSingleListExtend(
			&g_physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)
		)){
			newBlock = memSingleListInsertAfter(&g_physRigidBodyDefManager, (void **)start, (void *)prev);
		}
	}
	return(newBlock);
	#endif
}

physicsRigidBodyDef *modulePhysicsBodyDefNext(const physicsRigidBodyDef *const restrict bodyDef){
	return(memSingleListNext(bodyDef));
}

// Free a rigid body base that has been allocated.
void modulePhysicsBodyDefFree(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict bodyDef, physicsRigidBodyDef *const restrict prev){
	physRigidBodyDefDelete(bodyDef);
	memSingleListFree(&g_physRigidBodyDefManager, (void **)start, (void *)bodyDef, (void *)prev);
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
	MEMSINGLELIST_LOOP_END(g_physRigidBodyDefManager, i)
	memSingleListClear(&g_physRigidBodyDefManager);
}


// Allocate a new rigid body array.
physicsRigidBody *modulePhysicsBodyAlloc(){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAlloc(&g_physRigidBodyManager));
	#else
	physicsRigidBody *newBlock = memDoubleListAlloc(&g_physRigidBodyManager);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAlloc(&g_physRigidBodyManager);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body at the beginning of an array.
physicsRigidBody *modulePhysicsBodyPrepend(physicsRigidBody **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListPrepend(&g_physRigidBodyManager, (void **)start));
	#else
	physicsRigidBody *newBlock = memDoubleListPrepend(&g_physRigidBodyManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)
		)){
			newBlock = memDoubleListPrepend(&g_physRigidBodyManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body at the end of an array.
physicsRigidBody *modulePhysicsBodyAppend(physicsRigidBody **const restrict start){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListAppend(&g_physRigidBodyManager, (void **)start));
	#else
	physicsRigidBody *newBlock = memDoubleListAppend(&g_physRigidBodyManager, (void **)start);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)
		)){
			newBlock = memDoubleListAppend(&g_physRigidBodyManager, (void **)start);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body before the element "next".
physicsRigidBody *modulePhysicsBodyInsertBefore(physicsRigidBody **const restrict start, physicsRigidBody *const restrict next){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertBefore(&g_physRigidBodyManager, (void **)start, (void *)next));
	#else
	physicsRigidBody *newBlock = memDoubleListInsertBefore(&g_physRigidBodyManager, (void **)start, (void *)next);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertBefore(&g_physRigidBodyManager, (void **)start, (void *)next);
		}
	}
	return(newBlock);
	#endif
}

// Insert a rigid body after the element "prev".
physicsRigidBody *modulePhysicsBodyInsertAfter(physicsRigidBody **const restrict start, physicsRigidBody *const restrict prev){
	#ifndef MEMORYREGION_EXTEND_ALLOCATORS
	return(memDoubleListInsertAfter(&g_physRigidBodyManager, (void **)start, (void *)prev));
	#else
	physicsRigidBody *newBlock = memDoubleListInsertAfter(&g_physRigidBodyManager, (void **)start, (void *)prev);
	// If we've run out of memory, allocate some more!
	if(newBlock == NULL){
		if(memDoubleListExtend(
			&g_physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)
		)){
			newBlock = memDoubleListInsertAfter(&g_physRigidBodyManager, (void **)start, (void *)prev);
		}
	}
	return(newBlock);
	#endif
}

physicsRigidBody *modulePhysicsBodyNext(const physicsRigidBody *const restrict body){
	return(memDoubleListNext(body));
}

physicsRigidBody *modulePhysicsBodyPrev(const physicsRigidBody *const restrict body){
	return(memDoubleListPrev(body));
}

// Free a rigid body that has been allocated.
void modulePhysicsBodyFree(physicsRigidBody **const restrict start, physicsRigidBody *const restrict body){
	physRigidBodyDelete(body);
	memDoubleListFree(&g_physRigidBodyManager, (void **)start, (void *)body);
}

// Free an entire rigid body array.
void modulePhysicsBodyFreeArray(physicsRigidBody **const restrict start){
	physicsRigidBody *body = *start;
	while(body != NULL){
		modulePhysicsBodyFree(start, body);
		body = *start;
	}
}

// Delete every rigid body in the manager.
void modulePhysicsBodyClear(){
	MEMDOUBLELIST_LOOP_BEGIN(g_physRigidBodyManager, i, physicsRigidBody)
		modulePhysicsBodyFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physRigidBodyManager, i)
	memDoubleListClear(&g_physRigidBodyManager);
}