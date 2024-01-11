#include "billboard.h"


#include <stddef.h>


void billboardInit(billboard *const restrict billboardData){
	billboardData->axis = NULL;
	billboardData->target = NULL;
	billboardData->scale = 1.f/BILLBOARD_SCALE_CALIBRATION_DISTANCE;

	billboardData->flags = BILLBOARD_DISABLED;
}


#warning "This can almost certainly be optimized using quaternion swing-twist decomposition."
void billboardState(
	const billboard *const restrict billboardData,
	const camera *const restrict cam, const vec3 *const restrict centroid,
	mat3x4 rootState, mat3x4 *const restrict out
){

	// Use the camera's axes for billboarding.
	// We can just use the columns of its view matrix.
	if(flagsAreSet(billboardData->flags, BILLBOARD_SPRITE)){
		// Note that the view matrix is the inverse of the camera's
		// transformation matrix. Because rotation matrices are orthogonal,
		// we can take the rotation matrix's transpose to invert it.
		vec3InitSet(
			(vec3 *)rootState.m[0],
			cam->viewMatrix.m[0][0], cam->viewMatrix.m[1][0], cam->viewMatrix.m[2][0]
		);
		if(billboardData->axis != NULL){
			*((vec3 *)rootState.m[1]) = *billboardData->axis;
		}else{
			vec3InitSet(
				(vec3 *)rootState.m[1],
				cam->viewMatrix.m[0][1], cam->viewMatrix.m[1][1], cam->viewMatrix.m[2][1]
			);
		}
		vec3InitSet(
			(vec3 *)rootState.m[2],
			cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]
		);


		// Scale the renderable based on its distance from the camera.
		if(flagsAreSet(billboardData->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, centroid) * billboardData->scale;
			mat3x4ScalePre(&rootState, distance, distance, distance);
		}

	// Lock some axes to prevent billboarding around them.
	}else if(flagsAreSet(billboardData->flags, BILLBOARD_LOCK_XYZ)){
		vec3 up;
		vec3 forward;
		vec3 trans;
		mat3x4 rot;

		// The up vector is the axis to billboard on.
		if(billboardData->axis != NULL){
			up = *billboardData->axis;
		}else{
			up = cam->up;
		}

		// Set the eye and target vectors.
		if(flagsAreSet(billboardData->flags, BILLBOARD_TARGET_SPRITE)){
			vec3InitSet(&forward, cam->viewMatrix.m[0][2], cam->viewMatrix.m[1][2], cam->viewMatrix.m[2][2]);
		}else if(billboardData->target != NULL){
			vec3SubtractVec3Out(billboardData->target, centroid, &forward);
		}else{
			vec3SubtractVec3Out(&cam->pos, centroid, &forward);
		}

		// Prevent the renderable from rotating on some axes.
		if(flagsAreUnset(billboardData->flags, BILLBOARD_LOCK_X)){
			forward.y = 0.f;
		}
		if(flagsAreUnset(billboardData->flags, BILLBOARD_LOCK_Y)){
			forward.x = 0.f;
		}
		vec3NormalizeVec3Fast(&forward);
		if(flagsAreUnset(billboardData->flags, BILLBOARD_LOCK_Z)){
			vec3InitSet(&up, 0.f, 1.f, 0.f);
		}


		// Translate the matrix to the origin so it can be transformed correctly.
		trans = *((vec3 *)rootState.m[3]);
		vec3InitZero((vec3 *)rootState.m[3]);

		// Scale the renderable based on its distance from the camera.
		if(flagsAreSet(billboardData->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, centroid) * billboardData->scale;
			mat3x4Scale(&rootState, distance, distance, distance);
		}

		// Rotate the matrix to face the target and move it back to where it was.
		mat3x4RotateForward(&rot, &forward, &up);
		mat3x4MultiplyMat3x4P2(rot, &rootState);
		*((vec3 *)rootState.m[3]) = trans;

	// If we're not using sprites or locking any axes, just use scale billboarding.
	}else{
		if(flagsAreSet(billboardData->flags, BILLBOARD_SCALE)){
			const float distance = cameraDistance(cam, centroid) * billboardData->scale;
			mat3x4ScalePre(&rootState, distance, distance, distance);
		}
	}

	*out = rootState;
}