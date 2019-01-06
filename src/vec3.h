#ifndef vec3_h
#define vec3_h


typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;


void vec3InitZero(vec3 *v);
void vec3InitSet(vec3 *v, const float x, const float y, const float z);

void vec3Add(const vec3 *v, const float x, const float y, const float z, vec3 *out);
void vec3AddS(const vec3 *v, const float x, vec3 *out);
void vec3AddVec3(const vec3 *v1, const vec3 *v2, vec3 *out);
void vec3SubtractFrom(const vec3 *v, const float x, const float y, const float z, vec3 *out);
void vec3SubtractSFrom(const vec3 *v, const float x, vec3 *out);
void vec3SubtractFromS(const vec3 *v, const float x, vec3 *out);
void vec3SubtractVec3From(const vec3 *v1, const vec3 *v2, vec3 *out);
void vec3MultiplyS(const vec3 *v, const float x, vec3 *out);
void vec3MultiplyVec3(const vec3 *v1, const vec3 *v2, vec3 *out);
void vec3DivideByS(const vec3 *v, const float x, vec3 *out);
void vec3DivideSBy(const vec3 *v, const float x, vec3 *out);
void vec3DivideByVec3(const vec3 *v1, const vec3 *v2, vec3 *out);

float vec3Norm(const float x, const float y, const float z);
float vec3NormVec3(const vec3 *v);
float vec3Magnitude(const float x, const float y, const float z);
float vec3MagnitudeVec3(const vec3 *v);
float vec3DistanceSquared(const vec3 *v, const float x, const float y, const float z);
float vec3DistanceSquaredVec3(const vec3 *v1, const vec3 *v2);
float vec3Dot(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2);
float vec3DotVec3Float(const vec3 *v, const float x, const float y, const float z);
float vec3DotVec3(const vec3 *v1, const vec3 *v2);
void vec3Cross(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, vec3 *out);
void vec3CrossVec3Float(const vec3 *v, const float x, const float y, const float z, vec3 *out);
void vec3CrossFloatVec3(const float x, const float y, const float z, const vec3 *v, vec3 *out);
void vec3CrossVec3(const vec3 *v1, const vec3 *v2, vec3 *out);
void vec3Normalize(const float x, const float y, const float z, vec3 *out);
void vec3NormalizeVec3(const vec3 *v, vec3 *out);
void vec3Negate(const vec3 *v, vec3 *out);

void vec3DegToRad(vec3 *v);
void vec3RadToDeg(vec3 *v);

void vec3Lerp(const vec3 *v1, const vec3 *v2, const float time, vec3 *out);
void vec3LerpFast(const vec3 *v, const vec3 *offset, const float time, vec3 *out);


#endif