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
renderableDef *moduleRenderableDefPrepend(renderableDef **start);
renderableDef *moduleRenderableDefAppend(renderableDef **start);
renderableDef *moduleRenderableDefInsertBefore(renderableDef **start, renderableDef *prevData);
renderableDef *moduleRenderableDefInsertAfter(renderableDef **start, renderableDef *data);
void moduleRenderableDefFree(renderableDef **start, renderableDef *renderDef, renderableDef *prevData);
void moduleRenderableDefFreeArray(renderableDef **start);
void moduleRenderableDefClear();

renderable *moduleRenderableAlloc();
renderable *moduleRenderablePrepend(renderable **start);
renderable *moduleRenderableAppend(renderable **start);
renderable *moduleRenderableInsertBefore(renderable **start, renderable *prevData);
renderable *moduleRenderableInsertAfter(renderable **start, renderable *data);
void moduleRenderableFree(renderable **start, renderable *render, renderable *prevData);
void moduleRenderableFreeArray(renderable **start);
void moduleRenderableClear();


extern memorySingleList g_renderableDefManager;
extern memorySingleList g_renderableManager;


#endif