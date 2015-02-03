#pragma once
#include "Sizes.h"

class Touch
{
public:
	Touch();
	TDPoint start;
	TDPoint current;
	TDPoint end;
	bool bActive;
	bool bDragFlag;
	int tap_count;
};

float Distance(float x1, float y1, float x2, float y2);

class Touches
{
public:
	int TouchStart(int x, int y);
	int TouchMove(int x, int y);
	int TouchEnd(int x, int y);
	bool IsDragging(int t);
	Touch* TouchSpot(int t);
	Touch* TouchSpotRotated(int t);
	bool IsActive(int t);
protected:
	int AvailableSlot();
	int Closest(int x, int y);
	Touch m_touches[10];
	Touch m_rotated_return_val;
};

extern Touches theTouches;