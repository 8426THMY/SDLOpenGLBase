#include "program.h"


#include <stdio.h>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include "settingsProgram.h"

#include "moduleTexture.h"
#include "moduleTextureGroup.h"
#include "moduleSkeleton.h"
#include "moduleModel.h"
#include "modulePhysics.h"
#include "moduleRenderable.h"
#include "moduleObject.h"

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


#warning "This is just used for testing previous states."
static size_t renderState = 0;


return_t programInit(program *const restrict prg, char *const restrict prgDir){
	prg->windowWidth = 640;
	prg->windowHeight = 480;

	prg->running = 1;

	prg->mouseX = 0;
	prg->mouseY = 0;
	prg->keyStates = SDL_GetKeyboardState(NULL);

	cameraInit(&prg->cam);
	prg->cam.pos.z = 5.f;

	timestepInit(&prg->step, UPDATE_RATE, FRAME_RATE);

	// Set the current working directory. This ensures that
	// we're looking for resources in the correct directory.
	fileSetWorkingDirectory(prgDir, NULL);


	return(initLibs(prg) && setupModules() == MODULE_SETUP_SUCCESS && initResources(prg));
}

void programLoop(program *const restrict prg){
	// FPS-independent logic.
	unsigned int updates = 0;
	unsigned int renders = 0;

	Uint32 nextPrint = SDL_GetTicks();
	float nextUpdate = nextPrint;
	float nextRender = nextUpdate;
	nextPrint += 1000;
	while(prg->running){
		Uint32 startTime;
		// Note: This loop freezes the game if our input and update functions take longer than updateTime.
		while((startTime = SDL_GetTicks()) >= nextUpdate){
			input(prg);
			update(prg);

			nextUpdate += prg->step.updateTime;
			++updates;
		}

		// Make sure we don't exceed our framerate cap!
		if(prg->step.renderRate <= 0.f || (startTime = SDL_GetTicks()) >= nextRender){
			// Determine the interpolation period for when we render the scene!
			prg->step.renderDelta = (startTime - (nextUpdate - prg->step.updateTime)) * prg->step.updateTickrate;
			if(prg->step.renderDelta < 0.f){
				prg->step.renderDelta = 0.f;
			}else if(prg->step.renderDelta > 1.f){
				prg->step.renderDelta = 1.f;
			}

			render(prg);

			nextRender += prg->step.renderTime;
			/** We use this block to stop it from exceeding the framerate if   **/
			/** there was a lag spike. Unfortunately, it misbehaves with very  **/
			/** high framerates. We should be able to fix this by using a high **/
			/** resolution timer, such as SDL_GetPerformanceCounter().         **/
			/*if(startTime > nextRender){
				nextRender = startTime;
			}*/
			++renders;
		}


		// Print our update rate and framerate every second!
		if((startTime = SDL_GetTicks()) > nextPrint){
			nextPrint = startTime + 1000;

			printf("Updates: %u\n", updates);
			printf("Renders: %u\n", renders);

			updates = 0;
			renders = 0;
		}
	}
}

void programClose(program *const restrict prg){
	shaderDeleteProgram(prg->objectShader.programID);
	shaderDeleteProgram(prg->spriteShader.programID);
	cleanupModules();

	SDL_DestroyWindow(prg->window);
	SDL_Quit();
}


