#include "physicsJointPrismatic.h"


#include "physicsRigidBody.h"


/*
** We start with two linear constraints that prevent the
** bodies from moving perpendicular to the constraint axis
** "a" and an angular constraint that prevents them from
** rotating relative to each other.
**
** C1 : d . u1 = 0
** C2 : d . u2 = 0
** C3 : qB - qA - qI = 0
**
** Note that "u1" and "u2" are the two tangent vectors,
** "qA" and "qB" are the orientations of the two bodies
** and "qI" is their initial relative orientation.
**
**
** We will begin by differentiating the linear constraints,
** constraints letting "d = (pB - pA)":
**
** C1' : ((vB + wB X rB) - (vA + wA X rA)) . u1 + d . (wA X u1) = 0
**     : -(u1 . vA) - (((rA + d) X u1) . wA) + (u1 . vB) + ((rB X u1) . wB) = 0
** C2' : -(u2 . vA) - (((rA + d) X u2) . wA) + (u2 . vB) + ((rB X u2) . wB) = 0
**
** Note that for this derivation, we assume that the sliding
** axis "a" has been transformed into body A's space. Hence,
** we get that "u1" and "u2" are rotating at "wA":
**
** d(u1)/dt = wA X u1
** d(u2)/dt = wA X u2
**
**
** By inspection:
**
**     [-u1, -((rA + d) X u1), u1, (rB X u1)]
** J = [-u2, -((rA + d) X u2), u2, (rB X u2)]
**
**        [mA^-1     0     0     0]
**        [    0 IA^-1     0     0]
** M^-1 = [    0     0 mB^-1     0]
**        [    0     0     0 IB^-1]
**
**       [             -u1,              -u2]
**       [-((rA + d) X u1), -((rA + d) X u2)]
** J^T = [              u1,               u2]
**       [       (rB X u1),        (rB X u2)]
**
**
** Calculating JM^-1J^T gives us a 2x2 matrix, which we can
** invert to get our effective mass:
**
**         [-u1 * mA^-1, -((rA + d) X u1) * IA^-1, u1 * mB^-1, (rB X u1) * IB^-1]
** JM^-1 = [-u2 * mA^-1, -((rA + d) X u2) * IA^-1, u2 * mB^-1, (rB X u2) * IB^-1]
**
** K = JM^-1J^T
** [K]_00 = mA^-1 + mB^-1 + ((((rA + d) X u1) * IA^-1) . ((rA + d) X u1)) + (((rB X u1) * IB^-1) . (rB X u1))
** [K]_01 =                 ((((rA + d) X u1) * IA^-1) . ((rA + d) X u2)) + (((rB X u1) * IB^-1) . (rB X u2))
** [K]_10 =                 ((((rA + d) X u1) * IA^-1) . ((rA + d) X u2)) + (((rB X u1) * IB^-1) . (rB X u2))
** [K]_11 = mA^-1 + mB^-1 + ((((rA + d) X u2) * IA^-1) . ((rA + d) X u2)) + (((rB X u2) * IB^-1) . (rB X u2))
**
**
** For the angular constraint, we get:
**
** C3' : wB - wA = 0
**
**
** By inspection:
**
** J = [0, -1, 0, 1]
** V = [vA, wA, vB, wB]
**
**
** Using this, we get that the effective mass is the inverse of:
**
** JM^-1J^T = IA^-1 + IB^-1
*/

#warning "We use the same constraint for the motor and the limits."
/**
** We might also choose to impose limits on how near or far the two
** bodies may be from each other. We define the following constraints
** to create this behaviour, where "a" is the constraint axis:
**
** C4 : (d . a) - d_min >= 0
** C5 : d_max - (d . a) >= 0
**
** Note that the distance between the bodies on
** the constraint axis is given by "d = (d . a)".
**
**
** Differentiate, as we want to make a change in velocity:
**
** C4' : ((vB + wB X rB) - (vA + wA X rA)) . a + d . (wA X a) >= 0
**     : -(a . vA) - (((rA + d) X a) . wA) + (a . vB) + ((rB X a) . wB) = 0
** C5' : (a . vA) + (((rA + d) X a) . wA) - (a . vB) - ((rB X a) . wB) = 0 >= 0
**
** Note that as before, we assume that the sliding axis
** axis "a" has been transformed into body A's space. Hence:
**
** da/dt = wA X a
**
**
** By inspection:
**
** J1 = [-a, -((rA + d) X a),  a,  (rB X a)]
** J2 = [ a,  ((rA + d) X a), -a, -(rB X a)]
**
**
** Using this, we get that the effective mass is the inverse of:
**
** J1M^-1J1^T = J2M^-1J2^T = mA^-1 + mB^-1 + (((rA + d) X a) * IA^-1) . ((rA + d) X a) + (((rB X a) * IB^-1) . (rB X a))
**/

