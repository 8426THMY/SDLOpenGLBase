#include "physicsJointSphere.h"


#include "physicsRigidBody.h"

#include "vec2.h"
#include "quat.h"
#include "utilMath.h"


#define PHYSJOINTSPHERE_LIMITS_FREE        0x00
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
#define PHYSJOINTSPHERE_LIMITS_SWING       0x01
#define PHYSJOINTSPHERE_LIMITS_TWIST_LOWER 0x02
#define PHYSJOINTSPHERE_LIMITS_TWIST_UPPER 0x04
#define PHYSJOINTSPHERE_LIMITS_TWIST       (PHYSJOINTSPHERE_LIMITS_TWIST_LOWER | PHYSJOINTSPHERE_LIMITS_TWIST_UPPER)
#else
#define PHYSJOINTSPHERE_LIMITS_X_LOWER     0x01
#define PHYSJOINTSPHERE_LIMITS_X_UPPER     0x02
#define PHYSJOINTSPHERE_LIMITS_X           (PHYSJOINTSPHERE_LIMITS_X_LOWER | PHYSJOINTSPHERE_LIMITS_X_UPPER)
#define PHYSJOINTSPHERE_LIMITS_Y_LOWER     0x04
#define PHYSJOINTSPHERE_LIMITS_Y_UPPER     0x08
#define PHYSJOINTSPHERE_LIMITS_Y           (PHYSJOINTSPHERE_LIMITS_Y_LOWER | PHYSJOINTSPHERE_LIMITS_Y_UPPER)
#define PHYSJOINTSPHERE_LIMITS_Z_LOWER     0x10
#define PHYSJOINTSPHERE_LIMITS_Z_UPPER     0x20
#define PHYSJOINTSPHERE_LIMITS_Z           (PHYSJOINTSPHERE_LIMITS_Z_LOWER | PHYSJOINTSPHERE_LIMITS_Z_UPPER)
#endif


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
** C2 : (RB - RA) . s - s_limit <= 0,
** C3 : (RB - RA) . t - t_upper <= 0,
** C4 : (RB - RA) . t - t_lower >= 0.
**
** where s denotes the swing axis, t denotes the twist axis,
** and s_limit, t_upper and t_lower are the swing and twist
** axis limits. The twist axis is chosen as body B's x-axis.
**
** The velocity constraints for these are
**
** C2' : (wB - wA) . s <= 0,
** C3' : (wB - wA) . t <= 0,
** C4' : (wB - wA) . t >= 0.
**
** Note that constraints C3' and C4' appear to conflict.
** This is fine, as we only solve these constraints when
** they're broken, and it's impossible to break both the
** upper and lower limits simultaneously.
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
** J3 = [0, -t, 0, t] = J4,
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
** and similarly for K3 = K4. Note that K1 is a 3x3 matrix,
** but K2, K3 and K4 are simply scalars.
**
** ----------------------------------------------------------------------
**
** To limit the relative orientation of rigid body B with
** respect to rigid body A, we use swing-twist decomposition
** and constrain the swing and twist axes individually.
**
** Note that if a_limit is the axis limit and "(RB - RA) . a"
** is the relative angle along the axis a, then
**
** b = ((RB - RA) . a - a_limit)/dt = C/dt
**
** is the angular velocity required for the limit to be reached.
** Note that to prevent all rotation along a, we apply the impulse
**
** lambda = -JV/K <= 0.
**
** To prevent rotation along a only when the limit has
** been exceeded, we should instead apply the impulse
**
** lambda = -(JV + C/dt)/K = -(JV + b)/K.
**
** Note that it is generally unstable to add all of b back into the
** impulse, so we multiply it by a Baumgarte constant B in [0, 1]:
**
** b = BC/dt.
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
#warning "There still seems to be a bit too much 'bounce-back' considering we have no restitution."
#warning "This is especially noticeable when we use the ellipse normal for the swing axis with highly elliptic limits."
#warning "We also need to implement Euler angle constraints. Maybe use a separate joint for those if they work well?"
/**
How do we handle the axes for the biases? Should they just be the cardinal axes?
This would perhaps make sense, as we are using relative orientations.
https://github.com/erincatto/box2d/blob/master/src/dynamics/b2_revolute_joint.cpp
https://github.com/bulletphysics/bullet3/blob/master/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
https://github.com/kovertopz/jReactPhysics3D/blob/master/src/main/java/net/smert/jreactphysics3d/constraint/BallAndSocketJoint.java
https://github.com/dtecta/motion-toolkit
https://github.com/DigitalRune/DigitalRune/blob/master/Source/DigitalRune.Physics/Constraints/TwistSwingLimit.cs
Maybe investigate the generic 6-DOF constraints in Bullet.
The way they handle angle limits might be better than just swing-twist.
https://www.gamedev.net/forums/topic/589924-most-efficient-method-of-implementing-6dof-objects/
https://github.com/bulletphysics/bullet3/blob/master/src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp
**/


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

