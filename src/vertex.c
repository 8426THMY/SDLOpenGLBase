#include "vertex.h"


#include <string.h>


// This works so long as our vertices have no padding.
return_t vertexDifferent(const vertex *v1, const vertex *v2){
	return(memcmp(v1, v2, sizeof(*v2)));
}