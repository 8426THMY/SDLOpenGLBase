#include "particleInitializer.h"


#include <stdlib.h>


void particleInitializerRandomPosSphere(const void *initializer, particle *part){
	part->state.pos.x = 2.f * (((float)rand())/((float)RAND_MAX) - 0.5f);
	part->state.pos.y = 2.f * (((float)rand())/((float)RAND_MAX) - 0.5f);
	part->state.pos.z = 2.f * (((float)rand())/((float)RAND_MAX) - 0.5f);

	part->lifetime = 0.32f;
}