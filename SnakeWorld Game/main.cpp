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
// WinMain
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
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

