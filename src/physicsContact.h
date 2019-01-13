#ifndef physicsContact_h
#define physicsContact_h


#define PHYSCONTACT_NUM_TANGENTS 2


#include "contact.h"


typedef struct physicsContact {
	vec3 rA;
	vec3 rB;

	float penetration;
	//Stores the bias term, that is, the
	//restitution plus the Baumgarte term.
	float bias;

	//Stores the result of 1/JM^-1J^T (which is equivalent
	//to the inverse denominator of the impulse equation)
	//prior to collision response, as it only needs to be
	//calculated once.
	float impulseDenom;
	//Similar to "impulseDenom", but uses the tangent
	//vectors. These values are used for friction.
	float frictionDenom[PHYSCONTACT_NUM_TANGENTS];
} physicsContact;

typedef struct physicsRigidBody physicsRigidBody;
//A physics manifold is similar to a regular contact manifold,
//but stores additional information required to solve contacts.
typedef struct physicsManifold {
	physicsContact contacts[CONTACT_MAX_POINTS];
	byte_t numContacts;

	vec3 normal;
	//As well as the contact normal, we also need to store
	//the tangent vectors that together form an orthonormal
	//basis for when we calculate the effect of friction.
	vec3 tangents[PHYSCONTACT_NUM_TANGENTS];

	physicsRigidBody *bodyA;
	physicsRigidBody *bodyB;
} physicsManifold;


void physManifoldPresolve(physicsManifold *pm, float dt);
void physManifoldSolve(physicsManifold *pm);


/**
contactManifold *curManifold = manifolds;
contactManifold *lastManifold = &manifolds[numManifolds];
for(; curManifold < lastManifold; ++curManifold){
	manifoldPresolve();
}

for(i = 0; i < SOLVER_NUM_ITERATIONS; ++i){
	curManifold = manifolds;
	for(; curManifold < lastManifold; ++curManifold){
		manifoldSolve();
	}
}
**/


#endif