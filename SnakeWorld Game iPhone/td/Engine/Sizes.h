#pragma once
#include "global.h"
#include "TargetValue.h"

class TAPoint
{
public:
	TAPoint();
	//property <Point, float, PROPERTY_READ_WRITE> x;
	//property <Point, float, PROPERTY_READ_WRITE> y;
	//property <Point, float, PROPERTY_READ_WRITE> z;

	TargetValue xa;
	TargetValue ya;
	TargetValue za;
	float getX() { return xa.getValue(); }
	void  setX(float value) { xa.setValue(value); xa.setEaseType(Static); }

	float getY() { return ya.getValue(); }
	void  setY(float value) { ya.setValue(value); ya.setEaseType(Static); }

	float getZ() { return za.getValue(); }
	//float getZ() { return za.getValue(); }
	void  setZ(float value) { za.setValue(value); za.setEaseType(Static); }

	void  setXY(float x, float y) 
	{
		setX(x);setY(y);
	}
	void  setXYZ(float x, float y, float z) 
	{
		setX(x);setY(y);setZ(z);
	}
	void AnimateTo(float x_end, float y_end, uint ticks, TargetType type = Linear, TargetRepeat repeat = None)
	{
		xa.AnimateTo(x_end, ticks, type, repeat);
		ya.AnimateTo(y_end, ticks, type, repeat);
	}
protected:

};

class TASize
{
public:
	TASize();
	//property <Size, float, PROPERTY_READ_WRITE> w;
	//property <Size, float, PROPERTY_READ_WRITE> h;
	TargetValue m_w;
	TargetValue m_h;

	float getW() { return m_w.getValue();}
	void  setW(float w) { m_w.setValue(w);  m_w.setEaseType(Static); }
	float getH() { return m_h.getValue(); }
	void  setH(float h) { m_h.setValue(h);  m_h.setEaseType(Static); }
	void  setWH(float w, float h) {setW(w); setH(h);}

	void AnimateTo(float w_end, float h_end, uint ticks, TargetType type = Linear, TargetRepeat repeat = None)
	{
		m_w.AnimateTo(w_end, ticks, type, repeat);
		m_h.AnimateTo(h_end, ticks, type, repeat);
	}
};

class RectTag
{
public:
	RectTag() { x = 0; y = 0; w = 0; h = 0; }
	RectTag(float x_val, float y_val, float w_val, float h_val) { x = x_val; y = y_val; w = w_val; h = h_val; }
	float centerx() { return x + (w / 2.0f); }
	float centery() { return y + (h / 2.0f); }
	float right() { return x + w - 1; }
	float left() { return x; }
	float top () { return y + h - 1; }
	float bottom() { return y; }
	float x;
	float y;
	float w;
	float h;
	RectTag & operator =(const RectTag &other) { x = other.x; y = other.y; w = other.w; h = other.h; return *this; }
};
typedef RectTag TARect;

class TAColor
{
public:
	TAColor();
	TargetValue r;
	TargetValue g;
	TargetValue b;
	TargetValue a;
	void SetColor(float rval, float gval, float bval, float aval);
};