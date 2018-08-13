#include "physicsRigidBody.h"


void rbInitProperties(physicsRigidBody *body){
	rbCalculateCentroid(body);
	rbCalculateInertiaTensor(body);
}


void rbInstIntegrateVelocitySymplecticEuler(physRigidBodyInst *rb, const float time){
	vec3 linearAcceleration;
	//Calculate the body's linear acceleration.
	vec3MultiplyS(&rb->netForce, rb->body->inverseMass * time, &linearAcceleration);
	//Add the linear acceleration to the linear velocity.
	vec3AddVec3(&rb->linearVelocity, &linearAcceleration, &rb->linearVelocity);

	vec3 angularAcceleration;
	//Calculate the body's angular acceleration.
	vec3MultiplyS(&rb->netTorque, time, &angularAcceleration);
	mat3MultiplyVec3(&rb->inverseInertiaTensor, &angularAcceleration, &angularAcceleration);
	//Add the angular acceleration to the angular velocity.
	vec3AddVec3(&rb->angularVelocity, &angularAcceleration, &rb->angularVelocity);
}

void rbInstIntegratePositionSymplecticEuler(physRigidBodyInst *rb, const float time){
	vec3 linearVelocityDelta;
	vec3MultiplyS(&rb->linearVelocity, time, &linearVelocityDelta);
	//Compute the object's new position.
	vec3AddVec3(&rb->pos, &linearVelocityDelta, &rb->pos);

	quat angularVelocityDelta;
	//Calculate the change in orientation.
	const float halfTime = time * 0.5f;
	angularVelocityDelta.x = rb->angularVelocity.x * halfTime;
	angularVelocityDelta.y = rb->angularVelocity.y * halfTime;
	angularVelocityDelta.z = rb->angularVelocity.z * halfTime;
	angularVelocityDelta.w = 0.f;
	quatMultiplyQuat(&angularVelocityDelta, &rb->rot, &angularVelocityDelta);
	//Compute the object's new orientation.
	quatAddVec4(&rb->rot, &angularVelocityDelta, &rb->rot);
	//Don't forget to normalize it, as this method can introduce errors.
	quatNormalizeQuat(&rb->rot, &rb->rot);
}


void rbCalculateCentroid(physicsRigidBody *body){
	body->mass = 0.f;
	vec3InitZero(&body->centroid);

	size_t i;
	for(i = 0; i < body->numColliders; ++i){
		//Get the physical properties of the current collider.
		const physicsCollider *curCollider = &body->colliders[i];

		//Accumulate the mass.
		body->mass += curCollider->mass;
		//Add this collider's contribution to the rigid body's centroid.
		vec3AddVec3(&body->centroid, &curCollider->centroid, &body->centroid);


		/*//Get the physical properties of the current collider.
		const physicsCollider *curCollider = &body->colliders[i];
		const float colliderMass = curCollider->mass;
		vec3 weightedCentroid;
		vec3MultiplyS(&curCollider->centroid, colliderMass, &weightedCentroid);

		//Accumulate the mass.
		body->mass += colliderMass;
		//Add this collider's contribution to the rigid body's centroid.
		vec3AddVec3(&body->centroid, &weightedCentroid, &body->centroid);*/
	}

	//We don't want to divide by zero.
	if(body->mass != 0.f){
		body->inverseMass = 1.f / body->mass;
		vec3MultiplyS(&body->centroid, body->inverseMass, &body->centroid);
	}else{
		body->inverseMass = 0.f;
	}
}

void rbCalculateInertiaTensor(physicsRigidBody *body){
	size_t i;
	for(i = 0; i < body->numColliders; ++i){
		//Get the physical properties of the current collider.
		float inertiaTensor[6];
		colliderCalculateInertia(&body->colliders[i], &body->centroid, inertiaTensor);

		//Add the collider's contribution to the rigid body's inertia tensor.
		body->inverseInertiaTensor.m[0][0] += inertiaTensor[0];
		body->inverseInertiaTensor.m[1][1] += inertiaTensor[1];
		body->inverseInertiaTensor.m[2][2] += inertiaTensor[2];
		body->inverseInertiaTensor.m[0][1] += inertiaTensor[3];
		body->inverseInertiaTensor.m[0][2] += inertiaTensor[4];
		body->inverseInertiaTensor.m[1][2] += inertiaTensor[5];


		/*//Get the physical properties of the current collider.
		const physicsCollider *curCollider = &body->colliders[i];
		vec3 centroidDist;
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
		body->inverseInertiaTensor.m[0][0] += curCollider->inertiaTensor.m[0][0] + (yy + zz);
		body->inverseInertiaTensor.m[0][1] += curCollider->inertiaTensor.m[0][1] - (x * y);
		body->inverseInertiaTensor.m[0][2] += curCollider->inertiaTensor.m[0][2] - (x * z);
		body->inverseInertiaTensor.m[1][1] += curCollider->inertiaTensor.m[1][1] + (xx + zz);
		body->inverseInertiaTensor.m[1][2] += curCollider->inertiaTensor.m[1][2] - (y * z);
		body->inverseInertiaTensor.m[2][2] += curCollider->inertiaTensor.m[2][2] + (xx + yy);*/
	}

	//These should be the same as the values we've already calculated.
	body->inverseInertiaTensor.m[1][0] = body->inverseInertiaTensor.m[0][1];
	body->inverseInertiaTensor.m[2][0] = body->inverseInertiaTensor.m[0][2];
	body->inverseInertiaTensor.m[2][1] = body->inverseInertiaTensor.m[1][2];

	//The inverse inertia tensor is more useful to us than the regular one.
	mat3Invert(&body->inverseInertiaTensor, &body->inverseInertiaTensor);
}