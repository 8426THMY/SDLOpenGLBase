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
** For the point-to-point constraint, we define aA and aB to be
** the offsets of the "socket" and "ball" from the rigid bodies
** respectively. Of course, these two points should coincide, so
**
** C : (pB + aB) - (pA + aA) = 0.
**
** Differentiate with respect to time to get a velocity constraint:
**
** C1' : (vB +   wB X aB) - (vA +   wA X aA) = 0,
**     : (vB -   aB X wB) - (vA -   aA X wA) = 0,
**     : (vB - [aB]_X*wB) - (vA - [aA]_X*wA) = 0,
**
** where "[.]_X" denotes the skew-symmetric "cross product" matrix.
**
** We also need to impose a constraint on the angular velocities
** of the rigid bodies themselves, not the joint axis:
**
** C2' : wB - wA = 0.
**
** This constraint in particular is used to limit the relative
** rotation of rigid body B on all three axes (taken in rigid
** body A's space).
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
** We will use two separate Jacobians, J1 for C1' and J2 for C2'.
**
** J1 = [-I_3, [aA]_X, I_3, -[aB]_X],
**
** J2 = [0, -1, 0, 1].
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
**        [-[aA]_X]
** J1^T = [  I_3  ].
**        [ [aB]_X]
**
**
** Evaluating this expression gives us the following matrix for our
** point-to-point constraint:
**
**               [-mA^(-1) * I_3   ]
**               [-IA^(-1) * [aA]_X]
** M^(-1)*J1^T = [ mB^(-1) * I_3   ],
**               [ IB^(-1) * [aB]_X]
**
** K1 = J1*M^(-1)*J1^T
**    = (mA^(-1) + mB^(-1))*I_3 - [aA]_X*IA^(-1)*[aA]_X - [aB]_X*IB^(-1)*[aB]_X,
**
** For our angular constraint, we simply get:
**
**               [    0   ]
**               [-IA^(-1)]
** M^(-1)*J2^T = [    0   ].
**               [ IB^(-1)]
**
** K2 = J2*M^(-1)*J2^T = IA^(-1) + IB^(-1).
**
** Note that K1 and K2 are both 3x3 matrices.
**
** ----------------------------------------------------------------------
**
** For the angular constraint, we also introduce a bias dependent
** on C2. Although C2 wasn't given, we use it to clamp the minimum
** and maximum rotation on each axis. To solve for C2, we need to:
**
** 1. Compute the relative orientation Rr (A -> B).
** 2. Convert Rr to Euler angles (Re).
** 3. Clamp Re to Rc using the upper and lower bounds on the angles.
** 4. The rotation required to correct the relative orientation is Rd = conj(Rc)*Rr.
** 5. Let b = C2/dt = axisAngle(Rd)/dt.
** 6. Compute relative angular velocity JV (A -> B).
** 7. Compute lambda = -(JV + b)/(JM^(-1)J^T).
** 8. Apply the impulse.
**
** Note that the bias term 'b' should be in axis-angle form.
** Also, we use Rd = Rc -> Rr to get the correction with respect to body A.
**
** ----------------------------------------------------------------------
*/
/**
How do we handle the axes for the biases? Should they just be the cardinal axes?
This would perhaps make sense, as we are using relative orientations.

https://github.com/erincatto/box2d/blob/master/src/dynamics/b2_revolute_joint.cpp
https://github.com/bulletphysics/bullet3/blob/master/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
https://github.com/kovertopz/jReactPhysics3D/blob/master/src/main/java/net/smert/jreactphysics3d/constraint/BallAndSocketJoint.java
https://github.com/dtecta/motion-toolkit
**/


// Forward-declare any helper functions!
static void updateConstraintData(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
static void calculateLinearMass(
	const vec3 aA, const vec3 aB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict linearMass
);
static void calculateAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict angularMass
);
static void calculateBias(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float dt
);


/*
** The inputs for the ball and socket joint shouldn't be too mysterious:
**     axisA            - Vector from rigid body A's point of reference to the socket.
**     axisB            - Vector from rigid body B's point of reference to the ball.
**     {min,max}{X,Y,Z} - Angles used to clamp the relative orientation of body B.
** Note that the minimum and maximum angles are given in radians.
** The axes are taken relative to rigid body A's frame of reference.
*/
void physJointSphereInit(
	physicsJointSphere *const restrict joint,
	const vec3 *const restrict axisA, const vec3 *const restrict axisB,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
){

	joint->axisLocalA = *axisA;
	joint->axisLocalB = *axisB;

	vec3InitZero(&joint->linearImpulse);
	vec3InitZero(&joint->angularImpulse);
}


#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(physicsJointSphere *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	// Apply the accumulated impulses.
	physRigidBodyApplyImpulseBoostInverse(bodyA, &joint->axisGlobalA, &joint->linearImpulse, &joint->angularImpulse);
	physRigidBodyApplyImpulseBoost(bodyB, &joint->axisGlobalB, &joint->linearImpulse, &joint->angularImpulse);
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
	calculateLinearMass(
		((physicsJointSphere *)joint)->axisGlobalA, ((physicsJointSphere *)joint)->axisGlobalB,
		bodyA, bodyB, &((physicsJointSphere *)joint)->linearMass
	);
	calculateAngularMass(bodyA, bodyB, &((physicsJointSphere *)joint)->angularMass);
	calculateBias((physicsJointSphere *)joint, bodyA, bodyB, dt);
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

	// Solve the linear point-to-point constraint last,
	// as it's more important that it's satisfied.
	{
		vec3 relativeVelocity;
		vec3 impulse;

		// v_socket   = vA + wA X aA
		// v_ball     = vB + wB X aB
		// v_relative = v_ball - v_socket

		// Calculate the total velocity of the socket.
		vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointSphere *)joint)->axisGlobalA, &impulse);
		vec3AddVec3(&impulse, &bodyA->linearVelocity);
		// Calculate the total velocity of the ball.
		vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointSphere *)joint)->axisGlobalB, &relativeVelocity);
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
		physRigidBodyApplyImpulseInverse(bodyA, &((physicsJointSphere *)joint)->axisGlobalA, &impulse);
		physRigidBodyApplyImpulse(bodyB, &((physicsJointSphere *)joint)->axisGlobalB, &impulse);
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
	float linearError;

	// Solve the linear point-to-point constraint.
	{
		vec3 aA, aB;
		vec3 constraint;
		mat3 linearMass;
		vec3 impulse;

		vec3MultiplyVec3Out(&bodyA->state.scale, &((physicsJointSphere *)joint)->axisLocalA, &aA);
		quatRotateVec3Fast(&bodyA->state.rot, &aA);
		vec3MultiplyVec3Out(&bodyB->state.scale, &((physicsJointSphere *)joint)->axisLocalB, &aB);
		quatRotateVec3Fast(&bodyB->state.rot, &aB);

		// Calculate the displacement from the ball to the socket:
		// -C1 = (pA + aA) - (pB - aB).
		vec3AddVec3Out(&bodyA->centroid, &aA, &constraint);
		vec3SubtractVec3From(&constraint, &bodyB->centroid);
		vec3SubtractVec3From(&constraint, &aB);
		linearError = vec3MagnitudeVec3(&constraint);

		calculateLinearMass(aA, aB, bodyA, bodyB, &linearMass);
		mat3Solve(&linearMass, &constraint, &impulse);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulseInverse(bodyA, &aA, &impulse);
		physRigidBodyApplyImpulse(bodyB, &aB, &impulse);
	}

	return(linearError <= PHYSCONSTRAINT_LINEAR_SLOP);
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
	vec3MultiplyVec3Out(&bodyA->state.scale, &joint->axisLocalA, &joint->axisGlobalA);
	quatRotateVec3Fast(&bodyA->state.rot, &joint->axisGlobalA);
	vec3MultiplyVec3Out(&bodyB->state.scale, &joint->axisLocalB, &joint->axisGlobalB);
	quatRotateVec3Fast(&bodyB->state.rot, &joint->axisGlobalB);
}

