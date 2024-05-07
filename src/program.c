#include "program.h"


#include <stdio.h>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include "settingsProgram.h"
#include "cvars.h"

#include "moduleCommand.h"
#include "moduleTexture.h"
#include "moduleTextureGroup.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "modulePhysics.h"
#include "moduleObject.h"
#include "moduleParticle.h"
#include "memoryManager.h"

#include "timer.h"
#include "utilMath.h"
#include "utilFile.h"

/** TEMPORARY DEBUG DRAW STUFF **/
#include "debugDraw.h"


#define MODULE_SETUP_SUCCESS 0


// Forward-declare any helper functions!
static void input(program *const restrict prg);
static void updateCameras(program *const restrict prg);
static void updateObjects(program *const restrict prg);
static void updatePhysics(program *const restrict prg);
static void update(program *const restrict prg);
static void render(program *const restrict prg);

static return_t initLibs(program *const restrict prg);
static return_t initResources(program *const restrict prg);
static return_t setupModules();
static void cleanupModules();


return_t programInit(program *const restrict prg, char *const restrict prgDir){
	// Set the current working directory. This ensures that
	// we're looking for resources in the correct directory.
	fileSetWorkingDirectory(prgDir, NULL);

	timestepInit(&prg->step, PRG_UPDATE_RATE, PRG_FRAME_RATE, PRG_TIME_SPEED);
	// Initialize our timing system.
	timerInit();

	if(!initLibs(prg) || setupModules() != MODULE_SETUP_SUCCESS){
		cv_prg_running = 0;
		return(0);
	}


	// Initialize the command system and input manager.
	cmdSysInit(&prg->cmdSys);
	cmdBufferInit(&prg->cmdBuffer);
	inputMngrInit(&prg->inputMngr);

	// Add the commands for our default list of cvars.
	cmdSysAddFunction(&prg->cmdSys, "exit", &c_exit);
	cmdSysAddFunction(&prg->cmdSys, "mousemove", &c_mousemove);

	inputMngrKeyboardBind(&prg->inputMngr, SDL_SCANCODE_ESCAPE, "exit", sizeof("exit") - 1);


	return(initResources(prg));
}

void programLoop(program *const restrict prg){
	float nextUpdate = timerGetTimeFloat();
	float nextRender = nextUpdate;

	float nextPrint = nextUpdate;
	unsigned int renders = 0;

	while(cv_prg_running){
		float curTime = timerGetTimeFloat();

		// Note: This loop freezes the game if our input and update
		//       functions take longer than "prg->step.updateTime".
		if(curTime >= nextUpdate){
			input(prg);
			update(prg);
			nextUpdate += prg->step.updateTimeScaled;

		// Make sure we don't exceed our framerate cap!
		}else if(curTime >= nextRender){
			prg->step.renderDelta = floatClamp(
				1.f - (nextUpdate - curTime) * prg->step.updateTickrateScaled,
				0.f, 1.f
			);
			render(prg);
			// If we've fallen over a frame behind, set the next render time to be
			// the last frame we missed. This will make us render again as soon as
			// possible, but only while we're consistently missing our deadlines.
			if(curTime - nextRender >= prg->step.renderTime){
				nextRender = curTime - fmod(curTime - nextRender, prg->step.renderTime);

			// Otherwise, pace the frames as usual.
			}else{
				nextRender += prg->step.renderTime;
			}

			++renders;
		}

		// Print out the framerate roughly every second.
		if(curTime >= nextPrint){
			printf("Renders: %u\n", renders);
			renders = 0;
			nextPrint = curTime + 1000.f;
		}

		sleepUntilFloat(floatMin(nextUpdate, nextRender));
	}
}

void programClose(program *const restrict prg){
	puts("\nDeleting object shader...");
	shaderDeleteProgram(prg->objectShader.programID);
	puts("Deleting sprite shader...");
	shaderDeleteProgram(prg->spriteShader.programID);

	puts("Deleting input manager...");
	inputMngrDelete(&prg->inputMngr);
	puts("Deleting command buffer...");
	cmdBufferDelete(&prg->cmdBuffer);
	puts("Deleting command system...\n");
	cmdSysDelete(&prg->cmdSys);

	cleanupModules();


	IMG_Quit();

	SDL_DestroyWindow(prg->window);
	SDL_Quit();
}


