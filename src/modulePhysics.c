#include "modulePhysics.h"


memoryPool aabbNodeManager;
memoryPool physContactPairManager;
memoryPool physSeparationPairManager;
memorySingleList physColliderManager;
memorySingleList physRigidBodyDefManager;
memorySingleList physRigidBodyManager;


#warning "What if we aren't using the global memory manager?"


return_t modulePhysicsSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager.
	return(
		// aabbNode
		memPoolInit(
			&aabbNodeManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_AABBNODES_MANAGER_SIZE)),
			MODULE_AABBNODES_MANAGER_SIZE, MODULE_AABBNODES_ELEMENT_SIZE
		) != NULL &&
		// physicsContactPair
		memPoolInit(
			&physContactPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE)),
			MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE, MODULE_PHYSCOLLIDER_ELEMENT_SIZE
		) != NULL &&
		// physicsSeparationPair
		memPoolInit(
			&physSeparationPairManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE)),
			MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE, MODULE_PHYSSEPARATIONPAIRS_ELEMENT_SIZE
		) != NULL &&
		// physicsCollider
		memSingleListInit(
			&physColliderManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSCOLLIDER_MANAGER_SIZE)),
			MODULE_PHYSCOLLIDER_MANAGER_SIZE, MODULE_PHYSCOLLIDER_ELEMENT_SIZE
		) != NULL &&
		// physicsRigidBodyDef
		memSingleListInit(
			&physRigidBodyDefManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE, MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE
		) != NULL &&
		// physicsRigidBody
		memSingleListInit(
			&physRigidBodyManager,
			memoryManagerGlobalAlloc(memoryGetRequiredSize(MODULE_PHYSRIGIDBODY_MANAGER_SIZE)),
			MODULE_PHYSRIGIDBODY_MANAGER_SIZE, MODULE_PHYSRIGIDBODY_ELEMENT_SIZE
		) != NULL
	);
}

void modulePhysicsCleanup(){
	// physicsRigidBody
	modulePhysicsBodyClear();
	memoryManagerGlobalFree(memSingleListRegionStart(physRigidBodyManager.region));
	// physicsRigidBodyDef
	modulePhysicsBodyDefClear();
	memoryManagerGlobalFree(memSingleListRegionStart(physRigidBodyDefManager.region));
	// physicsCollider
	modulePhysicsColliderClear();
	memoryManagerGlobalFree(memSingleListRegionStart(physColliderManager.region));
	// physicsSeparationPair
	modulePhysicsSeparationPairClear();
	memoryManagerGlobalFree(memPoolRegionStart(physSeparationPairManager.region));
	// physicsContactPair
	modulePhysicsContactPairClear();
	memoryManagerGlobalFree(memPoolRegionStart(physContactPairManager.region));
	// aabbNode
	modulePhysicsAABBNodeClear();
	memoryManagerGlobalFree(memPoolRegionStart(aabbNodeManager.region));
}


// Allocate memory for an AABB tree node and return a handle to it.
aabbNode *modulePhysicsAABBNodeAlloc(){
	return(memPoolAlloc(&aabbNodeManager));
}

// Free an AABB tree node that has been allocated.
void modulePhysicsAABBNodeFree(aabbNode *node){
	memPoolFree(&aabbNodeManager, node);
}

// Delete every AABB tree node in the manager.
void modulePhysicsAABBNodeClear(){
	MEMPOOL_LOOP_BEGIN(aabbNodeManager, i, aabbNode *)
		modulePhysicsAABBNodeFree(i);
	MEMPOOL_LOOP_END(aabbNodeManager, i, aabbNode *, return)
}


// Allocate memory for a contact
// pair and return a handle to it.
physicsContactPair *modulePhysicsContactPairAlloc(){
	return(memPoolAlloc(&physContactPairManager));
}

// Free a contact pair that has been allocated.
void modulePhysicsContactPairFree(physicsContactPair *cPair){
	physContactPairDelete(cPair);
	memPoolFree(&physContactPairManager, cPair);
}

