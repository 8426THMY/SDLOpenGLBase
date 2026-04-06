#ifndef lightDir_h
#define lightDir_h


/**
https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
https://gamedev.net/forums/topic/648924-sun-shadows/5101333/
https://ogldev.org/www/tutorial47/tutorial47.html
**/
// Directional light.
typedef struct lightDir {
	vec3 direction;

	// Colour.
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} lightDir;


#endif