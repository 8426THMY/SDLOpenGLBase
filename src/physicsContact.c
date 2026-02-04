#include "physicsContact.h"


#include <stdint.h>
#include <string.h>

#include "math.h"
#include "mat3.h"
#include "utilMath.h"

#include "physicsRigidBody.h"
#include "physicsCollider.h"


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
** b = b_separation + b_restitution
**   = B/dt * C(x) + e * C'(x),
**
** C' : JV + b >= 0,
**
** where B is the Baumgarte constant and e is the contact restitution.
**
** ----------------------------------------------------------------------
**
** Using sequential impulses, we apply velocities in order to
** satisfy constraints. If V and V_f are the current and final
** velocity vectors, then we can write
**
** V_f = V + DV,
**
** where DV is the change in velocity due to our constraint.
** Like usual, we can write this velocity in terms a force
** which we will call the constraint force F_C,
**
** DV = M^{-1} * F_C.
**
** We don't want our constraints to introduce energy into
** the system, so F_C must be perpendicular to V. However,
** from our velocity constraint, we have JV = 0, which
** implies that J^T is perpendicular to V. Therefore, we
** can say that F_C is some scalar multiple of J^T, so
**
** DV = lambda * M^{-1} * J^T.
**
** Plugging this into J(V + DV) + b = 0 and solving for lambda,
**
** J(V + DV) + b = 0,
** JV + J * lambda * M^{-1} * J^T + b = 0,
** J * lambda * M^{-1} * J^T = -(JV + b),
** lambda = -(JV + b)/(JM^{-1}J^T),
**
** as required.
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
**       [    -n   ]
**       [-(rA X n)]
** J^T = [     n   ].
**       [ (rB X n)]
**
**
** Evaluating this expression gives us:
**
** JM^{-1}J^T = mA^{-1} + mB^{-1} + ((rA X n) . (IA^{-1} * (rA X n))) + ((rB X n) . (IB^{-1} * (rB X n))).
**
** ----------------------------------------------------------------------
*/


// Forward-declare any helper functions!
static float combineFriction(const float mu1, const float mu2);
static float combineRestitution(const float e1, const float e2);

