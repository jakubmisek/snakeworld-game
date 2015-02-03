#ifndef _THREADEDLOADER_H_
#define _THREADEDLOADER_H_

//////////////////////////////////////////////////////////////////////////
#include <Windows.h>	// threads
#include "locker.h"

//////////////////////////////////////////////////////////////////////////
// new thread starting function
DWORD	WINAPI	ThreadedLoaderStartRoutine( LPVOID param );

//////////////////////////////////////////////////////////////////////////
// threaded loader class
class IThreadedLoader: public ILocker
{
public:

	enum	EState
	{
		NotLoaded = 0,
		Loading,
		Loaded,
	};

	IThreadedLoader();
	virtual ~IThreadedLoader();

	// current loader state
	inline EState GetState(){ return m_state; }

	/* starting functions here, will init their own parameters and call StartThread */

protected:

	bool	StartThread( bool threaded = true );	// create new thread and call main loading process
	void	OnDestruct(bool forced = false);	// derived class must call this in dtor

	virtual	DWORD DoLoading() = 0;	// the main loading process

private:

	friend DWORD WINAPI ThreadedLoaderStartRoutine( LPVOID param );

	EState	m_state;
};


#endif//_THREADEDLOADER_H_