#ifndef _GAMEWINDOW_H_
#define _GAMEWINDOW_H_

//////////////////////////////////////////////////////////////////////////
#include "connection.h"	// connection to the server
#include "downloader.h"	// download manager

#include "locker.h"

#include <windows.h>	// HWND, HINSTANCE
#include "myworld.h"	// world

#include "menu.h"		// game menu
#include "menuitemtextureselect.h"	// texture select
#include "xml.h"		// XmlDocument

#include "device.h"			// graphics
#include "soundmanager.h"	// sound

#include "webdownloader.h"	// update servers list for the next time

//////////////////////////////////////////////////////////////////////////
#define	MAX_NAME_LENGTH			(32)
#define	MAX_DESCRIPTION_LENGTH	(64)
#define MAX_TEXTUREFILE_LENGTH	(128)
#define MAX_EMAIL_LENGTH	(255)
#define MAX_PASSWORD_LENGTH	(255)

//////////////////////////////////////////////////////////////////////////
// game menu screens
enum	EMenuScreen
{
	msMain = 0,
	msNewGame,
	msConnect,
	msConnecting,
	msReconnect,
	msSettings,
	msQuitGame,

	msInGame,

	msCount
};


//////////////////////////////////////////////////////////////////////////
// main window
class CGameWindow: public CGameMenuObserver, public CCommandsObserver, protected ILocker, public CMyGameWorldEvents, protected CWebDownloaderObserver
{
public:
	CGameWindow(HINSTANCE hInstance);
	virtual ~CGameWindow();

	//
	// window
	//

	bool	Create(LPSTR lpCmdLine, int nShowCmd);

	void	DoFrame();

	HWND	GetHWnd(){ return m_hWnd; }

	//
	// connection
	//

	CConnection*	GetConnection(){ return &m_connection; }

	virtual void OnConnectionCommand( const unsigned char uCmdId, CBinaryReader*pbr );	// new command received, cmd and params are null-terminated strings
	virtual void OnConnectionConnected( CConnection*pConnection, CString*message );				// connection connected / closed

	//
	// menu
	//

	// handles press button event
	virtual void	OnCommandButton( IGameMenuControl*pControlFrom, int iCmd );

	// get current command receiver
	IGameControlReceiver*	GetCurrentReceiver();

	struct SSettings
	{
		int m_iCurrentGraphics,		// current graphics settings index
			m_iCurrentGrass,		// current grass settings index
			m_iCurrentAspectRatio,	// current aspect ratio settings index
			m_iCurrentFullscreen,	// full-screen enabled
			m_iCurrentShadow;		// shadow enabled

		WCHAR	m_szName[MAX_NAME_LENGTH+1];	// players name
		WCHAR	m_szDescription[MAX_DESCRIPTION_LENGTH+1];	// players description
		WCHAR	m_szTextureFile[MAX_TEXTUREFILE_LENGTH+1];	// texture file name

		int	m_iAudio;		// audio enabled

		int	m_iChatVisibility;	// 0 - 2

		WCHAR	m_szUserEmail[MAX_EMAIL_LENGTH+1];	// user login email
		WCHAR	m_szUserPassword[MAX_PASSWORD_LENGTH+1];	// user login password

		void	Reset();
		bool	Load( TCHAR*szFileName );
		bool	Save( TCHAR*szFileName );
	};

private:

	bool		CreateGameWindow( unsigned int iWidth, unsigned int iHeight, int nShowCmd );
	bool		CheckUsersLanguage();
	void		UpdateWindowTitle();

	HINSTANCE	m_hInstance;
	HWND		m_hWnd;

	CMyGameWorld	m_gameWorld;

	DWORD		m_dwLastFrameTime;
	double		m_dFps;

	void		FrameMove( double dStep );
	void		Render9( CDX9Device&dev );
	void		Render10( CDX10Device&dev );

	void		UpdateFps( double dStep );
	void		DrawFps();

	// menu and menu screens
	CGameMenu	m_menu;

	CGameMenuScreen	*m_menuScreens[msCount];
	IGameMenuControl*m_pEmailCtrl,*m_pPasswordCtrl,*m_pUsernameCtrl;

	bool	InitMenuScreens();
	void	InitConnectMenu();
	bool	FillConnectMenu( CGameMenuScreen*pScreen, XmlDocument&doc );
	bool	InitTextureSelect( CMenuTextureSelect*pTextureSelect );

	virtual void OnDownloaded( char*pdata, unsigned long ndata );

	CMenuSelect	*m_pSelectFullscreen;

	SSettings m_iCurrentSettings;			// current settings ( = image of settings file )
	
	CAudioPath	m_audioStart;

	// connection to the server
	CConnection	m_connection;
	CDownloader	m_downloader;

	CWebDownloader	m_serversupdate;

	// game world events observer
	virtual void	OnChatMessageReceived( CString&strFrom, CString&strMessage );

	// render device
	CDXDevice m_d3ddevice;

	// audio device
	CSoundManager	m_audio;
	
};

#endif//_GAMEWINDOW_H_