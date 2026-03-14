#include "renderQueue.h"


renderQueueKey renderQueueKeyOpaque(
	const bfloat16 depth,
	const unsigned int vao,
	const unsigned int material
){

	return(
		(depth    << RENDER_QUEUE_KEY_OPAQUE_OFFSET_DEPTH)    |
		(vao      << RENDER_QUEUE_KEY_OPAQUE_OFFSET_VAO)      |
		(material << RENDER_QUEUE_KEY_OPAQUE_OFFSET_MATERIAL)
	);
}

renderQueueKey renderQueueKeyTranslucent(
	const bfloat16 depth,
	const unsigned int vao,
	const unsigned int material
){

	return(
		(depth    << RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_DEPTH)    |
		(vao      << RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_VAO)      |
		(material << RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_MATERIAL)
	);
}