#include "physicsRigidBody.h"


void rigidBodyDefInit(physicsRigidBodyDef *body){
	//rigidBodyDefCalculateCentroid(body);
	//rigidBodyDefCalculateInertiaTensor(body);
}


/*
** Calculate the body's increase in
** velocity for the current timestep.
**
** v^(n + 1) = v^n + F * m^-1 * dt
** w^(n + 1) = w^n + T * I^-1 * dt
*/
void rigidBodyIntegrateVelocitySymplecticEuler(physicsRigidBody *rb, const float dt){
	vec3 linearAcceleration;
	vec3 angularAcceleration;

	//Calculate the body's linear acceleration.
	vec3MultiplyS(&rb->netForce, rb->body->invMass * dt, &linearAcceleration);
	//Add the linear acceleration to the linear velocity.
	vec3AddVec3(&rb->linearVelocity, &linearAcceleration, &rb->linearVelocity);

	//Calculate the body's angular acceleration.
	vec3MultiplyS(&rb->netTorque, dt, &angularAcceleration);
	mat3MultiplyVec3(&rb->invInertia, &angularAcceleration, &angularAcceleration);
	//Add the angular acceleration to the angular velocity.
	vec3AddVec3(&rb->angularVelocity, &angularAcceleration, &rb->angularVelocity);
}

/*
** Calculate the body's change in
** position for the current timestep.
**
** x^(n + 1) = x^n + v^(n + 1) * dt
** dq/dt = 0.5 * w * q
** q^(n + 1) = q^n + dq/dt * dt
*/
void rigidBodyIntegratePositionSymplecticEuler(physicsRigidBody *rb, const float dt){
	vec3 linearVelocityDelta;

	vec3MultiplyS(&rb->linearVelocity, dt, &linearVelocityDelta);
	//Compute the object's new position.
	vec3AddVec3(&rb->pos, &linearVelocityDelta, &rb->pos);

	//Calculate the change in orientation.
	quatIntegrate(&rb->rot, &rb->angularVelocity, dt, &rb->rot);
	//Don't forget to normalize it, as
	//this process can introduce errors.
	quatNormalizeQuat(&rb->rot, &rb->rot);
}


/*
** Sum the mass and centroid of each of the body's
** colliders in order to find the combined centroid.
** This function will also calculate the inverse mass.
*/
/*void rigidBodyDefCalculateCentroid(physicsRigidBodyDef *body){
	const physicsCollider *curCollider = body->colliders;
	const physicsCollider *lastCollider = &body->colliders[body->numColliders];

	body->mass = 0.f;
	vec3InitZero(&body->centroid);

	//Get the physical properties of each collider.
	for(; curCollider < lastCollider; ++curCollider){
		//Non-weighted centroid.
		//Accumulate the mass.
		body->mass += curCollider->mass;
		//Add this collider's contribution to the rigid body's centroid.
		vec3AddVec3(&body->centroid, &curCollider->centroid, &body->centroid);

		//Weighted centroid.
		//const float colliderMass = curCollider->mass;
		//vec3 weightedCentroid;
		//vec3MultiplyS(&curCollider->centroid, colliderMass, &weightedCentroid);
		//
		////Accumulate the mass.
		//body->mass += colliderMass;
		////Add this collider's contribution to the rigid body's centroid.
		//vec3AddVec3(&body->centroid, &weightedCentroid, &body->centroid);
	}

	//We don't want to divide by zero.
	if(body->mass != 0.f){
		body->invMass = 1.f / body->mass;
		vec3MultiplyS(&body->centroid, body->invMass, &body->centroid);
	}else{
		body->invMass = 0.f;
	}
}*/

/*
** Sum each collider's inertia tensor and invert it
** to calculate the combined inverse inertia tensor.
*/
/*void rigidBodyDefCalculateInertiaTensor(physicsRigidBodyDef *body){
	const physicsCollider *curCollider = body->colliders;
	const physicsCollider *lastCollider = &body->colliders[body->numColliders];
	//Get the physical properties of each collider.
	for(; curCollider < lastCollider; ++curCollider){
		float inertia[6];
		colliderCalculateInertia(curCollider, &body->centroid, inertia);

		//Add the collider's contribution to the rigid body's inertia tensor.
		body->invInertia.m[0][0] += inertia[0];
		body->invInertia.m[1][1] += inertia[1];
		body->invInertia.m[2][2] += inertia[2];
		body->invInertia.m[0][1] += inertia[3];
		body->invInertia.m[0][2] += inertia[4];
		body->invInertia.m[1][2] += inertia[5];*/


		/*vec3 centroidDist;
		vec3SubtractVec3From(&body->centroid, &curCollider->centroid, &centroidDist);
		//We can multiply by mass here to save three multiplications later.
		vec3MultiplyS(&centroidDist, &curCollider->mass, &centroidDist);

		const float x = centroidDist.x;
		const float y = centroidDist.y;
		const float z = centroidDist.z;
		const float xx = x * x;
		const float yy = y * y;
		const float zz = z * z;

		//Add this collider's moment of inertia tensor to the rigid body's.
		//Note that in order to do this correctly, we must shift use the
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
		body->invInertia.m[0][0] += curCollider->inertia.m[0][0] + (yy + zz);
		body->invInertia.m[0][1] += curCollider->inertia.m[0][1] - (x * y);
		body->invInertia.m[0][2] += curCollider->inertia.m[0][2] - (x * z);
		body->invInertia.m[1][1] += curCollider->inertia.m[1][1] + (xx + zz);
		body->invInertia.m[1][2] += curCollider->inertia.m[1][2] - (y * z);
		body->invInertia.m[2][2] += curCollider->inertia.m[2][2] + (xx + yy);*/
	/*}

	//These should be the same as the values we've already calculated.
	body->invInertia.m[1][0] = body->invInertia.m[0][1];
	body->invInertia.m[2][0] = body->invInertia.m[0][2];
	body->invInertia.m[2][1] = body->invInertia.m[1][2];

	//The inverse inertia tensor is more useful to us than the regular one.
	mat3Invert(&body->invInertia, &body->invInertia);
}*/