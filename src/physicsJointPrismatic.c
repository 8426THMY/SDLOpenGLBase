#include "physicsJointPrismatic.h"


#include "physicsRigidBody.h"


#warning "This is all wrong, probably."
/*
** ----------------------------------------------------------------------
**
** Prismatic constraints only allow relative motion between two rigid
** bodies along a specified axis, which we call the constraint axis.
**
** ----------------------------------------------------------------------
**
** For the point-to-point constraint, we define rA and rB to be the
** offsets from the rigid bodies to a specific point on the constraint
** axis a. We want them to move along this axis a, so their movement
** along the tangents to this axis, u_1 and u_2, should be 0:
**
** C1 : d . u1 = 0,
** C2 : d . u2 = 0,
** C3 : qB - qA - qI = 0,
**
** Here, d = (pB + rB - pA - rA) is the separation between the two
** bodies, qA and qB are the orientations of the two bodies and qI
** is the initial offset in their orientations.
**
** Differentiate with respect to time to get velocity constraints:
**
** C1' : ((vB + wB X rB) - (vA + wA X rA)) . u1 + d . (wA X u1) = 0,
**     : -(u1 . vA) - (((rA + d) X u1) . wA) + (u1 . vB) + ((rB X u1) . wB) = 0,
** C2' : -(u2 . vA) - (((rA + d) X u2) . wA) + (u2 . vB) + ((rB X u2) . wB) = 0,
** C3' : wB - wA = 0.
**
** Note that for this derivation, we assume that the sliding
** axis a and its tangents have been transformed into body A's
** space. Hence, we get the following derivatives:
**
** da/dt    = wA X a,
** d(u1)/dt = wA X u1,
** d(u2)/dt = wA X u2.
**
** ----------------------------------------------------------------------
**
** Given the velocity vector
**
**     [vA]
**     [wA]
** V = [vB]
**     [wB]
**
** and the identity C' = JV, we can solve for the Jacobian J.
** We will use two separate Jacobians, one for the two linear
** constraints C1' and C2', which we will call J1, and the
** other for the angular constraint C3', which we will call J2.
**
**      [-u1, -((rA + d) X u1), u1, (rB X u1)]
** J1 = [-u2, -((rA + d) X u2), u2, (rB X u2)],
**
** J2 = [0, -1, 0, 1].
**
** ----------------------------------------------------------------------
**
** The effective mass for the constraint is given by JM^{-1}J^T,
** where M^{-1} is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**          [mA^{-1}    0       0       0   ]
**          [   0    IA^{-1}    0       0   ]
** M^{-1} = [   0       0    mB^{-1}    0   ],
**          [   0       0       0    IB^{-1}]
**
**        [       -u1,        -u2]
**        [-(rA X u1), -(rA X u2)]
** J1^T = [        u1,         u2],
**        [ (rB X u1),  (rB X u2)]
**
**        [ 0]
**        [-1]
** J2^T = [ 0].
**        [ 1]
**
**
** Evaluating this expression gives us the
** following matrix for our linear constraints:
**
**               [-mA^{-1} * u1,        -mA^{-1} * u2       ]
**               [-IA^{-1} * (rA X u1), -IA^{-1} * (rA X u2)]
** M^{-1}*J1^T = [ mB^{-1} * u1,         mB^{-1} * u2       ]
**               [ IB^{-1} * (rA X u1),  IB^{-1} * (rA X u2)]
**
** K = J1*M^{-1}*J1^T
** [K]_00 = mA^{-1} + mB^{-1} + (((rA + d) X u1) . (IA^{-1} * ((rA + d) X u1))) + ((rB X u1) . (IB^{-1} * (rB X u1))),
** [K]_01 =                     (((rA + d) X u1) . (IA^{-1} * ((rA + d) X u2))) + ((rB X u1) . (IB^{-1} * (rB X u2))),
** [K]_10 =                     (((rA + d) X u1) . (IA^{-1} * ((rA + d) X u2))) + ((rB X u1) . (IB^{-1} * (rB X u2))),
** [K]_11 = mA^{-1} + mB^{-1} + (((rA + d) X u2) . (IA^{-1} * ((rA + d) X u2))) + ((rB X u2) . (IB^{-1} * (rB X u2))).
**
** It is also worth noting that in this case, J1V is not
** a scalar either. It is the following 2x1 matrix:
**
**        [-(u1 . vA) - (((rA + d) X u1) . wA) + (u1 . vB) + ((rB X u1) . wB)]
** J1*V = [-(u2 . vA) - (((rA + d) X u2) . wA) + (u2 . vB) + ((rB X u2) . wB)].
**
** Multiplying the inverse of our effective mass by this matrix
** gives us a 2D vector, where each row contains the magnitude
** of the impulse to be applied in a tangent direction. From
** here, we just multiply the magnitudes by the directions
** and add them up to get the total impulse to apply.
**
**
** For our angular constraint, we simply get:
**
** J2*M^{-1}*J2^T = IA^{-1} + IB^{-1}.
**
** ----------------------------------------------------------------------
**
** Finally, adding a potential bias term, we have
**
** bn = B/dt * Cn(x),
**
** C_linear'  : J1*V + [b1, b2]^T,
** C_angular' : J2*V + b3,
**
** where B is the Baumgarte constant.
**
** ----------------------------------------------------------------------
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
** Using this, we get that the effective mass is:
**
** J1*M^{-1}J1^T = J2*M^{-1}J2^T = mA^{-1} + mB^{-1} + (((rA + d) X a) * IA^{-1}) . ((rA + d) X a) + (((rB X a) * IB^{-1}) . (rB X a))
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
** Using this, we get that the effective mass is:
**
** JM^{-1}J^T = mA^{-1} + mB^{-1}
**/


