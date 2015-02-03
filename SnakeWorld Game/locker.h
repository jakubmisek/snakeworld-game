#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <windows.h>

// locker class
// allow recursive locking !
class ILocker
{
public:
	ILocker();
	virtual ~ILocker();

	inline void Lock(){ EnterCriticalSection(&cs); }
	inline void Unlock(){ LeaveCriticalSection(&cs); }

private:
	// LOCK/UNLOCK
	CRITICAL_SECTION	cs;	// critical section
};


#define  lockit( locker, code )	\
{ \
	locker->Lock(); \
	{code} \
	locker->Unlock(); \
}

#define locked( code )	lockit( this, code )

#endif//_LOCKER_H_