static void input(program *const restrict prg){
	#warning "We should deal with this using SDL2 events instead."
	// If the aspect ratio doesn't match the window size you get
	// weird results, especially if you haven't resized the window.
	// Additionally, you may need to recalculate the perspective matrix
	// after changing the window size (or aspect ratio, preferably).
	int tempX = prg->windowWidth;
	int tempY = prg->windowHeight;
	SDL_GetWindowSize(prg->window, &prg->windowWidth, &prg->windowHeight);

	// If the window has been resized, resize the viewport!
	if(prg->windowWidth != tempX || prg->windowHeight != tempY){
		// Make sure we preserve the aspect ratio!
		tempX = prg->windowWidth / PRG_ASPECT_RATIO_X;
		tempY = prg->windowHeight / PRG_ASPECT_RATIO_Y;

		// If the window is too tall, add bars to the top and bottom.
		if(tempX < tempY){
			tempY = tempX * PRG_ASPECT_RATIO_Y;
			glViewport(0, (prg->windowHeight - tempY) / 2, prg->windowWidth, tempY);
			prg->windowHeight = tempY;

		// If the window is too wide, add bars to the sides.
		}else if(tempX > tempY){
			tempX = tempY * PRG_ASPECT_RATIO_X;
			glViewport((prg->windowWidth - tempX) / 2, 0, tempX, prg->windowHeight);
			prg->windowWidth = tempX;

		// Otherwise, we don't need any bars.
		}else{
			glViewport(0, 0, prg->windowWidth, prg->windowHeight);
		}

		// Update the camera's projection matrix using the new display port.
		cameraUpdateProjectionMatrix(&prg->cam, prg->windowWidth, prg->windowHeight);
	}


	// Handle user inputs and any other SDL2 events.
	inputMngrTakeInput(&prg->inputMngr, &prg->cmdBuffer);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

/** TEMPORARY CAMERA STUFF! **/
#define CAMERA_FRICTION_WISH  10.f
#define CAMERA_FRICTION       10.f
#define CAMERA_MAX_SPEED      20.f
#define CAMERA_ACCEL          4.f
#define CAMERA_ACCEL_FRICTION 2.f
static float cPitch = 0.f;
static float cYaw = 0.f;
static vec3 cVelocity = {.x = 0.f, .y = 0.f, .z = 0.f};
#include "utilMath.h"
static void updateCameras(program *const restrict prg){
	/** TEMPORARY CAMERA STUFF! **/
	#warning "Everything here should be handled by a game-specific camera controller structure."
	#warning "Some sample camera controllers are in the 'ideas' folder."
	mat3 rotMatrix;

	cPitch -= DEG_TO_RAD(0.022f * 9.f * cv_mouse_dy);
	cYaw -= DEG_TO_RAD(0.022f * 9.f * cv_mouse_dx);
	mat3InitEulerZXY(&rotMatrix, cPitch, cYaw, 0.f);

	/** Mega modified Quake 3 movement code. **/
	{
		vec3 wishdir = {.x = 0.f, .y = 0.f, .z = 0.f};

		if(prg->inputMngr.keyStates[SDL_SCANCODE_A]){
			vec3SubtractVec3P1(&wishdir, (vec3 *)&rotMatrix.m[0]);
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_D]){
			vec3AddVec3(&wishdir, (vec3 *)&rotMatrix.m[0]);
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_W]){
			vec3SubtractVec3P1(&wishdir, (vec3 *)&rotMatrix.m[2]);
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_S]){
			vec3AddVec3(&wishdir, (vec3 *)&rotMatrix.m[2]);
		}

		// pMoveFriction
		{
			const float speed = vec3MagnitudeVec3(&cVelocity);
			const float control = (speed < 0.5f) ? 0.5f : speed;
			float newSpeed = speed;

			if(wishdir.x != 0.f || wishdir.y != 0.f || wishdir.z != 0.f){
				newSpeed -= control * prg->step.updateDelta * CAMERA_FRICTION_WISH;
			}else{
				newSpeed -= control * prg->step.updateDelta * CAMERA_FRICTION;
			}

			if(newSpeed < 0.f){
				vec3InitZero(&cVelocity);
			}else{
				if(speed > 0.f){
					newSpeed /= speed;
				}
				vec3MultiplyS(&cVelocity, newSpeed);
			}
		}

		// pMoveGround
		if(wishdir.x != 0.f || wishdir.y != 0.f || wishdir.z != 0.f){
			const float wishspeed = CAMERA_MAX_SPEED;
			float addspeed;

			vec3NormalizeVec3(&wishdir);

			addspeed = wishspeed - cVelocity.x*wishdir.x - cVelocity.y*wishdir.y - cVelocity.z*wishdir.z;
			if(addspeed > 0.f){
				float accelspeed = CAMERA_ACCEL * wishspeed * CAMERA_ACCEL_FRICTION * prg->step.updateDelta;
				if(accelspeed > addspeed){
					accelspeed = addspeed;
				}
				vec3FmaP2(accelspeed, &wishdir, &cVelocity);
			}
		}
		vec3FmaP2(prg->step.updateDelta, &cVelocity, &prg->cam.pos);
	}

	mat4View(&prg->cam.viewMatrix, &prg->cam.pos, &rotMatrix);
}

