#define WIN32_LEAN_AND_MEAN
#include <stdio.h>

#include "gamewindow.h"
#include "mychat.h"	// in game menu screen with chat

#include "errorlog.h"

#include <MMSystem.h>	// timeGetTime

#include "texts.h"

//////////////////////////////////////////////////////////////////////////
//
static TCHAR* strSettingsFile = L"settings.img";
static TCHAR* strGameWndClass = L"snakeworld_wnd";
static TCHAR* strGameWndTitle = L"Snake World Game";

#define WM_CONNECTIONRECEIVE	(WM_USER+11)

#define SERVERS_XML		L"servers.xml"
#define	SERVERS_XML_HOST	"http://bins.sacredware.com"
#define	SERVERS_XML_SCRIPT	"/snakeworld/servers.xml"

//////////////////////////////////////////////////////////////////////////
// menu commands
enum	EMenuCommands
{
	mcConnect,
	mcCancelConnect,
	
	mcGoTo,

	mcQuitGame,

	mcGrass,
	mcGraphics,
	mcShadows,
	mcAspectRatio,
	mcFullscreen,
	mcAudioEnable,
	mcChatVisibility,

	mcTexture,
	mcName,
	mcDescription,

	mcUseAuthorization,
	mcUserEmail,
	mcUserPassword,

	mcCustomWorldConnect,

	mcChatMessage,

	mcNone,
};

//////////////////////////////////////////////////////////////////////////
// graphics settings
struct	SGraphicsSettings
{
	UINT width,height,samplescount;	
	CString *plabel;
};
const SGraphicsSettings graphicsettings[] =
{
	{640,480,1,&g_gameTexts.texts[tLowest]},{640,480,4,&g_gameTexts.texts[tLowestSmooth]},
	{800,600,1,&g_gameTexts.texts[tLow]},{800,600,4,&g_gameTexts.texts[tLowSmooth]},
	{1024,768,1,&g_gameTexts.texts[tMedium]}, {1024,768,4,&g_gameTexts.texts[tMediumSmooth]},
	{1600,1200,1,&g_gameTexts.texts[tHigh]}, {1600,1200,4, &g_gameTexts.texts[tHighSmooth]} };
const int graphicsettingscount = sizeof(graphicsettings) / sizeof(graphicsettings[0]);


struct	SAspectRatioSettings
{
	CString *plabel;
	float flAspect;
};
const SAspectRatioSettings aspectratiosettings[] =
{
	{&g_gameTexts.texts[t43], 4.0f / 3.0f},
	{&g_gameTexts.texts[t1610], 16.0f / 10.0f},
	{&g_gameTexts.texts[t169], 16.0f / 9.0f},};
const int aspectratiosettingscount = sizeof(aspectratiosettings) / sizeof(aspectratiosettings[0]);

//////////////////////////////////////////////////////////////////////////
// menu items creation
void	AddLoginMenuItems(CGameMenuScreen*pScreen, IGameMenuControl*pAuthorizationControl, CGameWindow::SSettings&iCurrentSettings, IGameMenuControl*pEmailCtrl,IGameMenuControl*pPswdCtrl)
{
	pScreen->Controls().InsertAfter(pPswdCtrl, pAuthorizationControl );
	pScreen->Controls().InsertAfter(pEmailCtrl, pAuthorizationControl );
}

void	AddUsernameMenuItems(CGameMenuScreen*pScreen, IGameMenuControl*pAuthorizationControl, CGameWindow::SSettings&iCurrentSettings, IGameMenuControl*pUsernameCtrl)
{
	pScreen->Controls().InsertAfter(pUsernameCtrl, pAuthorizationControl );
}



//////////////////////////////////////////////////////////////////////////
// settings

//////////////////////////////////////////////////////////////////////////
// reset settings to default
void	CGameWindow::SSettings::Reset()
{
	m_iCurrentGraphics = 3;
	m_iCurrentGrass = 1;
	m_iCurrentAspectRatio = 1;
	m_iCurrentFullscreen = 0;
	m_iCurrentShadow = 1;

	wcscpy_s( m_szName, MAX_NAME_LENGTH+1, L"NoName" );
	wcscpy_s( m_szDescription, MAX_DESCRIPTION_LENGTH+1, L"..." );
	wcscpy_s( m_szTextureFile, MAX_TEXTUREFILE_LENGTH+1, L"snake1.jpg" );
	wcscpy_s( m_szUserEmail, MAX_EMAIL_LENGTH+1, L"" );
	wcscpy_s( m_szUserPassword, MAX_PASSWORD_LENGTH+1, L"" );
	
	wcscpy_s( m_szCustomWorldName, MAX_NAME_LENGTH+1, L"Custom World" );

	m_iAudio = 3;

	m_iChatVisibility = 2;
}


//////////////////////////////////////////////////////////////////////////
// save settings to file
bool	CGameWindow::SSettings::Save( TCHAR*szFileName )
{
	HANDLE hFile;

	// create the file
	hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return false;
	}

	// write data
	DWORD written;
	if ( !WriteFile( hFile, this, sizeof(SSettings), &written, NULL ) )
		return false;
	
	// close the file
	CloseHandle(hFile);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// load settings from file
