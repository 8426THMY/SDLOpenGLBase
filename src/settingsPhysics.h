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

#define PHYSCONSTRAINT_LINEAR_SLOP 0.005f
#define PHYSCONSTRAINT_MAX_LINEAR_CORRECTION 0.2f
#define PHYSCONSTRAINT_ANGULAR_SLOP (2.f * DEG_TO_RAD)
#define PHYSCONSTRAINT_MAX_ANGULAR_CORRECTION (8.f * DEG_TO_RAD)

#define PHYSCONTACT_RESTITUTION_THRESHOLD 1.f
#define PHYSCONTACT_SEPARATION_BIAS 0.f

#define PHYSCONTACT_WARM_START
//#define PHYSCONTACT_STABILISER_BAUMGARTE
#define PHYSCONTACT_STABILISER_GAUSS_SEIDEL
#define PHYSCONTACT_BAUMGARTE_BIAS 0.1f
#define PHYSCONTACT_PENETRATION_ERROR_THRESHOLD (-3.f * PHYSCONSTRAINT_LINEAR_SLOP)

//#define PHYSCONTACT_USE_FRICTION_JOINT
//#define PHYSCONTACT_FRICTION_DELAY
#define PHYSCONTACT_FRICTION_GEOMETRIC_AVERAGE

#define PHYSJOINTFRICTION_WARM_START
#define PHYSJOINTDISTANCE_WARM_START
#define PHYSJOINTFIXED_WARM_START
#define PHYSJOINTREVOLUTE_WARM_START
#define PHYSJOINTPRISMATIC_WARM_START
#define PHYSJOINTSPHERE_WARM_START

#define PHYSJOINTDISTANCE_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTFIXED_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTREVOLUTE_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTPRISMATIC_STABILISER_GAUSS_SEIDEL
#define PHYSJOINTSPHERE_STABILISER_GAUSS_SEIDEL

//#define PHYSJOINTSPHERE_ANGULAR_CONSTRAINT_EULER
#define PHYSJOINTSPHERE_SWING_USE_ELLIPSE_NORMAL

#define PHYSISLAND_AABBTREE_NODE_EXPAND_BY_VELOCITY
#define PHYSISLAND_AABBTREE_NODE_PADDING 0.2f

#define PHYSCOLLIDER_DEFAULT_MASS        0.f
#define PHYSCOLLIDER_DEFAULT_DENSITY     0.f
#define PHYSCOLLIDER_DEFAULT_FRICTION    1.f
#define PHYSCOLLIDER_DEFAULT_RESTITUTION 1.f
#define PHYSCOLLIDER_EXPLICIT_MASS


#endif