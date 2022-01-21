#include "physicsJointFixed.h"


#include "physicsRigidBody.h"


/*
** ----------------------------------------------------------------------
**
** Fixed joints constrain the positions of the anchor points,
** rA and rB, to be coincident. We also constraint the relative
** orientations to be ?
**
** ----------------------------------------------------------------------
**
** The linear constraint is identical to that of the spherical joint.
** That is, we just want the difference between the anchors to be 0:
**
** C1 : (pB + rB) - (pA + rA) = 0.
**
** Differentiate with respect to time to get a velocity constraint:
**
** C1' : (vB +   wB X rB) - (vA +   wA X rA) = 0,
**     : (vB -   rB X wB) - (vA -   rA X wA) = 0,
**     : (vB - [rB]_X*wB) - (vA - [rA]_X*wA) = 0,
**
** where "[.]_X" denotes the skew-symmetric "cross product" matrix.
**
**
** We also need to impose constraints on the relative orientation
** of the rigid bodies, so if "(RB - RA)" ?
**
** C2 : (RB - RA) . a <= 0.
**
** ?
**
** The velocity constraints for this is
**
** C2' : (wB - wA) . a <= 0.
**
** ?
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
** We will use three separate Jacobians, J1 and J2, for
** C1' and C2'.
**
** J1 = [-I_3, [rA]_X, I_3, -[rB]_X],
** J2 = ?
**
** Note that "I_3" is the 3x3 identity matrix.
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
**        [ -I_3  ]
**        [-[rA]_X]
** J1^T = [  I_3  ].
**        [ [rB]_X]
**
**
** Evaluating this expression gives us the following matrix for our
** point-to-point constraint:
**
**               [-mA^{-1} * I_3   ]
**               [-IA^{-1} * [rA]_X]
** M^{-1}*J1^T = [ mB^{-1} * I_3   ],
**               [ IB^{-1} * [rB]_X]
**
** K1 = J1*M^{-1}*J1^T
**    = (mA^{-1} + mB^{-1})*I_3 - [rA]_X*IA^{-1}*[rA]_X - [rB]_X*IB^{-1}*[rB]_X,
**
** For our angular constraints, we simply get:
**
**               [     0    ]
**               [-IA^{-1}*s]
** M^{-1}*J2^T = [     0    ],
**               [ IB^{-1}*s]
**
** K2 = J2*M^{-1}*J2^T = s . (IA^{-1} + IB^{-1})*s,
**
** and similarly for K3 = K4. Note that K1 is a 3x3 matrix,
** but K2, K3 and K4 are simply scalars.
**
** ----------------------------------------------------------------------
*/


// Forward-declare any helper functions!
static void updateConstraintData(
	physicsJointFixed *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);

static void calculateLinearMass(
	const vec3 rA, const vec3 rB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict linearMass
);

static void calculateLinearBias(
	const vec3 *const restrict centroidA, const vec3 *const restrict centroidB,
	const vec3 *const restrict rA, const vec3 *const restrict rB,
	vec3 *const restrict linearBias
);


/*
** Set up some of the joint's properties. The anchor positions and
** orientations let us define relative
*/
void physJointFixedInit(
	physicsJointFixed *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const quat *const restrict anchorRotA, const quat *const restrict anchorRotB
){

	joint->anchorA = *anchorA;
	joint->anchorB = *anchorB;
	joint->anchorRotA = *anchorRotA;
	joint->anchorRotB = *anchorRotB;
}


#ifdef PHYSJOINTFIXED_WARM_START
void physJointFixedWarmStart(
	const physicsJointFixed *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	//
}
#endif

