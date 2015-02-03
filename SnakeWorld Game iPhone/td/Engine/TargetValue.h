#pragma once

#include "global.h"
#include "Serialize.h"

class MemFile;

enum TargetType { Static, Infinity, Linear, EaseOut, EaseIn, EaseOutIn, EaseInMiddle, Constant, JiggleIn, JiggleOut  };
enum TargetRepeat { None, Loop, Bounce, BounceFlip };
class TargetValue : public Timer, public Serialize
{
public:
	TargetValue();
	bool SyncState(MemFile* mf);
	void Animate(float start, float end, uint ticks, TargetType type = Linear, TargetRepeat repeat = None);
	void AnimateConstant(float start, float end, float increment, TargetRepeat repeat = None);
	void AnimateTo(float end, uint ticks, TargetType type = Linear, TargetRepeat repeat = None)
	{
		Animate(getValue(), end, ticks, type, repeat); 
	}
	void Move(float delta_per_second);
	void Move(float start, float delta_per_second);
	bool Ended();
	float getValue();
	void  setValue(float value);
	float getStart() { return start_value; }
	void  setStart(float value) { start_value = value; cur_value = value; }
	float getEnd() { return end_value; }
	void  setEnd(float value) { end_value = value; }
	TargetType getEaseType() { return ease_type; }
	void  setEaseType(TargetType value) { ease_type = value; }
	TargetRepeat getRepeatType() { return repeat_type; }
	void  setRepeatType(TargetRepeat value) { repeat_type = value; }
	uint RemainingTicks()
	{
		if (end_ticks < last_timer()) return 0;
		return end_ticks - last_timer(); 
	}
	//uint StartTicks() { return end_ticks - start_ticks; }
	//uint EndTicks() { return end_ticks - start_ticks; }
	virtual void SetTimer(td_timer* use_timer);
protected:
	uint start_ticks;
	uint end_ticks;
	float start_value;
	float end_value;
	float cur_value;
	float cur_increment;
	TargetType ease_type;
	TargetRepeat repeat_type;
};

class TargetValueAngle : public TargetValue
{
public:
	//property <TargetValueAngle, float, PROPERTY_READ_WRITE> Value;
	TargetValueAngle();
	void Animate(float start, float end, uint ticks, TargetType type = Linear, TargetRepeat = None);
	float getValue();
	void setValue(float value) {TargetValue::setValue(value);}
protected:
};