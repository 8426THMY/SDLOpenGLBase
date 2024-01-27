#include "modulePhysics.h"


#include "memoryManager.h"


moduleDefinePool(
	PhysicsAABBNode, aabbNode, g_aabbNodeManager,
	{}, MODULE_AABBNODE_MANAGER_SIZE
)
moduleDefineDoubleList(
	PhysicsContactPair, physicsContactPair, g_physContactPairManager,
	physContactPairDelete, MODULE_PHYSCONTACTPAIR_MANAGER_SIZE
)
moduleDefineDoubleList(
	PhysicsSeparationPair, physicsSeparationPair, g_physSeparationPairManager,
	physSeparationPairDelete, MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE
)
moduleDefineDoubleList(
	PhysicsJoint, physicsJoint, g_physJointManager,
	physJointDelete, MODULE_PHYSJOINT_MANAGER_SIZE
)
moduleDefineSingleList(
	PhysicsCollider, physicsCollider, g_physColliderManager,
	physColliderDelete, MODULE_PHYSCOLLIDER_MANAGER_SIZE
)
moduleDefineSingleList(
	PhysicsRigidBodyDef, physicsRigidBodyDef, g_physRigidBodyDefManager,
	physRigidBodyDefDelete, MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
)
moduleDefineDoubleList(
	PhysicsRigidBody, physicsRigidBody, g_physRigidBodyManager,
	physRigidBodyDelete, MODULE_PHYSRIGIDBODY_MANAGER_SIZE
)

// Free a collider instance that has been allocated.
void modulePhysicsColliderFreeInstance(
	physicsCollider **const restrict start,
	physicsCollider *const restrict collider,
	physicsCollider *const restrict prev
){

	physColliderDeleteInstance(collider);
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
		modulePhysicsRigidBodyFree(NULL, i);
	MEMDOUBLELIST_LOOP_END(g_physRigidBodyManager, i)
	memoryManagerGlobalDeleteRegions(g_physRigidBodyManager.region);
	// physicsRigidBodyDef
	MEMSINGLELIST_LOOP_BEGIN(g_physRigidBodyDefManager, i, physicsRigidBodyDef)
		modulePhysicsRigidBodyDefFree(NULL, i, NULL);
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