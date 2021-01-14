#include "physicsJointSphere.h"


#include "physicsRigidBody.h"

#include "vec4.h"
#include "utilMath.h"


/*
** ----------------------------------------------------------------------
**
** Spherical constraints involve a "point-to-point" constraint and
** a cone constraint that limits one rigid body's relative position
** and orientation to a cone about some user-specified axis.
** This constraint is similar to a ball and socket joint.
**
** ----------------------------------------------------------------------
**
** For the point-to-point constraint, we define rA and rB to be
** the offsets of the "socket" and "ball" from the rigid bodies
** respectively. Of course, these two points should coincide, so
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
** We also need to impose constraints on the angular velocities
** of the rigid bodies themselves, not the joint axis. Suppose we
** want to constrain rotation about the axis a. Then wB . a > 0
** implies that the rigid body is rotating towards (or past) the
** axis limit. Hence, we impose the constraints
**
** C2' : (wB - wA) . s <= 0,
** C3' : (wB - wA) . t <= 0,
**
** where 's' denotes the swing axis and 't' denotes the twist axis.
** The twist axis is chosen as body B's local x-axis.
**
** Note that in order to solve the angular constraints as C' >= 0
** constraints like we usually would, we just negate the swing and
** twist axes after computing them.
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
** We will use three separate Jacobians, J1, J2 and J3, for
** C1', C2' and C3'.
**
** J1 = [-I_3, [rA]_X, I_3, -[rB]_X],
** J2 = [0, -s, 0, s],
** J3 = [0, -t, 0, t],
**
** Note that "I_3" is the 3x3 identity matrix.
**
** ----------------------------------------------------------------------
**
** The effective mass for the constraint is given by JM^(-1)J^T,
** where M^(-1) is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**          [mA^(-1)    0       0       0   ]
**          [   0    IA^(-1)    0       0   ]
** M^(-1) = [   0       0    mB^(-1)    0   ],
**          [   0       0       0    IB^(-1)]
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
**               [-mA^(-1) * I_3   ]
**               [-IA^(-1) * [rA]_X]
** M^(-1)*J1^T = [ mB^(-1) * I_3   ],
**               [ IB^(-1) * [rB]_X]
**
** K1 = J1*M^(-1)*J1^T
**    = (mA^(-1) + mB^(-1))*I_3 - [rA]_X*IA^(-1)*[rA]_X - [rB]_X*IB^(-1)*[rB]_X,
**
** For our angular constraints, we simply get:
**
**               [     0    ]
**               [-IA^(-1)*s]
** M^(-1)*J2^T = [     0    ],
**               [ IB^(-1)*s]
**
** K2 = J2*M^(-1)*J2^T = s . (IA^(-1) + IB^(-1))*s,
**
** and similarly for K3. Note that K1 is a 3x3 matrix,
** but K2 and K3 are simply scalars.
**
** ----------------------------------------------------------------------
**
** To limit the relative orientation of rigid body B with
** respect to rigid body A, we use swing-twist decomposition
** and constrain the swing and twist axes individually.
**
** The biases for the swing and twist constraints, b1 and b2
** respectively, are defined as
**
** b1 = theta_s / dt,
** b2 = theta_t / dt,
**
** where theta_s and theta_t are the differences between the
** nearest angle limit and the current angle. For the twist
** axis, we are only constraining between two limits, but
** in the swing case we need to clamp the angle to the closest
** point on an ellipse.
**
** It is worth noting that we don't use an ellipse so much as
** four elliptical segments, one in each quadrant. The major
** and minor radii depend on the minimum or maximum angle limits
** for each axis.
**
** ----------------------------------------------------------------------
*/
#error "Investigate 6-DOF joints and maybe energy loss!"
#error "Double check how Bullet does stuff. It doesn't solve for the limits if they haven't been broken."
/** https://github.com/bulletphysics/bullet3/blob/master/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp **/
/** https://github.com/bulletphysics/bullet3/blob/master/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h **/
/** https://github.com/bulletphysics/bullet3/blob/master/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp **/


// Forward-declare any helper functions!
static void updateConstraintData(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
static void calculateLinearMass(
	const vec3 rA, const vec3 rB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict linearMass
);
static void calculateInverseAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const vec3 *const restrict swingAxis, const vec3 *const restrict twistAxis,
	float *const restrict swingInvMass, float *const restrict twistInvMass
);
static void calculateBias(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float frequency
);