#ifdef PHYSCONTACT_WARM_START
static void warmStartContactPoint(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
);
#endif
static float calculateEffectiveMass(
	const vec3 *const restrict pointA, const vec3 *const restrict pointB, const vec3 *const restrict normal,
	const mat3 *const restrict invInertiaA, const mat3 *const restrict invInertiaB, const float invMass
);
static void calculateInverseEffectiveMass(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
);
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void calculateBias(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float frequency
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

	const vec3 *const bodyACentroid = &cA->owner->centroid;
	const vec3 *const bodyBCentroid = &cB->owner->centroid;
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	const quat *const bodyARot      = &cA->owner->state.rot;
	const quat *const bodyBRot      = &cB->owner->state.rot;
	#endif

	vec3 normal;
	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	vec3 halfway;
	vec3InitZero(&halfway);
	#endif
	vec3InitZero(&normal);

	// Create a physics contact point for each regular one!
	// Note that we calculate the bias and effective mass when we presolve.
	do {
		vec3 curHalfway;
		vec3AddVec3Out(&cmContact->pA, &cmContact->pB, &curHalfway);
		vec3MultiplyS(&curHalfway, 0.5f);
		// We use the point halfway between the contact points when resolving collision.
		vec3SubtractVec3Out(&curHalfway, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3Out(&curHalfway, bodyBCentroid, &pmContact->rB);

		#ifdef PHYSCONTACT_USE_FRICTION_JOINT
		vec3AddVec3(&halfway, &curHalfway);
		#endif

		// When we're using non-linear Guass-Seidel, we
		// need the untransformed contact points and normal.
		//
		// Solving the positional constraints will obviously
		// only affect the position and orientation, not the
		// scale. Hence, we don't bother undoing scale here.
		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		vec3SubtractVec3Out(&cmContact->pA, bodyACentroid, &curHalfway);
		quatConjRotateVec3FastOut(bodyARot, &curHalfway, &pmContact->rAlocal);
		vec3SubtractVec3Out(&cmContact->pB, bodyBCentroid, &curHalfway);
		quatConjRotateVec3FastOut(bodyBRot, &curHalfway, &pmContact->rBlocal);
		#endif

		// Find the average contact normal.
		vec3AddVec3(&normal, &cmContact->normal);

		#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		pmContact->separation = cmContact->separation;
		#endif
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
	physContactNormal(pm) = normal;
	#ifdef PHYSCONTACT_FRICTION_DELAY
	// Delay the effects of friction for the first frame of collision.
	vec3InitZero(&physContactTangent(pm, 0));
	vec3InitZero(&physContactTangent(pm, 1));
	#else
	// Set the tangent vectors such that they form an
	// orthonormal basis together with the contact normal.
	orthonormalBasisFaster(&normal, &physContactTangent(pm, 0), &physContactTangent(pm, 1));
	#endif

	// If we're using a friction joint, we initialize it here.
	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	// Get the average point halfway between contact points.
	vec3DivideByS(&halfway, cm->numContacts);
	vec3SubtractVec3Out(&halfway, bodyACentroid, &pm->frictionJoint.rA);
	vec3SubtractVec3Out(&halfway, bodyBCentroid, &pm->frictionJoint.rB);

	vec2InitZero(&pm->frictionJoint.linearImpulse);
	pm->frictionJoint.angularImpulse = 0.f;
	#endif

	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	quatConjRotateVec3FastOut(bodyARot, &normal, &pm->normalLocal);
	#endif

	physContactFriction(pm) = combineFriction(cA->friction, cB->friction);
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
	const physicsContactPoint *const lastPhysContact = &pmContact[pm->numContacts];

	const vec3 *const bodyACentroid = &cA->owner->centroid;
	const vec3 *const bodyBCentroid = &cB->owner->centroid;
	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	const quat *const bodyARot      = &cA->owner->state.rot;
	const quat *const bodyBRot      = &cB->owner->state.rot;
	#endif

	unsigned int persistentFlags[CONTACT_MAX_POINTS];
	unsigned int *isPersistent = persistentFlags;

	vec3 normal;
	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	vec3 halfway;
	#endif

	vec3InitZero(&normal);
	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	vec3InitZero(&halfway);
	#endif
	// Initialise our flags to 0. A value of 0
	// means that a contact is not persistent.
	memset(persistentFlags, 0, sizeof(persistentFlags));

	do {
		physicsContactPoint *pmSwap = pm->contacts;
		do {
			// We've found a matching key pair, so the point is persistent!
			// This works since our contact keys have no padding.
			if(memcmp(&pmSwap->key, &cmContact->key, sizeof(pmSwap->key)) == 0){
				// We want contacts in the old physics manifold to be moved
				// to their new positions in the current manifold. This will
				// make it easier to copy the keys for non-persistent points.
				if(pmSwap != pmContact){
					const contactKey tempKey = pmContact->key;
					float tempImpulse;

					// Swap the keys.
					pmContact->key = pmSwap->key;
					pmSwap->key = tempKey;

					// Swap the accumulators.
					tempImpulse = pmContact->normalImpulse;
					pmContact->normalImpulse = pmSwap->normalImpulse;
					pmSwap->normalImpulse = tempImpulse;

					#ifndef PHYSCONTACT_USE_FRICTION_JOINT
					tempImpulse = pmContact->tangentImpulse[0];
					pmContact->tangentImpulse[0] = pmSwap->tangentImpulse[0];
					pmSwap->tangentImpulse[0] = tempImpulse;

					tempImpulse = pmContact->tangentImpulse[1];
					pmContact->tangentImpulse[1] = pmSwap->tangentImpulse[1];
					pmSwap->tangentImpulse[1] = tempImpulse;
					#endif
				}

				*isPersistent = 1;
				break;
			}

			++pmSwap;
		} while(pmSwap < lastPhysContact);

		// Find the average contact normal.
		vec3AddVec3(&normal, &cmContact->normal);

		++cmContact;
		++pmContact;
		++isPersistent;
	} while(cmContact < lastContact);


	vec3NormalizeVec3Fast(&normal);
	// Set the tangent vectors such that they form an
	// orthonormal basis together with the contact normal.
	orthonormalBasisFaster(&normal, &physContactTangent(pm, 0), &physContactTangent(pm, 1));
	physContactNormal(pm) = normal;

	#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
	quatConjRotateVec3FastOut(bodyARot, &normal, &pm->normalLocal);
	#endif

	physContactFriction(pm) = combineFriction(cA->friction, cB->friction);
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
		vec3SubtractVec3Out(&curHalfway, bodyACentroid, &pmContact->rA);
		vec3SubtractVec3Out(&curHalfway, bodyBCentroid, &pmContact->rB);

		#ifdef PHYSCONTACT_USE_FRICTION_JOINT
		vec3AddVec3(&halfway, &curHalfway);
		#endif

		// When we're using non-linear Guass-Seidel, we
		// need the untransformed contact points and normal.
		//
		// Solving the positional constraints will obviously
		// only affect the position and orientation, not the
		// scale. Hence, we don't bother undoing scale here.
		#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
		vec3SubtractVec3Out(&cmContact->pA, bodyACentroid, &curHalfway);
		quatConjRotateVec3FastOut(bodyARot, &curHalfway, &pmContact->rAlocal);
		vec3SubtractVec3Out(&cmContact->pB, bodyBCentroid, &curHalfway);
		quatConjRotateVec3FastOut(bodyBRot, &curHalfway, &pmContact->rBlocal);
		#endif

		#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		pmContact->separation = cmContact->separation;
		#endif

		// Contact point is not persistent, so we can reset its accumulators.
		if(!(*isPersistent)){
			pmContact->key = cmContact->key;
			pmContact->normalImpulse = 0.f;
			#ifndef PHYSCONTACT_USE_FRICTION_JOINT
			pmContact->tangentImpulse[0] = 0.f;
			pmContact->tangentImpulse[1] = 0.f;
			#endif

		#ifdef PHYSCONTACT_WARM_START
		// If it is, we can warm start it!
		}else{
			warmStartContactPoint(pm, pmContact, cA->owner, cB->owner);
		#endif
		}

		++cmContact;
		++pmContact;
		++isPersistent;
	} while(cmContact < lastContact);

	pm->numContacts = cm->numContacts;


	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	// Get the average point halfway between contact points.
	vec3DivideByS(&halfway, cm->numContacts);
	vec3SubtractVec3Out(&halfway, bodyACentroid, &pm->frictionJoint.rA);
	vec3SubtractVec3Out(&halfway, bodyBCentroid, &pm->frictionJoint.rB);

	#ifdef PHYSJOINTFRICTION_WARM_START
	physJointFrictionWarmStart(&pm->frictionJoint, cA->owner, cB->owner);
	#endif
	#endif
}


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
void physManifoldPresolve(
	physicsManifold *const restrict pm, const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB, const float frequency
){
#else
void physManifoldPresolve(
	physicsManifold *const restrict pm, const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){
#endif

	physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *const lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		calculateInverseEffectiveMass(pm, curContact, bodyA, bodyB);
		#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
		calculateBias(pm, curContact, bodyA, bodyB, frequency);
		#else
		calculateBias(pm, curContact, bodyA, bodyB);
		#endif
	}

	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	physJointFrictionCalculateInverseEffectiveMass(&pm->frictionJoint, bodyA, bodyB);
	#endif
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
		#endif
		solveNormal(pm, curContact, bodyA, bodyB);
		#ifdef PHYSCONTACT_USE_FRICTION_JOINT
		// Sum up the total impulse in the normal direction for each
		// contact point. We use this when calculating the friction.
		maxForce += curContact->normalImpulse;
		#endif
	}

	#ifdef PHYSCONTACT_USE_FRICTION_JOINT
	// If we're using a joint to simulate friction, we'll need to
	// solve its constraints after solving the contact constraints.
	physJointFrictionSolveVelocity(&pm->frictionJoint, bodyA, bodyB, maxForce);
	#endif
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times.
*/
#ifdef PHYSCONTACT_STABILISER_GAUSS_SEIDEL
float physManifoldSolvePosition(const physicsManifold *const restrict pm, physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB, float separation){
	const physicsContactPoint *curContact = pm->contacts;
	const physicsContactPoint *const lastContact = &curContact[pm->numContacts];

	for(; curContact < lastContact; ++curContact){
		const float newSeparation = solvePosition(pm, curContact, bodyA, bodyB);
		// The maximum error is given by the most penetrating contact point.
		if(newSeparation < separation){
			separation = newSeparation;
		}
	}

	return(separation);
}
#endif


// Initialise a contact pair from a manifold.
void physContactPairInit(
	physicsContactPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsContactPair *prev, physicsContactPair *next
){

	physPairRefresh(pair);

	pair->cA = cA;
	pair->cB = cB;

	// Make the previous pair in collider A's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cA->contacts = pair;
	}
	// Make the next pair in collider A's
	// linked list point to the new one.
	if(next != NULL){
		if(cA == next->cA){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	// Set the new pair's list
	// pointers for collider A.
	pair->prevA = prev;
	pair->nextA = next;

	prev = NULL;
	next = cB->contacts;
	// Find the position in collider B's list
	// where this pair should be inserted.
	// We insert it after the last pair
	// whose first collider is collider B.
	while(next != NULL && cB == next->cA){
		prev = next;
		next = next->nextA;
	}

	// Make the previous pair in collider B's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cB->contacts = pair;
	}
	// Make the next pair in collider B's
	// linked list point to the new one.
	if(next != NULL){
		next->prevB = pair;
	}
	// Set the new pair's list
	// pointers for collider B.
	pair->prevB = prev;
	pair->nextB = next;
}

// Initialise a separation pair from a separation.
void physSeparationPairInit(
	physicsSeparationPair *const restrict pair,
	physicsCollider *const restrict cA, physicsCollider *const restrict cB,
	physicsSeparationPair *prev, physicsSeparationPair *next
){

	physPairRefresh(pair);

	pair->cA = cA;
	pair->cB = cB;

	// Make the previous pair in collider A's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cA->separations = pair;
	}
	// Make the next pair in collider A's
	// linked list point to the new one.
	if(next != NULL){
		if(cA == next->cA){
			next->prevA = pair;
		}else{
			next->prevB = pair;
		}
	}
	// Set the new pair's list
	// pointers for collider A.
	pair->prevA = prev;
	pair->nextA = next;

	prev = NULL;
	next = cB->separations;
	// Find the position in collider B's list
	// where this pair should be inserted.
	// We insert it after the last pair
	// whose first collider is collider B.
	while(next != NULL && cB == next->cA){
		prev = next;
		next = next->nextA;
	}

	// Make the previous pair in collider B's
	// linked list point to the new one.
	if(prev != NULL){
		prev->nextA = pair;
	}else{
		cB->separations = pair;
	}
	// Make the next pair in collider B's
	// linked list point to the new one.
	if(next != NULL){
		next->prevB = pair;
	}
	// Set the new pair's list
	// pointers for collider B.
	pair->prevB = prev;
	pair->nextB = next;
}

