#include "physicsJointDistance.h"


#include <math.h>

#include "physicsJoint.h"
#include "physicsRigidBody.h"

#include "utilMath.h"


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
** C : d - L = 0,
**
** where d = (pB - pA)/||pB - pA|| is the normalized axis of separation for
** the two rigid bodies and L is the distance we want to maintain between them.
** Note that for fixed joints with L = 0, there is a singularity (since we
** need to normalize d). In this case, you probably want a spherical joint.
**
**
** Differentiate with respect to time to get a velocity constraint:
**
** C' : ((vB + wB X rB) - (vA + wA X rA)) . d = 0.
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
** The effective mass for the constraint is given by JM^{-1}J^T,
** where M^{-1} is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**          [mA^{-1}    0       0       0   ]
**          [   0    IA^{-1}    0       0   ]
** M^{-1} = [   0       0    mB^{-1}    0   ],
**          [   0       0       0    IB^{-1}]
**
**       [    -d   ]
**       [-(rA X d)]
** J^T = [     d   ].
**       [ (rB X d)]
**
**
** Evaluating this expression gives us:
**
** JM^{-1}J^T = mA^{-1} + mB^{-1} + ((rA X d) . (IA^{-1} * (rA X d))) + ((rB X d) . (IB^{-1} * (rB X d))).
**
** ----------------------------------------------------------------------
*/

/*
** "Magic" formulas for ODE's soft constraints:
**
** C  : d - L = 0
** C' : JV + b = 0
** JV + beta/h * C + gamma * lambda = 0
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
static float calculateEffectiveMass(
	const vec3 *const restrict rA, const vec3 *const restrict rB, const vec3 *const restrict rAB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
static void calculateBias(physicsJointDistance *const restrict joint, const float dt);


/*
** Set up some of the joint's properties. The damping
** ratio decides how quickly oscillations should cease:
**     0 = no damping,
**     1 = no oscillations.
*/
void physJointDistanceInit(
	physicsJointDistance *const restrict joint,
	const vec3 *const restrict anchorA, const vec3 *const restrict anchorB,
	const float distance, const float stiffness, const float damping
){

	joint->anchorA = *anchorA;
	joint->anchorB = *anchorB;

	joint->distance = distance;
	joint->stiffness = stiffness;
	joint->damping = damping;

	joint->impulse = 0.f;
}


