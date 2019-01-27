#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"

#include "physicsCollider.h"


typedef struct physicsRigidBodyDef {
	/*collider collider;

	//The rigid body's physical properties.
	float mass;
	float invMass;

	vec3 centroid;
	//Matrix that describes how the body
	//resists rotation around an axis.
	mat3 invInertia;*/
} physicsRigidBodyDef;

typedef struct physicsRigidBody {
	physicsCollider *colliders;
	uint_least16_t numColliders;

	float mass;
	float invMass;

	//We need to keep the local versions
	//for when we add new colliders.
	vec3 centroidLocal;
	vec3 centroidGlobal;
	mat3 invInertiaLocal;
	mat3 invInertiaGlobal;

	//Store the linear properties of the object.
	vec3 pos;
	vec3 linearVelocity;
	vec3 netForce;

	//Store the angular properties of the object.
	quat rot;
	vec3 angularVelocity;
	vec3 netTorque;
} physicsRigidBody;


/**
aabbNode {
	void *collider;
};


physicsCollider {
	physicsRigidBody *owner;
	aabbNode *node;
};

physicsRigidBody {
	physicsCollider *colliders;
	uint_least16_t numColliders;
};


physicsContactPair {
	physicsManifold manifold;

	physicsCollider *cA;
	physicsCollider *cB;

	physicsContactPair *prevA;
	physicsContactPair *nextA;
	physicsContactPair *prevB;
	physicsContactPair *nextB;
};

physicsSeparationPair {
	contactSeparation separation;

	physicsCollider *cA;
	physicsCollider *cB;

	physicsSeparationPair *prevA;
	physicsSeparationPair *nextA;
	physicsSeparationPair *prevB;
	physicsSeparationPair *nextB;
};


step(){
	for all bodies {
		integrate velocities;

		for all colliders {
			update vertices;
			update aabbs;
		}
	}
	for all bodies {
		for all colliders {
			for all colliding leaves in aabb tree {
				if collider < otherCollider {

					separation = NULL;
					for all separations pairs in collider {
						if pair contains these colliders {
							separation = pair;
							break;
						}
					}

					if narrowphase collision with separation {

						contact;
						if separation {
							delete separation pair(){
								pair->prevA->nextA = pair->nextA;
								pair->prevB->nextB = pair->nextB;
								free pair;
							}
							// Make sure the pair is added to each collider's list in order!
							contact = new contact pair;
						}else{
							contact = NULL;
							for all contact pairs in collider {
								if pair contains these colliders {
									contact = pair;
									break;
								}
							}
							if !contact {
								// Make sure the pair is added to each collider's list in order!
								contact = new contact pair;
							}
						}

						update contact pair;
						contact->active = 0;

					}else{

						if !separation {
							for all contact pairs in collider {
								if pair contains these colliders {
									delete contact pair(){
										pair->prevA->nextA = pair->nextA;
										pair->prevB->nextB = pair->nextB;
										free pair;
									}
									break;
								}
							}
							// Make sure the pair is added to each collider's list in order!
							separation = new separation pair;
						}

						update separation pair;
						separation->active = 0;

					}
				}
			}

			for all contacts and separations in collider {
				if collider < otherCollider {
					if pair->active == 0 {
						pair->active = 1;
					}else{
						delete contact or separation;
					}
				}
			}
		}
	}
}
**/


void physRigidBodyDefInit(physicsRigidBodyDef *body);

void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *rb, const float time);
void physRigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *rb, const float time);

void physRigidBodyUpdate(physicsRigidBody *body, const float dt);

//void physRigidBodyDefCalculateCentroid(physicsRigidBodyDef *body);
//void physRigidBodyDefCalculateInertiaTensor(physicsRigidBodyDef *body);


#endif