/*
** Delete a contact pair and remove it from the linked
** lists of both colliders involved in the contact.
*/
void physContactPairDelete(physicsContactPair *const restrict pair){
	physicsContactPair *nextPair = pair->nextA;
	physicsContactPair *prevPair = pair->prevA;
	// The next pair in collider A's list
	// should point to the previous one.
	if(nextPair != NULL){
		// Make sure we set the right pointer.
		if(nextPair->cA == pair->cA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	// The previous pair in collider A's
	// list should point to the next one.
	if(prevPair != NULL){
		prevPair->nextA = nextPair;

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->contacts = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		nextPair->prevB = prevPair;
	}
	if(prevPair != NULL){
		if(prevPair->cB == pair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		pair->cB->contacts = nextPair;
	}
}

/*
** Delete a separation pair and remove it from the linked
** lists of both colliders involved in the separation.
*/
void physSeparationPairDelete(physicsSeparationPair *const restrict pair){
	physicsSeparationPair *nextPair = pair->nextA;
	physicsSeparationPair *prevPair = pair->prevA;
	// The next pair in collider A's list
	// should point to the previous one.
	if(nextPair != NULL){
		// Make sure we set the right pointer.
		if(nextPair->cA == pair->cA){
			nextPair->prevA = prevPair;
		}else{
			nextPair->prevB = prevPair;
		}
	}
	// The previous pair in collider A's
	// list should point to the next one.
	if(prevPair != NULL){
		prevPair->nextA = nextPair;

	// If it's the first pair in the list,
	// update the collider's list pointer.
	}else{
		pair->cA->separations = nextPair;
	}

	nextPair = pair->nextB;
	prevPair = pair->prevB;
	// Do the same again but for collider B.
	if(nextPair != NULL){
		nextPair->prevB = prevPair;
	}
	if(prevPair != NULL){
		if(prevPair->cB == pair->cB){
			prevPair->nextB = nextPair;
		}else{
			prevPair->nextA = nextPair;
		}
	}else{
		pair->cB->separations = nextPair;
	}
}


static float combineFriction(const float mu1, const float mu2){
	#ifdef PHYSCONTACT_FRICTION_GEOMETRIC_AVERAGE
	return(sqrtf(mu1 * mu2));
	#else
	const float w1 = M_SQRT2 * (1.f - mu1) + 1.f;
	const float w2 = M_SQRT2 * (1.f - mu2) + 1.f;
	return((mu1*w1 + mu2*w2)/(w1 + w2));
	#endif
}

static float combineRestitution(const float e1, const float e2){
	return((e1 >= e2) ? e1 : e2);
}


/*
** If a contact point has persisted across multiple frames,
** we are able to warm start it. This will allow it to more
** quickly converge to its correct state when we're solving.
*/
#ifdef PHYSCONTACT_WARM_START
static void warmStartContactPoint(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	physicsRigidBody *const restrict bodyA, physicsRigidBody *const restrict bodyB
){

	vec3 impulse;

	// Warm start the contact point using the total
	// accumulated normal and frictional impulses.
	vec3MultiplySOut(&physContactNormal(pm), contact->normalImpulse, &impulse);
	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	vec3FmaP2(contact->tangentImpulse[0], &physContactTangent(pm, 0), &impulse);
	vec3FmaP2(contact->tangentImpulse[1], &physContactTangent(pm, 1), &impulse);
	#endif


	// Apply the accumulated impulse.
	physRigidBodyApplyImpulseInverse(bodyA, &contact->rA, &impulse);
	physRigidBodyApplyImpulse(bodyB, &contact->rB, &impulse);
}
#endif

/*
** Calculate the effective mass. This function is used to
** calculate it for the normal and both of the tangents.
*/
static float calculateEffectiveMass(
	const vec3 *const restrict pointA, const vec3 *const restrict pointB, const vec3 *const restrict normal,
	const mat3 *const restrict invInertiaA, const mat3 *const restrict invInertiaB, const float invMass
){

	vec3 rAn;
	vec3 IArAn;
	vec3 rBn;
	vec3 IBrBn;

	// JM^{-1}J^T = mA^{-1} + mB^{-1} + ((rA X n) . (IA^{-1} * (rA X n))) + ((rB X n) . (IB^{-1} * (rB X n)))
	vec3CrossVec3Out(pointA, normal, &rAn);
	mat3MultiplyVec3Out(invInertiaA, &rAn, &IArAn);
	vec3CrossVec3Out(pointB, normal, &rBn);
	mat3MultiplyVec3Out(invInertiaB, &rBn, &IBrBn);
	return(invMass + vec3DotVec3(&rAn, &IArAn) + vec3DotVec3(&rBn, &IBrBn));
}

/*
** Calculate the effective mass of the two colliding bodies, which
** won't change between velocity iterations. We can just do it once
** per update.
*/
static void calculateInverseEffectiveMass(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB
){

	const float invMass = bodyA->invMass + bodyB->invMass;
	const mat3 *const invInertiaA = &bodyA->invInertiaGlobal;
	const mat3 *const invInertiaB = &bodyB->invInertiaGlobal;
	float effectiveMass;

	// Calculate the inverse effective mass along the normal.
	effectiveMass = calculateEffectiveMass(
		&contact->rA, &contact->rB, &physContactNormal(pm),
		invInertiaA, invInertiaB, invMass
	);
	contact->invNormalMass = (effectiveMass > 0.f) ? 1.f/effectiveMass : 0.f;

	#ifndef PHYSCONTACT_USE_FRICTION_JOINT
	// Calculate the inverse effective mass along the first tangent.
	effectiveMass = calculateEffectiveMass(
		&contact->rA, &contact->rB, &physContactTangent(pm, 0),
		invInertiaA, invInertiaB, invMass
	);
	contact->invTangentMass[0] = (effectiveMass > 0.f) ? 1.f/effectiveMass : 0.f;

	// Calculate the inverse effective mass along the second tangent.
	effectiveMass = calculateEffectiveMass(
		&contact->rA, &contact->rB, &physContactTangent(pm, 1),
		invInertiaA, invInertiaB, invMass
	);
	contact->invTangentMass[1] = (effectiveMass > 0.f) ? 1.f/effectiveMass : 0.f;
	#endif
}

// Calculate the contact's bias term.
#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
static void calculateBias(
	const physicsManifold *const restrict pm, physicsContactPoint *const restrict contact,
	const physicsRigidBody *const restrict bodyA, const physicsRigidBody *const restrict bodyB,
	const float frequency
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
	tempBias = contact->separation + PHYSCONTACT_LINEAR_SLOP;
	// B = Baumgarte constant
	// b_separation = B/dt * C(x)
	contact->bias = (tempBias < 0.f) ? (PHYSCONTACT_BAUMGARTE_BIAS * frequency * tempBias) : 0.f;
	#endif


	// vA_contact = vA + wA X rA
	// vB_contact = vB + wB X rB
	// v_relative = vB_contact - vA_contact

	// Calculate the total velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &tempVelocity);
	vec3AddVec3(&tempVelocity, &bodyA->linearVelocity);
	// Calculate the total velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3P1(&contactVelocity, &tempVelocity);

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
		// b = b_separation + b_restitution
		//   = B/dt * C(x)  + e * (v_relative . n)
		contact->bias += pm->restitution * tempBias;
	#else
		// If we're not using Baumgarte stabilisation,
		// we only have the restitution bias here.
		// b = e * (v_relative . n)
		contact->bias = pm->restitution * tempBias;

	// If the relative velocity is parallel to the normal,
	// the bodies are moving away, so ignore restitution.
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
	const float maxFriction = physContactFriction(pm) * contact->normalImpulse;
	float oldImpulse;
	vec3 temp;
	vec3 contactVelocity;
	vec3 impulse;


	// vA_contact = vA + wA X rA
	// vB_contact = vB + wB X rB
	// v_relative = vB_contact - vA_contact

	// Calculate the total velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &temp);
	vec3AddVec3(&temp, &bodyA->linearVelocity);
	// Calculate the total velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3P1(&contactVelocity, &temp);


	// lambda = -(JV + b)/(JM^{-1}J^T)
	//        = -(v_relative . n)/K
	lambda = -vec3DotVec3(&contactVelocity, &physContactTangent(pm, 0)) * contact->invTangentMass[0];

	// -f < lambda < f
	// Clamp our accumulated impulse for the first tangent.
	oldImpulse = contact->tangentImpulse[0];
	contact->tangentImpulse[0] = floatClamp(oldImpulse + lambda, -maxFriction, maxFriction);
	vec3MultiplySOut(&physContactTangent(pm, 0), contact->tangentImpulse[0] - oldImpulse, &impulse);


	// lambda = -(JV + b)/(JM^{-1}J^T)
	//        = -(v_relative . n)/K
	lambda = -vec3DotVec3(&contactVelocity, &physContactTangent(pm, 1)) * contact->invTangentMass[1];

	// -f < lambda < f
	// Clamp our accumulated impulse for the second tangent.
	oldImpulse = contact->tangentImpulse[1];
	contact->tangentImpulse[1] = floatClamp(oldImpulse + lambda, -maxFriction, maxFriction);
	vec3FmaP2(contact->tangentImpulse[1] - oldImpulse, &physContactTangent(pm, 1), &impulse);


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
	vec3 impulse;
	vec3 contactVelocity;


	// vA_contact = vA + wA X rA
	// vB_contact = vB + wB X rB
	// v_relative = vB_contact - vA_contact

	// Calculate the total velocity of the contact point on body A.
	vec3CrossVec3Out(&bodyA->angularVelocity, &contact->rA, &impulse);
	vec3AddVec3(&impulse, &bodyA->linearVelocity);
	// Calculate the total velocity of the contact point on body B.
	vec3CrossVec3Out(&bodyB->angularVelocity, &contact->rB, &contactVelocity);
	vec3AddVec3(&contactVelocity, &bodyB->linearVelocity);
	// Calculate the relative velocity between the two points.
	vec3SubtractVec3P1(&contactVelocity, &impulse);


	// lambda = -(JV + b)/(JM^{-1}J^T)
	//        = -((v_relative . n) + b)/K
	lambda = -(vec3DotVec3(&contactVelocity, &physContactNormal(pm)) + contact->bias) * contact->invNormalMass;

	// C' >= 0
	// Clamp our accumulated impulse in the normal direction.
	oldImpulse = contact->normalImpulse;
	contact->normalImpulse = floatMax(oldImpulse + lambda, 0.f);
	vec3MultiplySOut(&physContactNormal(pm), contact->normalImpulse - oldImpulse, &impulse);

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
	float constraint;


	// Update the contact points' positions using
	// their new translations and orientations.
	quatRotateVec3FastOut(&bodyA->state.rot, &contact->rAlocal, &rA);
	vec3AddVec3(&rA, &bodyA->centroid);
	quatRotateVec3FastOut(&bodyB->state.rot, &contact->rBlocal, &rB);
	vec3AddVec3(&rB, &bodyB->centroid);
	// We also need to update the normal.
	quatRotateVec3FastOut(&bodyA->state.rot, &pm->normalLocal, &normal);

	// With the contact points now in global space,
	// we can find the new separation between them.
	vec3SubtractVec3P1(&rB, &rA);
	separation = vec3DotVec3(&rB, &normal);

	constraint = PHYSCONTACT_BAUMGARTE_BIAS * (separation + PHYSCONTACT_LINEAR_SLOP);

	// Clamp the constraint value.
	if(constraint < 0.f){
		float effectiveMass;

		// Calculate the transformed contact points'
		// offsets from their bodies' centres of mass.
		vec3SubtractVec3Out(&rB, &bodyA->centroid, &rA);
		vec3SubtractVec3P1(&rB, &bodyB->centroid);

		// JA = (IA * (rA X n)) . (rA X n)
		// JB = (IB * (rB X n)) . (rB X n)
		//
		// K = JM^{-1}J^T
		//   = mA^{-1} + mB^{-1} + JA + JB

		// We use *full* non-linear Gauss-Seidel, which
		// requires us to recompute the effective mass.
		effectiveMass = calculateEffectiveMass(
			&rA, &rB, &normal,
			&bodyA->invInertiaGlobal, &bodyB->invInertiaGlobal, bodyA->invMass + bodyB->invMass
		);

		// Make sure we don't apply invalid impulses. This also
		// doubles as a check to ensure we don't divide by zero.
		if(effectiveMass > 0.f){
			// Finish clamping the constraint value.
			// Ignoring clamping and slops, all we're doing here is
			// multiplying the normal by the impulse magnitude, -C/K.
			vec3MultiplyS(&normal, -floatMax(constraint, -PHYSCONTACT_MAX_LINEAR_CORRECTION)/effectiveMass);

			// Apply the correctional impulse.
			physRigidBodyApplyImpulsePositionInverse(bodyA, &rA, &normal);
			physRigidBodyApplyImpulsePosition(bodyB, &rB, &normal);
		}
	}


	return(separation);
}
#endif