/** Note: Some of this stuff should be inside the update function! **/
static void input(program *const restrict prg){
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
		tempX = prg->windowWidth / ASPECT_RATIO_X;
		tempY = prg->windowHeight / ASPECT_RATIO_Y;

		// If the window is too tall, add bars to the top and bottom.
		if(tempX < tempY){
			tempY = tempX * ASPECT_RATIO_Y;
			glViewport(0, (prg->windowHeight - tempY) / 2, prg->windowWidth, tempY);
			prg->windowHeight = tempY;

		// If the window is too wide, add bars to the sides.
		}else if(tempX > tempY){
			tempX = tempY * ASPECT_RATIO_X;
			glViewport((prg->windowWidth - tempX) / 2, 0, tempX, prg->windowHeight);
			prg->windowWidth = tempX;

		// Otherwise, we don't need any bars.
		}else{
			glViewport(0, 0, prg->windowWidth, prg->windowHeight);
		}
	}


	// Refresh SDL2 events!
	SDL_Event event;
	#warning "This is causing microstutters for some reason."
	while(SDL_PollEvent(&event) != 0){
		switch(event.type){
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&prg->mouseX, &prg->mouseY);
				// SDL_WarpMouseInWindow(prg->window, (prg->windowWidth / 2), (prg->windowHeight / 2));
			break;


			case SDL_QUIT:
				prg->running = 0;
			break;
		}
	}

	// Refresh SDL2 key presses! PollEvent should do this automatically, but we'll do it again just in case.
	#warning "So is this."
	SDL_PumpEvents();

	if(prg->keyStates[SDL_SCANCODE_ESCAPE]){
		prg->running = 0;
	}
	if(prg->keyStates[SDL_SCANCODE_R]){
		renderState = NUM_LOOKBACK_STATES - 1;
	}else{
		renderState = 0;
	}

	#warning "Selecting text in the console also causes crashes."
}

