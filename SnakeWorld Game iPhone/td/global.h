#pragma once


typedef struct timer_struct
{
	uint last_timer;
	uint elapsed_ticks;
} td_timer;

extern td_timer last_main_timer;
extern td_timer last_game_timer;

class Timer
{
protected:
	td_timer* m_timer;
public:
	Timer()
	{
		m_timer = &last_main_timer;
	}
	virtual td_timer* GetTimer() { return m_timer; }
	virtual void SetTimer(td_timer* use_timer) { m_timer = use_timer; }
	uint last_timer() { return m_timer->last_timer; }
	uint elapsed_ticks() { return m_timer->elapsed_ticks; }
};

extern int screen_orientation;	// 0 = portrait, -1 = button on right, 1 = button on left

#define pi 3.14159265f

#define BACKZ -2100.0f
#define TILEZ -1400.0f
#define FRONTZ 2100.0f
#define INTERFACEZ 1400.0f
#define GAMEZ 0.0f
#define OVERLAYZ 1000.0f
#define MISSLEZ 500.0f
#define SCREENZ 1500.0f

#define TILESIZE 20.0f

float absf(float val);

#ifndef tdmax
#define tdmax(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef tdmin
#define tdmin(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef clamp
#define clamp(val,min_val,max_val)  tdmax(tdmin(val, max_val), min_val)
#endif

typedef struct TDPointStruct
{
	int x;
	int y;
	uint ticks;
} TDPoint;

#include "Environment.h"