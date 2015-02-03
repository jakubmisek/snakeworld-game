#include "stdafx.h"
#include "Swiper.h"

Swiper::Swiper(void)
{
	m_bFlicked = false;
	m_bActive =	false;
	m_bAllowHorz = true;
	m_bAllowVert = true;
	x_pos = 0;
	x_pos_flick = 0;
	x_pos_start = 0;
	y_pos = 0;
	y_pos_flick = 0;
	y_pos_start = 0;
	m_bInside = false;
	scroll_to_x_active = false;
	scroll_to_y_active = false;
}

Swiper::~Swiper(void)
{
}

void Swiper::TouchBegan( int finger )
{
	m_bFlicked = false;
	m_bActive =	false;
	m_bInside = false;
	int startx = theTouches.TouchSpot(finger)->start.x;
	int starty = theTouches.TouchSpot(finger)->start.y;
	if (clamp(startx, m_x, m_x + m_w) == startx)
	{
		if (clamp(starty, m_y, m_y + m_h) == starty)
		{
			x_pos_start = startx;
			y_pos_start = starty;
			m_bInside = true;
			//Trace("startx = %d,%d %d,%d\n", x_pos_start,y_pos_start, startx,starty);
		}
	}
}

void Swiper::TouchDragging( int finger )
{
	if (!m_bInside) return;
	//int startx = theTouches.TouchSpot(finger)->start.x;
	//int starty = theTouches.TouchSpot(finger)->start.y;
	int curx = theTouches.TouchSpot(finger)->current.x;
	int cury = theTouches.TouchSpot(finger)->current.y;
	if (theTouches.TouchSpot(finger)->bDragFlag) m_bActive = true;
	if (!m_bActive) return;
	if (m_bAllowHorz)
	{
		x_pos -= curx - x_pos_start;
		x_pos_start = curx;
	}
	if (m_bAllowVert)
	{
		y_pos -= cury - y_pos_start;
		y_pos_start = cury;
	}
	last_point.x = curx;
	last_point.y = cury;
	//Trace("curx = %d,%d  %d,%d  %d,%d\n", x_pos, y_pos, curx, cury);
}

void Swiper::TouchEnded( int finger )
{
	if (!m_bInside) return;
	if (!m_bActive) return;
	int startx = theTouches.TouchSpot(finger)->start.x;
	int starty = theTouches.TouchSpot(finger)->start.y;
	int endx = theTouches.TouchSpot(finger)->end.x;
	int endy = theTouches.TouchSpot(finger)->end.y;
	if (m_bAllowHorz)
	{
		x_pos_flick = 0;
		x_pos -= endx - x_pos_start;
		if (absf((float)(endx - last_point.x)) > 2)
		{
			if (absf((float)(endy - starty)) <= 52)
			{
				x_pos_flick = (float)(endx - last_point.x);
				if (x_pos_flick < -25) x_pos_flick = -25;
				if (x_pos_flick > 25) x_pos_flick = 25;
				//Trace("FLICKED_HORZ ");
			}
		}
	}
	if (m_bAllowVert)
	{
		y_pos_flick = 0;
		y_pos -= endy - y_pos_start;
		if (absf((float)(endy - last_point.y)) > 2)
		{
			if (absf((float)(endx - startx)) <= 52)
			{
				y_pos_flick = (float)(endy - last_point.y);
				if (y_pos_flick < -25) y_pos_flick = -25;
				if (y_pos_flick > 25) y_pos_flick = 25;
				//Trace("FLICKED_VERT ");
			}
		}
	}
	m_bActive =	false;
}

void Swiper::Render()
{
	if (!m_bInside) return;
	if (m_bActive) return;	// if we are dragging, return and ignore
	if (scroll_to_x_active)
	{
		x_pos = (int)scroll_to_x.getValue();
		if (scroll_to_x.Ended()) scroll_to_x_active = false;
	}
	if (scroll_to_y_active)
	{
		y_pos = (int)scroll_to_y.getValue();
		if (scroll_to_y.Ended()) scroll_to_y_active = false;
	}
	if (m_bAllowHorz)
	{
		int x_pre_clamp = x_pos;
		if (x_pos < m_min_x_val) x_pos += ((m_min_x_val - x_pos)/2);
		if (x_pos > m_max_x_val) x_pos -= ((x_pos - m_max_x_val)/2);
		if (x_pos == x_pre_clamp)
			x_pos -= (int)x_pos_flick;
		else
			x_pos_flick = 0.0f;
		if (floor(x_pos_flick) != 0 && x_pos_flick > 0)
		{
			x_pos_flick /= 1.4f;
		}
		if (ceil(x_pos_flick) != 0 && x_pos_flick < 0)
		{
			x_pos_flick /= 1.4f;
		}
		//if (x_pos_flick != 0)
			//Trace("x_pos = %d, x_pos_flick = %d\n", x_pos, x_pos_flick);
	}
	if (m_bAllowVert)
	{
		int y_pre_clamp = y_pos;
		if (y_pos < m_min_y_val) y_pos += ((m_min_y_val - y_pos)/2);
		if (y_pos > m_max_y_val) y_pos -= ((y_pos - m_max_y_val)/2);
		if (y_pos == y_pre_clamp)
			y_pos -= (int)y_pos_flick;
		else
			y_pos_flick = 0.0f;
		if (floor(y_pos_flick) != 0 && y_pos_flick > 0)
		{
			y_pos_flick /= 1.2f;
		}
		if (ceil(y_pos_flick) != 0 && y_pos_flick < 0)
		{
			y_pos_flick /= 1.2f;
		}
		//if (y_pos_flick != 0)
			//Trace("y_pos = %d, y_pos_flick = %d\n", y_pos, y_pos_flick);
	}

}

void Swiper::SetYVal( int y, bool scroll /*= false*/, uint ticks /*= 1000*/ )
{
	if (scroll)
	{
		scroll_to_y.Animate((float)y_pos, (float)y, ticks, EaseOutIn);
		scroll_to_y_active = true;
	}
	else
	{ y_pos = y; }
}

void Swiper::SetXVal( int x, bool scroll /*= false*/, uint ticks /*= 1000*/ )
{
	if (scroll)
	{
		scroll_to_x.Animate((float)x_pos, (float)x, ticks, EaseOutIn);
		scroll_to_x_active = true;
	}
	else
	{ x_pos = x; }
}