#include "modulePhysics.h"


#include "memoryManager.h"


// aabbNode
moduleDefinePool(PhysicsAABBNode, aabbNode, g_aabbNodeManager, MODULE_AABBNODE_MANAGER_SIZE)
moduleDefinePoolFree(PhysicsAABBNode, aabbNode, g_aabbNodeManager)
// physicsContactPair
moduleDefineDoubleList(
	PhysicsContactPair, physicsContactPair,
	g_physContactPairManager, MODULE_PHYSCONTACTPAIR_MANAGER_SIZE
)
moduleDefineDoubleListFreeFlexible(
	PhysicsContactPair, physicsContactPair, g_physContactPairManager, physContactPairDelete
)
// physicsSeparationPair
moduleDefineDoubleList(
	PhysicsSeparationPair, physicsSeparationPair,
	g_physSeparationPairManager, MODULE_PHYSSEPARATIONPAIR_MANAGER_SIZE
)
moduleDefineDoubleListFreeFlexible(
	PhysicsSeparationPair, physicsSeparationPair,
	g_physSeparationPairManager, physSeparationPairDelete
)
// physicsJoint
moduleDefineDoubleList(
	PhysicsJoint, physicsJoint, g_physJointManager, MODULE_PHYSJOINT_MANAGER_SIZE
)
moduleDefineDoubleListFreeFlexible(
	PhysicsJoint, physicsJoint, g_physJointManager, physJointDelete
)
// physicsCollider
moduleDefineSingleList(
	PhysicsCollider, physicsCollider, g_physColliderManager, MODULE_PHYSCOLLIDER_MANAGER_SIZE
)
moduleDefineSingleListFreeFlexible(
	PhysicsColliderBase, physicsCollider, g_physColliderManager, physColliderDeleteBase
)
moduleDefineSingleListFreeFlexible(
	PhysicsColliderInstance, physicsCollider, g_physColliderManager, physColliderDeleteInstance
)
// physicsRigidBodyDef
moduleDefineSingleList(
	PhysicsRigidBodyDef, physicsRigidBodyDef,
	g_physRigidBodyDefManager, MODULE_PHYSRIGIDBODYDEF_MANAGER_SIZE
)
moduleDefineSingleListFreeFlexible(
	PhysicsRigidBodyDef, physicsRigidBodyDef,
	g_physRigidBodyDefManager, physRigidBodyDefDelete
)
// physicsRigidBody
moduleDefineDoubleList(
	PhysicsRigidBody, physicsRigidBody,
	g_physRigidBodyManager, MODULE_PHYSRIGIDBODY_MANAGER_SIZE
)
moduleDefineDoubleListFreeFlexible(
	PhysicsRigidBody, physicsRigidBody,
	g_physRigidBodyManager, physRigidBodyDelete
)


return_t modulePhysicsSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our managers.
	return(
		modulePhysicsAABBNodeInit()       &&
		modulePhysicsContactPairInit()    &&
		modulePhysicsSeparationPairInit() &&
		modulePhysicsJointInit()          &&
		modulePhysicsColliderInit()       &&
		modulePhysicsRigidBodyDefInit()   &&
		modulePhysicsRigidBodyInit()
	);
}

void modulePhysicsCleanup(){
	modulePhysicsRigidBodyDelete();
	modulePhysicsRigidBodyDefDelete();
	// Note that deleting the rigid bodies deletes
	// all collider instances, so it's fine to assume
	// the remaining physics colliders are all bases.
	modulePhysicsColliderBaseDelete();
	modulePhysicsJointDelete();
	modulePhysicsSeparationPairDelete();
	modulePhysicsContactPairDelete();
	modulePhysicsAABBNodeDelete();
}