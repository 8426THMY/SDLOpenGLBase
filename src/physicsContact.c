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
** Original constraint inequality:
**
** C : (pB - pA) . n >= 0
**
**
** Differentiate, as we want to make a change in velocity:
**
** C' : ((vB + wB X rB) - (vA + wA X rA)) . n >= 0
**
**
** By inspection:
**
** J = [-n, -(rA X n), n, (rB X n)]
** V = [vA, wA, vB, wB]
**
**
** After adding a bias term:
**
** C' : JV + b >= 0
**
**
** Expanding using "V = v + M^-1 * P" (semi-implicit Euler)
** and "P = lambda * J^T", then by solving for lambda:
**
** lambda = -(Jv + b)/JM^-1J^T
*/


// Forward-declare any helper functions!
static void warmStartContactPoint(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
static void calculateEffectiveMass(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateBias(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);

#ifndef PHYSCONTACT_USE_FRICTION_JOINT
static void solveTangents(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#endif
static void solveNormal(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
void solvePosition(const physicsManifold *pm, const physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#endif


// Build a physics manifold by expanding a contact manifold.
void physManifoldInit(physicsManifold *pm, const contactManifold *cm, const physicsCollider *cA, const physicsCollider *cB){
	physicsContactPoint *pmContact = pm->contacts;
	const contactPoint *cmContact = cm->contacts;
	const contactPoint *lastContact = &cmContact[cm->numContacts];

	const vec3 *bodyACentroid = &cA->owner->centroidGlobal;
	const vec3 *bodyBCentroid = &cB->owner->centroidGlobal;
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	const quat *bodyARot      = &cA->owner->transform.rot;
	const quat *bodyBRot      = &cB->owner->transform.rot;
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


	vec3NormalizeVec3(&normal);
	// Set the tangent vectors such that they form an
	// orthonormal basis together with the contact normal.
	normalBasis(&normal, &physContactTangent(pm, 0), &physContactTangent(pm, 1));
	physContactNormal(pm) = normal;

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
void physManifoldPersist(physicsManifold *pm, const contactManifold *cm, const physicsCollider *cA, const physicsCollider *cB){
	const contactPoint *cmContact = cm->contacts;
	const contactPoint *lastContact = &cmContact[cm->numContacts];
	physicsContactPoint *pmContact = pm->contacts;
	physicsContactPoint *pmSwap = pmContact;
	const physicsContactPoint *lastPhysContact = &pmContact[pm->numContacts];

	const vec3 *bodyACentroid = &cA->owner->centroidGlobal;
	const vec3 *bodyBCentroid = &cB->owner->centroidGlobal;
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	const quat *bodyARot      = &cA->owner->transform.rot;
	const quat *bodyBRot      = &cB->owner->transform.rot;
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


	vec3NormalizeVec3(&normal);
	// Set the tangent vectors such that they form an
	// orthonormal basis together with the contact normal.
	normalBasis(&normal, &physContactTangent(pm, 0), &physContactTangent(pm, 1));
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
void physManifoldPresolve(physicsManifold *pm, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		calculateEffectiveMass(pm, curContact, bodyA, bodyB);
		calculateBias(pm, curContact, bodyA, bodyB, dt);
	}
}

/*
** Apply an impulse to penetrating physics objects to ensure
** that they separate from each other on the next frame.
** This may be called multiple times with sequential impulse.
*/
void physManifoldSolveVelocity(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *lastContact = &curContact[pm->numContacts];
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
void physManifoldSolvePosition(const physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	const physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *lastContact = &curContact[pm->numContacts];

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
static void warmStartContactPoint(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
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
static void calculateEffectiveMass(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	vec3 rnA;
	vec3 rnIA;
	vec3 rnB;
	vec3 rnIB;

	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *invInertiaB = &bodyB->invInertiaGlobal;

	// JA = (IA^-1 * (rA X n)) . (rA X n)
	// JB = (IB^-1 * (rB X n)) . (rB X n)
	//
	// m_eff = 1/JM^-1J^T
	//       = 1/(mA^-1 + mB^-1 + JA + JB)

	// Calculate the effective mass along the normal.
	vec3CrossVec3Out(&contact->rA, &physContactNormal(pm), &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&contact->rB, &physContactNormal(pm), &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	contact->normalEffectiveMass = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);

	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	// Calculate the effective mass along the first tangent.
	vec3CrossVec3Out(&pm->tangents[0], &contact->rA, &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&pm->tangents[0], &contact->rB, &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	contact->tangentEffectiveMass[0] = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);

	// Calculate the effective mass along the second tangent.
	vec3CrossVec3Out(&pm->tangents[1], &contact->rA, &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&pm->tangents[1], &contact->rB, &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	contact->tangentEffectiveMass[1] = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);
	#endif
}

// Calculate the contact's bias term.
static void calculateBias(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	float tempBias;
	vec3 tempVelocity;
	vec3 contactVelocity;


	// b = -B/dt * d
	// Calculate the Baumgarte bias term.
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	tempBias = -contact->separation + PHYSCONSTRAINT_LINEAR_SLOP;
	contact->bias = (tempBias > 0.f) ? (PHYSCONTACT_BAUMGARTE_BIAS * dt * tempBias) : 0.f;
	#endif


	// vtA = vA + wA X rA
	// vtB = vB + wB X rB
	// vcR = vcB - vcA

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &tempVelocity);
	vec3AddVec3(&tempVelocity, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &tempVelocity);

	// Calculate the restitution bias term.
	tempBias = vec3DotVec3(&contactVelocity, &physContactNormal(pm));
	if(tempBias < -PHYSCONTACT_RESTITUTION_THRESHOLD){
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		contact->bias -= pm->restitution * tempBias;
	#else
		contact->bias = -pm->restitution * tempBias;
	}else{
		contact->bias = 0.f;
	#endif
	}
}


// Calculate the frictional impulse to apply.
#ifndef PHYSCONTACT_USE_FRICTION_JOINT
static void solveTangents(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	const float maxFriction = pm->friction * contact->normalImpulse;
	float oldImpulse;
	float newImpulse;
	vec3 temp;
	vec3 contactVelocity;
	vec3 impulse;

	// vtA = wA X rA + vA
	// vtB = wB X rB + vB
	// vcR = vcB - vcA

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &temp);
	vec3AddVec3(&temp, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &temp);


	// lambda = -(JV + b)/JM^-1J^T
	//        = -(vcR . n) * m_eff
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


	// lambda = -(JV + b)/JM^-1J^T
	//        = -(vcR . n) * m_eff
	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[1]) * contact->tangentEffectiveMass[1];
	oldImpulse = contact->tangentImpulse[1];
	newImpulse = oldImpulse + lambda;

	// -f < lambda < f
	// Clamp our accumulated impulse for the second tangent.
	if(newImpulse < -maxFriction){
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
static void solveNormal(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	float oldImpulse;
	float newImpulse;
	vec3 impulse;
	vec3 contactVelocity;

	// vcA = vA + wA X rA
	// vcB = vB + wB X rB
	// vcR = vcB - vcA

	// Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	// lambda = -(JV + b)/JM^-1J^T
	//        = -((vcR . n) + b) * m_eff
	lambda = -(vec3DotVec3(&contactVelocity, &physContactNormal(pm)) - contact->bias) * contact->normalEffectiveMass;
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


#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
/*
** Calculate the positional correction to apply using non-linear
** Gauss-Seidel. We usually call this multiple times per update,
** but by returning the amount of error we'll know when to stop.
*/
float solvePosition(const physicsManifold *pm, const physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
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
		// m_eff = 1/JM^-1J^T
		//       = 1/(mA^-1 + mB^-1 + JA + JB)

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