/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
** Joints are always active so we always warm start.
*/
#ifdef PHYSJOINTDISTANCE_WARM_START
void physJointDistanceWarmStart(const physicsJointDistance *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
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
	// We don't invert the effective mass just yet, as we need it for the bias.
	((physicsJointDistance *)joint)->invEffectiveMass = calculateEffectiveMass(
		&((physicsJointDistance *)joint)->rA, &((physicsJointDistance *)joint)->rB,
		&((physicsJointDistance *)joint)->rAB, bodyA, bodyB
	);
	calculateBias((physicsJointDistance *)joint, dt);
	// Now we can invert it.
	((physicsJointDistance *)joint)->invEffectiveMass = 1.f/((physicsJointDistance *)joint)->invEffectiveMass;
	#ifdef PHYSJOINTDISTANCE_WARM_START
	physJointDistanceWarmStart((physicsJointDistance *)joint, bodyA, bodyB);
	#endif
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointDistanceSolveVelocity(
	void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	float lambda;
	vec3 impulse;
	vec3 relativeVelocity;


	// vA_anchor  = vA + wA X rA
	// vB_anchor  = vB + wB X rB
	// v_relative = vB_anchor - vA_anchor

	// Calculate the total velocity of the anchor point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointDistance *)joint)->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total velocity of the anchor point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointDistance *)joint)->rB, &relativeVelocity);
	vec3AddVec3(&relativeVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3P1(&relativeVelocity, &impulse);


	// lambda = -(JV + b)/(JM^{-1}J^T)
	//        = -((v_relative . d) + b)/K
	lambda = -(vec3DotVec3(&relativeVelocity, &((physicsJointDistance *)joint)->rAB) + ((physicsJointDistance *)joint)->bias +
	         ((physicsJointDistance *)joint)->gamma * ((physicsJointDistance *)joint)->impulse) * ((physicsJointDistance *)joint)->invEffectiveMass;
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
return_t physJointDistanceSolvePosition(
	const void *const restrict joint, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	#ifdef PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL
	// If we're not using soft constraints, we can perform positional correction.
	if(((physicsJointDistance *)joint)->stiffness <= 0.f){
		vec3 rA;
		vec3 rB;
		vec3 rAB;

		// Transform the anchor points using the bodies' new scales and rotations.
		// We subtract the local centroid positions to get them relative to the centroids.
		vec3SubtractVec3Out(&((physicsJointDistance *)joint)->anchorA, &bodyA->base->centroid, &rA);
		transformDirection(&bodyA->state, &rA);
		vec3SubtractVec3Out(&((physicsJointDistance *)joint)->anchorB, &bodyB->base->centroid, &rB);
		transformDirection(&bodyB->state, &rB);

		// Find the relative position of the two bodies.
		// d = (pB - pA)
		//   = (cB + rB - cA - rA)
		vec3AddVec3Out(&bodyB->centroid, &rB, &rAB);
		vec3SubtractVec3P1(&rAB, &bodyA->centroid);
		vec3SubtractVec3P1(&rAB, &rA);

		{
			const float distance = vec3MagnitudeVec3(&rAB);
			const float error = distance - ((physicsJointDistance *)joint)->distance;
			if(distance > PHYSJOINT_LINEAR_SLOP){
				// Clamp the constraint value.
				const float constraint = error;/*floatClamp(
					PHYSJOINTDISTANCE_BAUMGARTE_BIAS * error,
					-PHYSJOINT_MAX_LINEAR_CORRECTION,
					PHYSJOINT_MAX_LINEAR_CORRECTION
				);*/
				float effectiveMass;

				vec3DivideByS(&rAB, distance);
				effectiveMass = calculateEffectiveMass(&rA, &rB, &rAB, bodyA, bodyB);
				vec3MultiplyS(&rAB, -constraint/effectiveMass);

				// Apply the correctional impulse.
				physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &rAB);
				physRigidBodyApplyImpulsePosition(bodyB, &rB, &rAB);
			}

			return(fabsf(error) <= PHYSJOINT_LINEAR_POSITIONAL_ERROR_THRESHOLD);
		}
	}
	#endif

	return(1);
}


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
	// We subtract the local centroid positions to get them relative to the centroids.
	vec3SubtractVec3Out(&joint->anchorA, &bodyA->base->centroid, &joint->rA);
	transformDirection(&bodyA->state, &joint->rA);
	vec3SubtractVec3Out(&joint->anchorB, &bodyB->base->centroid, &joint->rB);
	transformDirection(&bodyB->state, &joint->rB);

	// Find the relative position of the two bodies.
	// d = (pB - pA)
	//   = (cB + rB - cA - rA)
	vec3AddVec3Out(&bodyB->centroid, &joint->rB, &joint->rAB);
	vec3SubtractVec3P1(&joint->rAB, &bodyA->centroid);
	vec3SubtractVec3P1(&joint->rAB, &joint->rA);

	// When we calculate the effective mass, we
	// need to normalize the relative position.
	// We also use the magnitude for the bias term.
	distance = vec3MagnitudeVec3(&joint->rAB);
	if(distance > PHYSJOINT_LINEAR_SLOP){
		vec3DivideByS(&joint->rAB, distance);
	}else{
		vec3InitZero(&joint->rAB);
	}
	// Calculate C for our bias term.
	// beta = hk/(c + hk)
	// bias = beta/h * C
	joint->bias = distance - joint->distance;
}

/*
** Calculate the effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once
** per update. Note that we take its inverse elsewhere.
*/
static float calculateEffectiveMass(
	const vec3 *const restrict rA, const vec3 *const restrict rB, const vec3 *const restrict rAB,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	vec3 rAd;
	vec3 IArAd;
	vec3 rBd;
	vec3 IBrBd;

	// JM^{-1}J^T = mA^{-1} + mB^{-1} + ((rA X d) . (IA^{-1} * (rA X d))) + ((rB X d) . (IB^{-1} * (rB X d)))
	vec3CrossVec3Out(rA, rAB, &rAd);
	mat3MultiplyVec3Out(&bodyA->invInertiaGlobal, &rAd, &IArAd);
	vec3CrossVec3Out(rB, rAB, &rBd);
	mat3MultiplyVec3Out(&bodyB->invInertiaGlobal, &rBd, &IBrBd);
	// We don't take the inverse just yet, as we
	// need this when calculating the bias term.
	return(bodyA->invMass + bodyB->invMass + vec3DotVec3(&rAd, &IArAd) + vec3DotVec3(&rBd, &IBrBd));
}

/*
** Calculate the joint's bias term. This will help
** to soften the constraint and make it springy.
*/
static void calculateBias(physicsJointDistance *const restrict joint, const float dt){
	// Only use soft constraints if the stiffness is greater than 0.
	if(joint->stiffness <= 0.f){
		joint->gamma = 0.f;
		#ifdef PHYSJOINTDISTANCE_STABILISER_BAUMGARTE
		joint->bias *= PHYSJOINTFIXED_BAUMGARTE_BIAS/dt;
		#else
		joint->bias = 0.f;
		#endif
	}else{
		const float kdt = joint->stiffness * dt;

		// Calculate gamma:
		//     gamma = 1/(c + hk)
		// The extra 'h' in the denominator here is because
		// we're using impulses rather than forces.
		joint->gamma = dt * (joint->damping + kdt);
		joint->gamma = (joint->gamma != 0.f) ? 1.f / joint->gamma : 0.f;

		// Calculate the bias term. It should already be set to 'C'.
		//     beta = hk/(c + hk) = hk * gamma,
		//     bias = beta/h * C = k * gamma * C.
		// The factor of dt is to cancel the one we added to gamma.
		joint->bias *= kdt * joint->gamma;

		// lambda = -((v_relative . d) + b + gamma*lambda)/K,
		// lambda(K + gamma)/K = -((v_relative . d) + b)/K,
		// lambda = -((v_relative . d) + b)/(K + gamma).
		joint->invEffectiveMass += joint->gamma;
	}
}