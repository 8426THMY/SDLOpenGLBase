#include "sprite.h"


#include "vertex.h"


mesh spriteMeshDefault;


return_t spriteSetupDefault(){
	const vertex vertices[4] = {
		{
			.pos.x = -0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = -0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 0.f, .uv.y = 0.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = -0.5f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 0.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		},
		{
			.pos.x = 0.5f, .pos.y = 0.5f, .pos.z = 0.f,
			.uv.x = 1.f, .uv.y = 1.f,
			.normal.x = 0.f, .normal.y = 0.f, .normal.z = 1.f,
			.boneIDs = {0, -1, -1, -1}, .boneWeights = {1.f, 0.f, 0.f, 0.f}
		}
	};

	const size_t indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	meshGenerateBuffers(&spriteMeshDefault, vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices)/sizeof(*indices));


	return(1);
}