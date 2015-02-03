#include "stdafx.h"
#include "global.h"

#include <stdlib.h>
#include <math.h>

#include "Serialize.h"
#include "TargetValue.h"

//TargetValue* traced;

TargetValue::TargetValue()
{
	start_value = 0;
	end_value = 0;
	cur_value = 0;
	ease_type = Static;
	end_ticks = last_timer();
	start_ticks = last_timer();
}

void TargetValue::setValue(float value)
{
	start_value = value;
	cur_value = value;
	ease_type = Static;
}
float TargetValue::getValue()
{
	float val = 0;
	if (ease_type == Static) return start_value;
	if (ease_type == Infinity)
	{
		return start_value + (((float)(last_timer() - start_ticks) / 1000.0f) * end_value);
	}
	else if (ease_type == Constant)
	{
		if (Ended()) return end_value;
		float ratio = ((float)elapsed_ticks() / 1000.0f) * cur_increment;
		if (start_value > end_value && ratio > 0)
			cur_value -= ratio;
		else
			cur_value += ratio;
		return cur_value;
	}
	else
	{
		if (last_timer() >= end_ticks)
		{
			val = end_value;
			switch (repeat_type)
			{
			case Bounce:
			case BounceFlip:
				{
					uint tick_size = (end_ticks - start_ticks);
					start_ticks = end_ticks;
					end_ticks = end_ticks + tick_size;
					float dummy = start_value;
					start_value = end_value;
					cur_value = end_value;
					end_value = dummy;
					if (repeat_type == Bounce)
					{
						switch (ease_type) 
						{
						case EaseIn: ease_type = EaseOut; break;
						case EaseOut: ease_type = EaseIn; break;
						}
					}
				}
				break;
			case Loop:
				end_ticks = last_timer() + (end_ticks - start_ticks);
				start_ticks = last_timer();
				break;
			}

		}
		else
		{
			switch (ease_type)
			{
			case JiggleIn:
				{
					//(.5 * ((sin(x^3)+ sin(x) )* sin(x) )) + sin(.5*x)
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float x = (ratio ) * 3.14159f;
					//float sined = (.3f * ((sin(x*x*x) + sin(x)) * sin(x))) + sin(.5f * x);
					float sined = 1.0f - ( (cos( (.7f * x) * (.7f * x) * (.7f * x) )) * cos(x * .5f));
					//Trace("%f\n", sined);
					//Trace("%d %d %d %f\n", start_ticks, last_timer, end_ticks, sined);
					float pos = start_value + ((end_value - start_value) * sined);
					val = pos;
				}
				break;
			case JiggleOut:
				{
					//(.5 * ((sin(x^3)+ sin(x) )* sin(x) )) + sin(.5*x)
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float x = 3.14159f - ((ratio ) * 3.14159f);
					//float sined = (.3f * ((sin(x*x*x) + sin(x)) * sin(x))) + sin(.5f * x);
					float sined = ( (cos( (.7f * x) * (.7f * x) * (.7f * x) )) * cos(x * .5f));
					//Trace("%f\n", sined);
					//Trace("%d %d %d %f\n", start_ticks, last_timer, end_ticks, sined);
					float pos = start_value + ((end_value - start_value) * sined);
					val = pos;
				}
				break;
			}
			switch (ease_type)
			{
			case Linear:
				{
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float pos = start_value + ((end_value - start_value) * ratio);
					val = pos;
				}
				break;
			case EaseOut:
				{
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float sined = ratio * ratio * ratio;
					float pos = start_value + ((end_value - start_value) * sined);
					val = pos;
				}
				break;
			}
			switch (ease_type)
			{
			case EaseIn:
				{
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float sined = ((1 - ratio) * (1 - ratio) * (1 - ratio));
					float pos = end_value - ((end_value - start_value) * sined);
					val = pos;
//					if (this == traced)					Trace("%f %f %f %f st=%d et=%d", start_value, end_value, ratio, sined, start_ticks, end_ticks);
				}
				break;
			case EaseOutIn:
				{
					//.5 + .5 * sin (x-.5)*3.1415
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float xs = (ratio - 0.5f) * 3.14159f;
					float sined = sin(xs) * 0.5f + 0.5f;
					float pos = start_value + ((end_value - start_value) * sined);
					//if (this == traced)
						//Trace("%f %f %f %f ", start_value, end_value, xs, sined);
					val = pos;
				}
				break;
			case EaseInMiddle:
				{
					//((x-.5)*2)^3/2+.5
					float ratio = (float)(last_timer() - start_ticks) / (float)(end_ticks - start_ticks);
					float xs = (ratio - 0.5f) * 2.0f;
					float sined = (xs*xs*xs) / 2.0f + 0.5f;
					float pos = start_value + ((end_value - start_value) * sined);
					val = pos;
				}
			}
		}
	}
	//if (this == traced)		Trace("val=%f\n ", val);
	return val;
}

