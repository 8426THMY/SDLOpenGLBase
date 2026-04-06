#ifndef lightOrtho_h
#define lightOrtho_h


// Orthographic light.
typedef struct lightOrtho {
	vec3 position;
	vec3 direction;
	// Size of the cuboid in which the light exists.
	#warning "It might be nice to support circular lights."
	float width;
	float height;

	// Colour.
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//renderTarget target;
} lightOrtho;


#endif