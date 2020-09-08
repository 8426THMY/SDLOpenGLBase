#ifndef moduleRenderable_h
#define moduleRenderable_h


#include "utilTypes.h"

#include "memoryManager.h"
#include "memorySingleList.h"

#include "renderable.h"


#define MODULE_RENDERABLE
#define MODULE_RENDERABLE_SETUP_FAIL 6

#define MODULE_RENDERABLEDEF_ELEMENT_SIZE sizeof(renderableDef)
#define MODULE_RENDERABLE_ELEMENT_SIZE    sizeof(renderable)

#ifndef MEMORY_MODULE_NUM_RENDERABLEDEFS
	#define MEMORY_MODULE_NUM_RENDERABLEDEFS 1
#endif
#ifndef MEMORY_MODULE_NUM_RENDERABLES
	#define MEMORY_MODULE_NUM_RENDERABLES 1
#endif

#define MODULE_RENDERABLEDEF_MANAGER_SIZE memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_RENDERABLEDEFS, MODULE_RENDERABLEDEF_ELEMENT_SIZE)
#define MODULE_RENDERABLE_MANAGER_SIZE    memSingleListMemoryForBlocks(MEMORY_MODULE_NUM_RENDERABLES, MODULE_RENDERABLE_ELEMENT_SIZE)


return_t moduleRenderableSetup();
void moduleRenderableCleanup();

renderableDef *moduleRenderableDefAlloc();
renderableDef *moduleRenderableDefPrepend(renderableDef **const restrict start);
renderableDef *moduleRenderableDefAppend(renderableDef **const restrict start);
renderableDef *moduleRenderableDefInsertBefore(renderableDef **const restrict start, renderableDef *const restrict prevData);
renderableDef *moduleRenderableDefInsertAfter(renderableDef **const restrict start, renderableDef *const restrict data);
renderableDef *moduleRenderableDefNext(const renderableDef *const restrict renderDef);
void moduleRenderableDefFree(renderableDef **const restrict start, renderableDef *const restrict renderDef, renderableDef *const restrict prevData);
void moduleRenderableDefFreeArray(renderableDef **const restrict start);
void moduleRenderableDefClear();

renderable *moduleRenderableAlloc();
renderable *moduleRenderablePrepend(renderable **const restrict start);
renderable *moduleRenderableAppend(renderable **const restrict start);
renderable *moduleRenderableInsertBefore(renderable **const restrict start, renderable *const restrict prevData);
renderable *moduleRenderableInsertAfter(renderable **const restrict start, renderable *const restrict data);
renderable *moduleRenderableNext(const renderable *const restrict render);
void moduleRenderableFree(renderable **const restrict start, renderable *const restrict render, renderable *const restrict prevData);
void moduleRenderableFreeArray(renderable **const restrict start);
void moduleRenderableClear();


extern memorySingleList g_renderableDefManager;
extern memorySingleList g_renderableManager;


#endif