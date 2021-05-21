#include "physicsJointSphere.h"


#include "physicsRigidBody.h"

#include "vec2.h"
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
**
** We also need to impose constraints on the relative orientation
** of the rigid bodies, so if "(RB - RA) . a" represents the angle
** of the relative orientation along the axis a, then we require
**
** C2 : (RB - RA) . s - s_lim <= 0,
** C3 : (RB - RA) . t - t_lim <= 0,
**
** where s denotes the swing axis, t denotes the twist axis,
** and s_lim and t_lim are the swing and twist axis limits
** respectively. The twist axis is chosen as body B's x-axis.
**
** The velocity constraints for C2 and C3 are
**
** C2' : (wB - wA) . s <= 0,
** C3' : (wB - wA) . t <= 0.
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
** Note that if a_lim is the axis limit and "(RB - RA) . a"
** is the relative angle along the axis a, then
**
** (a_lim - (RB - RA) . a)/dt = -C/dt
**
** is the angular velocity required for the limit to be reached.
** Note that to prevent all rotation along a, we apply the impulse
**
** lambda = -JV/K <= 0.
**
** To prevent rotation along a only when the limit has
** been exceeded, we should instead apply the impulse
**
** lambda = -(JV + C/dt)/K.
**
** Hence, we define the biases
**
** b2 = C2/dt,
** b3 = C3/dt.
**
** ----------------------------------------------------------------------
**
** Although undiscussed here, we support rectangular angle limits
** using Euler angles rather than the conical limit that swing-twist
** decomposition gives.
**
** This is implemented by effectively having a twist constraint
** for all three axes rather than just the x-axis.
**
** ----------------------------------------------------------------------
*/
#warning "We still need to implement some sort of restitution."


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
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static void calculateInverseAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const vec3 *const restrict swingAxis, const vec3 *const restrict twistAxis,
	float *const restrict swingInvMass, float *const restrict twistInvMass
);
#else
static void calculateInverseAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const vec3 *const restrict angularAxisX, const vec3 *const restrict angularAxisY, const vec3 *const restrict angularAxisZ,
	vec3 *const restrict angularInvMass
);
#endif
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static void calculateBias(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float *const restrict angularLimitsX, const float *const restrict angularLimitsY, const float *const restrict angularLimitsZ,
	vec3 *const restrict swingAxis, float *const restrict swingBias,
	vec3 *const restrict twistAxis, float *const restrict twistBias
);
#else
static void calculateBias(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float *const restrict angularLimitsX, const float *const restrict angularLimitsY, const float *const restrict angularLimitsZ,
	vec3 *const restrict angularAxisX, vec3 *const restrict angularAxisY, vec3 *const restrict angularAxisZ,
	vec3 *const restrict angularBias
);
#endif

#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static float calculateSwingLimit(
	const quat *const restrict swing, const quat *const restrict rotA,
	const float *const restrict angularLimitsY, const float *const restrict angularLimitsZ,
	vec3 *const restrict swingAxis
);
#endif
static float calculateAngularLimit(
	const float angle, const quat *const restrict rotA,
	const float *const restrict angularLimits,
	vec3 *const restrict axis
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

	joint->angularLimitsX[0] = minX;
	joint->angularLimitsX[1] = maxX;
	joint->angularLimitsY[0] = minY;
	joint->angularLimitsY[1] = maxY;
	joint->angularLimitsZ[0] = minZ;
	joint->angularLimitsZ[1] = maxZ;

	vec3InitZero(&joint->linearImpulse);
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	joint->swingImpulse = 0.f;
	joint->twistImpulse = 0.f;
	#else
	vec3InitZero(&joint->angularImpulse);
	#endif
}


