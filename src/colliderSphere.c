#include "colliderSphere.h"


/*
** Let S be the sphere of radius r centred at c and R
** the ray starting at p and travelling in direction v.
** That is, we have
**     S = {x in R^3 : ||x - c|| = 1},
**     R = {p + tv : t >= 0}.
** Note that if R intersects S, then it must intersect
** at the boundary at least once at and at most twice.
** Therefore, the following equation must be satisfied:
**     ||p - c + tv||^2 = r^2,
**     <=> t^2 + 2t((p - c) . v) + ||p - c||^2 - r^2 = 0,
**     => t = ((p - c) . v) (+/-) sqrt(((p - c) . v)^2 - ||p - c||^2 + r^2).
** Here, we have assumed that ||v|| = 1 for convenience.
** If the input to the square root is negative, R does
** not intersect S. If it's positive, we still need to
** check whether t >= 0, as otherwise it intersects S
** behind p.
*/
return_t colliderSphereRaycast(){
	return(0);
}