static void calculateLinearBias(
	const vec3 *const restrict centroidA, const vec3 *const restrict centroidB,
	const vec3 *const restrict rA, const vec3 *const restrict rB,
	vec3 *const restrict linearBias
);

#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static flags_t calculateAngularBias(
	const physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	vec3 *const restrict swingAxis, float *const restrict swingBias,
	vec3 *const restrict twistAxis, float *const restrict twistBias
);
#else
static flags_t calculateAngularBias(
	const physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	vec3 *const restrict angularAxisX, vec3 *const restrict angularAxisY, vec3 *const restrict angularAxisZ,
	vec3 *const restrict angularBias
);
#endif
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static flags_t calculateSwingLimit(
	const quat *const restrict swing,
	const quat *const restrict anchorRotGlobalA, const vec3 *const restrict twistAxis,
	const float angularLimitsY[2], const float angularLimitsZ[2],
	vec3 *const restrict swingAxis, float *const restrict swingBias
);
#endif
static flags_t calculateAngularLimit(
	const float angle, const float angularLimits[2],
	const flags_t lowerLimitFlag, const flags_t upperLimitFlag,
	vec3 *const restrict axis, float *const restrict angleBias
);

#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
static void swingImpulseRemoveTwist(const physicsJointSphere *const restrict joint, vec3 *const restrict swingImpulse);
#endif
static void calculateAngularVelocityImpulse(
	const vec3 *const restrict relVel, const vec3 *const restrict axis,
	const float bias, const float invMass,
	float *const restrict accumulatedImpulse, vec3 *const restrict impulse
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
	const quat *const restrict anchorRotA, const quat *const restrict anchorRotB,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
){

	joint->anchorA = *anchorA;
	joint->anchorB = *anchorB;
	joint->anchorRotA = *anchorRotA;
	joint->anchorRotB = *anchorRotB;

	joint->limitStates = PHYSJOINTSPHERE_LIMITS_FREE;

	#warning "If these limits are invalid (minimum is positive or maximum is negative), we should alter the 'anchorRots' to compensate."
	#warning "We should also clamp the y limit if we're using Euler constraints."
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
void physJointSphereWarmStart(
	const physicsJointSphere *const restrict joint, const flags_t limitStates,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 angularImpulse;
	vec3InitZero(&angularImpulse);

	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	// The angular impulse is the sum of the swing and twist impulses.
	// We recalculate them here because we need the new swing and twist axes.
	if(limitStates & PHYSJOINTSPHERE_LIMITS_SWING){
		vec3Fmaf(joint->swingImpulse, &joint->swingAxis, &angularImpulse);

		#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		swingImpulseRemoveTwist(joint, &angularImpulse);
		#endif
	}
	if(limitStates & PHYSJOINTSPHERE_LIMITS_TWIST){
		vec3Fmaf(joint->twistImpulse, &joint->twistAxis, &angularImpulse);
	}
	#else
	// The angular impulse is the sum of the three axis impulses.
	// We recalculate them here because we need the new constraint axes.
	if(limitStates & PHYSJOINTSPHERE_LIMITS_X){
		vec3Fmaf(joint->angularImpulse.x, &joint->angularAxisX, &angularImpulse);
	}
	if(limitStates & PHYSJOINTSPHERE_LIMITS_Y){
		vec3Fmaf(joint->angularImpulse.y, &joint->angularAxisY, &angularImpulse);
	}
	if(limitStates & PHYSJOINTSPHERE_LIMITS_Z){
		vec3Fmaf(joint->angularImpulse.z, &joint->angularAxisZ, &angularImpulse);
	}
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
	#ifdef PHYSJOINTSPHERE_WARM_START
	const flags_t oldLimitStates = ((physicsJointSphere *)joint)->limitStates;
	#endif

	updateConstraintData((physicsJointSphere *)joint, bodyA, bodyB);

	// We actually compute the constraint axes in "calculateAngularBias"
	// rather than in "updateConstraintData", as it makes things easier.
	//
	// Although this function accepts a pointer to a constant joint,
	// we modify it in this call through the last four parameters.
	// It's not the best practice, but at least it's efficient.
	#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
	((physicsJointSphere *)joint)->limitStates = calculateAngularBias(
		(physicsJointSphere *)joint,
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->swingAxis, &((physicsJointSphere *)joint)->swingBias,
		&((physicsJointSphere *)joint)->twistAxis, &((physicsJointSphere *)joint)->twistBias
	);

	// b = B/dt * C
	// The angular slop lets us solve the velocity constraint,
	// but removes the bias term when we're sufficiently close
	// to the limits. This helps prevent jittering when resting
	// at the limit points.
	((physicsJointSphere *)joint)->swingBias = floatMax(
		PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->swingBias - PHYSJOINT_ANGULAR_SLOP),
		0.f
	);
	((physicsJointSphere *)joint)->twistBias = floatMax(
		PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->twistBias - PHYSJOINT_ANGULAR_SLOP),
		0.f
	);

	calculateInverseAngularMass(
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->swingAxis,
		&((physicsJointSphere *)joint)->twistAxis,
		&((physicsJointSphere *)joint)->swingInvMass,
		&((physicsJointSphere *)joint)->twistInvMass
	);
	#else
	((physicsJointSphere *)joint)->limitStates = calculateAngularBias(
		(physicsJointSphere *)joint,
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->angularAxisX, &((physicsJointSphere *)joint)->angularAxisY, &((physicsJointSphere *)joint)->angularAxisZ,
		&((physicsJointSphere *)joint)->angularBias
	);

	// b = B/dt * C
	// The angular slop lets us solve the velocity constraint,
	// but removes the bias term when we're sufficiently close
	// to the limits. This helps prevent jittering when resting
	// at the limit points.
	((physicsJointSphere *)joint)->angularBias.x = floatMax(
		PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->angularBias.x - PHYSJOINT_ANGULAR_SLOP),
		0.f
	);
	((physicsJointSphere *)joint)->angularBias.y = floatMax(
		PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->angularBias.y - PHYSJOINT_ANGULAR_SLOP),
		0.f
	);
	((physicsJointSphere *)joint)->angularBias.z = floatMax(
		PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->angularBias.z - PHYSJOINT_ANGULAR_SLOP),
		0.f
	);

	calculateInverseAngularMass(
		bodyA, bodyB,
		&((physicsJointSphere *)joint)->angularAxisX, &((physicsJointSphere *)joint)->angularAxisY, &((physicsJointSphere *)joint)->angularAxisZ,
		&((physicsJointSphere *)joint)->angularInvMass
	);
	#endif

	// Compute the linear bias term.
	calculateLinearBias(
		&bodyA->centroid, &bodyB->centroid,
		&((physicsJointSphere *)joint)->rA, &((physicsJointSphere *)joint)->rB,
		&((physicsJointSphere *)joint)->linearBias
	);
	// b = B/dt * C
	vec3MultiplyS(&((physicsJointSphere *)joint)->linearBias, PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency);

	calculateLinearMass(
		((physicsJointSphere *)joint)->rA, ((physicsJointSphere *)joint)->rB,
		bodyA, bodyB, &((physicsJointSphere *)joint)->linearInvMass
	);
	// The performance of solving using Cramer's rule seems similar
	// to inverting. However, since we do our velocity solve step
	// multiple times using the same matrix for sequential impulse,
	// it's much faster to invert it here when we're presolving.
	mat3Invert(&((physicsJointSphere *)joint)->linearInvMass);

	#ifdef PHYSJOINTSPHERE_WARM_START
	// Warm start the constraint if the old limit states match the new ones.
	physJointSphereWarmStart(
		(physicsJointSphere *)joint, oldLimitStates & ((physicsJointSphere *)joint)->limitStates,
		bodyA, bodyB
	);
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
		// w_relative = wB - wA
		vec3SubtractVec3Out(&bodyB->angularVelocity, &bodyA->angularVelocity, &relativeVelocity);
		vec3InitZero(&impulse);


		#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
		// Solve the swing constraint.
		if(((physicsJointSphere *)joint)->limitStates & PHYSJOINTSPHERE_LIMITS_SWING){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->swingAxis,
				((physicsJointSphere *)joint)->swingBias, ((physicsJointSphere *)joint)->swingInvMass,
				&((physicsJointSphere *)joint)->swingImpulse, &impulse
			);

			#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
			swingImpulseRemoveTwist(joint, &impulse);
			#endif
		}else{
			((physicsJointSphere *)joint)->swingImpulse = 0.f;
		}
		// Solve the twist constraint.
		if(((physicsJointSphere *)joint)->limitStates & PHYSJOINTSPHERE_LIMITS_TWIST){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->twistAxis,
				((physicsJointSphere *)joint)->twistBias, ((physicsJointSphere *)joint)->twistInvMass,
				&((physicsJointSphere *)joint)->twistImpulse, &impulse
			);
		}else{
			((physicsJointSphere *)joint)->twistImpulse = 0.f;
		}
		#else
		// Solve the x-axis angular constraint.
		if(((physicsJointSphere *)joint)->limitStates & PHYSJOINTSPHERE_LIMITS_X){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->angularAxisX,
				((physicsJointSphere *)joint)->angularBias.x, ((physicsJointSphere *)joint)->angularInvMass.x,
				&((physicsJointSphere *)joint)->angularImpulse.x, &impulse
			);
		}else{
			((physicsJointSphere *)joint)->angularImpulse.x = 0.f;
		}
		// Solve the y-axis angular constraint.
		if(((physicsJointSphere *)joint)->limitStates & PHYSJOINTSPHERE_LIMITS_Y){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->angularAxisY,
				((physicsJointSphere *)joint)->angularBias.y, ((physicsJointSphere *)joint)->angularInvMass.y,
				&((physicsJointSphere *)joint)->angularImpulse.y, &impulse
			);
		}else{
			((physicsJointSphere *)joint)->angularImpulse.y = 0.f;
		}
		// Solve the z-axis angular constraint.
		if(((physicsJointSphere *)joint)->limitStates & PHYSJOINTSPHERE_LIMITS_Z){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->angularAxisZ,
				((physicsJointSphere *)joint)->angularBias.z, ((physicsJointSphere *)joint)->angularInvMass.z,
				&((physicsJointSphere *)joint)->angularImpulse.z, &impulse
			);
		}else{
			((physicsJointSphere *)joint)->angularImpulse.z = 0.f;
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

		// Calculate the total velocity of the
		// socket and store it in "relativeVelocity".
		vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointSphere *)joint)->rA, &relativeVelocity);
		vec3AddVec3(&relativeVelocity, &bodyA->linearVelocity);
		// Calculate the total velocity of
		// the ball and store it in "impulse".
		vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointSphere *)joint)->rB, &impulse);
		vec3AddVec3(&impulse, &bodyB->linearVelocity);
		// Calculate the relative velocity between the ball and socket.
		// Note that we actually compute "-v_relative" and store it in "impulse".
		vec3SubtractVec3P2(&relativeVelocity, &impulse);
		// Subtract the bias term from the negative relative velocity.
		vec3SubtractVec3P1(&impulse, &((physicsJointSphere *)joint)->linearBias);


		// Solve for the linear impulse:
		// J1*V + b1 = v_relative + b1,
		// K1*lambda = -(J1*V + b1).
		mat3MultiplyVec3(&((physicsJointSphere *)joint)->linearInvMass, &impulse);
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
		float swingDeviation, twistDeviation;
		vec3 twistImpulse;

		const flags_t limitStates = calculateAngularBias(
			(physicsJointSphere *)joint,
			bodyA, bodyB,
			&impulse, &swingDeviation,
			&twistImpulse, &twistDeviation
		);

		// Solve the constraints if they're being violated.
		if(limitStates != PHYSJOINTSPHERE_LIMITS_FREE){
			float swingInvMass;
			float twistInvMass;

			// Calculate the angular error.
			angularError = swingDeviation + fabsf(twistDeviation);

			// The Baumgarte bias stops us from adding the entire
			// correction in one step, which aids stability.
			swingDeviation = floatClamp(
				PHYSJOINTSPHERE_BAUMGARTE_BIAS * (swingDeviation - PHYSJOINT_ANGULAR_SLOP),
				0.f, PHYSJOINT_MAX_ANGULAR_CORRECTION
			);
			twistDeviation = floatClamp(
				PHYSJOINTSPHERE_BAUMGARTE_BIAS * (twistDeviation - PHYSJOINT_ANGULAR_SLOP),
				0.f, PHYSJOINT_MAX_ANGULAR_CORRECTION
			);

			calculateInverseAngularMass(bodyA, bodyB, &impulse, &twistImpulse, &swingInvMass, &twistInvMass);
			// lambda = -C/K
			vec3MultiplyS(&impulse, -swingDeviation*swingInvMass);
			vec3Fmaf(-twistDeviation*twistInvMass, &twistImpulse, &impulse);

			// Apply the correctional impulse.
			physRigidBodyApplyAngularImpulsePositionInverse(bodyA, impulse);
			physRigidBodyApplyAngularImpulsePosition(bodyB, impulse);
		}else{
			angularError = 0.f;
		}
	}
	#else
	{
		vec3 axisY;
		vec3 axisZ;
		vec3 angularDeviation;

		const flags_t limitStates = calculateAngularBias(
			(physicsJointSphere *)joint,
			bodyA, bodyB,
			&impulse, &axisY, &axisZ,
			&angularDeviation
		);

		// Solve the constraints if they're being violated.
		if(limitStates != PHYSJOINTSPHERE_LIMITS_FREE){
			vec3 invMass;

			// Calculate the angular error.
			angularError = fabsf(angularDeviation.x) + fabsf(angularDeviation.y) + fabsf(angularDeviation.z);

			// The Baumgarte bias stops us from adding the entire
			// correction in one step, which aids stability.
			angularDeviation.x = floatClamp(
				PHYSJOINTSPHERE_BAUMGARTE_BIAS * (angularDeviation.x - PHYSJOINT_ANGULAR_SLOP),
				0.f, PHYSJOINT_MAX_ANGULAR_CORRECTION
			);
			angularDeviation.y = floatClamp(
				PHYSJOINTSPHERE_BAUMGARTE_BIAS * (angularDeviation.y - PHYSJOINT_ANGULAR_SLOP),
				0.f, PHYSJOINT_MAX_ANGULAR_CORRECTION
			);
			angularDeviation.z = floatClamp(
				PHYSJOINTSPHERE_BAUMGARTE_BIAS * (angularDeviation.z - PHYSJOINT_ANGULAR_SLOP),
				0.f, PHYSJOINT_MAX_ANGULAR_CORRECTION
			);

			calculateInverseAngularMass(bodyA, bodyB, &impulse, &axisY, &axisZ, &invMass);
			// lambda = -C/K
			vec3MultiplyS(&impulse, -angularDeviation.x*invMass.x);
			vec3Fmaf(-angularDeviation.y*invMass.y, &axisY, &impulse);
			vec3Fmaf(-angularDeviation.z*invMass.z, &axisZ, &impulse);


			// Apply the correctional impulse.
			physRigidBodyApplyAngularImpulsePositionInverse(bodyA, impulse);
			physRigidBodyApplyAngularImpulsePosition(bodyB, impulse);
		}else{
			angularError = 0.f;
		}
	}
	#endif


	// Solve the linear point-to-point constraint.
	{
		vec3 rA;
		vec3 rB;
		vec3 constraint;
		mat3 linearMass;

		vec3SubtractVec3Out(&((physicsJointSphere *)joint)->anchorA, &bodyA->base->centroid, &rA);
		transformDirection(&bodyA->state, &rA);
		vec3SubtractVec3Out(&((physicsJointSphere *)joint)->anchorB, &bodyB->base->centroid, &rB);
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
		vec3MultiplyS(&constraint, PHYSJOINTSPHERE_BAUMGARTE_BIAS);

		// Solve for the impulse:
		// K1*lambda = -C1.
		calculateLinearMass(rA, rB, bodyA, bodyB, &linearMass);
		mat3Solve(&linearMass, &constraint, &impulse);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &impulse);
		physRigidBodyApplyImpulsePosition(bodyB, &rB, &impulse);
	}


	return(
		#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
		angularError <= 2.f*PHYSJOINT_ANGULAR_POSITIONAL_ERROR_THRESHOLD &&
		#else
		angularError <= 3.f*PHYSJOINT_ANGULAR_POSITIONAL_ERROR_THRESHOLD &&
		#endif
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
	physicsJointSphere *const restrict joint,
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
	mat3MultiplyVec3Out(&invInertiaSum, swingAxis, &IABa);
	*swingInvMass = 1.f / vec3DotVec3(swingAxis, &IABa);

	// Twist inverse effective mass (K3).
	mat3MultiplyVec3Out(&invInertiaSum, twistAxis, &IABa);
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
	mat3MultiplyVec3Out(&invInertiaSum, angularAxisX, &IABa);
	angularInvMass->x = 1.f / vec3DotVec3(angularAxisX, &IABa);

	// Inverse effective mass for the y-axis.
	mat3MultiplyVec3Out(&invInertiaSum, angularAxisY, &IABa);
	angularInvMass->y = 1.f / vec3DotVec3(angularAxisY, &IABa);

	// Inverse effective mass for the z-axis.
	mat3MultiplyVec3Out(&invInertiaSum, angularAxisZ, &IABa);
	angularInvMass->z = 1.f / vec3DotVec3(angularAxisZ, &IABa);
}
#endif


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


