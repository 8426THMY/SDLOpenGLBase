#include "physicsJointRevolute.h"


#include "physicsRigidBody.h"


/*
** ----------------------------------------------------------------------
**
** Fixed joints constrain the positions of the anchor points,
** rA and rB, to be coincident. We also constraint the orientations
** (excluding the offsets RA and RB) to be the same.
**
** ----------------------------------------------------------------------
**
** The linear constraint is identical to that of the spherical joint.
** That is, we just want the difference between the anchors to be 0:
**
** C1 : (pB + rB) - (pA + rA) = 0.
**
** Differentiate with respect to time to get a velocity constraint:
**
** C1' : 0 = (vB +   wB X rB) - (vA +   wA X rA)
**         = (vB -   rB X wB) - (vA -   rA X wA)
**         = (vB - [rB]_X*wB) - (vA - [rA]_X*wA),
**
** where "[.]_X" denotes the skew-symmetric "cross product" matrix.
**
**
** We also need to impose constraints on the relative orientation
** of the rigid bodies. If qA, qB are the orientations and RA, RB
** the rotational offsets of the rigid bodies, then the relative
** orientation is given by qR = (qB*RB) * conj(qA*RA). If qI is
** the identity quaternion, then we want
**
** C2 : 2proj(qR) = 0.
**
** Here, proj(qR) is the projection of qR onto the imaginary ik plane:
**
**           [0, 1, 0, 0]
** proj(q) = [0, 0, 0, 1]q = (q_x, q_z)^T.
**
** This projection is 0 if and only if qR is a rotation about the y-axis.
** Note also the extra factor of 2: this is merely for convenience,
** and vanishes when taking derivatives:
**
** C2' : 0 = 2proj(((d/dt)(qB*RB)) * conj(qA*RA) + (qB*RB) * ((d/dt)conj(qA*RA)))
**         = 2proj(d(qB)/dt * RB * conj(qA*RA) + (qB*RB) * conj(RA) * conj(d(qA)/dt))
**         = proj(wB * (qB*RB)*conj(qA*RA) + (qB*RB)*conj(qA*RA) * conj(wA))
**         = proj(wB*qR - qR*wA)
**         = proj(wB*qR) - proj(qR*wA).
**
** Here, it should be noted that wA, wB are being treated as
** quaternions with real part 0. Now, let q be a unit quaternion
** and v a vector. Similarly to how we handle the cross-products,
** we define the 3x3 matrices Q_L(q), Q_R(q) such that
**
** Q_L(q)v = proj(q * (i v_x + j v_y + k v_z)),
** Q_R(q)v = proj((i v_x + j v_y + k v_z) * q).
**
** Note that the quaternion product (left multiplication by q)
** can be represented by the following 4x3 matrix:
**
** [-q_x, -q_y, -q_z]
** [ q_w, -q_z,  q_y]
** [ q_z,  q_w, -q_x]
** [-q_y,  q_x,  q_w]
**
** The projection matrix is 2x4 and simply removes the first and
** third rows (corresponding to the real and j components of the
** product). It is easy to show that the resulting matrices are
** hence given by
**
**          [ q_w, -q_z,  q_y]
** Q_L(q) = [-q_y,  q_x,  q_w],
**
**          [ q_w,  q_z, -q_y]
** Q_R(q) = [ q_y, -q_x,  q_w].
**
** Using this notation, our velocity constraint can be written as
**
** C2' : Q_R(qR)wB - Q_L(qR)wA = 0.
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
** and the identity C' = JV, we can solve for the Jacobian J.
** We will use two separate Jacobians, J1 and J2, for C1' and C2'.
**
** J1 = [-I_3,   [rA]_X, I_3, -[rB]_X],
** J2 = [   0, -Q_L(qR),   0, Q_R(qR)].
**
** Note that "I_3" is the 3x3 identity matrix.
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
**        [ -I_3  ]
**        [-[rA]_X]
** J1^T = [  I_3  ].
**        [ [rB]_X]
**
**
** Evaluating this expression gives us the following matrix for our
** point-to-point constraint:
**
**               [-mA^{-1} * I_3   ]
**               [-IA^{-1} * [rA]_X]
** M^{-1}*J1^T = [ mB^{-1} * I_3   ],
**               [ IB^{-1} * [rB]_X]
**
** K1 = J1*M^{-1}*J1^T
**    = (mA^{-1} + mB^{-1})*I_3 - [rA]_X*IA^{-1}*[rA]_X - [rB]_X*IB^{-1}*[rB]_X,
**
** For our angular constraint, using the fact that
** Q_R(qR)^T = Q_L(qR), we get:
**
**        [    0   ]
**        [-Q_R(qR)]
** J2^T = [    0   ].
**        [ Q_L(qR)]
**
**               [       0       ]
**               [-IA^{-1}Q_R(qR)]
** M^{-1}*J2^T = [       0       ],
**               [ IB^{-1}Q_L(qR)]
**
** K2 = J2*M^{-1}*J2^T
**    = Q_L(qR)IA^{-1}Q_R(qR) + Q_R(qR)IB^{-1}Q_L(qR).
**
** Note that K1 is a 3x3 matrix and K2 is a 2x2 matrix.
**
** ----------------------------------------------------------------------
*/


/*
** Calculate any values required by collision resolution
** that are not expected to change between iterations.
** Such values include the effective mass and the bias.
*/
void physJointRevolutePresolve(
	physicsJointRevolute *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB,
	const float dt
){

	//
}

/*
** Apply an impulse to the physics objects to make sure
** they are within the constraints imposed by the joint.
** This may be called multiple times with sequential impulse.
*/
void physJointRevoluteSolveVelocity(
	physicsJointRevolute *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
){

	//
}

/*
** When we're using full non-linear Gauss-Seidel, we apply
** an impulse directly to the rigid bodies' positions.
** This may also be called multiple times, but by returning
** the amount of error we'll know when to stop.
*/
return_t physJointRevoluteSolvePosition(
	const physicsJointRevolute *const restrict joint,
	physicsRigidBody *const restrict bodyA,
	physicsRigidBody *const restrict bodyB
){

	#ifdef PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL
	#endif

	return(1);
}