#include "physicsJointFriction.h"


#include <math.h>

#include "physicsJoint.h"
#include "physicsRigidBody.h"


/*
** With friction constraints, we start with three velocity
** constraints derived similarly to the contact constraint.
** We have two linear ones, one for each tangent, and an
** angular one that uses the normal direction.
**
** C1' : |((vB + wB X rB) - (vA + wA X rA)) . u1| <= maxFriction
** C2' : |((vB + wB X rB) - (vA + wA X rA)) . u2| <= maxFriction
** C3' : |(wB - wA) . n| <= maxFriction
**
** In these equations, "maxFriction" is the magnitude of the
** greatest force friction can apply. For contacts, it is the
** sum of the accumulated impulses for each contact point
** multiplied by the coefficient of friction.
**
**
** We'll begin by deriving the effective mass for the linear
** component. Note that our Jacobian is a 2x4 matrix rather
** than the usual 1x4, as we have two linear constraints.
**
**     [-u1, -(rA X u1), u1, (rB X u1)]
** J = [-u2, -(rA X u2), u2, (rB X u2)]
**
**        [mA^-1     0     0     0]
**        [    0 IA^-1     0     0]
** M^-1 = [    0     0 mB^-1     0]
**        [    0     0     0 IB^-1]
**
**       [       -u1,        -u2]
**       [-(rA X u1), -(rA X u2)]
** J^T = [        u1,         u2]
**       [ (rB X u1),  (rB X u2)]
**
**
** Calculating JM^-1J^T gives us a 2x2 matrix, which we can
** invert to get our effective mass:
**
**         [-u1 * mA^-1, -(rA X u1) * IA^-1, u1 * mB^-1, (rB X u1) * IB^-1]
** JM^-1 = [-u2 * mA^-1, -(rA X u2) * IA^-1, u2 * mB^-1, (rB X u2) * IB^-1]
**
** K = JM^-1J^T
** [K]_00 = mA^-1 + mB^-1 + (((rA X u1) * IA^-1) . (rA X u1)) + (((rB X u1) * IB^-1) . (rB X u1))
** [K]_01 =                 (((rA X u1) * IA^-1) . (rA X u2)) + (((rB X u1) * IB^-1) . (rB X u2))
** [K]_10 =                 (((rA X u1) * IA^-1) . (rA X u2)) + (((rB X u1) * IB^-1) . (rB X u2))
** [K]_11 = mA^-1 + mB^-1 + (((rA X u2) * IA^-1) . (rA X u2)) + (((rB X u2) * IB^-1) . (rB X u2))
**
**
** It is also worth noting that in this case, "JV" is not
** a scalar either. It is the following 2x1 matrix:
**
**      [((vB + wB X rB) - (vA + wA X rA)) . u1]
** JV = [((vB + wB X rB) - (vA + wA X rA)) . u2]
**
** Multiplying the inverse of our effective mass by this matrix
** gives us a 2D vector, where each row contains the magnitude
** of the impulse to be applied in a tangent direction. From
** here, we just multiply the magnitudes by the directions
** and add them up to get the total impulse to apply.
**
**
** For the angular constraint, we get the following Jacobian:
**
** J = [0, -n, 0, n]
**
** Using this, we get that the effective mass is the inverse of:
**
** JM^-1J^T = ((n * IA^-1) . n) + ((n * IB^-1) . n)
*/


#warning "We never set joint->linearImpulse."
/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
*/
void physJointFrictionWarmStart(const physicsJointFriction *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	/*
	b3Vec3 P1 = vcm->tangentImpulse.x * vcm->tangent1;
	b3Vec3 P2 = vcm->tangentImpulse.y * vcm->tangent2;
	b3Vec3 P3 = vcm->motorImpulse * vcm->normal;

	vA -= mA * (P1 + P2);
	wA -= iA * (b3Cross(vcm->rA, P1 + P2) + P3);

	vB += mB * (P1 + P2);
	wB += iB * (b3Cross(vcm->rB, P1 + P2) + P3);
	*/

	vec3 linearImpulse;
	vec3 angularImpulse;
	vec3 impulse;

	// Calculate the linear impulse.
	vec3MultiplySOut(&joint->tangents[0], joint->linearImpulse.x, &linearImpulse);
	vec3MultiplySOut(&joint->tangents[1], joint->linearImpulse.y, &impulse);
	vec3AddVec3(&linearImpulse, &impulse);

	// Calculate the angular impulse.
	vec3MultiplySOut(&joint->normal, joint->angularImpulse, &angularImpulse);


	// Apply the accumulated impulses.
	physRigidBodyApplyImpulseAngularBias(bodyA, &joint->rA, &linearImpulse, &angularImpulse);
	physRigidBodyApplyImpulseAngularBiasInverse(bodyB, &joint->rB, &linearImpulse, &angularImpulse);
}