// Forward-declare any helper functions!
static void updateConstraintData(
	physicsJointPrismatic *const restrict joint,
	const physicsRigidBody *const restrict bodyA,
	const physicsRigidBody *const restrict bodyB
);
static void calculateEffectiveMass(
	physicsJointPrismatic *const restrict joint,
	const physicsRigidBody *const restrict bodyA,
	const physicsRigidBody *const restrict bodyB
);


void physJointPrismaticInit(physicsJointPrismatic *const restrict joint){
	//
}


/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
** Joints are always active so we always warm start.
*/
#ifdef PHYSJOINTPRISMATIC_WARM_START
void physJointPrismaticWarmStart(
	const physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
){

	const float totalImpulse = joint->limitImpulse + joint->motorImpulse;
	vec3 linearImpulse;
	vec3 angularImpulseA;
	vec3 angularImpulseB;

	// Calculate the linear impulse.
	vec3MultiplySOut(&joint->tangentsGlobal[0], joint->linearImpulse.x, &linearImpulse);
	vec3FmaP2(joint->linearImpulse.y, &joint->tangentsGlobal[1], &linearImpulse);
	vec3FmaP2(totalImpulse, &joint->axisGlobal, &linearImpulse);

	// Calculate the angular impulse for body A.
	vec3MultiplySOut(&joint->rAu1, joint->linearImpulse.x, &angularImpulseA);
	vec3FmaP2(joint->linearImpulse.y, &joint->rAu2, &angularImpulseA);
	vec3FmaP2(totalImpulse, &joint->rAa, &angularImpulseA);

	// Calculate the angular impulse for body B.
	vec3MultiplySOut(&joint->rBu1, joint->linearImpulse.x, &angularImpulseB);
	vec3FmaP2(joint->linearImpulse.y, &joint->rBu2, &angularImpulseB);
	vec3FmaP2(totalImpulse, &joint->rBa, &angularImpulseB);


	// Apply the accumulated impulses.
	physRigidBodyApplyLinearImpulseInverse(bodyA, &linearImpulse);
	physRigidBodyApplyAngularImpulseInverse(bodyA, angularImpulseA);
	physRigidBodyApplyLinearImpulse(bodyB, &linearImpulse);
	physRigidBodyApplyAngularImpulse(bodyB, angularImpulseB);
}
#endif