bool	CGameWindow::SSettings::Load( TCHAR*szFileName )
{
	HANDLE hFile;

	// create the file
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return false;
	}
	
	// write data
	DWORD dataread;
	if ( !ReadFile( hFile, this, sizeof(SSettings), &dataread, NULL ) )
		return false;

	// close the file
	CloseHandle(hFile);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Game Window Procedure
LRESULT CALLBACK GameWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// pointer to the CGameWindow class (valid for all messages except WM_CREATE)
	CGameWindow *pGameWindow = (CGameWindow*)GetWindowLongPtr( hWnd, GWL_USERDATA );
	IGameControlReceiver	*pControlReceiver = pGameWindow ? pGameWindow->GetCurrentReceiver() : NULL;

	// process messages
	switch (msg)
	{
	case WM_CREATE:
		{
			CREATESTRUCT *createstruct = (CREATESTRUCT*)lParam;

			// store pointer to the CGameWindow class
			SetWindowLongPtr(hWnd,GWL_USERDATA, (LONG_PTR)createstruct->lpCreateParams);

			break;
		}

	case WM_PAINT:
		{
			ValidateRect(hWnd,NULL);
			return 0;
		}		

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	/*case WM_CONNECTIONRECEIVE:

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			// read from the socket
			pGameWindow->GetConnection()->Receive();
			break;
		case FD_CLOSE:
			// connection closed
			pGameWindow->m_connection.Disconnect();
			break;
		}

		return 0;*/

	case WM_SYSKEYDOWN:
		switch( wParam )
		{
		case VK_RETURN:
			{
				// Toggle full screen upon alt-enter 
				DWORD dwMask = (1 << 29);
				if( (lParam & dwMask) != 0 ) // Alt is down also
				{
					// Toggle the full screen/window mode
					/*DXUTPause( true, true );
					DXUTToggleFullScreen();
					DXUTPause( false, false );*/
					pGameWindow->OnCommandButton( NULL, mcFullscreen );
					
					return 0;
				}
			}
		case VK_MENU:
			{

				return 0;
			}
		}
		break;
	case WM_KEYDOWN:
		if ( pControlReceiver )
		{
			if ( pControlReceiver->OnWmKeyDown( pGameWindow, wParam, lParam ) )
				return 0;			
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
		if (pControlReceiver)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			float xPos = (float)LOWORD(lParam) / (float)rc.right; 
			float yPos = (float)HIWORD(lParam) / (float)rc.bottom;

			if ( msg == WM_MOUSEMOVE )
				pControlReceiver->OnMouseMove( pGameWindow, xPos, yPos  );
			else if (msg == WM_LBUTTONDOWN)
				pControlReceiver->OnClick( pGameWindow, xPos, yPos);

			return 0;
		}
		break;
	case WM_CHAR:
		if (pControlReceiver)
		{
			if ( pControlReceiver->OnWmChar( pGameWindow, wParam, lParam ) )
				return 0;
		}
		break;
	case WM_MOVE:
		if (pGameWindow)
			pGameWindow->DoFrame();
		break;
	}

	/*WCHAR x[128];
	wsprintf(x,L"0x%x\n",msg);
	OutputDebugString(x);*/

	// default procedure
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
// GameWindow ctor
CGameWindow::CGameWindow(HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_hWnd = NULL;
	m_pDefaultConnectButton = 0;

	m_dwLastFrameTime = 0;

	m_dFps = 0.0;

	ZeroMemory( m_menuScreens, sizeof(m_menuScreens) );
	m_pSelectFullscreen = NULL;

	// current graphics settings
	m_iCurrentSettings.Reset();

	//
	// Init WSA
	//
	WSAData wsadata;
	WSAStartup( MAKEWORD(1,1), &wsadata );

}

//////////////////////////////////////////////////////////////////////////
// GameWindow dtor
CGameWindow::~CGameWindow()
{
	// close connection
	m_connection.Disconnect();

	// remove invalid pointers
	m_customWorldNameCopies.Clear(false);

	// close WSA
	WSACleanup();

	// save settings
	m_iCurrentSettings.Save( strSettingsFile );

	// delete menu screens
	for (int i = 0; i < msCount; ++i)
		SAFE_DELETE( m_menuScreens[i] );
}

