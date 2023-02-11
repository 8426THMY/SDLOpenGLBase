#include "particleRendererSprite.h"


#include "transform.h"

#include "particleManager.h"


void particleRendererBeamInitBatch(
	const void *const restrict renderer,
	renderBatch *const restrict batch,
){

	if(batch->type != RENDER_BATCH_TYPE_SPRITE){
		// Draw the previous batch if it's still in use.
		if(batch->type != RENDER_BATCH_TYPE_UNUSED){
			renderBatchDraw(batch);
		}
		spriteBatchInit(&batch.sBatch);
		batch->type = RENDER_BATCH_TYPE_SPRITE;

		#warning "Bind any textures or uniforms here!"
		#warning "We also need some way of checking what is already bound."
	}
}

/*
** Fill the buffer objects up with particle data from
** each particle system node in the array specified.
**
** For beams, we use the polyboarding technique presented
** by Eric Lengyel in "Mathematics for 3D Game Programming
** and Computer Graphics, Third Edition".
*/
void particleRendererBeamBatch(
	const void *const restrict renderer,
	const particleManager *const restrict manager,
	renderBatch *const restrict batch,
	const camera *const restrict cam, const float dt
){

	// We can't draw a polyboard with only one point.
	if(manager->numParticles > 1){
		const particleRendererBeam *const beamRenderer =
			(const particleRendererBeam *const)renderer;
		const float halfWidth = beamRenderer->halfWidth;

		const particle *curParticle  = manager->first;
		const particle *prevParticle = curParticle;
		spriteBatch *const sBatch = (spriteBatch *const)&batch->sBatch;
		spriteVertex *curVertex = &sBatch->vertices[sBatch->numVertices];
		spriteIndex *curIndex   = &sBatch->indices[sBatch->numIndices];

		vec3 D;

		// Add polyboard vertices for all but the last particle.
		// While we can handle the first particle in this loop,
		// we'll have to handle the last particle outside of it.
		#error "We have some polyboard testing stuff in 'main.c'!"
		#error "The implementation there may be more efficient."
		while(curParticle != NULL){
			const particle *nextParticle = curParticle->next;
			vec3 Z;
			vec3 T;

			// If the batch is full, draw it and start a new one!
			if(
				sBatch->numVertices + 2 > SPRITE_BUFFER_MAX_VERTICES ||
				sBatch->numIndices  + 2 > SPRITE_BUFFER_MAX_INDICES
			){

				spriteBatchDraw(sBatch);
				spriteBatchInit(sBatch);
				// Reset the vertex and index pointers.
				curVertex = sBatch->vertices;
				curIndex  = sBatch->indices;

				// If we've added some vertices already, we'll need
				// to add the last two back to continue the strip.
				if(curParticle != prevParticle){
					vec3AddVec3Out(&prevParticle->state[0].pos, &D, curVertex);
					++curVertex;
					vec3SubtractVec3Out(&prevParticle->state[0].pos, &D, curVertex);
					++curVertex;

					*curIndex = sBatch->numVertices;
					++curIndex;
					*curIndex = sBatch->numVertices + 1;
					++curIndex;

					sBatch->numVertices += 2;
					sBatch->numIndices  += 2;
				}
			}

			#warning "Temporary, until we know how we want to do billboarding."
			// Z_i = (C - P_i)/||C - P_i||
			vec3SubtractVec3Out(&cam->pos, &curParticle->state[0].pos, &Z);
			vec3NormalizeVec3(&Z);
			// Special case for the last particle in the list.
			if(nextParticle == NULL){
				// T_n = (P_n - P_{n-1})/||P_n - P_{n-1}||
				vec3SubtractVec3Out(&curParticle->state[0].pos, &prevParticle->state[0].pos, &T);
			}else{
				// T_i = (P_{i+1} - P_{i-1})/||P_{i+1} - P_{i-1}||
				vec3SubtractVec3Out(&nextParticle->state[0].pos, &prevParticle->state[0].pos, &T);
			}
			vec3NormalizeVec3(&T);
			// D_i = r*(T_i X Z_i)
			vec3CrossVec3Out(&T, &Z, &D);
			vec3MultiplyS(&D, halfWidth);

			// G_i = P_i + D_i
			vec3AddVec3Out(&curParticle->state[0].pos, &D, &curVertex->pos);
			#warning "Temporary, until we know how we want to do textures."
			vec2InitSet(&curVertex->uv, 0.f, 0.f);
			++curVertex;
			// H_i = P_i - D_i
			vec3SubtractVec3Out(&curParticle->state[0].pos, &D, &curVertex->pos);
			#warning "Temporary, until we know how we want to do textures."
			vec2InitSet(&curVertex->uv, 0.f, 0.f);
			++curVertex;

			*curIndex = sBatch->numVertices;
			++curIndex;
			*curIndex = sBatch->numVertices + 1;
			++curIndex;

			sBatch->numVertices += 2;
			sBatch->numIndices  += 2;

			prevParticle = curParticle;
			curParticle = nextParticle;
		}

		// If we've filled the index buffer,
		// we should draw the batch now.
		if(sBatch->numIndices == SPRITE_BUFFER_MAX_INDICES){
			spriteBatchDraw(sBatch);
			spriteBatchInit(sBatch);

		// Otherwise, we need to add an index for primitive
		// restart, since we're using triangle strips.
		}else{
			*curIndex = SPRITE_PRIMITIVE_RESTART_INDEX;
		}
	}
}