#include "physicsJointDistance.h"


#include <math.h>

#include "physicsJoint.h"
#include "physicsRigidBody.h"


/*
** ----------------------------------------------------------------------
**
** Distance constraints ensure that the two points specified, one on
** each rigid body, are always a certain distance from each other.
** We allow develoeprs to set how rigidly this constraint should be
** satisfied, which controls how spring-like it should behave.
**
** ----------------------------------------------------------------------
**
** Distance constraint equation:
**
** C : (1/2)(d^2 - L^2) = 0,
**   : (1/2)(d . d - L^2) = 0,
**
** where d = (pB - pA) is the current separation of the points on the two
** rigid bodies and L is the distance we want to maintain between them.
**
**
** Differentiate with respect to time to get a velocity constraint:
**
** C' : ((vB + wB X rB) - (vA + wA X rA)) . (pB - pA) = 0.
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
** and the identity C' = JV, we can solve for the Jacobian J:
**
** J = [-d, -(rA X d), d, (rB X d)].
**
**
** Finally, adding a potential bias term, we have
**
** C' : JV + b >= 0.
**
** This bias term is explained below.
**
** ----------------------------------------------------------------------
**
** The effective mass for the constraint is given by (JM^(-1))J^T,
** where M^(-1) is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**          [mA^(-1)    0       0       0   ]
**          [   0    IA^(-1)    0       0   ]
** M^(-1) = [   0       0    mB^(-1)    0   ]
**          [   0       0       0    IB^(-1)],
**
**       [    -d   ]
**       [-(rA X d)]
** J^T = [     d   ]
**       [ (rB X d)].
**
**
** Evaluating this expression gives us:
**
** (JM^(-1))J^T = mA^(-1) + mB^(-1) + ((rA X d) . (IA^(-1) * (rA X d))) + ((rB X d) . (IB^(-1) * (rB X d))).
**
** ----------------------------------------------------------------------
*/

/*
** "Magic" formulas for ODE's soft constraints:
**
** C  : d - L = 0
** C' : JV + b = 0
** JV + beta/h * C(x) + gamma * lambda = 0
**
** gamma = 1/(c + hk)
** beta = hk/(c + hk)
**
**
** Note that h is the timestep, k is the spring's
** stiffness and c is the spring's damping coefficient.
*/


