#include "physicsJointFriction.h"


#include <math.h>

#include "utilMath.h"

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
** Friction constraint inequalities:
**
** C1' : |((vB + (wB X rB)) - (vA + (wA X rA))) . u1| <= maxFriction,
** C2' : |((vB + (wB X rB)) - (vA + (wA X rA))) . u2| <= maxFriction,
** C3' : |(wB - wA) . n| <= maxFriction,
**
** where n is the surface normal with tangents u1 and u2.
** Note that we begin with velocity constraints here.
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
** We will use two separate Jacobians, one for the two linear
** constraints C1' and C2', which we will call J1, and the
** other for the angular constraint C3', which we will call J2.
**
**      [-u1, -(rA X u1), u1, (rB X u1)]
** J1 = [-u2, -(rA X u2), u2, (rB X u2)],
**
** J2 = [0, -n, 0, n].
**
** ----------------------------------------------------------------------
**
** The effective mass for the constraint is given by (JM^-1)J^T,
** where M^-1 is the inverse mass matrix and J^T is the transposed
** Jacobian.
**
**        [mA^-1  0    0    0  ]
**        [  0  IA^-1  0    0  ]
** M^-1 = [  0    0  mB^-1  0  ]
**        [  0    0    0  IB^-1],
**
**        [       -u1,        -u2]
**        [-(rA X u1), -(rA X u2)]
** J1^T = [        u1,         u2]
**        [ (rB X u1),  (rB X u2)],
**
**        [ 0]
**        [-n]
** J2^T = [ 0]
**        [ n].
**
**
** Evaluating this expression gives us the
** following matrix for our linear constraints:
**
**          [-u1 * mA^-1, -(rA X u1) * IA^-1, u1 * mB^-1, (rB X u1) * IB^-1]
** J1M^-1 = [-u2 * mA^-1, -(rA X u2) * IA^-1, u2 * mB^-1, (rB X u2) * IB^-1]
**
** K = (J1M^-1)J1^T
** [K]_00 = mA^-1 + mB^-1 + (((rA X u1) * IA^-1) . (rA X u1)) + (((rB X u1) * IB^-1) . (rB X u1)),
** [K]_01 =                 (((rA X u1) * IA^-1) . (rA X u2)) + (((rB X u1) * IB^-1) . (rB X u2)),
** [K]_10 =                 (((rA X u1) * IA^-1) . (rA X u2)) + (((rB X u1) * IB^-1) . (rB X u2)),
** [K]_11 = mA^-1 + mB^-1 + (((rA X u2) * IA^-1) . (rA X u2)) + (((rB X u2) * IB^-1) . (rB X u2)).
**
** It is also worth noting that in this case, J1V is not
** a scalar either. It is the following 2x1 matrix:
**
**       [((vB + wB X rB) - (vA + wA X rA)) . u1]
** J1V = [((vB + wB X rB) - (vA + wA X rA)) . u2].
**
** Multiplying the inverse of our effective mass by this matrix
** gives us a 2D vector, where each row contains the magnitude
** of the impulse to be applied in a tangent direction. From
** here, we just multiply the magnitudes by the directions
** and add them up to get the total impulse to apply.
**
**
** For our angular constraint, we simply get:
**
** (J2M^-1)J2^T = ((n * IA^-1) . n) + ((n * IB^-1) . n).
**
** ----------------------------------------------------------------------
*/


#warning "This is incomplete."
/*
** By applying the impulse from the previous update,
** we can make the constraint converge more quickly.
*/
void physJointFrictionWarmStart(const physicsJointFriction *joint, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	vec3 linearImpulse;
	vec3 angularImpulse;

	// Calculate the linear impulse.
	vec3MultiplySOut(&joint->tangents[0], joint->linearImpulse.x, &linearImpulse);
	vec3MultiplySOut(&joint->tangents[1], joint->linearImpulse.y, &angularImpulse);
	vec3AddVec3(&linearImpulse, &angularImpulse);

	// Calculate the angular impulse.
	vec3MultiplySOut(&joint->normal, joint->angularImpulse, &angularImpulse);


	// Apply the accumulated impulses.
	physRigidBodyApplyImpulse(bodyA, &joint->rA, &linearImpulse);
	physRigidBodyApplyAngularImpulse(bodyA, angularImpulse);
	physRigidBodyApplyImpulse(bodyB, &joint->rB, &linearImpulse);
	physRigidBodyApplyAngularImpulseInverse(bodyB, angularImpulse);
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


	// K = (JM^-1)J^T
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

	// Calculate the inverse linear effective mass.
	joint->linearMass.m[0][0] = invMass + vec3DotVec3(&rAu1IA, &rAu1) + vec3DotVec3(&rBu1IB, &rBu1);
	joint->linearMass.m[0][1] =
	joint->linearMass.m[1][0] = vec3DotVec3(&rAu1IA, &rAu2) + vec3DotVec3(&rBu1IB, &rBu2);
	joint->linearMass.m[1][1] = invMass + vec3DotVec3(&rAu2IA, &rAu2) + vec3DotVec3(&rBu2IB, &rBu2);
	mat2Invert(&joint->linearMass);


	// (JM^-1)J^T = ((IA^-1 * n) . n) + ((IB^-1 * n) . n)
	mat3MultiplyByVec3Out(invInertiaA, &joint->normal, &rAu1IA);
	mat3MultiplyByVec3Out(invInertiaB, &joint->normal, &rBu1IB);

	// Calculate the inverse angular effective mass.
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


	// lambda    = -JV/((JM^-1)J^T)
	// lambda[0] = -(v_relative . u1)/K
	// lambda[1] = -(v_relative . u2)/K
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
		vec2MultiplyS(&joint->linearImpulse, maxFriction * invSqrtFast(impulseMagnitude));
		vec2SubtractVec2FromOut(&joint->linearImpulse, &oldImpulse, &lambda);
	}

	// Add the impulse magnitudes for both tangent directions.
	vec3MultiplySOut(&joint->tangents[0], lambda.x, &relativeVelocity);
	vec3MultiplySOut(&joint->tangents[1], lambda.y, &impulse);
	vec3AddVec3(&impulse, &relativeVelocity);

	// Apply the correctional linear impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &joint->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &joint->rB, &impulse);


	// lambda = -JV/((JM^-1)J^T)
	//        = -((wB - wA) . n)/K
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

	physRigidBodyApplyAngularImpulseInverse(bodyA, impulse);
	physRigidBodyApplyAngularImpulse(bodyB, impulse);
}