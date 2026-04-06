#ifndef lightSpot_h
#define lightSpot_h


// Perspective spotlight.
typedef struct lightSpot {
	vec3 position;
	vec3 direction;
	// Cosines of the angles between which
	// the spotlight intensity fades out.
	float innerAngle;
	float outerAngle;

	// Colour.
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Attenuation.
	float constant;
	float linear;
	float quadratic;

	//renderTarget target;
} lightSpot;


#endif