// Delete every contact pair in the manager.
void modulePhysicsContactPairClear(){
	MEMPOOL_LOOP_BEGIN(physContactPairManager, i, physicsContactPair *)
		modulePhysicsContactPairFree(i);
	MEMPOOL_LOOP_END(physContactPairManager, i, physicsContactPair *, return)
}


// Allocate memory for a separation pair and return a handle to it.
physicsSeparationPair *modulePhysicsSeparationPairAlloc(){
	return(memPoolAlloc(&physSeparationPairManager));
}

// Free a separation pair that has been allocated.
void modulePhysicsSeparationPairFree(physicsSeparationPair *sPair){
	physSeparationPairDelete(sPair);
	memPoolFree(&physSeparationPairManager, sPair);
}

// Delete every separation pair in the manager.
void modulePhysicsSeparationPairClear(){
	MEMPOOL_LOOP_BEGIN(physSeparationPairManager, i, physicsSeparationPair *)
		modulePhysicsSeparationPairFree(i);
	MEMPOOL_LOOP_END(physSeparationPairManager, i, physicsSeparationPair *, return)
}


// Allocate a new collider array.
physicsCollider *modulePhysicsColliderAlloc(){
	return(memSingleListAlloc(&physColliderManager));
}

// Insert a collider at the beginning of an array.
physicsCollider *modulePhysicsColliderPrepend(physicsCollider **start){
	return(memSingleListPrepend(&physColliderManager, (void **)start));
}

// Insert a collider at the end of an array.
physicsCollider *modulePhysicsColliderAppend(physicsCollider **start){
	return(memSingleListAppend(&physColliderManager, (void **)start));
}

// Insert a collider after the element "prevData".
physicsCollider *modulePhysicsColliderInsertBefore(physicsCollider **start, physicsCollider *prevData){
	return(memSingleListInsertBefore(&physColliderManager, (void **)start, (void *)prevData));
}

// Insert a collider after the element "data".
physicsCollider *modulePhysicsColliderInsertAfter(physicsCollider **start, physicsCollider *data){
	return(memSingleListInsertAfter(&physColliderManager, (void **)start, (void *)data));
}

// Free a collider instance that has been allocated.
void modulePhysicsColliderFreeInstance(physicsCollider **start, physicsCollider *collider, physicsCollider *prevData){
	physColliderDeleteInstance(collider);
	memSingleListFree(&physColliderManager, (void **)start, (void *)collider, (void *)prevData);
}

// Free a collider that has been allocated.
void modulePhysicsColliderFree(physicsCollider **start, physicsCollider *collider, physicsCollider *prevData){
	physColliderDelete(collider);
	memSingleListFree(&physColliderManager, (void **)start, (void *)collider, (void *)prevData);
}

// Free an entire collider instance array.
void modulePhysicsColliderFreeInstanceArray(physicsCollider **start){
	physicsCollider *collider = *start;
	while(collider != NULL){
		modulePhysicsColliderFreeInstance(start, collider, NULL);
		collider = *start;
	}
}

// Free an entire collider array.
void modulePhysicsColliderFreeArray(physicsCollider **start){
	physicsCollider *collider = *start;
	while(collider != NULL){
		modulePhysicsColliderFree(start, collider, NULL);
		collider = *start;
	}
}

// Delete every collider in the manager.
void modulePhysicsColliderClear(){
	MEMSINGLELIST_LOOP_BEGIN(physColliderManager, i, physicsCollider *)
		modulePhysicsColliderFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(physColliderManager, i, physicsCollider *, return)
}


// Allocate a new rigid body base array.
physicsRigidBodyDef *modulePhysicsBodyDefAlloc(){
	return(memSingleListAlloc(&physRigidBodyDefManager));
}

// Insert a rigid body base at the beginning of an array.
physicsRigidBodyDef *modulePhysicsBodyDefPrepend(physicsRigidBodyDef **start){
	return(memSingleListPrepend(&physRigidBodyDefManager, (void **)start));
}

