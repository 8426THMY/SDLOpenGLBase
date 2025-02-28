#include "particleRendererSprite.h"


#include "cubicSpline.h"
#include "transform.h"

#include "particleManager.h"

#include "memoryManager.h"


// Forward-declare any helper functions!
static void polyboardSetupSpline(
	cubicSpline *const restrict spline,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const float dt
);
static void polyboardAddVertices(
	spriteRendererBatched *const restrict batchedRenderer,
	const vec3 *const restrict curPos,
	const vec3 *const restrict prevPos,
	const vec3 *const restrict nextPos,
	const vec3 *const restrict camPos,
	const float r, const float tileWidth,
	spriteVertex *const restrict G, spriteVertex *const restrict H
);


void particleRendererBeamInitBatch(
	const void *const restrict renderer,
	spriteRenderer *const restrict batch,
){

	// Draw the old batch if it isn't compatible with the new one!
	#warning "We need to check the textures in use here."
	#warning "Renderers should probably have a shared component that lets us check compatibility easily."
	if(batch->type != SPRITE_RENDERER_TYPE_BATCHED){
		#error "This is all probably incorrect. Same for the other renderers."
		spriteRendererDraw(batch);
		spriteRendererInit(batch, SPRITE_RENDERER_TYPE_BATCHED);

		#warning "Bind any textures or uniforms here!"
	}
}

