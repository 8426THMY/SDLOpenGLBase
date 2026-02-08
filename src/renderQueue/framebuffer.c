#include "framebuffer.h"


// This is the ID of the framebuffer that's currently bound.
GLuint curFramebufferID = FRAMEBUFFER_DEFAULT_ID;
framebufferViewport curViewport = {.x = 0.f, .y = 0.f, .w = 0.f, .h = 0.f};


void framebufferInit(framebuffer *const restrict fb){
	fb->id = FRAMEBUFFER_DEFAULT_ID;
}

void framebufferBind(
	const framebuffer *const restrict fb,
	const framebufferViewport *const restrict viewport
){

	// If the new framebuffer isn't currently bound,
	// we should bind it and the new viewport.
	if(fb->id != curFramebufferID){
		curFramebufferID = fb->id;
		curViewport = *viewport;
		glBindFramebuffer(GL_FRAMEBUFFER, curFramebufferID);
		glViewport(curViewport.x, curViewport.y, curViewport.w, curViewport.h);

	// Otherwise, if only the viewport changed, update it!
	}else if(viewport != curViewport){
		curViewport = *viewport;
		glViewport(curViewport.x, curViewport.y, curViewport.w, curViewport.h);
	}
}

void framebufferDelete(framebuffer *const restrict fb){
	if(fb->id != FRAMEBUFFER_DEFAULT_ID){
		glDeleteFramebuffers(1, &fb->id);
	}
}