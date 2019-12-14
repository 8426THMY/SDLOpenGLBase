#ifndef vec3_h
#define vec3_h


typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;


void vec3InitZero(vec3 *v);
vec3 vec3InitZeroR();
void vec3InitSet(vec3 *v, const float x, const float y, const float z);
vec3 vec3InitSetR(const float x, const float y, const float z);

void vec3Add(vec3 *v, const float x, const float y, const float z);
void vec3AddOut(const vec3 *v, const float x, const float y, const float z, vec3 *out);
vec3 vec3AddR(vec3 v, const float x, const float y, const float z);
void vec3AddS(vec3 *v, const float x);
void vec3AddSOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3AddSR(vec3 v, const float x);
void vec3AddVec3(vec3 *v1, const vec3 *v2);
void vec3AddVec3Out(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3AddVec3R(vec3 v1, const vec3 v2);
void vec3Subtract(vec3 *v, const float x, const float y, const float z);
void vec3SubtractOut(const vec3 *v, const float x, const float y, const float z, vec3 *out);
vec3 vec3SubtractR(vec3 v, const float x, const float y, const float z);
void vec3SubtractFrom(vec3 *v, const float x, const float y, const float z);
void vec3SubtractFromOut(const vec3 *v, const float x, const float y, const float z, vec3 *out);
vec3 vec3SubtractFromR(vec3 v, const float x, const float y, const float z);
void vec3SubtractSFrom(vec3 *v, const float x);
void vec3SubtractSFromOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3SubtractSFromR(vec3 v, const float x);
void vec3SubtractFromS(vec3 *v, const float x);
void vec3SubtractFromSOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3SubtractFromSR(vec3 v, const float x);
void vec3SubtractVec3From(vec3 *v1, const vec3 *v2);
void vec3SubtractFromVec3(vec3 *v1, const vec3 *v2);
void vec3SubtractVec3FromOut(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3SubtractVec3FromR(vec3 v1, const vec3 v2);

void vec3MultiplyS(vec3 *v, const float x);
void vec3MultiplySOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3MultiplySR(vec3 v, const float x);
void vec3MultiplyVec3(vec3 *v1, const vec3 *v2);
void vec3MultiplyVec3Out(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3MultiplyVec3R(vec3 v1, const vec3 v2);
void vec3DivideByS(vec3 *v, const float x);
void vec3DivideBySOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3DivideBySR(vec3 v, const float x);
void vec3DivideSBy(vec3 *v, const float x);
void vec3DivideSByOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3DivideSByR(vec3 v, const float x);
void vec3DivideSByFast(vec3 *v, const float x);
void vec3DivideSByFastOut(const vec3 *v, const float x, vec3 *out);
vec3 vec3DivideSByFastR(vec3 v, const float x);
void vec3DivideByVec3(vec3 *v1, const vec3 *v2);
void vec3DivideVec3By(vec3 *v1, const vec3 *v2);
void vec3DivideByVec3Out(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3DivideByVec3R(vec3 v1, const vec3 v2);
void vec3DivideByVec3Fast(vec3 *v1, const vec3 *v2);
void vec3DivideVec3ByFast(vec3 *v1, const vec3 *v2);
void vec3DivideByVec3FastOut(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3DivideByVec3FastR(vec3 v1, const vec3 v2);

float vec3Magnitude(const float x, const float y, const float z);
float vec3MagnitudeVec3(const vec3 *v);
float vec3MagnitudeVec3R(const vec3 v);
float vec3DistanceSquared(const vec3 *v, const float x, const float y, const float z);
float vec3DistanceSquaredR(const vec3 v, const float x, const float y, const float z);
float vec3DistanceSquaredVec3(const vec3 *v1, const vec3 *v2);
float vec3DistanceSquaredVec3R(const vec3 v1, const vec3 v2);

float vec3Dot(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2);
float vec3DotVec3Float(const vec3 *v, const float x, const float y, const float z);
float vec3DotVec3FloatR(const vec3 v, const float x, const float y, const float z);
float vec3DotVec3(const vec3 *v1, const vec3 *v2);
float vec3DotVec3R(const vec3 v1, const vec3 v2);
float vec3Norm(const float x, const float y, const float z);
float vec3NormVec3(const vec3 *v);
float vec3NormVec3R(const vec3 v);

void vec3Cross(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2, vec3 *out);
vec3 vec3CrossR(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2);
void vec3CrossVec3Float(const vec3 *v, const float x, const float y, const float z, vec3 *out);
vec3 vec3CrossVec3FloatR(const vec3 v, const float x, const float y, const float z);
void vec3CrossFloatVec3(const float x, const float y, const float z, const vec3 *v, vec3 *out);
vec3 vec3CrossFloatVec3R(const float x, const float y, const float z, const vec3 v);
void vec3CrossByVec3(vec3 *v1, const vec3 *v2);
void vec3CrossVec3By(const vec3 *v1, vec3 *v2);
void vec3CrossVec3Out(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3CrossVec3R(const vec3 v1, const vec3 v2);

void vec3Normalize(const float x, const float y, const float z, vec3 *out);
void vec3NormalizeFast(const float x, const float y, const float z, vec3 *out);
vec3 vec3NormalizeR(const float x, const float y, const float z);
vec3 vec3NormalizeFastR(const float x, const float y, const float z);
void vec3NormalizeVec3(vec3 *v);
void vec3NormalizeVec3Fast(vec3 *v);
void vec3NormalizeVec3Out(const vec3 *v, vec3 *out);
void vec3NormalizeVec3FastOut(const vec3 *v, vec3 *out);
vec3 vec3NormalizeVec3R(vec3 v);
vec3 vec3NormalizeVec3FastR(vec3 v);
void vec3Negate(vec3 *v);
void vec3NegateOut(const vec3 *v, vec3 *out);
vec3 vec3NegateR(vec3 v);

void vec3DegToRad(vec3 *v);
void vec3RadToDeg(vec3 *v);

void vec3Lerp(const vec3 *v1, const vec3 *v2, const float time, vec3 *out);
vec3 vec3LerpR(vec3 v1, const vec3 v2, const float time);
void vec3LerpFast(const vec3 *v, const vec3 *offset, const float time, vec3 *out);
vec3 vec3LerpFastR(vec3 v, const vec3 offset, const float time);

void vec3Min(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3MinR(vec3 v1, const vec3 v2);
void vec3Max(const vec3 *v1, const vec3 *v2, vec3 *out);
vec3 vec3MaxR(vec3 v1, const vec3 v2);


#endif