//////////////////////////////////////////////////////////////////////////
// create the game window
bool	CGameWindow::Create( LPSTR lpCmdLine, int nShowCmd )
{
	//
	// load settings
	//
	m_iCurrentSettings.Load( strSettingsFile );

	if (!CheckUsersLanguage())
	{
		AddLog(L"Error: Cannot obtain users language.");
		return false;
	}

	//
	// Create the window
	//
	if (!CreateGameWindow(graphicsettings[m_iCurrentSettings.m_iCurrentGraphics].width,graphicsettings[m_iCurrentSettings.m_iCurrentGraphics].height,nShowCmd))
	{
		AddLog(L"Error: Unable to create window.");
		return false;
	}

	//
	// Create the render device
	//
	if (!m_d3ddevice.CreateDevice(
		m_hWnd, m_iCurrentSettings.m_iCurrentFullscreen?true:false,
		graphicsettings[m_iCurrentSettings.m_iCurrentGraphics].width,graphicsettings[m_iCurrentSettings.m_iCurrentGraphics].height,
		graphicsettings[m_iCurrentSettings.m_iCurrentGraphics].samplescount,
		aspectratiosettings[m_iCurrentSettings.m_iCurrentAspectRatio].flAspect))
	{
		AddLog(L"Error: Unable to create any graphics device. Forcing settings reset.");
		m_iCurrentSettings.Reset();
		return false;
	}

	//
	UpdateWindowTitle();

	//
	// Init audio
	//
	if ( !m_audio.InitSoundManager() )
	{
		AddLog(L"Warning: Unable to initialize sound device. Audio won't be enabled.");
		// do not exit, continue without sound support // return false;
	}


	//
	// Create the game menu
	//
	if ( !m_menu.CreateMenu(m_d3ddevice,&m_audio) )
	{
		AddLog(L"Error: Unable to create menu object.");
		return false;
	}

	if ( !InitMenuScreens() )
	{
		AddLog(L"Error: Unable to initialize menu.");
		return false;
	}

	//
	// Create the world
	//
	if ( !m_gameWorld.CreateWorld( m_d3ddevice, &m_downloader, this ) )
	{
		AddLog(L"Error: Unable to create game world.");
		return false;
	}

	//
	// start updating servers.xml
	//
	m_serversupdate.StartDownload(this, SERVERS_XML_HOST, SERVERS_XML_SCRIPT);


	//
	// start, reset the last frame time value
	m_dwLastFrameTime = timeGetTime();

	// ok
	return true;
}


//////////////////////////////////////////////////////////////////////////
// check for users language name
bool	CGameWindow::CheckUsersLanguage()
{
	// WIN 2000 and later
	LANGID lng = GetUserDefaultUILanguage();
	
	m_gameWorld.SetUsersLanguage( lng );

	g_gameTexts.LoadTexts( (unsigned int)lng );

	//
	return true;
}


//////////////////////////////////////////////////////////////////////////
// creates the main window
void	CGameWindow::UpdateWindowTitle()
{
	WCHAR	buff[256];
	WCHAR	*pd;

	if ( m_d3ddevice.pDX10 )
	{
		pd = L"DirectX 10";
	}
	else if (m_d3ddevice.pDX9)
	{
		pd = L"DirectX 9";
	}
	else
	{
		pd = L"none";
	}

	swprintf_s(buff, L"%s (%s)", strGameWndTitle, pd );

	SetWindowText( m_hWnd, buff );
}


