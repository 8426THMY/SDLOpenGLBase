#include "camera.h"


#include "utilMath.h"

#include "settingsProgram.h"


void cameraInit(
	camera *const restrict cam, const flags8_t flags,
	const float width, const float height
){

	vec3InitZero(&cam->pos);
	quatInitIdentity(&cam->rot);

	vec3InitSet(&cam->up, 0.f, 1.f, 0.f);
	cam->fov = PRG_FOV_DEFAULT;

	cam->flags = flags;

	mat4InitIdentity(&cam->viewMatrix);
	cameraUpdateProjectionMatrix(cam, width, height);
	mat4InitIdentity(&cam->vpMatrix);
}


void cameraUpdateProjectionMatrix(
	camera *const restrict cam,
	const float width, const float height
){

	// Use an orthographic projection matrix.
	if(flagsContainsSubset(cam->flags, CAMERA_TYPE_ORTHO)){
		const float aspectRatio = 1.f/((width < height) ? width : height);
		const float widthRatio  = width * aspectRatio;
		const float heightRatio = height * aspectRatio;
		mat4Orthographic(&cam->projMatrix, widthRatio, 0.f, heightRatio, 0.f, CAMERA_NEAR, CAMERA_FAR);

	// Use a perspective projection matrix.
	}else if(flagsContainsSubset(cam->flags, CAMERA_TYPE_FRUSTUM)){
		mat4Perspective(&cam->projMatrix, cam->fov, width/height, CAMERA_NEAR, CAMERA_FAR);

	// Use a fixed 1 unit per screen pixel scale for the camera.
	// This is usually used for GUI elements, so we treat the
	// top-left corner of the screen as the origin, with the x-axis
	// increasing to the right and the y-axis increasing upwards.
	// This means that when the window is resized, GUI elements are
	// scaled towards the top-left corner of the screen.
	}else if(flagsContainsSubset(cam->flags, CAMERA_TYPE_FIXED_SIZE)){
		mat4Orthographic(&cam->projMatrix, 0.f, width, -height, 0.f, 0.f, CAMERA_FAR);
	}else{
		mat4InitIdentity(&cam->projMatrix);
	}
}

// Only update the view projection matrix if the others have been changed.
void cameraUpdateViewProjectionMatrix(camera *const restrict cam){
	mat4MultiplyMat4Out(cam->projMatrix, cam->viewMatrix, &cam->vpMatrix);
}


// Compute a target's distance from a camera!
float cameraDistance(const camera *const restrict cam, const vec3 *const restrict target){
	return(vec3DistanceVec3(&cam->pos, target));
}

// Compute a target's distance from a camera! This is useful for depth sorting.
float cameraDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target){
	return(vec3DistanceSquaredVec3(&cam->pos, target));
}

/*
** Compute a target's distance from a camera!
** The sign is negative if the target is
** behind the camera and positive otherwise.
*/
float cameraSignedDistance(const camera *const restrict cam, const vec3 *const restrict target){
	vec3 tc;
	vec3SubtractVec3Out(&cam->pos, target, &tc);
	// The dot product of the camera's backward vector
	// and the vector from the target to the camera is
	// negative if the target is behind the camera.
	return(copySign(
		vec3MagnitudeVec3(&tc),
		vec3DotVec3Float(&tc, cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2])
	));
}

/*
** Compute a target's distance from a camera! This is useful for depth sorting.
** The sign is negative if the target is behind the camera and positive otherwise.
*/
float cameraSignedDistanceSquared(const camera *const restrict cam, const vec3 *const restrict target){
	vec3 tc;
	vec3SubtractVec3Out(&cam->pos, target, &tc);
	// The dot product of the camera's backward vector
	// and the vector from the target to the camera is
	// negative if the target is behind the camera.
	return(copySign(
		vec3MagnitudeSquaredVec3(&tc),
		vec3DotVec3Float(&tc, cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2])
	));
}