/*
** Calculate the joint's angular bias term. This is used
** for clamping the relative orientation of rigid body B.
*/
#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
static flags_t calculateAngularBias(
	const physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	vec3 *const restrict swingAxis, float *const restrict swingBias,
	vec3 *const restrict twistAxis, float *const restrict twistBias
){

	// We need to calculate the new swing
	// and twist axes and the angle limits.
	//
	// Ideally, we would calculate the axes
	// in "updateConstraintData", but doing
	// it here just makes things easier.

	vec3 xAxisA;
	quat swing, twist;
	flags_t limitStates;

	// Calculate the swing bias.
	{
		quat anchorRotGlobalA, anchorRotGlobalB;
		quat qAB;

		// Get the global constraint orientations for the rigid bodies.
		quatMultiplyQuatOut(bodyA->state.rot, joint->anchorRotA, &anchorRotGlobalA);
		quatMultiplyQuatOut(bodyB->state.rot, joint->anchorRotB, &anchorRotGlobalB);
		// Get the relative orientation from body A to body B:
		// qAB = B*conj(A).
		quatMultiplyQuatConjOut(anchorRotGlobalB, anchorRotGlobalA, &qAB);
		// These represent the x-axis in the two rigid body's reference frames.
		// The swing is the quaternion giving the shortest arc between these vectors.
		quatBasisX(&anchorRotGlobalA, &xAxisA);
		quatBasisX(&anchorRotGlobalB, twistAxis);
		// Decompose the relative orientation into swing and twist components.
		// We take the twist component to be the rotation around the x-axis.
		quatSwingTwistFaster(&qAB, &xAxisA, &twist, &swing);

		// Compute the constraint term for the swing limit,
		// C2 = (RB - RA) . s - s_lim.
		// The maximum swing limit is also computed
		// here using the limits on the y- and z-axes.
		limitStates = calculateSwingLimit(
			&swing,
			&anchorRotGlobalA, twistAxis,
			joint->angularLimitsY, joint->angularLimitsZ,
			swingAxis, swingBias
		);
	}

	// Calculate the twist bias.
	{
		float twistAngle = quatAngle(&twist);
		// Our quaternion angle function returns a value in [0, 2pi].
		// However, we want our angles in the interval [-pi, pi].
		if(twistAngle > M_PI){
			twistAngle -= 2.f*M_PI;
		}
		// The twist quaternion's axis should be parallel
		// to rigid body A's x-axis. If it's antiparallel,
		// we should negate the twist angle to compensate.
		if(vec3DotVec3(&xAxisA, (vec3 *)&twist.x) < 0.f){
			twistAngle = -twistAngle;
		}
		// Compute the constraint term for the twist limit,
		// C2 = (RB - RA) . t - t_lim.
		limitStates |= calculateAngularLimit(
			twistAngle, joint->angularLimitsX,
			PHYSJOINTSPHERE_LIMITS_TWIST_LOWER, PHYSJOINTSPHERE_LIMITS_TWIST_UPPER,
			twistAxis, twistBias
		);
	}

	return(limitStates);
}
#else
static flags_t calculateAngularBias(
	const physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	vec3 *const restrict angularAxisX, vec3 *const restrict angularAxisY, vec3 *const restrict angularAxisZ,
	vec3 *const restrict angularBias
){

	// We need to calculate the new constraint axes and the angle limits.
	//
	// Ideally, we would calculate the axes in "updateConstraintData",
	// but doing it here just makes things easier.
	#error "This doesn't work at all. Another approach is required."
	/**
	 ** Let's try a new approach:
	 **
	 **     1. Calculate the Euler angles for the current orientation, q_i.
	 **     2. Clamp them to the desired intervals.
	 **     3. Construct a quaternion, q_f from the clamped angles.
	 **     4. The quaternion q_c = q_f*conj(q_i) gives the offset from the desired orientation.
	 **     5. Use its axis and angle to create a constraint in the usual way.
	 **/

	quat anchorRotGlobalA, anchorRotGlobalB;
	quat qAB;
	vec3 angles;
	flags_t limitStates;

	// Get the global constraint orientations for the rigid bodies.
	quatMultiplyQuatOut(bodyA->state.rot, joint->anchorRotA, &anchorRotGlobalA);
	quatMultiplyQuatOut(bodyB->state.rot, joint->anchorRotB, &anchorRotGlobalB);
	// Get the relative orientation from body A to body B:
	// qAB = B*conj(A).
	quatMultiplyQuatConjOut(anchorRotGlobalB, anchorRotGlobalA, &qAB);
	// Compute the constraint axes. These are simply the
	// x, y and z axes in rigid body A's coordinate frame.
	quatBasisX(&anchorRotGlobalA, angularAxisX);
	quatBasisY(&anchorRotGlobalA, angularAxisY);
	quatBasisZ(&anchorRotGlobalA, angularAxisZ);
	// Decompose the relative orientation into
	// angles about rigid body A's x, y and z axes.
	quatToEulerAnglesXYZ(qAB, &angles);

	limitStates = calculateAngularLimit(
		angles.x, joint->angularLimitsX,
		PHYSJOINTSPHERE_LIMITS_X_LOWER, PHYSJOINTSPHERE_LIMITS_X_UPPER,
		angularAxisX, &angularBias->x
	);
	limitStates |= calculateAngularLimit(
		angles.y, joint->angularLimitsY,
		PHYSJOINTSPHERE_LIMITS_Y_LOWER, PHYSJOINTSPHERE_LIMITS_Y_UPPER,
		angularAxisY, &angularBias->y
	);
	limitStates |= calculateAngularLimit(
		angles.z, joint->angularLimitsZ,
		PHYSJOINTSPHERE_LIMITS_Z_LOWER, PHYSJOINTSPHERE_LIMITS_Z_UPPER,
		angularAxisZ, &angularBias->z
	);

	return(limitStates);
}
#endif

