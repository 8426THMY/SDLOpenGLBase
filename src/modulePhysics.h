#ifndef modulePhysics_h
#define modulePhysics_h


#include "aabbTree.h"
#include "physicsJoint.h"
#include "physicsContact.h"
#include "physicsCollider.h"
#include "physicsRigidBody.h"

#include "utilTypes.h"
#include "moduleShared.h"


#define MODULE_PHYSICS
#define MODULE_PHYSICS_SETUP_FAIL 5

#define MODULE_AABBNODE_ELEMENT_SIZE           sizeof(aabbNode)
#define MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE    sizeof(physicsContactPair)
#define MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE sizeof(physicsSeparationPair)
#define MODULE_PHYSJOINT_ELEMENT_SIZE          sizeof(physicsJoint)
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
#ifndef MEMORY_MODULE_NUM_PHYSJOINTS
	#define MEMORY_MODULE_NUM_PHYSJOINTS 1
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

#define MODULE_AABBNODE_MANAGER_SIZE \
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_AABBNODES, MODULE_AABBNODE_ELEMENT_SIZE)
#define MODULE_PHYSCONTACTPAIR_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCONTACTPAIRS, MODULE_PHYSCONTACTPAIR_ELEMENT_SIZE)
#define MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS, MODULE_PHYSSEPARATIONPAIR_ELEMENT_SIZE)
#define MODULE_PHYSJOINT_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSJOINTS, MODULE_PHYSJOINT_ELEMENT_SIZE)
#define MODULE_PHYSCOLLIDER_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCOLLIDERS, MODULE_PHYSCOLLIDER_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS, MODULE_PHYSRIGIDBODYDEF_ELEMENT_SIZE)
#define MODULE_PHYSRIGIDBODY_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODIES, MODULE_PHYSRIGIDBODY_ELEMENT_SIZE)


moduleDeclarePool(PhysicsAABBNode, aabbNode, g_aabbNodeManager)
moduleDeclareDoubleList(PhysicsContactPair, physicsContactPair, g_physContactPairManager)
moduleDeclareDoubleList(PhysicsSeparationPair, physicsSeparationPair, g_physSeparationPairManager)
moduleDeclareDoubleList(PhysicsJoint, physicsJoint, g_physJointManager)
moduleDeclareSingleList(PhysicsCollider, physicsCollider, g_physColliderManager)
moduleDeclareSingleList(PhysicsRigidBodyDef, physicsRigidBodyDef, g_physRigidBodyDefManager)
moduleDeclareDoubleList(PhysicsRigidBody, physicsRigidBody, g_physRigidBodyManager)
void modulePhysicsColliderFreeInstance(
	physicsCollider **const restrict start,
	physicsCollider *const restrict collider,
	physicsCollider *const restrict prev
);
void modulePhysicsColliderFreeInstanceArray(physicsCollider **const restrict start);

return_t modulePhysicsSetup();
void modulePhysicsCleanup();


#endif