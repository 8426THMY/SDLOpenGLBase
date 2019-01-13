#include "physicsContact.h"


#define PHYSCONTACT_BAUMGARTE_BIAS   0.4f
#define PHYSCONTACT_PENETRATION_SLOP 0.05f


#include "utilMath.h"

#include "mat3.h"

#include "physicsRigidBody.h"


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the impulse denominator and the
** bias term.
*/
void physManifoldPresolve(physicsManifold *pm, const float dt){
	//There is no need to calculate this
	//stuff for every contact point.
	const float invMass = pm->bodyA->body->invMass + pm->bodyB->body->invMass;
	mat3 *invInertiaA = &pm->bodyA->invInertia;
	mat3 *invInertiaB = &pm->bodyB->invInertia;
	const float baumgarte = -PHYSCONTACT_BAUMGARTE_BIAS * (1.f / dt);

	physicsContact *curContact = pm->contacts;
	physicsContact *lastContact = &pm->contacts[pm->numContacts];

	//Set the tangent vectors such that they form an
	//orthonormal basis together with the contact normal.
	normalBasis(&pm->normal, &pm->tangents[0], &pm->tangents[1]);


	//Loop through every contact point in the manifold,
	//calculating their impulse denominators and bias terms.
	for(; curContact < lastContact; ++curContact){
		//rA X n
		vec3 rAX;
		//IA * (rA X n)
		vec3 rAXI;
		//rB X n
		vec3 rBX;
		//IB * (rB X n)
		vec3 rBXI;
		const float newPenetration = curContact->penetration + PHYSCONTACT_PENETRATION_SLOP;

		//Calculate the normal impulse denominator.
		vec3CrossVec3(&curContact->rA, &pm->normal, &rAX);
		vec3CrossVec3(&curContact->rB, &pm->normal, &rBX);
		mat3MultiplyVec3R(invInertiaA, &rAX, &rAXI);
		mat3MultiplyVec3R(invInertiaB, &rBX, &rBXI);
		curContact->impulseDenom = 1.f / (invMass + vec3DotVec3(&rAX, &rAXI) + vec3DotVec3(&rBX, &rBXI));

		//Calculate the first tangent impulse denominator.
		vec3CrossVec3(&pm->tangents[0], &curContact->rA, &rAX);
		vec3CrossVec3(&pm->tangents[0], &curContact->rB, &rBX);
		mat3MultiplyVec3R(invInertiaA, &rAX, &rAXI);
		mat3MultiplyVec3R(invInertiaB, &rBX, &rBXI);
		curContact->frictionDenom[0] = 1.f / (invMass + vec3DotVec3(&rAX, &rAXI) + vec3DotVec3(&rBX, &rBXI));

		//Calculate the second tangent impulse denominator.
		vec3CrossVec3(&pm->tangents[1], &curContact->rA, &rAX);
		vec3CrossVec3(&pm->tangents[1], &curContact->rB, &rBX);
		mat3MultiplyVec3R(invInertiaA, &rAX, &rAXI);
		mat3MultiplyVec3R(invInertiaB, &rBX, &rBXI);
		curContact->frictionDenom[1] = 1.f / (invMass + vec3DotVec3(&rAX, &rAXI) + vec3DotVec3(&rBX, &rBXI));


		//Calculate the bias term (restitution and Baumgarte term).
		curContact->bias = (newPenetration > 0.f) ? (baumgarte * newPenetration) : 0.f;

		/**
		q3ContactState *c = cs->contacts + j;

		// Precalculate JM^-1JT forcontact and friction constraints
		q3Vec3 raCn = q3Cross(c->ra, cs->normal);
		q3Vec3 rbCn = q3Cross(c->rb, cs->normal);
		r32 nm = cs->mA + cs->mB;
		r32 tm[2];
		tm[0] = nm;
		tm[1] = nm;

		nm += q3Dot(raCn, cs->iA * raCn) + q3Dot(rbCn, cs->iB * rbCn);
		c->normalMass = q3Invert(nm);

		for(i32 i = 0; i < 2; ++i){
			q3Vec3 raCt = q3Cross(cs->tangentVectors[i], c->ra);
			q3Vec3 rbCt = q3Cross(cs->tangentVectors[i], c->rb);
			tm[i] += q3Dot(raCt, cs->iA * raCt) + q3Dot(rbCt, cs->iB * rbCt);
			c->tangentMass[i] = q3Invert(tm[i]);
		}

		// Precalculate bias factor
		c->bias = -Q3_BAUMGARTE * (r32(1.0) / dt) * q3Min(r32(0.0), c->penetration + Q3_PENETRATION_SLOP);

		// Warm start contact
		q3Vec3 P = cs->normal * c->normalImpulse;

		if(m_enableFriction){
			P += cs->tangentVectors[0] * c->tangentImpulse[0];
			P += cs->tangentVectors[1] * c->tangentImpulse[1];
		}

		vA -= P * cs->mA;
		wA -= cs->iA * q3Cross(c->ra, P);

		vB += P * cs->mB;
		wB += cs->iB * q3Cross(c->rb, P);

		// Add in restitution bias
		r32 dv = q3Dot(vB + q3Cross(wB, c->rb) - vA - q3Cross(wA, c->ra), cs->normal);

		if(dv < -r32(1.0))
			c->bias += -(cs->restitution) * dv;
		**/
	}
}