/** TEMPORARY PHYSICS STUFF!! **/
object *controlObj = NULL;
physicsRigidBody *controlPhys = NULL;
#include "utilMath.h"
static void updateCameras(program *const restrict prg){
	if(prg->keyStates[SDL_SCANCODE_LEFT]){
		prg->cam.pos.x -= 10.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_RIGHT]){
		prg->cam.pos.x += 10.f * prg->step.updateDelta;
	}

	if(prg->keyStates[SDL_SCANCODE_UP]){
		prg->cam.pos.y += 10.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_DOWN]){
		prg->cam.pos.y -= 10.f * prg->step.updateDelta;
	}

	if(prg->keyStates[SDL_SCANCODE_W]){
		prg->cam.pos.z -= 10.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_S]){
		prg->cam.pos.z += 10.f * prg->step.updateDelta;
	}




	/** TEMPORARY TESTING STUFF! **/
	if(controlObj != NULL){
		quat rot = quatNormalizeQuatC(mat4ToQuatC(mat4RotateToFaceC(controlObj->state.pos, prg->cam.pos, vec3InitSetC(0.f, 1.f, 0.f))));
		vec3 angles = quatToEulerAnglesC(rot);printf("%f, %f, %f\n", angles.x, angles.y, angles.z);


		angles.x = clampFloat(angles.x, -M_PI_4, M_PI_4);
		angles.y = clampFloat(angles.y, -M_PI_4, M_PI_4);
		angles.z = clampFloat(angles.z, 0.f, 0.f);

		rot = quatNormalizeQuatFastC(quatInitEulerVec3RadC(angles));
		rot.w = fabsf(rot.w);


		controlObj->state.rot = rot;
	}
	/** TEMPORARY PHYSICS STUFF! **/
	if(controlPhys != NULL){
		if(prg->keyStates[SDL_SCANCODE_J]){
			vec3 F = vec3InitSetC(-40.f * controlPhys->mass, 0.f, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->keyStates[SDL_SCANCODE_L]){
			vec3 F = vec3InitSetC(40.f * controlPhys->mass, 0.f, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->keyStates[SDL_SCANCODE_I]){
			vec3 F = vec3InitSetC(0.f, 40.f * controlPhys->mass, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->keyStates[SDL_SCANCODE_K]){
			vec3 F = vec3InitSetC(0.f, -40.f * controlPhys->mass, 0.f);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->keyStates[SDL_SCANCODE_U]){
			vec3 F = vec3InitSetC(0.f, 0.f, -40.f * controlPhys->mass);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
		if(prg->keyStates[SDL_SCANCODE_O]){
			vec3 F = vec3InitSetC(0.f, 0.f, 40.f * controlPhys->mass);
			physRigidBodyApplyLinearForce(controlPhys, &F);
		}
	}
}

static void updateObjects(program *const restrict prg){
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, curObj, object)
		objectUpdate(curObj, prg->step.updateTime);
	MEMSINGLELIST_LOOP_END(g_objectManager, curObj)

	// Update the models' positions and rotations!
	/** Temporary if statement for temporary code. Don't want the program to crash, do we? **/
	/*if(allRenderObjects.size > 2){
		renderObjState *currentObj = ((renderObject *)vectorGet(&allRenderObjects, 1))->states[0];
		interpTransAddRotEulerDeg(&currentObj->transform, 0.f, 0.f, 2.f, prg->step.updateDelta);

		currentObj = ((renderObject *)vectorGet(&allRenderObjects, 2))->states[0];
		interpTransAddRotEulerDeg(&currentObj->transform, 2.f, 2.f, 2.f, prg->step.updateDelta);
	}*/
}

/** TEMPORARY STUFF! **/
#include "physicsIsland.h"
physicsIsland island;
static void updatePhysics(program *const restrict prg){
	#ifdef PHYSCONTACT_STABILISER_BAUMGARTE
	physIslandUpdate(&island, prg->step.updateDelta, prg->step.updateRate);
	#else
	physIslandUpdate(&island, prg->step.updateDelta);
	#endif
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
	updateCameras(prg);
	updateObjects(prg);
	updatePhysics(prg);


	/** TEMPORARY PARTICLE UPDATE STUFF! **/
	particleSysUpdate(&partSys, prg->step.updateDelta);


	/** TEMPORARY GUI UPDATE STUFF! **/
	/*if(prg->keyStates[SDL_SCANCODE_LEFT]){
		gui.root.pos.x -= 100.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_RIGHT]){
		gui.root.pos.x += 100.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_UP]){
		gui.root.pos.y += 100.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_DOWN]){
		gui.root.pos.y -= 100.f * prg->step.updateDelta;
	}

	if(prg->keyStates[SDL_SCANCODE_A]){
		gui.root.scale.x -= 100.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_D]){
		gui.root.scale.x += 100.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_W]){
		gui.root.scale.y += 100.f * prg->step.updateDelta;
	}
	if(prg->keyStates[SDL_SCANCODE_S]){
		gui.root.scale.y -= 100.f * prg->step.updateDelta;
	}*/

	guiElementUpdate(&gui, prg->step.updateTime);
}

static void render(program *const restrict prg){
	#warning "We should stop clearing the colour buffer eventually."
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	cameraUpdateViewProjectionMatrix(&prg->cam, prg->windowWidth, prg->windowHeight);

	glUseProgram(prg->objectShader.programID);
	MEMSINGLELIST_LOOP_BEGIN(g_objectManager, curObj, object)
		#warning "We'll need the camera in this function for billboards. Just pass it instead of the matrix."
		objectDraw(curObj, &prg->cam, &prg->objectShader, prg->step.renderDelta);
	MEMSINGLELIST_LOOP_END(g_objectManager, curObj)

	/** TEMPORARY PARTICLE RENDER STUFF! **/
	glUseProgram(prg->spriteShader.programID);
	//particleSysDraw(&partSys, &prg->cam, &prg->spriteShader, prg->step.renderDelta);

	/** TEMPORARY GUI RENDER STUFF! **/
	glUseProgram(prg->spriteShader.programID);
	/** Do we need this? **/
	glClear(GL_DEPTH_BUFFER_BIT);
	//guiElementDraw(&gui, prg->windowWidth, prg->windowHeight, &prg->spriteShader);


	SDL_GL_SwapWindow(prg->window);
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

	// Initialize the SDL2 Image library!
	if(!IMG_Init(IMG_INIT_PNG)){
		printf(
			"Unable to initialize SDL2 Image library!\n"
			"Error: %s\n", IMG_GetError()
		);
		return(0);
	}

	// Create a window using SDL2!
	prg->window = SDL_CreateWindow("NewSDLOpenGLBaseC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, prg->windowWidth, prg->windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if(!prg->window){
		printf(
			"Unable to create SDL2 window!\n"
			"Error: %s\n", SDL_GetError()
		);
		return(0);
	}

	// Create an OpenGL context using SDL2!
	if(!SDL_GL_CreateContext(prg->window)){
		printf(
			"Unable to create an OpenGL context!\n"
			"Error: %s\n", SDL_GetError()
		);
		return(0);
	}

	// SDL_ShowCursor(SDL_DISABLE);
	SDL_GL_SetSwapInterval(0);


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

	#warning "Don't enable this unless the object is translucent."
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);


	return(1);
}

static return_t initResources(program *const restrict prg){
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


	/** TEMPORARY OBJECT STUFF **/
	model *mdl;
	renderableDef *renderDef;// = moduleRenderableDefAlloc();
	objectDef *objDef;// = moduleObjectDefAlloc();
	object *obj;// = moduleObjectAlloc();
	skeletonAnimDef *animDef;
	#if 0
	model *mdl;
	renderableDef *renderDef = moduleRenderableDefAlloc();
	objectDef *objDef = moduleObjectDefAlloc();
	object *obj = moduleObjectAlloc();
	skeletonAnimDef *animDef;


	mdl = modelSMDLoad("soldier_reference.smd", sizeof("soldier_reference.smd"));
	renderableDefInit(renderDef, mdl);
	objectDefInit(objDef);
	objDef->skele = mdl->skele;
	objDef->renderables = renderDef;

	objectInit(obj, objDef);
	//obj->state.pos = vec3InitSetC(1.f, 2.f, 3.f);
	//obj->state.rot = quatInitEulerDegC(45.f, 10.f, 23.f);
	//obj->state.scale.z = 0.1f;
	// Temporary object stuff.
	//mdl = modelSMDLoad(mdl, "scout_reference.smd");
	//skeleObjInit(&obj->skeleData, mdl->skele);
	controlObj = obj;

	// Temporary animation stuff.
	#warning "Playing 'soldier_animations_anims_old/a_runN_LOSER.smd' on the Scout makes his left arm flip."
	animDef = skeleAnimSMDLoad("soldier_animations_anims_old/a_runN_MELEE.smd", sizeof("soldier_animations_anims_old/a_runN_MELEE.smd"));
	if(animDef != NULL){
		obj->skeleData.anims = moduleSkeletonAnimPrepend(&obj->skeleData.anims);
		skeleAnimInit(obj->skeleData.anims, animDef, 0.5f);
	}

	animDef = skeleAnimSMDLoad("soldier_animations_anims_old/stand_MELEE.smd", sizeof("soldier_animations_anims_old/stand_MELEE.smd"));
	if(animDef != NULL){
		obj->skeleData.anims = moduleSkeletonAnimPrepend(&obj->skeleData.anims);
		skeleAnimInit(obj->skeleData.anims, animDef, 0.5f);
	}
	#endif


	/** TEMPORARY PHYSICS STUFF **/
	#warning "We kind of need to do joints."
	#warning "An input manager would be neat, too."
	physIslandInit(&island);
	// Create the base physics object.
	objDef = moduleObjectDefAlloc();
	objectDefInit(objDef);
	physRigidBodyDefLoad(&objDef->physBodies, "cube.tdp", sizeof("cube.tdp"));
	objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(size_t));
	*objDef->physBoneIDs = 0;
	objDef->numBodies = 1;
	renderDef = moduleRenderableDefAlloc();
	renderableDefInit(renderDef, &g_mdlDefault);
	objDef->renderables = renderDef;

	// Set up an instance.
	obj = moduleObjectAlloc();
	objectInit(obj, objDef);
	physIslandInsertRigidBody(&island, obj->physBodies);
	printf("Ground: %u -> %u\n", obj->physBodies, obj->physBodies->colliders);
	obj->state.pos.y = -4.f;
	obj->state.scale.x = obj->state.scale.z = 100.f;
	physRigidBodySetScale(obj->physBodies, vec3InitSetC(20.f, 0.f, 20.f));
	physRigidBodyIgnoreLinear(obj->physBodies);physRigidBodyIgnoreSimulation(obj->physBodies);
	obj->physBodies->mass = 0.f;
	mat3InitZero(&obj->physBodies->invInertiaLocal);mat3InitZero(&obj->physBodies->invInertiaGlobal);
	obj->physBodies->invMass = 0.f;
	objectPreparePhysics(obj);

	/** EVEN MORE TEMPORARY PHYSICS STUFF **/
	// Create the base physics object.
	mdl = modelOBJLoad("cubeQuads.obj", sizeof("cubeQuads.obj"));
	objDef = moduleObjectDefAlloc();
	objectDefInit(objDef);
	physRigidBodyDefLoad(&objDef->physBodies, "cube.tdp", sizeof("cube.tdp"));
	objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(size_t));
	*objDef->physBoneIDs = 0;
	objDef->numBodies = 1;
	renderDef = moduleRenderableDefAlloc();
	renderableDefInit(renderDef, mdl);
	objDef->renderables = renderDef;

	size_t i;
	// Set up instances.
	for(i = 0; i < 1; ++i){
		obj = moduleObjectAlloc();
		objectInit(obj, objDef);
		physIslandInsertRigidBody(&island, obj->physBodies);
		printf("Cube %u: %u -> %u\n", i, obj->physBodies, obj->physBodies->colliders);
		/*if(i == 0){
			controlPhys = obj->physBodies;
			physRigidBodyIgnoreAngular(obj->physBodies);
		}*/
		obj->state.pos.y = ((float)i)*2.f;
		objectPreparePhysics(obj);
	}

	/** MORE TEMPORARY PHYSICS STUFF **/
	// Create the base physics object.
	objDef = moduleObjectDefAlloc();
	objectDefInit(objDef);
	physRigidBodyDefLoad(&objDef->physBodies, "egg.tdp", sizeof("egg.tdp"));
	objDef->physBoneIDs = memoryManagerGlobalAlloc(sizeof(size_t));
	*objDef->physBoneIDs = 0;
	objDef->numBodies = 1;
	renderDef = moduleRenderableDefAlloc();
	renderableDefInit(renderDef, mdl);
	objDef->renderables = renderDef;

	// Set up an instance.
	obj = moduleObjectAlloc();
	objectInit(obj, objDef);
	physIslandInsertRigidBody(&island, obj->physBodies);
	printf("Egg: %u -> %u\n", obj->physBodies, obj->physBodies->colliders);
	obj->state.pos.y = 0.f;obj->state.pos.z = -2.f;
	quatInitEulerDeg(&obj->state.rot, 0.f, 45.f, 45.f);

	physRigidBodyIgnoreLinear(obj->physBodies);physRigidBodyIgnoreSimulation(obj->physBodies);
	obj->physBodies->mass = 0.f;
	mat3InitZero(&obj->physBodies->invInertiaLocal);mat3InitZero(&obj->physBodies->invInertiaGlobal);
	obj->physBodies->invMass = 0.f;

	objectPreparePhysics(obj);


	/** EVEN MORE TEMPORARY PARTICLE STUFF **/
	spriteSetupDefault();

	particleSysDefInit(&partSysDef);
	partSysDef.maxParticles = SPRITE_MAX_INSTANCES;
	partSysDef.initializers = partSysDef.lastInitializer = memoryManagerGlobalAlloc(sizeof(*partSysDef.initializers));
	partSysDef.initializers->func = &particleInitializerRandomPosSphere;
	partSysDef.emitters = memoryManagerGlobalAlloc(sizeof(*partSysDef.emitters));
	partSysDef.emitters->func = &particleEmitterContinuous;
	partSysDef.numEmitters = 1;
	partSysDef.operators = partSysDef.lastOperator = memoryManagerGlobalAlloc(sizeof(*partSysDef.operators));
	partSysDef.operators->func = &particleOperatorAddGravity;

	particleSysInit(&partSys, &partSysDef);


	/** TEMPORARY FONT STUFF **/
	const texture *atlasArray[2] = {
		textureLoad("gui/PxPlusIBMBIOS.0.tdt", sizeof("gui/PxPlusIBMBIOS.0.tdt")),
		textureLoad("gui/PxPlusIBMBIOS.1.tdt", sizeof("gui/PxPlusIBMBIOS.1.tdt"))
	};
	//textFontLoad(&fontIBM, "./resource/fonts/PxPlus_IBM_BIOS.tdf");
	textFontLoad(&fontIBM, SPRITE_IMAGE_TYPE_MSDF, atlasArray, "./resource/fonts/PxPlus_IBM_BIOS.ttf", "./resource/fonts/PxPlus_IBM_BIOS-msdf-temp.csv");

	guiElementInit(&gui, GUI_ELEMENT_TYPE_TEXT);
	gui.root.pos.x = -320.f;
	gui.root.pos.y = 240.f;
	gui.root.scale.x = gui.root.scale.y = 0.5f;
	guiTextInit(&gui.data.text, &fontIBM, sizeof("The quick brown fox jumps over the lazy dog!"));
	textBufferWrite(&gui.data.text.buffer, "The quick brown fox jumps over the lazy dog!", sizeof("The quick brown fox jumps over the lazy dog!"));

	gui.data.text.width  = 640.f;
	gui.data.text.height = 480.f;


	/** TEMPORARY PANEL STUFF **/
	/*guiElementInit(&gui, GUI_ELEMENT_TYPE_PANEL);
	gui.root.pos.x = -320.f;
	gui.root.pos.y = 240.f;
	gui.root.scale.x = gui.root.scale.y = 100.f;
	guiPanelInit(&gui.data.panel);

	gui.data.panel.borderTexState.texGroup = texGroupLoad("gui/border.tdg", sizeof("gui/border.tdg"));
	gui.data.panel.bodyTexState.texGroup   = texGroupLoad("gui/body.tdg", sizeof("gui/body.tdg"));

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

	flagsSet(gui.data.panel.flags, GUI_PANEL_TILE_BODY);*/


	return(1);
}

