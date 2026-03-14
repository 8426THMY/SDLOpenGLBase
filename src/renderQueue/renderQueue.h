#ifndef renderQueue_h
#define renderQueue_h


#include <stdint.h>
#include <limits.h>


// A render queue can contain at most 4096 entities.
#define RENDER_QUEUE_MAX_ITEMS 4096


#define RENDER_QUEUE_KEY_TYPE uint64_t

#define RENDER_QUEUE_KEY_NUM_BITS_DEPTH    16
#define RENDER_QUEUE_KEY_NUM_BITS_VAO      24
#define RENDER_QUEUE_KEY_NUM_BITS_MATERIAL 24
#define RENDER_QUEUE_KEY_NUM_BITS         (sizeof(RENDER_QUEUE_KEY_TYPE)*CHAR_BIT)

// Return a 64-bit integer where the first n bits, starting
// from skip, are set to 1, and all the others are set to 0.
#define renderQueueKeyMask(n, skip) (                                            \
	(((RENDER_QUEUE_KEY_TYPE)-1) >> (RENDER_QUEUE_KEY_NUM_BITS - (n))) << (skip) \
)


// Bit masks for opaque render keys.
// mmmmmmmmmmmmmmmmmmmmmmmmvvvvvvvvvvvvvvvvvvvvvvvvdddddddddddddddd
#define RENDER_QUEUE_KEY_OPAQUE_OFFSET_DEPTH 0
#define RENDER_QUEUE_KEY_OPAQUE_OFFSET_VAO \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH
#define RENDER_QUEUE_KEY_OPAQUE_OFFSET_MATERIAL (                   \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH + RENDER_QUEUE_KEY_NUM_BITS_VAO \
)

#define RENDER_QUEUE_KEY_OPAQUE_MASK_DEPTH renderQueueKeyMask(            \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH, RENDER_QUEUE_KEY_OPAQUE_OFFSET_DEPTH \
)
#define RENDER_QUEUE_KEY_OPAQUE_MASK_VAO renderQueueKeyMask(          \
	RENDER_QUEUE_KEY_NUM_BITS_VAO, RENDER_QUEUE_KEY_OPAQUE_OFFSET_VAO \
)
#define RENDER_QUEUE_KEY_OPAQUE_MASK_MATERIAL renderQueueKeyMask(               \
	RENDER_QUEUE_KEY_NUM_BITS_MATERIAL, RENDER_QUEUE_KEY_OPAQUE_OFFSET_MATERIAL \
)

// Bit masks for translucent render keys.
// ddddddddddddddddmmmmmmmmmmmmmmmmmmmmmmmmvvvvvvvvvvvvvvvvvvvvvvvv
#define RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_VAO 0
#define RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_MATERIAL \
	RENDER_QUEUE_KEY_NUM_BITS_VAO
#define RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_DEPTH (                    \
	RENDER_QUEUE_KEY_NUM_BITS_VAO + RENDER_QUEUE_KEY_NUM_BITS_MATERIAL \
)

#define RENDER_QUEUE_KEY_TRANSLUCENT_MASK_VAO renderQueueKeyMask(          \
	RENDER_QUEUE_KEY_NUM_BITS_VAO, RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_VAO \
)
#define RENDER_QUEUE_KEY_TRANSLUCENT_MASK_MATERIAL renderQueueKeyMask(               \
	RENDER_QUEUE_KEY_NUM_BITS_MATERIAL, RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_MATERIAL \
)
#define RENDER_QUEUE_KEY_TRANSLUCENT_MASK_DEPTH renderQueueKeyMask(            \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH, RENDER_QUEUE_KEY_TRANSLUCENT_OFFSET_DEPTH \
)


#warning "Should we split up the material component into shader, texture and pass components?"
#warning "Since we're running out of bits, can we use even more buckets?"


typedef RENDER_QUEUE_KEY_TYPE renderQueueKey;

typedef struct renderQueueKeyValue {
	renderQueueKey key;
	void *value;
} renderQueueKeyValue;

typedef struct renderQueue {
	renderQueueKeyValue keyVals[RENDER_QUEUE_MAX_ITEMS];
	size_t numKeyVals;
} renderQueue;


renderQueueKey renderQueueKeyOpaque(
	const bfloat16 depth,
	const unsigned int vao,
	const unsigned int material
);
renderQueueKey renderQueueKeyTranslucent(
	const bfloat16 depth,
	const unsigned int vao,
	const unsigned int material
);


#endif