#ifndef lightPoint_h
#define lightPoint_h


// Omnidirectional light.
typedef struct lightPoint {
	vec3 position;

	// Colour.
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Attenuation.
	float constant;
	float linear;
	float quadratic;

	//renderTarget targets[6];
} lightPoint;


#endif