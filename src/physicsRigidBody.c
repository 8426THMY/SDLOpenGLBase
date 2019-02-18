#include "physicsRigidBody.h"


#include "modulePhysics.h"


#define RIGIDBODYDEF_PATH_PREFIX        ".\\resource\\physics\\"
#define RIGIDBODYDEF_PATH_PREFIX_LENGTH (sizeof(RIGIDBODYDEF_PATH_PREFIX) - 1)


void physRigidBodyDefInit(physicsRigidBodyDef *bodyDef){
	//
}

void physRigidBodyInit(physicsRigidBody *body){
	//
}


//Load a rigid body, including any of its colliders.
void physRigidBodyDefLoad(physicsRigidBodyDef *bodyDef, const char *bodyPath){
	//
}


/*
** Adds a physics collider to a rigid body's list. This
** assumes that the collider's centroid and moment of
** inertia tensor have already been calculated. Note
** that this assumes that the body's inertia tensor has
** note yet been inverted. It will not invert it, either.
*/
void physRigidBodyDefAddCollider(physicsRigidBodyDef *bodyDef, const float mass, const vec3 *centroid, const mat3 *inertia){
	vec3 tempCentroid;
	float xx;
	float yy;
	float zz;
	mat3 tempInertia = *inertia;


	//Calculate the collider's contribution to the body's centroid.
	vec3MultiplySOut(centroid, mass, &tempCentroid);
	vec3MultiplyS(&bodyDef->centroid, bodyDef->mass);

	//Calculate the new mass and inverse mass.
	bodyDef->mass += mass;
	bodyDef->invMass = 1.f / bodyDef->mass;

	//Add this collider's contribution to the centroid.
	vec3AddVec3(&bodyDef->centroid, &tempCentroid);
	vec3MultiplyS(&bodyDef->centroid, bodyDef->invMass);


	//Add this collider's contribution to the inertia tensor.
	//We use the parallel axis theorem to translate it from
	//the collider's centroid to the rigid body's centroid.
	//
	//J = I + m(dot(R, R) * E - out(R, R))
	//
	//J = new inertia tensor
	//I = old inertia tensor
	//m = mass
	//R = vector displacement from old to new center of mass
	//E = 3x3 identity matrix
	//
	//This can be simplified as follows to reduce matrix operations.
	//
	//d = dot(R, R) = xx + yy + zz
	//dI = [d 0 0]
	//     [0 d 0]
	//     [0 0 d]
	//P = out(R, R) = [xx yx zx]
	//                [xy yy zy]
	//                [xz yz zz]
	//dI - P = [(yy + zz)    -yx       -zx   ]
	//         [   -xy    (xx + zz)    -zy   ]
	//         [   -xz       -yz    (xx + yy)]

	vec3SubtractVec3FromOut(&bodyDef->centroid, centroid, &tempCentroid);
	xx = tempCentroid.x * tempCentroid.x;
	yy = tempCentroid.y * tempCentroid.y;
	zz = tempCentroid.z * tempCentroid.z;

	//Translate the inertia tensor using the rigid body's centroid.
	tempInertia.m[0][0] += (yy + zz) * mass;
	tempInertia.m[0][1] -= (tempCentroid.x * tempCentroid.y) * mass;
	tempInertia.m[0][2] -= (tempCentroid.x * tempCentroid.z) * mass;
	tempInertia.m[1][1] += (xx + zz) * mass;
	tempInertia.m[1][2] -= (tempCentroid.y * tempCentroid.z) * mass;
	tempInertia.m[2][2] += (xx + yy) * mass;

	//Add the collider's contribution to the body's inertia tensor.
	bodyDef->invInertia.m[0][0] += tempInertia.m[0][0];
	bodyDef->invInertia.m[0][1] += tempInertia.m[0][1];
	bodyDef->invInertia.m[0][2] += tempInertia.m[0][2];
	bodyDef->invInertia.m[1][0] = bodyDef->invInertia.m[0][1];
	bodyDef->invInertia.m[1][1] += tempInertia.m[1][1];
	bodyDef->invInertia.m[1][2] += tempInertia.m[1][2];
	bodyDef->invInertia.m[2][0] = bodyDef->invInertia.m[0][2];
	bodyDef->invInertia.m[2][1] = bodyDef->invInertia.m[1][2];
	bodyDef->invInertia.m[2][2] += tempInertia.m[2][2];
}