/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointFixedPresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
){

	const float frequency = 1.f/dt;


	updateConstraintData((physicsJointFixed *)joint, bodyA, bodyB);


	/** ANGULAR STUFF **/


	// Compute the linear bias term.
	calculateLinearBias(
		&bodyA->centroid, &bodyB->centroid,
		&((physicsJointFixed *)joint)->rA, &((physicsJointFixed *)joint)->rB,
		&((physicsJointFixed *)joint)->linearBias
	);
	// b = B/dt * C
	vec3MultiplyS(&((physicsJointFixed *)joint)->linearBias, PHYSJOINTFIXED_BAUMGARTE_BIAS * frequency);

	calculateLinearMass(
		((physicsJointFixed *)joint)->rA, ((physicsJointFixed *)joint)->rB,
		bodyA, bodyB, &((physicsJointFixed *)joint)->linearInvMass
	);
	// The performance of solving using Cramer's rule seems similar
	// to inverting. However, since we do our velocity solve step
	// multiple times using the same matrix for sequential impulse,
	// it's much faster to invert it here when we're presolving.
	mat3Invert(&((physicsJointFixed *)joint)->linearInvMass);


	#ifdef PHYSJOINTFIXED_WARM_START
	// Warm start the constraint if the old limit states match the new ones.
	physJointFixedWarmStart((physicsJointFixed *)joint, bodyA, bodyB);
	#endif
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointFixedSolveVelocity(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 relativeVelocity;
	vec3 impulse;


	/** ANGULAR PART **/


	// Solve the linear point-to-point constraint last,
	// as it's more important that it's satisfied.
	{
		// v_socket   = vA + wA X rA
		// v_ball     = vB + wB X rB
		// v_relative = v_ball - v_socket

		// Calculate the total velocity of the
		// socket and store it in "relativeVelocity".
		vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointFixed *)joint)->rA, &relativeVelocity);
		vec3AddVec3(&relativeVelocity, &bodyA->linearVelocity);
		// Calculate the total velocity of
		// the ball and store it in "impulse".
		vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointFixed *)joint)->rB, &impulse);
		vec3AddVec3(&impulse, &bodyB->linearVelocity);
		// Calculate the relative velocity between the ball and socket.
		// Note that we actually compute "-v_relative" and store it in "impulse".
		vec3SubtractVec3P2(&relativeVelocity, &impulse);
		// Subtract the bias term from the negative relative velocity.
		vec3SubtractVec3P1(&impulse, &((physicsJointFixed *)joint)->linearBias);


		// Solve for the linear impulse:
		// J1*V + b1 = v_relative + b1,
		// K1*lambda = -(J1*V + b1).
		mat3MultiplyVec3(&((physicsJointFixed *)joint)->linearInvMass, &impulse);
		vec3AddVec3(&((physicsJointFixed *)joint)->linearImpulse, &impulse);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulseInverse(bodyA, &((physicsJointFixed *)joint)->rA, &impulse);
		physRigidBodyApplyImpulse(bodyB, &((physicsJointFixed *)joint)->rB, &impulse);
	}
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
return_t physJointFixedSolvePosition(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	#ifdef PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL
	float linearError;
	vec3 impulse;


	/** ANGULAR PART **/


	// Solve the linear point-to-point constraint.
	{
		vec3 rA;
		vec3 rB;
		vec3 constraint;
		mat3 linearMass;

		vec3SubtractVec3Out(&((physicsJointFixed *)joint)->anchorA, &bodyA->base->centroid, &rA);
		transformDirection(&bodyA->state, &rA);
		vec3SubtractVec3Out(&((physicsJointFixed *)joint)->anchorB, &bodyB->base->centroid, &rB);
		transformDirection(&bodyB->state, &rB);

		// Calculate the displacement from the ball to the socket:
		// -C1 = (pA + rA) - (pB - rB).
		// Note that we want -C1, so we swap the inputs around.
		calculateLinearBias(
			&bodyB->centroid, &bodyA->centroid,
			&rB, &rA,
			&constraint
		);
		linearError = vec3MagnitudeVec3(&constraint);
		vec3MultiplyS(&constraint, PHYSJOINTFIXED_BAUMGARTE_BIAS);

		// Solve for the impulse:
		// K1*lambda = -C1.
		calculateLinearMass(rA, rB, bodyA, bodyB, &linearMass);
		mat3Solve(&linearMass, &constraint, &impulse);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &impulse);
		physRigidBodyApplyImpulsePosition(bodyB, &rB, &impulse);
	}

	return(
		linearError <= PHYSJOINT_LINEAR_POSITIONAL_ERROR_THRESHOLD
	);
	#else
	return(1);
	#endif
}


/*
** Update the global positions of the anchor points
** and some of the other variables that depend on them.
*/
static void updateConstraintData(
	physicsJointFixed *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	// Transform the axis points using the bodies' new scales and rotations.
	vec3SubtractVec3Out(&joint->anchorA, &bodyA->base->centroid, &joint->rA);
	transformDirection(&bodyA->state, &joint->rA);
	vec3SubtractVec3Out(&joint->anchorB, &bodyB->base->centroid, &joint->rB);
	transformDirection(&bodyB->state, &joint->rB);
}


