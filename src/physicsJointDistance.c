#include "physicsJointDistance.h"


#include <math.h>

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
** Expanding using "V = v + M^-1 * P" and
** "P = lambda * J^T", then solving for lambda:
**
** lambda = -(Jv + b)/JM^-1J^T
*/


//Forward-declare any helper functions!
static void updateConstraintData(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateEffectiveMass(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateBias(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);


/*
** Set up some of the joint's properties. Note that the frequency passed
** into this function should be given in hertz (that is, cycles per
** second) whereas the value we store is the angular frequency (radians
** per second).
**
** The damping ratio decides how quickly oscillations should cease.
** 0 = no damping
** 1 = no oscillations
*/
void physJointDistanceInit(physicsJointDistance *joint, const float frequency, const float dampingRatio){
	//w = 2 * pi * f
	joint->angularFrequency = 2.f * M_PI * frequency;
	//d = 2 * w * z
	joint->damping = 2.f * joint->angularFrequency * dampingRatio;
}


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointDistancePresolve(void *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	updateConstraintData((physicsJointDistance *)joint, bodyA, bodyB);
	calculateEffectiveMass((physicsJointDistance *)joint, bodyA, bodyB);
	calculateBias((physicsJointDistance *)joint, bodyA, bodyB, dt);
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointDistanceSolveVelocity(void *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	/*float lambda;
	float oldImpulse;
	float newImpulse;
	vec3 impulse;
	vec3 contactVelocity;

	//vcA = vA + wA X rA
	//vcB = vB + wB X rB
	//vcR = vcB - vcA

	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &((physicsJointDistance *)joint)->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &((physicsJointDistance *)joint)->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	//lambda = -(JV + b)/JM^-1J^T
	//       = ((-(1 + e) * vcR) . n)/m_eff
	lambda = -(vec3DotVec3(&contactVelocity, &pm->normal) + ((physicsJointDistance *)joint)->bias) * ((physicsJointDistance *)joint)->normalEffectiveMass;
	oldImpulse = ((physicsJointDistance *)joint)->normalImpulse;
	newImpulse = oldImpulse + lambda;

	//C' >= 0
	//If our impulse magnitude is valid, multiply it
	//by the direction so we can apply the impulse.
	if(newImpulse >= 0.f){
		((physicsJointDistance *)joint)->normalImpulse = newImpulse;
		vec3MultiplySOut(&pm->normal, lambda, &impulse);

	//Clamp our accumulated impulse so it doesn't become negative.
	}else{
		((physicsJointDistance *)joint)->normalImpulse = 0.f;
		vec3MultiplySOut(&pm->normal, -oldImpulse, &impulse);
	}

	//Apply the correctional impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &((physicsJointDistance *)joint)->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, &((physicsJointDistance *)joint)->rB, impulse);*/
}


/*
** Updates the global positions of the anchor points
** and some of the other variables that depend on them.
*/
static void updateConstraintData(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	//Transform the anchor points from local space to global space!
	vec3SubtractVec3FromOut(&joint->anchorA, &bodyA->centroidLocal, &joint->rA);
	transformStateTransformVec3(&bodyA->transform, &joint->rA, &joint->rA);
	vec3SubtractVec3FromOut(&joint->anchorB, &bodyB->centroidLocal, &joint->rB);
	transformStateTransformVec3(&bodyB->transform, &joint->rB, &joint->rB);

	//p = (pB - pA)
	vec3SubtractVec3FromOut(&joint->rA, &joint->rB, &joint->rAB);
}

/*
** Calculate the effective mass of the two colliding bodies, which
** won't change between solver iterations. We can just do it once.
*/
static void calculateEffectiveMass(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	vec3 rpA;
	vec3 rpIA;
	vec3 rpB;
	vec3 rpIB;

	//JA = (IA * (rA X p)) . (rA X p)
	//JB = (IB * (rB X p)) . (rB X p)
	//
	//m_eff = 1/JM^-1J^T
	//      = 1/(mA^-1 + mB^-1 + JA + JB)

	vec3CrossVec3Out(&joint->rA, &joint->rAB, &rpA);
	mat3MultiplyByVec3Out(&bodyA->invInertiaGlobal, &rpA, &rpIA);
	vec3CrossVec3Out(&joint->rB, &joint->rAB, &rpB);
	mat3MultiplyByVec3Out(&bodyB->invInertiaGlobal, &rpB, &rpIB);
	joint->effectiveMass = 1.f / (
		bodyA->invMass + bodyB->invMass +
		vec3DotVec3(&rpA, &rpIA) +
		vec3DotVec3(&rpB, &rpIB)
	);
}

//
static void calculateBias(physicsJointDistance *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	//
}