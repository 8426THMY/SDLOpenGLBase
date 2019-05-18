#include "physicsContact.h"


#include <string.h>

#include "math.h"
#include "mat3.h"
#include "utilMath.h"

#include "physicsRigidBody.h"
#include "physicsCollider.h"


#define combineFriction(f1, f2)    (sqrtf((f1) * (f2)))
#define combineRestitution(r1, r2) (((r1) >= (r2)) ? (r1) : (r2))


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
** Expanding using "V = v + M^-1 * P" and
** "P = lambda * J^T", then solving for lambda:
**
** lambda = -(Jv + b)/JM^-1J^T
*/


//Forward-declare any helper functions!
static void warmStartContact(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
static void calculateEffectiveMass(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateBias(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);

static void solveTangents(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
static void solveNormal(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
void solvePosition(const physicsManifold *pm, const physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
#endif


//Build a physics manifold by expanding a contact manifold.
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

	//Create a physics contact point for each regular one!
	do {
		pmContact->key = cmContact->key;

		#warning "Perform this on the point halfway between pA and pB".
		vec3SubtractVec3FromOut(&cmContact->pA, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3FromOut(&cmContact->pB, bodyBCentroid, &pmContact->rB);

		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		quatApplyInverseRotationFast(bodyARot, &pmContact->rA, &pmContact->rAlocal);
		quatApplyInverseRotationFast(bodyBRot, &pmContact->rB, &pmContact->rBlocal);
		#endif

		pmContact->separation = cmContact->separation;

		pmContact->normalImpulse = 0.f;
		pmContact->tangentImpulse[0] = 0.f;
		pmContact->tangentImpulse[1] = 0.f;

		++pmContact;
		++cmContact;
	} while(cmContact < lastContact);

	pm->numContacts = cm->numContacts;


	pm->normal = cm->normal;
	//Set the tangent vectors such that they form an
	//orthonormal basis together with the contact normal.
	normalBasis(&pm->normal, &pm->tangents[0], &pm->tangents[1]);

	pm->friction = combineFriction(cA->friction, cB->friction);
	pm->restitution = combineRestitution(cA->restitution, cB->restitution);
}

//Update any contact points that have persisted.
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

	unsigned int persistentFlags[CONTACT_MAX_POINTS];
	unsigned int *isPersistent = persistentFlags;
	//Initialise our flags to 0. A value of 0
	//means that a contact is not persistent.
	memset(persistentFlags, 0, sizeof(persistentFlags));

	do {
		do {
			//We've found a matching key pair, so the point is persistent!
			if(memcmp(&pmContact->key, &cmContact->key, sizeof(pmContact->key)) == 0){
				//We want contacts in the old physics manifold to be moved
				//to their new positions in the current manifold. This will
				//make it easier to copy the keys for non-persistent points.
				if(pmContact != pmSwap){
					const contactKey tempKey = pmSwap->key;
					float tempImpulse;

					//Swap the keys.
					pmSwap->key = pmContact->key;
					pmContact->key = tempKey;

					//Swap the accumulators.
                    tempImpulse = pmSwap->normalImpulse;
                    pmSwap->normalImpulse = pmContact->normalImpulse;
                    pmContact->normalImpulse = tempImpulse;

                    tempImpulse = pmSwap->tangentImpulse[0];
                    pmSwap->tangentImpulse[0] = pmContact->tangentImpulse[0];
                    pmContact->tangentImpulse[0] = tempImpulse;

                    tempImpulse = pmSwap->tangentImpulse[1];
                    pmSwap->tangentImpulse[1] = pmContact->tangentImpulse[1];
                    pmContact->tangentImpulse[1] = tempImpulse;
				}

				*isPersistent = 1;
				break;
			}

			++pmContact;
			++isPersistent;
		} while(pmContact < lastPhysContact);

		++cmContact;
		++pmSwap;
	} while(cmContact < lastContact);


	pm->normal = cm->normal;
	//Set the tangent vectors such that they form an
	//orthonormal basis together with the contact normal.
	normalBasis(&pm->normal, &pm->tangents[0], &pm->tangents[1]);

	pm->friction = combineFriction(cA->friction, cB->friction);
	pm->restitution = combineRestitution(cA->restitution, cB->restitution);


	cmContact = cm->contacts;
	pmContact = pm->contacts;
	isPersistent = persistentFlags;
	//Initialise any non-persistent contacts and
	//warm start contacts that are persistent!
	do {
		#warning "Perform this on the point halfway between pA and pB".
		vec3SubtractVec3FromOut(&cmContact->pA, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3FromOut(&cmContact->pB, bodyBCentroid, &pmContact->rB);

		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		quatApplyInverseRotationFast(bodyARot, &pmContact->rA, &pmContact->rAlocal);
		quatApplyInverseRotationFast(bodyBRot, &pmContact->rB, &pmContact->rBlocal);
		#endif

		pmContact->separation = cmContact->separation;

		//Contact is not persistent, so we can reset the accumulators.
		if(!(*isPersistent)){
			pmContact->key = cmContact->key;
			pmContact->normalImpulse = 0.f;
			pmContact->tangentImpulse[0] = 0.f;
			pmContact->tangentImpulse[1] = 0.f;

		//If it is, we can warm start it!
		}else{
			warmStartContact(pm, pmContact, cA->owner, cB->owner);
		}

		++cmContact;
		++pmContact;
		++isPersistent;
	} while(cmContact < lastContact);

	pm->numContacts = cm->numContacts;
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

	for(; curContact < lastContact; ++curContact){
		solveTangents(pm, curContact, bodyA, bodyB);
		solveNormal(pm, curContact, bodyA, bodyB);
	}
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times.
*/
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
void physManifoldSolvePosition(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		solvePosition(curContact, bodyA, bodyB);
	}
}
#endif


/*
** If a contact point has persisted across multiple frames,
** we are able to warm start it. This will allow it to more
** quickly converge to its correct state when we're solving.
**/
static void warmStartContact(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	vec3 impulse;
	vec3 accumulator;

	//Warm start the contact point using the total
	//accumulated normal and frictional impulses.
	vec3MultiplySOut(&pm->normal, contact->normalImpulse, &impulse);
	vec3MultiplySOut(&pm->tangents[0], contact->tangentImpulse[0], &accumulator);
	vec3AddVec3(&impulse, &accumulator);
	vec3MultiplySOut(&pm->tangents[1], contact->tangentImpulse[1], &accumulator);
	vec3AddVec3(&impulse, &accumulator);

	//Apply the accumulated impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, impulse);
}

/*
** Calculate the effective mass of the two colliding bodies, which
** won't change between solver iterations. We can just do it once.
*/
static void calculateEffectiveMass(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	vec3 rnA;
	vec3 rnIA;
	vec3 rnB;
	vec3 rnIB;

	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *invInertiaB = &bodyB->invInertiaGlobal;

	//JA = (IA * (rA X n)) . (rA X n)
	//JB = (IB * (rB X n)) . (rB X n)
	//
	//m_eff = 1/JM^-1J^T
	//      = 1/(mA^-1 + mB^-1 + JA + JB)

	//Calculate the normal effective mass.
	vec3CrossVec3Out(&contact->rA, &pm->normal, &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&contact->rB, &pm->normal, &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	contact->normalEffectiveMass = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);

	//Calculate the first tangent effective mass.
	vec3CrossVec3Out(&pm->tangents[0], &contact->rA, &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&pm->tangents[0], &contact->rB, &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	contact->tangentEffectiveMass[0] = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);

	//Calculate the second tangent effective mass.
	vec3CrossVec3Out(&pm->tangents[1], &contact->rA, &rnA);
	mat3MultiplyByVec3Out(invInertiaA, &rnA, &rnIA);
	vec3CrossVec3Out(&pm->tangents[1], &contact->rB, &rnB);
	mat3MultiplyByVec3Out(invInertiaB, &rnB, &rnIB);
	contact->tangentEffectiveMass[1] = 1.f / (
		invMass +
		vec3DotVec3(&rnA, &rnIA) +
		vec3DotVec3(&rnB, &rnIB)
	);
}

//Calculate the contact's bias term, which will help prevent jitter.
static void calculateBias(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	float tempBias;
	vec3 tempVelocity;
	vec3 contactVelocity;


	//b = -B/dt * d
	//Calculate the Baumgarte bias term.
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	tempBias = contact->separation + PHYSCONTACT_LINEAR_SLOP;
	contact->bias = (tempBias > 0.f) ? (PHYSCONTACT_BAUMGARTE_BIAS * dt * tempBias) : 0.f;
	#endif


	//vtA = vA + wA X rA
	//vtB = vB + wB X rB
	//vcR = vcB - vcA

	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &tempVelocity);
	vec3AddVec3(&tempVelocity, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &tempVelocity);

	//Calculate the restitution bias term.
	tempBias = vec3DotVec3(&contactVelocity, &pm->normal);
	if(tempBias < -PHYSCONTACT_RESTITUTION_THRESHOLD){
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		contact->bias += pm->restitution * tempBias;
	#else
		contact->bias = pm->restitution * tempBias;
	}else{
		contact->bias = 0.f;
	#endif
	}
}


//Calculate the frictional impulse to apply.
static void solveTangents(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	const float maxFriction = pm->friction * contact->normalImpulse;
	float oldImpulse;
	float newImpulse;
	vec3 impulse;
	vec3 contactVelocity;

	//vtA = wA X rA + vA
	//vtB = wB X rB + vB
	//vcR = vcB - vcA

	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	//lambda = -(JV + b)/JM^-1J^T
	//       = ((-(1 + e) * vcR) . n)/m_eff
	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[0]) * contact->tangentEffectiveMass[0];
	oldImpulse = contact->tangentImpulse[0];
	newImpulse = oldImpulse + lambda;

	//-f < lambda < f
	//Clamp our accumulated impulse for the first tangent.
	if(newImpulse < -maxFriction){
		contact->tangentImpulse[0] = -maxFriction;
		vec3MultiplySOut(&pm->tangents[0], -maxFriction - oldImpulse, &impulse);
	}else if(newImpulse > maxFriction){
		contact->tangentImpulse[0] = maxFriction;
		vec3MultiplySOut(&pm->tangents[0], maxFriction - oldImpulse, &impulse);
	}else{
		contact->tangentImpulse[0] = newImpulse;
		vec3MultiplySOut(&pm->tangents[0], lambda, &impulse);
	}

	//Apply the correctional impulse for the first tangent.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, impulse);


	//lambda = -(JV + b)/JM^-1J^T
	//       = ((-(1 + e) * vcR) . n)/m_eff
	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[1]) * contact->tangentEffectiveMass[1];
	oldImpulse = contact->tangentImpulse[1];
	newImpulse = oldImpulse + lambda;

	//-f < lambda < f
	//Clamp our accumulated impulse for the second tangent.
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

	//Apply the correctional impulse for the second tangent.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, impulse);
}

//Calculate the correctional normal impulse to apply.
static void solveNormal(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	float oldImpulse;
	float newImpulse;
	vec3 impulse;
	vec3 contactVelocity;

	//vcA = vA + wA X rA
	//vcB = vB + wB X rB
	//vcR = vcB - vcA

	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	//lambda = -(JV + b)/JM^-1J^T
	//       = ((-(1 + e) * vcR) . n)/m_eff
	lambda = -(vec3DotVec3(&contactVelocity, &pm->normal) + contact->bias) * contact->normalEffectiveMass;
	oldImpulse = contact->normalImpulse;
	newImpulse = oldImpulse + lambda;

	//C' >= 0
	//If our impulse magnitude is valid, multiply it
	//by the direction so we can apply the impulse.
	if(newImpulse >= 0.f){
		contact->normalImpulse = newImpulse;
		vec3MultiplySOut(&pm->normal, lambda, &impulse);

	//Clamp our accumulated impulse so it doesn't become negative.
	}else{
		contact->normalImpulse = 0.f;
		vec3MultiplySOut(&pm->normal, -oldImpulse, &impulse);
	}

	//Apply the correctional impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, impulse);
}


/**
Solve velocity constraints.
Integrate positions.
Solve positions constraints {
	Update rA and rB using new positions.
	Calculate new separation and Baumgarte bias term.
	Compute effective mass and impulse magnitude.
	Apply impulse directly to position.
}
**/
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
//Calculate the positional correction to apply.
void solvePosition(const physicsManifold *pm, const physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	vec3 rA;
	vec3 rB;
	vec3 normal;
	float separation;

	vec3 rnA;
	vec3 rnIA;
	vec3 rnB;
	vec3 rnIB;
	float effectiveMass;


	//Update the normal and contact points
	//using the rigid bodys' new positions.
	/** We need the untransformed normal. **/
	quatApplyRotationFast(&bodyA->transform.rot, &pm->normal, &normal);
	/** Why do they use "point - cA" and "point - cB"? **/
	/** Also, "contact->rA" and "contact->rB" are transformed, but we need them to be untransformed here. **/
	transformStateTransformVec3(&bodyA->transform, &contact->rAlocal, &rA);
	transformStateTransformVec3(&bodyB->transform, &contact->rBlocal, &rB);

	//Now find the new separation between the points.
	vec3SubtractVec3FromOut(&rB, &rA, &rnA);
	separation = vec3DotVec3(&rnA, &normal);


	//JA = (IA * (rA X n)) . (rA X n)
	//JB = (IB * (rB X n)) . (rB X n)
	//
	//m_eff = 1/JM^-1J^T
	//      = 1/(mA^-1 + mB^-1 + JA + JB)

	//Calculate the effective mass.
	vec3CrossVec3Out(&rA, &normal, &rnA);
	mat3MultiplyByVec3Out(&bodyA->invInertiaGlobal, &rnA, &rnIA);
	vec3CrossVec3Out(&rB, &normal, &rnB);
	mat3MultiplyByVec3Out(&bodyB->invInertiaGlobal, &rnB, &rnIB);
	effectiveMass = bodyA->invMass + bodyB->invMass + vec3DotVec3(&rnA, &rnIA) + vec3DotVec3(&rnB, &rnIB);

	//Make sure we don't apply negative impulses.
	if(effectiveMass > 0.f){
		float bias = PHYSCONTACT_BAUMGARTE_BIAS * (separation + PHYSCONTACT_LINEAR_SLOP);
		vec3 impulse;

		//Clamp the bias term.
		if(bias < -PHYSCONTACT_MAX_LINEAR_CORRECTION){
			bias = -PHYSCONTACT_MAX_LINEAR_CORRECTION;
		}else if(bias > 0.f){
			bias = 0.f;
		}

		vec3MultiplySOut(&normal, bias / effectiveMass, &impulse);

		//Apply the correctional impulse.
		physRigidBodyApplyPositionalImpulseInverse(bodyA, &rA, impulse);
		physRigidBodyApplyPositionalImpulse(bodyB, &rB, impulse);
	}
}
#endif