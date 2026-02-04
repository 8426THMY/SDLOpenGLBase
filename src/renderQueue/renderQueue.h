#ifndef renderQueue_h
#define renderQueue_h


#include <stdint.h>


// A render queue can contain at most 4096 entities.
#define RENDER_QUEUE_MAX_ITEMS 4096


#define RENDER_QUEUE_KEY_TYPE uint64_t

#define RENDER_QUEUE_KEY_NUM_BITS_SHADER  14
#define RENDER_QUEUE_KEY_NUM_BITS_TEXTURE 17
#define RENDER_QUEUE_KEY_NUM_BITS_VAO     17
#define RENDER_QUEUE_KEY_NUM_BITS_DEPTH   16
#define RENDER_QUEUE_KEY_NUM_BITS         (sizeof(RENDER_QUEUE_KEY_TYPE)*sizeof(char))

// Return a 64-bit integer where the first n bits, starting
// from skip, are set to 1, and all the others are set to 0.
#define renderQueueKeyMask(n, skip) (                                        \
	(((RENDER_QUEUE_KEY_TYPE)-1) >> (RENDER_QUEUE_KEY_NUM_BITS - n)) << skip \
)


// Bit masks for opaque render keys.
// sssssssssssssstttttttttttttttttvvvvvvvvvvvvvvvvvdddddddddddddddd
#define RENDER_QUEUE_KEY_MASK_OPAQUE_SHADER renderQueueKeyMask( \
	RENDER_QUEUE_KEY_NUM_BITS_SHADER, 0                         \
)
#define RENDER_QUEUE_KEY_MASK_OPAQUE_TEXTURE renderQueueKeyMask(        \
	RENDER_QUEUE_KEY_NUM_BITS_TEXTURE, RENDER_QUEUE_KEY_NUM_BITS_SHADER \
)
#define RENDER_QUEUE_KEY_MASK_OPAQUE_VAO renderQueueKeyMask(             \
	RENDER_QUEUE_KEY_NUM_BITS_VAO,                                       \
	RENDER_QUEUE_KEY_NUM_BITS_SHADER + RENDER_QUEUE_KEY_NUM_BITS_TEXTURE \
)
#define RENDER_QUEUE_KEY_MASK_OPAQUE_DEPTH renderQueueKeyMask(          \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH, RENDER_QUEUE_KEY_NUM_BITS_SHADER + \
	RENDER_QUEUE_KEY_NUM_BITS_TEXTURE + RENDER_QUEUE_KEY_NUM_BITS_VAO   \
)

// Bit masks for translucent render keys.
// ddddddddddddddddsssssssssssssstttttttttttttttttvvvvvvvvvvvvvvvvv
#define RENDER_QUEUE_KEY_MASK_TRANSLUCENT_DEPTH renderQueueKeyMask( \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH, 0                              \
)
#define RENDER_QUEUE_KEY_MASK_TRANSLUCENT_SHADER renderQueueKeyMask(  \
	RENDER_QUEUE_KEY_NUM_BITS_SHADER, RENDER_QUEUE_KEY_NUM_BITS_DEPTH \
)
#define RENDER_QUEUE_KEY_MASK_TRANSLUCENT_TEXTURE renderQueueKeyMask(  \
	RENDER_QUEUE_KEY_NUM_BITS_TEXTURE,                                 \
	RENDER_QUEUE_KEY_NUM_BITS_DEPTH + RENDER_QUEUE_KEY_NUM_BITS_SHADER \
)
#define RENDER_QUEUE_KEY_MASK_TRANSLUCENT_VAO renderQueueKeyMask(        \
	RENDER_QUEUE_KEY_NUM_BITS_VAO, RENDER_QUEUE_KEY_NUM_BITS_DEPTH +     \
	RENDER_QUEUE_KEY_NUM_BITS_SHADER + RENDER_QUEUE_KEY_NUM_BITS_TEXTURE \
)


#warning "Should we split up the material component into shader, texture and pass components?"
#warning "It might also be smart to have two queues, one for opaque and one for translucent objects."
#warning "In fact, since we're running out of bits, can we use even more 'buckets'?"


typedef RENDER_QUEUE_KEY_TYPE renderQueueKey;

typedef struct renderQueueKeyValue {
	renderQueueKey key;
	void *value;
} renderQueueKeyValue;

typedef struct renderQueue {
	renderQueueKeyValue keyVals[RENDER_QUEUE_MAX_ITEMS];
	size_t numKeyVals;
} renderQueue;


#endif