/*
** Calculate the linear effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once per update.
*/
static void calculateLinearMass(
	const vec3 aA, const vec3 aB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict linearMass
){

	// Compute the linear effective mass given by:
	// K1 = (mA^(-1) + mB^(-1))*I_3 - [aA]_X*IA^(-1)*[aA]_X - [aB]_X*IB^(-1)*[aB]_X.
	//
	// This probably isn't so good, as [aA]_X*IA^(-1)*[aA]_X and [aB]_X*IB^(-1)*[aB]_X
	// are necessarily singular. We don't invert the matrix here, as it's faster to
	// solve for lambda using Cramer's rule.

	// Compute body A's contribution to K1.
	{
		// Note that the matrix [aA]_X*IA^(-1)*[aA]_X is symmetric, so we
		// only need to compute the half of the off-diagonal elements.

		// These variables bring it down to 27 multiplications and 14 subtractions.
		// An alternative method gives 24 multiplications and 21 subtractions,
		// but float multiplication is generally faster and has less error.
		const float I[6] = {
			bodyA->invInertiaGlobal.m[0][0], bodyA->invInertiaGlobal.m[0][1], bodyA->invInertiaGlobal.m[0][2],
			                                 bodyA->invInertiaGlobal.m[1][1], bodyA->invInertiaGlobal.m[1][2],
			                                                                  bodyA->invInertiaGlobal.m[2][2]
		};
		const float a3xy = aA.z*I[1];
		const float A[8] = {
			aA.y*I[2] -      a3xy, aA.y*I[4] - aA.z*I[3], aA.y*I[5] - aA.z*I[4],
			aA.z*I[0] - aA.x*I[2],      a3xy - aA.x*I[4], aA.z*I[2] - aA.x*I[5],
			aA.x*I[1] - aA.y*I[0], aA.x*I[3] - aA.y*I[1]
		};

		// Compute -[aA]_X*IA^(-1)*[aA]_X.
		// We don't set the lower triangular components here, as
		// we can just do that when adding body B's contribution.
		linearMass->m[0][0] = aA.y*A[2] - aA.z*A[1];
		linearMass->m[0][1] = aA.z*A[0] - aA.x*A[2];
		linearMass->m[0][2] = aA.x*A[1] - aA.y*A[0];
		linearMass->m[1][1] = aA.z*A[3] - aA.x*A[5];
		linearMass->m[1][2] = aA.x*A[4] - aA.y*A[3];
		linearMass->m[2][2] = aA.x*A[7] - aA.y*A[6];
	}
	// Compute body B's contribution to K1.
	{
		const float I[6] = {
			bodyB->invInertiaGlobal.m[0][0], bodyB->invInertiaGlobal.m[0][1], bodyB->invInertiaGlobal.m[0][2],
			                                 bodyB->invInertiaGlobal.m[1][1], bodyB->invInertiaGlobal.m[1][2],
			                                                                  bodyB->invInertiaGlobal.m[2][2]
		};
		const float a3xy = aB.z*I[1];
		const float A[8] = {
			aB.y*I[2] -      a3xy, aB.y*I[4] - aB.z*I[3], aB.y*I[5] - aB.z*I[4],
			aB.z*I[0] - aB.x*I[2],      a3xy - aB.x*I[4], aB.z*I[2] - aB.x*I[5],
			aB.x*I[1] - aB.y*I[0], aB.x*I[3] - aB.y*I[1]
		};
		const float invMass = bodyA->invMass + bodyB->invMass;

		// Compute (mA^(-1) + mB^(-1))*I_3 - [aB]_X*IB^(-1)*[aB]_X.
		linearMass->m[0][0] += aB.y*A[2] - aB.z*A[1] + invMass;
		linearMass->m[0][1] += aB.z*A[0] - aB.x*A[2];
		linearMass->m[0][2] += aB.x*A[1] - aB.y*A[0];
		linearMass->m[1][0] = linearMass->m[0][1];
		linearMass->m[1][1] += aB.z*A[3] - aB.x*A[5] + invMass;
		linearMass->m[1][2] += aB.x*A[4] - aB.y*A[3];
		linearMass->m[2][0] = linearMass->m[0][2];
		linearMass->m[2][1] = linearMass->m[1][2];
		linearMass->m[2][2] += aB.x*A[7] - aB.y*A[6] + invMass;
	}
}

/*
** Calculate the angular effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once per update.
*/
static void calculateAngularMass(
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	mat3 *const restrict angularMass
){

	// K2 = IA^(-1) + IB^(-1).
	// We don't invert it here, as it's faster to
	// use Cramer's rule later to solve for lambda.
	mat3AddMat3Out(&bodyA->invInertiaGlobal, &bodyB->invInertiaGlobal, angularMass);
}

/*
** Calculate the joint's bias term. This is used for
** clamping the relative orientation of rigid body B.
*/
static void calculateBias(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float dt
){

	//
}