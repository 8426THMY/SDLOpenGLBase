#include "physicsJointSphere.h"


#include "physicsRigidBody.h"

#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
#include "vec2.h"
#endif
#include "utilMath.h"


#define PHYSJOINTSPHERE_LIMITS_FREE        0x00
#define PHYSJOINTSPHERE_LIMITS_SWING       0x01
#define PHYSJOINTSPHERE_LIMITS_TWIST_LOWER 0x02
#define PHYSJOINTSPHERE_LIMITS_TWIST_UPPER 0x04
#define PHYSJOINTSPHERE_LIMITS_TWIST       (PHYSJOINTSPHERE_LIMITS_TWIST_LOWER | PHYSJOINTSPHERE_LIMITS_TWIST_UPPER)


#ifdef PHYSJOINTSPHERE_DEBUG
vec3 swingAxisDebug = {0.f, 0.f, 0.f};
float swingAngleDebug = 0.f;
vec3 twistAxisDebug = {0.f, 0.f, 0.f};
float twistAngleDebug = 0.f;
#endif


#warning "Try and add spring constraints here."
/** https://github.com/jrouwe/JoltPhysics/blob/master/Jolt/Physics/Constraints/SwingTwistConstraint.cpp **/
#warning "In PhysX, they use the stiffness parameter to control restitution."
/** https://github.com/ezEngine/docs-src/blob/main/pages/docs/physics/joints/physx-spherical-joint-component.md/ **/
#warning "Try to use the soft constraints from the distance joint, but maybe make it use a single softness parameter in [0, 1]."
#warning "They do something similar to what we want here."
/** https://github.com/DigitalRune/DigitalRune/blob/master/Source/DigitalRune.Physics/Constraints/TwistSwingLimit.cs **/


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
** C1' : 0 = (vB +   wB X rB) - (vA +   wA X rA)
**         = (vB -   rB X wB) - (vA -   rA X wA)
**         = (vB - [rB]_X*wB) - (vA - [rA]_X*wA),
**
** where "[.]_X" denotes the skew-symmetric "cross product" matrix.
**
**
** We also need to impose constraints on the relative orientation of the
** rigid bodies, so if "theta(a)" represents the angle of the relative
** orientation "((qB*RB) * conj(qA*RA))" along the axis a, then we require
**
** C2 : theta(s) - s_limit <= 0,
** C3 : theta(t) - t_upper <= 0,
** C4 : theta(t) - t_lower >= 0.
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
** J2 = [   0,     -s,   0,       s],
** J3 = [   0,     -t,   0,       t] = J4.
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
** Note that transposing the Jacobian transposes [rA]_X.
** As it's skew-symmetric, we need to flip its sign.
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
**
** To limit the relative orientation of rigid body B with
** respect to rigid body A, we use swing-twist decomposition
** and constrain the swing and twist axes individually.
**
** Note that if a_limit is the axis limit and "theta(a)" is the angle of
** the relative orientation "((qB*RB) * conj(qA*RA))" along the axis a, then
**
** b = (theta(a) - a_limit)/dt = C/dt
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
** b = B/dt * C.
**
** ----------------------------------------------------------------------
*/
#warning "We still need to implement some sort of softness - check how Open Dynamics Engine does it."
#warning "We also need to implement Euler angle constraints. Maybe use a separate joint for those if they work well?"
#warning "It would be nice if we could fix warm starting and the Gauss-Seidel stabilizer, too."
/**
*** For the last point, check how ReactPhysics3D works maybe.
*** Their ball and socket joints are a bit different to ours, but close enough to be usable.
***     https://github.com/DanielChappuis/reactphysics3d/blob/master/src/systems/SolveBallAndSocketJointSystem.cpp
*** They seem to allow warm starting and Gauss-Seidel, but both are implemented almost identically to ours.
**/
#warning "One of the issues for Gauss-Seidel seems to be that angular corrections are undone when correcting the position."
#warning "We could possibly get around this by using a single Jacobian that encompasses everything."
#warning "How efficient would this be, though?"
#warning "If only there were some way of writing the point-to-point constraint such that it doesn't require the angular velocity."
/**
*** In some situations, the twist constraint seems to jitter. I've only observed this once and
*** I haven't really investigated it much, but it's almost certainly due to the above issues.
**/
#warning "The reason the box bounces back after hitting an angle limit is due to warmstarting and Baumgarte stabilization."
#warning "Warmstarting works great with high temporal coherence, but after hitting an angle limit, we get an abrupt change in velocity."
#warning "ReactPhysics3D seems to use both warmstarting and Baumgarte stabilization for their ball and socket constraints, but it seems that Bullet uses neither."
#warning "The false restitution is pretty ugly, and it makes adding actual restitution painful, so maybe we should go the Bullet route."


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

