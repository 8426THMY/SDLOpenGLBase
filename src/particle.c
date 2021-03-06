#include "particle.h"


#include <math.h>

#include "sort.h"


// Particles use this sprite by default.
sprite partSpriteDefault;


void particleDefInit(particleDef *const restrict partDef){
	partDef->spriteData = partSpriteDefault;
	partDef->texGroup = &g_texGroupDefault;
}

void particleInit(particle *const restrict part){
	transformStateInit(&part->state);
	vec3InitZero(&part->velocity);

	part->currentAnim = 0;
	animationInit(&part->texAnim, ANIMATION_LOOP_INDEFINITELY);

	part->lifetime = INFINITY;
	part->camDistance = -INFINITY;
}


void particleUpdate(particle *const restrict part, const float time){
	// Integrate the particle's position using symplectic Euler.
	vec3Fmaf(time, &part->velocity, &part->state.pos);

	// Update the texture group animation, among other things.
}


// Return whether or not a particle is still alive.
return_t particleAlive(particle *const restrict part, const float time){
	part->lifetime -= time;
	return(part->lifetime > 0.f);
}

/*
** Compare two particles for use with sorting.
** Only three values can be returned:
**
** SORT_COMPARE_LESSER  - p1's distance from the camera is greater than p2's.
** SORT_COMPARE_EQUAL   - p1's distance from the camera is equal to p2's.
** SORT_COMPARE_GREATER - p1's distance from the camera is less than p2's.
*/
return_t particleCompare(const void *const restrict p1, const void *const restrict p2){
	// Since we want to draw farther particles first, we swap their order in this function.
	return(compareFloat(((particle *)p2)->camDistance, ((particle *)p1)->camDistance));
}


/*
** By setting the distance from the camera to negative
** infinity, we guarantee that the particle will be
** moved to the end of the array during sorting.
*/
#warning "Is this necessary?"
void particleDelete(particle *const restrict part){
	part->lifetime = 0.f;
	part->camDistance = -INFINITY;
}

void particleDefDelete(particleDef *const restrict partDef){
	// Only delete the system's mesh if
	// it's not the default sprite.
	if(spriteDifferent(&partDef->spriteData, &partSpriteDefault)){
		spriteDelete(&partDef->spriteData);
	}
}


return_t particleSetup(){
	const spriteVertex vertices[4] = {
		{
			.pos.x = -0.5f, .pos.y =  0.5f, .pos.z = 0.f,
			.uv.x = 0.f, 0.f
		},
		{
			.pos.x = -0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 0.f, 1.f
		},
		{
			.pos.x =  0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 1.f, 1.f
		},
		{
			.pos.x =  0.5f, .pos.y =  0.5f, .pos.z = 0.f,
			.uv.x = 1.f, 0.f
		}
	};

	const spriteVertexIndex_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	spriteGenerateBuffers(&partSpriteDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}

void particleCleanup(){
	spriteDelete(&partSpriteDefault);
}