//////////////////////////////////////////////////////////////////////////
// creates the main window
bool	CGameWindow::CreateGameWindow( unsigned int iWidth, unsigned int iHeight, int nShowCmd )
{
	//
	// Register class
	//
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = GameWndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = m_hInstance;
	wcex.hIcon          = LoadIcon( NULL, MAKEINTRESOURCE(IDI_APPLICATION) );
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = strGameWndClass;
	wcex.hIconSm        = LoadIcon( NULL, MAKEINTRESOURCE(IDI_APPLICATION) );

	if( !RegisterClassEx(&wcex) )
		return false;

	//
	// Create window
	//
	RECT rc = { 0, 0, iWidth, iHeight };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	m_hWnd = CreateWindow(
		strGameWndClass, strGameWndTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL,
		m_hInstance,
		this);

	if( !m_hWnd )
		return false;

	//
	// Show window
	//
	ShowWindow( m_hWnd, nShowCmd );

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// init game menu screens
bool	CGameWindow::InitMenuScreens()
{
	int i;

	// setup menu screens
	m_menuScreens[msMain] = new CGameMenuScreen( D3DXVECTOR4(0,0,0,0.5), g_gameTexts[tMainMenu] );
	m_menuScreens[msNewGame] = new CGameMenuScreen( D3DXVECTOR4(0,0.3f,0,0.5f), g_gameTexts[tNewGame] );
	m_menuScreens[msConnect] = new CGameMenuScreen( D3DXVECTOR4(0.0f,0.1f,0.2f,0.5f), g_gameTexts[tConnectTo] );
	m_menuScreens[msConnecting] = new CGameMenuScreen( D3DXVECTOR4(0.0f,0.1f,0.2f,0.5f), g_gameTexts[tConnecting] );
	m_menuScreens[msReconnect] = new CGameMenuScreen( D3DXVECTOR4(1.0f,0.1f,0.1f,0.5f), g_gameTexts[tReconnect] );
	m_menuScreens[msQuitGame] = new CGameMenuScreen( D3DXVECTOR4(0.05f,0.15f,0.3f,0.5f), g_gameTexts[tQuitGame] );
	m_menuScreens[msSettings] = new CGameMenuScreen( D3DXVECTOR4(0.2f,0.1f,0.0f,0.5f), g_gameTexts[tSettings] );
	m_menuScreens[msInGame] = new CInGameScreen(mcChatMessage);

	// main menu
	m_menuScreens[msMain]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tNewGame], m_menuScreens[msNewGame] ) );
	m_menuScreens[msMain]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tSettings], m_menuScreens[msSettings] ) );
	m_menuScreens[msMain]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tQuitGame], m_menuScreens[msQuitGame] ) );

	// new game menu
	m_menuScreens[msNewGame]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tStartGame], m_menuScreens[msConnect] ) );

	{
		bool bUseAuthorization = m_iCurrentSettings.m_szUserEmail[0]!=0&&m_iCurrentSettings.m_szUserPassword[0]!=0;

		m_gameWorld.SetUsersName( CString(m_iCurrentSettings.m_szName) );
		m_gameWorld.SetUsersEmail( CString(m_iCurrentSettings.m_szUserEmail) );
		m_gameWorld.SetUsersPassword( CString(m_iCurrentSettings.m_szUserPassword) );
		m_gameWorld.SetUsersDescription( CString(m_iCurrentSettings.m_szDescription) );

		CMenuCheckBox*pAuthorizationBox;
		m_menuScreens[msNewGame]->Controls().Add( pAuthorizationBox = new CMenuCheckBox( mcUseAuthorization, g_gameTexts[tUseAuthorization], bUseAuthorization ) );

		m_pEmailCtrl = new CMenuEditBox( mcUserEmail, g_gameTexts[tEmail], &CString(m_iCurrentSettings.m_szUserEmail), MAX_EMAIL_LENGTH);
		m_pPasswordCtrl = new CMenuEditBox( mcUserPassword, g_gameTexts[tPassword], &CString(m_iCurrentSettings.m_szUserPassword), MAX_PASSWORD_LENGTH, true);
		m_pUsernameCtrl = new CMenuEditBox( mcName, g_gameTexts[tName], &CString(m_iCurrentSettings.m_szName), MAX_NAME_LENGTH);

		if (bUseAuthorization)
		{
			AddLoginMenuItems(m_menuScreens[msNewGame],pAuthorizationBox,m_iCurrentSettings, m_pEmailCtrl, m_pPasswordCtrl);
		}
		else
		{
			AddUsernameMenuItems(m_menuScreens[msNewGame],pAuthorizationBox,m_iCurrentSettings, m_pUsernameCtrl);
		}

		
		//m_menuScreens[msNewGame]->Controls().Add( new CMenuEditBox( mcDescription, g_gameTexts[tDescription], &CString(m_iCurrentSettings.m_szDescription), MAX_DESCRIPTION_LENGTH ) );
	}

	CMenuTextureSelect*pTextureSelect = new CMenuTextureSelect( mcTexture, g_gameTexts[tTexture], CString(L"media\\snakes\\") );
	InitTextureSelect(pTextureSelect);
	m_menuScreens[msNewGame]->Controls().Add( pTextureSelect );

	m_menuScreens[msNewGame]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tBack], m_menuScreens[msMain] ) );

	// connecting menu
	m_menuScreens[msConnecting]->Controls().Add( new CMenuButton( mcNone, g_gameTexts[tWait] ) );
	m_menuScreens[msConnecting]->Controls().Add( new CMenuButton( mcCancelConnect, g_gameTexts[tCancel] ) );
	
	// connect menu &&
	// reconnect menu
	InitConnectMenu();
	
	// quit game menu
	m_menuScreens[msQuitGame]->Controls().Add( new CMenuButton( mcQuitGame, g_gameTexts[tYes] ) );
	m_menuScreens[msQuitGame]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tNo], m_menuScreens[msMain] ) );

	// settings menu
	CMenuSelect	*pSelectGrass = new CMenuSelect(mcGrass, g_gameTexts[tGrassDetail]);
	pSelectGrass->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tOff], 0 ) );
	pSelectGrass->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tLowest], 1 ) );
	pSelectGrass->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tLow], 2 ) );
	pSelectGrass->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tMedium], 3 ) );
	pSelectGrass->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tHigh], 4 ) );
	pSelectGrass->SetSelectedItemIndex( this, m_iCurrentSettings.m_iCurrentGrass );	// => m_gameWorld.SetGrassDetail
	m_menuScreens[msSettings]->Controls().Add( pSelectGrass );

	if ( m_audio.IsInitialized() )
	{
		CMenuSelect	*pSelectAudio = new CMenuSelect(mcAudioEnable, g_gameTexts[tAudio]);
		pSelectAudio->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tOff], 0 ) );
		pSelectAudio->Items().Add( new CMenuSelect::SMenuSelectItem( CString(L"33%"), 1 ) );
		pSelectAudio->Items().Add( new CMenuSelect::SMenuSelectItem( CString(L"66%"), 2 ) );
		pSelectAudio->Items().Add( new CMenuSelect::SMenuSelectItem( CString(L"100%"), 3 ) );
		pSelectAudio->SetSelectedItemIndex( this, m_iCurrentSettings.m_iAudio );	// => m_audio update
		m_menuScreens[msSettings]->Controls().Add( pSelectAudio );
	}

	CMenuSelect	*pSelectChatVisibility = new CMenuSelect(mcChatVisibility, g_gameTexts[tChatVisibility]);
	pSelectChatVisibility->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tOff], 0 ) );
	pSelectChatVisibility->Items().Add( new CMenuSelect::SMenuSelectItem( CString(L"33%"), 1 ) );
	pSelectChatVisibility->Items().Add( new CMenuSelect::SMenuSelectItem( CString(L"66%"), 2 ) );
	pSelectChatVisibility->Items().Add( new CMenuSelect::SMenuSelectItem( CString(L"100%"), 3 ) );
	pSelectChatVisibility->SetSelectedItemIndex( this, m_iCurrentSettings.m_iChatVisibility );	// => m_audio update
	m_menuScreens[msSettings]->Controls().Add( pSelectChatVisibility );

	CMenuSelect	*pSelectGraphics = new CMenuSelect(mcGraphics, g_gameTexts[tGraphics]);
	for (i = 0; i < graphicsettingscount; ++i)
		pSelectGraphics->Items().Add( new CMenuSelect::SMenuSelectItem( *graphicsettings[i].plabel, i ) );
	pSelectGraphics->SetSelectedItemIndex( NULL, m_iCurrentSettings.m_iCurrentGraphics );

	m_menuScreens[msSettings]->Controls().Add( pSelectGraphics );

	CMenuSelect	*pSelectAspectRatio = new CMenuSelect(mcAspectRatio, g_gameTexts[tFullscreenAspectRatio]);
	for (i = 0; i < aspectratiosettingscount; ++i)
		pSelectAspectRatio->Items().Add( new CMenuSelect::SMenuSelectItem( *aspectratiosettings[i].plabel, i ) );
	pSelectAspectRatio->SetSelectedItemIndex( NULL, m_iCurrentSettings.m_iCurrentAspectRatio );
	m_menuScreens[msSettings]->Controls().Add( pSelectAspectRatio );

	/*CMenuSelect	*pSelectShadows = new CMenuSelect(mcShadows, CString("Shadows"));
	pSelectShadows->Items().Add( new CMenuSelect::SMenuSelectItem( CString("Off"), 0 ) );
	pSelectShadows->Items().Add( new CMenuSelect::SMenuSelectItem( CString("On"), 1 ) );
	pSelectShadows->SetSelectedItemIndex( NULL, 1 );
	m_menuScreens[msSettings]->Controls().Add( pSelectShadows );*/	// always enable

	m_pSelectFullscreen = new CMenuSelect(mcFullscreen, g_gameTexts[tFullscreen]);
	m_pSelectFullscreen->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tNo], 0 ) );
	m_pSelectFullscreen->Items().Add( new CMenuSelect::SMenuSelectItem( g_gameTexts[tYes], 1 ) );
	m_pSelectFullscreen->SetSelectedItemIndex( NULL, m_iCurrentSettings.m_iCurrentFullscreen );	// => m_gameWorld.SetGrassDetail
	m_menuScreens[msSettings]->Controls().Add( m_pSelectFullscreen );

	m_menuScreens[msSettings]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tBack], m_menuScreens[msMain] ) );

	// set current menu screen
	m_menu.SetCurrentScreen( m_menuScreens[msMain], false );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// reload servers.xml
