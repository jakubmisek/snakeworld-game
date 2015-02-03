#include "stdafx.h"
#include "global.h"
#include "Touches.h"
#include <math.h>

//touch_struct touch_began[3];
//touch_struct touch_last[3];
//touch_struct touch[3];
//touch_struct touch_ended[3];
//TouchType touch_type[3];

#define DRAG_THRESHOLD 10.0f

Touches theTouches;

Touch::Touch()
{
	start.x = 0;
	start.y = 0;
	start.ticks = 0;
	current.x = 0;
	current.y = 0;
	current.ticks = 0;
	end.x = 0;
	end.y = 0;
	end.ticks = 0;
	bActive = false;
	bDragFlag = false;
	tap_count = 0;
}


int Touches::TouchStart( int x, int y )
{
//	TheProfile.Increment(STAT_SCREEN_TOUCHED);
	int t = AvailableSlot();
	if (last_main_timer.last_timer < m_touches[t].end.ticks + 500)
		m_touches[t].tap_count++;
	else
		m_touches[t].tap_count = 0;

	m_touches[t].start.x = x;
	m_touches[t].start.y = y;
	m_touches[t].start.ticks = last_main_timer.last_timer;
	m_touches[t].current.x = x;
	m_touches[t].current.y = y;
	m_touches[t].current.ticks = last_main_timer.last_timer;
	m_touches[t].bActive = true;
	m_touches[t].bDragFlag = false;
	return t;
}

int Touches::TouchMove( int x, int y )
{
	int t = Closest(x, y);
	if (t != -1)
	{
		m_touches[t].current.x = x;
		m_touches[t].current.y = y;
		m_touches[t].current.ticks = last_main_timer.last_timer;
		if (Distance((float)m_touches[t].current.x, (float)m_touches[t].current.y, (float)m_touches[t].start.x, (float)m_touches[t].start.y) > DRAG_THRESHOLD)
		{
			m_touches[t].bDragFlag = true;
		}
	}
	return t;
}

int Touches::TouchEnd( int x, int y )
{
	int t = Closest(x, y);
	if (t != -1)
	{
		m_touches[t].bActive = false;
		if (Distance((float)m_touches[t].current.x, (float)m_touches[t].current.y, (float)m_touches[t].start.x, (float)m_touches[t].start.y) > DRAG_THRESHOLD)
		{
			m_touches[t].bDragFlag = true;
		}
		m_touches[t].current.x = x;
		m_touches[t].current.y = y;
		m_touches[t].current.ticks = last_main_timer.last_timer;
		m_touches[t].end.x = x;
		m_touches[t].end.y = y;
		m_touches[t].end.ticks = last_main_timer.last_timer;
	}
	return t;
}

int Touches::AvailableSlot()
{
	for (int i = 0; i < 10; i++)
	{
		if (!m_touches[i].bActive)
			return i;
	}
	return 9;
}

int Touches::Closest( int x, int y )
{
	int ret = -1;
	float distance;
	for (int i = 0; i < 10; i++)
	{
		if (m_touches[i].bActive)
		{
			if (ret == -1)
			{
				ret = i;
				distance = Distance((float)m_touches[i].current.x, (float)m_touches[i].current.y, (float)x, (float)y);
			}
			else
			{
				float distance2 = Distance((float)m_touches[i].current.x, (float)m_touches[i].current.y, (float)x, (float)y);
				if (distance2 < distance)
				{
					distance = distance2;
					ret = i;
				}
			}
		}
	}
	return ret;
}

float Distance( float x1, float y1, float x2, float y2 )
{
	return (float)(sqrt((float)pow(x1 - x2, 2.0f) + (float)pow(y1 - y2, 2.0f)));
}

Touch* Touches::TouchSpot( int t )
{
	return &(m_touches[t]);
}

bool Touches::IsActive( int t )
{
	return m_touches[t].bActive;
}

bool Touches::IsDragging( int t )
{
	return m_touches[t].bDragFlag;
}

Touch* Touches::TouchSpotRotated( int t )
{
	if (screen_orientation == 0) return TouchSpot( t );
	if (screen_orientation == -1)
	{
		m_rotated_return_val.start.x = 479 - m_touches[t].start.y;
		m_rotated_return_val.start.y = m_touches[t].start.x;
		m_rotated_return_val.start.ticks = m_touches[t].start.ticks;
		m_rotated_return_val.current.x = 479 - m_touches[t].current.y;
		m_rotated_return_val.current.y = m_touches[t].current.x;
		m_rotated_return_val.current.ticks = m_touches[t].current.ticks;
		m_rotated_return_val.end.x = 479 - m_touches[t].end.y;
		m_rotated_return_val.end.y = m_touches[t].end.x;
		m_rotated_return_val.end.ticks = m_touches[t].end.ticks;
	}
	if (screen_orientation == 1)
	{
		m_rotated_return_val.start.x = m_touches[t].start.y;
		m_rotated_return_val.start.y = 319 - m_touches[t].start.x;
		m_rotated_return_val.start.ticks = m_touches[t].start.ticks;
		m_rotated_return_val.current.x = m_touches[t].current.y;
		m_rotated_return_val.current.y = 319 - m_touches[t].current.x;
		m_rotated_return_val.current.ticks = m_touches[t].current.ticks;
		m_rotated_return_val.end.x = m_touches[t].end.y;
		m_rotated_return_val.end.y = 319 - m_touches[t].end.x;
		m_rotated_return_val.end.ticks = m_touches[t].end.ticks;
	}
	return &m_rotated_return_val;
}