#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(const physicsJointSphere *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	vec3 angularImpulse;

	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	// The angular impulse is the sum of the swing and twist impulses.
	// We recalculate them here because we need the new swing and twist axes.
	vec3MultiplySOut(&joint->swingAxis, joint->swingImpulse, &angularImpulse);
	vec3Fmaf(joint->twistImpulse, &joint->twistAxis, &angularImpulse);
	#else
	// The angular impulse is the sum of the three axis impulses.
	// We recalculate them here because we need the new constraint axes.
	vec3MultiplySOut(&joint->angularAxisX, joint->angularImpulse.x, &angularImpulse);
	vec3Fmaf(joint->angularImpulse.y, &joint->angularAxisY, &angularImpulse);
	vec3Fmaf(joint->angularImpulse.z, &joint->angularAxisZ, &angularImpulse);
	#endif

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

	const float frequency = 1.f/dt;

	updateConstraintData((physicsJointSphere *)joint, bodyA, bodyB);

	// We actually compute the constraint axes in "calculateBias"
	// rather than in "updateConstraintData", as it makes things easier.
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	calculateBias(
		bodyA, bodyB,
		((physicsJointSphere *)joint)->angularLimitsX, ((physicsJointSphere *)joint)->angularLimitsY, ((physicsJointSphere *)joint)->angularLimitsZ,
		&((physicsJointSphere *)joint)->swingAxis, &((physicsJointSphere *)joint)->swingBias,
		&((physicsJointSphere *)joint)->twistAxis, &((physicsJointSphere *)joint)->twistBias
	);
	// b2 = C2/dt <= 0
	((physicsJointSphere *)joint)->swingBias = minFloat(0.f, ((physicsJointSphere *)joint)->swingBias * frequency);
	// b3 = C3/dt <= 0
	((physicsJointSphere *)joint)->twistBias = minFloat(0.f, ((physicsJointSphere *)joint)->twistBias * frequency);

	calculateInverseAngularMass(
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->swingAxis,
		&((physicsJointSphere *)joint)->twistAxis,
		&((physicsJointSphere *)joint)->swingInvMass,
		&((physicsJointSphere *)joint)->twistInvMass
	);
	#else
	calculateBias(
		bodyA, bodyB,
		((physicsJointSphere *)joint)->angularLimitsX, ((physicsJointSphere *)joint)->angularLimitsY, ((physicsJointSphere *)joint)->angularLimitsZ,
		&((physicsJointSphere *)joint)->angularAxisX, &((physicsJointSphere *)joint)->angularAxisY, &((physicsJointSphere *)joint)->angularAxisZ,
		&((physicsJointSphere *)joint)->angularBias
	);
	// b = C/dt <= 0
	((physicsJointSphere *)joint)->angularBias.x = minFloat(0.f, ((physicsJointSphere *)joint)->angularBias.x * frequency);
	((physicsJointSphere *)joint)->angularBias.y = minFloat(0.f, ((physicsJointSphere *)joint)->angularBias.y * frequency);
	((physicsJointSphere *)joint)->angularBias.z = minFloat(0.f, ((physicsJointSphere *)joint)->angularBias.z * frequency);

	calculateInverseAngularMass(
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->angularAxisX, &((physicsJointSphere *)joint)->angularAxisY, &((physicsJointSphere *)joint)->angularAxisZ,
		&((physicsJointSphere *)joint)->angularInvMass
	);
	#endif

	calculateLinearMass(
		((physicsJointSphere *)joint)->rA, ((physicsJointSphere *)joint)->rB,
		bodyA, bodyB, &((physicsJointSphere *)joint)->linearMass
	);

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

		// w_relative = wB - wA
		vec3SubtractVec3FromOut(&bodyB->angularVelocity, &bodyA->angularVelocity, &relativeVelocity);


		#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
		{
			// Calculate the Lagrange multiplier for the swing constraint.
			// lambda = -(J2*V + b2)/(J2*M^(-1)*J2^T)
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->swingAxis) +
				((physicsJointSphere *)joint)->swingBias
			) * ((physicsJointSphere *)joint)->swingInvMass;

			// Clamp the accumulated swing impulse magnitude.
			oldImpulse = ((physicsJointSphere *)joint)->swingImpulse;
			((physicsJointSphere *)joint)->swingImpulse = minFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->swingImpulse - oldImpulse;

			// Calculate the swing impulse.
			vec3MultiplySOut(&((physicsJointSphere *)joint)->swingAxis, lambda, &impulse);
		}

		{
			// Calculate the Lagrange multiplier for the twist constraint.
			// lambda = -(J3*V + b3)/(J3*M^(-1)*J3^T)
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->twistAxis) +
				((physicsJointSphere *)joint)->twistBias
			) * ((physicsJointSphere *)joint)->twistInvMass;

			// Clamp the accumulated twist impulse magnitude.
			oldImpulse = ((physicsJointSphere *)joint)->twistImpulse;
			((physicsJointSphere *)joint)->twistImpulse = minFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->twistImpulse - oldImpulse;

			// Calculate the twist impulse and add it to the swing impulse.
			vec3Fmaf(lambda, &((physicsJointSphere *)joint)->twistAxis, &impulse);
		}
		#else
		// Solve for the angular impulse along the x-axis.
		{
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->angularAxisX) +
				((physicsJointSphere *)joint)->angularBias.x
			) * ((physicsJointSphere *)joint)->angularInvMass.x;

			oldImpulse = ((physicsJointSphere *)joint)->angularImpulse.x;
			((physicsJointSphere *)joint)->angularImpulse.x = minFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->angularImpulse.x - oldImpulse;

			vec3MultiplySOut(&((physicsJointSphere *)joint)->angularAxisX, lambda, &impulse);
		}

		// Solve for the angular impulse along the y-axis.
		{
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->angularAxisY) +
				((physicsJointSphere *)joint)->angularBias.y
			) * ((physicsJointSphere *)joint)->angularInvMass.y;

			oldImpulse = ((physicsJointSphere *)joint)->angularImpulse.y;
			((physicsJointSphere *)joint)->angularImpulse.y = minFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->angularImpulse.y - oldImpulse;

			vec3Fmaf(lambda, &((physicsJointSphere *)joint)->angularAxisY, &impulse);
		}

		// Solve for the angular impulse along the z-axis.
		{
			lambda = -(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->angularAxisZ) +
				((physicsJointSphere *)joint)->angularBias.z
			) * ((physicsJointSphere *)joint)->angularInvMass.z;

			oldImpulse = ((physicsJointSphere *)joint)->angularImpulse.z;
			((physicsJointSphere *)joint)->angularImpulse.z = minFloat(0.f, oldImpulse + lambda);
			lambda = ((physicsJointSphere *)joint)->angularImpulse.z - oldImpulse;

			vec3Fmaf(lambda, &((physicsJointSphere *)joint)->angularAxisZ, &impulse);
		}
		#endif


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


	// Solve the angular constraints.
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	{
		float swingAngle;
		vec3 twistImpulse;
		float twistAngle;

		float swingInvMass;
		float twistInvMass;


		calculateBias(
			bodyA, bodyB,
			((physicsJointSphere *)joint)->angularLimitsX, ((physicsJointSphere *)joint)->angularLimitsY, ((physicsJointSphere *)joint)->angularLimitsZ,
			&impulse, &swingAngle,
			&twistImpulse, &twistAngle
		);
		swingAngle = clampFloat(swingAngle - PHYSCONSTRAINT_ANGULAR_SLOP, 0.f, PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION);
		twistAngle = clampFloat(twistAngle - PHYSCONSTRAINT_ANGULAR_SLOP, 0.f, PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION);

		// Calculate the angular error.
		// These angles are always positive.
		angularError = swingAngle + twistAngle;


		calculateInverseAngularMass(bodyA, bodyB, &impulse, &twistImpulse, &swingInvMass, &twistInvMass);
		// lambda = -C2/K2
		vec3MultiplyS(&impulse, -swingAngle*swingInvMass);
		// lambda = -C3/K3
		vec3Fmaf(-twistAngle*twistInvMass, &twistImpulse, &impulse);


		// Apply the correctional impulse.
		physRigidBodyApplyAngularImpulsePositionInverse(bodyA, impulse);
		physRigidBodyApplyAngularImpulsePosition(bodyB, impulse);
	}
	#else
	{
		vec3 angularImpulseY;
		vec3 angularImpulseZ;
		vec3 angularBias;

		vec3 angularInvMass;


		calculateBias(
			bodyA, bodyB,
			((physicsJointSphere *)joint)->angularLimitsX, ((physicsJointSphere *)joint)->angularLimitsY, ((physicsJointSphere *)joint)->angularLimitsZ,
			&impulse, &angularImpulseY, &angularImpulseZ,
			&angularBias
		);
		angularBias.x = clampFloat(angularBias.x - PHYSCONSTRAINT_ANGULAR_SLOP, 0.f, PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION);
		angularBias.y = clampFloat(angularBias.y - PHYSCONSTRAINT_ANGULAR_SLOP, 0.f, PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION);
		angularBias.z = clampFloat(angularBias.z - PHYSCONSTRAINT_ANGULAR_SLOP, 0.f, PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION);

		// Calculate the angular error.
		// These angles are always positive.
		angularError = angularBias.x + angularBias.y + angularBias.z;


		calculateInverseAngularMass(bodyA, bodyB, &impulse, &angularImpulseY, &angularImpulseZ, &angularInvMass);
		// lambda = -C/K
		vec3MultiplyS(&impulse, -angularBias.x*angularInvMass.x);
		vec3Fmaf(-angularBias.y*angularInvMass.y, &angularImpulseY, &impulse);
		vec3Fmaf(-angularBias.z*angularInvMass.z, &angularImpulseZ, &impulse);


		// Apply the correctional impulse.
		physRigidBodyApplyAngularImpulsePositionInverse(bodyA, impulse);
		physRigidBodyApplyAngularImpulsePosition(bodyB, impulse);
	}
	#endif


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
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static void calculateInverseAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const vec3 *const restrict swingAxis, const vec3 *const restrict twistAxis,
	float *const restrict swingInvMass, float *const restrict twistInvMass
){

	mat3 invInertiaSum;
	vec3 IABa;
	mat3AddMat3Out(&bodyA->invInertiaGlobal, &bodyB->invInertiaGlobal, &invInertiaSum);

	// Swing inverse effective mass (K2).
	mat3MultiplyVec3ByOut(&invInertiaSum, swingAxis, &IABa);
	*swingInvMass = 1.f / vec3DotVec3(swingAxis, &IABa);

	// Twist inverse effective mass (K3).
	mat3MultiplyVec3ByOut(&invInertiaSum, twistAxis, &IABa);
	*twistInvMass = 1.f / vec3DotVec3(twistAxis, &IABa);
}
#else
static void calculateInverseAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const vec3 *const restrict angularAxisX, const vec3 *const restrict angularAxisY, const vec3 *const restrict angularAxisZ,
	vec3 *const restrict angularInvMass
){

	mat3 invInertiaSum;
	vec3 IABa;
	mat3AddMat3Out(&bodyA->invInertiaGlobal, &bodyB->invInertiaGlobal, &invInertiaSum);

	// This is equivalent to multiplying the sum of the inertia tensors
	// by a matrix whose columns are given by the constraint axes.

	// Inverse effective mass for the x-axis.
	mat3MultiplyVec3ByOut(&invInertiaSum, angularAxisX, &IABa);
	angularInvMass->x = 1.f / vec3DotVec3(angularAxisX, &IABa);

	// Inverse effective mass for the y-axis.
	mat3MultiplyVec3ByOut(&invInertiaSum, angularAxisY, &IABa);
	angularInvMass->y = 1.f / vec3DotVec3(angularAxisY, &IABa);

	// Inverse effective mass for the z-axis.
	mat3MultiplyVec3ByOut(&invInertiaSum, angularAxisZ, &IABa);
	angularInvMass->z = 1.f / vec3DotVec3(angularAxisZ, &IABa);
}
#endif