void	CGameWindow::OnDownloaded( char*pdata, unsigned long ndata ) 
{
	if ( pdata && ndata )
	{
		// start pattern
		const char szstart[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><servers>";
		const int nstart = sizeof(szstart);

		if ( ndata < nstart || memcmp( pdata,szstart, nstart - 1 ) != 0 )
			return;

		HANDLE hFile;

		// create the file
		hFile = CreateFile( SERVERS_XML, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

		if (hFile == INVALID_HANDLE_VALUE) 
		{
			return;
		}

		// write data
		DWORD written;
		WriteFile( hFile, pdata, ndata, &written, NULL );
		
		// close the file
		CloseHandle(hFile);

		AddLog(L"Info: servers.xml downloaded and saved successfully.");
	
		InitConnectMenu();
	}
	
}


//////////////////////////////////////////////////////////////////////////
// read servers.xml
void	CGameWindow::InitConnectMenu()
{
	XmlDocument docServers;

	docServers.LoadFromFile( SERVERS_XML );

	locked( 

		m_customWorldNameCopies.Clear(false);

		FillConnectMenu( m_menuScreens[msConnect], docServers );
		FillConnectMenu( m_menuScreens[msReconnect], docServers );		
		

		m_menuScreens[msConnect]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tBack], m_menuScreens[msNewGame] ) );
		m_menuScreens[msReconnect]->Controls().Add( new CMenuGoToButton( mcGoTo, g_gameTexts[tBack], m_menuScreens[msMain] ) );

	);
}


//////////////////////////////////////////////////////////////////////////
// insert connect buttons
bool	CGameWindow::FillConnectMenu( CGameMenuScreen*pScreen, XmlDocument&doc )
{
	pScreen->Controls().Clear(true);

	XmlNode*pRoot = doc.DocumentElement();

	if ( !pRoot )
		return false;

	XmlNodeList&nodes = pRoot->ChildNodes;

	char buff[64];

	for ( XmlNodeList::iterator it = nodes.GetIterator(); !it.EndOfList(); ++it )
	{
		XmlAttribute*paddr = it->Attributes[L"address"];
		XmlAttribute*pport = it->Attributes[L"port"];
		XmlAttribute*pworldname = it->Attributes[L"worldname"];
		if ( paddr && pport )
		{
			// address to char
			int n = WideCharToMultiByte( CP_ACP, 0, paddr->Value.str(), paddr->Value.length(), buff, 64, NULL, NULL );
			buff[n] = 0;
			// port to int
			int port = wcstol( pport->Value.str(), NULL, 10 );

			// insert connect button
			pScreen->Controls().Add(
				m_pDefaultConnectButton = 
				new CMenuConnectButton(
					mcConnect,
					it->InnerText,
					buff,
					port,
					pworldname?(pworldname->Value):(CString(L""))
					)
				);
		}
	}

	// custom world name
	// insert connect button
	CMenuEditBox *b;
	pScreen->Controls().Add(
		b = new CMenuEditBox( mcCustomWorldConnect, CString(L""), &CString(m_iCurrentSettings.m_szCustomWorldName), MAX_NAME_LENGTH)
		);

	m_customWorldNameCopies.Add(b);


	return true;
}