/*
** Apply an impulse to penetrating physics objects to ensure
** that they separate from each other on the next frame. This
** should be called directly after "manifoldPresolve", and may
** be called multiple times if we are using sequential impulse.
*/
void physManifoldSolve(physicsManifold *pm){
	physicsContact *curContact = pm->contacts;
	physicsContact *lastContact = &pm->contacts[pm->numContacts];
	for(; curContact < lastContact; ++curContact){
		//
	}
}
/**
//Find the objects' torques.
//What we're doing here is basically "I^-1 * cross(r, n)^2".
var torqueSqrA:Number = crossProduct(rA, currentPair.contactNormal);
var torqueA:Number = currentPair.objA.inverseInertiaScalar * torqueSqrA;
torqueSqrA *= torqueA;
var torqueSqrB:Number = crossProduct(rB, currentPair.contactNormal);
var torqueB:Number = currentPair.objB.inverseInertiaScalar * torqueSqrB;
torqueSqrB *= torqueB;
//We can also use "cross(I^-1 * cross(r, n), r)", and doing it
//this way should be equivalent, but it's clearly not as efficient.
//Note that if we do it this way, we need to find the dot
//product of the two torque points' sum and the contact normal.
var torqueA:Number = currentPair.objA.inverseInertiaScalar * crossProduct(rA, currentPair.contactNormal);
var torqueSqrA:Point = crossProduct3D(torqueA, rA);
var torqueB:Number = currentPair.objB.inverseInertiaScalar * crossProduct(rB, currentPair.contactNormal);
var torqueSqrB:Point = crossProduct3D(torqueB, rB);
**/
/**
Currently understand all of the presolve stuff except for this:

// Precalculate JM^-1JT forcontact and friction constraints
q3Vec3 raCn = q3Cross(c->ra, cs->normal);
q3Vec3 rbCn = q3Cross(c->rb, cs->normal);
r32 nm = cs->mA + cs->mB;
r32 tm[2];
tm[0] = nm;
tm[1] = nm;

nm += q3Dot(raCn, cs->iA * raCn) + q3Dot(rbCn, cs->iB * rbCn);
c->normalMass = q3Invert(nm);

for(i32 i = 0; i < 2; ++i){
	q3Vec3 raCt = q3Cross(cs->tangentVectors[i], c->ra);
	q3Vec3 rbCt = q3Cross(cs->tangentVectors[i], c->rb);
	tm[i] += q3Dot(raCt, cs->iA * raCt) + q3Dot(rbCt, cs->iB * rbCt);
	c->tangentMass[i] = q3Invert(tm[i]);
}
**/

/*
#define Q3_SLEEP_LINEAR r32( 0.01 )

#define Q3_SLEEP_ANGULAR r32( (3.0 / 180.0) * q3PI )

#define Q3_SLEEP_TIME r32( 0.5 )

#define Q3_BAUMGARTE r32( 0.2 )

#define Q3_PENETRATION_SLOP r32( 0.05 )
*/

