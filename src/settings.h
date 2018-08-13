#ifndef settings_h
#define settings_h


#define ASPECT_RATIO_X 4
#define ASPECT_RATIO_Y 3

#define BASE_UPDATE_RATE 60.f
#define UPDATE_RATE 60.f
//We'll probably get undefined results if this is less than or equal to 0.
#define NUM_LOOKBACK_STATES (unsigned int)UPDATE_RATE
#define FRAME_RATE 120.f

#define FOV_DEFAULT 60.f


#endif