/** TEMPORARY PHYSICS STUFF!! **/
physicsRigidBody *controlPhys = NULL;
object *controlObj = NULL;
object *debugObj = NULL;
static void updateObjects(program *const restrict prg){
	/** TEMPORARY PHYSICS STUFF! **/
	if(controlPhys != NULL){
		if(prg->inputMngr.keyStates[SDL_SCANCODE_J]){
			const vec3 F = vec3InitSetC(-40.f * controlPhys->mass, 0.f, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_L]){
			const vec3 F = vec3InitSetC(40.f * controlPhys->mass, 0.f, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_I]){
			const vec3 F = vec3InitSetC(0.f, 40.f * controlPhys->mass, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
			/*const vec3 F = vec3InitSetC(40.f * controlPhys->mass, 0.f, 0.f);
			const vec3 r = vec3AddVec3C(controlPhys->centroid, vec3InitSetC(0.f, 0.5f, 0.f));
			physRigidBodyApplyAngularForce(controlPhys, &r, &F);*/
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_K]){
			const vec3 F = vec3InitSetC(0.f, -40.f * controlPhys->mass, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
			/*const vec3 F = vec3InitSetC(-40.f * controlPhys->mass, 0.f, 0.f);
			const vec3 r = vec3AddVec3C(controlPhys->centroid, vec3InitSetC(0.f, 0.5f, 0.f));
			physRigidBodyApplyAngularForce(controlPhys, &r, &F);*/
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_U]){
			const vec3 F = vec3InitSetC(0.f, 0.f, -40.f * controlPhys->mass);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->inputMngr.keyStates[SDL_SCANCODE_O]){
			const vec3 F = vec3InitSetC(0.f, 0.f, 40.f * controlPhys->mass);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		/** printf("p: %f, %f, %f\nv: %f, %f, %f\n",
			controlPhys->centroid.x, controlPhys->centroid.y, controlPhys->centroid.z,
			controlPhys->linearVelocity.x, controlPhys->linearVelocity.y, controlPhys->linearVelocity.z
		); **/
	}
	if(controlObj != NULL){
		//controlObj->boneTransforms[0].rot = mat3x4ToQuatC(mat3x4RotateToFaceC(controlObj->boneTransforms[0].pos, prg->cam.pos, vec3InitSetC(0.f, 1.f, 0.f)));
	}
	#warning "Remove debug object when done."
	#error "Uncomment errors in particleSystem.h, particle.c and particleRenderer.c once this is done."
	#error "Handle the rigid body simulation flags properly! See the comment in the header file for details."
	#error "While you're at it, add a function for calculating the relative velocity!"
	#error "Might be a good idea to check module allocation functions for failure."
	#error "Updating bones takes a long time..."
	#error "We might have to use matrices for everything, and just convert to our transform format whenever we want to do interpolation."
	if(debugObj != NULL){
		static float t = 0.f;

		//const quat rot = quatInitAxisAngleC(swingAxisDebug, swingAngleDebug);
		//const quat rot = quatInitAxisAngleC(twistAxisDebug, twistAngleDebug);
		#ifdef PHYSJOINTSPHERE_DEBUG
		const quat rot = quatMultiplyQuatC(quatInitAxisAngleC(swingAxisDebug, swingAngleDebug), quatInitAxisAngleC(twistAxisDebug, twistAngleDebug));
		//const quat rot = quatConjMultiplyQuatC(quatMultiplyQuatC(quatInitAxisAngleC(swingAxisDebug, swingAngleDebug), quatInitAxisAngleC(twistAxisDebug, twistAngleDebug)), controlPhys->state.rot);
		#else
		const quat rot = quatInitIdentityC();
		#endif
		debugObj->boneTransforms[0].pos = vec3AddVec3C(
			vec3AddVec3C(
				vec3InitSetC(0.23907208442687988f, -0.20703732967376709f+2.f, -0.17244648933410645f-2.f),
				quatRotateVec3FastC(rot, vec3InitSetC(2.5f, 0.f, 0.f))
			),
			vec3NegateC(quatRotateVec3FastC(rot, debugObj->physBodies->base->centroid))
		);

		debugObj->boneTransforms[0].rot = rot;

		t += 0.01f;
	}

	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, curObj, object)
		objectUpdate(curObj, prg->step.updateTime);
	MEMSINGLELIST_LOOP_END(g_objectManager, curObj)

	// Update the models' positions and rotations!
	/** Temporary if statement for temporary code. Don't want the program to crash, do we? **/
	/*if(allRenderObjects.size > 2){
		renderObjState *currentObj = ((renderObject *)vectorGet(&allRenderObjects, 1))->states[0];
		interpTransAddRotEuler(&currentObj->transform, 0.f, 0.f, DEG_TO_RAD(2.f), prg->step.updateDelta);

		currentObj = ((renderObject *)vectorGet(&allRenderObjects, 2))->states[0];
		interpTransAddRotEuler(&currentObj->transform, 2.f * DEG_TO_RAD, DEG_TO_RAD(2.f), 2.f * DEG_TO_RAD, prg->step.updateDelta);
	}*/
}

/** TEMPORARY STUFF! **/
#include "physicsIsland.h"
physicsIsland island;
static void updatePhysics(program *const restrict prg){
	physIslandUpdate(&island, prg->step.updateDelta);
}

/** TEMPORARY STUFF! **/
#include "particleSystem.h"
#include "textFont.h"
#include "guiElement.h"
particleSystemDef partSysDef;
particleSystem partSys;
guiElement gui;
textFont fontIBM;
static void update(program *const restrict prg){
	// Execute the command buffer.
	cmdBufferExecute(&prg->cmdBuffer, &prg->cmdSys);


	updateObjects(prg);
	updatePhysics(prg);
	updateCameras(prg);


	/** TEMPORARY PARTICLE UPDATE STUFF! **/
	particleSysUpdate(&partSys, prg->step.updateDelta);


	/** TEMPORARY GUI UPDATE STUFF! **/
	if(prg->inputMngr.keyStates[SDL_SCANCODE_LEFT]){
		gui.state.pos.x -= 100.f * prg->step.updateDelta;
	}
	if(prg->inputMngr.keyStates[SDL_SCANCODE_RIGHT]){
		gui.state.pos.x += 100.f * prg->step.updateDelta;
	}
	if(prg->inputMngr.keyStates[SDL_SCANCODE_UP]){
		gui.state.pos.y += 100.f * prg->step.updateDelta;
	}
	if(prg->inputMngr.keyStates[SDL_SCANCODE_DOWN]){
		gui.state.pos.y -= 100.f * prg->step.updateDelta;
	}

	if(prg->inputMngr.keyStates[SDL_SCANCODE_A]){
		gui.state.scale.x -= 100.f * prg->step.updateDelta;
	}
	if(prg->inputMngr.keyStates[SDL_SCANCODE_D]){
		gui.state.scale.x += 100.f * prg->step.updateDelta;
	}
	if(prg->inputMngr.keyStates[SDL_SCANCODE_W]){
		gui.state.scale.y -= 100.f * prg->step.updateDelta;
	}
	if(prg->inputMngr.keyStates[SDL_SCANCODE_S]){
		gui.state.scale.y += 100.f * prg->step.updateDelta;
	}

	guiElementUpdate(&gui, prg->step.updateTime);
}

