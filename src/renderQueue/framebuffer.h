#ifndef framebuffer_h
#define framebuffer_h


#define FRAMEBUFFER_DEFAULT_ID 0


typedef struct framebufferViewport {
	int x;
	int y;
	unsigned int w;
	unsigned int h;
} framebufferViewport;

typedef struct framebuffer {
	unsigned int id;
} framebuffer;


void framebufferInit(framebuffer *const restrict fb);
void framebufferBind(
	const framebuffer *const restrict fb,
	const framebufferViewport *const restrict viewport
);
void framebufferDelete(framebuffer *const restrict fb);


#endif