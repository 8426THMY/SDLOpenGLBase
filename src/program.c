#include "program.h"


#include <stdio.h>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include "vector.h"
#include "settingsProgram.h"


#include "moduleTexture.h"
#include "moduleTextureGroup.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "modulePhysics.h"
#include "renderObject.h"


#define MODULE_SETUP_SUCCESS 0


//Forward-declare any helper functions!
static void input(program *prg);
static void updateCameras(program *prg);
static void updateRenderObjects(program *prg);
static void update(program *prg);
static void render(program *prg);

static return_t initLibs(program *prg);
static return_t setupModules();
static void cleanupModules();
static void initResources();


#warning "This is just used for testing previous states."
static size_t renderState = 0;


return_t programInit(program *prg){
	prg->windowWidth = 640;
	prg->windowHeight = 480;

	prg->running = 1;

	prg->mouseX = 0;
	prg->mouseY = 0;
	prg->keyStates = SDL_GetKeyboardState(NULL);

	mat4InitZero(&prg->projectionMatrix);
	cameraInit(&prg->cam);
	cameraStateSetPos(prg->cam.states[0], 0.f, 0.f, 5.f);

	fpsInit(&prg->framerate, UPDATE_RATE, FRAME_RATE);


	if(initLibs(prg) && setupModules() == MODULE_SETUP_SUCCESS){
		initResources(prg);

		return(1);
	}

	return(0);
}

void programLoop(program *prg){
	//FPS-independent logic.
	unsigned int updates = 0;
	unsigned int renders = 0;

	Uint32 nextPrint = SDL_GetTicks();
	float nextUpdate = nextPrint;
	float nextRender = nextUpdate;
	nextPrint += 1000;
	while(prg->running){
		Uint32 startTime;
		//Note: This loop freezes the game if our input and update functions take longer than updateTime.
		while((startTime = SDL_GetTicks()) >= nextUpdate){
			input(prg);
			update(prg);

			nextUpdate += prg->framerate.updateTime;
			++updates;
		}

		//Make sure we don't exceed our framerate cap!
		if(prg->framerate.renderRate <= 0.f || (startTime = SDL_GetTicks()) >= nextRender){
			//Determine the interpolation period for when we render the scene!
			prg->framerate.renderDelta = (startTime - (nextUpdate - prg->framerate.updateTime)) / prg->framerate.updateTime;
			if(prg->framerate.renderDelta < 0.f){
				prg->framerate.renderDelta = 0.f;
			}else if(prg->framerate.renderDelta > 1.f){
				prg->framerate.renderDelta = 1.f;
			}

			render(prg);

			nextRender += prg->framerate.renderTime;
			/** We use this block to stop it from exceeding the framerate if   **/
			/** there was a lag spike. Unfortunately, it misbehaves with very  **/
			/** high framerates. We should be able to fix this by using a high **/
			/** resolution timer, such as SDL_GetPerformanceCounter().         **/
			/*if(startTime > nextRender){
				nextRender = startTime;
			}*/
			++renders;
		}


		//Print our update rate and framerate every second!
		if((startTime = SDL_GetTicks()) > nextPrint){
			nextPrint = startTime + 1000;

			printf("Updates: %u\n", updates);
			printf("Renders: %u\n", renders);

			updates = 0;
			renders = 0;
		}
	}
}

void programClose(program *prg){
	cleanupModules();

	SDL_DestroyWindow(prg->window);
	SDL_Quit();
}


