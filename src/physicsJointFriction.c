#include "physicsJointFriction.h"


#include <math.h>

#include "physicsJoint.h"
#include "physicsRigidBody.h"


/*
** With friction constraints, we start with three velocity
** constraints. We have two linear ones, one for each tangent,
** and an angular one that uses the normal direction.
**
** C1' : |((vB + wB X rB) - (vA + wA X rA)) . t1| <= maxFriction
** C2' : |((vB + wB X rB) - (vA + wA X rA)) . t2| <= maxFriction
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
**     [-t1, -(rA X t1), t1, (rB X t1)]
** J = [-t2, -(rA X t2), t2, (rB X t2)]
**
**       [       -t1,        -t2]
**       [-(rA X t1), -(rA X t2)]
** J^T = [        t1,         t2]
**       [ (rB X t1),  (rB X t2)]
**
**     [mA^-1     0     0     0]
**     [    0 IA^-1     0     0]
** M = [    0     0 mB^-1     0]
**     [    0     0     0 IB^-1]
**
**
** Calculating JM^-1J^T gives us a 2x2 matrix, which we can
** invert to get our effective mass:
**
**         [mA^-1 * -t1, IA^-1 * -(rA X t1), mB^-1 * t1, IB^-1 * (rB X t1)]
** JM^-1 = [mA^-1 * -t2, IA^-1 * -(rA X t2), mB^-1 * t2, IB^-1 * (rB X t2)]
**
** K = JM^-1J^T
** [K]_00 = mA^-1 + mB^-1 + (IA^-1 * (rA X t1) . (rA X t1)) + (IB^-1 * (rB X t1) . (rB X t1))
** [K]_01 = (IA^-1 * (rA X t1) . (rA X t2)) + (IB^-1 * (rB X t1) . (rB X t2))
** [K]_10 = [K]_01
** [K]_11 = mA^-1 + mB^-1 + (IA^-1 * (rA X t2) . (rA X t2)) + (IB^-1 * (rB X t2) . (rB X t2))
**
**            [mA^-1 + mB^-1 + (IA^-1 * (rA X t1) . (rA X t1)) + (IB^-1 * (rB X t1) . (rB X t1)),                 (IA^-1 * (rA X t1) . (rA X t2)) + (IB^-1 * (rB X t1) . (rB X t2))]
** JM^-1J^T = [                (IA^-1 * (rA X t1) . (rA X t2)) + (IB^-1 * (rB X t1) . (rB X t2)), mA^-1 + mB^-1 + (IA^-1 * (rA X t2) . (rA X t2)) + (IB^-1 * (rB X t2) . (rB X t2))]
**
**
** It is also worth noting that in this case, "JV" is not
** a scalar either. It is the following 2x1 matrix:
**
**      [((vB + wB X rB) - (vA + wA X rA)) . t1]
** JV = [((vB + wB X rB) - (vA + wA X rA)) . t2]
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
** JM^-1J^T = ((IA^-1 * n) . n) + ((IB^-1 * n) . n)
*/


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

	vec3 rt1A;
	vec3 rt2A;
	vec3 rt1B;
	vec3 rt2B;

	vec3 rt1IA;
	vec3 rt2IA;
	vec3 rt1IB;
	vec3 rt2IB;


	// Calculate the linear effective mass.
	vec3CrossVec3Out(&joint->rA, &joint->tangents[0], &rt1A);
	vec3CrossVec3Out(&joint->rA, &joint->tangents[1], &rt2A);
	vec3CrossVec3Out(&joint->rB, &joint->tangents[0], &rt1B);
	vec3CrossVec3Out(&joint->rB, &joint->tangents[1], &rt2B);

	mat3MultiplyByVec3Out(invInertiaA, &rt1A, &rt1IA);
	mat3MultiplyByVec3Out(invInertiaA, &rt2A, &rt2IA);
	mat3MultiplyByVec3Out(invInertiaB, &rt1B, &rt1IB);
	mat3MultiplyByVec3Out(invInertiaB, &rt2B, &rt2IB);

	// K = JM^-1J^T
	// [K]_00 = mA^-1 + mB^-1 + (IA^-1 * (rA X t1) . (rA X t1)) + (IB^-1 * (rB X t1) . (rB X t1))
	// [K]_01 = (IA^-1 * (rA X t1) . (rA X t2)) + (IB^-1 * (rB X t1) . (rB X t2))
	// [K]_10 = [K]_01
	// [K]_11 = mA^-1 + mB^-1 + (IA^-1 * (rA X t2) . (rA X t2)) + (IB^-1 * (rB X t2) . (rB X t2))

	joint->linearMass.m[0][0] = invMass + vec3DotVec3(&rt1IA, &rt1A) + vec3DotVec3(&rt1IB, &rt1B);
	joint->linearMass.m[0][1] =
	joint->linearMass.m[1][0] = vec3DotVec3(&rt1IA, &rt2A) + vec3DotVec3(&rt1IB, &rt2B);
	joint->linearMass.m[1][1] = invMass + vec3DotVec3(&rt2IA, &rt2A) + vec3DotVec3(&rt2IB, &rt2B);

	// m_eff = 1/K
	mat2Invert(&joint->linearMass);


	// Calculate the angular effective mass.
	mat3MultiplyByVec3Out(invInertiaA, &joint->normal, &rt1IA);
	mat3MultiplyByVec3Out(invInertiaB, &joint->normal, &rt1IB);

	// JM^-1J^T = ((IA^-1 * n) . n) + ((IB^-1 * n) . n)
	joint->angularMass = 1.f / (
		vec3DotVec3(&rt1IA, &joint->normal) +
		vec3DotVec3(&rt1IB, &joint->normal)
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
	vec3 contactVelocity;

	// vcA = vA + wA X rA
	// vcB = vB + wB X rB
	// vcR = vcB - vcA

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &joint->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &joint->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	// lambda = -JV/JM^-1J^T
	//        = -(vcR * t) * m_eff
	// Calculate the magnitudes for the linear impulses.
	lambda.x = -vec3DotVec3(&contactVelocity, &joint->tangents[0]);
	lambda.y = -vec3DotVec3(&contactVelocity, &joint->tangents[1]);
	mat2MultiplyByVec2(&joint->linearMass, &lambda);

	oldImpulse = joint->linearImpulse;
	vec2AddVec2(&joint->linearImpulse, &lambda);

	// C' <= maxFriction
	// If our accumulated impulse magnitude is larger than the
	// total force friction is allowed to apply, we need to clamp it.
	#warning "We recalculate this magnitude when we normalize. Yuck."
	if(vec2DotVec2(&joint->linearImpulse, &joint->linearImpulse) > maxFriction * maxFriction){
		vec2NormalizeVec2(&joint->linearImpulse);
		vec2MultiplyS(&joint->linearImpulse, maxFriction);
		vec2SubtractVec2FromOut(&joint->linearImpulse, &oldImpulse, &lambda);
	}

	// Add the impulse magnitudes for both tangent directions.
	vec3MultiplySOut(&joint->tangents[0], lambda.x, &contactVelocity);
	vec3MultiplySOut(&joint->tangents[1], lambda.y, &impulse);
	vec3AddVec3(&impulse, &contactVelocity);

	// Apply the correctional linear impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &joint->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &joint->rB, &impulse);


	// lambda = -JV/JM^-1J^T
	//        = -((wB - wA) . n) * m_eff
	// Calculate the magnitude for the angular impulse.
	vec3SubtractVec3FromOut(&bodyB->angularVelocity, &bodyA->angularVelocity, &contactVelocity);
	lambda.x = -vec3DotVec3(&contactVelocity, &joint->normal) * joint->angularMass;
	lambda.y = joint->angularImpulse;
	oldImpulse.x = joint->angularImpulse + lambda.x;

	// Clamp our accumulated impulse for the angular constraint.
	if(oldImpulse.x < -maxFriction){
		joint->angularImpulse = -maxFriction;
		vec3MultiplySOut(&joint->normal, -maxFriction - lambda.y, &impulse);
	}else if(oldImpulse.x > maxFriction){
		joint->angularImpulse = maxFriction;
		vec3MultiplySOut(&joint->normal, maxFriction - lambda.y, &impulse);
	}else{
		joint->angularImpulse = oldImpulse.x;
		vec3MultiplySOut(&joint->normal, lambda.x, &impulse);
	}

	physRigidBodyApplyAngularImpulseInverse(bodyA, &joint->rA, impulse);
	physRigidBodyApplyAngularImpulse(bodyB, &joint->rB, impulse);
}