// Set up the allocators for our modules.
#warning "What if we aren't using the global memory manager?"
static return_t setupModules(){
	puts("Beginning setup...\n");
	memoryManagerGlobalInit(MEMORY_HEAPSIZE);

	#ifdef MODULE_SHADER
	if(!moduleShaderSetup()){
		return(MODULE_SHADER_SETUP_FAIL);
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
	#ifdef MODULE_RENDERABLE
	if(!moduleRenderableSetup()){
		return(MODULE_RENDERABLE_SETUP_FAIL);
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
	/** YET MORE TEMPORARY GUI STUFF **/
	if(gui.type == GUI_ELEMENT_TYPE_TEXT){
		textFontDelete(&fontIBM);
	}
	guiElementDelete(&gui);
	#ifdef MODULE_PARTICLE
	moduleParticleCleanup();
	#endif
	#ifdef MODULE_OBJECT
	moduleObjectCleanup();
	#endif
	#ifdef MODULE_RENDERABLE
	moduleRenderableCleanup();
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
	#ifdef MODULE_SHADER
	moduleShaderCleanup();
	#endif

	/** THIS IS TEMPORARY **/
	debugDrawCleanup();

	memTreePrintAllSizes(&g_memManager);
	memoryManagerGlobalDelete();
	puts("Cleanup complete!\n");
}