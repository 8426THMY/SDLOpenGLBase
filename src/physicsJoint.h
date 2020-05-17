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

#if \
	defined(PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL) || defined(PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL) ||     \
	defined(PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL) || defined(PHYSJOINTPRISMATIC_STABILISER_GAUSS_SEIDEL) || \
	defined(PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL)

	#define PHYSJOINT_USE_POSITIONAL_CORRECTION
#endif


typedef uint_least8_t physJointType_t;

#warning "We might be able to remove this."
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

	// Rigid body A's address in memory is always
	// guaranteed to be greater than rigid body B's.
	physicsRigidBody *bodyA;
	physicsRigidBody *bodyB;
} physicsJoint;


typedef struct physicsRigidBody physicsRigidBody;

void physJointPresolve(
	physicsJoint *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
void physJointSolveVelocity(physicsJoint *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);
void physJointSolvePosition(physicsJoint *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB);


extern void (*physJointPresolveTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
extern void (*physJointSolveVelocityTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
extern float (*physJointSolvePositionTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);


#endif