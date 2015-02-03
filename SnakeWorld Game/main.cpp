#include <windows.h>
#include "gamewindow.h"
#include "errorlog.h"

//////////////////////////////////////////////////////////////////////////
// message loop
// process window messages
// if there are no messages, render the scene
void	DoMessageLoop(CGameWindow&aGameWindow)
{
	MSG msg = {0};

	while( WM_QUIT != msg.message )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			aGameWindow.DoFrame();
		}
	}

}


//////////////////////////////////////////////////////////////////////////
// check the instance is already running
bool	CheckInstanceRunning()
{
	#define MUTEX_NAME	(L"snakeworld_game_instance_mutex")

	HANDLE instanceMutex = OpenMutex(MUTEX_ALL_ACCESS , TRUE, MUTEX_NAME);

	if (instanceMutex != NULL)
	{
		MessageBox(NULL, L"The game is already running.", L"Snake World",MB_ICONWARNING);
		return true;
	}

	instanceMutex = CreateMutex(NULL,TRUE,MUTEX_NAME);
	
	return false;
}


//////////////////////////////////////////////////////////////////////////
// WinMain
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	if (CheckInstanceRunning())
		return -1;

	// create the main window object
	CGameWindow aGameWindow(hInstance);

	// create the window & direct3D
	if ( !aGameWindow.Create(lpCmdLine,nShowCmd) )
	{
		CloseWindow( aGameWindow.GetHWnd() );

		CString strErr;
		GetLog(strErr);

		MessageBox( NULL, strErr, L"Snake world Game", MB_ICONERROR );
		return -1;
	}

	// process windows messages & render
	DoMessageLoop(aGameWindow);

	// done
	return 0;
}

