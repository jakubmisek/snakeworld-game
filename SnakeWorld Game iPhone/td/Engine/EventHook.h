#pragma once

typedef enum event_callback_message_enum
{
	EVENT_Render,
	EVENT_TouchBegan,
	EVENT_TouchDragging,
	EVENT_TouchEnded
} event_callback_message;

class EventHook
{
protected:
	EventHook();
	virtual ~EventHook();

public:
	virtual void Render() {};
	virtual void TouchBegan(int finger) {};
	virtual void TouchDragging(int finger) {};
	virtual void TouchEnded(int finger) {};
};
