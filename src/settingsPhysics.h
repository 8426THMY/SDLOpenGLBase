#ifndef settingsPhysics_h
#define settingsPhysics_h


// Collider settings.
#define COLLIDER_HULL_USE_VERTEX_WEIGHT


// Physics settings.
#define PHYSICS_BASE_UPDATE_RATE 120.f
#define PHYSICS_UPDATE_RATE 120.f
#define PHYSICS_VELOCITY_SOLVER_NUM_ITERATIONS 4
#define PHYSICS_POSITION_SOLVER_NUM_ITERATIONS 4

#define CONTACT_MANIFOLD_SIMPLE_KEYS

#define PHYSCONSTRAINT_LINEAR_SLOP 0.05f
#define PHYSCONSTRAINT_MAX_LINEAR_CORRECTION 0.2f

#define PHYSCONTACT_RESTITUTION_THRESHOLD -1.f

#define PHYSCONTACT_STABILISER_BAUMGARTE
#define PHYSCONTACT_BAUMGARTE_BIAS 0.4f
#define PHYSCONTACT_GAUSS_SEIDEL_NUM_ITERATIONS 4
#define PHYSCONTACT_PENETRATION_ERROR_THRESHOLD (-3.f * PHYSCONSTRAINT_LINEAR_SLOP)

#define PHYSCONTACT_USE_FRICTION_JOINT

#define PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTPRISMATIC_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL

#define PHYSISLAND_AABB_EXPAND_BY_VELOCITY
#define PHYSISLAND_AABB_EXTRA_SPACE 0.2f

#define PHYSCOLLIDER_DEFAULT_MASS        0.f
#define PHYSCOLLIDER_DEFAULT_DENSITY     0.f
#define PHYSCOLLIDER_DEFAULT_FRICTION    1.f
#define PHYSCOLLIDER_DEFAULT_RESTITUTION 1.f
#define PHYSCOLLIDER_EXPLICIT_MASS


#endif