static void render(program *const restrict prg){
	#warning "We should stop clearing the colour buffer eventually."
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	cameraUpdateViewProjectionMatrix(&prg->cam);

	glUseProgram(prg->objectShader.programID);
	// Send the new model view projection matrix to the shader!
	glUniformMatrix4fv(prg->objectShader.vpMatrixID, 1, GL_FALSE, (GLfloat *)&prg->cam.viewProjectionMatrix);
	// Render each object.
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, curObj, object)
		#warning "We'll need the camera in this function for billboards. Just pass it instead of the matrix."
		objectDraw(curObj, &prg->cam, &prg->objectShader, prg->step.renderDelta);
	MEMSINGLELIST_LOOP_END(g_objectManager, curObj)

	/** TEMPORARY PARTICLE RENDER STUFF! **/
	glUseProgram(prg->spriteShader.programID);
	particleSysDraw(&partSys, &prg->cam, &prg->spriteShader, prg->step.renderDelta);

	/** TEMPORARY GUI RENDER STUFF! **/
	glUseProgram(prg->spriteShader.programID);
	/** Do we need this? **/
	glClear(GL_DEPTH_BUFFER_BIT);
	//guiElementDraw(&gui, prg->windowWidth, prg->windowHeight, &prg->spriteShader);


	#if GFX_ENABLE_DOUBLEBUFFERING
	SDL_GL_SwapWindow(prg->window);
	#else
	glFlush();
	#endif
}


static return_t initLibs(program *const restrict prg){
	// Initialize the SDL2 video subsystem!
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf(
			"Unable to initialize SDL2 video subsystem!\n"
			"Error: %s\n", SDL_GetError()
		);
		return(0);
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GFX_DEFAULT_GL_VERSION_MAJOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GFX_DEFAULT_GL_VERSION_MINOR);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, GFX_ENABLE_DOUBLEBUFFERING);

	// Initialize the SDL2 Image library!
	if(!IMG_Init(IMG_INIT_PNG)){
		printf(
			"Unable to initialize SDL2 Image library!\n"
			"Error: %s\n", IMG_GetError()
		);
		return(0);
	}

	// Create a window using SDL2!
	prg->window = SDL_CreateWindow(
		"NewSDLOpenGLBaseC",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		PRG_WINDOW_DEFAULT_WIDTH, PRG_WINDOW_DEFAULT_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if(!prg->window){
		printf(
			"Unable to create SDL2 window!\n"
			"Error: %s\n", SDL_GetError()
		);
		return(0);
	}
	prg->windowWidth = PRG_WINDOW_DEFAULT_WIDTH;
	prg->windowHeight = PRG_WINDOW_DEFAULT_HEIGHT;

	// Create an OpenGL context using SDL2!
	if(!SDL_GL_CreateContext(prg->window)){
		printf(
			"Unable to create an OpenGL context!\n"
			"Error: %s\n", SDL_GetError()
		);
		return(0);
	}
	SDL_GL_SetSwapInterval(0);

	#ifdef _WIN32
	#warning "SDL has a nasty tendency to set the timer resolution and forget about it, which just drains power."
	#warning "Although we don't use SDL's timers, it does, so this might break stuff somewhere."
	timeEndPeriod(1);
	#endif
	//SDL_ShowCursor(SDL_DISABLE);


	// Initialize the GLEW library!
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf(
			"Unable to initialize GLEW!\n"
			"Error: %s\n", glewGetErrorString(glewError)
		);
		return(0);
	}

	glewExperimental = GL_TRUE;


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	#warning "Don't enable this unless an object is translucent."
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);


	return(1);
}