/** Note: Some of this stuff should be inside the update function! **/
static void input(program *prg){
	//If the aspect ratio doesn't match the window size you get weird results, especially if you haven't resized the window.
	//Additionally, you may need to recalculate the perspective matrix after changing the window size (or aspect ratio, preferably).
	int tempWidth = prg->windowWidth;
	int tempHeight = prg->windowHeight;
	SDL_GetWindowSize(prg->window, &prg->windowWidth, &prg->windowHeight);

	//If the window has been resized, resize the viewport!
	if(prg->windowWidth != tempWidth || prg->windowHeight != tempHeight){
		//Make sure we preserve the aspect ratio!
		tempWidth = prg->windowWidth / ASPECT_RATIO_X;
		tempHeight = prg->windowHeight / ASPECT_RATIO_Y;
		if(tempWidth < tempHeight){
			tempHeight = tempWidth * ASPECT_RATIO_Y;
			tempWidth  = prg->windowWidth;
		}else if(tempWidth > tempHeight){
			tempWidth  = tempHeight * ASPECT_RATIO_X;
			tempHeight = prg->windowHeight;
		}else{
			tempWidth = prg->windowWidth;
			tempHeight = prg->windowHeight;
		}

		glViewport((prg->windowWidth - tempWidth) / 2, (prg->windowHeight - tempHeight) / 2, tempWidth, tempHeight);
	}


	//Refresh SDL2 events!
	SDL_Event event;
	while(SDL_PollEvent(&event) != 0){
		switch(event.type){
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&prg->mouseX, &prg->mouseY);
				//SDL_WarpMouseInWindow(prg->window, (prg->windowWidth / 2), (prg->windowHeight / 2));
			break;


			case SDL_QUIT:
				prg->running = 0;
			break;
		}
	}


	//Refresh SDL2 key presses! PollEvent should do this automatically, but we'll do it again just in case.
	SDL_PumpEvents();

	if(prg->keyStates[SDL_SCANCODE_ESCAPE]){
		prg->running = 0;
	}
	if(prg->keyStates[SDL_SCANCODE_R]){
		renderState = NUM_LOOKBACK_STATES - 1;
	}else{
		renderState = 0;
	}
}

static void updateCameras(program *prg){
	cameraStateShift(&prg->cam);

	//If our camera exists, react to the user's input and move it!
	if(prg->cam.states[0] != NULL){
		if(prg->keyStates[SDL_SCANCODE_LEFT]){
			cameraStateAddPosX(prg->cam.states[0], -0.25f, prg->framerate.updateDelta);
		}
		if(prg->keyStates[SDL_SCANCODE_RIGHT]){
			cameraStateAddPosX(prg->cam.states[0], 0.25f, prg->framerate.updateDelta);
		}

		if(prg->keyStates[SDL_SCANCODE_UP]){
			cameraStateAddPosY(prg->cam.states[0], 0.25f, prg->framerate.updateDelta);
		}
		if(prg->keyStates[SDL_SCANCODE_DOWN]){
			cameraStateAddPosY(prg->cam.states[0], -0.25f, prg->framerate.updateDelta);
		}

		if(prg->keyStates[SDL_SCANCODE_W]){
			cameraStateAddPosZ(prg->cam.states[0], -0.25f, prg->framerate.updateDelta);
		}
		if(prg->keyStates[SDL_SCANCODE_S]){
			cameraStateAddPosZ(prg->cam.states[0], 0.25f, prg->framerate.updateDelta);
		}
	}
}

static void updateRenderObjects(program *prg){
	/*size_t a;
	//Create a new state for all of our renderObjects!
	for(a = 0; a < allRenderObjects.size; ++a){
		renderObject *currentObj = ((renderObject *)vectorGet(&allRenderObjects, a));
		renderObjStateShift(currentObj);

		renderObjState *currentState = (renderObjState *)currentObj->states[0];
		if(currentState != NULL){
			texGroupAnimInstUpdateAnim(&currentState->texGroup, prg->framerate.updateTime);

			if(currentState->skeleObj->numAnims > 0){
				skeletonAnimInst *curAnim = currentState->skeleObj->anims;
				skeletonAnimInst *lastAnim = &curAnim[currentState->skeleObj->numAnims];
				//Update all of the animations!
				do {
					skeleAnimInstUpdate((skeleAnimState *)curAnim->states[0], prg->framerate.updateTime);
				} while(curAnim < lastAnim);

				//Merge all of the animations!
				//

				//Find the bones' global positions!
				skeleObjGenerateRenderState(currentState->skeleObj);
			}
		}
	}*/


	//Update the models' positions and rotations!
	/** Temporary if statement for temporary code. Don't want the program to crash, do we? **/
	/*if(allRenderObjects.size > 2){
		renderObjState *currentObj = ((renderObject *)vectorGet(&allRenderObjects, 1))->states[0];
		interpTransAddRotEulerDeg(&currentObj->transform, 0.f, 0.f, 2.f, prg->framerate.updateDelta);

		currentObj = ((renderObject *)vectorGet(&allRenderObjects, 2))->states[0];
		interpTransAddRotEulerDeg(&currentObj->transform, 2.f, 2.f, 2.f, prg->framerate.updateDelta);
	}*/
}