#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
/*
** Calculate the swing axis and bias, and return the state of the swing limit!
** The swing bias is positive when the constraint is violated, and zero otherwise.
*/
static flags_t calculateSwingLimit(
	const quat *const restrict swing,
	const quat *const restrict anchorRotGlobalA, const vec3 *const restrict twistAxis,
	const float angularLimitsY[2], const float angularLimitsZ[2],
	vec3 *const restrict swingAxis, float *const restrict swingBias
){

	// Regardless of whether or not we use the ellipse normal
	// as the swing axis, we will always get an axis of (0, 0, 0)
	// when there is no relative rotation between the two bodies.
	// We don't actually care much about this case though, as our
	// constraint should always be satisfied.
	if(quatIsIdentity(swing->w)){
		quatBasisY(swing, swingAxis);
	}else{
		vec3 constraintAxis;
		#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		vec2 ellipseNormal;
		#endif
		float swingDeviation;

		// The constraint axis is rigid body B's x-axis
		// in the coordinate frame of rigid body A.
		// This is the axis we want to constrain within our cone.
		quatConjRotateVec3FastOut(anchorRotGlobalA, twistAxis, &constraintAxis);
		// As rigid body B rotates around the y-axis of rigid
		// body A, the z-component of the constraint axis decreases.
		// Conversely, the y-component increases as we rotate about z.
		//
		// Note that we use different limits depending
		// on which quadrant of the ellispse we're in.
		#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		swingDeviation = quatAngle(swing) - clampEllipseDistanceNormalFast(
			constraintAxis.y, constraintAxis.z,
			(constraintAxis.y > 0.f) ?  angularLimitsZ[1] : -angularLimitsZ[0],
			(constraintAxis.z > 0.f) ? -angularLimitsY[0] :  angularLimitsY[1],
			&ellipseNormal
		);
		#else
		swingDeviation = quatAngle(swing) - clampEllipseDistanceFast(
			constraintAxis.y, constraintAxis.z,
			(constraintAxis.y > 0.f) ?  angularLimitsZ[1] : -angularLimitsZ[0],
			(constraintAxis.z > 0.f) ? -angularLimitsY[0] :  angularLimitsY[1]
		);
		#endif

		#ifndef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		quatAxis(swing, swingAxis);
		#else
		// Using the ellipse normal should be more stable, although
		// we are using the angle for the swing axis, not this one.

		// The ellipse normal we compute is unit length, so since the
		// local swing axis is either of the form (1, 0, 0) or (0, -y, z),
		// we don't need to normalize the new swing axis.
		swingAxis->x = 0.f;
		swingAxis->y = -ellipseNormal.y;
		swingAxis->z = ellipseNormal.x;
		// Bring the swing axis from body B's space to global space.
		quatRotateVec3Fast(anchorRotGlobalA, swingAxis);
		#endif

		// If the constraint is broken, this term is positive.
		if(swingDeviation > 0.f){
			*swingBias = swingDeviation;
			return(PHYSJOINTSPHERE_LIMITS_SWING);
		}
	}

	// If the constraint is satisfied, set the bias to 0.
	*swingBias = 0.f;
	return(PHYSJOINTSPHERE_LIMITS_FREE);
}
#endif