/*
** Calculate the joint's bias term. This is used for
** clamping the relative orientation of rigid body B.
*/
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static void calculateBias(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float *const restrict angularLimitsX, const float *const restrict angularLimitsY, const float *const restrict angularLimitsZ,
	vec3 *const restrict swingAxis, float *const restrict swingBias,
	vec3 *const restrict twistAxis, float *const restrict twistBias
){

	// We need to calculate the new swing
	// and twist axes and the angle limits.
	//
	// Ideally, we would calculate the axes
	// in "updateConstraintData", but doing
	// it here just makes things easier.

	quat qAB;
	quat swing, twist;
	float twistAngle;

	// Get the relative orientation from body A to body B:
	// qAB = conj(A)*B.
	quatMultiplyConjByQuatOut(bodyB->state.rot, bodyA->state.rot, &qAB);
	// Decompose the relative orientation into swing and twist components.
	vec3InitSet(twistAxis, 1.f, 0.f, 0.f);
	quatSwingTwistFaster(&qAB, twistAxis, &twist, &swing);
	// We need to normalize the swing and twist quaternions, just in case.
	quatNormalizeQuatFast(&swing);
	quatNormalizeQuatFast(&twist);

	*swingBias = calculateSwingLimit(
		&swing, &bodyA->state.rot,
		angularLimitsY,
		angularLimitsZ,
		swingAxis
	);

	// If the twist quaternion's axis is antiparallel to the
	// twist axis, we need to negate the axis and the angle.
	//
	// This should only occur when the angle is greater than
	// pi, which corresponds to a negative angle (and axis).
	if(twist.x < 0.f){
		twistAxis->x = -1.f;
		twistAngle = -quatAngle(&twist);
	}else{
		twistAngle = quatAngle(&twist);
	}
	*twistBias = calculateAngularLimit(
		twistAngle, &bodyA->state.rot,
		angularLimitsX,
		twistAxis
	);
}
#else
static void calculateBias(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float *const restrict angularLimitsX, const float *const restrict angularLimitsY, const float *const restrict angularLimitsZ,
	vec3 *const restrict angularAxisX, vec3 *const restrict angularAxisY, vec3 *const restrict angularAxisZ,
	vec3 *const restrict angularBias
){

	// We need to calculate the new constraint axes and the angle limits.
	//
	// Ideally, we would calculate the axes in "updateConstraintData",
	// but doing it here just makes things easier.

	quat qAB;
	vec3 angles;

	// Get the relative orientation from body A to body B:
	// qAB = conj(A)*B.
	quatMultiplyConjByQuatOut(bodyB->state.rot, bodyA->state.rot, &qAB);
	// Decompose the relative orientation into x, y and z components.
	quatToEulerAnglesZXY(qAB, &angles);
	// If an angle is negative, we should negate its constraint axis.
	vec3InitSet(angularAxisX, (angles.x < 0.f) ? -1.f : 1.f, 0.f, 0.f);
	vec3InitSet(angularAxisY, 0.f, (angles.y < 0.f) ? -1.f : 1.f, 0.f);
	vec3InitSet(angularAxisZ, 0.f, 0.f, (angles.z < 0.f) ? -1.f : 1.f);

	angularBias->x = calculateAngularLimit(
		angles.x, &bodyA->state.rot,
		angularLimitsX,
		angularAxisX
	);
	angularBias->y = calculateAngularLimit(
		angles.y, &bodyA->state.rot,
		angularLimitsY,
		angularAxisY
	);
	angularBias->z = calculateAngularLimit(
		angles.z, &bodyA->state.rot,
		angularLimitsZ,
		angularAxisZ
	);
}
#endif