static void calculateLinearBias(
	const vec3 *const restrict centroidA, const vec3 *const restrict centroidB,
	const vec3 *const restrict rA, const vec3 *const restrict rB,
	vec3 *const restrict linearBias
);

static flags_t calculateAngularBias(
	const physicsJointSphere *const restrict joint,
	const quat *const restrict rotA, const quat *const restrict rotB,
	vec3 *const restrict swingAxis, float *const restrict swingBias,
	vec3 *const restrict twistAxis, float *const restrict twistBias
);
static flags_t calculateSwingLimit(
	const quat *const restrict swing,
	const quat *const restrict rotOffsetGlobalA, const vec3 *const restrict twistAxis,
	const float angularLimitsY[2], const float angularLimitsZ[2],
	vec3 *const restrict swingAxis, float *const restrict swingBias
);
static flags_t calculateTwistLimit(
	const float angle, const float angularLimits[2],
	vec3 *const restrict axis, float *const restrict angleBias
);

#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
static void swingImpulseRemoveTwist(const vec3 *const restrict twistAxis, vec3 *const restrict swingImpulse);
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
	const quat *const restrict rotOffsetA, const quat *const restrict rotOffsetB,
	const float restitution,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
){

	joint->anchorA = *anchorA;
	joint->anchorB = *anchorB;
	joint->rotOffsetA = *rotOffsetA;
	joint->rotOffsetB = *rotOffsetB;

	joint->limitStates = PHYSJOINTSPHERE_LIMITS_FREE;

	joint->restitution = restitution;

	#warning "If these limits are invalid (minimum is positive or maximum is negative), we should alter the 'rotOffsets' to compensate."
	#warning "We should also clamp the y limit if we're using Euler constraints."
	joint->angularLimitsX[0] = minX;
	joint->angularLimitsX[1] = maxX;
	joint->angularLimitsY[0] = minY;
	joint->angularLimitsY[1] = maxY;
	joint->angularLimitsZ[0] = minZ;
	joint->angularLimitsZ[1] = maxZ;

	vec3InitZero(&joint->linearImpulse);
	joint->swingImpulse = 0.f;
	joint->twistImpulse = 0.f;
}


