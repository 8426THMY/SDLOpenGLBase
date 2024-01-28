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
	memPoolMemoryForBlocks(MEMORY_MODULE_NUM_AABBNODES, sizeof(aabbNode))
#define MODULE_PHYSCONTACTPAIR_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCONTACTPAIRS, sizeof(physicsContactPair))
#define MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSSEPARATIONPAIRS, sizeof(physicsSeparationPair))
#define MODULE_PHYSJOINT_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSJOINTS, sizeof(physicsJoint))
#define MODULE_PHYSCOLLIDER_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSCOLLIDERS, sizeof(physicsCollider))
#define MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE \
	memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODYDEFS, sizeof(physicsRigidBodyDef))
#define MODULE_PHYSRIGIDBODY_MANAGER_SIZE \
	memDoubleListMemoryForBlocks(MEMORY_MODULE_NUM_PHYSRIGIDBODIES, sizeof(physicsRigidBody))


// aabbNode
moduleDeclarePool(PhysicsAABBNode, aabbNode, g_aabbNodeManager)
moduleDeclarePoolFree(PhysicsAABBNode, aabbNode)
// physicsContactPair
moduleDeclareDoubleList(PhysicsContactPair, physicsContactPair, g_physContactPairManager)
moduleDeclareDoubleListFree(PhysicsContactPair, physicsContactPair)
// physicsSeparationPair
moduleDeclareDoubleList(PhysicsSeparationPair, physicsSeparationPair, g_physSeparationPairManager)
moduleDeclareDoubleListFree(PhysicsSeparationPair, physicsSeparationPair)
// physicsJoint
moduleDeclareDoubleList(PhysicsJoint, physicsJoint, g_physJointManager)
moduleDeclareDoubleListFree(PhysicsJoint, physicsJoint)
// physicsCollider
moduleDeclareSingleList(PhysicsCollider, physicsCollider, g_physColliderManager)
moduleDeclareSingleListFree(PhysicsColliderBase, physicsCollider)
moduleDeclareSingleListFree(PhysicsColliderInstance, physicsCollider)
// physicsRigidBodyDef
moduleDeclareSingleList(PhysicsRigidBodyDef, physicsRigidBodyDef, g_physRigidBodyDefManager)
moduleDeclareSingleListFree(PhysicsRigidBodyDef, physicsRigidBodyDef)
// physicsRigidBody
moduleDeclareDoubleList(PhysicsRigidBody, physicsRigidBody, g_physRigidBodyManager)
moduleDeclareDoubleListFree(PhysicsRigidBody, physicsRigidBody)

return_t modulePhysicsSetup();
void modulePhysicsCleanup();


#endif