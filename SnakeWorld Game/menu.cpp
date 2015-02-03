#include "menu.h"


//////////////////////////////////////////////////////////////////////////
// menu ctor
CGameMenu::CGameMenu()
{
	m_pCurrentScreen = m_pPrevScreen = NULL;
	m_flFade = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// menu dtor
CGameMenu::~CGameMenu()
{
	
}


//////////////////////////////////////////////////////////////////////////
// set new menu screen
void	CGameMenu::SetCurrentScreen( CGameMenuScreen*pNewScreen, bool bAnimation )
{
	m_pPrevScreen = m_pCurrentScreen;
	m_pCurrentScreen = pNewScreen;

	if ( bAnimation )
		m_flFade = -1.0f;
	else
		m_flFade = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// animate the menu
void	CGameMenu::FrameMove( double dStep )
{
	if ( m_pPrevScreen || m_flFade < 0.0f )
	{
		// move with screens
		m_flFade += (float)(dStep*2.0);

		if ( m_flFade >= 0.0f )
		{	// prev screen no more visible
			m_pPrevScreen = NULL;
			m_flFade = 0.0f;
		}

	}

	if ( m_pCurrentScreen )
		m_pCurrentScreen->FrameMove(dStep);
}


//////////////////////////////////////////////////////////////////////////
// render the menu
// (if its visible)
void	CGameMenu::RenderMenu10( CDXDevice&dev, ID3D10ShaderResourceView*pBackground )
{
	float flPos = m_flFade + 1.0f;

	flPos *= flPos;

	if ( m_pPrevScreen )
	{
		m_pPrevScreen->RenderScreen10( dev, pBackground, this, flPos );
	}

	if ( m_pCurrentScreen )
	{
		m_pCurrentScreen->RenderScreen10( dev, pBackground, this, flPos - 1.0f );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the menu using DX9
// (if its visible)
void	CGameMenu::RenderMenu9( CDXDevice&dev )
{
	float flPos = m_flFade + 1.0f;

	flPos *= flPos;

	if ( m_pPrevScreen )
	{
		m_pPrevScreen->RenderScreen9( dev, this, flPos );
	}

	if ( m_pCurrentScreen )
	{
		m_pCurrentScreen->RenderScreen9( dev, this, flPos - 1.0f );
	}
}


//////////////////////////////////////////////////////////////////////////
// controls
void	CGameMenu::OnClick( CGameMenuObserver*pObserver, float x, float y )
{
	if (m_pCurrentScreen)
	{
		m_audioPath.PlaySegment( &m_pSndManager->sndClick, false, false );
		m_pCurrentScreen->OnClick( pObserver, x,y );
	}
}
void	CGameMenu::OnMouseMove( CGameMenuObserver*pObserver, float x, float y )
{
	if (m_pCurrentScreen)
		m_pCurrentScreen->OnMouseMove(pObserver, x,y );
}
bool	CGameMenu::OnWmChar( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam )
{
	IGameControlReceiver*pScreen = m_pCurrentScreen;

	if (pScreen)
	{
		if ( (WCHAR)wParam == VK_RETURN )
		{
			m_audioPath.PlaySegment( &m_pSndManager->sndClick, false, false );
		}

		return pScreen->OnWmChar( pObserver, wParam, lParam );
	}
	else
		return false;
}

bool	CGameMenu::OnWmKeyDown( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam )
{
	IGameControlReceiver*pScreen = m_pCurrentScreen;

	if (pScreen)
		return pScreen->OnWmKeyDown( pObserver, wParam, lParam );
	else
		return false;
}