#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(
	const physicsJointSphere *const restrict joint,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 angularImpulse;
	vec3InitZero(&angularImpulse);

	// The angular impulse is the sum of the swing and twist impulses.
	// We recalculate them here because we need the new swing and twist axes.
	if(flagsAreSet(((physicsJointSphere *)joint)->limitStates, PHYSJOINTSPHERE_LIMITS_SWING)){
		vec3Fmaf(joint->swingImpulse, &joint->swingAxis, &angularImpulse);
		#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		swingImpulseRemoveTwist(&joint->twistAxis, &angularImpulse);
		#endif
	}
	if(flagsAreSet(((physicsJointSphere *)joint)->limitStates, PHYSJOINTSPHERE_LIMITS_TWIST)){
		vec3Fmaf(joint->twistImpulse, &joint->twistAxis, &angularImpulse);
	}

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
	flags_t changedLimits = ((physicsJointSphere *)joint)->limitStates;


	updateConstraintData((physicsJointSphere *)joint, bodyA, bodyB);

	// Angular constraints.
	{
		// We actually compute the constraint axes in "calculateAngularBias"
		// rather than in "updateConstraintData", as it makes things easier.
		//
		// Although this function accepts a pointer to a constant joint,
		// we modify it in this call through the last four parameters.
		// It's not the best practice, but at least it's efficient.
		((physicsJointSphere *)joint)->limitStates = calculateAngularBias(
			(physicsJointSphere *)joint,
			&bodyA->state.rot, &bodyB->state.rot,
			&((physicsJointSphere *)joint)->swingAxis, &((physicsJointSphere *)joint)->swingBias,
			&((physicsJointSphere *)joint)->twistAxis, &((physicsJointSphere *)joint)->twistBias
		);
		// Figure out which limits have been either enabled or disabled.
		changedLimits ^= ((physicsJointSphere *)joint)->limitStates;

		calculateInverseAngularMass(
			bodyA, bodyB,
			&((physicsJointSphere *)joint)->swingAxis,
			&((physicsJointSphere *)joint)->twistAxis,
			&((physicsJointSphere *)joint)->swingInvMass,
			&((physicsJointSphere *)joint)->twistInvMass
		);

		// b = B/dt * C
		// The angular slop lets us solve the velocity constraint,
		// but removes the bias term when we're sufficiently close
		// to the limits. This helps prevent jittering when resting
		// at the limit points.
		#ifdef PHYSJOINTSPHERE_STABILISER_BAUMGARTE
		((physicsJointSphere *)joint)->swingBias = floatMax(
			PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->swingBias - PHYSJOINT_ANGULAR_SLOP),
			0.f
		);
		((physicsJointSphere *)joint)->twistBias = floatMax(
			PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency * (((physicsJointSphere *)joint)->twistBias - PHYSJOINT_ANGULAR_SLOP),
			0.f
		);
		#else
		((physicsJointSphere *)joint)->swingBias = 0.f;
		((physicsJointSphere *)joint)->twistBias = 0.f;
		#endif
		#warning "This seems to add too much energy into the system. In particular, the twist blows up in certain positions."
		#warning "Admittedly, this is a naive way of doing it: we should add it to the linear term."
		#warning "Oh, and restitution for twist makes no sense whatsoever. Let's remove that."
		#warning "Alternatively, what about using soft, spring constraints like Erin Catto suggests? I think they do this in Jolt and PhysX."
		// Add the restitution bias.
		/*{
			vec3 relativeVelocity;
			vec3SubtractVec3Out(&bodyB->angularVelocity, &bodyA->angularVelocity, &relativeVelocity);

			// Constrain the swing restitution bias. We need to make sure
			// that the relative velocity is parallel to the swing axis.
			// This means that when the constraint is broken, we're moving
			// away from the angular limit point.
			((physicsJointSphere *)joint)->swingBias += ((physicsJointSphere *)joint)->restitution * floatClamp(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->swingAxis),
				0.f, INFINITY//PHYSJOINTSPHERE_RESTITUTION_MIN_SWING, PHYSJOINTSPHERE_RESTITUTION_MAX_SWING
			);
			// Constrain the twist restitution bias. The angular velocity around
			// the twist axis can get quite large when the swing angle is near
			// pi radians, and this can cause the joint to blow up.
			((physicsJointSphere *)joint)->twistBias += ((physicsJointSphere *)joint)->restitution * floatClamp(
				vec3DotVec3(&relativeVelocity, &((physicsJointSphere *)joint)->twistAxis),
				0.f, INFINITY//PHYSJOINTSPHERE_RESTITUTION_MIN_TWIST, PHYSJOINTSPHERE_RESTITUTION_MAX_TWIST
			);
		}*/
	}

	// Linear constraint.
	{
		calculateLinearMass(
			((physicsJointSphere *)joint)->rA, ((physicsJointSphere *)joint)->rB,
			bodyA, bodyB, &((physicsJointSphere *)joint)->linearInvMass
		);
		// The performance of solving using Cramer's rule seems similar
		// to inverting. However, since we do our velocity solve step
		// multiple times using the same matrix for sequential impulse,
		// it's much faster to invert it here when we're presolving.
		mat3Invert(&((physicsJointSphere *)joint)->linearInvMass);

		#ifdef PHYSJOINTSPHERE_STABILISER_BAUMGARTE
		// Compute the linear bias term.
		calculateLinearBias(
			&bodyA->centroid, &bodyB->centroid,
			&((physicsJointSphere *)joint)->rA, &((physicsJointSphere *)joint)->rB,
			&((physicsJointSphere *)joint)->linearBias
		);
		// b = B/dt * C
		vec3MultiplyS(&((physicsJointSphere *)joint)->linearBias, PHYSJOINTSPHERE_BAUMGARTE_BIAS * frequency);
		#else
		vec3InitZero(&((physicsJointSphere *)joint)->linearBias);
		#endif
	}


	// Reset the accumulated impulses if the limits have changed state.
	if(flagsAreSet(changedLimits, PHYSJOINTSPHERE_LIMITS_SWING)){
		((physicsJointSphere *)joint)->swingImpulse = 0.f;
	}
	if(flagsAreSet(changedLimits, PHYSJOINTSPHERE_LIMITS_TWIST)){
		((physicsJointSphere *)joint)->twistImpulse = 0.f;
	}
	// Disable linear warmstarting while the swing constraint is violated.
	// This is a little hacky, but seems to give much more accurate results.
	if(flagsAreSet(((physicsJointSphere *)joint)->limitStates, PHYSJOINTSPHERE_LIMITS_SWING)){
		vec3InitZero(&((physicsJointSphere *)joint)->linearImpulse);
	}
	#ifdef PHYSJOINTSPHERE_WARM_START
	// We reset the accumulated impulses in here if necessary.
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
		// C' = JV = wB - wA
		vec3SubtractVec3Out(&bodyB->angularVelocity, &bodyA->angularVelocity, &relativeVelocity);
		vec3InitZero(&impulse);


		// Solve the swing constraint.
		if(flagsAreSet(((physicsJointSphere *)joint)->limitStates, PHYSJOINTSPHERE_LIMITS_SWING)){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->swingAxis,
				((physicsJointSphere *)joint)->swingBias, ((physicsJointSphere *)joint)->swingInvMass,
				&((physicsJointSphere *)joint)->swingImpulse, &impulse
			);

			#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
			swingImpulseRemoveTwist(&((physicsJointSphere *)joint)->twistAxis, &impulse);
			#endif
		}
		// Solve the twist constraint.
		if(flagsAreSet(((physicsJointSphere *)joint)->limitStates, PHYSJOINTSPHERE_LIMITS_TWIST)){
			calculateAngularVelocityImpulse(
				&relativeVelocity, &((physicsJointSphere *)joint)->twistAxis,
				((physicsJointSphere *)joint)->twistBias, ((physicsJointSphere *)joint)->twistInvMass,
				&((physicsJointSphere *)joint)->twistImpulse, &impulse
			);
		}else{
			((physicsJointSphere *)joint)->twistImpulse = 0.f;
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
	float swingError;
	float twistError;
	float linearError;
	vec3 impulse;


	// Solve the angular constraints.
	{
		float swingDeviation, twistDeviation;
		vec3 twistImpulse;

		const flags_t limitStates = calculateAngularBias(
			(physicsJointSphere *)joint,
			&bodyA->state.rot, &bodyB->state.rot,
			&impulse, &swingDeviation,
			&twistImpulse, &twistDeviation
		);

		// Solve the constraints if they're being violated.
		if(limitStates != PHYSJOINTSPHERE_LIMITS_FREE){
			float swingInvMass;
			float twistInvMass;

			// Calculate the angular errors.
			swingError = swingDeviation;
			twistError = fabsf(twistDeviation);

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
			#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
			swingImpulseRemoveTwist(&twistImpulse, &impulse);
			#endif
			vec3Fmaf(-twistDeviation*twistInvMass, &twistImpulse, &impulse);

			// Apply the correctional impulse.
			physRigidBodyApplyAngularImpulsePositionInverse(bodyA, impulse);
			physRigidBodyApplyAngularImpulsePosition(bodyB, impulse);
		}else{
			swingError = 0.f;
			twistError = 0.f;
		}
	}


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
		swingError <= PHYSJOINT_ANGULAR_POSITIONAL_ERROR_THRESHOLD &&
		twistError <= PHYSJOINT_ANGULAR_POSITIONAL_ERROR_THRESHOLD &&
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

	// Compute the linear effective mass given by
	//     K1 = (mA^{-1} + mB^{-1})*I_3 - [rA]_X*IA^{-1}*[rA]_X - [rB]_X*IB^{-1}*[rB]_X.
	// We don't invert the matrix here, as it's faster to solve
	// for lambda using Cramer's rule in the position solver step.

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
		// Recall that the "cross product" matrix for rA is given by
		//              [    0, -rA_z,  rA_y]
		//     [rA]_X = [ rA_z,     0, -rA_x].
		//              [-rA_y,  rA_x,     0]
		// We begin by calculating A = -IA^{-1}*[rA]_X. We don't need
		// to compute the last element here, as it's only necessary
		// to compute the lower triangular components, and the final
		// matrix -[rA]_X*IA^{-1}*[rA]_X is guaranteed to be symmetric.
		const float a3xy = rA.z*I[1];
		const float A[8] = {
			     rA.y*I[2] - a3xy, rA.y*I[4] - rA.z*I[3], rA.y*I[5] - rA.z*I[4],
			rA.z*I[0] - rA.x*I[2],      a3xy - rA.x*I[4], rA.z*I[2] - rA.x*I[5],
			rA.x*I[1] - rA.y*I[0], rA.x*I[3] - rA.y*I[1]
		};

		// Compute -[rA]_X*IA^{-1}*[rA]_X. We can just set the lower
		// triangular components when we add body B's contribution.
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
		// Calculate A = -IB^{-1}*[rB]_X.
		const float a3xy = rB.z*I[1];
		const float A[8] = {
			     rB.y*I[2] - a3xy, rB.y*I[4] - rB.z*I[3], rB.y*I[5] - rB.z*I[4],
			rB.z*I[0] - rB.x*I[2],      a3xy - rB.x*I[4], rB.z*I[2] - rB.x*I[5],
			rB.x*I[1] - rB.y*I[0], rB.x*I[3] - rB.y*I[1]
		};
		const float invMass = bodyA->invMass + bodyB->invMass;

		// Add body B's contribution and the diagonal term to the effective mass:
		//    K1 += (mA^{-1} + mB^{-1})*I_3 - [rB]_X*IB^{-1}*[rB]_X.
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
	mat3MultiplyVec3Out(&invInertiaSum, swingAxis, &IABa);
	*swingInvMass = 1.f / vec3DotVec3(swingAxis, &IABa);

	// Twist inverse effective mass (K3).
	mat3MultiplyVec3Out(&invInertiaSum, twistAxis, &IABa);
	*twistInvMass = 1.f / vec3DotVec3(twistAxis, &IABa);
}