static float calculateSwingLimit(
	const quat *const restrict swing, const quat *const restrict rotA,
	const float *const restrict angleLimitsY, const float *const restrict angleLimitsZ,
	vec3 *const restrict swingAxis
);
static float calculateTwistLimit(
	const quat *const restrict twist, const quat *const restrict rotA,
	const float *const restrict angleLimitsX,
	vec3 *const restrict twistAxis
);


/*
** The inputs for the ball and socket joint shouldn't be too mysterious:
**     anchorA          - Vector from rigid body A's point of reference to the socket.
**     anchorB          - Vector from rigid body B's point of reference to the ball.
**     {min,max}{X,Y,Z} - Angles used to clamp the relative orientation of body B.
** Note that the minimum and maximum angles are given in radians.
** The axes are taken relative to rigid body A's frame of reference.
*/
void physJointSphereInit(
	physicsJointSphere *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
){

	joint->anchorA = *anchorA;
	joint->anchorB = *anchorB;

	joint->angleLimitsX[0] = minX;
	joint->angleLimitsX[1] = maxX;
	joint->angleLimitsY[0] = minY;
	joint->angleLimitsY[1] = maxY;
	joint->angleLimitsZ[0] = minZ;
	joint->angleLimitsZ[1] = maxZ;

	vec3InitZero(&joint->linearImpulse);
}


#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(const physicsJointSphere *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	vec3 angularImpulse;
	vec3 twistImpulse;

	// The angular impulse is the sum of the swing and twist impulses.
	// We recalculate them here because we need the new swing and twist axes.
	vec3MultiplySOut(&joint->swingAxis, joint->swingImpulse, &angularImpulse);
	vec3MultiplySOut(&joint->twistAxis, joint->twistImpulse, &twistImpulse);
	vec3AddVec3(&angularImpulse, &twistImpulse);

	// Apply the accumulated impulses.
	physRigidBodyApplyImpulseBoostInverse(bodyA, &joint->rA, &joint->linearImpulse, &angularImpulse);
	physRigidBodyApplyImpulseBoost(bodyB, &joint->rB, &joint->linearImpulse, &angularImpulse);
}
#endif

