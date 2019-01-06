#ifndef contact_h
#define contact_h


#define CONTACT_MAX_POINTS 4
#define CONTACT_NUM_TANGENTS 2


#include <stdlib.h>

#include "vec3.h"


typedef struct contactPoint {
	//Offsets of the contact points from
	//their colliders' centres of mass.
	vec3 rA;
	vec3 rB;

	//Penetration depth.
	float penetration;
    //Stores the bias term, that is, the
    //restitution plus the Baumgarte term.
    float bias;

	//Stores 1/JM^-1J^T prior to collision response, as it
	//only needs to be calculated once. This is equivalent
	//to the denominator of the impulse formula:
	//
	//1 / (mA + mB + (((IA * (rA X n)) X rA) + ((IB * (rB X n)) X rB)) . n)
    float impulseDenom;
    //Similar to "impulseDenom", but uses the tangent
    //vectors. These values are used for friction.
    float frictionDenom[2];
} contactPoint;


typedef struct physicsRigidBody physicsRigidBody;
//Stores every contact point involved in the collision.
typedef struct contactManifold {
	contactPoint contacts[CONTACT_MAX_POINTS];
	size_t numContacts;

	vec3 normal;
	vec3 tangents[CONTACT_NUM_TANGENTS];

	physicsRigidBody *bodyA;
	physicsRigidBody *bodyB;
} contactManifold;


void manifoldPresolve(contactManifold *cm);
void manifoldSolve(contactManifold *cm);


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