// Calculate the joint's linear bias term.
static void calculateLinearBias(
	const vec3 *const restrict centroidA, const vec3 *const restrict centroidB,
	const vec3 *const restrict rA, const vec3 *const restrict rB,
	vec3 *const restrict linearBias
){

	// Calculate the displacement from the ball to the socket:
	// C1 = (pB + rB) - (pA + rA).
	vec3AddVec3Out(centroidB, rB, linearBias);
	vec3SubtractVec3P1(linearBias, centroidA);
	vec3SubtractVec3P1(linearBias, rA);
}


/*
** Calculate the joint's angular bias term. This is used
** for clamping the relative orientation of rigid body B.
*/
static flags_t calculateAngularBias(
	const physicsJointSphere *const restrict joint,
	const quat *const restrict rotA, const quat *const restrict rotB,
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
		quat rotOffsetGlobalA, rotOffsetGlobalB;
		quat qR;

		// Get the global constraint orientations for the rigid bodies.
		quatMultiplyQuatOut(*rotA, joint->rotOffsetA, &rotOffsetGlobalA);
		quatMultiplyQuatOut(*rotB, joint->rotOffsetB, &rotOffsetGlobalB);
		// Get the relative orientation from body A to body B:
		// qR = (qB*RB)*conj(qA*RA).
		quatMultiplyQuatConjOut(rotOffsetGlobalB, rotOffsetGlobalA, &qR);
		// These represent the x-axis in the two rigid body's reference frames.
		// The swing is the quaternion giving the shortest arc between these vectors.
		quatBasisX(&rotOffsetGlobalA, &xAxisA);
		quatBasisX(&rotOffsetGlobalB, twistAxis);
		// Decompose the relative orientation into swing and twist components.
		// We take the twist component to be the rotation around the x-axis.
		quatSwingTwistFaster(&qR, &xAxisA, &twist, &swing);
		#ifdef PHYSJOINTSPHERE_DEBUG
		printf("t0: %f, %f, %f, %f\n", twist.x, twist.y, twist.z, twist.w);
		#endif

		// Compute the constraint term for the swing limit,
		// C2 = (RB - RA) . s - s_lim.
		// The maximum swing limit is also computed
		// here using the limits on the y- and z-axes.
		limitStates = calculateSwingLimit(
			&swing,
			&rotOffsetGlobalA, twistAxis,
			joint->angularLimitsY, joint->angularLimitsZ,
			swingAxis, swingBias
		);
		#ifdef PHYSJOINTSPHERE_DEBUG
		quat swingTest = quatInitAxisAngleC(*swingAxis, *swingBias);
		printf("s: %f, %f, %f, %f\n", swingTest.x, swingTest.y, swingTest.z, swingTest.w);
		#endif
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
		flagsSet(limitStates, calculateTwistLimit(
			twistAngle, joint->angularLimitsX, twistAxis, twistBias
		));
		#ifdef PHYSJOINTSPHERE_DEBUG
		twistAxisDebug = quatBasisXC(twist);
		quat twistTest = quatInitAxisAngleC(*twistAxis, *twistBias);
		printf("t: %f, %f, %f, %f\n", twistTest.x, twistTest.y, twistTest.z, twistTest.w);
		#endif
	}

	return(limitStates);
}

