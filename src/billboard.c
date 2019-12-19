#include "billboard.h"


#include <stddef.h>


void billboardInit(billboard *billboardData){
	billboardData->axis = NULL;
	billboardData->target = NULL;
	billboardData->scale = 1.f/BILLBOARD_SCALE_CALIBRATION_DISTANCE;

	billboardData->flags = BILLBOARD_DISABLED;
}


void billboardState(const billboard *billboardData, const camera *cam, const vec3 centroid, mat4 rootState, mat4 *out){
	// Use the camera's axes for billboarding.
	// We can just use the columns of its view matrix.
	if(flagsAreSet(billboardData->flags, BILLBOARD_SPRITE)){
		vec3InitSet((vec3 *)&rootState.m[0][0], cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0]);
		if(billboardData->axis != NULL){
			*((vec3 *)&rootState.m[1][0]) = *billboardData->axis;
		}else{
			vec3InitSet((vec3 *)&rootState.m[1][0], cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1]);
		}
		vec3InitSet((vec3 *)&rootState.m[2][0], cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]);

		rootState.m[0][3] =
		rootState.m[1][3] =
		rootState.m[2][3] = 0.f;
		rootState.m[3][3] = 1.f;


		// Scale the renderable based on its distance from the camera.
		if(flagsAreSet(billboardData->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, &centroid) * billboardData->scale;
			mat4Scale(&rootState, distance, distance, distance);
		}

	// Lock some axes to prevent billboarding around them.
	}else if(flagsAreSet(billboardData->flags, BILLBOARD_LOCK_XYZ)){
		vec3 eye, target, up;
		mat4 rot;

		// The up vector is the axis to billboard on.
		if(billboardData->axis != NULL){
			up = *billboardData->axis;
		}else{
			up = cam->up;
		}

		// Set the eye and target vectors.
		if(flagsAreSet(billboardData->flags, BILLBOARD_TARGET_SPRITE)){
			eye = *cam->target;
			target = cam->pos;
		}else if(billboardData->target != NULL){
			eye = centroid;
			target = *billboardData->target;
		}else{
			eye = centroid;
			target = cam->pos;
		}

		// Prevent the renderable from rotating on some axes.
		if(flagsAreUnset(billboardData->flags, BILLBOARD_LOCK_X)){
			target.y = eye.y;
		}
		if(flagsAreUnset(billboardData->flags, BILLBOARD_LOCK_Y)){
			target.x = eye.x;
		}
		if(flagsAreUnset(billboardData->flags, BILLBOARD_LOCK_Z)){
			vec3InitSet(&up, 0.f, 1.f, 0.f);
		}


		// Translate the matrix to the origin so it can be transformed correctly.
		mat4TranslatePre(&rootState, -centroid.x, -centroid.y, -centroid.z);

		// Scale the renderable based on its distance from the camera.
		if(flagsAreSet(billboardData->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, &centroid) * billboardData->scale;
			mat4ScalePre(&rootState, distance, distance, distance);
		}

		// Rotate the matrix to face the target and move it back to where it was.
		mat4RotateToFace(&rot, &eye, &target, &up);
		mat4MultiplyMat4By(&rootState, rot);
		mat4TranslatePre(&rootState, centroid.x, centroid.y, centroid.z);

	// If we're not using sprites or locking any axes, just use scale billboarding.
	}else{
		if(flagsAreSet(billboardData->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, &centroid) * billboardData->scale;
			mat4Scale(&rootState, distance, distance, distance);
		}
	}

	*out = rootState;
}