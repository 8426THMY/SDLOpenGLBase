#ifndef physicsJoint_h
#define physicsJoint_h


#include <stdint.h>


#include "settingsPhysics.h"

#include "physicsJointDistance.h"
#include "physicsJointFixed.h"
#include "physicsJointRevolute.h"
#include "physicsJointPrismatic.h"
#include "physicsJointSphere.h"


#define PHYSJOINT_NUM_TYPES 5

#ifndef PHYSCONSTRAINT_LINEAR_SLOP
	#define PHYSCONSTRAINT_LINEAR_SLOP 0.05f
#endif
#ifndef PHYSCONSTRAINT_MAX_LINEAR_CORRECTION
	#define PHYSCONSTRAINT_MAX_LINEAR_CORRECTION 0.2f
#endif


typedef uint_least8_t physJointType_t;
typedef uint_least8_t physJointBodyIndex_t;


typedef struct physicsJointDef {
	// This array should be large enough
	// to store any type of joint.
	union {
		physicsJointDistanceDef distance;
		physicsJointFixedDef fixed;
		physicsJointRevoluteDef revolute;
		physicsJointPrismaticDef prismatic;
		physicsJointSphereDef sphere;
	} data;
	// Stores which type of
	// joint this object is.
	physJointType_t type;

	// The objects that store these joints should have
	// an array of rigid bodies. These indicate which
	// of those bodies are associated with this joint.
	physJointBodyIndex_t indexA;
	physJointBodyIndex_t indexB;
} physicsJointDef;

typedef struct physicsJoint {
	// This array should be large enough
	// to store any type of joint.
	union {
		physicsJointDistance distance;
		physicsJointFixed fixed;
		physicsJointRevolute revolute;
		physicsJointPrismatic prismatic;
		physicsJointSphere sphere;
	} data;
	// Stores which type of
	// joint this object is.
	physJointType_t type;
} physicsJoint;


typedef struct physicsRigidBody physicsRigidBody;

void physJointPresolve(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt);
void physJointSolveVelocity(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
void physJointSolvePosition(physicsJoint *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


extern void (*physJointPresolveTable[PHYSJOINT_NUM_TYPES])(
	void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt
);
extern void (*physJointSolveVelocityTable[PHYSJOINT_NUM_TYPES])(
	void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB
);
extern void (*physJointSolvePositionTable[PHYSJOINT_NUM_TYPES])(
	void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB
);


#endif