#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
/*
** Calculate the swing axis and return the bias!
** The return value should be negative when the constraint is satisfied.
*/
static float calculateSwingLimit(
	const quat *const restrict swing, const quat *const restrict rotA,
	const float *const restrict angularLimitsY, const float *const restrict angularLimitsZ,
	vec3 *const restrict swingAxis
){

	#ifndef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
	// Note that when there is no relative rotation,
	// the swing axis will be parallel to the twist axis.
	//
	// Bullet corrects this during the velocity solver,
	// but I'm not sure it's really worth worrying about.
	quatAxis(swing, swingAxis);
	// Bring the swing axis from body B's space to global space.
	quatRotateVec3Fast(rotA, swingAxis);

	return(
		quatAngle(swing) - clampEllipseDistanceFast(
			swing->z, swing->y,
			(swing->z > 0.f) ? angularLimitsZ[1] : -angularLimitsZ[0],
			(swing->y > 0.f) ? angularLimitsY[1] : -angularLimitsY[0]
		)
	);
	#else
	vec2 ellipseNormal;
	float swingLimit;

	// Note that the swing axis is identical to the quaternion's imaginary
	// part except for a factor of sin(theta/2), we we can use either here.
	swingLimit = clampEllipseDistanceNormalFast(
		swing->z, swing->y,
		(swing->z > 0.f) ? angularLimitsZ[1] : -angularLimitsZ[0],
		(swing->y > 0.f) ? angularLimitsY[1] : -angularLimitsY[0],
		&ellipseNormal
	);
	// The ellipse normal we compute is unit length, so since the
	// swing axis is either of the form (1, 0, 0) or (0, y, z),
	// we don't need to normalize the new swing axis.
	swingAxis->x = 0.f;
	swingAxis->y = ellipseNormal.y;
	swingAxis->z = ellipseNormal.x;
	// Bring the swing axis from body B's space to global space.
	quatRotateVec3Fast(rotA, swingAxis);

	return(quatAngle(swing) - swingLimit);
	#endif
}
#endif

/*
** Calculate the angular constraint for some axis and return the bias!
** The return value should be negative when the constraint is satisfied.
**
** This assumes that the twist axis supplied has been
** partially set already, and needs only be rotated.
*/
static float calculateAngularLimit(
	const float angle, const quat *const restrict rotA,
	const float *const restrict angularLimits,
	vec3 *const restrict axis
){

	// Bring the constraint axis from body B's space to global space.
	quatRotateVec3Fast(rotA, axis);

	return(maxFloat(
		angularLimits[0] - angle,
		angle - angularLimits[1]
	));
}