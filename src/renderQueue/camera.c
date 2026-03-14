#include "camera.h"


#include "utilMath.h"

#include "settingsProgram.h"


// Forward-declare any helper functions!
static void cameraStateInit(cameraState *const restrict state);


void cameraInit(camera *const restrict cam, const cameraType type){
	cameraStateInit(&cam->state);
	cam->prevState = cam->state;
	cam->type = type;
}


void cameraComputeViewMatrix(
	const camera *const restrict cam, const float dt,
	mat3x4 *const restrict viewMatrix
){

	vec3 pos;
	quat rot;

	// Interpolate between the previous and current states.
	vec3LerpOut(&cam->prevState.pos, &cam->state.pos, dt, &pos);
	quatSlerpFasterOut(&cam->prevState.rot, &cam->state.rot, dt, &rot);
	// Generate the view matrix.
	mat3x4ViewQuat(viewMatrix, &pos, &rot);
}

void cameraComputeProjectionMatrix(
	const camera *const restrict cam, const float dt,
	const float width, const float height,
	mat4 *const restrict projMatrix
){

	switch(cam->type){
		// Use a perspective projection matrix.
		case CAMERA_TYPE_FRUSTUM:
			// Only update the projection matrix
			// if the field of view has changed.
			if(cam->state.fov != cam->prevState.fov){
				const float fov = floatLerpFast(
					cam->prevState.fov, cam->state.fov, dt
				);
				mat4Perspective(
					&cam->projMatrix,
					fov, width/height,
					CAMERA_NEAR, CAMERA_FAR
				);
			}
		break;
		// Use an orthographic projection matrix.
		case CAMERA_TYPE_ORTHO:
			const float aspectRatio = 1.f/((width < height) ? width : height);
			const float widthRatio  = width * aspectRatio;
			const float heightRatio = height * aspectRatio;
			mat4Orthographic(
				&cam->projMatrix,
				widthRatio, 0.f,
				heightRatio, 0.f,
				CAMERA_NEAR, CAMERA_FAR
			);
		break;
		// Use a fixed 1 unit per screen pixel scale for the camera.
		// This is usually used for GUI elements, so we treat the
		// top-left corner of the screen as the origin, with the x-axis
		// increasing to the right and the y-axis increasing upwards.
		// This means that when the window is resized, GUI elements are
		// scaled towards the top-left corner of the screen.
		case CAMERA_TYPE_FIXED_SIZE:
			mat4Orthographic(
				&cam->projMatrix,
				0.f, width,
				-height, 0.f,
				0.f, CAMERA_FAR
			);
		break;
	}
}

// Only update the view projection matrix if the others have been changed.
void cameraUpdateViewProjectionMatrix(camera *const restrict cam){
	mat4MultiplyMat4Out(cam->projMatrix, cam->viewMatrix, &cam->vpMatrix);
}


/*
** Compute the signed distance from the camera!
** The sign is positive if the target is in front
** of the camera and positive otherwise.
*/
float cameraDistance(
	const mat3x4 *const restrict viewMatrix,
	const vec3 *const restrict target
){

	// We're really just computing the negative
	// z-coordinate of viewMatrix * target.
	return(-vec3DotVec3Float(
		target,
		viewMatrix->m[0][2], viewMatrix->m[1][2], viewMatrix->m[2][2]
	) - viewMatrix->m[3][2]);
}

/*
** Compute the distance from a camera as a 16-bit
** float! The sign is positive if the target is in
** front of the camera and positive otherwise.
*/
bfloat16 cameraDistance16(
	const mat3x4 *const restrict viewMatrix,
	const vec3 *const restrict target
){

	return(floatToBfloat16(cameraDistance(viewMatrix, target)));
}


static void cameraStateInit(cameraState *const restrict state){
	vec3InitZero(&state->pos);
	quatInitIdentity(&state->rot);
	state->fov = PRG_FOV_DEFAULT;
}