/*
** Return the number of indices we'll need for the batch.
** This is really just the number of vertices, though we
** add an extra index for primitive restart.
*/
size_t particleRendererBeamBatchSize(
	const void *const restrict renderer, const size_t numParticles
){

	return(
		2*(numParticles + (numParticles - 1) *
		((const particleRendererBeam *const)renderer)->subdivisions) + 1
	);
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
	const particleRenderer *const restrict renderer,
	spriteRenderer *const restrict batch,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const camera *const restrict cam, const float dt
){

	// A beam renderer needs at least two particles to draw.
	if(numParticles >= 2){
		const particleRendererBeam partRenderer =
			*((const particleRendererBeam *const)renderer);
		// This is how much we should increment our
		// spline parameter t for each subdivision.
		const float subdivInc = 1.f/((float)(partRenderer.subdivisions + 1);
		// Correct the tile width to account for subdivisions.
		// If the tile width is set to zero, we should make
		// the texture span the entire polyboard once.
		const float tileWidthInc = (partRenderer.tileWidth == 0.f) ?
			subdivInc/((float)(numParticles - 1)) :
			subdivInc*partRenderer.tileWidth;

		cubicSpline spline;
		size_t curParticle = 0;

		vec3 curPos;
		vec3 prevPos;
		const vec3 *const restrict camPos = camera->pos;
		float tileWidth = 0.f;
		spriteVertex G, H;

		spriteRendererBatched *const batchedRenderer = &batch->data.batchedRenderer;


		// If the batch is full, draw it and start a new one!
		// We always add two vertices and indices at a time.
		if(spriteRendererBatchedHasRoom(batchedRenderer, 2, 2)){
			spriteRendererBatchedDraw(batchedRenderer);
			spriteRendererBatchedOrphan(batchedRenderer);
		}

		polyboardSetupSpline(&spline, keyValues, numParticles, dt);
		// The current and previous positions should
		// default to the first point on the spline.
		curPos  = spline.d[i];
		prevPos = curPos;

		// Construct polyboard vertices for each particle except the last.
		for(; curParticle < numParticles - 1; ++curParticle){
			unsigned int curSubdiv = 0;
			float t = subdivInc;
			// Subdivide the beam between particles.
			for(; curSubdiv <= partRenderer.subdivisions; ++curSubdiv){
				vec3 nextPos;

				// Compute the next interpolated position.
				cubicSplineEvaluate(&spline, curParticle, t, &nextPos);
				t += subdivInc;

				// Construct the polyboard vertices
				// for the current particle.
				polyboardAddVertices(
					batchedRenderer,
					&curPos, &prevPos, &nextPos, camPos,
					partRenderer.halfWidth, tileWidth,
					&G, &H
				);
				tileWidth += tileWidthInc;
				// Add them to the batch!
				spriteRendererBatchedAddIndex(batchedRenderer, batchedRenderer->numVertices);
				spriteRendererBatchedAddVertex(batchedRenderer, G);
				spriteRendererBatchedAddIndex(batchedRenderer, batchedRenderer->numVertices);
				spriteRendererBatchedAddVertex(batchedRenderer, H);

				// If the batch is full, draw it and start a new one!
				// We always add two vertices and indices at a time.
				if(spriteRendererBatchedHasRoom(batchedRenderer, 2, 2)){
					spriteRendererBatchedDraw(batchedRenderer);
					spriteRendererBatchedOrphan(batchedRenderer);

					// We'll need to add the last two vertices
					// back to continue the triangle strip.
					spriteRendererBatchedAddIndex(batchedRenderer, 0);
					spriteRendererBatchedAddVertex(batchedRenderer, G);
					spriteRendererBatchedAddIndex(batchedRenderer, 1);
					spriteRendererBatchedAddVertex(batchedRenderer, H);
				}

				prevPos = curPos;
				curPos = nextPos;
			}
		}
		
		// Construct the polyboard
		// vertices for the last particle.
		polyboardAddVertices(
			batchedRenderer,
			&curPos, &prevPos, &curPos, camPos,
			partRenderer.halfWidth, tileWidth,
			&G, &H
		);
		// Add them to the batch!
		spriteRendererBatchedAddIndex(batchedRenderer, batchedRenderer->numVertices);
		spriteRendererBatchedAddVertex(batchedRenderer, G);
		spriteRendererBatchedAddIndex(batchedRenderer, batchedRenderer->numVertices);
		spriteRendererBatchedAddVertex(batchedRenderer, H);

		// If we've filled the index buffer,
		// we should draw the batch now.
		if(spriteRendererBatchedIsFull(batchedRenderer)){
			spriteRendererBatchedDraw(batchedRenderer);
			spriteRendererBatchedOrphan(batchedRenderer);

		// Otherwise, we need to add an index for primitive
		// restart, since we're using triangle strips.
		}else{
			spriteRendererBatchedAddIndex(batchedRenderer, SPRITE_PRIMITIVE_RESTART_INDEX);
		}
	}
}


static void polyboardSetupSpline(
	cubicSpline *const restrict spline,
	const keyValue *const restrict keyValues, const size_t numParticles,
	const float dt
){

	const keyValue *curKeyValue = keyValues;
	const keyValue *const lastKeyValue = &keyValues[numParticles];
	// Array of interpolated particle positions.
	vec3 *interpPos = memoryManagerGlobalAlloc(
		sizeof(*interpPos) * numParticles
	);
	vec3 *curInterpPos = interpPos;
	if(interpPos == NULL){
		/** MALLOC FAILED **/
	}

	// Compute the interpolated position of each particle.
	for(; curKeyValue != lastKeyValue; ++curKeyValue){
		const particle *const curParticle = curKeyValue->key;
		// We only need the position, so there's no
		// need to interpolate the full transform.
		vec3Lerp(
			&curParticle->prevState.pos,
			&curParticle->state.pos,
			dt, curInterpPos
		);
		++curInterpPos;
	}

	// Set up the spline using the interpolated positions!
	cubicSplineInit(&spline, interpPos, numParticles);
	// We no longer need the array of particle positions.
	memoryManagerGlobalFree(interpPos);
}

static void polyboardAddVertices(
	spriteRendererBatched *const restrict batchedRenderer,
	const vec3 *const restrict curPos,
	const vec3 *const restrict prevPos,
	const vec3 *const restrict nextPos,
	const vec3 *const restrict camPos,
	const float r, const float tileWidth,
	spriteVertex *const restrict G, spriteVertex *const restrict H
){

	vec3 D;

	// Z_i = (C - P_i)/||C - P_i||
	vec3SubtractVec3Out(camPos, curPos, &Z);
	vec3NormalizeVec3(&Z);
	// T_i = (P_{i+1} - P_{i-1})/||P_{i+1} - P_{i-1}||
	vec3SubtractVec3Out(nextPos, prevPos, &T);
	vec3NormalizeVec3(&T);
	// D_i = r*(T_i X Z_i)
	vec3CrossVec3Out(&T, &Z, &D);
	vec3MultiplyS(&D, r);

	// G_i = P_i + D_i
	vec3AddVec3Out(curPos, &D, &G->pos);
	vec2InitSet(&G->uv, tileWidth, 0.f);
	G->normal = Z;

	// H_i = P_i - D_i
	vec3SubtractVec3Out(curPos, &D, &H->pos);
	vec2InitSet(&H->uv, tileWidth, 1.f);
	H->normal = Z;
}