static return_t initResources(program *const restrict prg){
	printf("Beginning to initialize resources...\n");

	const GLuint objVertexShaderID      = shaderLoad("./resource/shaders/vertexShader.gls",         GL_VERTEX_SHADER);
	const GLuint spriteVertexShaderID   = shaderLoad("./resource/shaders/spriteVertexShader.gls",   GL_VERTEX_SHADER);
	const GLuint objFragmentShaderID    = shaderLoad("./resource/shaders/fragmentShader.gls",       GL_FRAGMENT_SHADER);
	const GLuint spriteFragmentShaderID = shaderLoad("./resource/shaders/spriteFragmentShader.gls", GL_FRAGMENT_SHADER);
	const GLuint objectProgramID        = shaderLoadProgram(objVertexShaderID, objFragmentShaderID);
	const GLuint spriteProgramID        = shaderLoadProgram(spriteVertexShaderID, spriteFragmentShaderID);

	shaderDelete(spriteFragmentShaderID);
	shaderDelete(objFragmentShaderID);
	shaderDelete(spriteVertexShaderID);
	shaderDelete(objVertexShaderID);

	// Load, compile and attach our OpenGL shader programs!
	if(!meshShaderInit(&prg->objectShader, objectProgramID) || !spriteShaderInit(&prg->spriteShader, spriteProgramID)){
		shaderDeleteProgram(spriteProgramID);
		shaderDeleteProgram(objectProgramID);
		return(0);
	}


	/** TEMPORARY CAMERA STUFF **/
	cameraInit(&prg->cam, CAMERA_TYPE_FRUSTUM, prg->windowWidth, prg->windowHeight);
	prg->cam.pos.z = 5.f;


	/** TEMPORARY OBJECT STUFF **/
	#if 0
	modelDef *mdlDef;
	objectDef *objDef;// = moduleObjectDefAlloc();
	object *obj;// = moduleObjectAlloc();
	#else
	modelDef *mdlDef;
	objectDef *objDef = moduleObjectDefAlloc();
	object *obj = moduleObjectAlloc();
	skeletonAnimDef *animDef;


	//mdlDef = modelDefSMDLoad("soldier_reference.smd", sizeof("soldier_reference.smd") - 1);
	mdlDef = modelDefSMDLoad("scout_reference.smd", sizeof("scout_reference.smd") - 1);
	objectDefInit(objDef);
	objDef->skele = mdlDef->skele;
	objDef->mdlDefs = memoryManagerGlobalAlloc(sizeof(*objDef->mdlDefs));
	*objDef->mdlDefs = mdlDef;
	objDef->numModels = 1;

	objectInit(obj, objDef);
	//obj->boneTransforms[0].pos = vec3InitSetC(0.f, -2.f, 2.f);//vec3InitSetC(-1.f, -2.f, -3.f);
	//obj->boneTransforms[0].rot = quatInitEulerXYZC(DEG_TO_RAD(45.f), DEG_TO_RAD(10.f), DEG_TO_RAD(23.f));
	//obj->boneTransforms[0].scale.z = 0.1f;
	controlObj = obj;

	// Temporary animation stuff.
	animDef = skeleAnimSMDLoad("soldier_animations_anims_old/layer_taunt07.smd", sizeof("soldier_animations_anims_old/layer_taunt07.smd") - 1);
	//animDef = skeleAnimSMDLoad("soldier_animations_anims_old/a_runN_MELEE.smd", sizeof("soldier_animations_anims_old/a_runN_MELEE.smd") - 1);
	//animDef = skeleAnimSMDLoad("soldier_animations_anims_old/a_runN_LOSER.smd", sizeof("soldier_animations_anims_old/a_runN_LOSER.smd") - 1);
	if(animDef != NULL){
		obj->skeleState.anims = moduleSkeletonAnimPrepend(&obj->skeleState.anims);
		skeleAnimInit(obj->skeleState.anims, animDef, 1.f, 1.f);
	}

	/*animDef = skeleAnimSMDLoad("soldier_animations_anims_old/stand_MELEE.smd", sizeof("soldier_animations_anims_old/stand_MELEE.smd") - 1);
	if(animDef != NULL){
		obj->skeleState.anims = moduleSkeletonAnimPrepend(&obj->skeleState.anims);
		skeleAnimInit(obj->skeleState.anims, animDef, 1.f, 0.5f);
	}*/
	#endif


	/** TEMPORARY PHYSICS STUFF **/
	physIslandInit(&island);
	#if 1
	// Create the base physics object.
	objDef = moduleObjectDefAlloc();
	objectDefInit(objDef);
	physRigidBodyDefLoad(&objDef->physBodies, "cube.tdp", sizeof("cube.tdp") - 1);
	objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(*objDef->physBoneIDs));
	*objDef->physBoneIDs = 0;
	objDef->numBodies = 1;
	objDef->mdlDefs = memoryManagerGlobalAlloc(sizeof(*objDef->mdlDefs));
	*objDef->mdlDefs = &g_mdlDefDefault;
	objDef->numModels = 1;

	// Set up an instance.
	obj = moduleObjectAlloc();
	objectInit(obj, objDef);
	printf("Ground: %u -> %u\n", obj->physBodies, obj->physBodies->colliders);
	obj->boneTransforms[0].pos.y = -4.f;
	obj->boneTransforms[0].scale.x = obj->boneTransforms[0].scale.z = 100.f;
	physRigidBodySetScale(obj->physBodies, vec3InitSetC(20.f, 0.f, 20.f));
	obj->physBodies->mass = 0.f;
	mat3InitZero(&obj->physBodies->invInertiaLocal);
	mat3InitZero(&obj->physBodies->invInertiaGlobal);
	obj->physBodies->invMass = 0.f;
	objectPreparePhysics(obj);
	physRigidBodyIgnoreSimulation(obj->physBodies);
	physIslandInsertRigidBody(&island, obj->physBodies);

	#warning "Maybe test contacts to make sure there's no energy loss/gain or anything."
	#if 1
	{
		physicsRigidBody *egg, *cube;


		// Create the base physics object.
		mdlDef = modelDefOBJLoad("cubeQuads.obj", sizeof("cubeQuads.obj") - 1);
		objDef = moduleObjectDefAlloc();
		objectDefInit(objDef);
		physRigidBodyDefLoad(&objDef->physBodies, "cube.tdp", sizeof("cube.tdp") - 1);
		objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(*objDef->physBoneIDs));
		*objDef->physBoneIDs = 0;
		objDef->numBodies = 1;
		objDef->mdlDefs = memoryManagerGlobalAlloc(sizeof(*objDef->mdlDefs));
		*objDef->mdlDefs = mdlDef;
		objDef->numModels = 1;

		// Set up an instance.
		obj = moduleObjectAlloc();
		objectInit(obj, objDef);
		printf("Cube %u: %u -> %u\n", 0, obj->physBodies, obj->physBodies->colliders);
		// Testing sphere joint positional correction:
		//obj->boneTransforms[0].pos = vec3InitSetC(-1.497113f, -0.173426f, -2.172446f);
		//obj->boneTransforms[0].rot = quatInitSetC(0.516704f, 0.f, 0.f, -0.856164f);
		obj->boneTransforms[0].pos = vec3InitSetC(0.23907208442687988f+2.5f, -0.70703732967376709f+2.f, -0.17244648933410645f-2.f);
		// Testing distance joint:
		// To get the gravitational equilibrium for the spring constraint, add mg/k to the y position.
		//obj->boneTransforms[0].pos = vec3InitSetC(0.23907208442687988f, -0.70703732967376709f-0.5f, -0.17244648933410645f-2.f);
		//obj->physBodies->linearVelocity.x = 12.f;

		//obj->physBodies->mass = 0.f;
		//mat3InitZero(&obj->physBodies->invInertiaLocal);
		//mat3InitZero(&obj->physBodies->invInertiaGlobal);
		//obj->physBodies->invMass = 0.f;

		objectPreparePhysics(obj);
		//physRigidBodyIgnoreSimulation(obj->physBodies);
		cube = obj->physBodies;


		// Debug object.
		obj = moduleObjectAlloc();
		objectInit(obj, objDef);
		objectPreparePhysics(obj);
		physRigidBodyIgnoreSimulation(obj->physBodies);
		physRigidBodyIgnoreCollisions(obj->physBodies);
		debugObj = obj;


		// Create the base physics object.
		objDef = moduleObjectDefAlloc();
		objectDefInit(objDef);
		physRigidBodyDefLoad(&objDef->physBodies, "egg.tdp", sizeof("egg.tdp") - 1);
		objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(*objDef->physBoneIDs));
		*objDef->physBoneIDs = 0;
		objDef->numBodies = 1;
		objDef->mdlDefs = memoryManagerGlobalAlloc(sizeof(*objDef->mdlDefs));
		*objDef->mdlDefs = mdlDef;
		objDef->numModels = 1;

		// Set up an instance.
		obj = moduleObjectAlloc();
		objectInit(obj, objDef);
		printf("Egg: %u -> %u\n", obj->physBodies, obj->physBodies->colliders);
		obj->boneTransforms[0].pos.y = 2.f;obj->boneTransforms[0].pos.z = -2.f;
		//quatInitEulerXYZ(&obj->boneTransforms[0].rot, 0.f, M_PI_2, M_PI_2);

		obj->physBodies->mass = 0.f;
		mat3InitZero(&obj->physBodies->invInertiaLocal);
		mat3InitZero(&obj->physBodies->invInertiaGlobal);
		obj->physBodies->invMass = 0.f;

		objectPreparePhysics(obj);
		physRigidBodyIgnoreSimulation(obj->physBodies);
		egg = obj->physBodies;


		{
			#define TEST_PHYSJOINTSPHERE
			#ifdef TEST_PHYSJOINTSPHERE
			physicsJoint *joint = modulePhysicsJointAlloc();
			const vec3 anchorA = egg->base->centroid;
			const vec3 anchorB = vec3AddVec3C(cube->base->centroid, vec3InitSetC(-2.5f, 0.f, 0.f));
			const quat rotOffsetA = quatInitIdentityC();
			const quat rotOffsetB = quatInitIdentityC();
			#warning "This stuff still needs a lot of work. The positional contraint works fine with everything, it's just the angular one."
			#warning "At the moment, the angular constraints are disabled. Try putting a test object on the constraint cone."
			physJointSphereInit(
				&joint->data.sphere,
				&anchorA, &anchorB,
				&rotOffsetA, &rotOffsetB,
				1.f,
				0.f, 0.f,
				-M_PI_4, M_PI_4,
				-M_PI_2, M_PI_2
			);
			joint->type = PHYSJOINT_TYPE_SPHERE;
			physJointAdd(joint, egg, cube);
			#elif defined(TEST_PHYSJOINTDISTANCE)
			physicsJoint *joint = modulePhysicsJointAlloc();
			const vec3 anchorA = egg->base->centroid;
			const vec3 anchorB = cube->base->centroid;
			physJointDistanceInit(&joint->data.distance, &anchorA, &anchorB, 2.5f, 4.f*M_PI*M_PI*10.f, 0.f);
			joint->type = PHYSJOINT_TYPE_DISTANCE;
			physJointPairAdd(joint, egg, cube);
			#elif defined(TEST_PHYSJOINTFIXED)
			physicsJoint *joint = modulePhysicsJointAlloc();
			const vec3 anchorA = egg->base->centroid;
			const vec3 anchorB = vec3AddVec3C(cube->base->centroid, vec3InitSetC(-2.5f, 0.f, 0.f));
			const quat rotOffsetA = quatInitIdentityC();
			const quat rotOffsetB = quatInitIdentityC();
			physJointFixedInit(&joint->data.fixed, &anchorA, &anchorB, &rotOffsetA, &rotOffsetB);
			joint->type = PHYSJOINT_TYPE_FIXED;
			physJointPairAdd(joint, egg, cube);
			#endif
		}
		physIslandInsertRigidBody(&island, cube);
		physIslandInsertRigidBody(&island, egg);
		controlPhys = cube;
	}
	#else
	/** EVEN MORE TEMPORARY PHYSICS STUFF **/
	// Create the base physics object.
	mdlDef = modelDefOBJLoad("cubeQuads.obj", sizeof("cubeQuads.obj") - 1);
	objDef = moduleObjectDefAlloc();
	objectDefInit(objDef);
	physRigidBodyDefLoad(&objDef->physBodies, "cube.tdp", sizeof("cube.tdp") - 1);
	objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(*objDef->physBoneIDs));
	*objDef->physBoneIDs = 0;
	objDef->numBodies = 1;
	objDef->mdlDefs = memoryManagerGlobalAlloc(sizeof(*objDef->mdlDefs));
	*objDef->mdlDefs = mdlDef;
	objDef->numModels = 1;

	size_t i;
	// Set up instances.
	for(i = 0; i < 4; ++i){
		obj = moduleObjectAlloc();
		objectInit(obj, objDef);
		printf("Cube %u: %u -> %u\n", i, obj->physBodies, obj->physBodies->colliders);
		obj->boneTransforms[0].pos.y = ((float)i)*2.f;
		objectPreparePhysics(obj);
		physIslandInsertRigidBody(&island, obj->physBodies);
	}
	controlPhys = obj->physBodies;
	physRigidBodyIgnoreAngular(obj->physBodies);

	/** MORE TEMPORARY PHYSICS STUFF **/
	// Create the base physics object.
	objDef = moduleObjectDefAlloc();
	objectDefInit(objDef);
	physRigidBodyDefLoad(&objDef->physBodies, "egg.tdp", sizeof("egg.tdp") - 1);
	objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(*objDef->physBoneIDs));
	*objDef->physBoneIDs = 0;
	objDef->numBodies = 1;
	objDef->mdlDefs = memoryManagerGlobalAlloc(sizeof(*objDef->mdlDefs));
	*objDef->mdlDefs = mdlDef;
	objDef->numModels = 1;

	// Set up an instance.
	obj = moduleObjectAlloc();
	objectInit(obj, objDef);
	printf("Egg: %u -> %u\n", obj->physBodies, obj->physBodies->colliders);
	obj->boneTransforms[0].pos.y = 0.f;obj->boneTransforms[0].pos.z = -2.f;
	quatInitEulerXYZ(&obj->boneTransforms[0].rot, 0.f, M_PI_2, M_PI_2);

	obj->physBodies->mass = 0.f;
	mat3InitZero(&obj->physBodies->invInertiaLocal);
	mat3InitZero(&obj->physBodies->invInertiaGlobal);
	obj->physBodies->invMass = 0.f;

	objectPreparePhysics(obj);
	physRigidBodyIgnoreSimulation(obj->physBodies);
	physIslandInsertRigidBody(&island, obj->physBodies);
	#endif
	#endif


	/** EVEN MORE TEMPORARY PARTICLE STUFF **/
	particleSetup();

	particleSysDefInit(&partSysDef);
	partSysDef.maxParticles = SPRITE_MAX_INSTANCES;
	partSysDef.initializers = memoryManagerGlobalAlloc(sizeof(*partSysDef.initializers));
	partSysDef.lastInitializer = &partSysDef.initializers[0];
	partSysDef.initializers->func = &particleInitializerRandomPosSphere;
	partSysDef.emitters = memoryManagerGlobalAlloc(sizeof(*partSysDef.emitters));
	partSysDef.emitters->func = &particleEmitterContinuous;
	partSysDef.numEmitters = 1;
	partSysDef.operators = memoryManagerGlobalAlloc(2*sizeof(*partSysDef.operators));
	partSysDef.lastOperator = &partSysDef.operators[1];
	partSysDef.operators[0].func = &particleOperatorAddGravity;
	partSysDef.operators[1].func = &particleOperatorDecayLifetime;
	partSysDef.renderers = memoryManagerGlobalAlloc(sizeof(*partSysDef.renderers));
	partSysDef.lastRenderer = &partSysDef.renderers[0];
	partSysDef.renderers->func = &particleRendererSprites;

	particleSysInit(&partSys, &partSysDef);


	/** TEMPORARY FONT STUFF **/
	#if 0
	guiElementSetup();
	const texture *atlasArray[2] = {
		textureLoad("gui/PxPlusIBMBIOS.0.tdt", sizeof("gui/PxPlusIBMBIOS.0.tdt")),
		textureLoad("gui/PxPlusIBMBIOS.1.tdt", sizeof("gui/PxPlusIBMBIOS.1.tdt"))
	};
	//textFontLoad(&fontIBM, "./resource/fonts/PxPlus_IBM_BIOS.tdf");
	textFontLoad(&fontIBM, SPRITE_IMAGE_TYPE_MSDF, atlasArray, "./resource/fonts/PxPlus_IBM_BIOS.ttf", "./resource/fonts/PxPlus_IBM_BIOS-msdf-temp.csv");

	guiElementInit(&gui, GUI_ELEMENT_TYPE_TEXT);
	gui.state.pos.x = 0.f;
	gui.state.pos.y = 0.f;
	gui.state.scale.x = gui.state.scale.y = 0.5f;
	guiTextInit(&gui.data.text, &fontIBM, sizeof("The quick brown fox jumps over the lazy dog!"));
	textBufferWrite(&gui.data.text.buffer, "The quick brown fox jumps over the lazy dog!", sizeof("The quick brown fox jumps over the lazy dog!"));

	gui.data.text.width  = 640.f;
	gui.data.text.height = 480.f;


	/** TEMPORARY PANEL STUFF **/
	#else
	guiElementSetup();
	guiElementInit(&gui, GUI_ELEMENT_TYPE_PANEL);
	gui.state.pos.x = 0.f;
	gui.state.pos.y = 0.f;
	gui.state.scale.x = gui.state.scale.y = 100.f;
	guiPanelInit(&gui.data.panel);

	gui.data.panel.borderTexState.texGroup = texGroupLoad("gui/border.tdg", sizeof("gui/border.tdg") - 1);
	gui.data.panel.bodyTexState.texGroup   = texGroupLoad("gui/body.tdg", sizeof("gui/body.tdg") - 1);

	// Bottom-right corner.
	gui.data.panel.uvCoords[0].x = 0.75f;
	gui.data.panel.uvCoords[0].y = 0.8f;
	gui.data.panel.uvCoords[0].w = 0.25f;
	gui.data.panel.uvCoords[0].h = 0.2f;
	// Top-right corner.
	gui.data.panel.uvCoords[1].x = 0.f;
	gui.data.panel.uvCoords[1].y = 0.8f;
	gui.data.panel.uvCoords[1].w = 0.25f;
	gui.data.panel.uvCoords[1].h = 0.2f;
	// Top-left corner.
	gui.data.panel.uvCoords[2].x = 0.25f;
	gui.data.panel.uvCoords[2].y = 0.8f;
	gui.data.panel.uvCoords[2].w = 0.25f;
	gui.data.panel.uvCoords[2].h = 0.2f;
	// Bottom-left corner.
	gui.data.panel.uvCoords[3].x = 0.5f;
	gui.data.panel.uvCoords[3].y = 0.8f;
	gui.data.panel.uvCoords[3].w = 0.25f;
	gui.data.panel.uvCoords[3].h = 0.2f;

	// Right edge.
	gui.data.panel.uvCoords[4].x = 0.f;
	gui.data.panel.uvCoords[4].y = 0.6f;
	gui.data.panel.uvCoords[4].w = 1.f;
	gui.data.panel.uvCoords[4].h = 0.2f;
	// Top edge.
	gui.data.panel.uvCoords[5].x = 0.f;
	gui.data.panel.uvCoords[5].y = 0.f;
	gui.data.panel.uvCoords[5].w = 1.f;
	gui.data.panel.uvCoords[5].h = 0.2f;
	// Left edge.
	gui.data.panel.uvCoords[6].x = 0.f;
	gui.data.panel.uvCoords[6].y = 0.2f;
	gui.data.panel.uvCoords[6].w = 1.f;
	gui.data.panel.uvCoords[6].h = 0.2f;
	// Bottom edge.
	gui.data.panel.uvCoords[7].x = 0.f;
	gui.data.panel.uvCoords[7].y = 0.4f;
	gui.data.panel.uvCoords[7].w = 1.f;
	gui.data.panel.uvCoords[7].h = 0.2f;

	flagsSet(gui.data.panel.flags, GUI_PANEL_TILE_BODY);
	#endif


	printf("Finished initializing resources!\n");


	return(1);
}

