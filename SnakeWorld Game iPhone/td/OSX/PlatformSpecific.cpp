/*
 *  PlatformSpecific.cpp
 *  Snakeworld
 *
 */

#include "PlatformSpecific.h"

void TDBuzz()
{
#if TARGET_OS_IPHONE
	SoundEngine_Vibrate();
#endif
}