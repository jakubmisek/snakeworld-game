#pragma once

#include "Touches.h"
#include "EventHook.h"

class Swiper : public EventHook
{
public:
	Swiper(void);
	virtual ~Swiper(void);

	virtual void TouchBegan(int finger);
	virtual void TouchDragging(int finger);
	virtual void TouchEnded(int finger);
	virtual void Render();

	bool WasFlicked() { return m_bFlicked; }
	bool IsDragging() { return m_bActive; }

	void SetArea(int x, int y, int w, int h)
	{
		m_x = x;
		m_y = y;
		m_w = w;
		m_h	= h;
	}

	void SetRange(int min_x_val, int max_x_val, int min_y_val, int max_y_val)
	{
		//Trace("\nSet Range: %d, %d  %d, %d\n", min_x_val, max_x_val, min_y_val, max_y_val);
		m_min_x_val = min_x_val; m_max_x_val = max_x_val; 
		m_min_y_val = min_y_val; m_max_y_val = max_y_val; 
	}
	void SetXRange(int min_x_val, int max_x_val) 
	{
		m_min_x_val = min_x_val; m_max_x_val = max_x_val; 
	}
	void SetYRange(int min_y_val, int max_y_val) 
	{
		m_min_y_val = min_y_val; m_max_y_val = max_y_val; 
	}

	void SetDirections(bool horz, bool vert) { m_bAllowHorz = horz; m_bAllowVert = vert; }

	void SetVals(int x, int y) { x_pos = x; y_pos = y; }
	void SetXVal(int x, bool scroll = false, uint ticks = 1000);
	void SetYVal(int y, bool scroll = false, uint ticks = 1000);
	int GetXVal() { return x_pos; }
	int GetYVal() { return y_pos; }

protected:
	bool scroll_to_x_active;
	bool scroll_to_y_active;
	TargetValue scroll_to_x;
	TargetValue scroll_to_y;
	bool m_bFlicked;
	int x_pos;
	int x_pos_start;
	float x_pos_flick;
	int y_pos;
	int y_pos_start;
	float y_pos_flick;

	TDPoint last_point;
	int m_x, m_y, m_w, m_h;
	int m_min_x_val, m_max_x_val;
	int m_min_y_val, m_max_y_val;
	bool m_bActive;
	bool m_bInside;
	bool m_bAllowHorz;
	bool m_bAllowVert;
};
