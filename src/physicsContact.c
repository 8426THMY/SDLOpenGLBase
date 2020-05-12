#include "physicsContact.h"


#include <string.h>

#include "math.h"
#include "mat3.h"
#include "utilMath.h"

#include "physicsRigidBody.h"
#include "physicsCollider.h"


#define combineFriction(mu1, mu2)    (sqrtf((mu1) * (mu2)))
#define combineRestitution(e1, e2) (((e1) >= (e2)) ? (e1) : (e2))


/*
** ----------------------------------------------------------------------
**
** Contact constraints ensure that the separation between two
** bodies is always greater than or equal to zero, that is,
** they aren't penetrating. We also solve a friction constraint
** here, check "physicsJointFriction.c" for more information.
**
** ----------------------------------------------------------------------
**
** Contact constraint inequality:
**
** C : (pB - pA) . n >= 0,
**
** where pA and pB are the global positions of the
** contact points and n is the contact normal.
**
**
** Differentiate with respect to time to get a velocity constraint:
**
** C' : (((wB X rB) + vB) - ((wA X rA) + vA)) . n >= 0.
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
** J = [-n, -(rA X n), n, (rB X n)].
**
**
** Finally, adding a potential bias term, we have
**
** b = b_penetration + b_restitution
**   = B/dt * C(x) + e * C'(x),
**
** C' : JV + b >= 0,
**
** where B is the Baumgarte constant and e is the contact restitution.
**
** ----------------------------------------------------------------------
**
** Semi-implicit Euler:
**
** V   = V_i + dt * M^-1 * F_ext,
** V_f = V   + dt * M^-1 * F_C.
**
** Where V_i is the initial velocity vector, V_f is the final
** velocity vector, F_ext is the external force on the body
** F_C is the constraint force and dt is the timestep.
**
**
** Using F_C = J^T * lambda and lambda' = dt * lambda, we can
** solve for the impulse magnitude (constraint Lagrange
** multiplier) lambda':
**
** JV_f + b = 0
** J(V + dt * M^-1 * F_C) + b = 0
** JV + dt * (JM^-1)F_C + b = 0
** JV + dt * (JM^-1)J^T . lambda + b = 0
** dt * (JM^-1)J^T . lambda = -(JV + b)
** dt * lambda = -(JV + b)/((JM^-1)J^T)
** lambda' = -(JV + b)/((JM^-1)J^T).
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
**       [    -n   ]
**       [-(rA X n)]
** J^T = [     n   ]
**       [ (rB X n)].
**
**
** Evaluating this expression gives us:
**
** (JM^-1)J^T = mA^-1 + mB^-1 + ((rA X n) . (IA^-1 * (rA X n))) + ((rB X n) . (IB^-1 * (rB X n))).
**
** ----------------------------------------------------------------------
*/