/*
** Sum the mass, centroid and intertia tensor of each of the body's
** colliders in order to find the combined centroid and inverse
** inertia tensor. This will also calculate the body's inverse mass.
*/
void physRigidBodyDefGenerateProperties(physicsRigidBodyDef *bodyDef){
	float tempMass = 0.f;
	vec3 tempCentroid;
	float tempInertia[6];
	physicsCollider *curCollider = bodyDef->colliders;

	vec3InitZero(&tempCentroid);
	memset(tempInertia, 0.f, sizeof(tempInertia));

	//Calculate the rigid body's combined mass and centroid!
	while(curCollider != NULL){
		vec3 curCentroid;

		/** Convex hulls should have their vertices weighted. **/
		physColliderGenerateCentroid(curCollider, &curCentroid);

		//Add the collider's contribution to the rigid body's centroid.
		/** We clearly don't take into account the collider's mass. **/
		vec3MultiplyS(&curCentroid, 0.f);
		vec3AddVec3(&tempCentroid, &curCentroid);
		tempMass += 0.f;

		curCollider = memSingleListNext(curCollider);
	}

	bodyDef->mass = tempMass;
	bodyDef->centroid = tempCentroid;


	if(tempMass != 0.f){
		bodyDef->invMass = 1.f / tempMass;
		vec3MultiplyS(&tempCentroid, bodyDef->invMass);


		curCollider = bodyDef->colliders;
		//Calculate the rigid body's combined intertia tensor!
		while(curCollider != NULL){
			mat3 curInertia;

			/** Convex hulls should have their vertices weighted. **/
			physColliderGenerateInertia(curCollider, &tempCentroid, &curInertia);

			//Add the collider's contribution to the rigid body's inertia tensor.
			tempInertia[0] += curInertia.m[0][0];
			tempInertia[1] += curInertia.m[1][1];
			tempInertia[2] += curInertia.m[2][2];
			tempInertia[3] += curInertia.m[0][1];
			tempInertia[4] += curInertia.m[0][2];
			tempInertia[5] += curInertia.m[1][2];

			curCollider = memSingleListNext(curCollider);
		}

		bodyDef->invInertia.m[0][0] = tempInertia[0];
		bodyDef->invInertia.m[1][1] = tempInertia[1];
		bodyDef->invInertia.m[2][2] = tempInertia[2];
		bodyDef->invInertia.m[0][1] = tempInertia[3];
		bodyDef->invInertia.m[0][2] = tempInertia[4];
		bodyDef->invInertia.m[1][2] = tempInertia[5];
		//These should be the same as the values we've already calculated.
		bodyDef->invInertia.m[1][0] = tempInertia[3];
		bodyDef->invInertia.m[2][0] = tempInertia[4];
		bodyDef->invInertia.m[2][1] = tempInertia[5];

		//The inverse inertia tensor is more useful to us than the regular one.
		mat3Invert(&bodyDef->invInertia);
	}else{
		bodyDef->invMass = 0.f;
		mat3InitZero(&bodyDef->invInertia);
	}
}


/*
** Calculate the body's increase in
** velocity for the current timestep.
**
** v^(n + 1) = v^n + F * m^-1 * dt
** w^(n + 1) = w^n + T * I^-1 * dt
*/
void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *body, const float dt){
	vec3 linearAcceleration;
	vec3 angularAcceleration;

	//Calculate the body's linear acceleration.
	vec3MultiplySOut(&body->netForce, body->invMass * dt, &linearAcceleration);
	//Add the linear acceleration to the linear velocity.
	vec3AddVec3(&body->linearVelocity, &linearAcceleration);

	//Calculate the body's angular acceleration.
	vec3MultiplySOut(&body->netTorque, dt, &angularAcceleration);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &angularAcceleration);
	//Add the angular acceleration to the angular velocity.
	vec3AddVec3(&body->angularVelocity, &angularAcceleration);
}

/*
** Calculate the body's change in
** position for the current timestep.
**
** x^(n + 1) = x^n + v^(n + 1) * dt
** dq/dt = 0.5 * w * q
** q^(n + 1) = q^n + dq/dt * dt
*/
void physRigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *body, const float dt){
	vec3 linearVelocityDelta;

	vec3MultiplySOut(&body->linearVelocity, dt, &linearVelocityDelta);
	//Compute the object's new position.
	vec3AddVec3(&body->transform.pos, &linearVelocityDelta);

	//Calculate the change in orientation.
	quatIntegrate(&body->transform.rot, &body->angularVelocity, dt);
	//Don't forget to normalize it, as
	//this process can introduce errors.
	quatNormalizeQuat(&body->transform.rot);
}


//Add a translational and rotational impulse to a rigid body.
void physRigidBodyApplyImpulse(physicsRigidBody *body, const vec3 *r, const vec3 *J){
	vec3 impulse = *J;

	//Linear velocity.
	vec3MultiplyS(&impulse, body->mass);
	vec3AddVec3(&body->linearVelocity, &impulse);
	//Angular velocity.
	vec3CrossVec3(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3AddVec3(&body->angularVelocity, &impulse);
}

//Subtract a translational and rotational impulse from a rigid body.
void physRigidBodyApplyImpulseInverse(physicsRigidBody *body, const vec3 *r, const vec3 *J){
	vec3 impulse = *J;

	//Linear velocity.
	vec3MultiplyS(&impulse, body->mass);
	vec3SubtractVec3From(&body->linearVelocity, &impulse);
	//Angular velocity.
	vec3CrossVec3(r, J, &impulse);
	mat3MultiplyByVec3(&body->invInertiaGlobal, &impulse);
	vec3SubtractVec3From(&body->angularVelocity, &impulse);
}


/*
** Update a rigid body. This involves moving and
** rotating its centroid and inertia tensor, updating
** its velocity and updating all of its colliders.
*/
void physRigidBodyUpdate(physicsRigidBody *body, const float dt){
	physicsCollider *curCollider;


	/** update centroid **/
	/** update inverse inertia tensor **/

	//Update the body's velocity.
	physRigidBodyIntegrateVelocitySymplecticEuler(body, dt);


	curCollider = body->colliders;
	//For every physics collider that is a part of
	//this rigid body, we will need to update its
	//base collider and its node in the broadphase.
	while(curCollider != NULL){
		physColliderUpdate(curCollider, NULL);
		curCollider = memSingleListNext(curCollider);
	}
}


void physRigidBodyDefDelete(physicsRigidBodyDef *bodyDef){
	modulePhysicsColliderFreeArray(&bodyDef->colliders);
}

void physRigidBodyDelete(physicsRigidBody *body){
	modulePhysicsColliderFreeArray(&body->colliders);
}