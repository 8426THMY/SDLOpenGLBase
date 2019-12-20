#ifndef particle_h
#define particle_h


#include "vec3.h"
#include "transform.h"

#include "mesh.h"
#include "textureGroup.h"

#include "utilTypes.h"


typedef struct particleDef {
	mesh meshData;
	textureGroup *texGroup;
} particleDef;

typedef struct particle {
	transformState state;
	vec3 velocity;

	// The particle definition stores the actual
	// texture group that the particle uses.
	size_t currentAnim;
	textureGroupAnim texAnim;

	float lifetime;
} particle;


void particleDefInit(particleDef *partDef);

return_t particleUpdate(particle *part, const float time);



#endif