// Forward-declare any helper functions!
static void warmStartContactPoint(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
static void calculateEffectiveMass(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void calculateBias(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float dt
);
#else
static void calculateBias(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
#endif

#ifndef PHYSCONTACT_USE_FRICTION_JOINT
static void solveTangents(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif
static void solveNormal(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);

#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
float solvePosition(
	const physicsManifold *const restrict pm, const physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif


// Build a physics manifold by expanding a contact manifold.
void physManifoldInit(
	physicsManifold *const restrict pm, const contactManifold *const restrict cm,
	const physicsCollider *const restrict cA, const physicsCollider *const restrict cB
){

	physicsContactPoint *pmContact = pm->contacts;
	const contactPoint *cmContact = cm->contacts;
	const contactPoint *const lastContact = &cmContact[cm->numContacts];

	const vec3 *const bodyACentroid = &cA->owner->centroidGlobal;
	const vec3 *const bodyBCentroid = &cB->owner->centroidGlobal;
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	const quat *const bodyARot      = &cA->owner->transform.rot;
	const quat *const bodyBRot      = &cB->owner->transform.rot;
	#endif

	vec3 normal;
	vec3 halfway;
	vec3InitZero(&normal);
	vec3InitZero(&halfway);

	// Create a physics contact point for each regular one!
	do {
		vec3 curHalfway;
		vec3AddVec3Out(&cmContact->pA, &cmContact->pB, &curHalfway);
		vec3MultiplyS(&curHalfway, 0.5f);
		// We use the point halfway between the contact points when resolving collision.
		vec3SubtractVec3FromOut(&curHalfway, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3FromOut(&curHalfway, bodyBCentroid, &pmContact->rB);

		#ifdef PHYSCONTACT_USE_FRICTION_JOINT
		vec3AddVec3(&halfway, &curHalfway);
		#endif

		// When we're using non-linear Guass-Seidel, we
		// need the untransformed contact points and normal.
		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		vec3SubtractVec3FromOut(&cmContact->pA, bodyACentroid, &curHalfway);
		quatRotateVec3InverseFast(bodyARot, &curHalfway, &pmContact->rAlocal);
		vec3SubtractVec3FromOut(&cmContact->pB, bodyBCentroid, &curHalfway);
		quatRotateVec3InverseFast(bodyBRot, &curHalfway, &pmContact->rBlocal);

		quatRotateVec3InverseFast(bodyARot, &cmContact->normal, &pmContact->normalLocal);
		#endif

		// Find the average contact normal.
		vec3AddVec3(&normal, &cmContact->normal);

		pmContact->separation = cmContact->separation;
		pmContact->key = cmContact->key;

		pmContact->normalImpulse = 0.f;
		#ifndef PHYSCONTACT_USE_FRICTION_JOINT
		pmContact->tangentImpulse[0] = 0.f;
		pmContact->tangentImpulse[1] = 0.f;
		#endif

		++pmContact;
		++cmContact;
	} while(cmContact < lastContact);

	pm->numContacts = cm->numContacts;


	vec3NormalizeVec3Fast(&normal);
	// Set the tangent vectors such that they form an
	// orthonormal basis together with the contact normal.
	normalBasisFast(&normal, &physContactTangent(pm, 0), &physContactTangent(pm, 1));
	physContactNormal(pm) = normal;

	// If we're using a friction join, we initialize it here.
	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	// Get the average point halfway between contact points.
	vec3DivideByS(&halfway, cm->numContacts);
	vec3SubtractVec3FromOut(&halfway, bodyACentroid, &pm->frictionJoint.rA);
	vec3SubtractVec3FromOut(&halfway, bodyBCentroid, &pm->frictionJoint.rB);

	vec2InitZero(&pm->frictionJoint.linearImpulse);
	pm->frictionJoint.angularImpulse = 0.f;
	#endif

	pm->friction = combineFriction(cA->friction, cB->friction);
	pm->restitution = combineRestitution(cA->restitution, cB->restitution);
}

// Update any contact points that have persisted.
void physManifoldPersist(
	physicsManifold *const restrict pm, const contactManifold *const restrict cm,
	const physicsCollider *const restrict cA, const physicsCollider *const restrict cB
){

	const contactPoint *cmContact = cm->contacts;
	const contactPoint *const lastContact = &cmContact[cm->numContacts];
	physicsContactPoint *pmContact = pm->contacts;
	physicsContactPoint *pmSwap = pmContact;
	const physicsContactPoint *const lastPhysContact = &pmContact[pm->numContacts];

	const vec3 *const bodyACentroid = &cA->owner->centroidGlobal;
	const vec3 *const bodyBCentroid = &cB->owner->centroidGlobal;
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	const quat *const bodyARot      = &cA->owner->transform.rot;
	const quat *const bodyBRot      = &cB->owner->transform.rot;
	#endif

	vec3 normal;
	vec3 halfway;

	unsigned int persistentFlags[CONTACT_MAX_POINTS];
	unsigned int *isPersistent = persistentFlags;

	vec3InitZero(&normal);
	vec3InitZero(&halfway);
	// Initialise our flags to 0. A value of 0
	// means that a contact is not persistent.
	memset(persistentFlags, 0, sizeof(persistentFlags));

	do {
		do {
			// We've found a matching key pair, so the point is persistent!
			if(memcmp(&pmContact->key, &cmContact->key, sizeof(pmContact->key)) == 0){
				// We want contacts in the old physics manifold to be moved
				// to their new positions in the current manifold. This will
				// make it easier to copy the keys for non-persistent points.
				if(pmContact != pmSwap){
					const contactKey tempKey = pmSwap->key;
					float tempImpulse;

					// Swap the keys.
					pmSwap->key = pmContact->key;
					pmContact->key = tempKey;

					// Swap the accumulators.
                    tempImpulse = pmSwap->normalImpulse;
                    pmSwap->normalImpulse = pmContact->normalImpulse;
                    pmContact->normalImpulse = tempImpulse;

					#ifndef PHYSCONTACT_USE_FRICTION_JOINT
                    tempImpulse = pmSwap->tangentImpulse[0];
                    pmSwap->tangentImpulse[0] = pmContact->tangentImpulse[0];
                    pmContact->tangentImpulse[0] = tempImpulse;

                    tempImpulse = pmSwap->tangentImpulse[1];
                    pmSwap->tangentImpulse[1] = pmContact->tangentImpulse[1];
                    pmContact->tangentImpulse[1] = tempImpulse;
                    #endif
				}

				*isPersistent = 1;
				break;
			}

			++pmContact;
			++isPersistent;
		} while(pmContact < lastPhysContact);

		// Find the average contact normal.
		vec3AddVec3(&normal, &cmContact->normal);

		++cmContact;
		++pmSwap;
	} while(cmContact < lastContact);


	vec3NormalizeVec3Fast(&normal);
	// Set the tangent vectors such that they form an
	// orthonormal basis together with the contact normal.
	normalBasisFast(&normal, &physContactTangent(pm, 0), &physContactTangent(pm, 1));
	physContactNormal(pm) = normal;

	pm->friction = combineFriction(cA->friction, cB->friction);
	pm->restitution = combineRestitution(cA->restitution, cB->restitution);


	cmContact = cm->contacts;
	pmContact = pm->contacts;
	isPersistent = persistentFlags;
	// Initialise any non-persistent contacts and
	// warm start contacts that are persistent!
	do {
		vec3 curHalfway;
		vec3AddVec3Out(&cmContact->pA, &cmContact->pB, &curHalfway);
		vec3MultiplyS(&curHalfway, 0.5f);
		// We use the point halfway between the contact points when resolving collision.
		vec3SubtractVec3FromOut(&curHalfway, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3FromOut(&curHalfway, bodyBCentroid, &pmContact->rB);

		#ifdef PHYSCONTACT_USE_FRICTION_JOINT
		vec3AddVec3(&halfway, &curHalfway);
		#endif

		// When we're using non-linear Guass-Seidel, we
		// need the untransformed contact points and normal.
		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		vec3SubtractVec3FromOut(&cmContact->pA, bodyACentroid, &curHalfway);
		quatRotateVec3InverseFast(bodyARot, &curHalfway, &pmContact->rAlocal);
		vec3SubtractVec3FromOut(&cmContact->pB, bodyBCentroid, &curHalfway);
		quatRotateVec3InverseFast(bodyBRot, &curHalfway, &pmContact->rBlocal);

		quatRotateVec3InverseFast(bodyARot, &cmContact->normal, &pmContact->normalLocal);
		#endif

		pmContact->separation = cmContact->separation;

		// Contact point is not persistent, so we can reset its accumulators.
		if(!(*isPersistent)){
			pmContact->key = cmContact->key;
			pmContact->normalImpulse = 0.f;
			#ifndef PHYSCONTACT_USE_FRICTION_JOINT
			pmContact->tangentImpulse[0] = 0.f;
			pmContact->tangentImpulse[1] = 0.f;
			#endif

		// If it is, we can warm start it!
		}else{
			warmStartContactPoint(pm, pmContact, cA->owner, cB->owner);
		}

		++cmContact;
		++pmContact;
		++isPersistent;
	} while(cmContact < lastContact);

	pm->numContacts = cm->numContacts;


	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	// Get the average point halfway between contact points.
	vec3DivideByS(&halfway, cm->numContacts);
	vec3SubtractVec3FromOut(&halfway, bodyACentroid, &pm->frictionJoint.rA);
	vec3SubtractVec3FromOut(&halfway, bodyBCentroid, &pm->frictionJoint.rB);

	physJointFrictionWarmStart(&pm->frictionJoint, cA->owner, cB->owner);
	#endif
}


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physManifoldPresolve(
	physicsManifold *const restrict pm, const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB, const float dt
){
#else
void physManifoldPresolve(
	physicsManifold *const restrict pm, const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){
#endif

	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *const lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		calculateEffectiveMass(pm, curContact, bodyA, bodyB);
		#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		calculateBias(pm, curContact, bodyA, bodyB, dt);
		#else
		calculateBias(pm, curContact, bodyA, bodyB);
		#endif
	}
}

/*
** Apply an impulse to penetrating physics objects to ensure
** that they separate from each other on the next frame.
** This may be called multiple times with sequential impulse.
*/
void physManifoldSolveVelocity(physicsManifold *const restrict pm, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *const lastContact = &curContact[pm->numContacts];
	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	float maxForce = 0.f;
	#endif

	for(; curContact < lastContact; ++curContact){
		#ifndef PHYSCONTACT_USE_FRICTION_JOINT
		solveTangents(pm, curContact, bodyA, bodyB);
		solveNormal(pm, curContact, bodyA, bodyB);
		#else
		solveNormal(pm, curContact, bodyA, bodyB);
		// Sum up the total impulse in the normal direction for each
		// contact point. We use this when calculating the friction.
		maxForce += curContact->normalImpulse;
		#endif
	}

	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	// If we're using a joint to simulate friction, we'll need to
	// solve its constraints after solving the contact constraints.
	physJointFrictionSolveVelocity(&pm->frictionJoint, bodyA, bodyB, pm->friction * maxForce);
	#endif
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times.
*/
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
void physManifoldSolvePosition(const physicsManifold *const restrict pm, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB){
	const physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *const lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		solvePosition(pm, curContact, bodyA, bodyB);
	}
}
#endif


/*
** If a contact point has persisted across multiple frames,
** we are able to warm start it. This will allow it to more
** quickly converge to its correct state when we're solving.
*/
static void warmStartContactPoint(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 impulse;
	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	vec3 accumulator;

	// Warm start the contact point using the total
	// accumulated normal and frictional impulses.
	vec3MultiplySOut(&pm->normal, contact->normalImpulse, &impulse);
	vec3MultiplySOut(&pm->tangents[0], contact->tangentImpulse[0], &accumulator);
	vec3AddVec3(&impulse, &accumulator);
	vec3MultiplySOut(&pm->tangents[1], contact->tangentImpulse[1], &accumulator);
	vec3AddVec3(&impulse, &accumulator);
	#else
	vec3MultiplySOut(&pm->frictionJoint.normal, contact->normalImpulse, &impulse);
	#endif


	// Apply the accumulated impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}

/*
** Calculate the effective mass of the two colliding bodies, which
** won't change between velocity iterations. We can just do it once
** per update.
*/
static void calculateEffectiveMass(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	vec3 rnA;
	vec3 rnIA;
	vec3 rnB;
	vec3 rnIB;

	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *const invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *const invInertiaB = &bodyB->invInertiaGlobal;

	// (JM^-1)J^T = mA^-1 + mB^-1 + (((rA X n) * IA^-1) . (rA X n)) + (((rB X n) * IB^-1) . (rB X n))
	vec3CrossVec3Out(&contact->rA, &physContactNormal(pm), &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&contact->rB, &physContactNormal(pm), &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	// Calculate the inverse effective mass along the normal.
	contact->normalEffectiveMass = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);

	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	// (JM^-1)J^T = mA^-1 + mB^-1 + (((rA X u1) * IA^-1) . (rA X u1)) + (((rB X u1) * IB^-1) . (rB X u1))
	vec3CrossVec3Out(&contact->rA, &pm->tangents[0], &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&contact->rB, &pm->tangents[0], &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	// Calculate the inverse effective mass along the first tangent.
	contact->tangentEffectiveMass[0] = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);

	// (JM^-1)J^T = mA^-1 + mB^-1 + (((rA X u2) * IA^-1) . (rA X u2)) + (((rB X u2) * IB^-1) . (rB X u2))
	vec3CrossVec3Out(&contact->rA, &pm->tangents[1], &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&contact->rB, &pm->tangents[1], &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	// Calculate the inverse effective mass along the second tangent.
	contact->tangentEffectiveMass[1] = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);
	#endif
}

// Calculate the contact's bias term.
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void calculateBias(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float dt
){
#else
static void calculateBias(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){
#endif

	float tempBias;
	vec3 tempVelocity;
	vec3 contactVelocity;


	// Calculate the Baumgarte bias term.
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	// C(x) = (pB - pA) . n
	tempBias = contact->separation - PHYSCONSTRAINT_LINEAR_SLOP;
	// B = Baumgarte constant
	// b_penetration = B/dt * C(x)
	contact->bias = (tempBias > 0.f) ? (PHYSCONTACT_BAUMGARTE_BIAS * dt * tempBias) : 0.f;
	#endif


	// vA_contact = vA + wA X rA
	// vB_contact = vB + wB X rB
	// v_relative = vB_contact - vA_contact

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &tempVelocity);
	vec3AddVec3(&tempVelocity, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &tempVelocity);

	// After the contact, we want the following relative velocity v':
	// v' >= -e * (v_relative . n)
	// v' >= -b_restitution
	// v' + b_restitution >= 0
	// The restitution bias represents how much of the contact
	// velocity we should "give back" to the rigid bodies.

	// Calculate the restitution bias term.
	// b_restitution = e * (v_relative . n)
	tempBias = vec3DotVec3(&contactVelocity, &physContactNormal(pm));
	// Calculate the total bias.
	if(tempBias < -PHYSCONTACT_RESTITUTION_THRESHOLD){
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		// b = b_penetration + b_restitution
		//   = B/dt * C(x) + e * (v_relative . n)
		contact->bias += pm->restitution * tempBias;
	#else
		// If we're not using Baumgarte stabilisation,
		// we only have the restitution bias here.
		// b = e * (v_relative . n)
		contact->bias = pm->restitution * tempBias;
	}else{
		contact->bias = 0.f;
	#endif
	}
}


// Calculate the frictional impulse to apply.
#ifndef PHYSCONTACT_USE_FRICTION_JOINT
static void solveTangents(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	float lambda;
	const float maxFriction = pm->friction * contact->normalImpulse;
	float oldImpulse;
	float newImpulse;
	vec3 temp;
	vec3 contactVelocity;
	vec3 impulse;


	// vA_contact = vA + wA X rA
	// vB_contact = vB + wB X rB
	// v_relative = vB_contact - vA_contact

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &temp);
	vec3AddVec3(&temp, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &temp);


	// lambda = -(JV + b)/((JM^-1)J^T)
	//        = -(v_relative . n)/K
	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[0]) * contact->tangentEffectiveMass[0];
	oldImpulse = contact->tangentImpulse[0];
	newImpulse = oldImpulse + lambda;

	// -f < lambda < f
	// Clamp our accumulated impulse for the first tangent.
	if(newImpulse < -maxFriction){
		contact->tangentImpulse[0] = -maxFriction;
		vec3MultiplySOut(&pm->tangents[0], -maxFriction - oldImpulse, &temp);
	}else if(newImpulse > maxFriction){
		contact->tangentImpulse[0] = maxFriction;
		vec3MultiplySOut(&pm->tangents[0], maxFriction - oldImpulse, &temp);
	}else{
		contact->tangentImpulse[0] = newImpulse;
		vec3MultiplySOut(&pm->tangents[0], lambda, &temp);
	}


	// lambda = -(JV + b)/((JM^-1)J^T)
	//        = -(v_relative . n)/K
	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[1]) * contact->tangentEffectiveMass[1];
	oldImpulse = contact->tangentImpulse[1];
	newImpulse = oldImpulse + lambda;

	// -f < lambda < f
	// Clamp our accumulated impulse for the second tangent.
	if(newImpulse <= -maxFriction){
		contact->tangentImpulse[1] = -maxFriction;
		vec3MultiplySOut(&pm->tangents[1], -maxFriction - oldImpulse, &impulse);
	}else if(newImpulse > maxFriction){
		contact->tangentImpulse[1] = maxFriction;
		vec3MultiplySOut(&pm->tangents[1], maxFriction - oldImpulse, &impulse);
	}else{
		contact->tangentImpulse[1] = newImpulse;
		vec3MultiplySOut(&pm->tangents[1], lambda, &impulse);
	}


	// Sum the two tangential impulses.
	vec3AddVec3(&impulse, &temp);

	// Apply the frictional impulse to the rigid bodies.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}
#endif

// Calculate the correctional normal impulse to apply.
static void solveNormal(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	float lambda;
	float oldImpulse;
	float newImpulse;
	vec3 impulse;
	vec3 contactVelocity;


	// vA_contact = vA + wA X rA
	// vB_contact = vB + wB X rB
	// v_relative = vB_contact - vA_contact

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	// lambda = -(JV + b)/((JM^-1)J^T)
	//        = -((v_relative . n) + b)/K
	lambda = -(vec3DotVec3(&contactVelocity, &physContactNormal(pm)) + contact->bias) * contact->normalEffectiveMass;
	oldImpulse = contact->normalImpulse;
	newImpulse = oldImpulse + lambda;

	// C' >= 0
	// If our impulse magnitude is valid, multiply it
	// by the direction so we can apply the impulse.
	if(newImpulse >= 0.f){
		contact->normalImpulse = newImpulse;
		vec3MultiplySOut(&physContactNormal(pm), lambda, &impulse);

	// Otherwise, clamp our accumulated impulse so it doesn't become negative.
	}else{
		contact->normalImpulse = 0.f;
		vec3MultiplySOut(&physContactNormal(pm), -oldImpulse, &impulse);
	}

	// Apply the correctional impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}


/*
** Calculate the positional correction to apply using non-linear
** Gauss-Seidel. We usually call this multiple times per update,
** but by returning the amount of error we'll know when to stop.
*/
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
float solvePosition(
	const physicsManifold *const restrict pm, const physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 rA;
	vec3 rB;
	vec3 normal;
	float separation;

	vec3 rnA;
	vec3 rnIA;
	vec3 rnB;
	vec3 rnIB;
	float constraint;
	float effectiveMass;


	// Update the contact points' positions using
	// their new translations and orientations.
	quatRotateVec3Fast(&bodyA->transform.rot, &contact->rAlocal, &rA);
	vec3AddVec3(&rA, &bodyA->centroidGlobal);
	quatRotateVec3Fast(&bodyB->transform.rot, &contact->rBlocal, &rB);
	vec3AddVec3(&rB, &bodyB->centroidGlobal);
	// We also need to update the normal.
	quatRotateVec3Fast(&bodyA->transform.rot, &physContactNormal(pm), &normal);

	// With the contact points now in global space,
	// we can find the new separation between them.
	vec3SubtractVec3FromOut(&rB, &rA, &rnA);
	separation = vec3DotVec3(&rnA, &normal) - PHYSCONTACT_SEPARATION_BIAS_TOTAL;

	constraint = PHYSCONTACT_BAUMGARTE_BIAS * (separation + PHYSCONSTRAINT_LINEAR_SLOP);

	// Clamp the constraint value.
	if(constraint < 0.f){
		if(constraint < -PHYSCONTACT_MAX_LINEAR_CORRECTION){
			constraint = -PHYSCONTACT_MAX_LINEAR_CORRECTION;
		}


		// Calculate the transformed contact points'
		// offsets from their bodies' centres of mass.
		vec3SubtractVec3FromOut(&rB, &bodyA->centroidGlobal, &rA);
		vec3SubtractVec3From(&rB, &bodyB->centroidGlobal);

		// JA = (IA * (rA X n)) . (rA X n)
		// JB = (IB * (rB X n)) . (rB X n)
		//
		// K = (JM^-1)J^T
		//   = mA^-1 + mB^-1 + JA + JB

		// We use *full* non-linear Gauss-Seidel, which
		// requires us to recompute the effective mass.
		vec3CrossVec3Out(&rA, &normal, &rnA);
		mat3MultiplyByVec3Out(&bodyA->invInertiaGlobal, &rnA, &rnIA);
		vec3CrossVec3Out(&rB, &normal, &rnB);
		mat3MultiplyByVec3Out(&bodyB->invInertiaGlobal, &rnB, &rnIB);
		effectiveMass = bodyA->invMass + bodyB->invMass + vec3DotVec3(&rnA, &rnIA) + vec3DotVec3(&rnB, &rnIB);

		// Make sure we don't apply invalid impulses. This also
		// doubles as a check to ensure we don't divide by zero.
		if(effectiveMass > 0.f){
			vec3MultiplyS(&normal, -constraint / effectiveMass);

			// Apply the correctional impulse.
			physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &normal);
			physRigidBodyApplyImpulsePosition(bodyB, &rB, &normal);
		}
	}


	return(separation);
}
#endif