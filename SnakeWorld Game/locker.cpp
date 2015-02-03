#include "locker.h"


ILocker::ILocker()
{
	// init critical section
	ZeroMemory(&cs, sizeof(cs));
	InitializeCriticalSection( &cs );
}


ILocker::~ILocker()
{
	// delete critical section
	DeleteCriticalSection( &cs );
}