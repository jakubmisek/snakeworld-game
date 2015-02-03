#include "threadedloader.h"


//////////////////////////////////////////////////////////////////////////
// threaded loader thread function
DWORD WINAPI ThreadedLoaderStartRoutine( LPVOID param )
{
	IThreadedLoader*pLoader = (IThreadedLoader*)param;

	DWORD dwRet = -1;
	bool bLocked = false;

	try
	{
		if (pLoader && pLoader->m_state == IThreadedLoader::Loading)
		{
			pLoader->Lock();
			bLocked = true;

			dwRet = pLoader->DoLoading();	// class derived from IThreadedLoader should be destructed here (error will be thrown)
			pLoader->m_state = IThreadedLoader::Loaded;

			pLoader->Unlock();
			bLocked = false;
		}
	}
	catch (...)
	{
		DebugBreak();

		pLoader->m_state = IThreadedLoader::NotLoaded;

		if (bLocked)
		{
			pLoader->Unlock();// should kill app if pLoader does not exist
		}
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
// threaded loader ctor
IThreadedLoader::IThreadedLoader()
{
	m_state = NotLoaded;
}


//////////////////////////////////////////////////////////////////////////
// threaded loader dtor
IThreadedLoader::~IThreadedLoader()
{
	OnDestruct();
}


//////////////////////////////////////////////////////////////////////////
// start new thread and call StartLoading
bool	IThreadedLoader::StartThread(bool threaded)
{
	HANDLE threadid = NULL;

	locked(

		if (m_state != Loading)
		{
			if (threaded)
			{
				threadid = CreateThread( NULL, 0, ThreadedLoaderStartRoutine, this, 0, NULL );

				if (threadid)
				{
					m_state = Loading;
				}
			}
			else
			{
				DoLoading();

				m_state = Loaded;
			}
			
		}

	);

	return ( threadid != NULL );
}

void	IThreadedLoader::OnDestruct(bool forced)
{
	if (forced)m_state = NotLoaded;

	while(m_state == Loading)
	{
		locked({});
	}
}