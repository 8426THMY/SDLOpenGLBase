#include "physicsJointDistance.h"


#include <math.h>

#include "physicsJoint.h"
#include "physicsRigidBody.h"


/*
** Original constraint equation:
**
** C : (1/2)((pB - pA)^2 - L^2) = 0
**
**
** Differentiate, letting "p = (pB - pA)",
** as we want to make a change in velocity:
**
** C' : ((vB + wB X rB) - (vA + wA X rA)) . (pB - pA) = 0
**
**
** By inspection:
**
** J = [-p, -(rA X p), p, (rB X p)]
** V = [vA, wA, vB, wB]
**
**
** After adding a bias term:
**
** C' : JV + b = 0
**
**
** Expanding using "V = v + M^-1 * P" (semi-implicit Euler)
** and "P = lambda * J^T", then by solving for lambda:
**
** lambda = -(Jv + b)/JM^-1J^T
*/

/*
** "Magic" formulas for ODE's soft constraints:
**
** C : x - L = 0
** C' : JV + b = 0
** JV + beta/h * C(p) + gamma * lambda = 0
**
** gamma = 1/(hk + c)
** beta = hk/(hk + c)
**
**
** Note that h is the timestep, k is the spring's
** stiffness and c is the spring's damping coefficient.
*/


// Forward-declare any helper functions!
static void updateConstraintData(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateEffectiveMass(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateBias(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);


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
void physJointDistanceInit(physicsJointDistance *joint, const float frequency, const float dampingRatio){
	// w = 2pi * f
	joint->angularFrequency = 2.f * M_PI * frequency;
	// d = 2w * zeta
	joint->damping = 2.f * joint->angularFrequency * dampingRatio;
}


/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
** Joints are always active so we always warm start.
*/
void physJointDistanceWarmStart(physicsJointDistance *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt){
	vec3 impulse;

	#warning "Do we really need to do a division here? Surely we've stored 1/dt somewhere..."
	joint->impulse /= dt;
	vec3MultiplySOut(&joint->rAB, joint->impulse, &impulse);

	// Apply the accumulated impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &joint->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &joint->rB, &impulse);
}

/*
** Prepare the constraint for velocity solving. We need to
** update the joint's members to reflect the movement of
** the bodies that it is affecting.
*/
void physJointDistancePresolve(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt){
	updateConstraintData((physicsJointDistance *)joint, bodyA, bodyB);
	calculateEffectiveMass((physicsJointDistance *)joint, bodyA, bodyB);
	calculateBias((physicsJointDistance *)joint, bodyA, bodyB, dt);
	physJointDistanceWarmStart((physicsJointDistance *)joint, bodyA, bodyB, dt);
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointDistanceSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	vec3 impulse;
	vec3 contactVelocity;

	// vcA = vA + wA X rA
	// vcB = vB + wB X rB
	// vcR = vcB - vcA

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointDistance *)joint)->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointDistance *)joint)->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	// lambda = -(JV + b)/JM^-1J^T
	//        = -((vcR . p) + b) * m_eff
	lambda = -(vec3DotVec3(&contactVelocity, &((physicsJointDistance *)joint)->rAB) + ((physicsJointDistance *)joint)->bias +
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
float physJointDistanceSolvePosition(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	// If we're not using soft constraints, we can perform positional correction.
	if(((physicsJointDistance *)joint)->angularFrequency <= 0.f){
		vec3 rA;
		vec3 rB;
		vec3 rAB;
		float distance;
		float constraint;

		// Transform the anchor points using the bodies' new scales and rotations.
		vec3MultiplyVec3Out(&bodyA->transform.scale, &((physicsJointDistance *)joint)->anchorA, &rA);
		quatRotateVec3Fast(&bodyA->transform.rot, &rA, &rA);
		vec3MultiplyVec3Out(&bodyB->transform.scale, &((physicsJointDistance *)joint)->anchorB, &rB);
		quatRotateVec3Fast(&bodyB->transform.rot, &rB, &rB);

		// Find the relative position of the two bodies.
		// p = (pB - pA)
		//   = (cB + rB - cA - rA)
		rAB = bodyB->centroidGlobal;
		vec3AddVec3(&rAB, &rB);
		vec3SubtractVec3From(&rAB, &bodyA->centroidGlobal);
		vec3SubtractVec3From(&rAB, &rA);

		distance = vec3MagnitudeVec3(&rAB);
		constraint = distance - ((physicsJointDistance *)joint)->distance;
		// Clamp the constraint value.
		if(constraint < -PHYSCONSTRAINT_MAX_LINEAR_CORRECTION){
			constraint = -PHYSCONSTRAINT_MAX_LINEAR_CORRECTION;
		}else if(constraint > PHYSCONSTRAINT_MAX_LINEAR_CORRECTION){
			constraint = PHYSCONSTRAINT_MAX_LINEAR_CORRECTION;
		}

		#warning "Should we calculate the effective mass again? I remember reading something about this..."
		vec3MultiplyS(&rAB, -constraint * ((physicsJointDistance *)joint)->effectiveMass);

		// Apply the correctional impulse.
		physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &rAB);
		physRigidBodyApplyImpulsePosition(bodyB, &rB, &rAB);


		return(constraint);
	}


	return(0.f);
}
#endif


/*
** Updates the global positions of the anchor points
** and some of the other variables that depend on them.
*/
static void updateConstraintData(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	float distance;

	// Transform the anchor points using the bodies' new scales and rotations.
	vec3MultiplyVec3Out(&bodyA->transform.scale, &joint->anchorA, &joint->rA);
	quatRotateVec3Fast(&bodyA->transform.rot, &joint->rA, &joint->rA);
	vec3MultiplyVec3Out(&bodyB->transform.scale, &joint->anchorB, &joint->rB);
	quatRotateVec3Fast(&bodyB->transform.rot, &joint->rB, &joint->rB);

	// Find the relative position of the two bodies.
	// p = (pB - pA)
	//   = (cB + rB - cA - rA)
	joint->rAB = bodyB->centroidGlobal;
	vec3AddVec3(&joint->rAB, &joint->rB);
	vec3SubtractVec3From(&joint->rAB, &bodyA->centroidGlobal);
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
	// Calculate C(p) for our bias term.
	// beta = hk/(hk + c)
	// bias = beta/h * C(p)
	joint->bias = distance - joint->distance;
}

/*
** Calculate the effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once
** per update. Note that we don't take its inverse yet.
*/
static void calculateEffectiveMass(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	vec3 rpA;
	vec3 rpIA;
	vec3 rpB;
	vec3 rpIB;

	// JA = (IA^-1 * (rA X p)) . (rA X p)
	// JB = (IB^-1 * (rB X p)) . (rB X p)
	//
	// m_eff = 1/JM^-1J^T
	//       = 1/(mA^-1 + mB^-1 + JA + JB)

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
static void calculateBias(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	// Only use soft constraints if the frequency is greater than 0.
	if(joint->angularFrequency > 0.f){
		const float tempMass = 1.f / joint->effectiveMass;
		// k = m_eff * w^2
		const float stiffness = tempMass * joint->angularFrequency * joint->angularFrequency;

		// c = m_eff * d
		// gamma = 1/(hk + c)
		joint->gamma = 1.f / (dt * (dt * stiffness + tempMass * joint->damping));
		// beta = hk/(hk + c)
		// bias = beta/h * C(p)
		joint->bias *= dt * stiffness * joint->gamma;

		joint->effectiveMass += joint->gamma;
	}else{
		joint->gamma = 0.f;
		joint->bias = 0.f;
	}

	joint->effectiveMass = 1.f / joint->effectiveMass;
}