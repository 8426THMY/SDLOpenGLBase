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
#define MODULE_PHYSICS_SETUP_FAIL 5

#define MODULE_AABBNODE_ELEMENT_SIZE           sizeof(aabbNode)
#define MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE    sizeof(physicsContactPair)
#define MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE sizeof(physicsSeparationPair)
#define MODULE_PHYSJOINTPAIR_ELEMENT_SIZE      sizeof(physicsJointPair)
#define MODULE_PHYSCOLLIDER_ELEMENT_SIZE       sizeof(physicsCollider)
#define MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE   sizeof(physicsRigidBodyDef)
#define MODULE_PHYSRIGIDBODY_ELEMENT_SIZE      sizeof(physicsRigidBody)

#ifndef MEMORY_MODULE_NUM_AABBNODES
	#define MEMORY_MODULE_NUM_AABBNODES 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSCONTACTPAIRS
	#define MEMORY_MODULE_NUM_PHYSCONTACTPAIRS 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS
	#define MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS 1
#endif
#ifndef MEMORY_MODULE_NUM_PHYSJOINTPAIRS
	#define MEMORY_MODULE_NUM_PHYSJOINTPAIRS 1
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

#define MODULE_AABBNODE_MANAGER_SIZE           memPoolMemoryForBlocks(MEMORY_MODULE_NUM_AABBNODES, MODULE_AABBNODE_ELEMENT_SIZE)
#define MODULE_PHYSCONTACTPAIR_MANAGER_SIZE    memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCONTACTPAIRS, MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE)
#define MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS, MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE)
#define MODULE_PHYSJOINTPAIR_MANAGER_SIZE      memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSJOINTPAIRS, MODULE_PHYSJOINTPAIR_ELEMENT_SIZE)
#define MODULE_PHYSCOLLIDER_MANAGER_SIZE       memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCOLLIDERS, MODULE_PHYSCOLLIDER_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE   memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS, MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODY_MANAGER_SIZE      memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODIES, MODULE_PHYSRIGIDBODY_ELEMENT_SIZE)


return_t modulePhysicsSetup();
void modulePhysicsCleanup();

aabbNode *modulePhysicsAABBNodeAlloc();
void modulePhysicsAABBNodeFree(aabbNode *const restrict node);
void modulePhysicsAABBNodeClear();

physicsContactPair *modulePhysicsContactPairAlloc();
physicsContactPair *modulePhysicsContactPairPrepend(physicsContactPair **const restrict start);
physicsContactPair *modulePhysicsContactPairAppend(physicsContactPair **const restrict start);
physicsContactPair *modulePhysicsContactPairInsertBefore(physicsContactPair **const restrict start, physicsContactPair *const restrict next);
physicsContactPair *modulePhysicsContactPairInsertAfter(physicsContactPair **const restrict start, physicsContactPair *const restrict prev);
physicsContactPair *modulePhysicsContactPairNext(const physicsContactPair *const restrict cPair);
physicsContactPair *modulePhysicsContactPairPrev(const physicsContactPair *const restrict cPair);
void modulePhysicsContactPairFree(physicsContactPair **const restrict start, physicsContactPair *const restrict cPair);
void modulePhysicsContactPairFreeArray(physicsContactPair **const restrict start);
void modulePhysicsContactPairClear();

physicsSeparationPair *modulePhysicsSeparationPairAlloc();
physicsSeparationPair *modulePhysicsSeparationPairPrepend(physicsSeparationPair **const restrict start);
physicsSeparationPair *modulePhysicsSeparationPairAppend(physicsSeparationPair **const restrict start);
physicsSeparationPair *modulePhysicsSeparationPairInsertBefore(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict next);
physicsSeparationPair *modulePhysicsSeparationPairInsertAfter(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict prev);
physicsSeparationPair *modulePhysicsSeparationPairNext(const physicsSeparationPair *const restrict sPair);
physicsSeparationPair *modulePhysicsSeparationPairPrev(const physicsSeparationPair *const restrict sPair);
void modulePhysicsSeparationPairFree(physicsSeparationPair **const restrict start, physicsSeparationPair *const restrict sPair);
void modulePhysicsSeparationPairFreeArray(physicsSeparationPair **const restrict start);
void modulePhysicsSeparationPairClear();