// Insert a rigid body base at the end of an array.
physicsRigidBodyDef *modulePhysicsBodyDefAppend(physicsRigidBodyDef **start){
	return(memSingleListAppend(&physRigidBodyDefManager, (void **)start));
}

// Insert a rigid body base after the element "prevData".
physicsRigidBodyDef *modulePhysicsBodyDefInsertBefore(physicsRigidBodyDef **start, physicsRigidBodyDef *prevData){
	return(memSingleListInsertBefore(&physRigidBodyDefManager, (void **)start, (void *)prevData));
}

// Insert a rigid body base after the element "data".
physicsRigidBodyDef *modulePhysicsBodyDefInsertAfter(physicsRigidBodyDef **start, physicsRigidBodyDef *data){
	return(memSingleListInsertAfter(&physRigidBodyDefManager, (void **)start, (void *)data));
}

// Free a rigid body base that has been allocated.
void modulePhysicsBodyDefFree(physicsRigidBodyDef **start, physicsRigidBodyDef *bodyDef, physicsRigidBodyDef *prevData){
	physRigidBodyDefDelete(bodyDef);
	memSingleListFree(&physRigidBodyDefManager, (void **)start, (void *)bodyDef, (void *)prevData);
}

// Free an entire rigid body base array.
void modulePhysicsBodyDefFreeArray(physicsRigidBodyDef **start){
	physicsRigidBodyDef *bodyDef = *start;
	while(bodyDef != NULL){
		modulePhysicsBodyDefFree(start, bodyDef, NULL);
		bodyDef = *start;
	}
}

// Delete every rigid body base in the manager.
void modulePhysicsBodyDefClear(){
	MEMSINGLELIST_LOOP_BEGIN(physRigidBodyDefManager, i, physicsRigidBodyDef *)
		modulePhysicsBodyDefFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(physRigidBodyDefManager, i, physicsRigidBodyDef *, return)
}


// Allocate a new rigid body array.
physicsRigidBody *modulePhysicsBodyAlloc(){
	return(memSingleListAlloc(&physRigidBodyManager));
}

// Insert a rigid body at the beginning of an array.
physicsRigidBody *modulePhysicsBodyPrepend(physicsRigidBody **start){
	return(memSingleListPrepend(&physRigidBodyManager, (void **)start));
}

// Insert a rigid body at the end of an array.
physicsRigidBody *modulePhysicsBodyAppend(physicsRigidBody **start){
	return(memSingleListAppend(&physRigidBodyManager, (void **)start));
}

// Insert a rigid body after the element "prevData".
physicsRigidBody *modulePhysicsBodyInsertBefore(physicsRigidBody **start, physicsRigidBody *prevData){
	return(memSingleListInsertBefore(&physRigidBodyManager, (void **)start, (void *)prevData));
}

// Insert a rigid body after the element "data".
physicsRigidBody *modulePhysicsBodyInsertAfter(physicsRigidBody **start, physicsRigidBody *data){
	return(memSingleListInsertAfter(&physRigidBodyManager, (void **)start, (void *)data));
}

// Free a rigid body that has been allocated.
void modulePhysicsBodyFree(physicsRigidBody **start, physicsRigidBody *body, physicsRigidBody *prevData){
	physRigidBodyDelete(body);
	memSingleListFree(&physRigidBodyManager, (void **)start, (void *)body, (void *)prevData);
}

// Free an entire rigid body array.
void modulePhysicsBodyFreeArray(physicsRigidBody **start){
	physicsRigidBody *body = *start;
	while(body != NULL){
		modulePhysicsBodyFree(start, body, NULL);
		body = *start;
	}
}

// Delete every rigid body in the manager.
void modulePhysicsBodyClear(){
	MEMSINGLELIST_LOOP_BEGIN(physRigidBodyManager, i, physicsRigidBody *)
		modulePhysicsBodyFree(NULL, i, NULL);
	MEMSINGLELIST_LOOP_END(physRigidBodyManager, i, physicsRigidBody *, return)
}