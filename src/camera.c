#include "camera.h"


#include "utilMath.h"

#include "settingsProgram.h"


void cameraInit(camera *cam){
	vec3InitZero(&cam->pos);
	quatInitIdentity(&cam->rot);
	cam->fov = FOV_DEFAULT;

	vec3InitSet(&cam->up, 0.f, 1.f, 0.f);
	cam->target = NULL;

	mat4InitIdentity(&cam->viewMatrix);
	mat4InitIdentity(&cam->projectionMatrix);
	mat4InitIdentity(&cam->viewProjectionMatrix);

	cam->flags = CAMERA_TYPE_FRUSTUM;
}


void cameraUpdateViewMatrix(camera *cam){
	// Make the camera look at its target if it has one.
	//if(cam->target != NULL){
		//mat4LookAt(&cam->viewMatrix, &cam->pos, cam->target, &cam->up);
		vec3 target = {0.f, 0.f, 0.f};
		mat4LookAt(&cam->viewMatrix, &cam->pos, &target, &cam->up);
		mat4RotateQuat(&cam->viewMatrix, &cam->rot);
	//}
}

void cameraUpdateProjectionMatrix(camera *cam, const float windowWidth, const float windowHeight){
	// If the camera is set in projection mode, create a perspective matrix.
	if(flagsAreSet(cam->flags, CAMERA_TYPE_FRUSTUM)){
		mat4Perspective(&cam->projectionMatrix, cam->fov*DEG_TO_RAD, windowWidth/windowHeight, 0.1f/cam->fov, 1000.f);

	// Otherwise, use an orthographic matrix.
	}else{
		const float screenSizeMod = 1.f / ((windowWidth < windowHeight) ? windowWidth : windowHeight);
		mat4Orthographic(&cam->projectionMatrix, 0.f, windowWidth*screenSizeMod, 0.f, windowHeight*screenSizeMod, 0.1f, 1000.f);
	}
}

// Only update the view projection matrix if the others should be changed.
void cameraUpdateViewProjectionMatrix(camera *cam, const float windowWidth, const float windowHeight){
	const int viewUpdate = 1;
	const int projectionUpdate = 1;

	if(viewUpdate){
		cameraUpdateViewMatrix(cam);
	}
	if(projectionUpdate){
		cameraUpdateProjectionMatrix(cam, windowWidth, windowHeight);
	}

	// Update the view projection matrix if either of its components have changed.
	if(viewUpdate || projectionUpdate){
		mat4MultiplyByMat4Out(cam->projectionMatrix, cam->viewMatrix, &cam->viewProjectionMatrix);
	}
}


// Compute a target's distance from a camera!
float cameraDistance(const camera *cam, const vec3 *target){
	return(sqrtf(vec3DistanceSquaredVec3(&cam->pos, target)));
}

// Compute the square of a target's distance from a camera! This is useful for depth sorting.
float cameraDistanceSquared(const camera *cam, const vec3 *target){
	return(vec3DistanceSquaredVec3(&cam->pos, target));
}