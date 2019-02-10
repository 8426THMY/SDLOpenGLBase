#include "physicsContact.h"


#include <string.h>

#include "math.h"
#include "mat3.h"
#include "utilMath.h"

#include "physicsRigidBody.h"
#include "physicsCollider.h"


#define combineFriction(f1, f2)    (sqrtf((f1) * (f2)))
#define combineRestitution(r1, r2) (((r1) >= (r2)) ? (r1) : (r2))


//Forward-declare any helper functions!
static void warmStartContact(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
static void calculateImpulses(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB);
static void calculateBias(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt);

static void solveTangents(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);
static void solveNormal(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB);


//Build a physics manifold by expanding a contact manifold.
void physManifoldInit(physicsManifold *pm, const contactManifold *cm, const physicsCollider *cA, const physicsCollider *cB){
	physicsContactPoint *pmContact = pm->contacts;
	const contactPoint *cmContact = cm->contacts;
	const contactPoint *lastContact = &cmContact[cm->numContacts];

	const vec3 *bodyACentroid = &cA->owner->centroidGlobal;
	const vec3 *bodyBCentroid = &cB->owner->centroidGlobal;

	//Create a physics contact point for each regular one!
	do {
		pmContact->key = cmContact->key;

		vec3SubtractVec3FromOut(&cmContact->position, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3FromOut(&cmContact->position, bodyBCentroid, &pmContact->rB);

		pmContact->penetration = cmContact->penetration;

		pmContact->normalAccumulator = 0.f;
		pmContact->tangentAccumulator[0] = 0.f;
		pmContact->tangentAccumulator[1] = 0.f;

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
				//to their new positions in the regular manifold. This will
				//make it easier to copy the keys for non-persistent points.
				if(pmContact != pmSwap){
					const contactKey tempKey = pmSwap->key;
					float tempAccumulator;

					//Swap the keys.
					pmSwap->key = pmContact->key;
					pmContact->key = tempKey;

					//Swap the accumulators.
                    tempAccumulator = pmSwap->normalAccumulator;
                    pmSwap->normalAccumulator = pmContact->normalAccumulator;
                    pmContact->normalAccumulator = tempAccumulator;

                    tempAccumulator = pmSwap->tangentAccumulator[0];
                    pmSwap->tangentAccumulator[0] = pmContact->tangentAccumulator[0];
                    pmContact->tangentAccumulator[0] = tempAccumulator;

                    tempAccumulator = pmSwap->tangentAccumulator[1];
                    pmSwap->tangentAccumulator[1] = pmContact->tangentAccumulator[1];
                    pmContact->tangentAccumulator[1] = tempAccumulator;
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
		vec3SubtractVec3FromOut(&cmContact->position, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3FromOut(&cmContact->position, bodyBCentroid, &pmContact->rB);
		pmContact->penetration = cmContact->penetration;

		//Contact is not persistent, so we can reset the accumulators.
		if(!(*isPersistent)){
			pmContact->key = cmContact->key;
			pmContact->normalAccumulator = 0.f;
			pmContact->tangentAccumulator[0] = 0.f;
			pmContact->tangentAccumulator[1] = 0.f;

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
** Such values include the impulse denominator and the
** bias term.
*/
void physManifoldPresolve(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB, const float dt){
	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		calculateImpulses(pm, curContact, bodyA, bodyB);
		calculateBias(pm, curContact, bodyA, bodyB, dt);
	}
}

/*
** Apply an impulse to penetrating physics objects to ensure
** that they separate from each other on the next frame.
** This may be called multiple times with sequential impulse.
*/
void physManifoldSolve(physicsManifold *pm, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		solveTangents(pm, curContact, bodyA, bodyB);
		solveNormal(pm, curContact, bodyA, bodyB);
	}
}


/*
** If a contact point has persisted across multiple frames,
** we are able to warm start it. This will allow it to more
** quickly converge to its correct state when we're solving.
**/
static void warmStartContact(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	//What we call "impulse", Erin Catto refers
	//to as "p" (momentum?) in most of his papers.
	vec3 impulse;
	vec3 accumulator;

	//Warm start the contact point using the total
	//accumulated normal and frictional impulses.
	vec3MultiplySOut(&pm->normal, contact->normalAccumulator, &impulse);
	vec3MultiplySOut(&pm->tangents[0], contact->tangentAccumulator[0], &accumulator);
	vec3AddVec3(&impulse, &accumulator);
	vec3MultiplySOut(&pm->tangents[1], contact->tangentAccumulator[1], &accumulator);
	vec3AddVec3(&impulse, &accumulator);

	//Apply the accumulated impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}

/*
** Calculate the effective mass of the two colliding bodies, which
** won't change between solver iterations. We can just do it once.
*/
static void calculateImpulses(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB){
	vec3 wImpulseA;
	vec3 wDeltaA;
	vec3 wImpulseB;
	vec3 wDeltaB;

	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *invInertiaB = &bodyB->invInertiaGlobal;

	//Calculate the normal impulse denominator.
	vec3CrossVec3(&contact->rA, &pm->normal, &wImpulseA);
	mat3MultiplyByVec3Out(invInertiaA, &wImpulseA, &wDeltaA);
	vec3CrossVec3(&contact->rB, &pm->normal, &wImpulseB);
	mat3MultiplyByVec3Out(invInertiaB, &wImpulseB, &wDeltaB);
	contact->normalImpulse = 1.f / (
		invMass +
		vec3DotVec3(&wImpulseA, &wDeltaA) +
		vec3DotVec3(&wImpulseB, &wDeltaB)
	);

	//Calculate the first tangent impulse denominator.
	vec3CrossVec3(&pm->tangents[0], &contact->rA, &wImpulseA);
	mat3MultiplyByVec3Out(invInertiaA, &wImpulseA, &wDeltaA);
	vec3CrossVec3(&pm->tangents[0], &contact->rB, &wImpulseB);
	mat3MultiplyByVec3Out(invInertiaB, &wImpulseB, &wDeltaB);
	contact->tangentImpulse[0] = 1.f / (
		invMass +
		vec3DotVec3(&wImpulseA, &wDeltaA) +
		vec3DotVec3(&wImpulseB, &wDeltaB)
	);

	//Calculate the second tangent impulse denominator.
	vec3CrossVec3(&pm->tangents[1], &contact->rA, &wImpulseA);
	mat3MultiplyByVec3Out(invInertiaA, &wImpulseA, &wDeltaA);
	vec3CrossVec3(&pm->tangents[1], &contact->rB, &wImpulseB);
	mat3MultiplyByVec3Out(invInertiaB, &wImpulseB, &wDeltaB);
	contact->tangentImpulse[1] = 1.f / (
		invMass +
		vec3DotVec3(&wImpulseA, &wDeltaA) +
		vec3DotVec3(&wImpulseB, &wDeltaB)
	);
}

//Calculate the contact's bias term, which will help prevent jitter.
static void calculateBias(const physicsManifold *pm, physicsContactPoint *contact, const physicsRigidBody *bodyA, const physicsRigidBody *bodyB, const float dt){
	float tempBias;
	vec3 tempVelocity;
	vec3 contactVelocity;


	//Calculate the Baumgarte bias term.
	tempBias = contact->penetration + PHYSCONTACT_PENETRATION_SLOP;
	contact->bias = (tempBias > 0.f) ? (-PHYSCONTACT_BAUMGARTE_BIAS * dt * tempBias) : 0.f;


	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3(&bodyA->angularVelocity, &contact->rA, &tempVelocity);
	vec3AddVec3(&tempVelocity, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &tempVelocity);

	//Calculate the restitution bias term.
	tempBias = vec3DotVec3(&contactVelocity, &pm->normal);
	if(tempBias < PHYSCONTACT_RESTITUTION_THRESHOLD){
		contact->bias += pm->restitution * tempBias;
	}
}


//Calculate the correctional friction impulse to apply.
static void solveTangents(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	const float lambdaClamp = pm->friction * contact->normalAccumulator;
	float oldAccumulator;
	float newAccumulator;
	vec3 impulse;
	vec3 contactVelocity;

	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[0]) * contact->tangentImpulse[0];
	oldAccumulator = contact->tangentAccumulator[0];
	newAccumulator = oldAccumulator + lambda;

	//Clamp our accumulated impulse for the first tangent.
	if(-lambdaClamp > newAccumulator){
		contact->tangentAccumulator[0] = -lambdaClamp;
		vec3MultiplySOut(&pm->tangents[0], -lambdaClamp - oldAccumulator, &impulse);
	}else if(lambdaClamp < newAccumulator){
		contact->tangentAccumulator[0] = lambdaClamp;
		vec3MultiplySOut(&pm->tangents[0], lambdaClamp - oldAccumulator, &impulse);
	}else{
		contact->tangentAccumulator[0] = newAccumulator;
		vec3MultiplySOut(&pm->tangents[0], lambda, &impulse);
	}

	//Apply the correctional impulse for the first tangent.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);


	lambda = -vec3DotVec3(&contactVelocity, &pm->tangents[1]) * contact->tangentImpulse[1];
	oldAccumulator = contact->tangentAccumulator[1];
	newAccumulator = oldAccumulator + lambda;

	//Clamp our accumulated impulse for the second tangent.
	if(-lambdaClamp > newAccumulator){
		contact->tangentAccumulator[1] = -lambdaClamp;
		vec3MultiplySOut(&pm->tangents[1], -lambdaClamp - oldAccumulator, &impulse);
	}else if(lambdaClamp < newAccumulator){
		contact->tangentAccumulator[1] = lambdaClamp;
		vec3MultiplySOut(&pm->tangents[1], lambdaClamp - oldAccumulator, &impulse);
	}else{
		contact->tangentAccumulator[1] = newAccumulator;
		vec3MultiplySOut(&pm->tangents[1], lambda, &impulse);
	}

	//Apply the correctional impulse for the second tangent.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}

//Calculate the correctional normal impulse to apply.
static void solveNormal(const physicsManifold *pm, physicsContactPoint *contact, physicsRigidBody *bodyA, physicsRigidBody *bodyB){
	float lambda;
	float oldAccumulator;
	float newAccumulator;
	vec3 impulse;
	vec3 contactVelocity;

	//Calculate the total linear velocity of the contact point on body A.
	vec3CrossVec3(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	//Calculate the total linear velocity of the contact point on body B.
	vec3CrossVec3(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	//Calculate the relative velocity between the two points.
	vec3SubtractVec3From(&contactVelocity, &impulse);


	lambda = contact->normalImpulse * (-vec3DotVec3(&contactVelocity, &pm->normal) * contact->bias);
	oldAccumulator = contact->normalAccumulator;
	newAccumulator = oldAccumulator + lambda;

	//Clamp our accumulated impulse so it doesn't become negative.
	if(newAccumulator >= 0.f){
		contact->normalAccumulator = newAccumulator;
		vec3MultiplySOut(&pm->normal, lambda, &impulse);
	}else{
		contact->normalAccumulator = 0.f;
		vec3MultiplySOut(&pm->normal, -oldAccumulator, &impulse);
	}

	//Apply the correctional impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}