/*
void q3ContactSolver::PreSolve(r32 dt){
	for(i32 i = 0; i < m_contactCount; ++i){
		q3ContactConstraintState *cs = m_contacts + i;

		q3Vec3 vA = m_velocities[cs->indexA].v;
		q3Vec3 wA = m_velocities[cs->indexA].w;
		q3Vec3 vB = m_velocities[cs->indexB].v;
		q3Vec3 wB = m_velocities[cs->indexB].w;

		for(i32 j = 0; j < cs->contactCount; ++j){
			q3ContactState *c = cs->contacts + j;

			// Precalculate JM^-1JT forcontact and friction constraints
			q3Vec3 raCn = q3Cross(c->ra, cs->normal);
			q3Vec3 rbCn = q3Cross(c->rb, cs->normal);
			r32 nm = cs->mA + cs->mB;
			r32 tm[2];
			tm[0] = nm;
			tm[1] = nm;

			nm += q3Dot(raCn, cs->iA * raCn) + q3Dot(rbCn, cs->iB * rbCn);
			c->normalMass = q3Invert(nm);

			for(i32 i = 0; i < 2; ++i){
				q3Vec3 raCt = q3Cross(cs->tangentVectors[i], c->ra);
				q3Vec3 rbCt = q3Cross(cs->tangentVectors[i], c->rb);
				tm[i] += q3Dot(raCt, cs->iA * raCt) + q3Dot(rbCt, cs->iB * rbCt);
				c->tangentMass[i] = q3Invert(tm[i]);
			}

			// Precalculate bias factor
			c->bias = -Q3_BAUMGARTE * (r32(1.0) / dt) * q3Min(r32(0.0), c->penetration + Q3_PENETRATION_SLOP);

			// Warm start contact
			q3Vec3 P = cs->normal * c->normalImpulse;

			if(m_enableFriction){
				P += cs->tangentVectors[0] * c->tangentImpulse[0];
				P += cs->tangentVectors[1] * c->tangentImpulse[1];
			}

			vA -= P * cs->mA;
			wA -= cs->iA * q3Cross(c->ra, P);

			vB += P * cs->mB;
			wB += cs->iB * q3Cross(c->rb, P);

			// Add in restitution bias
			r32 dv = q3Dot(vB + q3Cross(wB, c->rb) - vA - q3Cross(wA, c->ra), cs->normal);

			if(dv < -r32(1.0))
				c->bias += -(cs->restitution) * dv;
		}

		m_velocities[cs->indexA].v = vA;
		m_velocities[cs->indexA].w = wA;
		m_velocities[cs->indexB].v = vB;
		m_velocities[cs->indexB].w = wB;
	}
}

//--------------------------------------------------------------------------------------------------
void q3ContactSolver::Solve(){
	for(i32 i = 0; i < m_contactCount; ++i){
		q3ContactConstraintState *cs = m_contacts + i;

		q3Vec3 vA = m_velocities[cs->indexA].v;
		q3Vec3 wA = m_velocities[cs->indexA].w;
		q3Vec3 vB = m_velocities[cs->indexB].v;
		q3Vec3 wB = m_velocities[cs->indexB].w;

		for(i32 j = 0; j < cs->contactCount; ++j){
			q3ContactState *c = cs->contacts + j;

			// relative velocity at contact
			q3Vec3 dv = vB + q3Cross(wB, c->rb) - vA - q3Cross(wA, c->ra);

			// Friction
			if(m_enableFriction){
				for(i32 i = 0; i < 2; ++i){
					r32 lambda = -q3Dot(dv, cs->tangentVectors[i]) * c->tangentMass[i];

					// Calculate frictional impulse
					r32 maxLambda = cs->friction * c->normalImpulse;

					// Clamp frictional impulse
					r32 oldPT = c->tangentImpulse[i];
					c->tangentImpulse[i] = q3Clamp(-maxLambda, maxLambda, oldPT + lambda);
					lambda = c->tangentImpulse[i] - oldPT;

					// Apply friction impulse
					q3Vec3 impulse = cs->tangentVectors[i] * lambda;
					vA -= impulse * cs->mA;
					wA -= cs->iA * q3Cross(c->ra, impulse);

					vB += impulse * cs->mB;
					wB += cs->iB * q3Cross(c->rb, impulse);
				}
			}

			// Normal
			{
				dv = vB + q3Cross(wB, c->rb) - vA - q3Cross(wA, c->ra);

				// Normal impulse
				r32 vn = q3Dot(dv, cs->normal);

				// Factor in positional bias to calculate impulse scalar j
				r32 lambda = c->normalMass * (-vn + c->bias);

				// Clamp impulse
				r32 tempPN = c->normalImpulse;
				c->normalImpulse = q3Max(tempPN + lambda, r32(0.0));
				lambda = c->normalImpulse - tempPN;

				// Apply impulse
				q3Vec3 impulse = cs->normal * lambda;
				vA -= impulse * cs->mA;
				wA -= cs->iA * q3Cross(c->ra, impulse);

				vB += impulse * cs->mB;
				wB += cs->iB * q3Cross(c->rb, impulse);
			}
		}

		m_velocities[cs->indexA].v = vA;
		m_velocities[cs->indexA].w = wA;
		m_velocities[cs->indexB].v = vB;
		m_velocities[cs->indexB].w = wB;
	}
}


void Manifold::ApplyImpulse(){
	for(ContactPoint& contact: contactPoints){
		SolveContactPoint(contact);
	}
}

void Manifold::SolveContactPoint(ContactPoint& c){
	Vector3 r1 = c.relPosA;
	Vector3 r2 = c.relPosB;

	Vector3 v0 = pnodeA->GetLinearVelocity()
		+ Vector3::Cross(pnodeA->GetAngularVelocity(), r1);
	Vector3 v1 = pnodeB->GetLinearVelocity()
		+ Vector3::Cross(pnodeB->GetAngularVelocity(), r2);

	Vector3 dv = v1 - v0;

	// Collision Resolution

	float constraintMass =
		(pnodeA ->GetInverseMass() + pnodeB ->GetInverseMass()) +
		Vector3::Dot(c.colNormal,
		Vector3::Cross(pnodeA ->GetInverseInertia()
		* Vector3::Cross(r1, c.colNormal), r1) +
		Vector3::Cross(pnodeB ->GetInverseInertia()
		* Vector3::Cross(r2, c.colNormal), r2));

	if(constraintMass > 0.0f){
		float jn = max(-Vector3::Dot(dv, c.colNormal) + c.b_term, 0.0f);
		jn = jn / constraintMass;

		pnodeA ->SetLinearVelocity(pnodeA ->GetLinearVelocity()
			- c.colNormal *(jn * pnodeA ->GetInverseMass()));
		pnodeB ->SetLinearVelocity(pnodeB ->GetLinearVelocity()
			+ c.colNormal *(jn * pnodeB ->GetInverseMass()));

		pnodeA ->SetAngularVelocity(pnodeA ->GetAngularVelocity()
			- pnodeA ->GetInverseInertia()
			* Vector3::Cross(r1, c.colNormal * jn));
		pnodeB ->SetAngularVelocity(pnodeB ->GetAngularVelocity()
			+ pnodeB ->GetInverseInertia()
			* Vector3::Cross(r2, c.colNormal * jn));
	}

	// Friction
	Vector3 tangent = dv - c.colNormal * Vector3::Dot(dv, c.colNormal);
	float tangent_len = tangent.Length();

	if(tangent_len > 1e-6f){
		tangent = tangent / tangent_len;
		float frictionalMass =(pnodeA->GetInverseMass()
			+ pnodeB->GetInverseMass()) + Vector3::Dot(tangent,
			Vector3::Cross(pnodeA->GetInverseInertia()
			* Vector3::Cross(r1, tangent), r1) +
			Vector3::Cross(pnodeB->GetInverseInertia()
			* Vector3::Cross(r2, tangent), r2));

		if(frictionalMass > 0.0f){
			float frictionCoef =
				(pnodeA->GetFriction() * pnodeB->GetFriction());
			float jt = -Vector3::Dot(dv, tangent) * frictionCoef;

			jt = jt / frictionalMass;

			pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
				- tangent *(jt * pnodeA->GetInverseMass()));
			pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
				+ tangent *(jt * pnodeB->GetInverseMass()));

			pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				- pnodeA->GetInverseInertia()
				* Vector3::Cross(r1, tangent * jt));
			pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				+ pnodeB->GetInverseInertia()
				* Vector3::Cross(r2, tangent * jt));
		}
	}
}


// After c.sumImpulseFriction = (This is used in the  next  tutorial)

//  Baumgarte  Offset (Adds  energy  to the  system  to  counter  slight
//  solving  errors  that  accumulate  over  time - known as ’constraint
// drift ’)

// Very  slightly  different  to the  one we used to  prevent  the  distance
//  constraints  breaking , as we now  allow a minimum  allowed  error.

// In this case , we  allow  the  objects  to  overlap  by 1mm  before  adding
// in  correctional  energy  to  counteract  the error , this is a little
//  dirty  trick  that  results  in us  always  getting a manifold  for
//  constantly  colliding  objects  which  normally  we  would  get  collide
// one  frame  and  not  the next , endlessly  jittering. If you’re
//  interested , the  _slop  part (and  this  issue in  general) is  usually
//  handled  in  physics  engines  by  pretending  the  collision  volume  is
//  larger  than it is  during  the  narrowphase , so  manifolds  are
//  generated  for  colliding , and  almost  colliding  pairs.

const  float  baumgarte_scalar = 0.1f;
const  float  baumgarte_slop = 0.001f;
const  float  penetration_slop =
	min(c.colPenetration + baumgarte_slop , 0.0f);

c.b_term  +=
	-(baumgarte_scalar / PhysicsEngine :: Instance()->GetDeltaTime ())
	* penetration_slop;

// Compute  Elasticity  Term

// This is the  total  velocity  going  into  the  collision  relative  to the
//  collision  normal , as  elasticity  is ’adding ’ energy  back  into  the
//  system  we can  attach  it to our ’b’ term  which we  already  add to
// ’jt’ when  solving  the  contact  constraint.

const  float  elasticity =
pnodeA ->GetElasticity () * pnodeB ->GetElasticity ();
const  float  elatisity_term = Vector3 ::Dot(c.colNormal ,

pnodeA ->GetLinearVelocity ()
	+ Vector3 :: Cross(c.relPosA , pnodeA ->GetAngularVelocity ())
	- pnodeB ->GetLinearVelocity ()
	- Vector3 :: Cross(c.relPosB , pnodeB ->GetAngularVelocity ()));

c.b_term  += (elasticity * elatisity_term) / contactPoints.size ();
*/