/** Don't forget, some modules may not always be loaded! **/
static void update(program *prg){
	updateCameras(prg);
	updateRenderObjects(prg);
}

static void render(program *prg){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/** Temporary check to see if the current renderState exists for our one and only camera. **/
	if(prg->cam.states[renderState] != NULL){
		mat4 viewMatrix, viewProjectionMatrix;
		vec3 target, up;
		vec3InitZero(&target);
		vec3InitSet(&up, 0.f, 1.f, 0.f);
		vec3 camPos;
		interpVec3GenRenderState(&((cameraState *)prg->cam.states[renderState])->pos, prg->framerate.renderDelta, &camPos);
		//Generate a view matrix that looks from the camera to target!
		mat4LookAt(&viewMatrix, &camPos, &target, &up);
		//Multiply it by the projection matrix!
		mat4MultiplyByMat4Out(&prg->projectionMatrix, &viewMatrix, &viewProjectionMatrix);
		/*mat4 viewProjectionMatrix = prg->projectionMatrix;
		cameraStateGenerateViewMatrix((cameraState *)prg->cam.states[renderState], prg->framerate.renderDelta, &viewProjectionMatrix);*/

		/*size_t i;
		//Draw our objects!
		for(i = 0; i < allRenderObjects.size; ++i){
			renderObject *currentObj = (renderObject *)vectorGet(&allRenderObjects, i);
			if(renderState < currentObj->numStates && currentObj->states[renderState] != NULL){
				renderObjStateDraw(currentObj->states[renderState], NULL, &viewProjectionMatrix, &prg->shaderProgram, prg->framerate.renderDelta);
			}
		}*/
	}


	SDL_GL_SwapWindow(prg->window);
}


static return_t initLibs(program *prg){
	//Initialize the SDL2 video subsystem!
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("Unable to initialize SDL2 video subsystem!\n"
		       "Error: %s\n", SDL_GetError());
		return(0);
	}

	//Initialize the SDL2 Image library!
	if(!IMG_Init(IMG_INIT_PNG)){
		printf("Unable to initialize SDL2 Image library!\n"
		       "Error: %s\n", IMG_GetError());
		return(0);
	}

	//Create a window using SDL2!
	prg->window = SDL_CreateWindow("NewSDLOpenGLBaseC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, prg->windowWidth, prg->windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!prg->window){
		printf("Unable to create SDL2 window!\n"
		       "Error: %s\n", SDL_GetError());
		return(0);
	}

	//Create an OpenGL context using SDL2!
	if(!SDL_GL_CreateContext(prg->window)){
		printf("Unable to create an OpenGL context!\n"
		       "Error: %s\n", SDL_GetError());
		return(0);
	}

	//SDL_ShowCursor(SDL_DISABLE);
	SDL_GL_SetSwapInterval(0);


	//Initialize the GLEW library!
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("Unable to initialize GLEW!\n"
		       "Error: %s\n", glewGetErrorString(glewError));
		return(0);
	}

	glewExperimental = GL_TRUE;


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);


	//Load, compile and attach our OpenGL shaders!
	if(!shaderLoad(&prg->shaderProgram, ".\\resource\\shaders\\vertexShader.gls", ".\\resource\\shaders\\fragmentShader.gls")){
		return(0);
	}

	//Create the perspective matrix and get its uniform variable ID in the shader.
	mat4Perspective(&prg->projectionMatrix, ((cameraState *)prg->cam.states[0])->fov.next * M_PI / 180.f, ((float)prg->windowWidth / (float)prg->windowHeight), 0.1f, 100.f);
	//mat4Orthographic(&prg->projectionMatrix, -5.f, 5.f, -5.f, 5.f, 0.1f, 100.f);


	return(1);
}