/*
** Calculate the effective mass of the constraint, which won't
** change between velocity iterations. We can just do it once
** per update.
*/
void physJointFrictionCalculateEffectiveMass(physicsJointFriction *joint, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *invInertiaB = &bodyB->invInertiaGlobal;

	vec3 rAu1;
	vec3 rAu2;
	vec3 rBu1;
	vec3 rBu2;

	vec3 rAu1IA;
	vec3 rAu2IA;
	vec3 rBu1IB;
	vec3 rBu2IB;


	// K = JM^-1J^T
	// [K]_00 = mA^-1 + mB^-1 + (((rA X u1) * IA^-1) . (rA X u1)) + (((rB X u1) * IB^-1) . (rB X u1))
	// [K]_01 =                 (((rA X u1) * IA^-1) . (rA X u2)) + (((rB X u1) * IB^-1) . (rB X u2))
	// [K]_10 = [K]_01
	// [K]_11 = mA^-1 + mB^-1 + (((rA X u2) * IA^-1) . (rA X u2)) + (((rB X u2) * IB^-1) . (rB X u2))
	vec3CrossVec3Out(&joint->rA, &joint->tangents[0], &rAu1);
	vec3CrossVec3Out(&joint->rA, &joint->tangents[1], &rAu2);
	vec3CrossVec3Out(&joint->rB, &joint->tangents[0], &rBu1);
	vec3CrossVec3Out(&joint->rB, &joint->tangents[1], &rBu2);

	mat3MultiplyByVec3Out(invInertiaA, &rAu1, &rAu1IA);
	mat3MultiplyByVec3Out(invInertiaA, &rAu2, &rAu2IA);
	mat3MultiplyByVec3Out(invInertiaB, &rBu1, &rBu1IB);
	mat3MultiplyByVec3Out(invInertiaB, &rBu2, &rBu2IB);

	// Calculate the linear effective mass.
	joint->linearMass.m[0][0] = invMass + vec3DotVec3(&rAu1IA, &rAu1) + vec3DotVec3(&rBu1IB, &rBu1);
	joint->linearMass.m[0][1] =
	joint->linearMass.m[1][0] = vec3DotVec3(&rAu1IA, &rAu2) + vec3DotVec3(&rBu1IB, &rBu2);
	joint->linearMass.m[1][1] = invMass + vec3DotVec3(&rAu2IA, &rAu2) + vec3DotVec3(&rBu2IB, &rBu2);
	// m_eff = 1/K
	mat2Invert(&joint->linearMass);


	// JM^-1J^T = ((IA^-1 * n) . n) + ((IB^-1 * n) . n)
	mat3MultiplyByVec3Out(invInertiaA, &joint->normal, &rAu1IA);
	mat3MultiplyByVec3Out(invInertiaB, &joint->normal, &rBu1IB);

	// Calculate the angular effective mass.
	joint->angularMass = 1.f / (
		vec3DotVec3(&rAu1IA, &joint->normal) +
		vec3DotVec3(&rBu1IB, &joint->normal)
	);
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointFrictionSolveVelocity(physicsJointFriction *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float maxFriction){
	vec2 lambda;
	vec2 oldImpulse;
	vec3 impulse;
	vec3 relativeVelocity;
	float impulseMagnitude;

	// vA_anchor  = vA + wA X rA
	// vB_anchor  = vB + wB X rB
	// v_relative = vB_anchor - vA_anchor

	// Calculate the total linear velocity of the anchor point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &joint->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the anchor point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &joint->rB, &relativeVelocity);
	vec3AddVec3(&relativeVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&relativeVelocity, &impulse);


	// lambda    = -JV/JM^-1J^T
	// lambda[0] = -(v_relative . u1) * m_eff
	// lambda[1] = -(v_relative . u2) * m_eff
	// Calculate the magnitudes for the linear impulses.
	lambda.x = -vec3DotVec3(&relativeVelocity, &joint->tangents[0]);
	lambda.y = -vec3DotVec3(&relativeVelocity, &joint->tangents[1]);
	mat2MultiplyByVec2(&joint->linearMass, &lambda);

	oldImpulse = joint->linearImpulse;
	vec2AddVec2(&joint->linearImpulse, &lambda);

	// C' <= maxFriction
	// If our accumulated impulse magnitude is larger than the
	// total force friction is allowed to apply, we need to clamp it.
	impulseMagnitude = vec2DotVec2(&joint->linearImpulse, &joint->linearImpulse);
	if(impulseMagnitude > maxFriction * maxFriction){
		// Normalize the vector and multiply by maxFriction at the same time.
		vec2MultiplyS(&joint->linearImpulse, maxFriction / impulseMagnitude);
		vec2SubtractVec2FromOut(&joint->linearImpulse, &oldImpulse, &lambda);
	}

	// Add the impulse magnitudes for both tangent directions.
	vec3MultiplySOut(&joint->tangents[0], lambda.x, &relativeVelocity);
	vec3MultiplySOut(&joint->tangents[1], lambda.y, &impulse);
	vec3AddVec3(&impulse, &relativeVelocity);

	// Apply the correctional linear impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &joint->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &joint->rB, &impulse);


	// lambda = -JV/JM^-1J^T
	//        = -((wB - wA) . n) * m_eff
	// Calculate the magnitude for the angular impulse.
	vec3SubtractVec3FromOut(&bodyB->angularVelocity, &bodyA->angularVelocity, &relativeVelocity);
	lambda.x = -vec3DotVec3(&relativeVelocity, &joint->normal) * joint->angularMass;
	lambda.y = joint->angularImpulse;
	impulseMagnitude = joint->angularImpulse + lambda.x;

	// Clamp our accumulated impulse for the angular constraint.
	if(impulseMagnitude <= -maxFriction){
		joint->angularImpulse = -maxFriction;
		vec3MultiplySOut(&joint->normal, -maxFriction - lambda.y, &impulse);
	}else if(impulseMagnitude > maxFriction){
		joint->angularImpulse = maxFriction;
		vec3MultiplySOut(&joint->normal, maxFriction - lambda.y, &impulse);
	}else{
		joint->angularImpulse = impulseMagnitude;
		vec3MultiplySOut(&joint->normal, lambda.x, &impulse);
	}

	physRigidBodyApplyAngularImpulseInverse(bodyA, &joint->rA, impulse);
	physRigidBodyApplyAngularImpulse(bodyB, &joint->rB, impulse);
}