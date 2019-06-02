#ifndef vec2_h
#define vec2_h


typedef struct vec2 {
	float x;
	float y;
} vec2;


void vec2InitZero(vec2 *v);
vec2 vec2InitZeroR();
void vec2InitSet(vec2 *v, const float x, const float y);
vec2 vec2InitSetR(vec2 v, const float x, const float y);

void vec2Add(vec2 *v, const float x, const float y);
void vec2AddOut(const vec2 *v, const float x, const float y, vec2 *out);
vec2 vec2AddR(vec2 v, const float x, const float y);
void vec2AddS(vec2 *v, const float x);
void vec2AddSOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2AddSR(vec2 v, const float x);
void vec2AddVec2(vec2 *v1, const vec2 *v2);
void vec2AddVec2Out(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2AddVec2R(vec2 v1, const vec2 v2);
void vec2Subtract(vec2 *v, const float x, const float y);
void vec2SubtractOut(const vec2 *v, const float x, const float y, vec2 *out);
vec2 vec2SubtractR(vec2 v, const float x, const float y);
void vec2SubtractFrom(vec2 *v, const float x, const float y);
void vec2SubtractFromOut(const vec2 *v, const float x, const float y, vec2 *out);
vec2 vec2SubtractFromR(vec2 v, const float x, const float y);
void vec2SubtractSFrom(vec2 *v, const float x);
void vec2SubtractSFromOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2SubtractSFromR(vec2 v, const float x);
void vec2SubtractFromS(vec2 *v, const float x);
void vec2SubtractFromSOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2SubtractFromSR(vec2 v, const float x);
void vec2SubtractVec2From(vec2 *v1, const vec2 *v2);
void vec2SubtractFromVec2(vec2 *v1, const vec2 *v2);
void vec2SubtractVec2FromOut(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2SubtractVec2FromR(vec2 v1, const vec2 v2);

void vec2MultiplyS(vec2 *v, const float x);
void vec2MultiplySOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2MultiplySR(vec2 v, const float x);
void vec2MultiplyVec2(vec2 *v1, const vec2 *v2);
void vec2MultiplyVec2Out(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2MultiplyVec2R(vec2 v1, const vec2 v2);
void vec2DivideByS(vec2 *v, const float x);
void vec2DivideBySOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2DivideBySR(vec2 v, const float x);
void vec2DivideSBy(vec2 *v, const float x);
void vec2DivideSByOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2DivideSByR(vec2 v, const float x);
void vec2DivideSByFast(vec2 *v, const float x);
void vec2DivideSByFastOut(const vec2 *v, const float x, vec2 *out);
vec2 vec2DivideSByFastR(vec2 v, const float x);
void vec2DivideByVec2(vec2 *v1, const vec2 *v2);
void vec2DivideVec2By(vec2 *v1, const vec2 *v2);
void vec2DivideByVec2Out(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2DivideByVec2R(vec2 v1, const vec2 v2);
void vec2DivideByVec2Fast(vec2 *v1, const vec2 *v2);
void vec2DivideVec2ByFast(vec2 *v1, const vec2 *v2);
void vec2DivideByVec2FastOut(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2DivideByVec2FastR(vec2 v1, const vec2 v2);

float vec2Magnitude(const float x, const float y);
float vec2MagnitudeVec2(const vec2 *v);
float vec2MagnitudeVec2R(const vec2 v);
float vec2DistanceSquared(const vec2 *v, const float x, const float y);
float vec2DistanceSquaredR(const vec2 v, const float x, const float y);
float vec2DistanceSquaredVec2(const vec2 *v1, const vec2 *v2);
float vec2DistanceSquaredVec2R(const vec2 v1, const vec2 v2);

float vec2Dot(const float x1, const float y1, const float x2, const float y2);
float vec2DotVec2Float(const vec2 *v, const float x, const float y);
float vec2DotVec2FloatR(const vec2 v, const float x, const float y);
float vec2DotVec2(const vec2 *v1, const vec2 *v2);
float vec2DotVec2R(const vec2 v1, const vec2 v2);
float vec2Norm(const float x, const float y);
float vec2NormVec2(const vec2 *v);
float vec2NormVec2R(const vec2 v);

void vec2Normalize(const float x, const float y, vec2 *out);
void vec2NormalizeFast(const float x, const float y, vec2 *out);
vec2 vec2NormalizeR(const float x, const float y);
vec2 vec2NormalizeRFast(const float x, const float y);
void vec2NormalizeVec2(vec2 *v);
void vec2NormalizeVec2Fast(vec2 *v);
void vec2NormalizeVec2Out(const vec2 *v, vec2 *out);
void vec2NormalizeVec2OutFast(const vec2 *v, vec2 *out);
vec2 vec2NormalizeVec2R(vec2 v);
vec2 vec2NormalizeVec2RFast(vec2 v);
void vec2Negate(vec2 *v);
void vec2NegateOut(const vec2 *v, vec2 *out);
vec2 vec2NegateR(vec2 v);

void vec2DegToRad(vec2 *v);
void vec2RadToDeg(vec2 *v);

void vec2Lerp(const vec2 *v1, const vec2 *v2, const float time, vec2 *out);
vec2 vec2LerpR(vec2 v1, const vec2 v2, const float time);
void vec2LerpFast(const vec2 *v, const vec2 *offset, const float time, vec2 *out);
vec2 vec2LerpFastR(vec2 v, const vec2 offset, const float time);

void vec2Min(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2MinR(vec2 v1, const vec2 v2);
void vec2Max(const vec2 *v1, const vec2 *v2, vec2 *out);
vec2 vec2MaxR(vec2 v1, const vec2 v2);


#endif