#warning "We're testing physics now, so this stuff isn't necessary."
static void initResources(){
	/*skeleAnimLoadSMD(moduleSkeletonAnimAlloc(), "soldier_animations_anims\\jump_float_PRIMARY.smd");
	//skeleAnimLoadSMD(moduleSkeletonAnimAlloc(), "soldier_animations_anims\\selectionMenu_Anim0l.smd");
	//skeleAnimLoadSMD(moduleSkeletonAnimAlloc(), "soldier_animations_anims\\competitive_winnerstate_idle.smd");
	//skeleAnimLoadSMD(moduleSkeletonAnimAlloc(), "scout_animations_anims\\a_runN_PRIMARY.smd");
	//skeleAnimLoadSMD(moduleSkeletonAnimAlloc(), "scout_animations_anims\\selectionMenu_Anim01.smd");
	//Someday, this function won't need to exist.
	//Load all of the models we're using!
	modelLoadOBJ(moduleModelAlloc(), "drNeoCortex.obj");
	modelLoadOBJ(moduleModelAlloc(), "nTrance.obj");
	modelLoadOBJ(moduleModelAlloc(), "neoTwin.obj");
	modelLoadOBJ(moduleModelAlloc(), "cubeQuads.obj");
	modelLoadSMD(moduleModelAlloc(), "soldier_reference.smd");*/
	/** The Scout's arms don't work properly because the model's **/
	/** skeleton has more bones than the animations' skeletons.  **/
	//modelLoadSMD(moduleModelAlloc(), "scout_reference.smd");

	//Create renderObjects to represent the models we want to draw!
	/*renderObjCreate(5);
	renderObjState *currentObj = ((renderObject *)vectorGet(&allRenderObjects, allRenderObjects.size - 1))->states[0];
	if(loadedSkeleAnims.size > 0){
		skeleObjAddAnim(currentObj->skeleObj, (skeletonAnim *)vectorGet(&loadedSkeleAnims, 0));
	}
	interpTransSetPosX(&currentObj->transform, -2.f);


	renderObjCreate(2);
	currentObj = ((renderObject *)vectorGet(&allRenderObjects, allRenderObjects.size - 1))->states[0];
	interpTransSetPosX(&currentObj->transform, 2.f);

	renderObjCreate(3);
	currentObj = ((renderObject *)vectorGet(&allRenderObjects, allRenderObjects.size - 1))->states[0];
	interpTransSetPosY(&currentObj->transform, -2.f);*/
}

//Set up the allocators for our modules.
#warning "What if we aren't using the global memory manager?"
static return_t setupModules(){
	puts("Beginning setup...\n");
	memoryManagerGlobalInit(MEMORY_HEAPSIZE);

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
	#ifdef MODULE_RENDEROBJ
	if(!moduleRenderObjSetup()){
		return(MODULE_RENDEROBJ_SETUP_FAIL);
	}
	#endif

	memTreePrintAllSizes(&memManager);
	puts("Setup complete!\n");


	return(MODULE_SETUP_SUCCESS);
}

static void cleanupModules(){
	puts("Beginning cleanup...\n");
	memTreePrintAllSizes(&memManager);

	#ifdef MODULE_RENDEROBJ
	moduleRenderObjCleanup();
	#endif
	#ifdef MODULE_MODEL
	moduleModelCleanup();
	#endif
	#ifdef MODULE_PHYSICS
	modulePhysicsCleanup();
	#endif
	#ifdef MODULE_SKELETON
	moduleSkeletonCleanup();
	#endif
	#ifdef MODULE_TEXGROUP
	moduleTexGroupCleanup();
	#endif
	#ifdef MODULE_TEXTURE
	moduleTextureCleanup();
	#endif

	memTreePrintAllSizes(&memManager);
	memoryManagerGlobalDelete();
	puts("Cleanup complete!\n");
}