void TargetValue::AnimateConstant( float start, float end, float increment, TargetRepeat repeat /*= None*/ )
{
	start_value = start; 
	cur_value = start;
	cur_increment = increment;
	end_value = end; 
	start_ticks = last_timer(); 
	end_ticks = start_ticks; 
	ease_type = Constant;
	repeat_type = repeat;
}
void TargetValue::Animate( float start, float end, uint ticks, TargetType type, TargetRepeat repeat )
{
	start_value = start; 
	cur_value = start;
	end_value = end; 
	end_ticks = last_timer() + ticks; 
	start_ticks = last_timer(); 
	ease_type = type;
	repeat_type = repeat;
}

void TargetValue::Move( float delta_per_second )
{
	end_value = delta_per_second;
	start_ticks = last_timer();
	ease_type = Infinity;
	repeat_type = None;
}
void TargetValue::Move( float start, float delta_per_second )
{
	start_value = start;
	cur_value = start;
	end_value = delta_per_second;
	start_ticks = last_timer();
	ease_type = Infinity;
	repeat_type = None;
}

bool TargetValue::Ended()
{
	if (ease_type == Constant)
	{
		if (start_value <= end_value && cur_value >= end_value) return true;
		if (start_value >= end_value && cur_value <= end_value) return true;
		return false;
	}
	return last_timer() > end_ticks;
}

void TargetValue::SetTimer( td_timer* use_timer )
{
//	uint old_ticks = m_timer->elapsed_ticks;
//	uint new_ticks = use_timer->elapsed_ticks;
	uint old_abs = m_timer->last_timer;
	uint new_abs = use_timer->last_timer;
	if (old_abs != new_abs)
	{
		int dif = (int)old_abs - (int)new_abs;
		start_ticks -= dif;
		end_ticks -= dif;
	}
	Timer::SetTimer(use_timer);
}

bool TargetValue::SyncState( MemFile* mf )
{
	Serialize::SyncState(mf);
	SyncUInt(start_ticks);
	SyncUInt(end_ticks);
	SyncFloat(start_value);
	SyncFloat(end_value);
	SyncFloat(cur_value);
	SyncFloat(cur_increment);
	int tt = ease_type;
	SyncInt(tt);
	ease_type = (TargetType)tt;
	int tr = repeat_type;
	SyncInt(tr);
	repeat_type = (TargetRepeat)tr;

	return true;
}

void TargetValueAngle::Animate( float start, float end, uint ticks, TargetType type, TargetRepeat repeat )
{
	end_ticks = last_timer() + ticks; 
	start_ticks = last_timer(); 
	ease_type = type;
	repeat_type = repeat;

	float clockwise = abs(end - start);
	float counterclock1 = abs((end - 360) - start);
	float counterclock2 = abs((end + 360) - start);
	start_value = start;
	cur_value = start;
	if (clockwise < counterclock1 && clockwise < counterclock2)
	{
		end_value = end;
	}
	else
	{
		if (counterclock1 < counterclock2)
		{
			end_value = end - 360;
		}
		else
		{
			end_value = end + 360;
		}
	}
}

TargetValueAngle::TargetValueAngle()
{
	//TargetValue::TargetValue();
	//Value.setContainer(this);
	//Value.getter(&TargetValueAngle::getValue);
	//Value.setter(&TargetValueAngle::setValue);
}

float TargetValueAngle::getValue()
{
	float ret = TargetValue::getValue();
	if (ease_type == Infinity)
	{
		if (ret < 0) start_value += 360;
		if (ret > 360) start_value -= 360;
	}
	return ret;
}