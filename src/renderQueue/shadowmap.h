#ifndef shadowmap_h
#define shadowmap_h


typedef struct shadowmap {
	mat3x4 viewMatrix;
	mat4 vpMatrix;
	renderFrustum frustum;

	renderQueue queue;
} shadowmap;


#endif