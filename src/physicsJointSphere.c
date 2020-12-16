#include "physicsJointSphere.h"


#include "physicsRigidBody.h"


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
** This is a vector, which makes things a bit harder later on.
** To make things easier later on, we dot these with the standard
** basis {e1, e2, e3}:
**
** C1: ((pB + aB) - (pA + aA)) . e1 = 0,
** C2: ((pB + aB) - (pA + aA)) . e2 = 0,
** C3: ((pB + aB) - (pA + aA)) . e3 = 0.
**
** Differentiate with respect to time to get a velocity constraint:
**
** C1' : (vB +   wB X aB) - (vA +   wA X aA) = 0,
**     : (vB -   aB X wB) - (vA -   aA X wA) = 0,
**     : (vB - [aB]_X*wB) - (vA - [aA]_X*wA) = 0,
**
** where "[.]_X" denotes the skew-symmetric "cross product" matrix.
**
** We also need to impose a constraint on the angular velocities:
**
** C2' : wB - wA = 0.
**
** This constraint in particular is used to limit the relative
** rotation of rigid body B on the three cardinal axes (in rigid
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
** J1 = [-I, [aA]_X, I, -[aB]_X],
**
** J2 = [0, -1, 0, 1].
**
** ----------------------------------------------------------------------
**
** The effective mass for the constraint is given by (JM^(-1))J^T,
** where M^(-1) is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**          [mA^(-1)    0       0       0   ]
**          [   0    IA^(-1)    0       0   ]
** M^(-1) = [   0       0    mB^(-1)    0   ],
**          [   0       0       0    IB^(-1)]
**
**        [  -I   ]
**        [-[aA]_X]
** J1^T = [   I   ].
**        [ [aB]_X]
**
**
** Evaluating this expression gives us the following matrix for our
** point-to-point constraint:
**
** J1*M^(-1) = [-mA^(-1), [aA]_X*IA^(-1), mB^(-1), -[aB]_X*IB^(-1)],
**
** K1 = (J1*M^(-1))J1^T
**    = (mA^(-1) + mB^(-1))*I_3 - [aA]_X*IA^(-1)*[aA]_X - [aB]_X*IB^(-1)*[aB]_X,
**
** where "I_3" is the 3x3 identity matrix.
** For our angular constraint, we simply get:
**
** J2*M^(-1) = [0, -IA^(-1), 0, IB^(-1)],
**
** K2 = (J2*M^(-1))J2^T = IA^(-1) + IB^(-1).
**
** Note that K1 and K2 are both 3x3 matrices.
**
** ----------------------------------------------------------------------
**
** For the angular constraint, we also introduce a bias dependent
** on C2. Although C2 wasn't given, we use it to clamp the minimum
** and maximum rotation on each axis. For each Euler angle:
**
** b_{upper} = angle_max - angle >= 0,
** b_{lower} = angle - angle_min >= 0.
**
** ----------------------------------------------------------------------
*/
/**
How do we handle the axes for the biases? Should they just be the cardinal axes?
This would perhaps make sense, as we are using relative orientations.

https://github.com/erincatto/box2d/blob/master/src/dynamics/b2_revolute_joint.cpp
https://github.com/bulletphysics/bullet3/blob/master/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
https://github.com/dtecta/motion-toolkit
**/


// Forward-declare any helper functions!
static void updateConstraintData(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
static void calculateEffectiveMass(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
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
	const vec3 *axisA, const vec3 *axisB,
	const float minX, const float maxX,
	const float minY, const float maxY,
	const float minZ, const float maxZ
){

	joint->axisLocalA = *axisA;
	joint->axisLocalB = *axisB;

	vec3InitSet(&joint->anglesMin, minX, minY, minZ);
	vec3InitSet(&joint->anglesMax, maxX, maxY, maxZ);
}


#ifdef PHYSJOINTSPHERE_WARM_START
void physJointSphereWarmStart(physicsJointSphere *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	//
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
	calculateEffectiveMass((physicsJointSphere *)joint, bodyA, bodyB);
	#ifdef PHYSJOINTSPHERE_WARM_START
	physJointSphereWarmStart((physicsJointSphere *)joint, bodyA, bodyB);
	#endif
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointSphereSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	//
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
#ifdef PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL
return_t physJointSphereSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	return(1);
}
#endif


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
** Calculate the effective mass of the constraint, which won't change
** between velocity iterations. We can just do it once per update.
*/
static void calculateEffectiveMass(
	physicsJointSphere *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	// Compute the linear effective mass given by:
	// K1 = (mA^(-1) + mB^(-1))*I_3 - [aA]_X*IA^(-1)*[aA]_X - [aB]_X*IB^(-1)*[aB]_X.
	//
	// I'm not sure if there's a better way to do
	// this than to just plough through the maths.
	{
		// Compute body A's contribution to K1. Note that the matrix "[a]_X*I^(-1)*[a]_X"
		// is symmetric, so we only need to compute the half of the off-diagonal elements.

		// These variables bring it down to 27 multiplications and 14 subtractions.
		// An alternative method gives 24 multiplications and 21 subtractions,
		// but float multiplication is generally faster and has less error.
		//
		// Note that "a" refers to the axis and "A" is "[a]_X*I^(-1)".
		const vec3 a = joint->axisGlobalA;
		const float I[6] = {
			bodyA->invInertiaGlobal.m[0][0], bodyA->invInertiaGlobal.m[0][1], bodyA->invInertiaGlobal.m[0][2],
			                                 bodyA->invInertiaGlobal.m[1][1], bodyA->invInertiaGlobal.m[1][2],
			                                                                  bodyA->invInertiaGlobal.m[2][2]
		};
		const float a3xy = a.z*I[1];
		const float A[8] = {
			a.y*I[2] -     a3xy, a.y*I[4] - a.z*I[3], a.y*I[5] - a.z*I[4],
			a.z*I[0] - a.x*I[2],     a3xy - a.x*I[4], a.z*I[2] - a.x*I[5],
			a.x*I[1] - a.y*I[0], a.x*I[3] - a.y*I[1]
		};
		const float invMass = bodyA->invMass;

		// Compute "mA^(-1)*I_3 - [aA]_X*IA^(-1)*[aA]_X".
		// We don't set the lower triangular components here, as
		// we can just do that when adding body B's contribution.
		joint->linearMass.m[0][0] = a.y*A[2] - a.z*A[1] + invMass;
		joint->linearMass.m[0][1] = a.z*A[0] - a.x*A[2];
		joint->linearMass.m[0][2] = a.x*A[1] - a.y*A[0];
		joint->linearMass.m[1][1] = a.x*A[5] - a.z*A[3] + invMass;
		joint->linearMass.m[1][2] = a.y*A[3] - a.x*A[4];
		joint->linearMass.m[2][2] = a.y*A[6] - a.x*A[7] + invMass;
	}
	{
		// Compute body B's contribution to K1.
		const vec3 a = joint->axisGlobalB;
		const float I[6] = {
			bodyB->invInertiaGlobal.m[0][0], bodyB->invInertiaGlobal.m[0][1], bodyB->invInertiaGlobal.m[0][2],
			                                 bodyB->invInertiaGlobal.m[1][1], bodyB->invInertiaGlobal.m[1][2],
			                                                                  bodyB->invInertiaGlobal.m[2][2]
		};
		const float a3xy = a.z*I[1];
		const float A[8] = {
			a.y*I[2] -     a3xy, a.y*I[4] - a.z*I[3], a.y*I[5] - a.z*I[4],
			a.z*I[0] - a.x*I[2],     a3xy - a.x*I[4], a.z*I[2] - a.x*I[5],
			a.x*I[1] - a.y*I[0], a.x*I[3] - a.y*I[1]
		};
		const float invMass = bodyB->invMass;

		// Compute "mB^(-1)*I_3 - [aB]_X*IB^(-1)*[aB]_X".
		joint->linearMass.m[0][0] += a.y*A[2] - a.z*A[1] + invMass;
		joint->linearMass.m[0][1] += a.z*A[0] - a.x*A[2];
		joint->linearMass.m[0][2] += a.x*A[1] - a.y*A[0];
		joint->linearMass.m[1][0] = joint->linearMass.m[0][1];
		joint->linearMass.m[1][1] += a.x*A[5] - a.z*A[3] + invMass;
		joint->linearMass.m[1][2] += a.y*A[3] - a.x*A[4];
		joint->linearMass.m[2][0] = joint->linearMass.m[0][2];
		joint->linearMass.m[2][1] = joint->linearMass.m[1][2];
		joint->linearMass.m[2][2] += a.y*A[6] - a.x*A[7] + invMass;
	}

	/**
	vec3 rAd;
	vec3 IArAd;
	vec3 rBd;
	vec3 IBrBd;

	// (JM^(-1))J^T = mA^(-1) + mB^(-1) + ((rA X d) . (IA^(-1) * (rA X d))) + ((rB X d) . (IB^(-1) * (rB X d)))
	vec3CrossVec3Out(&joint->rA, &joint->rAB, &rAd);
	mat3MultiplyByVec3Out(&bodyA->invInertiaGlobal, &rAd, &IArAd);
	vec3CrossVec3Out(&joint->rB, &joint->rAB, &rBd);
	mat3MultiplyByVec3Out(&bodyB->invInertiaGlobal, &rBd, &IBrBd);
	// We don't take the inverse just yet, as we
	// need this when calculating the bias term.
	return(bodyA->invMass + bodyB->invMass + vec3DotVec3(&rAd, &IArAd) + vec3DotVec3(&rBd, &IBrBd));
	**/
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