#ifndef modulePhysics_h
#define modulePhysics_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memoryPool.h"
#include "memorySingleList.h"
#include "memoryDoubleList.h"

#include "aabbTree.h"
#include "physicsConstraintPair.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"


#define MODULE_PHYSICS
#define MODULE_PHYSICS_SETUP_FAIL 4

#define MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE    sizeof(physicsContactPair)
#define MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE sizeof(physicsSeparationPair)
#define MODULE_AABBNODE_ELEMENT_SIZE           sizeof(aabbNode)
#define MODULE_PHYSCOLLIDER_ELEMENT_SIZE        sizeof(physicsCollider)
#define MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE    sizeof(physicsRigidBodyDef)
#define MODULE_PHYSRIGIDBODY_ELEMENT_SIZE       sizeof(physicsRigidBody)

#ifndef MEMORY_MODULE_NUM_PHYSCONTACTPAIR
	#define MEMORY_MODULE_NUM_PHYSCONTACTPAIR 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSSEPARATIONPAIR
	#define MEMORY_MODULE_NUM_PHYSSEPARATIONPAIR 1
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

#define MODULE_PHYSCONTACTPAIR_MANAGER_SIZE    memPoolMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCONTACTPAIR, MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE)
#define MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE memPoolMemoryForBlocks(MEMORY_MODULE_NUM_PHYSSEPARATIONPAIR, MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE)
#define MODULE_AABBNODE_MANAGER_SIZE           memPoolMemoryForBlocks(MEMORY_MODULE_NUM_AABBNODES, MODULE_AABBNODE_ELEMENT_SIZE)
#define MODULE_PHYSCOLLIDER_MANAGER_SIZE        memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCOLLIDERS, MODULE_PHYSCOLLIDER_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE    memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS, MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODY_MANAGER_SIZE       memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODIES, MODULE_PHYSRIGIDBODY_ELEMENT_SIZE)


return_t modulePhysicsSetup();
void modulePhysicsCleanup();

aabbNode *modulePhysicsAABBNodeAlloc();
void modulePhysicsAABBNodeFree(aabbNode *const restrict node);
void modulePhysicsAABBNodeClear();

physicsContactPair *modulePhysicsContactPairAlloc();
physicsContactPair *modulePhysicsContactPairPrepend(physicsContactPair **const restrict start);
physicsContactPair *modulePhysicsContactPairAppend(physicsContactPair **const restrict start);
physicsContactPair *modulePhysicsContactPairInsertBefore(physicsContactPair **const restrict start, physicsContactPair *const restrict prevData);
physicsContactPair *modulePhysicsContactPairInsertAfter(physicsContactPair **const restrict start, physicsContactPair *const restrict data);
physicsContactPair *modulePhysicsContactPairNext(const physicsContactPair *const restrict cPair);
physicsContactPair *modulePhysicsContactPairPrev(const physicsContactPair *const restrict cPair);
void modulePhysicsContactPairFree(physicsContactPair **const restrict start, physicsContactPair *const restrict cPair);
void modulePhysicsContactPairFreeArray(physicsContactPair **const restrict start);
void modulePhysicsContactPairClear();

physicsSeparationPair *modulePhysicsSeparationPairAlloc();
physicsSeparationPair *modulePhysicsSeparationPairPrepend(physicsSeparationPair **const restrict start);
physicsSeparationPair *modulePhysicsSeparationPairAppend(physicsSeparationPair **const restrict start);
physicsSeparationPair *modulePhysicsSeparationPairInsertBefore(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict prevData);
physicsSeparationPair *modulePhysicsSeparationPairInsertAfter(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict data);
physicsSeparationPair *modulePhysicsSeparationPairNext(const physicsSeparationPair *const restrict sPair);
physicsSeparationPair *modulePhysicsSeparationPairPrev(const physicsSeparationPair *const restrict sPair);
void modulePhysicsSeparationPairFree(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict sPair);
void modulePhysicsSeparationPairFreeArray(physicsSeparationPair **const restrict start);
void modulePhysicsSeparationPairClear();

physicsCollider *modulePhysicsColliderAlloc();
physicsCollider *modulePhysicsColliderPrepend(physicsCollider **const restrict start);
physicsCollider *modulePhysicsColliderAppend(physicsCollider **const restrict start);
physicsCollider *modulePhysicsColliderInsertBefore(physicsCollider **const restrict start, physicsCollider *const restrict prevData);
physicsCollider *modulePhysicsColliderInsertAfter(physicsCollider **const restrict start, physicsCollider *const restrict data);
physicsCollider *modulePhysicsColliderNext(const physicsCollider *const restrict collider);
void modulePhysicsColliderFreeInstance(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prevData);
void modulePhysicsColliderFree(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prevData);
void modulePhysicsColliderFreeInstanceArray(physicsCollider **const restrict start);
void modulePhysicsColliderFreeArray(physicsCollider **const restrict start);
void modulePhysicsColliderClear();

physicsRigidBodyDef *modulePhysicsBodyDefAlloc();
physicsRigidBodyDef *modulePhysicsBodyDefPrepend(physicsRigidBodyDef **const restrict start);
physicsRigidBodyDef *modulePhysicsBodyDefAppend(physicsRigidBodyDef **const restrict start);
physicsRigidBodyDef *modulePhysicsBodyDefInsertBefore(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict prevData);
physicsRigidBodyDef *modulePhysicsBodyDefInsertAfter(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict data);
physicsRigidBodyDef *modulePhysicsBodyDefNext(const physicsRigidBodyDef *const restrict bodyDef);
void modulePhysicsBodyDefFree(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict bodyDef, physicsRigidBodyDef *const restrict prevData);
void modulePhysicsBodyDefFreeArray(physicsRigidBodyDef **const restrict start);
void modulePhysicsBodyDefClear();

physicsRigidBody *modulePhysicsBodyAlloc();
physicsRigidBody *modulePhysicsBodyPrepend(physicsRigidBody **const restrict start);
physicsRigidBody *modulePhysicsBodyAppend(physicsRigidBody **const restrict start);
physicsRigidBody *modulePhysicsBodyInsertBefore(physicsRigidBody **const restrict start, physicsRigidBody *const restrict prevData);
physicsRigidBody *modulePhysicsBodyInsertAfter(physicsRigidBody **const restrict start, physicsRigidBody *const restrict data);
physicsRigidBody *modulePhysicsBodyNext(const physicsRigidBody *const restrict body);
physicsRigidBody *modulePhysicsBodyPrev(const physicsRigidBody *const restrict body);
void modulePhysicsBodyFree(physicsRigidBody **const restrict start, physicsRigidBody *const restrict body);
void modulePhysicsBodyFreeArray(physicsRigidBody **const restrict start);
void modulePhysicsBodyClear();


extern memoryPool g_aabbNodeManager;
extern memoryDoubleList g_physContactPairManager;
extern memoryDoubleList g_physSeparationPairManager;
extern memorySingleList g_physColliderManager;
extern memorySingleList g_physRigidBodyDefManager;
extern memoryDoubleList g_physRigidBodyManager;


#endif