physicsJointPair *modulePhysicsJointPairAlloc();
physicsJointPair *modulePhysicsJointPairPrepend(physicsJointPair **const restrict start);
physicsJointPair *modulePhysicsJointPairAppend(physicsJointPair **const restrict start);
physicsJointPair *modulePhysicsJointPairInsertBefore(physicsJointPair **const restrict start, physicsJointPair *const restrict next);
physicsJointPair *modulePhysicsJointPairInsertAfter(physicsJointPair **const restrict start, physicsJointPair *const restrict prev);
physicsJointPair *modulePhysicsJointPairNext(const physicsJointPair *const restrict sPair);
physicsJointPair *modulePhysicsJointPairPrev(const physicsJointPair *const restrict sPair);
void modulePhysicsJointPairFree(physicsJointPair **const restrict start, physicsJointPair *const restrict sPair);
void modulePhysicsJointPairFreeArray(physicsJointPair **const restrict start);
void modulePhysicsJointPairClear();

physicsCollider *modulePhysicsColliderAlloc();
physicsCollider *modulePhysicsColliderPrepend(physicsCollider **const restrict start);
physicsCollider *modulePhysicsColliderAppend(physicsCollider **const restrict start);
physicsCollider *modulePhysicsColliderInsertAfter(physicsCollider **const restrict start, physicsCollider *const restrict prev);
physicsCollider *modulePhysicsColliderNext(const physicsCollider *const restrict collider);
void modulePhysicsColliderFreeInstance(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prev);
void modulePhysicsColliderFree(physicsCollider **const restrict start, physicsCollider *const restrict collider, physicsCollider *const restrict prev);
void modulePhysicsColliderFreeInstanceArray(physicsCollider **const restrict start);
void modulePhysicsColliderFreeArray(physicsCollider **const restrict start);
void modulePhysicsColliderClear();

physicsRigidBodyDef *modulePhysicsBodyDefAlloc();
physicsRigidBodyDef *modulePhysicsBodyDefPrepend(physicsRigidBodyDef **const restrict start);
physicsRigidBodyDef *modulePhysicsBodyDefAppend(physicsRigidBodyDef **const restrict start);
physicsRigidBodyDef *modulePhysicsBodyDefInsertAfter(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict prev);
physicsRigidBodyDef *modulePhysicsBodyDefNext(const physicsRigidBodyDef *const restrict bodyDef);
void modulePhysicsBodyDefFree(physicsRigidBodyDef **const restrict start, physicsRigidBodyDef *const restrict bodyDef, physicsRigidBodyDef *const restrict prev);
void modulePhysicsBodyDefFreeArray(physicsRigidBodyDef **const restrict start);
void modulePhysicsBodyDefClear();

physicsRigidBody *modulePhysicsBodyAlloc();
physicsRigidBody *modulePhysicsBodyPrepend(physicsRigidBody **const restrict start);
physicsRigidBody *modulePhysicsBodyAppend(physicsRigidBody **const restrict start);
physicsRigidBody *modulePhysicsBodyInsertBefore(physicsRigidBody **const restrict start, physicsRigidBody *const restrict next);
physicsRigidBody *modulePhysicsBodyInsertAfter(physicsRigidBody **const restrict start, physicsRigidBody *const restrict prev);
physicsRigidBody *modulePhysicsBodyNext(const physicsRigidBody *const restrict body);
physicsRigidBody *modulePhysicsBodyPrev(const physicsRigidBody *const restrict body);
void modulePhysicsBodyFree(physicsRigidBody **const restrict start, physicsRigidBody *const restrict body);
void modulePhysicsBodyFreeArray(physicsRigidBody **const restrict start);
void modulePhysicsBodyClear();


extern memoryPool g_aabbNodeManager;
extern memoryDoubleList g_physContactPairManager;
extern memoryDoubleList g_physSeparationPairManager;
extern memoryDoubleList g_physJointPairManager;
extern memorySingleList g_physColliderManager;
extern memorySingleList g_physRigidBodyDefManager;
extern memoryDoubleList g_physRigidBodyManager;


#endif