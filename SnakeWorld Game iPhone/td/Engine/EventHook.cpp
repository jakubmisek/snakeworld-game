#include "stdafx.h"
#include "EventHook.h"
#include "MainObj.h"

extern MainObj* _mainObj;

EventHook::EventHook()
{
	_mainObj->AddHook(this);
}

EventHook::~EventHook()
{
	_mainObj->RemoveHook(this);
}

