#ifndef modulePhysics_h
#define modulePhysics_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"
#include "memorySingleList.h"

#include "aabbTree.h"
#include "physicsContactPair.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"


#define MODULE_PHYSICS
#define MODULE_PHYSICS_SETUP_FAIL 4

#define MODULE_PHYSCONTACTPAIRS_ELEMENT_SIZE    sizeof(physicsContactPair)
#define MODULE_PHYSSEPARATIONPAIRS_ELEMENT_SIZE sizeof(physicsSeparationPair)
#define MODULE_AABBNODES_ELEMENT_SIZE           sizeof(aabbNode)
#define MODULE_PHYSCOLLIDER_ELEMENT_SIZE        sizeof(physicsCollider)
#define MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE    sizeof(physicsRigidBodyDef)
#define MODULE_PHYSRIGIDBODY_ELEMENT_SIZE       sizeof(physicsRigidBody)

#ifndef MEMORY_MODULE_NUM_PHYSCONTACTPAIRS
	#define MEMORY_MODULE_NUM_PHYSCONTACTPAIRS 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS
	#define MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS 1
#endif
#ifndef MEMORY_MODULE_NUM_AABBNODES
	#define MEMORY_MODULE_NUM_AABBNODES 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSCOLLIDERS
	#define MEMORY_MODULE_NUM_PHYSCOLLIDERS 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS
	#define MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSRIGIDBODIES
	#define MEMORY_MODULE_NUM_PHYSRIGIDBODIES 1
#endif

#define MODULE_PHYSCONTACTPAIRS_MANAGER_SIZE    memPoolMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCONTACTPAIRS, MODULE_PHYSCONTACTPAIRS_ELEMENT_SIZE)
#define MODULE_PHYSSEPARATIONPAIRS_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS, MODULE_PHYSSEPARATIONPAIRS_ELEMENT_SIZE)
#define MODULE_AABBNODES_MANAGER_SIZE           memPoolMemoryForBlocks(MEMORY_MODULE_NUM_AABBNODES, MODULE_AABBNODES_ELEMENT_SIZE)
#define MODULE_PHYSCOLLIDER_MANAGER_SIZE        memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCOLLIDERS, MODULE_PHYSCOLLIDER_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE    memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS, MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODY_MANAGER_SIZE       memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODIES, MODULE_PHYSRIGIDBODY_ELEMENT_SIZE)


return_t modulePhysicsSetup();
void modulePhysicsCleanup();

aabbNode *modulePhysicsAABBNodeAlloc();
void modulePhysicsAABBNodeFree(aabbNode *node);
void modulePhysicsAABBNodeClear();

physicsContactPair *modulePhysicsContactPairAlloc();
void modulePhysicsContactPairFree(physicsContactPair *cPair);
void modulePhysicsSeparationPairClear();

physicsSeparationPair *modulePhysicsSeparationPairAlloc();
void modulePhysicsSeparationPairFree(physicsSeparationPair *sPair);
void modulePhysicsSeparationPairClear();

physicsCollider *modulePhysicsColliderAlloc();
physicsCollider *modulePhysicsColliderPrepend(physicsCollider **start);
physicsCollider *modulePhysicsColliderAppend(physicsCollider **start);
physicsCollider *modulePhysicsColliderInsertBefore(physicsCollider **start, physicsCollider *prevData);
physicsCollider *modulePhysicsColliderInsertAfter(physicsCollider **start, physicsCollider *data);
void modulePhysicsColliderFree(physicsCollider **start, physicsCollider *collider, physicsCollider *prevData);
void modulePhysicsColliderFreeArray(physicsCollider **start);
void modulePhysicsColliderClear();

physicsRigidBodyDef *modulePhysicsBodyDefAlloc();
physicsRigidBodyDef *modulePhysicsBodyDefPrepend(physicsRigidBodyDef **start);
physicsRigidBodyDef *modulePhysicsBodyDefAppend(physicsRigidBodyDef **start);
physicsRigidBodyDef *modulePhysicsBodyDefInsertBefore(physicsRigidBodyDef **start, physicsRigidBodyDef *prevData);
physicsRigidBodyDef *modulePhysicsBodyDefInsertAfter(physicsRigidBodyDef **start, physicsRigidBodyDef *data);
void modulePhysicsBodyDefFree(physicsRigidBodyDef **start, physicsRigidBodyDef *bodyDef, physicsRigidBodyDef *prevData);
void modulePhysicsBodyDefFreeArray(physicsRigidBodyDef **start);
void modulePhysicsBodyDefClear();

physicsRigidBody *modulePhysicsBodyAlloc();
physicsRigidBody *modulePhysicsBodyPrepend(physicsRigidBody **start);
physicsRigidBody *modulePhysicsBodyAppend(physicsRigidBody **start);
physicsRigidBody *modulePhysicsBodyInsertBefore(physicsRigidBody **start, physicsRigidBody *prevData);
physicsRigidBody *modulePhysicsBodyInsertAfter(physicsRigidBody **start, physicsRigidBody *data);
void modulePhysicsBodyFree(physicsRigidBody **start, physicsRigidBody *body, physicsRigidBody *prevData);
void modulePhysicsBodyFreeArray(physicsRigidBody **start);
void modulePhysicsBodyClear();


extern memoryPool aabbNodeManager;
extern memoryPool physContactPairManager;
extern memoryPool physSeparationPairManager;
extern memorySingleList physColliderManager;
extern memorySingleList physRigidBodyDefManager;
extern memorySingleList physRigidBodyManager;


#endif