//////////////////////////////////////////////////////////////////////////
// init texture select control
// list available textures and select the current
bool	CGameWindow::InitTextureSelect( CMenuTextureSelect*pTextureSelect )
{
	int i = 0, iselected = 0;

	WIN32_FIND_DATA data;
	HANDLE hFind;

	// Find the first file in the directory.
	hFind = FindFirstFile(L"media\\snakes\\*.jpg", &data);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return false;
	} 
	else 
	{
		do
		{
			pTextureSelect->Items().Add( new CMenuTextureSelect::SMenuSelectTexture( CString(data.cFileName), i ) );

			if ( wcsncmp( data.cFileName, m_iCurrentSettings.m_szTextureFile, MAX_TEXTUREFILE_LENGTH+1 ) == 0 )
				iselected = i;

			++i;

		} while (FindNextFile(hFind, &data) != 0);
		
		FindClose(hFind);
	}
	
	pTextureSelect->SetSelectedItemIndex( this, iselected );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// do moves
void	CGameWindow::DoFrame()
{
	// get current frame time
	DWORD dwCurFrameTime = timeGetTime();

	if (dwCurFrameTime == m_dwLastFrameTime)
		return;	// no move

	// get the time in seconds elapsed from the last frame move
	double dStep = ( (double)( dwCurFrameTime - m_dwLastFrameTime ) ) * 0.001;
	if (dStep > 1.0)	dStep =  1.0;

	locked(

		// do moves
		FrameMove( dStep );

		// render the window
		if ( m_d3ddevice.pDX10 )
			Render10(*m_d3ddevice.pDX10);
		else if ( m_d3ddevice.pDX9 )
			Render9(*m_d3ddevice.pDX9);
	);

	//DrawFps();
	
	
	// blt the backbuffer
	// check for errors
	m_d3ddevice.Present();

	// update last frame move time
	m_dwLastFrameTime = dwCurFrameTime;
}


//////////////////////////////////////////////////////////////////////////
// update fps
void	CGameWindow::UpdateFps( double dStep )
{
	m_dFps = ( m_dFps * 10.0 + (1.0 / dStep) ) / 11.0;	// average fps for last 11 frames
}


//////////////////////////////////////////////////////////////////////////
// render current fps
void	CGameWindow::DrawFps()
{
	WCHAR buff[64];
	swprintf_s( buff, L"fps:%i", (int)m_dFps );

	m_d3ddevice.RenderText( CString(buff), D3DXCOLOR(1,0,0,1),D3DXVECTOR2(0,0.9f) );
}

//////////////////////////////////////////////////////////////////////////
// move with the game objects
void	CGameWindow::FrameMove( double dStep )
{
	// update fps
	UpdateFps(dStep);

	// move with the world objects
	m_gameWorld.FrameMove( m_d3ddevice, m_audio, dStep );

	// animate the menu
	m_menu.FrameMove( dStep );

	// audio update
	m_audio.UpdateAudio( (float)dStep );
}


//////////////////////////////////////////////////////////////////////////
// render the window using DX 10
void	CGameWindow::Render10( CDX10Device&dev )
{
	if ( dev.BeginRender() )
	{
		//
		// render the world
		//
		m_gameWorld.Render( m_d3ddevice );

		dev.EndRender();

		//
		// render the menu
		//

		m_menu.RenderMenu10( m_d3ddevice, dev.pRenderTargetShaderView );

		// used device name
		//dev.RenderText( CString(L"DirectX 10"), D3DXCOLOR(1,1,1,0.2f), D3DXVECTOR2(0,0) );
	}
	
}


//////////////////////////////////////////////////////////////////////////
// render the window using D3D 9
void	CGameWindow::Render9( CDX9Device&dev )
{
	if (SUCCEEDED(dev.pd3dDevice->BeginScene()))
	{
		m_gameWorld.Render( m_d3ddevice );
		m_menu.RenderMenu9( m_d3ddevice );

		// used device name
		//dev.RenderText( CString(L"DirectX 9"), D3DXCOLOR(1,1,1,0.2f), D3DXVECTOR2(0,0) );

		dev.pd3dDevice->EndScene();
	}
	
}


