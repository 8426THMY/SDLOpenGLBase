#include "moduleParticle.h"


#include "memoryManager.h"


return_t moduleParticleSetup(){
	// The module's setup will be successful if we
	// can allocate enough memory for our manager
	// and the error object can be setup correctly.
	return(
		1
	);
}

void moduleParticleCleanup(){
	//
}