// Forward-declare any helper functions!
static void updateConstraintData(
	physicsJointDistance *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
static void calculateEffectiveMass(
	physicsJointDistance *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
static void calculateBias(
	physicsJointDistance *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float dt
);


/*
** Set up some of the joint's properties. Note that the frequency passed
** into this function should be given in hertz (that is, cycles per
** second) whereas the value we store is the angular frequency (radians
** per second). If the frequency is 0, soft constraints will be disabled.
**
** The damping ratio decides how quickly oscillations should cease.
** 0 = no damping
** 1 = no oscillations
*/
void physJointDistanceInit(physicsJointDistance *const restrict joint, const float frequency, const float dampingRatio){
	// w = 2pi * f
	joint->angularFrequency = 2.f * M_PI * frequency;
	// damp = 2w * zeta
	joint->damping = 2.f * joint->angularFrequency * dampingRatio;
}


/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
** Joints are always active so we always warm start.
*/
#ifdef PHYSJOINTDISTANCE_WARM_START
void physJointDistanceWarmStart(physicsJointDistance *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	vec3 impulse;

	vec3MultiplySOut(&joint->rAB, joint->impulse, &impulse);


	// Apply the accumulated impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &joint->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &joint->rB, &impulse);
}
#endif

/*
** Prepare the constraint for velocity solving. We need to
** update the joint's members to reflect the movement of
** the bodies that it is affecting.
*/
void physJointDistancePresolve(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, const float dt
){

	updateConstraintData((physicsJointDistance *)joint, bodyA, bodyB);
	calculateEffectiveMass((physicsJointDistance *)joint, bodyA, bodyB);
	calculateBias((physicsJointDistance *)joint, bodyA, bodyB, dt);
	#ifdef PHYSJOINTDISTANCE_WARM_START
	physJointDistanceWarmStart((physicsJointDistance *)joint, bodyA, bodyB);
	#endif
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointDistanceSolveVelocity(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	float lambda;
	vec3 impulse;
	vec3 relativeVelocity;


	// vA_anchor  = vA + wA X rA
	// vB_anchor  = vB + wB X rB
	// v_relative = vB_anchor - vA_anchor

	// Calculate the total linear velocity of the anchor point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointDistance *)joint)->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the anchor point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointDistance *)joint)->rB, &relativeVelocity);
	vec3AddVec3(&relativeVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&relativeVelocity, &impulse);


	// lambda = -(JV + b)/((JM^(-1))J^T)
	//        = -((v_relative . d) + b)/K
	lambda = -(vec3DotVec3(&relativeVelocity, &((physicsJointDistance *)joint)->rAB) + ((physicsJointDistance *)joint)->bias +
	         ((physicsJointDistance *)joint)->gamma * ((physicsJointDistance *)joint)->impulse) * ((physicsJointDistance *)joint)->effectiveMass;
	((physicsJointDistance *)joint)->impulse += lambda;
	vec3MultiplySOut(&((physicsJointDistance *)joint)->rAB, lambda, &impulse);

	// Apply the correctional impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &((physicsJointDistance *)joint)->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &((physicsJointDistance *)joint)->rB, &impulse);
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
#ifdef PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL
return_t physJointDistanceSolvePosition(void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	// If we're not using soft constraints, we can perform positional correction.
	if(((physicsJointDistance *)joint)->angularFrequency <= 0.f){
		vec3 rA;
		vec3 rB;
		vec3 rAB;
		float distance;
		float constraint;

		// Transform the anchor points using the bodies' new scales and rotations.
		vec3MultiplyVec3Out(&bodyA->state.scale, &((physicsJointDistance *)joint)->anchorA, &rA);
		quatRotateVec3Fast(&bodyA->state.rot, &rA);
		vec3MultiplyVec3Out(&bodyB->state.scale, &((physicsJointDistance *)joint)->anchorB, &rB);
		quatRotateVec3Fast(&bodyB->state.rot, &rB);

		// Find the relative position of the two bodies.
		// d = (pB - pA)
		//   = (cB + rB - cA - rA)
		rAB = bodyB->centroid;
		vec3AddVec3(&rAB, &rB);
		vec3SubtractVec3From(&rAB, &bodyA->centroid);
		vec3SubtractVec3From(&rAB, &rA);

		distance = vec3MagnitudeVec3(&rAB);
		constraint = distance - ((physicsJointDistance *)joint)->distance;
		// Clamp the constraint value.
		if(constraint <= -PHYSCONSTRAINT_MAX_LINEAR_CORRECTION){
			constraint = -PHYSCONSTRAINT_MAX_LINEAR_CORRECTION;
		}else if(constraint > PHYSCONSTRAINT_MAX_LINEAR_CORRECTION){
			constraint = PHYSCONSTRAINT_MAX_LINEAR_CORRECTION;
		}

		#warning "Should we calculate the effective mass again? I remember reading something about this..."
		vec3MultiplyS(&rAB, -constraint * ((physicsJointDistance *)joint)->effectiveMass);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &rAB);
		physRigidBodyApplyImpulsePosition(bodyB, &rB, &rAB);


		return(1);//return(constraint);
	}


	return(0);
}
#endif


/*
** Update the global positions of the anchor points
** and some of the other variables that depend on them.
*/
static void updateConstraintData(
	physicsJointDistance *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	float distance;

	// Transform the anchor points using the bodies' new scales and rotations.
	vec3MultiplyVec3Out(&bodyA->state.scale, &joint->anchorA, &joint->rA);
	quatRotateVec3Fast(&bodyA->state.rot, &joint->rA);
	vec3MultiplyVec3Out(&bodyB->state.scale, &joint->anchorB, &joint->rB);
	quatRotateVec3Fast(&bodyB->state.rot, &joint->rB);

	// Find the relative position of the two bodies.
	// d = (pB - pA)
	//   = (cB + rB - cA - rA)
	joint->rAB = bodyB->centroid;
	vec3AddVec3(&joint->rAB, &joint->rB);
	vec3SubtractVec3From(&joint->rAB, &bodyA->centroid);
	vec3SubtractVec3From(&joint->rAB, &joint->rA);

	// When we calculate the effective mass, we
	// need to normalize the relative position.
	// We also use the magnitude for the bias term.
	distance = vec3MagnitudeVec3(&joint->rAB);
	if(distance > PHYSCONSTRAINT_LINEAR_SLOP){
		vec3DivideByS(&joint->rAB, distance);
	}else{
		vec3InitZero(&joint->rAB);
	}
	// Calculate C(x) for our bias term.
	// beta = hk/(c + hk)
	// bias = beta/h * C(x)
	joint->bias = distance - joint->distance;
}

/*
** Calculate the effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once
** per update. Note that we take its inverse elsewhere.
*/
static void calculateEffectiveMass(
	physicsJointDistance *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	vec3 rpA;
	vec3 rpIA;
	vec3 rpB;
	vec3 rpIB;

	// (JM^(-1))J^T = mA^(-1) + mB^(-1) + ((rA X d) . (IA^(-1) * (rA X d))) + ((rB X d) . (IB^(-1) * (rB X d)))
	vec3CrossVec3Out(&joint->rA, &joint->rAB, &rpA);
	mat3MultiplyByVec3Out(&bodyA->invInertiaGlobal, &rpA, &rpIA);
	vec3CrossVec3Out(&joint->rB, &joint->rAB, &rpB);
	mat3MultiplyByVec3Out(&bodyB->invInertiaGlobal, &rpB, &rpIB);
	// We don't take the inverse just yet, as we
	// need this when calculating the bias term.
	joint->effectiveMass = bodyA->invMass + bodyB->invMass + vec3DotVec3(&rpA, &rpIA) + vec3DotVec3(&rpB, &rpIB);
}

/*
** Calculate the joint's bias term. This will help
** to soften the constraint and make it springy.
*/
static void calculateBias(
	physicsJointDistance *const restrict joint,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float dt
){

	// Only use soft constraints if the frequency is greater than 0.
	if(joint->angularFrequency <= 0.f){
		joint->gamma = 0.f;
		joint->bias = 0.f;
	}else{
		const float invEffectiveMass = 1.f / joint->effectiveMass;
		// k = K * w^2
		const float stiffness = invEffectiveMass * joint->angularFrequency * joint->angularFrequency;

		// c = K * damp
		// gamma = 1/(c + hk) = 1/(hk + K * damp)
		joint->gamma = dt * (invEffectiveMass * joint->damping + dt * stiffness);
		joint->gamma = (joint->gamma != 0.f) ? 1.f / joint->gamma : 0.f;
		// beta = hk/(c + hk) = hk * gamma
		// bias = beta/h * C(x) = k * gamma * C(x)
		// Note that bias is already set to C(x).
		joint->bias *= dt * stiffness * joint->gamma;

		// lambda = -((v_relative . d) + beta/h * C(x) + gamma*lambda)/K
		//        = -((v_relative . d) + b)/K - (gamma*lambda)/K
		//
		// lambda(1 + gamma/K) = -((v_relative . d) + b)/K
		//
		// lambda = -((v_relative . d) + b)/K(1 + gamma/K)
		//        = -((v_relative . d) + b)/(K + gamma)
		joint->effectiveMass += joint->gamma;
	}

	joint->effectiveMass = 1.f / joint->effectiveMass;
}