/*
** Calculate the linear effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once per update.
*/
static void calculateLinearMass(
	const vec3 rA, const vec3 rB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict linearMass
){

	// Compute the linear effective mass given by:
	// K1 = (mA^{-1} + mB^{-1})*I_3 - [rA]_X*IA^{-1}*[rA]_X - [rB]_X*IB^{-1}*[rB]_X.
	//
	// This probably isn't so good, as [rA]_X*IA^{-1}*[rA]_X and [rB]_X*IB^{-1}*[rB]_X
	// are necessarily singular. We don't invert the matrix here, as it's faster to
	// solve for lambda using Cramer's rule.

	// Compute body A's contribution to K1.
	{
		// Note that the matrix [rA]_X*IA^{-1}*[rA]_X is symmetric, so we
		// only need to compute the half of the off-diagonal elements.

		// These variables bring it down to 27 multiplications and 14 subtractions.
		// An alternative method gives 24 multiplications and 21 subtractions,
		// but float multiplication is generally faster and has less error.
		const float I[6] = {
			bodyA->invInertiaGlobal.m[0][0], bodyA->invInertiaGlobal.m[0][1], bodyA->invInertiaGlobal.m[0][2],
			                                 bodyA->invInertiaGlobal.m[1][1], bodyA->invInertiaGlobal.m[1][2],
			                                                                  bodyA->invInertiaGlobal.m[2][2]
		};
		const float a3xy = rA.z*I[1];
		const float A[8] = {
			rA.y*I[2] -      a3xy, rA.y*I[4] - rA.z*I[3], rA.y*I[5] - rA.z*I[4],
			rA.z*I[0] - rA.x*I[2],      a3xy - rA.x*I[4], rA.z*I[2] - rA.x*I[5],
			rA.x*I[1] - rA.y*I[0], rA.x*I[3] - rA.y*I[1]
		};

		// Compute -[rA]_X*IA^{-1}*[rA]_X.
		// We don't set the lower triangular components here, as
		// we can just do that when adding body B's contribution.
		linearMass->m[0][0] = rA.y*A[2] - rA.z*A[1];
		linearMass->m[0][1] = rA.z*A[0] - rA.x*A[2];
		linearMass->m[0][2] = rA.x*A[1] - rA.y*A[0];
		linearMass->m[1][1] = rA.z*A[3] - rA.x*A[5];
		linearMass->m[1][2] = rA.x*A[4] - rA.y*A[3];
		linearMass->m[2][2] = rA.x*A[7] - rA.y*A[6];
	}
	// Compute body B's contribution to K1.
	{
		const float I[6] = {
			bodyB->invInertiaGlobal.m[0][0], bodyB->invInertiaGlobal.m[0][1], bodyB->invInertiaGlobal.m[0][2],
			                                 bodyB->invInertiaGlobal.m[1][1], bodyB->invInertiaGlobal.m[1][2],
			                                                                  bodyB->invInertiaGlobal.m[2][2]
		};
		const float a3xy = rB.z*I[1];
		const float A[8] = {
			rB.y*I[2] -      a3xy, rB.y*I[4] - rB.z*I[3], rB.y*I[5] - rB.z*I[4],
			rB.z*I[0] - rB.x*I[2],      a3xy - rB.x*I[4], rB.z*I[2] - rB.x*I[5],
			rB.x*I[1] - rB.y*I[0], rB.x*I[3] - rB.y*I[1]
		};
		const float invMass = bodyA->invMass + bodyB->invMass;

		// Compute (mA^{-1} + mB^{-1})*I_3 - [rB]_X*IB^{-1}*[rB]_X.
		linearMass->m[0][0] += rB.y*A[2] - rB.z*A[1] + invMass;
		linearMass->m[0][1] += rB.z*A[0] - rB.x*A[2];
		linearMass->m[0][2] += rB.x*A[1] - rB.y*A[0];
		linearMass->m[1][0] = linearMass->m[0][1];
		linearMass->m[1][1] += rB.z*A[3] - rB.x*A[5] + invMass;
		linearMass->m[1][2] += rB.x*A[4] - rB.y*A[3];
		linearMass->m[2][0] = linearMass->m[0][2];
		linearMass->m[2][1] = linearMass->m[1][2];
		linearMass->m[2][2] += rB.x*A[7] - rB.y*A[6] + invMass;
	}
}


// Calculate the joint's linear bias term.
static void calculateLinearBias(
	const vec3 *const restrict centroidA, const vec3 *const restrict centroidB,
	const vec3 *const restrict rA, const vec3 *const restrict rB,
	vec3 *const restrict linearBias
){

	// Calculate the displacement from the ball to the socket:
	// C1 = (pB - rB) - (pA + rA).
	vec3AddVec3Out(centroidB, rB, linearBias);
	vec3SubtractVec3P1(linearBias, centroidA);
	vec3SubtractVec3P1(linearBias, rA);
}