// Set up the allocators for our modules.
static return_t setupModules(){
	puts("Beginning setup...\n");
	memoryManagerGlobalInit(MEMORY_HEAPSIZE);

	#ifdef MODULE_COMMAND
	if(!moduleCommandSetup()){
		return(MODULE_COMMAND_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_TEXTURE
	if(!moduleTextureSetup()){
		return(MODULE_TEXTURE_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_TEXGROUP
	if(!moduleTexGroupSetup()){
		return(MODULE_TEXGROUP_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_SKELETON
	if(!moduleSkeletonSetup()){
		return(MODULE_SKELETON_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_PHYSICS
	if(!modulePhysicsSetup()){
		return(MODULE_PHYSICS_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_MODEL
	if(!moduleModelSetup()){
		return(MODULE_MODEL_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_OBJECT
	if(!moduleObjectSetup()){
		return(MODULE_OBJECT_SETUP_FAIL);
	}
	#endif
	#ifdef MODULE_PARTICLE
	if(!moduleParticleSetup()){
		return(MODULE_PARTICLE_SETUP_FAIL);
	}
	#endif

	/** THIS IS TEMPORARY **/
	debugDrawSetup();

	memTreePrintAllSizes(&g_memManager);
	puts("Setup complete!\n");


	return(MODULE_SETUP_SUCCESS);
}

static void cleanupModules(){
	puts("Beginning cleanup...\n");
	//memTreePrintAllSizes(&g_memManager);

	/** YET MORE TEMPORARY PHYSICS STUFF **/
	physIslandDelete(&island);
	/** YET MORE TEMPORARY PARTICLE STUFF **/
	particleSysDelete(&partSys);
	particleSysDefDelete(&partSysDef);
	particleCleanup();
	/** YET MORE TEMPORARY GUI STUFF **/
	if(gui.type == GUI_ELEMENT_TYPE_TEXT){
		textFontDelete(&fontIBM);
	}
	guiElementDelete(&gui);
	guiElementCleanup();

	/** THIS IS TEMPORARY **/
	debugDrawCleanup();

	#ifdef MODULE_PARTICLE
	puts("Cleaning up particles...");
	moduleParticleCleanup();
	#endif
	#ifdef MODULE_OBJECT
	puts("Cleaning up objects...");
	moduleObjectCleanup();
	#endif
	#ifdef MODULE_MODEL
	puts("Cleaning up models...");
	moduleModelCleanup();
	#endif
	#ifdef MODULE_PHYSICS
	puts("Cleaning up physics...");
	modulePhysicsCleanup();
	#endif
	#ifdef MODULE_SKELETON
	puts("Cleaning up skeletons...");
	moduleSkeletonCleanup();
	#endif
	#ifdef MODULE_TEXGROUP
	puts("Cleaning up texture groups...");
	moduleTexGroupCleanup();
	#endif
	#ifdef MODULE_TEXTURE
	puts("Cleaning up textures...");
	moduleTextureCleanup();
	#endif
	#ifdef MODULE_COMMAND
	puts("Cleaning up commands...");
	moduleCommandCleanup();
	#endif
	printf("\n");

	memTreePrintAllSizes(&g_memManager);
	memoryManagerGlobalDelete();
	puts("Cleanup complete!\n");
}