#ifndef PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
/*
** Calculate the swing axis and bias, and return the state of the swing limit!
** The swing bias is positive when the constraint is violated, and zero otherwise.
*/
static flags_t calculateSwingLimit(
	const quat *const restrict swing,
	const quat *const restrict rotOffsetGlobalA, const vec3 *const restrict twistAxis,
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
		quatConjRotateVec3FastOut(rotOffsetGlobalA, twistAxis, &constraintAxis);
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
		#ifdef PHYSJOINTSPHERE_DEBUG
		swingAngleDebug = clampEllipseDistanceNormalFast(
			constraintAxis.y, constraintAxis.z,
			(constraintAxis.y > 0.f) ?  angularLimitsZ[1] : -angularLimitsZ[0],
			(constraintAxis.z > 0.f) ? -angularLimitsY[0] :  angularLimitsY[1],
			&ellipseNormal
		);
		#endif
		#else
		swingDeviation = quatAngle(swing) - clampEllipseDistanceFast(
			constraintAxis.y, constraintAxis.z,
			(constraintAxis.y > 0.f) ?  angularLimitsZ[1] : -angularLimitsZ[0],
			(constraintAxis.z > 0.f) ? -angularLimitsY[0] :  angularLimitsY[1]
		);
		#ifdef PHYSJOINTSPHERE_DEBUG
		swingAngleDebug = clampEllipseDistanceFast(
			constraintAxis.y, constraintAxis.z,
			(constraintAxis.y > 0.f) ?  angularLimitsZ[1] : -angularLimitsZ[0],
			(constraintAxis.z > 0.f) ? -angularLimitsY[0] :  angularLimitsY[1]
		);
		#endif
		#endif

		#ifdef PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL
		// Using the ellipse normal should be more stable, although
		// we are using the angle for the swing axis, not this one.

		// The ellipse normal we compute is unit length, so since the
		// local swing axis is either of the form (1, 0, 0) or (0, -y, z),
		// we don't need to normalize the new swing axis.
		swingAxis->x = 0.f;
		swingAxis->y = -ellipseNormal.y;
		swingAxis->z = ellipseNormal.x;
		// Bring the swing axis from body B's space to global space.
		quatRotateVec3Fast(rotOffsetGlobalA, swingAxis);
		#else
		quatAxis(swing, swingAxis);
		#endif
		#ifdef PHYSJOINTSPHERE_DEBUG
		swingAxisDebug = *swingAxis;
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
** Calculate the twist constraint and return which of the two specified
** limit states it's in! We want a positive bias when the constraint is
** broken, which means we may need to negate the constraint axis.
*/
static flags_t calculateTwistLimit(
	const float angle, const float angularLimits[2],
	vec3 *const restrict axis, float *const restrict angleBias
){

	// If the lower limit is broken, use it for the bias.
	// Note that the bias is negative when the lower limit is broken,
	// so to get a positive bias, we need to negate the constraint axis.
	if(angle < angularLimits[0]){
		*angleBias = angularLimits[0] - angle;
		vec3Negate(axis);
		#ifdef PHYSJOINTSPHERE_DEBUG
		twistAngleDebug = angularLimits[0];
		#endif
		return(PHYSJOINTSPHERE_LIMITS_TWIST_LOWER);

	// If the upper limit is broken, use it instead.
	// The bias is positive when the upper limit is broken.
	}else if(angle > angularLimits[1]){
		*angleBias = angle - angularLimits[1];
		#ifdef PHYSJOINTSPHERE_DEBUG
		twistAngleDebug = angularLimits[1];
		#endif
		return(PHYSJOINTSPHERE_LIMITS_TWIST_UPPER);
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
static void swingImpulseRemoveTwist(const vec3 *const restrict twistAxis, vec3 *const restrict swingImpulse){
	vec3 impulseTwistComponent;
	// Project the impulse onto the twist axis.
	vec3MultiplySOut(twistAxis, vec3DotVec3(swingImpulse, twistAxis), &impulseTwistComponent);
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
	// lambda = -(JV + b)/(JM^{-1}J^T)
	const float lambda = -(vec3DotVec3(relVel, axis) + bias) * invMass;

	// Clamp the accumulated impulse magnitude.
	const float oldImpulse = *accumulatedImpulse;
	*accumulatedImpulse = floatMin(oldImpulse + lambda, 0.f);

	// Calculate the total impulse.
	vec3Fmaf(*accumulatedImpulse - oldImpulse, axis, impulse);
}