#include "mychat.h"
#include "texts.h"
#include "menuitems.h"


#define MAX_CHAT_MESSAGE_LENGTH	(96)

#define CHAT_COLOR	( D3DXCOLOR( 1,1,1, 0.8f ) )	// chat text color
#define WARNING_COLOR	( D3DXCOLOR( 0.8f,0.0f,0.0f, 1.0f ) )	// chat text color

#define MIN_SEND_TIME_INTERVAL	(2.0)

CInGameScreen::CInGameScreen( int iChatMessageCommand )
: CGameMenuScreen( D3DXVECTOR4(1,1,1,1), CString(L"In-Game") )
, m_iChatMessageCommand(iChatMessageCommand)
, flMessagesYAdd(0)
, flLastMessageDeleted(0)
, m_dLastSendTime(MIN_SEND_TIME_INTERVAL)
, flAlphaVisibility( 1.0 )
{
	m_pChatMessage = new CMenuEditBox( m_iChatMessageCommand, g_gameTexts[tChat], &CString(L""), MAX_CHAT_MESSAGE_LENGTH );
}



CInGameScreen::~CInGameScreen()
{
	SAFE_DELETE(m_pChatMessage);
}


// animate controls
void	CInGameScreen::FrameMove(double dStep)
{
	CGameMenuScreen::FrameMove(dStep);

	// animate chat edit box
	if ( m_pChatMessage )
		m_pChatMessage->FrameMove(dStep,true);

	//
	m_dLastSendTime += dStep;

	flLastMessageDeleted += (float)dStep;
	
	// animate controls y-position
	if ( flMessagesYAdd > 0.0f )
	{
		float flMoveSpeed = 1.0f + flMessagesYAdd * 3.0f;

		flMessagesYAdd -= (float)dStep * flMoveSpeed;
		if (flMessagesYAdd < 0.0f)flMessagesYAdd = 0.0f;
	}
}


void	CInGameScreen::RenderScreen10(CDXDevice &dev, ID3D10ShaderResourceView *pBackground, CGameMenuContainer *pContainer, float flPosition)
{
	RenderScreen(dev,pContainer,flPosition);
}


void	CInGameScreen::RenderScreen9(CDXDevice &dev, CGameMenuContainer *pContainer, float flPosition)
{
	RenderScreen(dev,pContainer,flPosition);
}


// render controls
void	CInGameScreen::RenderScreen(CDXDevice &dev, CGameMenuContainer *pContainer, float flPosition)
{
	if ( flAlphaVisibility <= 0.0f )	return;

	// edit box
	if (m_pChatMessage && m_pChatMessage->Text() && m_pChatMessage->Text()[0])
		m_pChatMessage->RenderControl( dev, pContainer, D3DXVECTOR2( 0.01f+flPosition,0.9f ), true );

	// messages
	D3DXVECTOR2	vecPos(0.01f + flPosition, 0.01f + flMessagesYAdd);
	flCtrlHeight = (float)FONT_HEIGHT / (float)dev.ScreenHeight();
	
	for (GameMenuControlList::iterator it = m_controls.GetIterator();!it.EndOfList();++it)
	{
		IGameMenuControl*pCtrl = it;

		if (pCtrl)
			pCtrl->RenderControl( dev, pContainer, vecPos, false  );

		// new line
		vecPos.y += flCtrlHeight;
	}

	// delete lines over cca a half a screen
	// delete lines earlier if last one is too old
	if ( (vecPos.y > 0.9f - flCtrlHeight)||
		 (vecPos.y > 0.3f - flCtrlHeight && flLastMessageDeleted > 5.0f))
	{
		if ( m_controls.Count() > 0 )
		{
			flLastMessageDeleted = 0.0f;
			flMessagesYAdd += flCtrlHeight;
			m_controls.Remove(0,true);
		}
	}
}


// process window message
void	CInGameScreen::OnClick( CGameMenuObserver*pObserver, float x, float y )
{
	if ( flAlphaVisibility <= 0.0f )	return;

	if (m_pChatMessage)
	{
		m_pChatMessage->OnClick( pObserver, x,y );
	}
}


// process window message
void	CInGameScreen::OnMouseMove( CGameMenuObserver*pObserver, float x, float y )
{
	if ( flAlphaVisibility <= 0.0f )	return;

	if (m_pChatMessage)
	{
		m_pChatMessage->OnMouseMove(pObserver, x,y );
	}
}


// process window message
bool	CInGameScreen::OnWmChar( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam )
{
	if ( flAlphaVisibility <= 0.0f )	return false;

	if (m_pChatMessage)
	{
		if ( (WCHAR)wParam != VK_RETURN )
		{
			pObserver = NULL;
		}
		else
		{
			if ( m_pChatMessage->Text()[0] )// not zero length text
			{
				// check enter pushing flow
				if (m_dLastSendTime < MIN_SEND_TIME_INTERVAL)
				{
					m_controls.Add( new CMenuLabel(g_gameTexts[tChatFlowWarning] ,WARNING_COLOR,WARNING_COLOR,WARNING_COLOR) );

					pObserver = NULL;
				}
				else
				{
					m_dLastSendTime = 0.0;
				}
			}
		}

		return m_pChatMessage->OnWmChar( pObserver, wParam, lParam );
	}
	else
		return false;
}


// process window message
bool	CInGameScreen::OnWmKeyDown( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam )
{
	if ( flAlphaVisibility <= 0.0f )	return false;

	// ignored controls
	switch (wParam)
	{
	case VK_LEFT:
	case VK_RIGHT:
		return false;
	}

	// process key
	if (m_pChatMessage)
	{
		if ( wParam != VK_RETURN )
		{
			pObserver = NULL;
		}

		return m_pChatMessage->OnWmKeyDown( pObserver, wParam, lParam );
	}
	else
		return false;
}


// add message
void	CInGameScreen::AddMessage( CString&strFrom, CString&strMessage )
{
	if ( flAlphaVisibility <= 0.0f ) return;

	CString label = strFrom + CString(L": ") + strMessage;

	D3DXCOLOR col = CHAT_COLOR;
	col.a *= flAlphaVisibility;

	m_controls.Add( new CMenuLabel(label,col,col,col) );
}