/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointPrismaticPresolve(
	physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB,
	const float dt
){

	updateConstraintData(joint, bodyA, bodyB);
	calculateEffectiveMass(joint, bodyA, bodyB);
	#ifdef PHYSJOINTPRISMATIC_WARM_START
	physJointPrismaticWarmStart(joint, bodyA, bodyB);
	#endif
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointPrismaticSolveVelocity(
	physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
){

	/** We should calculate and add the bias here if we're using Baumgarte. **/
	/** Most of these need d though, so should we calculate it elsewhere?   **/
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
return_t physJointPrismaticSolvePosition(
	const physicsJointPrismatic *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
){

	#ifdef PHYSJOINTPRISMATIC_STABILISER_GAUSS_SEIDEL
	#endif

	return(1);
}


/*
** Transform some of the joint's variables
** based on the new states of the rigid bodies.
*/
static void updateConstraintData(
	physicsJointPrismatic *const restrict joint,
	const physicsRigidBody *const restrict bodyA,
	const physicsRigidBody *const restrict bodyB
){

	vec3 rA;
	vec3 rB;
	vec3 d;
	float axisSeparation;

	// Transform the anchor points using the bodies' new scales and rotations.
	vec3SubtractVec3Out(&joint->anchorA, &bodyA->base->centroid, &rA);
	transformDirection(&bodyA->state, &rA);
	vec3SubtractVec3Out(&joint->anchorB, &bodyB->base->centroid, &rB);
	transformDirection(&bodyB->state, &rB);

	// Find the relative position of the two bodies.
	// d = (pB - pA)
	//   = (cB + rB - cA - rA)
	// We don't subtract rA yet, as we'll just be adding it back.
	d = bodyB->centroid;
	vec3AddVec3(&d, &rB);
	vec3SubtractVec3P1(&d, &bodyA->centroid);


	// Transform the constraint axis and its tangents to body A's
	// space, then cross them by the transformed anchor points.
	// a
	transformDirectionOut(&bodyA->state, &joint->axisLocal, &joint->axisGlobal);
	vec3CrossVec3Out(&d, &joint->axisGlobal, &joint->rAa);
	vec3CrossVec3Out(&rB, &joint->axisGlobal, &joint->rBa);
	// u1
	transformDirectionOut(&bodyA->state, &joint->tangentsLocal[0], &joint->tangentsGlobal[0]);
	vec3CrossVec3Out(&d, &joint->tangentsGlobal[0], &joint->rAu1);
	vec3CrossVec3Out(&rB, &joint->tangentsGlobal[0], &joint->rBu1);
	// u2
	transformDirectionOut(&bodyA->state, &joint->tangentsLocal[1], &joint->tangentsGlobal[1]);
	vec3CrossVec3Out(&d, &joint->tangentsGlobal[1], &joint->rAu2);
	vec3CrossVec3Out(&rB, &joint->tangentsGlobal[1], &joint->rBu2);


	// Now we can subtract rA.
	vec3SubtractVec3P1(&d, &rA);
	// Calculate the rigid bodies' separation along the constraint axis.
	axisSeparation = vec3DotVec3(&d, &joint->axisGlobal);
	/** Do stuff **/
}

/*
** Calculate the effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once
** per update. Note that we take its inverse elsewhere.
*/
static void calculateEffectiveMass(
	physicsJointPrismatic *const restrict joint,
	const physicsRigidBody *const restrict bodyA,
	const physicsRigidBody *const restrict bodyB
){

	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *const invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *const invInertiaB = &bodyB->invInertiaGlobal;

	vec3 *rAa  = &joint->rAa;
	vec3 *rBa  = &joint->rBa;
	vec3 *rAu1 = &joint->rAu1;
	vec3 *rAu2 = &joint->rAu2;
	vec3 *rBu1 = &joint->rBu1;
	vec3 *rBu2 = &joint->rBu2;

	vec3 IArAu1;
	vec3 IArAu2;
	vec3 IBrBu1;
	vec3 IBrBu2;


	// JM^{-1}J^T = mA^{-1} + mB^{-1} + (((rA + d) X a) . (IA^{-1} * ((rA + d) X a))) + ((rB X a) . (IB^{-1} * (rB X a)))
	mat3MultiplyVec3Out(invInertiaA, rAa, &IArAu1);
	mat3MultiplyVec3Out(invInertiaB, rBa, &IBrBu1);

	// Calculate the inverse limit and motor effective mass.
	// The motor and both limits have the same effective mass.
	joint->limitMotorInvMass = 1.f / (invMass + vec3DotVec3(&IArAu1, rAa) + vec3DotVec3(&IBrBu1, rBa));


	// K = JM^{-1}J^T
	// [K]_00 = mA^{-1} + mB^{-1} + (((rA + d) X u1) . (IA^{-1} * ((rA + d) X u1))) + ((rB X u1) . (IB^{-1} * (rB X u1)))
	// [K]_01 =                     (((rA + d) X u1) . (IA^{-1} * ((rA + d) X u2))) + ((rB X u1) . (IB^{-1} * (rB X u2)))
	// [K]_10 = [K]_01
	// [K]_11 = mA^{-1} + mB^{-1} + (((rA + d) X u2) . (IA^{-1} * ((rA + d) X u2))) + ((rB X u2) . (IB^{-1} * (rB X u2)))
	mat3MultiplyVec3Out(invInertiaA, rAu1, &IArAu1);
	mat3MultiplyVec3Out(invInertiaA, rAu2, &IArAu2);
	mat3MultiplyVec3Out(invInertiaB, rBu1, &IBrBu1);
	mat3MultiplyVec3Out(invInertiaB, rBu2, &IBrBu2);

	// Calculate the inverse linear effective mass.
	joint->linearInvMass.m[0][0] = invMass + vec3DotVec3(&IArAu1, rAu1) + vec3DotVec3(&IBrBu1, rBu1);
	joint->linearInvMass.m[0][1] =
	joint->linearInvMass.m[1][0] = vec3DotVec3(&IArAu1, rAu2) + vec3DotVec3(&IBrBu1, rBu2);
	joint->linearInvMass.m[1][1] = invMass + vec3DotVec3(&IArAu2, rAu2) + vec3DotVec3(&IBrBu2, rBu2);
	mat2Invert(&joint->linearInvMass);


	// JM^{-1}J^T = IA^{-1} + IB^{-1}
	// Calculate the inverse angular effective mass.
    mat3AddMat3Out(invInertiaA, invInertiaB, &joint->angularInvMass);
	mat3Invert(&joint->angularInvMass);
}