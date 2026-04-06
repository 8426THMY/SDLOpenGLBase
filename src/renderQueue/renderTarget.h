#ifndef renderTarget_h
#define renderTarget_h


#include "framebuffer.h"

#include "camera.h"
#include "renderView.h"


typedef struct renderTarget {
	// Doubly linked list of views
	// drawn to this render target.
	renderView *views;
	// Render views are drawn from last to first.
	renderView *lastView;

	// Views may have their own cameras, but
	// we need a camera for the base view.
	camera cam;
	// Multiple render targets may share the same
	// framebuffer (splitscreen, for example).
	framebuffer *fb;
	framebufferViewport viewport;
} renderTarget;


typedef struct scene scene;
void renderTargetPreDraw(
	const renderTarget *const target,
	scene *const restrict scn, const float dt
);
void renderTargetDraw(const renderTarget *const target);


#warning "Dragonfru and ants suggested a depth first traversal to find render target dependencies."
#warning "This would probably work, but we wouldn't be able to multithread the pre-draw work."
#warning "I wonder if it's worth avoiding the extra framebuffer binds, though?"


#warning "What do we do if a view requires a different render target to be drawn first?"
#warning "This comes in two flavours: sometimes we need to create a new render target for each view (mirrors) and sometimes we can use the same one for all views (Breen monitors)."
#warning "Honestly, the view-based method below seems to be the most graceful."
#if 0
renderTarget {
	renderView *views;
	#warning "There doesn't seem to be a way to avoid storing dependencies."
	renderTarget *dependencies;
	flags_t drawn;
};

sceneDraw(){
	for(each camera){
		sceneAddRenderTarget();
	}
	while(target != NULL){
		renderTargetPreDraw(target){
			while(view != NULL){
				visCullerPopulateRenderQueues(scene, target, view){
					if(object in view){
						renderQueueInsert(object);
						renderObjectPreDraw(object){
							#warning "Objects can create views (which depend on the current render target)."
							if(object creates view){
								renderTargetAddView(target);
							}
							#warning "They can also create new render targets, which the current one depends on."
							if(object creates target){
								if(object.frameID != scene.frameID){
									object.target = sceneAddRenderTarget();
								}
								renderTargetAddDependency(target, object.target);
							}
						}
					}
				}
				view = renderViewNext(view);
			}
		}
		target = renderTargetNext(target);
	}
	for(each target){
		renderTargetDraw(target){
			if(drawn){
				return;
			}
			for(each dependency){
				renderTargetDraw(dependency);
			}

			// Draw stuff.

			drawn = 1;
		}
	}
}
#endif


#warning "Although it doesn't look like it, this should still draw everything in the same order as the current way."
#warning "An equivalent and maybe simpler way of doing this approach is to make the scene store all the views in a stack."

#warning "How do we free any render targets we need to create during this?"
#warning "I guess we can always do it when we're freeing the views."
#if 0
renderView {
	mat3x4 viewMatrix;
	mat4 vpMatrix;
	renderFrustum frustum;

	renderTarget {
		// Multiple render targets may share the same
		// framebuffer (splitscreen, for example).
		framebuffer *fb;
		framebufferViewport viewport;
	} target;

	// For each visible light source, we'll need to store its index
	// and an array of clusters (for forward+). When we're rendering
	// individual objects, we should select a subset of lights that
	// have the greatest impact on the mesh.
	light *lights[RENDER_VIEW_MAX_LIGHTS];
	size_t numLights;

	#warning "For shadow maps, we probably have to do things a bit differently."
	#warning "We only want to draw depth, and we want to ignore certain geometry and translucent objects."
	#warning "Should we have flags for these things?"
	#warning "Shadow maps don't need multiple buckets, so they should have their own structure."
	#warning "Shadow maps should only create subviews for things that use the same render target."

	renderQueue queues[RENDER_VIEW_NUM_BUCKETS];

	// Some render views may require other views to be drawn
	// before them, such as in the case of portals or mirrors.
	renderView *subviews;
};


sceneDraw(){
	for(each camera){
		sceneAddRenderView();
		renderViewInit(){
			visCullerPopulateRenderQueues(){
				if(object in view){
					renderQueueInsert(object);
					renderObjectPreDraw(){
						#warning "Objects can create views that are dependent or independent on the view they're in."
						#warning "In the former case (portals or mirrors), we need to create a subview every time."
						#warning "In the latter case (Breen monitors), we should create one subview and draw it before everything else that needs it."
						#warning "As long as subviews are drawn in post-order and new views are inserted after old ones, this should be fine."
						if(object creates subview){
							if(subview depends on parent || object->frameID != frameID){
								renderViewAddSubView();
							}
						}
					}
				}
			}
			// This will initialize all subviews in post-order.
			for(each subview){
				renderViewInit();
			}
		}
		renderViewPreDraw(){
			// This will pre-draw all subviews in post-order.
			for(each subview){
				renderViewPreDraw();
			}
			// pre-draw stuff
		}
	}
	for(each view){
		renderViewDraw(){
			// This will draw all subviews in post-order.
			for(each subview){
				renderViewDraw();
			}
			// draw stuff
		}
	}
}
#endif


#endif