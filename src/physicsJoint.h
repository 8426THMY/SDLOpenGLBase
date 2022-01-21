#ifndef physicsJoint_h
#define physicsJoint_h


#include <stdint.h>

#include "settingsPhysics.h"

#include "utilTypes.h"

#include "physicsJointDistance.h"
#include "physicsJointFixed.h"
#include "physicsJointRevolute.h"
#include "physicsJointPrismatic.h"
#include "physicsJointSphere.h"


#define PHYSJOINT_NUM_TYPES 5

#ifndef PHYSJOINT_LINEAR_SLOP
	#define PHYSJOINT_LINEAR_SLOP 0.05f
#endif
#ifndef PHYSJOINT_MAX_LINEAR_CORRECTION
	#define PHYSJOINT_MAX_LINEAR_CORRECTION 0.2f
#endif
#ifndef PHYSJOINT_LINEAR_POSITIONAL_ERROR_THRESHOLD
	#define PHYSJOINT_LINEAR_POSITIONAL_ERROR_THRESHOLD (3.f * PHYSJOINT_LINEAR_SLOP)
#endif
#ifndef PHYSJOINT_ANGULAR_SLOP
	#define PHYSJOINT_ANGULAR_SLOP 0.005f
#endif
#ifndef PHYSJOINT_MAX_ANGULAR_CORRECTION
	#define PHYSJOINT_MAX_ANGULAR_CORRECTION 0.02f
#endif
#ifndef PHYSJOINT_ANGULAR_POSITIONAL_ERROR_THRESHOLD
	#define PHYSJOINT_ANGULAR_POSITIONAL_ERROR_THRESHOLD (3.f * PHYSJOINT_ANGULAR_SLOP)
#endif

#if \
	defined(PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL) || defined(PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL) ||     \
	defined(PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL) || defined(PHYSJOINTPRISMATIC_STABILISER_GAUSS_SEIDEL) || \
	defined(PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL)

	#define PHYSJOINT_USE_POSITIONAL_CORRECTION
#endif


/*
** Constructing a Physics Constraint
** ---------------------------------
** Suppose we want to construct a new type of joint or constraint for our
** physics system. In our engine, we only require that three functions be
** defined in order to use the joint. These functions are:
**
**     1. Presolve;
**     2. Solve velocity;
**     3. Solve position.
**
** In order to design and solve new constraints, we use the following
** general process. This also summarizes our velocity solving step.
** Note that this process is for a single equality constraint, but in
** general we can apply it to cases where we have many constraints,
** that could be modelled using both equalities or inequalities.
**
**     1. Construct an equality, denoted C, that describes the constraint
**        on the rigid bodies' positions, pA, pB, and orientations, qA, qB.
**
**     2. Differentiate with respect to time to get a constraint C' on the
**        rigid bodies' velocities, vA, vB, and angular velocities, wA, wB.
**
**     3. We model velocity constraints as C' : Jv + b = 0, where J is
**        a 1x12 Jacobian matrix, v = (v_A, w_A, v_B, w_B)^T is the 12x1
**        velocity vector, and b is a bias term used to prevent jittering.
**
**     4. Supposing the constraint is not satisfied, we must solve for the
**        change in the velocity vector, Dv, such that J(v_i + dv) + b = 0.
**        By Newton's second law, we have that F = Mv'. Moving the mass
**        matrix to the left and integrating over our timestep, we get that
**            M^{-1}P = M^{-1}F(t_f - t_i) = v_f - v_i = dv,
**        where P is the constraint impulse. Because our constraints may not
**        do work, we know that our final state must satisfy
**            0 = dW = F^T v_f (t_f - t_i) = P^T v_f.
**        Because Jv_f = 0 in order for the constraint to be satisfied, this
**        suggests that P = lambda J^T, so dv = lambda M^{-1}J^T. At last,
**        plugging this into our constraint equation yields the solution
**            0 = Jv_i + lambda JM^{-1}J^T + b,
**            => lambda = -(Jv_i + b)/(JM^{-1}J^T).
**
**     5. For the bias term, we will typically use b = B(C/dt), where C is
**        the value of our positional constraint (which is non-zero if the
**        constraint is not satisfied) and B is some Baumgarte constant.
**
**     6. Our velocity solving step simply computes this change in velocity,
**        dv = lambda M^{-1}J^T, and adds it to the rigid bodies' velocities.
**
** For inequality constraints, we check that the constraint is not already
** satisfied before applying the impulse.
**
** Note that we also keep track of the total impulse we've applied over the
** course of the constraint's existence. At the beginning of each update,
** our presolving step will compute all of the constraint data, such as the
** effective mass, then apply this accumulated impulse to the rigid bodies
** in a process known as warmstarting. This initial guess typically helps
** the system to more quickly converge to a solution.
**
** The final step is the position solving step. Suppose the positional
** contraint is not satisfied. We would like to find the change in position,
** dp, such that C(p_i + dp) = 0. Several variable calculus tells us that
** the Jacobian is the best linear approximation, so
**
**     0 = C(p_i + dp) ~= C(p_i) + J dp.
**
** This change in position must satisfy our velocity inequality in order
** to not add energy to the system, as it simply differs from dv by a factor
** of the timestep, dt. Therefore, dp = lambda M^{-1}J^T, which gives
** lambda JM^{-1}J^T = -C(p_i). From here, it is easy to solve for lambda
** and hence dp. We solve this iteratively using a Gauss-Seidel solver.
*/


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
	physicsJoint *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB,
	const float dt
);
void physJointSolveVelocity(
	physicsJoint *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#ifdef PHYSJOINT_USE_POSITIONAL_CORRECTION
return_t physJointSolvePosition(
	const physicsJoint *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif


extern void (*const physJointPresolveTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
);
extern void (*const physJointSolveVelocityTable[PHYSJOINT_NUM_TYPES])(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#ifdef PHYSJOINT_USE_POSITIONAL_CORRECTION
extern return_t (*const physJointSolvePositionTable[PHYSJOINT_NUM_TYPES])(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif


#endif