/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointSpherePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
){

	updateConstraintData((physicsJointSphere *)joint, bodyA, bodyB);
	// We actually compute the swing and twist axes in "calculateBias"
	// rather than in "updateConstraintData", as it makes things easier.
	calculateBias((physicsJointSphere *)joint, bodyA, bodyB, 1.f/dt);
	calculateLinearMass(
		((physicsJointSphere *)joint)->rA, ((physicsJointSphere *)joint)->rB,
		bodyA, bodyB, &((physicsJointSphere *)joint)->linearMass
	);
	calculateInverseAngularMass(
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->swingAxis,
		&((physicsJointSphere *)joint)->twistAxis,
		&((physicsJointSphere *)joint)->swingInvMass,
		&((physicsJointSphere *)joint)->twistInvMass
	);

	// If the angular constraints aren't being
	// violated, we should reset their accumulators.
	if(((physicsJointSphere *)joint)->swingBias >= 0.f){
		((physicsJointSphere *)joint)->swingImpulse = 0.f;
	}
	if(((physicsJointSphere *)joint)->twistBias >= 0.f){
		((physicsJointSphere *)joint)->twistImpulse = 0.f;
	}
	#ifdef PHYSJOINTSPHERE_WARM_START
	physJointSphereWarmStart((physicsJointSphere *)joint, bodyA, bodyB);
	#endif
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointSphereSolveVelocity(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 relativeVelocity;
	vec3 impulse;

	// Solve the angular swing and twist constraints.
	{
		float lambda;
		float oldImpulse;
		vec3 tempImpulse;

		// w_relative = wB - wA
		vec3SubtractVec3FromOut(&bodyB->angularVelocity, &bodyA->angularVelocity, &relativeVelocity);


		if(((physicsJointSphere *)joint)->swingBias < 0.f){
			// Calculate the Lagrange multiplier for the swing constraint.
			// lambda = -(J2*V + b2)/(J2*M^(-1)*J2^T)
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->swingAxis) +
				((physicsJointSphere *)joint)->swingBias
			) * ((physicsJointSphere *)joint)->swingInvMass;

			// Clamp the accumulated swing impulse magnitude.
			oldImpulse = ((physicsJointSphere *)joint)->swingImpulse;
			((physicsJointSphere *)joint)->swingImpulse = maxFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->swingImpulse - oldImpulse;

			// Calculate the swing impulse.
			vec3MultiplySOut(&((physicsJointSphere *)joint)->swingAxis, lambda, &impulse);
		}else{
			vec3InitZero(&impulse);
		}


		if(((physicsJointSphere *)joint)->twistBias < 0.f){
			// Calculate the Lagrange multiplier for the twist constraint.
			// lambda = -(J3*V + b3)/(J3*M^(-1)*J3^T)
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->twistAxis) +
				((physicsJointSphere *)joint)->twistBias
			) * ((physicsJointSphere *)joint)->twistInvMass;

			// Clamp the accumulated twist impulse magnitude.
			oldImpulse = ((physicsJointSphere *)joint)->twistImpulse;
			((physicsJointSphere *)joint)->twistImpulse = maxFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->twistImpulse - oldImpulse;

			// Calculate the twist impulse and add it to the swing impulse.
			vec3MultiplySOut(&((physicsJointSphere *)joint)->twistAxis, lambda, &tempImpulse);
			vec3AddVec3(&impulse, &tempImpulse);
		}


		// Apply the correctional impulse.
		physRigidBodyApplyAngularImpulseInverse(bodyA, impulse);
		physRigidBodyApplyAngularImpulse(bodyB, impulse);
	}

	// Solve the linear point-to-point constraint last,
	// as it's more important that it's satisfied.
	{
		// v_socket   = vA + wA X rA
		// v_ball     = vB + wB X rB
		// v_relative = v_ball - v_socket

		// Calculate the total velocity of the socket.
		vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointSphere *)joint)->rA, &impulse);
		vec3AddVec3(&impulse, &bodyA->linearVelocity);
		// Calculate the total velocity of the ball.
		vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointSphere *)joint)->rB, &relativeVelocity);
		vec3AddVec3(&relativeVelocity, &bodyB->linearVelocity);
		// Calculate the relative velocity between the ball and socket.
		// Note that we actually compute -v_relative here.
		vec3SubtractFromVec3(&relativeVelocity, &impulse);


		// Solve for lambda using Cramer's rule:
		// JV = v_relative,
		// K1*lambda = -JV.
		mat3Solve(&((physicsJointSphere *)joint)->linearMass, &relativeVelocity, &impulse);
		vec3AddVec3(&((physicsJointSphere *)joint)->linearImpulse, &impulse);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulseInverse(bodyA, &((physicsJointSphere *)joint)->rA, &impulse);
		physRigidBodyApplyImpulse(bodyB, &((physicsJointSphere *)joint)->rB, &impulse);
	}
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
return_t physJointSphereSolvePosition(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	#ifdef PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL
	float angularError;
	float linearError;
	vec3 impulse;


	// Solve the angular swing and twist constraints.
	{
		float swingAngle;
		vec3 twistImpulse = {.x = 1.f, .y = 0.f, .z = 0.f};
		float twistAngle;

		float swingInvMass;
		float twistInvMass;


		{
			quat qAB;
			quat swing, twist;

			// Get the relative orientation from body A to body B:
			// qAB = conj(A)*B.
			quatMultiplyQuatConjByOut(bodyB->state.rot, bodyA->state.rot, &qAB);
			// Decompose the relative orientation into swing and twist components.
			quatSwingTwistFaster(&qAB, &twistImpulse, &twist, &swing);
			// The twist quaternion's axis may be antiparallel to the twist axis,
			// but we can just negate the quaternion due to double coverage.
			if(twist.x < 0.f){
				twist.x = -twist.x;
				twist.w = -twist.w;
			}
			// We need to normalize the swing and twist quaternions, just in case.
			quatNormalizeQuatFast(&swing);
			quatNormalizeQuatFast(&twist);

			swingAngle = clampFloat(
				calculateSwingLimit(
					&swing, &bodyA->state.rot,
					((physicsJointSphere *)joint)->angleLimitsY,
					((physicsJointSphere *)joint)->angleLimitsZ,
					&impulse
				) + PHYSCONSTRAINT_ANGULAR_SLOP,
				-PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION, 0.f
			);
			twistAngle = clampFloat(
				calculateTwistLimit(
					&twist, &bodyA->state.rot,
					((physicsJointSphere *)joint)->angleLimitsX,
					&twistImpulse
				) + PHYSCONSTRAINT_ANGULAR_SLOP,
				-PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION, 0.f
			);
		}

		// Calculate the angular error. The swing and twist
		// angles should always be negative since we clamp
		// them, but we want to sum their absolute values.
		angularError = -(swingAngle + twistAngle);


		calculateInverseAngularMass(bodyA, bodyB, &impulse, &twistImpulse, &swingInvMass, &twistInvMass);
		// lambda = -C2/K2
		vec3MultiplyS(&impulse, -swingAngle*swingInvMass);
		// lambda = -C3/K3
		vec3MultiplyS(&twistImpulse, -twistAngle*twistInvMass);
		vec3AddVec3(&impulse, &twistImpulse);


		// Apply the correctional impulse.
		physRigidBodyApplyAngularImpulsePositionInverse(bodyA, impulse);
		physRigidBodyApplyAngularImpulsePosition(bodyB, impulse);
	}


	// Solve the linear point-to-point constraint.
	{
		vec3 rA;
		vec3 rB;
		vec3 constraint;
		mat3 linearMass;

		vec3MultiplyVec3Out(&bodyA->state.scale, &((physicsJointSphere *)joint)->anchorA, &rA);
		quatRotateVec3Fast(&bodyA->state.rot, &rA);
		vec3MultiplyVec3Out(&bodyB->state.scale, &((physicsJointSphere *)joint)->anchorB, &rB);
		quatRotateVec3Fast(&bodyB->state.rot, &rB);

		// Calculate the displacement from the ball to the socket:
		// -C1 = (pA + rA) - (pB - rB).
		vec3AddVec3Out(&bodyA->centroid, &rA, &constraint);
		vec3SubtractVec3From(&constraint, &bodyB->centroid);
		vec3SubtractVec3From(&constraint, &rB);
		linearError = vec3MagnitudeVec3(&constraint);

		// Solve for the impulse:
		// K1*lambda = -C1.
		calculateLinearMass(rA, rB, bodyA, bodyB, &linearMass);
		mat3Solve(&linearMass, &constraint, &impulse);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &impulse);
		physRigidBodyApplyImpulsePosition(bodyB, &rB, &impulse);
	}


	return(
		linearError <= PHYSCONSTRAINT_LINEAR_SLOP &&
		angularError <= PHYSCONSTRAINT_LINEAR_SLOP
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
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	// Transform the axis points using the bodies' new scales and rotations.
	vec3MultiplyVec3Out(&bodyA->state.scale, &joint->anchorA, &joint->rA);
	quatRotateVec3Fast(&bodyA->state.rot, &joint->rA);
	vec3MultiplyVec3Out(&bodyB->state.scale, &joint->anchorB, &joint->rB);
	quatRotateVec3Fast(&bodyB->state.rot, &joint->rB);
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
	// K1 = (mA^(-1) + mB^(-1))*I_3 - [rA]_X*IA^(-1)*[rA]_X - [rB]_X*IB^(-1)*[rB]_X.
	//
	// This probably isn't so good, as [rA]_X*IA^(-1)*[rA]_X and [rB]_X*IB^(-1)*[rB]_X
	// are necessarily singular. We don't invert the matrix here, as it's faster to
	// solve for lambda using Cramer's rule.

	// Compute body A's contribution to K1.
	{
		// Note that the matrix [rA]_X*IA^(-1)*[rA]_X is symmetric, so we
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

		// Compute -[rA]_X*IA^(-1)*[rA]_X.
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

		// Compute (mA^(-1) + mB^(-1))*I_3 - [rB]_X*IB^(-1)*[rB]_X.
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

/*
** Calculate the inverse angular effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once per update.
*/
static void calculateInverseAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const vec3 *const restrict swingAxis, const vec3 *const restrict twistAxis,
	float *const restrict swingInvMass, float *const restrict twistInvMass
){

	mat3 invInertiaSum;
	vec3 IABa;
	mat3AddMat3Out(&bodyA->invInertiaGlobal, &bodyB->invInertiaGlobal, &invInertiaSum);

	// Swing inverse effective mass (K2).
	mat3MultiplyByVec3Out(&invInertiaSum, swingAxis, &IABa);
	*swingInvMass = 1.f / vec3DotVec3(swingAxis, &IABa);

	// Twist inverse effective mass (K3).
	mat3MultiplyByVec3Out(&invInertiaSum, twistAxis, &IABa);
	*twistInvMass = 1.f / vec3DotVec3(twistAxis, &IABa);
}

/*
** Calculate the joint's bias term. This is used for
** clamping the relative orientation of rigid body B.
*/
static void calculateBias(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float frequency
){

	// We need to calculate the new swing
	// and twist axes and the angle limits.
	//
	// Ideally, we would calculate the axes
	// in "updateConstraintData", but doing
	// it here just makes things easier.

	quat qAB;
	quat swing, twist;

	// Get the relative orientation from body A to body B:
	// qAB = conj(A)*B.
	quatMultiplyQuatConjByOut(bodyB->state.rot, bodyA->state.rot, &qAB);
	// Decompose the relative orientation into swing and twist components.
	vec3InitSet(&joint->twistAxis, 1.f, 0.f, 0.f);
	quatSwingTwistFaster(&qAB, &joint->twistAxis, &twist, &swing);
	// The twist quaternion's axis may be antiparallel to the twist axis,
	// but we can just negate the quaternion due to double coverage.
	if(twist.x < 0.f){
		twist.x = -twist.x;
		twist.w = -twist.w;
	}
	// We need to normalize the swing and twist quaternions, just in case.
	quatNormalizeQuatFast(&swing);
	quatNormalizeQuatFast(&twist);

	joint->swingBias = calculateSwingLimit(
		&swing, &bodyA->state.rot,
		joint->angleLimitsY,
		joint->angleLimitsZ,
		&joint->swingAxis
	) * frequency;
	joint->twistBias = calculateTwistLimit(
		&twist, &bodyA->state.rot,
		joint->angleLimitsX,
		&joint->twistAxis
	) * frequency;
}


// Calculate the swing axis and return the bias!
static float calculateSwingLimit(
	const quat *const restrict swing, const quat *const restrict rotA,
	const float *const restrict angleLimitsY, const float *const restrict angleLimitsZ,
	vec3 *const restrict swingAxis
){

	// Bring the swing axis from body B's space to global space.
	quatAxis(swing, swingAxis);
	// Recall that our constraints specify that C' <= 0.
	// In order to solve these as C' >= 0 constraints like
	// usual, we need to negate the swing and twist axes.
	swingAxis->y = -swingAxis->y;
	swingAxis->z = -swingAxis->z;
	quatRotateVec3Fast(rotA, swingAxis);

	return(
		clampEllipseDistanceFast(
			swing->z, swing->y,
			(swing->z > 0.f) ? angleLimitsZ[1] : -angleLimitsZ[0],
			(swing->y > 0.f) ? angleLimitsY[1] : -angleLimitsY[0]
		) - quatAngle(swing)
	);
}

/*
** Calculate the twist axis and return the bias!
**
** This assumes that the twist axis has been partially
** set already, and needs only be rotated and inverted.
*/
static float calculateTwistLimit(
	const quat *const restrict twist, const quat *const restrict rotA,
	const float *const restrict angleLimitsX,
	vec3 *const restrict twistAxis
){

	float twistAngle = quatAngle(twist);
	// Ensure the angle is in the range (-pi, pi].
	// We'll also need to flip the twist axis.
	if(twistAngle > M_PI){
		twistAngle -= 2.f*M_PI;

	// Rather than flipping the twist axis above, we
	// do it here, as we need to negate it later anyway.
	}else{
		twistAxis->x = -1.f;
	}
	// Bring the twist axis from body B's space to global space.
	quatRotateVec3Fast(rotA, twistAxis);

	return(minFloat(
		twistAngle - angleLimitsX[0],
		angleLimitsX[1] - twistAngle
	));
}