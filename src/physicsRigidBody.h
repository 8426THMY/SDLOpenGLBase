#ifndef physicsRigidBody_h
#define physicsRigidBody_h


#include <stdint.h>

#include "vec3.h"
#include "quat.h"
#include "mat3.h"
#include "transform.h"

#include "physicsCollider.h"
#include "physicsJoint.h"


// If we're using non-linear Gauss-Seidel stabilisation
// anywhere, we'll need to define some additional functions.
#if \
	defined PHYSCONTACT_STABILISER_GAUSS_SEIDEL        || defined PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL || \
	defined PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL     || defined PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL || \
	defined PHYSJOINTPRISMATIC_STABILISER_GAUSS_SEIDEL || defined PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL

	#define PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
#endif


typedef struct physicsRigidBodyDef {
	// Singly linked list of colliders used by this rigid body.
	physicsCollider *colliders;

	// The rigid body's physical properties.
	float mass;
	float invMass;

	vec3 centroid;
	// Matrix that describes how the body
	// resists rotation around an axis.
	mat3 invInertia;
} physicsRigidBodyDef;

#warning "Add some flags!"
#warning "Maybe store the regular local intertia tensor for more efficient scaling?"
#warning "Linear and angular damping would also be nice."
// Rigid body instance.
typedef struct physicsRigidBody {
	// Singly linked list of colliders used by this rigid body.
	physicsCollider *colliders;

	float mass;
	float invMass;

	// We need to keep the local versions
	// for when we add new colliders.
	vec3 centroidLocal;
	vec3 centroidGlobal;
	mat3 invInertiaLocal;
	mat3 invInertiaGlobal;

	// Defines the body's spacial configuration.
	// The position stored here should not be
	// confused with the rigid body's centroid!
	transformState state;

	// Defines the body's physical configuration.
	vec3 linearVelocity;
	vec3 angularVelocity;
	vec3 netForce;
	vec3 netTorque;

	// They also store a quad-list of joints
	// that they are a part of. These behave
	// similarly to the previous linked lists.
	physicsJoint *joints;
} physicsRigidBody;


void physRigidBodyDefInit(physicsRigidBodyDef *bodyDef);
void physRigidBodyInit(physicsRigidBody *body, const physicsRigidBodyDef *bodyDef);

return_t physRigidBodyDefLoad(physicsRigidBodyDef *bodyDef, const char *bodyPath);

void physRigidBodyDefSumCentroids(physicsRigidBodyDef *bodyDef, const vec3 *centroidArray, const size_t numBodies);
void physRigidBodyDefSumInertia(physicsRigidBodyDef *bodyDef, const mat3 *inertiaArray, const size_t numBodies);
void physRigidBodyDefAddCollider(physicsRigidBodyDef *bodyDef, const float mass, const vec3 *centroid, const mat3 *inertia);
void physRigidBodyDefGenerateProperties(physicsRigidBodyDef *bodyDef, const float **massArrays);

void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *body, const float time);
void physRigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *body, const float time);

void physRigidBodyApplyLinearImpulse(physicsRigidBody *body, vec3 J);
void physRigidBodyApplyLinearImpulseInverse(physicsRigidBody *body, vec3 J);
void physRigidBodyApplyAngularImpulse(physicsRigidBody *body, vec3 J);
void physRigidBodyApplyAngularImpulseInverse(physicsRigidBody *body, vec3 J);
void physRigidBodyApplyImpulse(physicsRigidBody *body, const vec3 *r, const vec3 *J);
void physRigidBodyApplyImpulseInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J);
#ifdef PHYSCOLLIDER_USE_POSITIONAL_CORRECTION
void physRigidBodyApplyImpulsePosition(physicsRigidBody *body, const vec3 *r, const vec3 *J);
void physRigidBodyApplyImpulsePositionInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J);
#endif

void physRigidBodyUpdate(physicsRigidBody *body, const float dt);

void physRigidBodyDefDelete(physicsRigidBodyDef *bodyDef);
void physRigidBodyDelete(physicsRigidBody *body);


/**
Load colliders.
Load bodies {
	How do we generate the centroid inertia tensor?
	We could do one of the following:
		Store an array of mat3s and centroids (one pair per collider)
		and a single vertex mass array (one used by every collider).

		Store an array of vertex mass arrays. Each array also contains
		the collider's mass and, possibly for hulls, an element at the
		end (containing NaN or something) that specifies whether or not
		the collider's vertices should be weighted.

		Add each collider's contribution to the centroid and inertia
		tensor the moment we finish loading it. This would require an
		extra division per collider and would also need the same vertex
		mass array used by the first method.
}

Component-wise addition for matrices (specifically mat3s)?
Strtoul base? Maybe check other strtox functions, too.
**/


#endif