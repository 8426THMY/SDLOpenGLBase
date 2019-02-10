#include "physicsRigidBody.h"


void physRigidBodyDefInit(physicsRigidBodyDef *bodyDef){
	//physRigidBodyDefCalculateCentroid(bodyDef);
	//physRigidBodyDefCalculateInertiaTensor(bodyDef);
}


/*
** Calculate the body's increase in
** velocity for the current timestep.
**
** v^(n + 1) = v^n + F * m^-1 * dt
** w^(n + 1) = w^n + T * I^-1 * dt
*/
void physRigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *body, const float dt){
	/*vec3 linearAcceleration;
	vec3 angularAcceleration;

	//Calculate the body's linear acceleration.
	vec3MultiplySOut(&body->netForce, body->body->invMass * dt, &linearAcceleration);
	//Add the linear acceleration to the linear velocity.
	vec3AddVec3(&body->linearVelocity, &linearAcceleration);

	//Calculate the body's angular acceleration.
	vec3MultiplySOut(&body->netTorque, dt, &angularAcceleration);
	mat3MultiplyVec3(&body->invInertia, &angularAcceleration, &angularAcceleration);
	//Add the angular acceleration to the angular velocity.
	vec3AddVec3(&body->angularVelocity, &angularAcceleration);*/
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
	/*vec3 linearVelocityDelta;

	vec3MultiplySOut(&body->linearVelocity, dt, &linearVelocityDelta);
	//Compute the object's new position.
	vec3AddVec3(&body->pos, &linearVelocityDelta);

	//Calculate the change in orientation.
	quatIntegrate(&body->rot, &body->angularVelocity, dt);
	//Don't forget to normalize it, as
	//this process can introduce errors.
	quatNormalizeQuat(&body->rot);*/
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
	physicsCollider *lastCollider;


	/** update centroid **/
	/** update inverse inertia tensor **/

	//Update the body's velocity.
	physRigidBodyIntegrateVelocitySymplecticEuler(body, dt);


	curCollider = body->colliders;
	lastCollider = &body->colliders[body->numColliders - 1];
	//For every physics collider that is a part of
	//this rigid body, we will need to update its
	//base collider and its node in the broadphase.
	for(; curCollider < lastCollider; ++curCollider){
		physColliderUpdate(curCollider, NULL);
	}
}


/*
** Sum the mass and centroid of each of the body's
** colliders in order to find the combined centroid.
** This function will also calculate the inverse mass.
*/
/*void physRigidBodyDefCalculateCentroid(physicsRigidBodyDef *bodyDef){
	const physicsCollider *curCollider = bodyDef->colliders;
	const physicsCollider *lastCollider = &bodyDef->colliders[bodyDef->numColliders];

	bodyDef->mass = 0.f;
	vec3InitZero(&bodyDef->centroid);

	//Get the physical properties of each collider.
	for(; curCollider < lastCollider; ++curCollider){
		//Non-weighted centroid.
		//Accumulate the mass.
		bodyDef->mass += curCollider->mass;
		//Add this collider's contribution to the rigid body's centroid.
		vec3AddVec3(&bodyDef->centroid, &curCollider->centroid);

		//Weighted centroid.
		//const float colliderMass = curCollider->mass;
		//vec3 weightedCentroid;
		//vec3MultiplySOut(&curCollider->centroid, colliderMass, &weightedCentroid);
		//
		////Accumulate the mass.
		//bodyDef->mass += colliderMass;
		////Add this collider's contribution to the rigid body's centroid.
		//vec3AddVec3(&bodyDef->centroid, &weightedCentroid);
	}

	//We don't want to divide by zero.
	if(bodyDef->mass != 0.f){
		bodyDef->invMass = 1.f / bodyDef->mass;
		vec3MultiplyS(&bodyDef->centroid, bodyDef->invMass);
	}else{
		bodyDef->invMass = 0.f;
	}
}*/

/*
** Sum each collider's inertia tensor and invert it
** to calculate the combined inverse inertia tensor.
*/
/*void physRigidBodyDefCalculateInertiaTensor(physicsRigidBodyDef *bodyDef){
	const physicsCollider *curCollider = bodyDef->colliders;
	const physicsCollider *lastCollider = &bodyDef->colliders[bodyDef->numColliders];
	//Get the physical properties of each collider.
	for(; curCollider < lastCollider; ++curCollider){
		float inertia[6];
		colliderCalculateInertia(curCollider, &bodyDef->centroid, inertia);

		//Add the collider's contribution to the rigid body's inertia tensor.
		bodyDef->invInertia.m[0][0] += inertia[0];
		bodyDef->invInertia.m[1][1] += inertia[1];
		bodyDef->invInertia.m[2][2] += inertia[2];
		bodyDef->invInertia.m[0][1] += inertia[3];
		bodyDef->invInertia.m[0][2] += inertia[4];
		bodyDef->invInertia.m[1][2] += inertia[5];*/


		/*vec3 centroidDist;
		vec3SubtractVec3FromOut(&bodyDef->centroid, &curCollider->centroid, &centroidDist);
		//We can multiply by mass here to save three multiplications later.
		vec3MultiplyS(&centroidDist, &curCollider->mass);

		const float x = centroidDist.x;
		const float y = centroidDist.y;
		const float z = centroidDist.z;
		const float xx = x * x;
		const float yy = y * y;
		const float zz = z * z;

		//Add this collider's moment of inertia tensor to the rigid body's.
		//Note that in order to do this correctly, we must use the
		//Parallel Axis Theorem to shift it to the rigid body's centroid.
		//
		//I = I_c + m * (d^2 * i - out(r, r))
		//
		//I   = new inertia tensor
		//I_c = current collider's inertia tensor
		//m   = current collider's mass
		//d   = scalar distance between the two centroids
		//i   = identity matrix
		//r   = vector distance between the two centroids
		//
		//If we define x, y and z as the values of r,
		//we can multiply m by the following matrix:
		//
		//[(yy + zz)    -xy       -xz   ]
		//[   -yx    (xx + zz)    -yz   ]
		//[   -zx       -zy    (xx + yy)]
		bodyDef->invInertia.m[0][0] += curCollider->inertia.m[0][0] + (yy + zz);
		bodyDef->invInertia.m[0][1] += curCollider->inertia.m[0][1] - (x * y);
		bodyDef->invInertia.m[0][2] += curCollider->inertia.m[0][2] - (x * z);
		bodyDef->invInertia.m[1][1] += curCollider->inertia.m[1][1] + (xx + zz);
		bodyDef->invInertia.m[1][2] += curCollider->inertia.m[1][2] - (y * z);
		bodyDef->invInertia.m[2][2] += curCollider->inertia.m[2][2] + (xx + yy);*/
	/*}

	//These should be the same as the values we've already calculated.
	bodyDef->invInertia.m[1][0] = bodyDef->invInertia.m[0][1];
	bodyDef->invInertia.m[2][0] = bodyDef->invInertia.m[0][2];
	bodyDef->invInertia.m[2][1] = bodyDef->invInertia.m[1][2];

	//The inverse inertia tensor is more useful to us than the regular one.
	mat3Invert(&bodyDef->invInertia, &bodyDef->invInertia);
}*/