/**
** Another useful feature is a motor constraint, which keeps a
** constant relative velocity along the constraint axis "a".
** This time, we start with a velocity constraint.
**
** C6' : (vB - vA) . a = 0
**
** Note that we don't care about the angular velocities, as
** the bodies will not have any relative angular velocity.
**
**
** By inspection:
**
** J = [-a, 0, a, 0]
**
**
** Using this, we get that the effective mass is the inverse of:
**
** JM^-1J^T = mA^-1 + mB^-1
**/


// Forward-declare any helper functions!
static void updateConstraintData(physicsJointPrismatic *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateEffectiveMass(physicsJointPrismatic *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateBias(physicsJointPrismatic *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);


void physJointPrismaticInit(physicsJointPrismatic *joint){
	//
}


/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
** Joints are always active so we always warm start.
*/
void physJointPrismaticWarmStart(physicsJointPrismatic *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dtRatio){
	//
}

/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointPrismaticPresolve(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt){
	updateConstraintData((physicsJointPrismatic *)joint, bodyA, bodyB);
	calculateEffectiveMass((physicsJointPrismatic *)joint, bodyA, bodyB);
	calculateBias((physicsJointPrismatic *)joint, bodyA, bodyB, dt);
	physJointPrismaticWarmStart((physicsJointPrismatic *)joint, bodyA, bodyB, 1.f/dt);
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointPrismaticSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	//
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
#ifdef PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL
float physJointPrismaticSolvePosition(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	return(0.f);
}
#endif


/*
** Transform some of the joint's variables
** based on the new states of the rigid bodies.
*/
static void updateConstraintData(physicsJointPrismatic *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	vec3 rA;
	vec3 rB;
	vec3 d;

	// Transform the anchor points using the bodies' new scales and rotations.
	vec3MultiplyVec3Out(&bodyA->transform.scale, &joint->anchorA, &rA);
	quatRotateVec3Fast(&bodyA->transform.rot, &rA, &rA);
	vec3MultiplyVec3Out(&bodyB->transform.scale, &joint->anchorB, &rB);
	quatRotateVec3Fast(&bodyB->transform.rot, &rB, &rB);

	// Find the relative position of the two bodies.
	// d = (pB - pA)
	//   = (cB + rB - cA - rA)
	// We don't subtract rA yet, as we'll just be adding it back.
	d = bodyB->centroidGlobal;
	vec3AddVec3(&d, &rB);
	vec3SubtractVec3From(&d, &bodyA->centroidGlobal);


	// Transform the constraint axis and its tangents to body A's
	// space, then cross them by the transformed anchor points.
	// a
	vec3MultiplyVec3Out(&bodyA->transform.scale, &joint->axisLocal, &joint->axisGlobal);
	quatRotateVec3Fast(&bodyA->transform.rot, &joint->axisGlobal, &joint->axisGlobal);
	vec3CrossVec3Out(&d, &joint->axisGlobal, &joint->rAa);
	vec3CrossVec3Out(&rB, &joint->axisGlobal, &joint->rBa);
	// u1
	vec3MultiplyVec3Out(&bodyA->transform.scale, &joint->tangentsLocal[0], &joint->tangentsGlobal[0]);
	quatRotateVec3Fast(&bodyA->transform.rot, &joint->tangentsGlobal[0], &joint->tangentsGlobal[0]);
	vec3CrossVec3Out(&d, &joint->tangentsGlobal[0], &joint->rAu1);
	vec3CrossVec3Out(&rB, &joint->tangentsGlobal[0], &joint->rBu1);
	// u2
	vec3MultiplyVec3Out(&bodyA->transform.scale, &joint->tangentsLocal[1], &joint->tangentsGlobal[1]);
	quatRotateVec3Fast(&bodyA->transform.rot, &joint->tangentsGlobal[1], &joint->tangentsGlobal[1]);
	vec3CrossVec3Out(&d, &joint->tangentsGlobal[1], &joint->rAu2);
	vec3CrossVec3Out(&rB, &joint->tangentsGlobal[1], &joint->rBu2);


	// Now we can subtract rA.
	vec3SubtractVec3From(&d, &rA);
	/** Do stuff **/
}

/*
** Calculate the effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once
** per update. Note that we take its inverse elsewhere.
*/
static void calculateEffectiveMass(physicsJointPrismatic *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *invInertiaB = &bodyB->invInertiaGlobal;

	vec3 *rAa  = &joint->rAa;
	vec3 *rBa  = &joint->rBa;
	vec3 *rAu1 = &joint->rAu1;
	vec3 *rAu2 = &joint->rAu2;
	vec3 *rBu1 = &joint->rBu1;
	vec3 *rBu2 = &joint->rBu2;

	vec3 rAu1IA;
	vec3 rAu2IA;
	vec3 rBu1IB;
	vec3 rBu2IB;


	// JM^-1J^T = mA^-1 + mB^-1 + (((rA + d) X a) * IA^-1) . ((rA + d) X a) + (((rB X a) * IB^-1) . (rB X a))
	mat3MultiplyByVec3Out(invInertiaA, rAa, &rAu1IA);
	mat3MultiplyByVec3Out(invInertiaB, rBa, &rBu1IB);

	// Calculate the limit effective mass.
	// The motor and both limits have the same effective mass.
	joint->limitMotorMass = 1.f / (invMass + vec3DotVec3(&rAu1IA, rAa) + vec3DotVec3(&rBu1IB, rBa));


	// K = JM^-1J^T
	// [K]_00 = mA^-1 + mB^-1 + ((((rA + d) X u1) * IA^-1) . ((rA + d) X u1)) + (((rB X u1) * IB^-1) . (rB X u1))
	// [K]_01 =                 ((((rA + d) X u1) * IA^-1) . ((rA + d) X u2)) + (((rB X u1) * IB^-1) . (rB X u2))
	// [K]_10 = [K]_01
	// [K]_11 = mA^-1 + mB^-1 + ((((rA + d) X u2) * IA^-1) . ((rA + d) X u2)) + (((rB X u2) * IB^-1) . (rB X u2))
	mat3MultiplyByVec3Out(invInertiaA, rAu1, &rAu1IA);
	mat3MultiplyByVec3Out(invInertiaA, rAu2, &rAu2IA);
	mat3MultiplyByVec3Out(invInertiaB, rBu1, &rBu1IB);
	mat3MultiplyByVec3Out(invInertiaB, rBu2, &rBu2IB);

	// Calculate the linear effective mass.
	joint->linearMass.m[0][0] = invMass + vec3DotVec3(&rAu1IA, rAu1) + vec3DotVec3(&rBu1IB, rBu1);
	joint->linearMass.m[0][1] =
	joint->linearMass.m[1][0] = vec3DotVec3(&rAu1IA, rAu2) + vec3DotVec3(&rBu1IB, rBu2);
	joint->linearMass.m[1][1] = invMass + vec3DotVec3(&rAu2IA, rAu2) + vec3DotVec3(&rBu2IB, rBu2);
	// m_eff = 1/K
	mat2Invert(&joint->linearMass);


	// JM^-1J^T = IA^-1 + IB^-1
	// Calculate the angular effective mass.
    mat3AddMat3Out(invInertiaA, invInertiaB, &joint->angularMass);
	// m_eff = 1/K
	mat3Invert(&joint->angularMass);
}

/*
** Calculate the joint's bias term. This will help
** to soften the constraint and make it springy.
*/
static void calculateBias(physicsJointPrismatic *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	//
}