//////////////////////////////////////////////////////////////////////////
// menu button pressed
void	CGameWindow::OnCommandButton( IGameMenuControl*pControlFrom, int iCmd, int cmdParam )
{
	EMenuCommands cmd = (EMenuCommands)iCmd;

	switch (cmd)
	{
	case mcGoTo:
		m_menu.SetCurrentScreen( ((CMenuGoToButton*)pControlFrom)->m_pScreen );
		break;
	case mcQuitGame:
		PostQuitMessage(0);
		break;
	case mcConnect:
		{
			CMenuConnectButton*pbtn = (CMenuConnectButton*)pControlFrom;

			m_menu.SetCurrentScreen( m_menuScreens[msConnecting] );

			m_gameWorld.SetWorldName( pbtn->m_strWorldName );

			m_downloader.SetAddress(pbtn->m_szAddress, pbtn->m_iPort);
			m_connection.Connect( pbtn->m_szAddress, pbtn->m_iPort, this, true );
		}
		break;
	case mcCustomWorldConnect:
		{
			CMenuEditBox*pbtn = (CMenuEditBox*)pControlFrom;

			wcscpy_s( m_iCurrentSettings.m_szCustomWorldName, MAX_NAME_LENGTH+1, pbtn->Text() );
			
			if (cmdParam == 1)
			{
				for (CList<CMenuEditBox>::iterator i = m_customWorldNameCopies.GetIterator(); !i.EndOfList(); ++i)
				{
					i->ResetText();
					wcscpy_s( i->Text(), MAX_NAME_LENGTH+1, m_iCurrentSettings.m_szCustomWorldName );
				}


				m_menu.SetCurrentScreen( m_menuScreens[msConnecting] );

				m_gameWorld.SetWorldName( CString(pbtn->Text()) );

				m_downloader.SetAddress(m_pDefaultConnectButton->m_szAddress, m_pDefaultConnectButton->m_iPort);
				m_connection.Connect( m_pDefaultConnectButton->m_szAddress, m_pDefaultConnectButton->m_iPort, this, true );
			}
		}
		break;
		break;
	case mcCancelConnect:
		m_connection.Disconnect();
		break;
	case mcGrass:
		{
			CMenuSelect*pSelectGrass = (CMenuSelect*)pControlFrom;
			m_iCurrentSettings.m_iCurrentGrass = pSelectGrass->GetSelectedItem()->val;
			
			m_gameWorld.SetGrassDetail( (float)(m_iCurrentSettings.m_iCurrentGrass) / 4.0f );
		}
		break;
	case mcAudioEnable:
		{
			CMenuSelect*pSelectAudio = (CMenuSelect*)pControlFrom;
			m_iCurrentSettings.m_iAudio = pSelectAudio->GetSelectedItem()->val;

			m_audio.SetVolume( (float)m_iCurrentSettings.m_iAudio / 3.0f );
		}
		break;
	case mcGraphics:
		{
			CMenuSelect*pSelectGraphics = (CMenuSelect*)pControlFrom;

			m_iCurrentSettings.m_iCurrentGraphics = pSelectGraphics->GetSelectedItem()->val;

			const SGraphicsSettings*p = &graphicsettings[m_iCurrentSettings.m_iCurrentGraphics];
			
			if ( m_d3ddevice.pDX10 )
			{
				if (!m_d3ddevice.pDX10->Resize( p->width, p->height, p->samplescount ) && m_iCurrentSettings.m_iCurrentGraphics > 0)
				{
					pSelectGraphics->SetSelectedItemIndex( this, m_iCurrentSettings.m_iCurrentGraphics - 1 );
				}
			}
			else if ( m_d3ddevice.pDX9 )
			{
				// app must be restarted
				pSelectGraphics->bShowStar = true;
			}
			
		}
		break;
	case mcAspectRatio:
		{
			CMenuSelect*pSelectAR = (CMenuSelect*)pControlFrom;
			m_iCurrentSettings.m_iCurrentAspectRatio = pSelectAR->GetSelectedItem()->val;

			if ( m_d3ddevice.pDX10 )
			{
				m_d3ddevice.pDX10->flFullScreenAspectRatio = ( aspectratiosettings[m_iCurrentSettings.m_iCurrentAspectRatio].flAspect );
			}
			else if (m_d3ddevice.pDX9)
			{
				m_d3ddevice.pDX9->flFullScreenAspectRatio = ( aspectratiosettings[m_iCurrentSettings.m_iCurrentAspectRatio].flAspect );
			}
			

			m_gameWorld.ResetProjection(m_d3ddevice);
		}
		break;
	case mcShadows:
		{
			CMenuSelect*pSelectShadows = (CMenuSelect*)pControlFrom;
			m_iCurrentSettings.m_iCurrentShadow = pSelectShadows->GetSelectedItem()->val;

			m_gameWorld.EnableShadows( m_iCurrentSettings.m_iCurrentShadow != 0 );
		}
		break;
	case mcFullscreen:
		{
			if (pControlFrom && pControlFrom == m_pSelectFullscreen)
			{
				m_iCurrentSettings.m_iCurrentFullscreen = m_pSelectFullscreen->GetSelectedItem()->val;

				if ( m_d3ddevice.pDX10 )
				{
					if ( m_d3ddevice.pDX10->ToggleFullscreen() )
					{
						m_gameWorld.ResetProjection(m_d3ddevice);
					}
				}
				else if (m_d3ddevice.pDX9)
				{
					// app must be restarted
					m_pSelectFullscreen->bShowStar = true;
				}				
			}
			else
			{
				m_pSelectFullscreen->SetSelectedItemIndex( this, m_iCurrentSettings.m_iCurrentFullscreen ? 0 : 1 );
			}
		}
		break;
	case mcUseAuthorization:
		{
			CMenuCheckBox*pUse = (CMenuCheckBox*)pControlFrom;

			m_menuScreens[msNewGame]->Controls().Remove( m_pEmailCtrl,false );
			m_menuScreens[msNewGame]->Controls().Remove( m_pPasswordCtrl,false );
			m_menuScreens[msNewGame]->Controls().Remove( m_pUsernameCtrl,false );

			if (pUse->IsChecked())
			{
				AddLoginMenuItems(m_menuScreens[msNewGame],pUse,m_iCurrentSettings, m_pEmailCtrl, m_pPasswordCtrl);
			}
			else
			{
				AddUsernameMenuItems(m_menuScreens[msNewGame],pUse,m_iCurrentSettings, m_pUsernameCtrl);

				wcscpy_s( m_iCurrentSettings.m_szUserPassword, MAX_PASSWORD_LENGTH+1, L"" );
				m_gameWorld.SetUsersPassword( CString(L"") );
				((CMenuEditBox*)m_pPasswordCtrl)->ResetText();
			}
		}
		break;
	case mcName:
		{
			CMenuEditBox*pName = (CMenuEditBox*)pControlFrom;

			wcscpy_s( m_iCurrentSettings.m_szName, MAX_NAME_LENGTH+1, pName->Text() );
			m_gameWorld.SetUsersName( CString(pName->Text()) );
		}
		break;
	case mcDescription:
		{
			CMenuEditBox*pDesc = (CMenuEditBox*)pControlFrom;

			wcscpy_s( m_iCurrentSettings.m_szDescription, MAX_DESCRIPTION_LENGTH+1, pDesc->Text() );
			m_gameWorld.SetUsersDescription( CString(pDesc->Text()) );
		}
		break;
	case mcTexture:
		{
			CMenuSelect*pSelectTexture = (CMenuSelect*)pControlFrom;

			CMenuSelect::SMenuSelectItem*pSelected = pSelectTexture->GetSelectedItem();

			if ( pSelected )
			{
				wcscpy_s( m_iCurrentSettings.m_szTextureFile, MAX_TEXTUREFILE_LENGTH+1, pSelected->str );
				m_gameWorld.SetUsersDefaultTexture( pSelected->str );
			}
		}
		break;
	case mcUserEmail:
		{
			CMenuEditBox*pEmail = (CMenuEditBox*)pControlFrom;

			wcscpy_s( m_iCurrentSettings.m_szUserEmail, MAX_EMAIL_LENGTH+1, pEmail->Text() );
			m_gameWorld.SetUsersEmail( CString(pEmail->Text()) );
		}
		break;
	case mcUserPassword:
		{
			CMenuEditBox*pPassword = (CMenuEditBox*)pControlFrom;

			wcscpy_s( m_iCurrentSettings.m_szUserPassword, MAX_PASSWORD_LENGTH+1, pPassword->Text() );
			m_gameWorld.SetUsersPassword( CString(pPassword->Text()) );
		}
		break;
	case mcChatMessage:
		{
			CMenuEditBox*pEdit = (CMenuEditBox*)pControlFrom;

			if ( pEdit->Text()[0] == 0 )
				break;	// zero length text

			CString str( pEdit->Text() );
			pEdit->ResetText();

			// send "str"
			m_gameWorld.Send_ChatMesasage( &m_connection, CString(L""), str );
		}
		break;
	case mcChatVisibility:
		{
			CMenuSelect*pSelectVisibility = (CMenuSelect*)pControlFrom;
			m_iCurrentSettings.m_iChatVisibility = pSelectVisibility->GetSelectedItem()->val;

			((CInGameScreen*)m_menuScreens[msInGame])->SetVisibility( (float)m_iCurrentSettings.m_iChatVisibility / 3.0f );
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
// game world events observer
// chat message received
void	CGameWindow::OnChatMessageReceived( CString&strFrom, CString&strMessage )
{
	((CInGameScreen*)m_menuScreens[msInGame])->AddMessage( strFrom, strMessage );
}


//////////////////////////////////////////////////////////////////////////
// current command receiver
IGameControlReceiver*CGameWindow::GetCurrentReceiver()
{
	if ( m_menu.WantInput() )
		return &m_menu;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////
// connection command observer
void	CGameWindow::OnConnectionCommand( const unsigned char uCmdId, CBinaryReader*pbr )
{
	
	// just send the command to the world
	m_gameWorld.OnConnectionCommand(uCmdId,pbr);

}


//////////////////////////////////////////////////////////////////////////
// connection connected/closed event
void	CGameWindow::OnConnectionConnected( CConnection*pConnection, CString*message )
{
	locked(

		// show/hide the menu
		if ( pConnection )
		{
			//
			// connected to the server
			// hide menu and play "start" sound
			//

			m_menu.SetCurrentScreen(m_menuScreens[msInGame]);

			m_audioStart.PlaySegment( &m_audio.sndStart, false, false );
		}
		else
		{
			//
			// connection was closed
			// disable downloader object and show "reconnect" menu
			//

			m_downloader.SetAddress(0,0);

			m_menu.SetCurrentScreen(m_menuScreens[msReconnect]);

			m_menuScreens[msReconnect]->Title = (message)?*message:g_gameTexts[tReconnect];
		}

		// 
		m_gameWorld.OnConnectionConnected(pConnection,message);

	);
}