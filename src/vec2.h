#ifndef vec2_h
#define vec2_h


typedef struct vec2 {
	float x;
	float y;
} vec2;


void vec2InitZero(vec2 *const restrict v);
vec2 vec2InitZeroC();
void vec2InitSet(vec2 *const restrict v, const float x, const float y);
vec2 vec2InitSetC(const float x, const float y);

void vec2Add(vec2 *const restrict v, const float x, const float y);
void vec2AddOut(const vec2 *const restrict v, const float x, const float y, vec2 *const restrict out);
vec2 vec2AddC(vec2 v, const float x, const float y);
void vec2AddS(vec2 *const restrict v, const float x);
void vec2AddSOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2AddSC(vec2 v, const float x);
void vec2AddVec2(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2AddVec2Out(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2AddVec2C(vec2 v1, const vec2 v2);
void vec2Subtract(vec2 *const restrict v, const float x, const float y);
void vec2SubtractOut(const vec2 *const restrict v, const float x, const float y, vec2 *const restrict out);
vec2 vec2SubtractC(vec2 v, const float x, const float y);
void vec2SubtractFrom(vec2 *const restrict v, const float x, const float y);
void vec2SubtractFromOut(const vec2 *const restrict v, const float x, const float y, vec2 *const restrict out);
vec2 vec2SubtractFromC(vec2 v, const float x, const float y);
void vec2SubtractSFrom(vec2 *const restrict v, const float x);
void vec2SubtractSFromOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2SubtractSFromC(vec2 v, const float x);
void vec2SubtractFromS(vec2 *const restrict v, const float x);
void vec2SubtractFromSOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2SubtractFromSC(vec2 v, const float x);
void vec2SubtractVec2From(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2SubtractFromVec2(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2SubtractVec2FromOut(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2SubtractVec2FromC(vec2 v1, const vec2 v2);

void vec2MultiplyS(vec2 *const restrict v, const float x);
void vec2MultiplySOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2MultiplySC(vec2 v, const float x);
void vec2MultiplyVec2(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2MultiplyVec2Out(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2MultiplyVec2C(vec2 v1, const vec2 v2);
void vec2DivideByS(vec2 *const restrict v, const float x);
void vec2DivideBySOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2DivideBySC(vec2 v, const float x);
void vec2DivideSBy(vec2 *const restrict v, const float x);
void vec2DivideSByOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2DivideSByC(vec2 v, const float x);
void vec2DivideSByFast(vec2 *const restrict v, const float x);
void vec2DivideSByFastOut(const vec2 *const restrict v, const float x, vec2 *const restrict out);
vec2 vec2DivideSByFastC(vec2 v, const float x);
void vec2DivideByVec2(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2DivideVec2By(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2DivideByVec2Out(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2DivideByVec2C(vec2 v1, const vec2 v2);
void vec2DivideByVec2Fast(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2DivideVec2ByFast(vec2 *const restrict v1, const vec2 *const restrict v2);
void vec2DivideByVec2FastOut(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2DivideByVec2FastC(vec2 v1, const vec2 v2);

void vec2Fmaf(const float x, const vec2 *const restrict u, vec2 *const restrict v);
void vec2FmafOut(const float x, const vec2 *const restrict u, const vec2 *const restrict v, vec2 *const restrict out);
vec2 vec2FmafC(const float x, const vec2 u, const vec2 v);

float vec2Magnitude(const float x, const float y);
float vec2MagnitudeVec2(const vec2 *const restrict v);
float vec2MagnitudeVec2C(const vec2 v);
float vec2MagnitudeSquared(const float x, const float y);
float vec2MagnitudeSquaredVec2(const vec2 *const restrict v);
float vec2MagnitudeSquaredVec2C(const vec2 v);
float vec2Distance(const vec2 *const restrict v, const float x, const float y);
float vec2DistanceC(const vec2 v, const float x, const float y);
float vec2DistanceSquared(const vec2 *const restrict v, const float x, const float y);
float vec2DistanceSquaredC(const vec2 v, const float x, const float y);
float vec2DistanceVec2(const vec2 *const restrict v1, const vec2 *const restrict v2);
float vec2DistanceVec2C(const vec2 v1, const vec2 v2);
float vec2DistanceSquaredVec2(const vec2 *const restrict v1, const vec2 *const restrict v2);
float vec2DistanceSquaredVec2C(const vec2 v1, const vec2 v2);

float vec2Dot(const float x1, const float y1, const float x2, const float y2);
float vec2DotVec2Float(const vec2 *const restrict v, const float x, const float y);
float vec2DotVec2FloatC(const vec2 v, const float x, const float y);
float vec2DotVec2(const vec2 *const restrict v1, const vec2 *const restrict v2);
float vec2DotVec2C(const vec2 v1, const vec2 v2);

void vec2Normalize(const float x, const float y, vec2 *const restrict out);
void vec2NormalizeFast(const float x, const float y, vec2 *const restrict out);
vec2 vec2NormalizeC(const float x, const float y);
vec2 vec2NormalizeFastC(const float x, const float y);
void vec2NormalizeVec2(vec2 *const restrict v);
void vec2NormalizeVec2Fast(vec2 *const restrict v);
void vec2NormalizeVec2Out(const vec2 *const restrict v, vec2 *const restrict out);
void vec2NormalizeVec2FastOut(const vec2 *const restrict v, vec2 *const restrict out);
vec2 vec2NormalizeVec2C(vec2 v);
vec2 vec2NormalizeVec2FastC(vec2 v);
void vec2Negate(vec2 *const restrict v);
void vec2NegateOut(const vec2 *const restrict v, vec2 *const restrict out);
vec2 vec2NegateC(vec2 v);

void vec2Orthogonal(const vec2 *const restrict v, vec2 *const restrict out);
vec2 vec2OrthogonalC(const vec2 v);

void vec2DegToRad(vec2 *const restrict v);
void vec2RadToDeg(vec2 *const restrict v);

void vec2Lerp(const vec2 *const restrict v1, const vec2 *const restrict v2, const float time, vec2 *const restrict out);
vec2 vec2LerpC(vec2 v1, const vec2 v2, const float time);
void vec2LerpDiff(const vec2 *const restrict v, const vec2 *offset, const float time, vec2 *const restrict out);
vec2 vec2LerpDiffC(vec2 v, const vec2 offset, const float time);

void vec2Min(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2MinC(vec2 v1, const vec2 v2);
void vec2Max(const vec2 *const restrict v1, const vec2 *const restrict v2, vec2 *const restrict out);
vec2 vec2MaxC(vec2 v1, const vec2 v2);


#endif