/*
** Calculate the angular constraint and return which of the two specified
** limit states it's in! We want a positive bias when the constraint is
** broken, which means we may need to negate the constraint axis.
*/
static flags_t calculateAngularLimit(
	const float angle, const float angularLimits[2],
	const flags_t lowerLimitFlag, const flags_t upperLimitFlag,
	vec3 *const restrict axis, float *const restrict angleBias
){

	// If the lower limit is broken, use it for the bias.
	// Note that the bias is negative when the lower limit is broken,
	// so to get a positive bias, we need to negate the constraint axis.
	if(angle < angularLimits[0]){
		*angleBias = angularLimits[0] - angle;
		vec3Negate(axis);
		return(lowerLimitFlag);

	// If the upper limit is broken, use it instead.
	// The bias is positive when the upper limit is broken.
	}else if(angle > angularLimits[1]){
		*angleBias = angle - angularLimits[1];
		return(upperLimitFlag);
	}

	*angleBias = 0.f;
	return(PHYSJOINTSPHERE_LIMITS_FREE);
}


#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
/*
** Given a swing impulse, remove the component parallel to the twist axis.
** The ellipse normal may not be orthogonal to the twist axis, so we need
** to filter it out. Otherwise, our swing impulses will introduce twisting.
*/
static void swingImpulseRemoveTwist(const physicsJointSphere *const restrict joint, vec3 *const restrict swingImpulse){
	vec3 impulseTwistComponent;
	// Project the impulse onto the twist axis.
	vec3MultiplySOut( &joint->twistAxis, vec3DotVec3(swingImpulse, &joint->twistAxis), &impulseTwistComponent);
	// Subtract the twist component from the impulse.
	vec3SubtractVec3P1(swingImpulse, &impulseTwistComponent);
}
#endif

/*
** Compute the impulse required to solve the angular velocity
** constraint for the specified axis. We do this a few times,
** so it's probably a good idea to have a helper function.
*/
static void calculateAngularVelocityImpulse(
	const vec3 *const restrict relVel, const vec3 *const restrict axis,
	const float bias, const float invMass,
	float *const restrict accumulatedImpulse, vec3 *const restrict impulse
){

	// Calculate the Lagrange multiplier.
	// lambda = -(JV + b)/(JM^(-1)J^T)
	float lambda = -(vec3DotVec3(relVel, axis) + bias) * invMass;

	// Clamp the accumulated impulse magnitude.
	const float oldImpulse = *accumulatedImpulse;
	*accumulatedImpulse = floatMin(oldImpulse + lambda, 0.f);
	lambda = *accumulatedImpulse - oldImpulse;

	// Calculate the total impulse.
	vec3Fmaf(lambda, axis, impulse);
}